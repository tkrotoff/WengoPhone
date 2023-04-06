// OSDepend.h -- This file is base on windows
//		1 - Create Thread
//		2 - Access USB HID Device Through NTDDK
//		3 - Mapped Memory For Access Exclusive, Avoid Access Conflict
// Yealink Network, China
// Writed by Jianrong Chen
// First Version : 2004-12-27

#ifdef _WIN32
#include <windows.h>
#include <process.h>

typedef enum _POOL_TYPE 
{
    NonPagedPool,
    PagedPool,
    NonPagedPoolMustSucceed,
    DontUseThisType,
    NonPagedPoolCacheAligned,
    PagedPoolCacheAligned,
    NonPagedPoolCacheAlignedMustS,
    MaxPoolType
}POOL_TYPE;


extern "C" {
	// Declare the C libraries used
	#include "setupapi.h"		// Must link in setupapi.lib
	#include <hidsdi.h>
	#include <hidpddi.h>
	#include <hidpi.h>
}

#endif //_WIN32

class CMyHid
{
public:
	CMyHid();
	~CMyHid();
	unsigned long OpenHid();
	void CloseHid();
	void StartComm(void* pParam);
	char* GetUSBAudioName();	
private:
	void *m_pHidPreparsedData;
	void * m_hDevInfo;
	void* m_lpvMem; 		// pointer to shared memory
	void* m_hMapObject;	// handle to file mapping
};


