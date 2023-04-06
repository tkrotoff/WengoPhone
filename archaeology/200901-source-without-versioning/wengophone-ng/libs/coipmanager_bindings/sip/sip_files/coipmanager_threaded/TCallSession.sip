class TCallSession : TSession /NoDefaultCtors/ {
%TypeHeaderCode
#include <coipmanager_threaded/callsessionmanager/TCallSession.h>
%End
public:
	~TCallSession();
	EnumPhoneCallState::PhoneCallState getState() const;
	void playTone(EnumTone::Tone tone);
	void playSoundFile(const std::string & soundFile);
	CodecList::AudioCodec getAudioCodecUsed() const;
	CodecList::VideoCodec getVideoCodecUsed() const;
	void enableVideo(bool enable);
	bool isVideoEnabled() const;
	int getDuration() const;
signals:
	void phoneCallStateChangedSignal(EnumPhoneCallState::PhoneCallState state);
	void videoFrameReceivedSignal(QImage remoteVideoFrame, QImage localVideoFrame);

private slots:

	void videoFrameReceivedSlot(piximage * remoteVideoFrame, piximage * localVideoFrame);

private:
	TCallSession();
};
