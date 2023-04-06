class EnumSessionError : NonCopyable {
%TypeHeaderCode
#include <coipmanager/session/EnumSessionError.h>
%End
public:

	enum SessionError {

		/** No Error. */
		SessionErrorNoError,

		/**
		 * Given parameter not valid.
		 */
		SessionErrorInvalidParameter,
	};
};
