class EnumChatTypingState : NonCopyable {
%TypeHeaderCode
#include <coipmanager_base/EnumChatTypingState.h>
%End
public:
	enum ChatTypingState {
		ChatTypingStateNotTyping,
		ChatTypingStateTyping,
		ChatTypingStateStopTyping
	};

	static std::string toString(ChatTypingState state);
};
