#include <winsock.h>

#include <unistd.h>

PHAPI_UTIL_EXPORTS int fcntl(int fd, int cmd, long arg) {
	if(cmd == F_SETFL) {
		return ioctlsocket(fd, cmd, &arg);
	}
	if(cmd == F_GETFL) {
		return 1;
	}
	return 0;
}