class TSMSSessionManager : ITCoIpSessionManager /NoDefaultCtors/ {
%TypeHeaderCode
#include <coipmanager_threaded/smssessionmanager/TSMSSessionManager.h>
%End
public:

	virtual ~TSMSSessionManager();

	TSMSSession * createTSMSSession() /Factory/;

private:

	TSMSSessionManager(TCoIpManager & tCoIpManager);

};
