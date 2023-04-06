class TChatSession : TSession /NoDefaultCtors/ {
%TypeHeaderCode
#include <coipmanager_threaded/chatsessionmanager/TChatSession.h>
%End
public:
	ChatMessageList getMessageHistory() const;
	void sendMessage(const std::string & message);
	void setTypingState(EnumChatTypingState::ChatTypingState state);
	EnumChatTypingState::ChatTypingState getTypingState(const Contact & contact) const;
signals:
	void messageAddedSignal();
	void statusMessageReceivedSignal(EnumChatStatusMessage::ChatStatusMessage status, std::string message);
	void typingStateChangedSignal(Contact contact, EnumChatTypingState::ChatTypingState state);
private:
	TChatSession();
};
