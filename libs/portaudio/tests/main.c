#include <portaudio.h>
#include <samplerate.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Prototypes
 */

/**
 * Print info message
 */
void print_info(const char *msg);

/**
 * Print error message and exit
 */
void print_error(const char *msg, const char *msg2);

/**
 * Sound callback.
 *
 * Called by portaudio thread
 */
int playCallback(
	const void *input, void *output,
	unsigned long frameCount,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData);

int recordCallback(
	const void *input, void *output,
	unsigned long frameCount,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData);

/**
 * List available devices
 */
void list_devices();

/**
 * Initialize portaudio.
 */
void initialize();

/**
 * Open stream.
 */
void open_stream();

/**
 * Start the sound thread.
 */
void start_streams();

/**
 * Open a sound file.
 *
 * The file must be a raw file
 */
void open_sound_file(const char *filename);

/**
 * Stop the sound thread.
 */
void stop_streams();

/**
 * Quit the sound thread.
 */
void close_streams();

/**
 * Uninitialize portaudio.
 */
void uninitialize();

#define FILE_RATE 8000
#define SAMPLE_RATE 44100
#define SAMPLE_PLAY_FORMAT paInt16
#define SAMPLE_RECORD_FORMAT paFloat32
#define SAMPLE_TYPE short
#define NUM_SECONDS 5
#define NUM_CHANNELS 1
#define BUFFER_SIZE 1024
#define AUDIODATA_SIZE (SAMPLE_RATE * NUM_SECONDS * NUM_CHANNELS)
#define FILEDATA_SIZE (FILE_RATE * NUM_SECONDS * NUM_CHANNELS)

/**
 * Contains data to send to callback
 * @todo fix short to int16 or real audio format
 */

typedef struct {
	unsigned size;
	SAMPLE_TYPE data[AUDIODATA_SIZE];
	unsigned sampleToGo;
	unsigned curIndex;
} AudioData;
 

typedef struct {
	short c1;
	short c2;
	short c3;
} Frame;

/*
 * Global variables
 */
PaStream *playStream;
PaStream *recordStream;
AudioData audio_data;
FILE *playedfile, *recordedfile;

/*
 * Implementation
 */
void print_info(const char *msg) {
	char buffer[512];
	sprintf(buffer, "** %s\n", msg);
	
	fprintf(stdout, buffer);
}


void print_error(const char *msg, const char *msg2) {
	char buffer[512];
	sprintf(buffer, "!! %s: %s\n", msg, msg2);
	
	fprintf(stderr, buffer);
	
	exit(EXIT_FAILURE);
}


void initialize() {
	PaError err;
	
	print_info("Initializing portaudio");
	
	err = Pa_Initialize();
	
	if (err != paNoError) {
		print_error("Can't initialize Portaudio", Pa_GetErrorText(err));
	}
	
	audio_data.size = AUDIODATA_SIZE;
}


void open_stream() {
	PaStreamParameters outputParameters;
	PaStreamParameters inputParameters;
	PaError err;
	
	print_info("Opening streams");

	outputParameters.device = Pa_GetDefaultOutputDevice();
	outputParameters.channelCount = NUM_CHANNELS;
	outputParameters.sampleFormat = SAMPLE_PLAY_FORMAT;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	//inputParameters.device = Pa_GetDefaultInputDevice();
	inputParameters.device = 1;
	inputParameters.channelCount = NUM_CHANNELS;
	inputParameters.sampleFormat = SAMPLE_RECORD_FORMAT;
	inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowOutputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;


	err = Pa_OpenStream(
		&playStream,
		NULL,
		&outputParameters,
		SAMPLE_RATE,
		BUFFER_SIZE,
		paClipOff,
		playCallback,
		&audio_data );

	if (err != paNoError) {
		print_error("Can't open play stream", Pa_GetErrorText(err));
	}

	err = Pa_OpenStream(
		&recordStream,
		&inputParameters,
		NULL,
		SAMPLE_RATE,
		BUFFER_SIZE,
		paClipOff,
		recordCallback,
		NULL);

	if (err != paNoError) {
		print_error("Can't open record stream", Pa_GetErrorText(err));
	}

}


void start_streams() {
	PaError err;

	print_info("Starting streams");

	err = Pa_StartStream(playStream);
	
	if (err != paNoError) {
		print_error("Can't start play stream", Pa_GetErrorText(err));
	}

	err = Pa_StartStream(recordStream);
	
	if (err != paNoError) {
		print_error("Can't start record stream", Pa_GetErrorText(err));
	}

}


void open_sound_file(const char *filename) {
	FILE *f;
	SRC_DATA data;
	float data_out[AUDIODATA_SIZE];
	float data_in[FILEDATA_SIZE];
	short converted_data[AUDIODATA_SIZE];
	int r;
	
	print_info("Loading sound file");
	
	f = fopen(filename, "rb");
	if (!f) {
		print_error("Can't open file", filename);
	}
	fread(audio_data.data, FILEDATA_SIZE * sizeof(short), 1, f);
	fclose(f);

	/* Change endianness */
	typedef struct {
		char a;
		char b;
	} Short;
	
	Short tmp, tmp2;
	register unsigned i;
	
	for (i = 0 ; i < FILEDATA_SIZE; i++) {
		memcpy(&tmp, &audio_data.data[i], sizeof(short));
		tmp2.a = tmp.b;
		tmp2.b = tmp.a;
		memcpy(&audio_data.data[i], &tmp2, sizeof(short));
	}
	
	/* Resampling */
	src_short_to_float_array(audio_data.data, data_in, FILEDATA_SIZE);
	
	memset(&data, 0, sizeof(SRC_DATA));
	data.data_in = data_in;
	data.data_out = data_out;
	data.src_ratio = (float)SAMPLE_RATE / (float)FILE_RATE;
	data.input_frames = FILEDATA_SIZE;
	data.output_frames = AUDIODATA_SIZE;
	printf("ratio: %f\n", data.src_ratio);
	
	SRC_STATE *state;
	int error;
	
	state = src_new(SRC_LINEAR, NUM_CHANNELS, &error);
	
	r = src_process(state, &data);
	
	src_delete(state);

	printf("error: %s, used: %d, gen: %d\n", src_strerror(r), 
		data.input_frames_used, data.output_frames_gen);
	
	src_float_to_short_array(data_out, converted_data, AUDIODATA_SIZE);
	memcpy(audio_data.data, converted_data, AUDIODATA_SIZE * sizeof(short));
	
	//memcpy(audio_data.data, data_out, AUDIODATA_SIZE * sizeof(float));
		
	audio_data.sampleToGo = NUM_SECONDS * SAMPLE_RATE;
	audio_data.curIndex = 0;
}

void stop_streams() {
	PaError err;
	
	print_info("Stopping streams");
	
	if (Pa_IsStreamActive(playStream)) {
		err = Pa_StopStream(playStream);
		
		if (err != paNoError ) {
			print_error("Can't stop play stream", Pa_GetErrorText(err));
		}
	}
	
	if (Pa_IsStreamActive(recordStream)) {
		err = Pa_StopStream(recordStream);
		
		if (err != paNoError ) {
			print_error("Can't stop record stream", Pa_GetErrorText(err));
		}
	}
}

void close_streams() {
	PaError err;
	
	print_info("Closing streams");
	
	err = Pa_CloseStream(playStream);
	
	if (err != paNoError ) {
		print_error("Can't close play stream", Pa_GetErrorText(err));
	}
/*
	err = Pa_CloseStream(recordStream);
	
	if (err != paNoError ) {
		print_error("Can't close record stream", Pa_GetErrorText(err));
	}
*/
}

void uninitialize() {
	PaError err;
	
	print_info("Uninitializing");
	
	err = Pa_Terminate();

	if (err != paNoError ) {
		print_error("Can't uninitialize portaudio", Pa_GetErrorText(err));
	}
}


int main (int argc, const char * argv[]) {
	initialize();

	list_devices();
	
	open_sound_file("sine16bit.sw");
	open_stream();
	
	playedfile = fopen("played.sw", "wb");
	recordedfile = fopen("recorded.sw", "wb");
	
	start_streams();
	Pa_Sleep(NUM_SECONDS * 1000);
	stop_streams();
	
	fclose(playedfile);
	fclose(recordedfile);
	
	close_streams();
	uninitialize();
	
	return EXIT_SUCCESS;
}


int playCallback(const void *input, void *output,
	unsigned long frameCount,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData) {

	AudioData *user = (AudioData *) userData;
	short *out = (short *) output;
	register unsigned i;
	int result;
	short outFrames[frameCount];
	short buf;
	
	if (user->sampleToGo < frameCount) {
		for (i = 0 ; i < user->sampleToGo ; i++) {
			buf = user->data[user->curIndex++];
			*out++ = buf;
			outFrames[i] = buf;
		}
		for ( ; i < frameCount ; i++) {
			*out++ = 0;
			outFrames[i] = 0;
		}
		
		result = paComplete;
	} else {	
		for (i = 0 ; i < frameCount ; i++) {
			buf = user->data[user->curIndex++];
			*out++ = buf;
			outFrames[i] = buf;
		}
		user->sampleToGo -= frameCount;
		
		result = paContinue;
	}

	fwrite(outFrames, sizeof(short), frameCount, playedfile);
	fflush(playedfile);

	return result;
}


int recordCallback(const void *input, void *output,
	unsigned long frameCount,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData) {

	float *in = (float *)input;
	register unsigned i;
	float frames[frameCount];

	for (i = 0 ; i < frameCount ; i++) {
		frames[i] = *in++;
	}

	fwrite(frames, sizeof(float), frameCount, recordedfile);
	fflush(recordedfile);
	
	return paContinue;
}


void list_devices() {
	register PaHostApiIndex i;
	register PaDeviceIndex j;
	const PaHostApiInfo *phai;
	const PaDeviceInfo *pdi;
	PaHostApiIndex numDevices = Pa_GetHostApiCount();

	printf("Available host APIs: %d\n", numDevices);
	
	for (i = 0 ; i < numDevices ; i++) {
		phai = Pa_GetHostApiInfo(i);
		printf("\t%d- %s: %d devices\n", i, phai->name, phai->deviceCount);
		for (j = 0 ; j < phai->deviceCount ; j++) {
			pdi = Pa_GetDeviceInfo(j);
			printf("\t\t%d - %s: max input:%d, max output:%d \n", j, pdi->name,
				pdi->maxInputChannels, pdi->maxOutputChannels);
		}
	}
	
	printf("\n");
}

