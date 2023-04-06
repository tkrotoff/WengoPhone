//YLMAIN.h Non-MFC DLL header file
// Yealink Network, China
// Writed by Jianrong Chen
// First Version : 2004-12-27

//sure it will be included only once
#ifndef _ATM_YLMAIN_H_
#define _ATM_YLMAIN_H_
#pragma once

//to speedup & reduce size
//#define VC_EXTRALEAN
//#include <windows.h>

//After completing your DLL, you can use DUMPBIN [options] files to view export table of DLL. 
// ...

//to export for C++ & C
#ifdef __cplusplus 
 extern "C" 
 {  
#endif

#define MY_TRACE

#define	KEYBUFFER_PTR_READ			0x80
#define	KEYBUFFER_VAL_READ			0x81
#define	VERSION_VAL_READ			0x87
#define	SERIALNO_VAL_READ			0x8E

#define	MIO_CON_READ				0x8D

#define	RING_CON_WRITE				0x01
#define	LED_CON_WRITE				0x05
#define	PSTN_CON_CHANNEL			0x07

#define	SIGNAL_CON_WRITE			0x09
#define	CALLERID_CON_WRITE			0x0D

#define	LINE_CON_WRITE				0x0E

#define	COMM_FAIL_STATUS			0xFF
#define	VERIFY_FAIL_STATUS			0xFE
#define	TYPE_FAIL_STATUS			0xFD
#define	PARA_FAIL_STATUS			0xFC

typedef struct _USBTEL_COMM
{
	unsigned char	bRequestType;		//请求类型
								//.7=1,device to host  =0 host to device
	unsigned char	bSize;				//有效数据包长度
	unsigned short	wOffsetAddress;		//偏移地址
	unsigned char	bData[11];
	unsigned char	bVerifyNum;
}USBTEL_COMM,*PUSBTEL_COMM;

//Here, the class will be exported with all of its members. Now you only need to create a new instance of the object and all members will be accessable 
//class export, with all definitions
class CTelBox
{
public:

	int volatile	g_pCommTask;
	int volatile	g_iCommTask;
	unsigned char	lInputReportBuffer[12][64];
	unsigned char	lOutputReportBuffer[12][64];
	unsigned char	m_HookStatus, m_RingStatus;
	unsigned char	m_DefaultChannel;
	char			m_SerialNo[20];
	char			m_KeyBuffer[51];
	char			*m_AudioName;
	bool			m_fUSBChannel, m_fGenOffhook;
	bool			m_fPSTNOffhook, m_fFirstKey;
	bool			m_fEmptyBuffer, m_fSignal;
	bool			m_fRinging;
	int				m_SecRing;
	int				m_SecDelay;
	int				m_SecSendCID;
	unsigned long	m_MyStatus;

	CTelBox();
	~CTelBox();
	unsigned long	Open(void* lpInBuffer, unsigned long nInBufferSize,
				 void* lpOutBuffer, unsigned long nOutBufferSize);
	void Close();

	unsigned long	DeviceIoControl(unsigned long dwIoControlCode, 
			void*	lpInBuffer,	 unsigned long	nInBufferSize,	
			void*	lpOutBuffer, unsigned long	nOutBufferSize);

	void PrepareData(unsigned char bRequestType,unsigned short wAddr,unsigned short nLength,void* bData);
	void ProcessData();
	void ProcessIOMessage(unsigned char IOVal);
	void PSTNStopRing();
	char TransferChar(unsigned char Val);		//Transfer From Hex to ASCII
};

//Here is a variable which will be accesable as extern in your "client" 
//exported variable
//DLL_EXPORT int exp_iVar=99;

//this function will be directly accessable 

//exported function
//DLL_EXPORT BOOL ExportedFunc(BOOL bParam);

//copmleted ... only close this "language unconveniences".
typedef void (*CALLBACK_YLUSBTEL)(void* wParam, void* lParam, unsigned long ParameterOfApp);
class CMyCallBack
{
public:
	CMyCallBack();
	virtual ~CMyCallBack();
	
	void Create(void* pCallBackFunction = 0, unsigned long dwParameterOfApplication = 0);

	void CallBack(unsigned long wParam,void* lParam = 0);

private:
	struct {
		CALLBACK_YLUSBTEL f;
		unsigned long lParam;
	}m_fCallBack;
};
extern CMyCallBack myCallBack;

#ifdef __cplusplus 
   } 
#endif

//EOF Header body
#endif 

