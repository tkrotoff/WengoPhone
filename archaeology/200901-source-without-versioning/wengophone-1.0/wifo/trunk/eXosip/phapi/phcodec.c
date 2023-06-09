#include <stdlib.h>
#ifndef _MSC_VER
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <dlfcn.h>
#endif

#include "phcodec.h"

//#include "gsm/gsm.h"
//#include "gsm/private.h"


#include "ilbc/iLBC_define.h"
#include "ilbc/iLBC_encode.h"
#include "ilbc/iLBC_decode.h"


#define DBG(x)


//#define FIXED_AMR 1





#ifndef EMBED

#define ENABLE_AMR_WB 1
#define ENABLE_AMR 1

#ifdef FIXED_AMR
#define ENABLE_AMR_EMBED 1
#undef ENABLE_AMR
#undef ENABLE_AMR_WB
#endif

#else /* EMBED */

//#define ENABLE_AMR_EMBED 1
#ifndef NO_GSM
#define NO_GSM 1
#endif
#ifndef NO_ILBC
#define NO_ILBC 1
#endif


#endif


#ifdef ENABLE_AMR_EMBED
#include "amr/amr.h"
#endif

#ifdef ENABLE_AMR_WB
#include "amrwb/dec_if.h"
#include "amrwb/enc_if.h"
#endif

#ifdef ENABLE_AMR
#include "amrnb/interf_dec.h"
#include "amrnb/interf_enc.h"
#include "amrnb/rom_enc.h"
#endif

#ifdef PHAPI_VIDEO_SUPPORT
#include "phcodec-h263.h"
#include "phcodec-mpeg4.h"
#include "phcodec-h264.h"
#endif


#ifndef NO_GSM
#define ENABLE_GSM 1
#include "gsm/gsm.h"
#include "gsm/private.h"
#endif
#ifndef NO_ILBC
#define ENABLE_ILBC 1
#endif

#if defined(WIN32) && !defined(__GNUC__)
# define inline _inline
#endif /* !WIN32 */

/*
 *  PCM - A-Law conversion
 *  Copyright (c) 2000 by Abramo Bagnara <abramo@alsa-project.org>
 *
 *  Wrapper for linphone Codec class by Simon Morlat <simon.morlat@free.fr>
 */

static inline int val_seg(int val)
{
	int r = 0;
	val >>= 7;
	if (val & 0xf0) {
		val >>= 4;
		r += 4;
	}
	if (val & 0x0c) {
		val >>= 2;
		r += 2;
	}
	if (val & 0x02)
		r += 1;
	return r;
}

/*
 * s16_to_alaw() - Convert a 16-bit linear PCM value to 8-bit A-law
 *
 * s16_to_alaw() accepts an 16-bit integer and encodes it as A-law data.
 *
 *		Linear Input Code	Compressed Code
 *	------------------------	---------------
 *	0000000wxyza			000wxyz
 *	0000001wxyza			001wxyz
 *	000001wxyzab			010wxyz
 *	00001wxyzabc			011wxyz
 *	0001wxyzabcd			100wxyz
 *	001wxyzabcde			101wxyz
 *	01wxyzabcdef			110wxyz
 *	1wxyzabcdefg			111wxyz
 *
 * For further information see John C. Bellamy's Digital Telephony, 1982,
 * John Wiley & Sons, pps 98-111 and 472-476.
 */

static inline unsigned char s16_to_alaw(int pcm_val)
{
	int		mask;
	int		seg;
	unsigned char	aval;

	if (pcm_val >= 0) {
		mask = 0xD5;
	} else {
		mask = 0x55;
		pcm_val = -pcm_val;
		if (pcm_val > 0x7fff)
			pcm_val = 0x7fff;
	}

	if (pcm_val < 256)
		aval = pcm_val >> 4;
	else {
		/* Convert the scaled magnitude to segment number. */
		seg = val_seg(pcm_val);
		aval = (seg << 4) | ((pcm_val >> (seg + 3)) & 0x0f);
	}
	return aval ^ mask;
}

/*
 * alaw_to_s16() - Convert an A-law value to 16-bit linear PCM
 *
 */
static inline alaw_to_s16(unsigned char a_val)
{
	int		t;
	int		seg;

	a_val ^= 0x55;
	t = a_val & 0x7f;
	if (t < 16)
		t = (t << 4) + 8;
	else {
		seg = (t >> 4) & 0x07;
		t = ((t & 0x0f) << 4) + 0x108;
		t <<= seg -1;
	}
	return ((a_val & 0x80) ? t : -t);
}
/*
 * s16_to_ulaw() - Convert a linear PCM value to u-law
 *
 * In order to simplify the encoding process, the original linear magnitude
 * is biased by adding 33 which shifts the encoding range from (0 - 8158) to
 * (33 - 8191). The result can be seen in the following encoding table:
 *
 *	Biased Linear Input Code	Compressed Code
 *	------------------------	---------------
 *	00000001wxyza			000wxyz
 *	0000001wxyzab			001wxyz
 *	000001wxyzabc			010wxyz
 *	00001wxyzabcd			011wxyz
 *	0001wxyzabcde			100wxyz
 *	001wxyzabcdef			101wxyz
 *	01wxyzabcdefg			110wxyz
 *	1wxyzabcdefgh			111wxyz
 *
 * Each biased linear code has a leading 1 which identifies the segment
 * number. The value of the segment number is equal to 7 minus the number
 * of leading 0's. The quantization interval is directly available as the
 * four bits wxyz.  * The trailing bits (a - h) are ignored.
 *
 * Ordinarily the complement of the resulting code word is used for
 * transmission, and so the code word is complemented before it is returned.
 *
 * For further information see John C. Bellamy's Digital Telephony, 1982,
 * John Wiley & Sons, pps 98-111 and 472-476.
 */

static unsigned char s16_to_ulaw(int pcm_val)	/* 2's complement (16-bit range) */
{
	int mask;
	int seg;
	unsigned char uval;

	if (pcm_val < 0) {
		pcm_val = 0x84 - pcm_val;
		mask = 0x7f;
	} else {
		pcm_val += 0x84;
		mask = 0xff;
	}
	if (pcm_val > 0x7fff)
		pcm_val = 0x7fff;

	/* Convert the scaled magnitude to segment number. */
	seg = val_seg(pcm_val);

	/*
	 * Combine the sign, segment, quantization bits;
	 * and complement the code word.
	 */
	uval = (seg << 4) | ((pcm_val >> (seg + 3)) & 0x0f);
	return uval ^ mask;
}

/*
 * ulaw_to_s16() - Convert a u-law value to 16-bit linear PCM
 *
 * First, a biased linear code is derived from the code word. An unbiased
 * output can then be obtained by subtracting 33 from the biased code.
 *
 * Note that this function expects to be passed the complement of the
 * original code word. This is in keeping with ISDN conventions.
 */
static inline int ulaw_to_s16(unsigned char u_val)
{
	int t;

	/* Complement to obtain normal u-law value. */
	u_val = ~u_val;

	/*
	 * Extract and bias the quantization bits. Then
	 * shift up by the segment number and subtract out the bias.
	 */
	t = ((u_val & 0x0f) << 3) + 0x84;
	t <<= (u_val & 0x70) >> 4;

	return ((u_val & 0x80) ? (0x84 - t) : (t - 0x84));
}




void mulaw_dec(char *mulaw_data /* contains size char */,
	       char *s16_data    /* contains size*2 char */,
	       int size)
{
  int i;
  for(i=0;i<size;i++)
    {
      *((signed short*)s16_data)=ulaw_to_s16( (unsigned char) mulaw_data[i]);
      s16_data+=2;
    }
}

void mulaw_enc(char *s16_data    /* contains pcm_size char */,
	       char *mulaw_data  /* contains pcm_size/2 char */,
	       int pcm_size)
{
  int i;
  int limit = pcm_size/2;
  for(i=0;i<limit;i++)
    {
      mulaw_data[i]=s16_to_ulaw( *((signed short*)s16_data) );
      s16_data+=2;
    }
}

void alaw_dec(char *alaw_data   /* contains size char */,
	      char *s16_data    /* contains size*2 char */,
	      int size)
{
  int i;
  for(i=0;i<size;i++)
    {
      ((signed short*)s16_data)[i]=alaw_to_s16( (unsigned char) alaw_data[i]);
    }
}

void alaw_enc(char *s16_data   /* contains 320 char */,
	      char *alaw_data  /* contains 160 char */,
	      int pcm_size)
{
  int i;
  int limit = pcm_size/2;
  for(i=0;i<limit;i++)
    {
      alaw_data[i]=s16_to_alaw( *((signed short*)s16_data) );
      s16_data+=2;
    }
}




static int pcmu_encode(void *ctx, const void *src, int srcsize, void *dst, int dstsize);
static int pcmu_decode(void *ctx, const void *src, int srcsize, void *dst, int dstsize);
static int pcma_encode(void *ctx, const void *src, int srcsize, void *dst, int dstsize);
static int pcma_decode(void *ctx, const void *src, int srcsize, void *dst, int dstsize);



static int pcmu_encode(void *ctx, const void *src, int srcsize, void *dst, int dstsize)
{
  mulaw_enc((char *) src, (char *) dst, srcsize);
  return srcsize/2;
}

static int pcmu_decode(void *ctx, const void *src, int srcsize, void *dst, int dstsize)
{
  mulaw_dec((char *) src, (char *) dst, srcsize);
  return srcsize*2;
}

static int pcma_encode(void *ctx, const void *src, int srcsize, void *dst, int dstsize)
{
  alaw_enc((char *) src, (char *) dst, srcsize);
  return srcsize/2;
}

static int pcma_decode(void *ctx, const void *src, int srcsize, void *dst, int dstsize)
{
  alaw_dec((char *) src, (char *) dst, srcsize);
  return srcsize*2;
}




static phcodec_t pcmu_codec = 
{
  "PCMU", 160, 320, 0, 0, 0, 0, 
  pcmu_encode, pcmu_decode
};

static phcodec_t pcma_codec =
{
  "PCMA", 160, 320, 0, 0, 0, 0, 
  pcma_encode, pcma_decode
};


#ifdef ENABLE_GSM

#define GSM_ENCODED_FRAME_SIZE 33
#define GSM_FRAME_SAMPLES 160
#define GSM_FRAME_DURATION 20

static int ph_gsm_encode(void *ctx, const void *src, int srcsize, void *dst, int dstsize);
static int ph_gsm_decode(void *ctx, const void *src, int srcsize, void *dst, int dstsize);
static void *ph_gsm_init(void *);
static void ph_gsm_cleanup(void *ctx);

static void *
ph_gsm_init(void *dummy)
{
  return gsm_create();
}

static void 
ph_gsm_cleanup(void *ctx)
{
  gsm_destroy(ctx);
}


int ph_gsm_encode(void *ctx, const void *src, int srcsize, void *dst, int dstsize)
{
  gsm_encode(ctx, src, dst);
  return GSM_ENCODED_FRAME_SIZE;

}

int ph_gsm_decode(void *ctx, const void *src, int srcsize, void *dst, int dstsize)
{
  gsm_decode(ctx, src, dst);
  return GSM_FRAME_SAMPLES*2;
}

static phcodec_t gsm_codec =
{
  "GSM", GSM_ENCODED_FRAME_SIZE, GSM_FRAME_SAMPLES*2, ph_gsm_init, ph_gsm_init, ph_gsm_cleanup, ph_gsm_cleanup, 
  ph_gsm_encode, ph_gsm_decode
};


#endif

#if defined(ENABLE_AMR_EMBED) || defined(ENABLE_AMR) || defined(ENABLE_AMR_WB)
typedef struct{
  UWord8 cmr;
  UWord8 toc;
}amr_rtphdr; 

#define AMR_DEFAULT_MODE 7
#define AMR_FRAME_SAMPLES 160
//short amr_block_size[16]={ 12, 13, 15, 17, 19, 20, 26, 31, 5, 0, 0, 0, 0, 0, 0, 0 };
#define AMR_ENCODED_FRAME_SIZE  32+2

#define AMRWB_DEFAULT_MODE 8
#define AMRWB_FRAME_SAMPLES 320
#ifdef IF2
//const UWord8 amrwb_block_size[16]= {18, 23, 33, 37, 41, 47, 51, 59, 61, 6, 6, 0, 0, 0, 1, 1};
#else
//const UWord8 amrwb_block_size[16]= {18, 24, 33, 37, 41, 47, 51, 59, 61, 6, 6, 0, 0, 0, 1, 1};
#endif
#define AMRWB_ENCODED_FRAME_SIZE  62+2

#endif 

#ifdef ENABLE_AMR_EMBED

/* frame size in serial bitstream file (frame type + serial stream + flags) */

#define SERIAL_FRAMESIZE (1+MAX_SERIAL_SIZE+5)
#define MAX_PACKED_SIZE (MAX_SERIAL_SIZE / 8 + 2)


/* static int amr_frame_bits[]={ 95, 103, 118, 134, 148, 159, 204, 244}; */
static int amr_packed_frame_size[]={ 12, 13, 15, 17, 19, 20, 26, 31, 5, 0,0,0,0,0,0,0};

static int ph_amr_encode(void *ctx, const void *src, int srcsize, void *dst, int dstsize);
static int ph_amr_decode(void *ctx, const void *src, int srcsize, void *dst, int dstsize);
static void *ph_amr_enc_init(void *);
static void ph_amr_enc_cleanup(void *ctx);
static void *ph_amr_dec_init(void *);
static void ph_amr_dec_cleanup(void *ctx);

struct amrencoder{
  enum Mode mode;
  enum Mode used_mode;
  enum TXFrameType tx_type;
  Word16 dtx;                     /* enable encoder DTX                */
  Word16 reset_flag;
  Speech_Encode_FrameState *enc;
  sid_syncState *sid_state;
  void *decoder;
};

struct amrdecoder{
  enum Mode mode;
  enum Mode used_mode;
  enum RXFrameType rx_type;
  enum TXFrameType tx_type;
  Word16 reset_flag;
  Word16 reset_flag_old;
  Word16 dtx;                     /* enable encoder DTX                */
  Speech_Decode_FrameState *dec;
  void * encoder;
};

void *ph_amr_enc_init(void *dummy){
  struct amrencoder *amr;
  DBG("AMR EMBED ENCODER INIT\n");
  amr = malloc(sizeof(struct amrencoder));
  if(amr == NULL){
    DBG("MALLOC FAILED!!!!!\n");
    return NULL;
  }
#ifdef AMR_DTX
  amr->dtx = 1;
#else
  amr->dtx = 0;
#endif
  DBG("AMR EMBED ENC initializing...");
  if (Speech_Encode_Frame_init(&amr->enc, amr->dtx, "encoder") || sid_sync_init (&amr->sid_state)){
    free(amr);
    DBG("failed\n");
    return NULL;
  }
  DBG("OK\n");
  amr->tx_type = (enum TXFrameType)0;
  amr->reset_flag = 0;
  amr->mode = AMR_DEFAULT_MODE;
  return amr;
}

void *ph_amr_dec_init(void *dummy){
  struct amrdecoder *amr;
  DBG("AMR EMBED DECODER init\n");
  amr = malloc(sizeof(struct amrdecoder));
  if(amr == NULL){
    DBG("MALLOC FAILED!!!!!\n");
    return NULL;
  }
  DBG("AMR EMBED DEC initializing...\n");
  if (Speech_Decode_Frame_init(&amr->dec, "Decoder")){
    free(amr);
    DBG("failed\n");
    return NULL;
  }
  DBG("OK\n");
  amr->rx_type = (enum RXFrameType)0;
  amr->tx_type = (enum TXFrameType)0;
  amr->reset_flag = 0;
  amr->reset_flag_old = 1;
  return amr;
}

void ph_amr_enc_cleanup(void *ctx){
  struct amrencoder *amr = (struct amrencoder *)ctx; 
  DBG("AMR ENC cleaning\n");
  Speech_Encode_Frame_exit (&amr->enc);
  free(amr);
}

void ph_amr_dec_cleanup(void *ctx){
  struct amrdecoder *amr = (struct amrdecoder *)ctx; 
  DBG("AMR DEC cleaning\n");
  Speech_Decode_Frame_exit (&amr->dec);
  free(amr);
}
/* ENCODE */
int ph_amr_encode(void *ctx, const void *src, int srcsize, void *dst, int dstsize){
  int i;
  struct amrencoder *amr = (struct amrencoder *)ctx; 
  Word16 *new_speech = (Word16 *)src;
  Word16 serial[SERIAL_FRAMESIZE];    /* Output bitstream buffer           */
  UWord8 *packed_bits =  (UWord8 *)dst+2;
  Word16 packed_size;
  static int frame=0;
  amr_rtphdr *hdr = (amr_rtphdr *)dst;

  frame++;

  /* sample must be 13-bit left aligned */
  for (i = 0; i < srcsize; i++)
         new_speech[i] &= 0xFFFC;

  /* zero flags and parameter bits */
  for (i = 0; i < SERIAL_FRAMESIZE; i++)
         serial[i] = 0;

     /* check for homing frame */
  amr->reset_flag = encoder_homing_frame_test(new_speech);
     
     /* encode speech */
  Speech_Encode_Frame(amr->enc, amr->mode, new_speech, &serial[1], &amr->used_mode); 
  sid_sync (amr->sid_state, amr->used_mode, &amr->tx_type);

  if ((frame%10) == 0) {
   DBG("\rtxtype=%d mode=%d used_mode=%d", amr->tx_type, amr->mode, amr->used_mode);
  }


  packed_size = PackBits(amr->used_mode, amr->mode, amr->tx_type, &serial[1], packed_bits);
  /* perform homing if homing frame was detected at encoder input */
  if(packed_size != 32)
    DBG("packed size: %d\n", packed_size);

  if (amr->reset_flag != 0){
    Speech_Encode_Frame_reset(amr->enc);
    sid_sync_reset(amr->sid_state);
  }
  hdr->cmr = amr->mode << 4;
  hdr->toc = *packed_bits & 0x07ff;  /* only 1 frame F=0 */
  hdr->toc |= 4;  /*  Q=1 */

  return (packed_size+2);
}
/* DECODE */
int ph_amr_decode(void *ctx, const void *src, int srcsize, void *dst, int dstsize){
  struct amrdecoder *amr = (struct amrdecoder *)ctx; 
  UWord8 toc, q, ft;
  UWord8 *packed_bits = (UWord8 *)src+2;
  Word16 *out = (Word16 *)dst;
  Word16 serial[SERIAL_FRAMESIZE];   /* coded bits                    */
  static int frame=0;
  int i;

  toc = *packed_bits;
  /* read rest of the frame based on ToC byte */
  q  = (toc >> 2) & 0x01;
  ft = (toc >> 3) & 0x0F;
  
  if(ft > 8 && ft < 15){
    /* invalid frame, discard all packet */
    printf ("\rRejected: rxtype=%d mode=%d", amr->rx_type, ft);
    return 0;
  }

  amr->rx_type = UnpackBits(q, ft, packed_bits+1, &amr->mode, &serial[1]);
  frame++;
  if ((frame%10) == 0) {
   printf ("\rrxtype=%d mode=%d", amr->rx_type, ft);
  }
  if (amr->rx_type == RX_NO_DATA)
    amr->mode = amr->dec->prev_mode;
  else
    amr->dec->prev_mode = amr->mode;
  
  /* if homed: check if this frame is another homing frame */
  if (amr->reset_flag_old == 1){
    /* only check until end of first subframe */
    amr->reset_flag = decoder_homing_frame_test_first(serial, amr->mode);
  }
  /* produce encoder homing frame if homed & input=decoder homing frame */
  if ((amr->reset_flag != 0) && (amr->reset_flag_old != 0)){
    for (i = 0; i < L_FRAME; i++){
      *out++ = EHF_MASK;
    }
  }else{     
    /* decode frame */
    Speech_Decode_Frame(amr->dec, amr->mode, &serial[1], amr->rx_type, out);
  }
  /* if not homed: check whether current frame is a homing frame */
  if (amr->reset_flag_old == 0){
    /* check whole frame */
    amr->reset_flag = decoder_homing_frame_test(&serial[1], amr->mode);
  }
  /* reset decoder if current frame is a homing frame */
  if (amr->reset_flag != 0){
    Speech_Decode_Frame_reset(amr->dec);
  }
  amr->reset_flag_old = amr->reset_flag;

  return AMR_FRAME_SAMPLES*2;
}

static phcodec_t amr_codec =
{
  "AMR", AMR_ENCODED_FRAME_SIZE, AMR_FRAME_SAMPLES*2, ph_amr_enc_init, ph_amr_dec_init, ph_amr_enc_cleanup, ph_amr_dec_cleanup, 
  ph_amr_encode, ph_amr_decode
};

#else

#ifdef ENABLE_AMR


static int ph_amr_encode(void *ctx, const void *src, int srcsize, void *dst, int dstsize);
static int ph_amr_decode(void *ctx, const void *src, int srcsize, void *dst, int dstsize);
static void *ph_amr_enc_init(void *dummy);
static void ph_amr_enc_cleanup(void *ctx);
static void *ph_amr_dec_init(void *dummy);
static void ph_amr_dec_cleanup(void *ctx);

static int ph_amr_encode(void *ctx, const void *src, int srcsize, void *dst, int dstsize)
{
  Word16 *inp = (Word16 *)src;
  UWord8 *out = (UWord8 *)dst+2;  /* reserve place for Payload header and TOC in octet aligned mode */
  Word16 mode = AMR_DEFAULT_MODE;
  Word16 coded_size;
  amr_rtphdr *hdr = (amr_rtphdr *)dst;

  coded_size = Encoder_Interface_Encode(ctx, mode, inp, out, 0);
  hdr->cmr = mode << 4;
  hdr->toc = *out & 0x07ff;  /* only 1 frame F=0 */
  hdr->toc |= 4;  /*  Q=1 */
  return coded_size+2;
}
static int ph_amr_decode(void *ctx, const void *src, int srcsize, void *dst, int dstsize)
{
  Word16 *out = (Word16 *)dst;
  UWord8 *inp = (UWord8 *)src + 2;
  Word16 mode;
  amr_rtphdr *hdr = (amr_rtphdr *)src;

  Decoder_Interface_Decode(ctx, inp,  out, 0);
  return AMR_FRAME_SAMPLES*2;
}

static void *ph_amr_enc_init(void *dummy)
{
  Word16 dtx;
  void *enc;
#ifdef AMR_DTX
  dtx = 1;
#else
  dtx = 0;
#endif
  DBG("AMR ENC initializing...");
  enc = Encoder_Interface_init(dtx);
  if (enc == NULL){
    DBG("failed\n");
    return NULL;
  }else{
    DBG("OK\n");
    return enc;
  }
  //  return (Encoder_Interface_init(dtx));
}
static void ph_amr_enc_cleanup(void *ctx)
{
  Encoder_Interface_exit(ctx);
}
static void *ph_amr_dec_init(void *dummy)
{
  void *dec;
  DBG("AMR DEC initializing...");
  dec = Decoder_Interface_init();
  if (dec == NULL){
    DBG("failed\n");
    return NULL;
  }else{
    DBG("OK\n");
    return dec;
  } 
  // return (Decoder_Interface_init());
}
static void ph_amr_dec_cleanup(void *ctx)
{
  Decoder_Interface_exit(ctx);
}
static phcodec_t amr_codec =
{
  "AMR", AMR_ENCODED_FRAME_SIZE, AMR_FRAME_SAMPLES*2, ph_amr_enc_init, ph_amr_dec_init, ph_amr_enc_cleanup, ph_amr_dec_cleanup, ph_amr_encode, ph_amr_decode
};
#endif
#endif


#ifdef ENABLE_AMR_WB

extern const UWord8 block_size[];

static int ph_amrwb_encode(void *ctx, const void *src, int srcsize, void *dst, int dstsize);
static int ph_amrwb_decode(void *ctx, const void *src, int srcsize, void *dst, int dstsize);
static void *ph_amrwb_enc_init(void *);
static void ph_amrwb_enc_cleanup(void *ctx);
static void *ph_amrwb_dec_init(void *);
static void ph_amrwb_dec_cleanup(void *ctx);

int enc_cnt, dec_cnt;

static int ph_amrwb_encode(void *ctx, const void *src, int srcsize, void *dst, int dstsize)
{
  Word16 *inp = (Word16 *)src;
  UWord8 *out = (UWord8 *)dst+2;
  Word16 mode = AMRWB_DEFAULT_MODE;
  Word16 allow_dtx;
  Word16 coded_size;
  amr_rtphdr *hdr = (amr_rtphdr *)dst;

#ifdef AMR_DTX
  allow_dtx = 1;
#else
  allow_dtx = 0;
#endif

  coded_size = E_IF_encode(ctx, mode, inp, out, allow_dtx);
  hdr->cmr = mode << 4;
  hdr->toc = *out & 0x07ff;  /* only 1 frame F=0 */
  hdr->toc |= 4;  /*  Q=1 */
  return coded_size+2;
}
static int ph_amrwb_decode(void *ctx, const void *src, int srcsize, void *dst, int dstsize)
{
  Word16 *out = (Word16 *)dst;
  UWord8 *inp = (UWord8 *)src+2;
  amr_rtphdr *hdr = (amr_rtphdr *)src;

  D_IF_decode( ctx, inp,  out, _good_frame);
  return(AMRWB_FRAME_SAMPLES*2);
}

static void *ph_amrwb_enc_init(void *dummy)
{
  DBG("AMR-WB ENC initializing...");
  return (E_IF_init());
}
static void ph_amrwb_enc_cleanup(void *ctx)
{
  DBG("AMR-WB ENC cleanup");
  E_IF_exit(ctx);
}
static void *ph_amrwb_dec_init(void *dummy)
{
  DBG("AMR-WB DEC initializing...");
  return (D_IF_init());
}
static void ph_amrwb_dec_cleanup(void *ctx)
{
  DBG("AMR-WB DEC cleanup");
  D_IF_exit(ctx);
}


static phcodec_t amr_wb_codec =
{
  "AMR-WB", AMRWB_ENCODED_FRAME_SIZE, AMRWB_FRAME_SAMPLES*2, ph_amrwb_enc_init, ph_amrwb_dec_init, ph_amrwb_enc_cleanup, ph_amrwb_dec_cleanup,  ph_amrwb_encode, ph_amrwb_decode
};
#endif

#ifdef ENABLE_ILBC

static int ph_ilbc_encode(void *ctx, const void *src, int srcsize, void *dst, int dstsize);
static int ph_ilbc_decode(void *ctx, const void *src, int srcsize, void *dst, int dstsize);
static void *ph_ilbc_dec_init(void *dummy);
static void *ph_ilbc_enc_init(void *dummy);
static void ph_ilbc_dec_cleanup(void *ctx);
static void ph_ilbc_enc_cleanup(void *ctx);

static void *ph_ilbc_dec_init(void *dummy)
{
    iLBC_Dec_Inst_t *ctx;

    ctx = malloc(sizeof(*ctx));

    initDecode(ctx, 20, 1);

    return ctx;
}


static void *ph_ilbc_enc_init(void *dummy)
{
    iLBC_Enc_Inst_t *ctx;

    ctx = malloc(sizeof(*ctx));

    initEncode(ctx, 20);

    return ctx;

}


static void ph_ilbc_dec_cleanup(void *ctx)
{
  free(ctx);
}

static void ph_ilbc_enc_cleanup(void *ctx)
{
  free(ctx);
}


static int ph_ilbc_decode(void *ctx, const void *src, int srcsize, void *dst, int dstsize)
{
    int k; 
    float decflt[BLOCKL_MAX], tmp; 
    short *decshrt = (short*) dst;
    iLBC_Dec_Inst_t *dec = (iLBC_Dec_Inst_t *) ctx;

 
    iLBC_decode(decflt, (unsigned char *)src, dec, 1); 
 
 
    for (k=0; k< dec->blockl; k++){  
        tmp=decflt[k]; 
        if (tmp<MIN_SAMPLE) 
            tmp=MIN_SAMPLE; 
        else if (tmp>MAX_SAMPLE) 
            tmp=MAX_SAMPLE; 
        decshrt[k] = (short) tmp; 
    } 
 
    return (dec->blockl*2); 

}


static int ph_ilbc_encode(void *ctx, const void *src, int srcsize, void *dst, int dstsize)
{
    float tmp[BLOCKL_MAX];
    short *indata = (short *) src;
    int k; 
    iLBC_Enc_Inst_t *enc = (iLBC_Enc_Inst_t *) ctx;
 
    for (k=0; k<enc->blockl; k++) 
      tmp[k] = (float)indata[k]; 
 
    /* do the actual encoding */ 
 
    iLBC_encode((unsigned char *)dst, tmp, enc); 
 
 
    return (enc->no_of_bytes); 

}

 
  





static phcodec_t ilbc_codec =
{
  "ILBC", NO_OF_BYTES_20MS, BLOCKL_20MS*2, ph_ilbc_enc_init, ph_ilbc_dec_init, ph_ilbc_enc_cleanup, ph_ilbc_dec_cleanup, 
  ph_ilbc_encode, ph_ilbc_decode
};

#endif

phcodec_t *ph_codec_list;

static phcodec_t *codec_table[] = 
{
	&pcmu_codec, &pcma_codec,
#ifdef ENABLE_ILBC
	&ilbc_codec,
#endif
#ifdef ENABLE_GSM
	&gsm_codec,
#endif
#if defined(ENABLE_AMR) || defined(ENABLE_AMR_EMBED)
	&amr_codec,
#endif
#ifdef ENABLE_AMR_WB
	&amr_wb_codec,
#endif
#ifdef PHAPI_VIDEO_SUPPORT
	&h263_codec,
	&mpeg4_codec,
	&h264_codec,
#endif
	0
};



void ph_media_register_codec(phcodec_t *codec)
{
  phcodec_t *last = ph_codec_list;

  if (!last)
    {
      ph_codec_list = codec;
      return;
    }

  while(last->next)
    last = last->next;

  last->next = codec;
}


#ifdef _MSC_VER
void ph_media_plugin_codec_init(const char *dirpath)
{
}

#else /* _MSC_VER */
/*
  scan the given directory path for a shared library files and load them 
 */
void ph_media_plugin_codec_init(const char *dirpath)
{
  DIR *dir;
  struct dirent *entry;
  struct stat st;
  char   modulename[512];
  ph_codec_plugin_init_type  plugin_init;
  void *lib;


  if (!dirpath || !dirpath[0])
    dirpath = "./codecs";


  dir = opendir(dirpath);
  if (!dir)
    return;

  while(0 != (entry = readdir(dir)))
    {
      if (!strstr(entry->d_name, ".so"))
	continue;

      snprintf(modulename, sizeof(modulename), "%s/%s", dirpath, entry->d_name);

      stat(modulename, &st);

      if (!(st.st_mode & S_IFREG))
	continue;

      lib = dlopen(modulename, RTLD_NOW);
      if (!lib)
	continue;

      
      plugin_init = (ph_codec_plugin_init_type) dlsym(lib, "_ph_codec_plugin_init");
      if (plugin_init)
	{
	  if (plugin_init(ph_media_register_codec))
	    {
	      dlclose(lib);
	    }
	}
      else
	dlclose(lib);

    }

  closedir(dir);
}

#endif


void ph_media_codecs_init(void)
{
  phcodec_t *codec;
  int i = 0;

#ifdef PH_VIDEO_SUPPORT
  ph_avcodec_wrapper_init();
#endif    
     
  while(0 != (codec= codec_table[i++]))
  {
	  codec->next = codec_table[i];
  }

  ph_codec_list = codec_table[0];

  ph_media_plugin_codec_init(0);
  
}


	


	
	
      
      






  
