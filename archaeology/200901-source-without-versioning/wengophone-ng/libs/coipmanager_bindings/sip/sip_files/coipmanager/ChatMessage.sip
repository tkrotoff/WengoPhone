class ChatMessage /NoDefaultCtors/ {
%TypeHeaderCode
#include <coipmanager/chatsessionmanager/ChatMessage.h>
%End
public:

	ChatMessage(const Peer & peer, const std::string & message);

	ChatMessage(const ChatMessage & chatMessage);

	~ChatMessage();

	const Date & getDate() const;

	const Time & getTime() const;

	const Peer & getPeer() const;

	const std::string & getMessage() const;

private:

	ChatMessage();

};

typedef std::list<ChatMessage> ChatMessageList;
