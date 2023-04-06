class TChatSessionManager : ITCoIpSessionManager /NoDefaultCtors/ {
%TypeHeaderCode
#include <coipmanager_threaded/chatsessionmanager/TChatSessionManager.h>
%End
public:
	TChatSession * createTChatSession() /Factory/ ;
signals:
	void newChatSessionCreatedSignal(TChatSession * tChatSession);
private:
	TChatSessionManager();
};
