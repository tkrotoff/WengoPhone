// YLTELBOX.cpp -- This file is base on standard c++
// Yealink Network, China
// Writed by Jianrong Chen
// First Version : 2004-12-27
// Updated : 2005-01-13 Add UnReady Status
// Updated : 2005-01-15 Add RingBack Status

#include "YLTELBOX.h"
#include "YLMAIN.h"
#include "OSDepend.h"
#ifdef MY_TRACE
#include <stdio.h>
#endif

CMyCallBack myCallBack;
CMyHid		myHid;
CTelBox		gTelBox;
bool		m_fOpenDevice;

//Regular DLL Entry Point, do not have any importancy 
//to me here
/*BOOL WINAPI DllMain(HINSTANCE hinstDLL, // DLL module handle
                    DWORD fdwReason, // for calling function
                    LPVOID lpvReserved   // reserved
                    )

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
 switch(ul_reason_for_call)
 {
  case DLL_PROCESS_ATTACH:
  break;

  case DLL_THREAD_ATTACH:
  break;

  case DLL_THREAD_DETACH:
  break;

  case DLL_PROCESS_DETACH: 
  break;

  default:
   break;
 }

 return true;
}
*/
//class export, with all definitions
//member method
//constructor
CTelBox::CTelBox()
{
	m_fOpenDevice = false;
}

//destructor
CTelBox::~CTelBox()
{
}

//*******************CALLBACK********************
CMyCallBack::CMyCallBack()
{
//	memset(&m_fCallBack,0,sizeof(m_fCallBack));
	m_fCallBack.f = 0;
	m_fCallBack.lParam = 0;
}

void CMyCallBack::Create(void* pCallBackFunction, unsigned long dwParameterOfApplication)
{
	m_fCallBack.f = (CALLBACK_YLUSBTEL)pCallBackFunction;
	m_fCallBack.lParam = dwParameterOfApplication;
}

CMyCallBack::~CMyCallBack()
{
}

void CMyCallBack::CallBack(unsigned long wParam,void* lParam )
{
	if(m_fCallBack.f)
	{
		(*m_fCallBack.f)((void*)wParam,lParam, m_fCallBack.lParam);
	}
}

unsigned long YL_DeviceIoControl(unsigned long dwIoControlCode, 
		void*	lpInBuffer,  unsigned long	nInBufferSize, 
		void*	lpOutBuffer, unsigned long	nOutBufferSize)
{
	unsigned long br = 0;
	CTelBox *ht = (CTelBox *)&gTelBox;
	try
	{
		br = ht->DeviceIoControl(dwIoControlCode,lpInBuffer,nInBufferSize,lpOutBuffer,nOutBufferSize);
	}
	catch(unsigned long BytesReturned)
	{
		br = BytesReturned;
	}
	return br;
}

unsigned long CTelBox::DeviceIoControl(unsigned long dwIoControlCode, 
			void*	lpInBuffer,  unsigned long	nInBufferSize, 
			void*	lpOutBuffer, unsigned long	nOutBufferSize)
{
	switch(dwIoControlCode){
	case YL_IOCTL_CLOSE_DEVICE:
		if(m_fOpenDevice) Close();
		myHid.CloseHid();
		m_fOpenDevice = false;
		return 0;
		
	case YL_IOCTL_OPEN_DEVICE:
		if(m_fOpenDevice) return YL_RETURN_HID_ISOPENED;
		return Open(lpInBuffer,nInBufferSize,lpOutBuffer,nOutBufferSize);

	default:
		if(!m_fOpenDevice) return YL_RETURN_HID_NO_OPEN;
		break;
	}

	unsigned char b0 = 0;
	unsigned char b1 = 1;
	unsigned char bData[2];
	if(dwIoControlCode<=YL_IOCTL_GEN_TALKING && dwIoControlCode!=YL_IOCTL_GEN_CALLIN)
		m_fRinging = false;
	switch(dwIoControlCode) {
	case YL_IOCTL_GEN_READY:
	case YL_IOCTL_GEN_UNREADY:
		if(m_MyStatus!=dwIoControlCode) m_fEmptyBuffer = true;
		m_MyStatus = dwIoControlCode;
		if(!m_fGenOffhook) {
			if(m_fPSTNOffhook) {
				PrepareData(PSTN_CON_CHANNEL, 0, 1, &b0);
				m_fPSTNOffhook = false;
			}
			if(m_DefaultChannel==0) YL_DeviceIoControl(YL_IOCTL_GEN_GOTOPSTN, 0, 0, 0, 0);
		}
		break;
	case YL_IOCTL_GEN_CALLIN:
		m_MyStatus = YL_IOCTL_GEN_CALLIN;
        unsigned short m_iVersion;
		if(lpInBuffer==0) m_KeyBuffer[0] = 0;
		else {
			unsigned char* pr = (unsigned char*)lpInBuffer;
			int i = 0;
			while(i<50) {
				if(*pr == 0) break;
				m_KeyBuffer[i++] = *pr++;
			}
			m_KeyBuffer[i] = 0;
			myCallBack.CallBack(YL_CALLBACK_GEN_KEYBUF_CHANGED, (void *)(char*)m_KeyBuffer);
		}
		if(!m_fGenOffhook && m_SecRing==0) {
			if(!m_fUSBChannel) YL_DeviceIoControl(YL_IOCTL_GEN_GOTOUSB, 0, 0, 0, 0);
			if(!m_fPSTNOffhook) {
				PrepareData(PSTN_CON_CHANNEL, 0, 1, &b1);
				m_fPSTNOffhook = true;
			}
		}
		if(m_iVersion >= VERSION_NEW_B2K && lpInBuffer != 0)
		{
			unsigned char strCID[11];
			unsigned char* pr = (unsigned char*)lpInBuffer;
			unsigned char* pr2 = (unsigned char*)lpInBuffer;
			int i = 0;
			while(i < 22) {
				while(*pr != 0 && (*pr < '0' || *pr > '9')) continue;
				if(*pr == 0) break;
				if(i % 2 == 0) strCID[i / 2] = (*pr - '0') << 4;
				else strCID[(i - 1) / 2] += *pr - '0';
				*pr++;
				i++;
			}
			if(i % 2 == 1)
			{
				strCID[i / 2] += 0x0F;
				i++;
			}
			for(; i < 22; i = i + 2) strCID[i / 2] = 0xFF;
			int numCID = (*pr2 >= 11) ? 11 : *pr2;
			m_SecSendCID =  ((numCID * 170) + 800) / 50 + 1;
			PrepareData(CALLERID_CON_WRITE, 0, 11, strCID);
//				+++++++++++++++++++++++++++;
		}
		else if(!m_fGenOffhook) m_fRinging = true;
		break;
	case YL_IOCTL_GEN_CALLOUT:
		m_MyStatus = YL_IOCTL_GEN_CALLOUT;
		if(lpInBuffer==0) m_KeyBuffer[0] = 0;
		else {
			unsigned char* pr = (unsigned char*)lpInBuffer;
			int i = 0;
			while(i<50) {
				if(*pr == 0) break;
				m_KeyBuffer[i++] = *pr++;
			}
			m_KeyBuffer[i] = 0;
		}
		myCallBack.CallBack(YL_CALLBACK_GEN_KEYBUF_CHANGED, (void *)(char*)m_KeyBuffer);
		if(m_fSignal) YL_DeviceIoControl(YL_IOCTL_CLOSE_SIGNAL, 0, 0, 0, 0);
		if(!m_fGenOffhook && m_SecRing==0) {
			if(!m_fUSBChannel) YL_DeviceIoControl(YL_IOCTL_GEN_GOTOUSB, 0, 0, 0, 0);
			if(!m_fPSTNOffhook) {
				PrepareData(PSTN_CON_CHANNEL, 0, 1, &b1);
				m_fPSTNOffhook = true;
			}
		}
		break;
	case YL_IOCTL_GEN_RINGBACK:
		m_MyStatus = YL_IOCTL_GEN_RINGBACK;
		break;
	case YL_IOCTL_GEN_TALKING:
		m_MyStatus = YL_IOCTL_GEN_TALKING;
		if(m_fSignal) YL_DeviceIoControl(YL_IOCTL_CLOSE_SIGNAL, 0, 0, 0, 0);
		if(!m_fGenOffhook && m_SecRing==0) {
			if(!m_fUSBChannel) YL_DeviceIoControl(YL_IOCTL_GEN_GOTOUSB, 0, 0, 0, 0);
			if(!m_fPSTNOffhook) {
				PrepareData(PSTN_CON_CHANNEL, 0, 1, &b1);
				m_fPSTNOffhook = true;
			}
		}
		break;
	case YL_IOCTL_GEN_GOTOUSB:
		if(m_fUSBChannel) break;
		PrepareData(LINE_CON_WRITE, 0, 1, &b0);
		bData[0] = 0xFF; bData[1] = 0x00;
		PrepareData(LED_CON_WRITE, 0, 2, bData);
		if(m_MyStatus>=YL_IOCTL_GEN_READY && m_fGenOffhook && !m_fPSTNOffhook) {
			PrepareData(PSTN_CON_CHANNEL, 0, 1, &b1);
			m_fPSTNOffhook = true;
		}
		m_fUSBChannel = true;
		if(m_MyStatus != YL_IOCTL_GEN_CALLIN) {
			m_KeyBuffer[0] = 0;
			myCallBack.CallBack(YL_CALLBACK_GEN_KEYBUF_CHANGED, (void *)(char *)m_KeyBuffer);
		}
		myCallBack.CallBack(YL_CALLBACK_GEN_INUSB, 0);
		break;
	case YL_IOCTL_GEN_GOTOPSTN:
		if(!m_fUSBChannel) break;
		if(m_fPSTNOffhook) {
			PrepareData(PSTN_CON_CHANNEL, 0, 1, &b0);
			m_fPSTNOffhook = false;
		//	Sleep(350);
			m_SecDelay = 7;
			break;
		}
		PrepareData(LINE_CON_WRITE, 0, 1, &b1);
		bData[0] = 0x00; bData[1] = 0xFF;
		PrepareData(LED_CON_WRITE, 0, 2, bData);
		m_fUSBChannel = false;
		m_KeyBuffer[0] = 0;
		myCallBack.CallBack(YL_CALLBACK_GEN_KEYBUF_CHANGED, (void *)(char *)m_KeyBuffer);
		myCallBack.CallBack(YL_CALLBACK_GEN_INPSTN, 0);
		break;
	case YL_IOCTL_GEN_DEFAULTPSTN:
		m_DefaultChannel = 0;
		if(m_fUSBChannel) YL_DeviceIoControl(YL_IOCTL_GEN_GOTOPSTN, 0, 0, 0, 0);
		break;
	case YL_IOCTL_GEN_DEFAULTUSB:
		m_DefaultChannel = 1;
		if(!m_fUSBChannel) YL_DeviceIoControl(YL_IOCTL_GEN_GOTOUSB, 0, 0, 0, 0);
		break;
	case YL_IOCTL_GEN_ONLYUSB:
		m_DefaultChannel = 2;
		if(!m_fUSBChannel) YL_DeviceIoControl(YL_IOCTL_GEN_GOTOUSB, 0, 0, 0, 0);
		break;
	case YL_IOCTL_OPEN_SIGNAL:
		PrepareData(SIGNAL_CON_WRITE, 0, 1, &b1);
		m_fSignal = true;
		break;
	case YL_IOCTL_CLOSE_SIGNAL:
		PrepareData(SIGNAL_CON_WRITE, 0, 1, &b0);
		m_fSignal = false;
		break;
	}
	return 0;
}

unsigned long  CTelBox::Open(void* lpInBuffer, unsigned long nInBufferSize, void* lpOutBuffer, unsigned long nOutBufferSize)
{
	//Search For HID
	unsigned long	dwReturnValue = myHid.OpenHid();
	if(dwReturnValue==YL_RETURN_SUCCESS) {
		m_fOpenDevice = true;
		myCallBack.Create(lpInBuffer,nInBufferSize);
		lOutputReportBuffer[10][1] = KEYBUFFER_PTR_READ;
		lOutputReportBuffer[11][1] = MIO_CON_READ;
		lOutputReportBuffer[10][2] = 0x01;
		lOutputReportBuffer[11][2] = 0x01;
		lOutputReportBuffer[10][16] = 0x7F;
		lOutputReportBuffer[11][16] = 0x72;
		for(int i=3; i<16; i++) {
			lOutputReportBuffer[10][i] = 0;
			lOutputReportBuffer[11][i] = 0;
		}
		m_HookStatus = 0xFF;
		m_RingStatus = 0x00;
		m_fFirstKey = true;
		m_SecDelay = 0;
		m_MyStatus = YL_IOCTL_GEN_UNREADY;
		m_KeyBuffer[0] = 0;
		m_KeyBuffer[51] = 0;
		m_DefaultChannel = 0;
		m_fRinging = false;
		myHid.StartComm(this);

		PrepareData(VERSION_VAL_READ, 0, 2, 0);
		PrepareData(SERIALNO_VAL_READ, 0, 10, 0);

		m_AudioName = myHid.GetUSBAudioName();
		myCallBack.CallBack(YL_CALLBACK_MSG_AUDIONAME, (void *)(char *)m_AudioName);
	}
	else myHid.CloseHid();

	return dwReturnValue;
}


void CTelBox::PrepareData(unsigned char bRequestType,unsigned short wAddr,unsigned short nLength,void* bData)
{
	while(nLength > 0)
	{
	//	memset(lInputReportBuffer[g_iCommTask], 0, 64);
	//	memset(lOutputReportBuffer[g_iCommTask], 0, 64);
		int	i;
		for(i=0; i<64; i++) {
			lInputReportBuffer[g_iCommTask][i] = 0;
			lOutputReportBuffer[g_iCommTask][i] = 0;
		}
		PUSBTEL_COMM pOut = (PUSBTEL_COMM)&lOutputReportBuffer[g_iCommTask][1];
		
		pOut->bRequestType = bRequestType;
		pOut->wOffsetAddress = (wAddr % 256) * 256 + (wAddr >> 8);
//		pOut->wOffsetAddress = wAddr;
		pOut->bSize = (nLength<11)?nLength:11;
		unsigned char* pr = (unsigned char*)bData;
		if(!(bRequestType & 0x80))	//Write
		{
		//	memcpy(pOut->bData, bData, pOut->bSize);
			for(i=0; i<pOut->bSize; i++)
				pOut->bData[i] = *pr++;
		}
		pOut->bVerifyNum = 0;
		unsigned char* pt = (unsigned char*)pOut;
		for(i=0; i<15; i++)
		{
			pOut->bVerifyNum -= *pt++;
		}

		g_iCommTask = (g_iCommTask+1) % 10;

		nLength -= pOut->bSize;
		wAddr += pOut->bSize;
		bData = (char*)bData + pOut->bSize;
	}
}

void CTelBox::ProcessData()
{
	int i = (g_pCommTask==0)?9:g_pCommTask-1;
	PUSBTEL_COMM pOut = (PUSBTEL_COMM)&lOutputReportBuffer[i][1];
	PUSBTEL_COMM pIn = (PUSBTEL_COMM)&lInputReportBuffer[i][1];
	if(pOut->bRequestType != pIn->bRequestType) return;
	switch(pOut->bRequestType) {
	case VERSION_VAL_READ:
		{
			unsigned short m_iVersion;
			m_iVersion = pIn->bData[0] * 256 + pIn->bData[1];
			myCallBack.CallBack(YL_CALLBACK_MSG_USBPHONE_VERSION, (void *)m_iVersion);
			if(m_iVersion < VERSION_B2K) {
				m_iVersion = YL_RETURN_DEV_VERSION_ERROR;
				myCallBack.CallBack(YL_CALLBACK_MSG_WARNING, (void *)m_iVersion);
			}
		}
		break;
	case SERIALNO_VAL_READ:
		{
			unsigned char	b;
		//	unsigned char	bSerialNo[10];
		//	memcpy(bSerialNo, pIn->bData, pOut->bSize);
			unsigned char* pt = (unsigned char*)pIn->bData;
			for(i=0; i<10; i++)	{
				b = *pt++;
				m_SerialNo[i*2] = TransferChar((b & 0xF0) >> 4);
				m_SerialNo[i*2+1] = TransferChar(b & 0x0F);
			}
		//	sprintf(m_SerialNo, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", 
		//		bSerialNo[0], bSerialNo[1], bSerialNo[2], bSerialNo[3], bSerialNo[4],
		//		bSerialNo[5], bSerialNo[6], bSerialNo[7], bSerialNo[8], bSerialNo[9]);
			myCallBack.CallBack(YL_CALLBACK_MSG_USBPHONE_SERIALNO, (void *)(char *)m_SerialNo);
		}
		break;
	case KEYBUFFER_VAL_READ:
		{
			unsigned char m_key;
			unsigned char MyKey=0xFF;
			m_key = pIn->bData[0];
			if(m_key>=0x00 && m_key<=0x09) MyKey=KEY_0+m_key;
			else if(m_key==0x0B) {
				if(!m_fFirstKey || m_DefaultChannel==2) MyKey=KEY_STAR;
				else {
					if(m_fUSBChannel) {
						if(m_MyStatus<YL_IOCTL_GEN_CALLIN) {
							YL_DeviceIoControl(YL_IOCTL_GEN_GOTOPSTN, 0, 0, 0, 0);
						}
					}
					else {
						YL_DeviceIoControl(YL_IOCTL_GEN_GOTOUSB, 0, 0, 0, 0);
						if(!m_fSignal && m_MyStatus<YL_IOCTL_GEN_CALLIN) YL_DeviceIoControl(YL_IOCTL_OPEN_SIGNAL, 0, 0, 0, 0);
					}
				}
			}
			else if(m_key==0x0C) {
				if(m_MyStatus==YL_IOCTL_GEN_READY && m_fUSBChannel)
					MyKey=KEY_SEND;
				else
					MyKey=KEY_POUND;
			}
			if(MyKey!=0xFF && m_fSignal) YL_DeviceIoControl(YL_IOCTL_CLOSE_SIGNAL, 0, 0, 0, 0);
			if(m_MyStatus == YL_IOCTL_GEN_TALKING) MyKey = 0xFF;
			if(MyKey!=0xFF) {
				m_fFirstKey = false;
				myCallBack.CallBack(YL_CALLBACK_GEN_KEYDOWN, (void *)MyKey);
				if(m_fEmptyBuffer) {
					m_KeyBuffer[0] = 0;
					m_fEmptyBuffer = false;
				}
			}

			if(m_MyStatus < YL_IOCTL_GEN_CALLIN)
			{
				if(MyKey >= KEY_0 && MyKey <= KEY_POUND)
				{
					char	key;
					if(MyKey <= KEY_9)
						key = MyKey - KEY_0 + 0x30;
					else if(MyKey==KEY_STAR)
						key = '*';
					else if(MyKey==KEY_POUND)
						key = '#';
					for(int i=0; i<50; i++) if(m_KeyBuffer[i]==0) break;
					if(i<50) {
						m_KeyBuffer[i] = key;
						m_KeyBuffer[i+1] = 0;
					}
					myCallBack.CallBack(YL_CALLBACK_GEN_KEYBUF_CHANGED, (void *)(char *)m_KeyBuffer);
				}
				else if(MyKey!=0xFF)
				{
					m_KeyBuffer[0] = 0;
					myCallBack.CallBack(YL_CALLBACK_GEN_KEYBUF_CHANGED, (void *)(char *)m_KeyBuffer);
				}
			}
		}
		break;
	default:
		break;
	}
}

// Process Two Type Messages : Offhook or Hangup; Ring Message From PSTN
void CTelBox::ProcessIOMessage(unsigned char IOVal)
{
	if((IOVal & 0x02) != m_HookStatus) {
		m_HookStatus = IOVal & 0x02;
		if(m_HookStatus == 0x00) {
			if(m_MyStatus != YL_IOCTL_GEN_CALLIN) {
				m_KeyBuffer[0] = 0;
				myCallBack.CallBack(YL_CALLBACK_GEN_KEYBUF_CHANGED, (void *)(char *)m_KeyBuffer);
			}

			if(m_fSignal) YL_DeviceIoControl(YL_IOCTL_CLOSE_SIGNAL, 0, 0, 0, 0);
			m_fGenOffhook = false;
			myCallBack.CallBack(YL_CALLBACK_GEN_HANGUP, 0);

			if(m_DefaultChannel>0 || m_MyStatus>=YL_IOCTL_GEN_CALLIN) {
				if(!m_fUSBChannel) {
					YL_DeviceIoControl(YL_IOCTL_GEN_GOTOUSB, 0, 0, 0, 0);
					if(m_MyStatus>=YL_IOCTL_GEN_CALLIN) {
						unsigned char b = 1;
						PrepareData(PSTN_CON_CHANNEL, 0, 1, &b);
						m_fPSTNOffhook = true;
					}
				}
			}
			else if(m_DefaultChannel==0 && m_fUSBChannel) YL_DeviceIoControl(YL_IOCTL_GEN_GOTOPSTN, 0, 0, 0, 0);

			if(m_fUSBChannel && m_MyStatus==YL_IOCTL_GEN_CALLIN) m_fRinging = true;
		}
		else {
			if(m_fUSBChannel && m_MyStatus==YL_IOCTL_GEN_CALLIN) {
				unsigned char b = 0;
				PrepareData(RING_CON_WRITE, 0, 1, &b);
			}
			if(!m_fSignal && m_MyStatus<YL_IOCTL_GEN_CALLIN) {
				YL_DeviceIoControl(YL_IOCTL_OPEN_SIGNAL, 0, 0, 0, 0);
			}

			if(m_SecRing>0) {
				PSTNStopRing();
				m_fFirstKey = false;
			}
			else if(m_MyStatus>=YL_IOCTL_GEN_CALLIN) {
				if(!m_fUSBChannel) YL_DeviceIoControl(YL_IOCTL_GEN_GOTOUSB, 0, 0, 0, 0);
			}
			if(m_MyStatus>=YL_IOCTL_GEN_READY && m_fUSBChannel && !m_fPSTNOffhook) {
				unsigned char b = 1;
				PrepareData(PSTN_CON_CHANNEL, 0, 1, &b);
				m_fPSTNOffhook = true;
			}
			m_fGenOffhook = true;
			myCallBack.CallBack(YL_CALLBACK_GEN_OFFHOOK, 0);
			if(m_MyStatus<YL_IOCTL_GEN_CALLIN) m_fFirstKey = true;
		}
	}
	if((IOVal & 0x01) != m_RingStatus) {
		m_RingStatus = IOVal & 0x01;
		if(m_RingStatus==0x01) {
			if(m_SecRing==0) myCallBack.CallBack(YL_CALLBACK_GEN_PSTNRING_START, 0);
			m_SecRing = 110;
		}
		if(m_RingStatus==0x01 && m_fUSBChannel) {
			YL_DeviceIoControl(YL_IOCTL_GEN_GOTOPSTN, 0, 0, 0, 0);
		}
	}
}

void CTelBox::PSTNStopRing()
{
	myCallBack.CallBack(YL_CALLBACK_GEN_PSTNRING_STOP, 0);
	if(m_SecRing > 0) {
		m_SecRing = 0;
		if(m_fUSBChannel) YL_DeviceIoControl(YL_IOCTL_GEN_GOTOPSTN, 0, 0, 0, 0);
	}
	else if(m_DefaultChannel>0 && !m_fGenOffhook || m_MyStatus>=YL_IOCTL_GEN_CALLIN) {
		YL_DeviceIoControl(YL_IOCTL_GEN_GOTOUSB, 0, 0, 0, 0);
		if(m_MyStatus>=YL_IOCTL_GEN_CALLIN) {
			unsigned char b = 1;
			PrepareData(PSTN_CON_CHANNEL, 0, 1, &b);
			m_fPSTNOffhook = true;
		}
	}
}

void CTelBox::Close()
{
	unsigned char b0 = 0;
	unsigned char b1 = 1;
	unsigned char bData[2];
	if(m_fUSBChannel) {
		if(m_fPSTNOffhook) {
			PrepareData(PSTN_CON_CHANNEL, 0, 1, &b0);
			m_fPSTNOffhook = false;
		}
		PrepareData(LINE_CON_WRITE, 0, 1, &b1);
		bData[0] = 0x00; bData[1] = 0xFF;
		PrepareData(LED_CON_WRITE, 0, 2, bData);
		m_fUSBChannel = false;
	}
	if(m_fSignal) PrepareData(SIGNAL_CON_WRITE, 0, 1, &b0);
}

//Transfer From Hex to ASCII
char CTelBox::TransferChar(unsigned char Val)
{
	if(Val > 15) return 0;
	if(Val < 10) return Val+0x30;
	else return Val+0x31;
}
