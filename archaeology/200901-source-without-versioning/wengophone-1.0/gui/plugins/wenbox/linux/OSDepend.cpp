#include "OSDepend.h"
#include "YLMAIN.h"
#include "YLTELBOX.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int             m_hHIDHandle;
unsigned char	InputReportBuffer[64];
unsigned char	OutputReportBuffer[64];
unsigned long	m_dwInputReportByteLength;
unsigned long	m_dwOutputReportByteLength;
unsigned char	m_ReadPointer = 0;
unsigned char	m_WritePointer = 0;
unsigned char	m_IOVal = 0;
bool			m_fUsbCommRunning = false;
unsigned long	m_DataWaitProcess = 0;
unsigned long	m_IOWaitProcess = 0;

void *MyProcess(void* pParam)
{
	CTelBox *ht = (CTelBox *)pParam;
	while(m_fUsbCommRunning)
	{
        sleep(10);
		if(m_DataWaitProcess > 0) {
			ht->ProcessData();
			m_DataWaitProcess--;
		}
		if(m_IOWaitProcess > 0) {
			ht->ProcessIOMessage(m_IOVal);
			m_IOWaitProcess--;
		}
	}
}

CMyHid::CMyHid()
{
	m_hDevInfo = 0;
	m_pHidPreparsedData = 0;
	m_hHIDHandle = 0;
	m_lpvMem = 0;
	m_hMapObject = 0;
	m_fUsbCommRunning = false;
	m_dwInputReportByteLength = 0;
	m_dwOutputReportByteLength = 0;
}

CMyHid::~CMyHid()
{
	CloseHid();
}

unsigned long CMyHid::OpenHid()
{
    return YL_RETURN_SUCCESS;
}

void CMyHid::CloseHid()
{
	m_fUsbCommRunning = false;
    sleep(100);

	if(m_hHIDHandle != 0)
	{
        close(m_hHIDHandle);
		m_hHIDHandle = 0;
	}
}

void CMyHid::StartComm(void* pParam)
{
    pthread_t m_processT;
    pthread_create(&m_processT, NULL, &MyProcess, pParam);
}

char* CMyHid::GetUSBAudioName()
{
	return "USB Audio Device";
}
