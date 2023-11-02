#if !defined(AFX_MV2MEM_H__0A5E73D6_18E1_4B20_92C4_AC2AD04A9977__INCLUDED_MV2BASE)
#define AFX_MV2MEM_H__0A5E73D6_18E1_4B20_92C4_AC2AD04A9977__INCLUDED_MV2BASE

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

#include "../include/wintype.h"
#include "../include/RIJNDAEL.h"

#define BUFSIZE 4096
#define DefMV2Symb 2345678
#define TABLE_LENGTH 260

#ifndef _MV2_MEMORY_BUFFER_V_
#define _MV2_MEMORY_BUFFER_V_
#pragma pack(push, 4)
typedef struct tagMV2_BUFFER {
	unsigned char* SrcBuf; // adr Buffer to the src
	unsigned char* CoreBuf; // adr Buffer to the core
	unsigned char* FlagBuf; // adr Buffer to the flags
	int		szSrcBuf; // size of the src buffer or information to the coding 
	int	 	szCoreBuf; // size of the core buffer or core
	int	 	szFlagBuf; // size of the core buffer or flags
	int		RetCode;
}MV2_BUFFER;
#pragma pack(pop)
#endif

#ifdef __GNUC__
#pragma GCC visibility push(hidden)
#endif


union AESCOUNTER{
    _UINT32 dwArr[4];
    _UINT32 dw;
    _UINT8 ch[16];
};
typedef struct tagAESSTATE{
	AESCOUNTER dwCounter;
	_UINT8 iBlock[16] = {};
	_UINT8 aesKey[16] = {};
	int pos = 0;
    int dwStart = 0;

    tagAESSTATE(){
        memset((_UINT8*) dwCounter.ch, 0, 16);
    };

    ~tagAESSTATE(){
        memset((_UINT8*) dwCounter.ch, 0, 16);
    };
	void NextPos() {
		if (++pos >=16) {
			pos = 0;
			dwCounter.dw++;
		}
    };
	void BackPos(){
		if (pos>0) {
			pos--;
		}
		else {
			dwCounter.dw--;
			pos = 15;
		}
    };
	
}AESSTATE;


//////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief The CRand654 class
#ifndef _MV2_CRAND654_
#define _MV2_CRAND654_
class CRand654{ //random generator
public:
	CRand654();
	void InitRand654();
	_UINT32 GetValue();
private:
    union {
        _UINT32  stat[4];
        struct {
            _UINT32 T1;
            _UINT32 T2;
            _UINT32 T3;
        }st;
    };

};
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
/// services functions for shuffling and unshuffling
void Shuffling(_UINT8 * inBuf, _UINT8 * outBuf, _UINT32 dwSize); // shuffling
void UnShuffling(_UINT8 * inBuf, _UINT8 * outBuf, _UINT32 dwSize); // unshuffling
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// основные структуры
//////////////////////////////////////////////////////////////////
typedef struct TABLE_FOR_CHAR// key table
{
	unsigned char _symb[256];	// charcters sequence
	unsigned char _bits[256];	// length core images in bits
	unsigned char _revSym[256];  // charcters table for inversion
	unsigned char _s1;			// first 8-digits symbol (for old)
	unsigned char _t1;			// image for first 8-digits symbol
	unsigned char _s2;			// second 8-digits symbol (for old)
	unsigned char _t2;			// image for second 8-digits symbol
	unsigned short _Mixed_par;	// mix parameters // old
	
}TABLE_FOR_CHAR;
//////////////////////////////////////////////////////////////////

typedef struct _MV2ZAGL{ //service byte representation
	unsigned char Reminder:3;	// count bit to core = count bit added to flags
	unsigned char Counter:5;		// Number of table %32
}_MV2ZAGL;
///////////////////////////////////////////////////////////////////////////////////////////////////
// class noising
typedef struct rc_key
{      						  
	unsigned char state[256] = {};
	unsigned char x = 0;        
	unsigned char y = 0;
} rc_key;

#define swap_byte(x,y) t = *(x); *(x) = *(y); *(y) = t

class CNoiseRC { // whitenining <=> noising
public:
	CNoiseRC() {};
	rc_key m_key;
	inline void prepare_key(unsigned char* key_data_ptr, int key_data_len)
	{
		unsigned char t, index1 = 0, index2 = 0;
		unsigned char* state;
		short counter;

		state = &m_key.state[0];// .state[0] ;//->state[0];
		for (counter = 0; counter < 256; counter++) 	state[counter] = _UINT8(counter);
		m_key.x = 0; m_key.y = 0; 	
		for (counter = 0; counter < 256; counter++)
		{
			index2 = (key_data_ptr[index1] + state[counter] + index2)&0xff;// % 256;
			swap_byte(&state[counter], &state[index2]);
			index1 = (index1 + 1)&0xff;//% key_data_len;
		}
	};
	inline void rc(unsigned char* buffer_ptr, int buffer_len)
	{
		unsigned char t, x, y, xorIndex;
		unsigned char* state;
		short counter;

		x = m_key.x;
		y = m_key.y;
		state = &m_key.state[0];
		for (counter = 0; counter < buffer_len; counter++)
		{
			x = (x + 1) % 256;
			y = (state[x] + y) % 256;
			swap_byte(&state[x], &state[y]);
			xorIndex = (state[x] + state[y]) & 0xFF;// % 256;
			buffer_ptr[counter] ^= state[xorIndex];
		}
		m_key.x = x;
		m_key.y = y;
	};


	
};
////////////////////////////////////////////////////////////////////////////////////////////////////
// implamentation memory operations
///////////////////////////////////////////////////////////////////////////////////////////////////
#define CVMBUF_INVALID_HANDLE  0x00ffffff
#define CVMBUF_INVALID_SIZE    0x00fffffe
#define CVMBUF_INVALID_DATA    0x00fffffd

//service memory class
#define CVMBUF_SET_CURRENT 0x00000001
#define CVMBUF_SET_BEGIN   0x00000002
#define CVMBUF_SET_END     0x00000003
class CVMBuf{
public:
	bool Release();
	_UINT32 GetError();
	bool AllocExisting(_UINT8 * hBuf, _UINT32 dwBufSize);
	_UINT32 SetPointer(long lOffset, _UINT32 dwFlag);
	_UINT32 GetPointer();
	bool Alloc(_UINT32 dwBufSize);
	_UINT32 dwError;
	_UINT32 GetSize();
	AESSTATE m_state;
	inline bool ReadData(_UINT8* tarBuf, _UINT32 dwRead, _UINT32* dwRealRead) 
	{
		dwError = 0;
		if (!Flags & 0x1) {
			*dwRealRead = 0;
			dwError = CVMBUF_INVALID_HANDLE; // invalid Buffer
			return false; // No Read Buffer
		}
		if (dwRead > dwRealSize - Pointer) {	*dwRealRead = dwRealSize - Pointer;	}
		else { *dwRealRead = dwRead; }
		if (*dwRealRead == 0)return true;

		try {
			memcpy(tarBuf, Buf + Pointer, *dwRealRead);
			Pointer += *dwRealRead;
		}
		catch (...) {
			*dwRealRead = 0;
			dwError = CVMBUF_INVALID_DATA;
			return false;
		}
		return true;
	};
	inline bool PseudoReadData(_UINT8*& tarBuf, _UINT32 dwRead, _UINT32* dwRealRead) {
		dwError = 0;
		if (!Flags & 0x1) {
			*dwRealRead = 0;
			dwError = CVMBUF_INVALID_HANDLE; // invalid Buffer
			return false; // No Read Buffer
		}
		if (dwRead > dwRealSize - Pointer) { *dwRealRead = dwRealSize - Pointer; }
		else { *dwRealRead = dwRead; }
		tarBuf = Buf + Pointer;
		Pointer += *dwRealRead;
		return true;
	};

	bool WriteData(_UINT8 *tarBuf, _UINT32 dwWrite, _UINT32 *dwRealWrite);
	bool ReadDataAes(_UINT8 * tarBuf, _UINT32 dwRead, _UINT32 * dwRealRead);
	bool WriteDataAes(_UINT8 *tarBuf, _UINT32 dwWrite, _UINT32 *dwRealWrite);
	void GetHash(unsigned char * output); // return 16 bytes hashvalue
	bool SetSize(_UINT32 dwNewSize = 0);
	CVMBuf();
    RIJNDAEL aes;
	~CVMBuf();
private:
	_UINT32 dwRealSize;
	_UINT32 Flags; 
	_UINT32 Pointer;
	_UINT32 dwSize;
	_UINT8 * Buf;
};
// key size constant defenition
enum KeySize { 
	sizeZero		= 0,
	size128bits		= 16,
	size256bits		= 32,
	size512bits		= 64,
	size1024bits	= 128,
	size2048bits	= 256,
	sizeLast
};
// MV2 errors and state deffinitions
enum MV2ERROR {
	MV2ERR_WORK = -1, // indicate that transformation is active
	MV2ERR_DWORK = -2, // indicate that invers transformation is active
	MV2ERR_OK = 0,     // all work canceled
	MV2ERR_SRCNOTEXIST =1, // error pointer on source text or sorce text size
	MV2ERR_OPEN = 2,	
	MV2ERR_SOURCELEN = 3,
	MV2ERR_INPUTRND = 22,
	MV2ERR_MEMALLOC = 40,	// can't allocated memory
	MV2ERR_DECRYPT = 65,	// decryption error
	MV2ERR_WRITE = 77,		// error write to memory (for example insufficient memory buffer )
	MV2ERR_WRITEFLAG = 79,  // error write to flag buffer
	MV2ERR_CREATEOUT = 80,	// 
	MV2ERR_KEYNOTSET = 99,  // there no any key is set
	MV2ERR_KEYINT = 101,	// can't initilase key
	MV2ERR_ENDFLAGS = 103,	// the flags is finished but core is not finished
	MV2ERR_ENDCORE = 104,	// the core is finished but flags is not finished
	MV2ERR_KEY = 105		// error key is set (old)

};

// key schedule service functions
void kexpan128(_UINT32 * key, _UINT8 * Table);	//// expansion 128 bit to 32 transformation
void kexpan256(_UINT32 * key, _UINT8 * Table); // expansion 256 bit to 32 transformation
void kexpan512B(_UINT32 * key, _UINT8 * Table); // expansion 512 bit to 32 transformation
void kexpan1024(_UINT32 * key, _UINT8 * Table);	// expansion 1024 bit to 32 transformation
void kexpan2048(_UINT32 * key, _UINT8 * Table);	// expansion 2048 bit to 32 transformation

void updatehash( unsigned char * mv2key, unsigned char * output);


#ifdef __GNUC__
#pragma GCC visibility pop
#endif
#endif
