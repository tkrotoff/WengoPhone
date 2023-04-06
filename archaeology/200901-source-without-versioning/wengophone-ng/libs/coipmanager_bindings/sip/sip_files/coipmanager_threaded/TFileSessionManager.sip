class TFileSessionManager : ITCoIpSessionManager /NoDefaultCtors/ {
%TypeHeaderCode
#include <coipmanager_threaded/filesessionmanager/TFileSessionManager.h>
%End
public:

	TSendFileSession * createTSendFileSession() /Factory/;

signals:

	void newTReceiveFileSessionCreatedSignal(TReceiveFileSession * fileSession);

	void needsUpgradeSignal();

	void peerNeedsUpgradeSignal(Contact contact);

private:

	TFileSessionManager();

};
