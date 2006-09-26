#ifndef __UNISTD_H__
#define __UNISTD_H__

#include <winbase.h>

#define sleep(sec)			Sleep(sec*1000)
#define usleep(microsec)	Sleep((int)microsec/1000)

#endif /* __UNISTD_H__ */