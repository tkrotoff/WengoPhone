class EnumChatStatusMessage : NonCopyable {
%TypeHeaderCode
#include <coipmanager/chatsessionmanager/EnumChatStatusMessage.h>
%End
public:
	enum ChatStatusMessage {
		ChatStatusMessageReceived,
		ChatStatusMessageError,
		ChatStatusMessageInfo
	};
};
