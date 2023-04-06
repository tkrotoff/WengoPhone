#ifndef _FASTMEMCPY_H_
#define _FASTMEMCPY_H_

#include <stddef.h>

const int BLOCK_SIZE = 2048;
BYTE *tbuf[BLOCK_SIZE];

int HasMMX;
int HasSIMD;

int whatCPUExtensions(void);
void fastmemcpy(void *dst, void *src, long nbytes);
void initmemcpy(void);

int whatCPUExtensions(void) {				
	__asm {							
		mov edi,0						
			pushfd							
			pop eax							
			mov edx,eax						
			xor eax,200000h						
			push eax						
			popfd							
			pushfd							
			pop eax							
			and eax,200000h						
			and edx,200000h						
			cmp eax,edx						
			je ExtensionsEnd					
			mov eax,1						
			/* 586 specific code */					
			cpuid							
			test edx,00800000h  // Test bit 23, for MMX existence.	
			je ExtensionsNoMMX					
			or edi,1						
ExtensionsNoMMX:					
		test edx, 02000000h // Test bit 25, for Streaming	
			je ExtensionsNoKNI  //  SIMD Extensions existence.	
			or edi,4						
ExtensionsNoKNI:					
		mov eax,080000000h  // Test CPUID bit 32, for		
			cpuid		    //  3DNow existence.		
			test eax,eax						
			jz ExtensionsEnd					
			mov eax,080000001h					
			cpuid							
			test edx,080000000h					
			je ExtensionsEnd					
			or edi,2						
ExtensionsEnd:						
		mov eax,edi						
	}							
	/*return eax;	*/
}	

void initmemcpy(void)
{
	int opt = whatCPUExtensions();
	if (HasMMX != 0 || HasSIMD != 0) {
		return;
	}
	if (opt == 0)
		HasMMX = 1;
	else if (opt == 2)
		HasSIMD = 1;
	{
		HasMMX = HasSIMD = 0;
	}
}
void fastmemcpy(void *dst, void *src, long nbytes)
{
	if (HasSIMD)
	{
		__asm { 
			mov esi, src 
				mov ecx, nbytes 
				mov ebx, ecx 
				shr ebx, 11 // 2048 bytes at a time 

				mov edi, dst 
				
loop2k: // Copy 2k into temporary buffer 

			push edi 
				mov edi, tbuf 
				mov ecx, 2048 
				shr ecx, 6 
				
loopMemToL1: 
			prefetchnta 64[ESI] // Prefetch next loop, non-temporal 

			prefetchnta 96[ESI] 
				
				movq mm1,  0[ESI] // Read in source data 

				movq mm2,  8[ESI] 
				movq mm3, 16[ESI] 
				movq mm4, 24[ESI] 
				movq mm5, 32[ESI] 
				movq mm6, 40[ESI] 
				movq mm7, 48[ESI] 
				movq mm0, 56[ESI] 
				
				movq  0[EDI], mm1 // Store into L1 

				movq  8[EDI], mm2 
				movq 16[EDI], mm3 
				movq 24[EDI], mm4 
				movq 32[EDI], mm5 
				movq 40[EDI], mm6 
				movq 48[EDI], mm7 
				movq 56[EDI], mm0 
				add esi, 64 
				add edi, 64 
				dec ecx 
				jnz loopMemToL1 
				
				pop edi // Now copy from L1 to system memory 

				push esi 
				mov esi, tbuf 
				mov ecx, 2048 
				shr ecx, 6 
				
loopL1ToMem: 
			movq mm1, 0[ESI] // Read in source data from L1 

				movq mm2, 8[ESI] 
				movq mm3, 16[ESI] 
				movq mm4, 24[ESI] 
				movq mm5, 32[ESI] 
				movq mm6, 40[ESI] 
				movq mm7, 48[ESI] 
				movq mm0, 56[ESI] 
				
				movntq 0[EDI], mm1 // Non-temporal stores 

				movntq 8[EDI], mm2 
				movntq 16[EDI], mm3 
				movntq 24[EDI], mm4 
				movntq 32[EDI], mm5 
				movntq 40[EDI], mm6 
				movntq 48[EDI], mm7 
				movntq 56[EDI], mm0 
				
				add esi, 64 
				add edi, 64 
				dec ecx 
				jnz loopL1ToMem 
				
				pop esi // Do next 2k block 

				dec ebx 
				jnz loop2k 
		} 

	}
	else if (HasMMX)
	{
		_asm { 
			mov esi, src 
				mov edi, dst 
				mov ecx, nbytes 
				shr ecx, 6 // 64 bytes per iteration 

				
loop1: 
			movq mm1,  0[ESI] // Read in source data 

				movq mm2,  8[ESI] 
				movq mm3, 16[ESI] 
				movq mm4, 24[ESI] 
				movq mm5, 32[ESI] 
				movq mm6, 40[ESI] 
				movq mm7, 48[ESI] 
				movq mm0, 56[ESI] 
				
				movntq  0[EDI], mm1 // Non-temporal stores 

				movntq  8[EDI], mm2 
				movntq 16[EDI], mm3 
				movntq 24[EDI], mm4 
				movntq 32[EDI], mm5 
				movntq 40[EDI], mm6 
				movntq 48[EDI], mm7 
				movntq 56[EDI], mm0 
				
				add esi, 64 
				add edi, 64 
				dec ecx 
				jnz loop1 
				
				emms 
		}
	} 


	
	if (nbytes < BLOCK_SIZE || (!HasMMX && !HasSIMD))
	{
		memcpy(dst,src,nbytes);
	}

	else
	{
		int Balance = nbytes % BLOCK_SIZE;
		if (Balance > 0)
		{
			BYTE *dest		= (BYTE*)dst+nbytes-Balance;
			BYTE *source	= (BYTE*)src+nbytes-Balance;
			memcpy(dest,source,Balance);
		}
	}
}

#endif
