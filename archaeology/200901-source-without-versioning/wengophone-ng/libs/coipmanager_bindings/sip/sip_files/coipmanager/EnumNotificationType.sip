class EnumNotificationType : NonCopyable {
%TypeHeaderCode
#include <coipmanager/notification/EnumNotificationType.h>
%End
public:
	enum NotificationType {
		NotificationTypeCallSessionIncoming,
		NotificationTypeCallSessionClosed,
	};
};
