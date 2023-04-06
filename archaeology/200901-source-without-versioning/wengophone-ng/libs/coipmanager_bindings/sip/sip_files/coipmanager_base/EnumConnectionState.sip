class EnumConnectionState : NonCopyable {
%TypeHeaderCode
#include <coipmanager_base/EnumConnectionState.h>
%End
public:
	enum ConnectionState {
		ConnectionStateDisconnected,
		ConnectionStateConnecting,
		ConnectionStateConnected,
	};
	static std::string toString(ConnectionState state);
};
