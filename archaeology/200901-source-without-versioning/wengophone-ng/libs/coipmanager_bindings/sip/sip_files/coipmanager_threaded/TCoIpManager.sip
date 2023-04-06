class TCoIpManager : QThread {
%TypeHeaderCode
#include <coipmanager_threaded/TCoIpManager.h>
%End
public:

	TCoIpManager(const CoIpManagerConfig & coIpManagerConfig);

	virtual ~TCoIpManager();

	void release();

	CoIpManagerConfig & getCoIpManagerConfig();

	TCallSessionManager & getTCallSessionManager();

	TChatSessionManager & getTChatSessionManager();

	TConnectManager & getTConnectManager();

	TFileSessionManager & getTFileSessionManager();

	TSMSSessionManager & getTSMSSessionManager();

	TUserProfileManager & getTUserProfileManager();

signals:

	void initialized();

private:

	virtual void run() /NewThread/;

};
