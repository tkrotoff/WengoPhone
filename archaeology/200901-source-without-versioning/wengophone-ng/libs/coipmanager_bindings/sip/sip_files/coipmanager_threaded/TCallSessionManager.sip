class TCallSessionManager : ITCoIpSessionManager /NoDefaultCtors/ {
%TypeHeaderCode
#include <coipmanager_threaded/callsessionmanager/TCallSessionManager.h>
%End
public:
	TCallSession * createTCallSession() /Factory/;
signals:
	void newCallSessionCreatedSignal(TCallSession * tCallSession);
private:
	TCallSessionManager();
};
