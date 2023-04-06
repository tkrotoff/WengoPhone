class EnumConnectionError : NonCopyable {
%TypeHeaderCode
#include <coipmanager/connectmanager/EnumConnectionError.h>
%End
public:
	enum ConnectionError {
		ConnectionErrorUnknown,
		ConnectionErrorNoImplementation,
		ConnectionErrorServer,
		ConnectionErrorTimeout,
		ConnectionErrorTooMuchRetry,
		ConnectionErrorInvalidLoginPassword,
	};
 static std::string toString(ConnectionError connectionError);
};
