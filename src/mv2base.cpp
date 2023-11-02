//mv2 -service file 
/*
   Copyright [2021] [Yury Vilanski]

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "stdio.h"
#include <stdlib.h>
#include <time.h>
#include "../include/mv2base.h"


/////////////////////////////////////////////////////////////////////////////////////////////
// Service functions
//////////////////////////////////////////////////////////
//#if 0
inline void Shuff16(_UINT32 dwBuf[4]){
    dwBuf[0] = (dwBuf[0] >> 19) | (dwBuf[0] << 13);		//a<<<13
    dwBuf[2] = (dwBuf[2] >> 29) | (dwBuf[2] << 3);		//c<<<3
    dwBuf[3] = dwBuf[3]^(dwBuf[2]^(dwBuf[0] << 3));	// d ^= c^(a<<3)
    dwBuf[1] = dwBuf[1]^(dwBuf[0]^dwBuf[2]);		// b ^= (a^c)
    dwBuf[3] = (dwBuf[3] >> 25) | (dwBuf[3] << 7);		//d<<<7
    dwBuf[1] = (dwBuf[1] << 1) | (dwBuf[1] >> 31);		// b<<<1
    dwBuf[0] = dwBuf[0]^(dwBuf[1]^dwBuf[3]);		// a ^= b^d
    dwBuf[2] = dwBuf[2]^(dwBuf[3]^(dwBuf[1] << 7));	// c ^= d^(b<<7)
    dwBuf[0] = (dwBuf[0] >> 27) | (dwBuf[0] << 5);		//a<<<5
    dwBuf[2] = (dwBuf[2] >> 10) | (dwBuf[2] << 22);		//c<<<22
}
//#endif
#if 0
inline void Shuff16(_UINT32 dwBuf[4]){
    __asm {
        mov		eax, dwBuf		; //load adress
        mov		ebx, dword ptr [eax]	; //a ->ebx
        mov		ecx, dword ptr [eax+4]	; //b ->ecx
        mov		edx, dword ptr [eax+8]	; //c ->edx

        // a = a <<< 13;
        rol		ebx, 13		; //ebx <-a<<<13
        // c = c <<< 3;
        rol		edx, 3		; //edx <-c<<<3
        // b = (a^b^c)<<<1
        // d = ((a<<3)^d^c)<<<7
        mov		_UINT32 PTR [eax], ebx ; //eax -> a
        xor		ecx, edx				;
        mov		_UINT32 PTR [eax+8], edx	; //edx -> c
        xor		ecx, ebx				;
        xor		edx, _UINT32 PTR [eax+12] ; //edx <- d^c
        shl		ebx, 3					;
        rol		ecx, 1					; //b-> ecx
        xor		edx, ebx				; //d^c^(a<<3) -> edx
        mov		_UINT32 PTR [eax+4], ecx	; //ecx -> b
        rol		edx, 7					; //d-> edx
        // a = (a^b^d)<<<5
        mov		ebx, ecx				;  //b-> ebx
        mov		_UINT32 PTR [eax+12], edx	; //edx -> d
        xor		ebx, _UINT32 PTR [eax]	; // a^b -> eñx;
        shl		ecx, 7					; // ecx <- b<<7
        xor		ebx, edx				; //ebx <- a^b^d
        //c = (c^d^(b<<7))<<<22
        xor		ecx,  _UINT32 PTR [eax+8]	; //ecx <- c^ (b<<7)
        rol		ebx, 5					;
        xor		ecx, edx		; //ecx <- (b<<7)^d
        mov		_UINT32 PTR [eax], ebx	; //ebx -> a
        rol		ecx, 22					;
        mov		_UINT32 PTR [eax+8], ecx	; //ecx -> c

    }
}
#endif
//#if 0
inline void UnShuff16(_UINT32 dwBuf[4]){
    dwBuf[2] = (dwBuf[2] << 10)|(dwBuf[2] >> 22);		//c>>>22
    dwBuf[0] = (dwBuf[0] << 27)|(dwBuf[0] >> 5);		//a>>>5
    dwBuf[2] = dwBuf[2]^(dwBuf[3]^(dwBuf[1] << 7));	// c ^= d^(b<<7)
    dwBuf[0] = dwBuf[0]^(dwBuf[1]^dwBuf[3]);		// a ^= b^d
    dwBuf[1] = (dwBuf[1] >> 1)|(dwBuf[1] << 31);		// b>>>1
    dwBuf[3] = (dwBuf[3] << 25)|(dwBuf[3] >> 7);		//d>>>7
    dwBuf[1] = dwBuf[1]^(dwBuf[0]^dwBuf[2]);		// b ^= (a^c)
    dwBuf[3] = dwBuf[3]^(dwBuf[2]^(dwBuf[0] << 3));	// d ^= c^(a<<3)
    dwBuf[2] = (dwBuf[2] << 29) | (dwBuf[2] >> 3);		//c>>>3
    dwBuf[0] = (dwBuf[0] << 19) | (dwBuf[0] >> 13);		//a<<<13

}
//#endif
#if 0
inline void UnShuff16(_UINT32 dwBuf[4]){

    dwBuf[2] = (dwBuf[2] << 10)|(dwBuf[2] >> 22);		//c>>>22
    dwBuf[0] = (dwBuf[0] << 27)|(dwBuf[0] >> 5);		//a>>>5
    dwBuf[2] = dwBuf[2]^(dwBuf[3]^(dwBuf[1] << 7));	// c ^= d^(b<<7)
    dwBuf[0] = dwBuf[0]^(dwBuf[1]^dwBuf[3]);		// a ^= b^d
    dwBuf[1] = (dwBuf[1] >> 1)|(dwBuf[1] << 31);		// b>>>1
    dwBuf[3] = (dwBuf[3] << 25)|(dwBuf[3] >> 7);		//d>>>7
    dwBuf[1] = dwBuf[1]^(dwBuf[0]^dwBuf[2]);		// b ^= (a^c)
    dwBuf[3] = dwBuf[3]^(dwBuf[2]^(dwBuf[0] << 3));	// d ^= c^(a<<3)
    dwBuf[2] = (dwBuf[2] << 29) | (dwBuf[2] >> 3);		//c>>>3
    dwBuf[0] = (dwBuf[0] << 19) | (dwBuf[0] >> 13);		//a<<<13
//#endif
//#if 0
    __asm {
        mov eax, dwBuf		; //load adress

//	dwBuf[2] = (dwBuf[2] << 10)|(dwBuf[2] >> 22);		//c>>>22
        mov ecx, dword ptr [eax+8]	; //eax <- c
        ror ecx, 22					;
//	dwBuf[0] = (dwBuf[0] << 27)|(dwBuf[0] >> 5);		//a>>>5
        mov ebx, dword ptr [eax]		;
        ror ebx, 5						;
//	dwBuf[2] = dwBuf[2]^(dwBuf[3]^(dwBuf[1] << 7));	// c ^= d^(b<<7)
        mov edx, dword ptr [eax+4]		; // edx <- b
        shl edx, 7						;  //b << 7
        xor ecx, edx					; // c = c^ edx
        mov edx, dword ptr [eax+12]		;	//edx <- d
        xor ecx, edx					;

//	dwBuf[0] = dwBuf[0]^(dwBuf[1]^dwBuf[3]);		// a ^= b^d
        xor ebx, edx					;
        xor ebx, dword ptr [eax+4]		;
        mov dword ptr [eax], ebx		;
//	dwBuf[1] = (dwBuf[1] >> 1)|(dwBuf[1] << 31);		// b>>>1
//	dwBuf[3] = (dwBuf[3] << 25)|(dwBuf[3] >> 7);		//d>>>7
        ror edx, 7	;
        mov dword ptr [eax+12], edx		;
        mov edx, dword ptr [eax+4]		;
        ror edx, 1	;
//	dwBuf[1] = dwBuf[1]^(dwBuf[0]^dwBuf[2]);		// b ^= (a^c)
        xor edx, ebx	;
        xor edx, ecx	;
        mov dword ptr [eax+4], edx

//	dwBuf[3] = dwBuf[3]^(dwBuf[2]^(dwBuf[0] << 3));	// d ^= c^(a<<3)
        mov edx, dword ptr [eax+12]	;
        shl ebx, 3
        xor edx, ebx;
        xor edx, ecx;
        mov ebx, dword ptr [eax]		;
        mov dword ptr [eax+12], edx;
//	dwBuf[2] = (dwBuf[2] << 29) | (dwBuf[2] >> 3);		//c>>>3
        ror ecx, 3
//	dwBuf[0] = (dwBuf[0] << 19) | (dwBuf[0] >> 13);		//a<<<13
        ror ebx, 13				;
        mov dword ptr [eax+8], ecx;
        mov dword ptr [eax], ebx;
    }
}
#endif
//inline
void Shuffling(_UINT8 * inBuf, _UINT8 * outBuf, _UINT32 dwSize){ // shuffling
    // inBuf  - input buffer
    // outBuf - output uffer sizeof(outBuf) >= sizeof(inBuf) >= dwSize
    // dwSize - size of input Buffer
    _UINT32 dwBUF[4];
//	_UINT8 *  bBuf = (_UINT8 *) &dwBUF[0];
    _UINT32 dwLoop = (dwSize>>4);
    _UINT32 dwAdd = dwSize - (dwLoop<<4);
    int i = 0;
    for (i=0; i < (int) dwLoop; i++){
        memcpy((_UINT8 *) dwBUF,inBuf+i*16,sizeof(dwBUF));
        Shuff16(dwBUF);
        memcpy((_UINT8 *) outBuf+i*16,dwBUF,sizeof(dwBUF));
    }
    if (dwAdd) memcpy(outBuf+16*dwLoop, inBuf+16*dwLoop,dwAdd);
#if 0
    if ((dwAdd >= 8)&&(dwLoop)) {
        memcpy(bBuf,inBuf+16*dwLoop,dwAdd);
        memcpy(bBuf+dwAdd,outBuf,16-dwAdd);
        Shuff16(dwBUF);
        memcpy(outBuf+16*dwLoop, bBuf,dwAdd);
        memcpy(outBuf,bBuf+dwAdd,16-dwAdd);
    }
    else {
        if (dwAdd) memcpy(outBuf + dwLoop*16, inBuf+16*dwLoop,dwAdd);
    }
#endif
}



// BUFFER Memory Classes
CVMBuf::CVMBuf( ){
	Flags = 0;
	Pointer = 0;
	Buf = 0;
	dwSize = 0;
	dwError = -1;
	dwRealSize = 0;
};


bool CVMBuf::AllocExisting(_UINT8 *hBuf, _UINT32 dwBufSize)
{
	if (Buf) {
		dwError = CVMBUF_INVALID_HANDLE;
		return false;
	}
	Flags = 0;
	if (!IsBadReadPtr(hBuf,dwBufSize)){
		Flags = Flags|0x00000001; // read dats
	}
//	if (!IsBadWritePtr(hBuf,dwBufSize)){
//		Flags = Flags|0x00000002; // write buf
//	}
	Pointer = 0;
	Buf = hBuf;
	dwSize = dwBufSize;
	dwRealSize = dwSize;
	dwError = 0;
	return true;
};



//////////////////////////////
CVMBuf::~CVMBuf(){
	if ((Flags & 0x00000004)){
		ZeroMemory(Buf, dwSize);
        free(Buf);
	}
}

bool CVMBuf::Release()
{
	if ((Flags & 0x00000004)){
		ZeroMemory(Buf, dwSize);
        free(Buf);
//        dwError = 0;
//		else
  //          dwError = CVMBUF_INVALID_HANDLE;
	}
//	else {
//		dwError = 0;
//	}
    dwError = 0;
	Buf  = 0;
	Flags = 0;
	Pointer = 0;
	dwRealSize = 0;
	dwSize = 0;
	return true;
}


//////////////////////////////
bool CVMBuf::Alloc(_UINT32 dwBufSize)
{
	dwError = 0;
	if (Buf) {
		dwError = CVMBUF_INVALID_HANDLE;
		return false;
	}
	Flags = 0;
	Pointer = 0;
	dwRealSize = 0;
	dwBufSize = (((dwBufSize+15)>>4)<<4);
    Buf = (_UINT8 *) malloc(dwBufSize);
	if (!IsBadReadPtr(Buf,dwBufSize)){
		Flags = Flags | 0x00000005; // read bufs and data was be allocated program
        dwSize = dwBufSize;
    }
    else {
        dwError = CVMBUF_INVALID_DATA;
        dwSize = 0;
    }
	return true;
}
//////////////////////////////

// read data to buffer
#if 0
bool CVMBuf::ReadData(_UINT8 *tarBuf, _UINT32 dwRead, _UINT32 *dwRealRead)
{
	dwError = 0;
	if (!Flags&0x1) {
		*dwRealRead = 0;
		dwError = CVMBUF_INVALID_HANDLE; // invalid Buffer
		return false; // No Read Buffer
	}
	if (dwRead > dwRealSize-Pointer){
		*dwRealRead = dwRealSize-Pointer;
	}
	else *dwRealRead = dwRead;
	if (*dwRealRead == 0)return true;

	try {
		memcpy(tarBuf,Buf+Pointer,*dwRealRead);
		Pointer += *dwRealRead;
	}
	catch(...){
		*dwRealRead = 0;
		dwError = CVMBUF_INVALID_DATA;
		return false;
	}
	return true;
}
#endif
//////////////////////////////////////////////////////////
// Read encrypted data in CTR Mode data to buffer

bool CVMBuf::ReadDataAes(_UINT8 * tarBuf, _UINT32 dwRead, _UINT32 * dwRealRead){
	_UINT32 i =0;
	dwError = 0;	
	if (!Flags&0x1) {
		*dwRealRead = 0;
		dwError = CVMBUF_INVALID_HANDLE; // invalid Buffer
		return false; // No Read Buffer
	}
	if (dwRead > dwRealSize-Pointer){
		*dwRealRead = dwRealSize-Pointer;
	}
	else 
		*dwRealRead = dwRead;
	if (*dwRealRead == 0) return true;

	try {
		if (m_state.dwCounter.dw != (m_state.dwStart + (Pointer>>4)))
		{// prepare dwCounter if not ready
			m_state.dwCounter.dw = m_state.dwStart + (Pointer>>4);
			aes.Encrypt(m_state.dwCounter.ch,m_state.iBlock);
		}
		m_state.pos = Pointer - ((Pointer>>4)<<4); // calculate iBlock position
        for ( i =0; i< *dwRealRead; i++){
			tarBuf[i] = Buf[Pointer +i] ^ m_state.iBlock[m_state.pos];
			m_state.pos++;
			if (m_state.pos >= 16){
				m_state.pos = 0;
				m_state.dwCounter.dw++;
				aes.Encrypt(m_state.dwCounter.ch,m_state.iBlock);
			}
		}
		Pointer += *dwRealRead;

	}
	catch(...){
		*dwRealRead = 0;
		dwError = CVMBUF_INVALID_DATA;
		return false;
	}

	return true;	
}
//////////////////////////////////////////////////////////
// write encrypted data in CTR Mode data to buffer

bool CVMBuf::WriteDataAes(_UINT8 *tarBuf, _UINT32 dwWrite, _UINT32 *dwRealWrite){
	_UINT32 i =0;
	dwError = 0;
	if (dwWrite == 0){
		*dwRealWrite = 0;
		return true;
	}
	if (dwWrite > dwSize-Pointer){
		*dwRealWrite = 0;//dwSize-Pointer;
	}
	else
		*dwRealWrite = dwWrite;
	if (*dwRealWrite == 0){
		dwError = CVMBUF_INVALID_SIZE; // mem
		return false;
	}
	if (IsBadReadPtr(tarBuf,*dwRealWrite)){
		*dwRealWrite = 0;
		dwError = CVMBUF_INVALID_DATA;
		return false;
	}
	try {
		if (m_state.dwCounter.dw != (m_state.dwStart + (Pointer>>4)))
		{// prepare dwCounter if not ready
			m_state.dwCounter.dw = m_state.dwStart + (Pointer>>4);
			aes.Encrypt(m_state.dwCounter.ch,m_state.iBlock);
		}
		m_state.pos = Pointer - ((Pointer>>4)<<4); // calculate iBlock position

		for(i = 0; i < *dwRealWrite; i++){
			Buf[Pointer +i] = tarBuf[i]^ m_state.iBlock[m_state.pos];
			m_state.pos++;
			if (m_state.pos >= 16){
				m_state.pos = 0;
				m_state.dwCounter.dw++;
				aes.Encrypt(m_state.dwCounter.ch,m_state.iBlock);
			}
		}
//		memcpy(Buf+Pointer,tarBuf,*dwRealWrite);
	}
	catch(...){
		*dwRealWrite = 0;
		dwError = CVMBUF_INVALID_DATA;
		return false;
	}
	Pointer += *dwRealWrite;
	if (Pointer > dwRealSize) 	dwRealSize = Pointer;
	return true;
}

//////////////////////////////
// write data to buffer

bool CVMBuf::WriteData(_UINT8 *tarBuf, _UINT32 dwWrite, _UINT32 *dwRealWrite)
{
	if (dwWrite == 0){
		*dwRealWrite = 0;
		dwError = 0;
		return true;
	}
//	if (!(Flags & 0x00000002)) {
//		*dwRealWrite = 0;
//		dwError = CVMBUF_INVALID_HANDLE;
//		return false; // No Read Buffer
//	}
	if (dwWrite > dwSize-Pointer){
		*dwRealWrite = 0;// dwSize-Pointer;
		dwError = CVMBUF_INVALID_SIZE; // mem
		return false;
	}
	else *dwRealWrite = dwWrite;
//	if ((*dwRealWrite == 0)&&(dwWrite>0)){
//		dwError = CVMBUF_INVALID_SIZE; // mem
//		return false;
//	}
	if (IsBadReadPtr(tarBuf,*dwRealWrite)){
		*dwRealWrite = 0;
		dwError = CVMBUF_INVALID_DATA;
		return false;
	}
	try {
		memcpy(Buf+Pointer,tarBuf,*dwRealWrite);
	}
	catch(...){
		*dwRealWrite = 0;
		dwError = CVMBUF_INVALID_DATA;
		return false;
	}
	Pointer += *dwRealWrite;
	if (Pointer > dwRealSize) 	dwRealSize = Pointer;
	dwError = 0;
	return true;
}
//////////////////////////////
_UINT32 CVMBuf::GetPointer()
{
	dwError = 0;
	return Pointer;
}
//////////////////////////////

_UINT32 CVMBuf::SetPointer(long lOffset, _UINT32 dwFlag)
{
	switch (dwFlag){
	case CVMBUF_SET_CURRENT:
		if (lOffset < 0){
			lOffset = -lOffset;
			if ((_UINT32) lOffset > Pointer) Pointer = 0;
			else Pointer -= lOffset;
		}
		else {
			if ((lOffset +Pointer) > dwRealSize) Pointer = dwRealSize;
			else Pointer += lOffset;
		}
		break;
	case CVMBUF_SET_BEGIN:
		if (lOffset < 0) Pointer = 0;
		else {
			if ((_UINT32) lOffset > dwRealSize) Pointer = dwRealSize;
			else{
				Pointer = lOffset;
			}
		}
		break;
	case CVMBUF_SET_END:
		if (lOffset < 0) lOffset = 0 - lOffset;
		if ( (dwRealSize - Pointer) > (_UINT32) lOffset) Pointer = 0;
		else Pointer = dwRealSize - lOffset;
		break;
	}
	dwError = 0;
	return Pointer;
}
/////////////////////////////////////
_UINT32 CVMBuf::GetError()
{
 return dwError;
}

_UINT32 CVMBuf::GetSize()
{
 return dwRealSize;
}

bool CVMBuf::SetSize(_UINT32 dwNewSize ){
	bool bRet = true;
	if (dwNewSize){
		if (dwNewSize > dwSize) {
			dwNewSize = dwSize;
			bRet = false;
		}
	}
	dwRealSize = dwNewSize;
	Pointer = dwRealSize;
	return bRet;
}

void CVMBuf::GetHash(unsigned char * output)
{
	_UINT32 dwBuf[4] = {0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF};
	_UINT32 i;
	if (!Flags&0x1) {
		dwError = CVMBUF_INVALID_HANDLE; // invalid Buffer
		return;
	}
	unsigned char * ucTemp = (unsigned char *) dwBuf;
	try{
		for (i= 0; i < dwRealSize; i++){
			ucTemp[i&0x0000000f] =  ucTemp[i&0x0000000f]^Buf[i];
			if ((i&0x0000000f) == 0x0000000f){
				dwBuf[0] = (dwBuf[0] >> 19) | (dwBuf[0] << 13);		//a<<<13
				dwBuf[2] = (dwBuf[2] >> 29) | (dwBuf[2] << 3);		//c<<<3
				dwBuf[3] = dwBuf[3]^(dwBuf[2]^(dwBuf[0] << 3));	// d ^= c^(a<<3)
				dwBuf[1] = dwBuf[1]^(dwBuf[0]^dwBuf[2]);		// b ^= (a^c)
				dwBuf[3] = (dwBuf[3] >> 25) | (dwBuf[3] << 7);		//d<<<7
				dwBuf[1] = (dwBuf[1] << 1) | (dwBuf[1] >> 31);		// b<<<1
				dwBuf[0] = dwBuf[0]^(dwBuf[1]^dwBuf[3]);		// a ^= b^d
				dwBuf[2] = dwBuf[2]^(dwBuf[3]^(dwBuf[1] << 7));	// c ^= d^(b<<7)
				dwBuf[0] = (dwBuf[0] >> 27) | (dwBuf[0] << 5);		//a<<<5
				dwBuf[2] = (dwBuf[2] >> 10) | (dwBuf[2] << 22);		//c<<<22
			}//shuffling
		}
		dwBuf[0] = dwBuf[0]^0xffffffff;
		dwBuf[1] = dwBuf[1]^0xffffffff;;
		dwBuf[2] = dwBuf[2]^0xffffffff;; 
		dwBuf[3] = dwBuf[3]^0xffffffff;;
		dwBuf[0] = (dwBuf[0] >> 19) | (dwBuf[0] << 13);		//a<<<13
		dwBuf[2] = (dwBuf[2] >> 29) | (dwBuf[2] << 3);		//c<<<3
		dwBuf[3] = dwBuf[3]^(dwBuf[2]^(dwBuf[0] << 3));	// d ^= c^(a<<3)
		dwBuf[1] = dwBuf[1]^(dwBuf[0]^dwBuf[2]);		// b ^= (a^c)
		dwBuf[3] = (dwBuf[3] >> 25) | (dwBuf[3] << 7);		//d<<<7
		dwBuf[1] = (dwBuf[1] << 1) | (dwBuf[1] >> 31);		// b<<<1
		dwBuf[0] = dwBuf[0]^(dwBuf[1]^dwBuf[3]);		// a ^= b^d
		dwBuf[2] = dwBuf[2]^(dwBuf[3]^(dwBuf[1] << 7));	// c ^= d^(b<<7)
		dwBuf[0] = (dwBuf[0] >> 27) | (dwBuf[0] << 5);		//a<<<5
		dwBuf[2] = (dwBuf[2] >> 10) | (dwBuf[2] << 22);		//c<<<22
		memcpy(output, (unsigned char *) dwBuf , 16); 
	}
	catch(...){
		return;
	}
}




/////////////////////////////////////////////////////////////////////////////////////////////
// Class CRand654;
/////////////////////////////////////
CRand654::CRand654(){
    InitRand654();
#if 0
    srand( (unsigned)time( NULL ) );
    st.T1 = rand()*GetTickCount();
    st.T2 = rand(); st.T2 = st.T2<<15; st.T2 += rand(); st.T2 *=rand();
    st.T3 = GetTickCount()*rand(); st.T3 = st.T3 << (rand()%13); st.T3 = st.T3*rand();
#endif
}
/////////////////////////////////////
void CRand654::InitRand654(){
    timespec tm;
    timespec_get(&tm, TIME_UTC);
    memcpy((_UINT8 *) &stat[0], &tm, _min(sizeof(timespec), sizeof(stat)));
    Shuff16(stat);

//	SYSTEM_INFO sInfo;
//	::GetSystemInfo(&sInfo);
	srand( (unsigned)time( NULL ) );
    st.T1 += rand()*GetTickCount0()+stat[3];
//    st.T2 = rand()*::GetVersion();
    st.T2 = (st.T2<<15)+stat[1];//sInfo.dwOemId;
    st.T2 += rand()+clock();// st.T2 *=rand()+clock();//sInfo.dwProcessorType;
    st.T3 -= GetTickCount0()*rand();
    st.T3 = stat[3]+(st.T3 << (rand()%13));
    UnShuff16(stat);
    st.T1 = stat[3];

//    st.T3 = (st.T3*rand()<<sInfo.dwNumberOfProcessors)+sInfo.dwAllocationGranularity - sInfo.dwActiveProcessorMask+::GetTickCount();
}
/////////////////////////////////////
_UINT32 CRand654::GetValue(){
	_UINT64 L1,L2,L3,L4;
    if ((st.T1==0)||(st.T2==0)||(st.T3==0)){
        timespec tm;
        timespec_get(&tm, 0);
		srand((unsigned)time( NULL ) + GetTickCount0());
        if (st.T1==0)	st.T1 = (GetTickCount0()+tm.tv_sec)*rand()+tm.tv_nsec;
        if (st.T2==0)	st.T2 = (GetTickCount0()-rand())*tm.tv_sec+tm.tv_nsec;
        if (st.T3==0)	st.T3 = (GetTickCount0()+tm.tv_nsec)*tm.tv_sec+rand();
	}
    L1 = st.T1; L2 = st.T2; L3 = st.T3;
	L4  = L1+L2+L3;
	L4 = Int64ShllMod32(L4,13);
    st.T1 = L2;
    st.T2 = L3;
	L4 = L4%((_UINT32)0xFFFFFFFB);//4294967291
    st.T3 = L4;
    return st.T3;
}

//inline 
void UnShuffling(_UINT8 * inBuf, _UINT8 * outBuf, _UINT32 dwSize){ // shuffling
	// inBuf  - input buffer
	// outBuf - output uffer sizeof(outBuf) >= sizeof(inBuf) >= dwSize
	// dwSize - size of input Buffer
	_UINT32 dwBUF[4];
//	_UINT8 * bBuf = (_UINT8 *) &dwBUF[0];
	_UINT32 dwLoop = (dwSize>>4);
	_UINT32 dwAdd = dwSize - (dwLoop<<4);
	int i = 0;
#if 0
	if ((dwAdd >= 8)&&(dwLoop)) {
		memcpy(bBuf,inBuf+16*dwLoop,dwAdd);
		memcpy(bBuf+dwAdd,inBuf,16-dwAdd);
		UnShuff16(dwBUF);
		memcpy(outBuf+16*dwLoop, bBuf,dwAdd);
		memcpy(outBuf,bBuf+dwAdd,16-dwAdd);
		// first block
		memcpy(bBuf,outBuf,16-dwAdd);
		memcpy(bBuf,inBuf+16-dwAdd,dwAdd);
	}
	else{
		if (dwLoop){
			memcpy(bBuf,inBuf,sizeof(dwBUF));
		}
	}
	if (dwLoop){
		UnShuff16(dwBUF);
		memcpy(outBuf,dwBUF,sizeof(dwBUF));

	}
//or (i=1; i < dwLoop; i++){
#endif
	for (i=0; i < (int) dwLoop; i++){
        memcpy((_UINT8 *)dwBUF,inBuf+i*16,sizeof(dwBUF));
		UnShuff16(dwBUF);
        memcpy(outBuf+i*16,(_UINT8 *) dwBUF,sizeof(dwBUF));
	}
	if (dwAdd) memcpy(outBuf+16*dwLoop, inBuf+16*dwLoop,dwAdd);
}
////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
void updatehash( unsigned char * mv2key, unsigned char * output){
	
	int i, j;
	_UINT32 dwTemp[4];
	_UINT32 dwTemp1[4];
	//memset(output, 0xff, 16);
	//j =0;
	// core shuffling
//	for (i=0; i< szCore; i++){
//		output[i&0x0000000f] =  output[i&0x0000000f]^pCore[i];
//		if (!(i&0x0000000f)){
//			Shuffling(output, output,16);
//		}
//	}
//	Shuffling(output, output,16);
	try{
		memcpy((unsigned char *) dwTemp, output, 16);
		for (i=0; i <32; i++){ // 32 table preparing
			for (j=0; j< 8; j++){
				memcpy((unsigned char *) dwTemp1, mv2key + 260*i + 16*j, 16);
				dwTemp[0] = dwTemp[0]^dwTemp1[0];
				dwTemp[1] = dwTemp[1]^dwTemp1[1];
				dwTemp[2] = dwTemp[2]^dwTemp1[2];
				dwTemp[3] = dwTemp[3]^dwTemp1[3];

				dwTemp[2] = (dwTemp[2] << 10)|(dwTemp[2] >> 22);		//c>>>22
				dwTemp[0] = (dwTemp[0] << 27)|(dwTemp[0] >> 5);		//a>>>5
				dwTemp[2] = dwTemp[2]^(dwTemp[3]^(dwTemp[1] << 7));	// c ^= d^(b<<7)
				dwTemp[0] = dwTemp[0]^(dwTemp[1]^dwTemp[3]);		// a ^= b^d
                dwTemp[1] = (dwTemp[1] >> 1)|(dwTemp[1] << 31);		// b>>>1
				dwTemp[3] = (dwTemp[3] << 25)|(dwTemp[3] >> 7);		//d>>>7
				dwTemp[1] = dwTemp[1]^(dwTemp[0]^dwTemp[2]);		// b ^= (a^c)
				dwTemp[3] = dwTemp[3]^(dwTemp[2]^(dwTemp[0] << 3));	// d ^= c^(a<<3)
				dwTemp[2] = (dwTemp[2] << 29) | (dwTemp[2] >> 3);		//c>>>3
				dwTemp[0] = (dwTemp[0] << 19) | (dwTemp[0] >> 13);		//a<<<13
			}// end table preparing
		}
		memcpy(output, (unsigned int *) dwTemp, 16);
	}
	catch(...){
		return;
	}

}
