// RIJNDAEL.h: interface for the RIJNDAEL class.
//
//////////////////////////////////////////////////////////////////////

#ifndef	SECURED_CLASS_LIBRARY_RIJNDAEL_ALGORITHM
#define SECURED_CLASS_LIBRARY_RIJNDAEL_ALGORITHM
 
#include "../include/wintype.h"
#include <immintrin.h>
#ifdef __GNUC__
#include <cpuid.h>
#pragma GCC visibility push(hidden)
#else
#include <intrin.h>
#endif
#include <vector>


static inline bool IsAesNIset() {
	bool bRet = false;
#ifndef __GNUC__
	int cpuInfo[4] = { -1 };
	__cpuid(cpuInfo, 1);
	bRet = (cpuInfo[2] & 0x02000000) ? true : false;
#else
	unsigned int eax = 0, ebx = 0, ecx = 0, edx = 0;
	__get_cpuid(1, &eax, &ebx, &ecx, &edx);
	bRet =  (ecx & 0x02000000) ? true : false;
#endif
return bRet;
}

typedef unsigned int	u32bit;
typedef unsigned char	u8bit;
typedef unsigned short	u16bit;
typedef unsigned long long	u64bit;
typedef	int	s32bit;

#ifdef __GNUC__
#pragma GCC push_options
#pragma GCC target("sse2,aes")
#endif

 static inline __m128i KeyExpantion128(__m128i Key, __m128i tmp) {
	__m128i s = _mm_xor_si128(Key, _mm_slli_si128(Key, 4));
	tmp = _mm_shuffle_epi32(tmp, _MM_SHUFFLE(3, 3, 3, 3));
	s = _mm_xor_si128(s, _mm_slli_si128(s, 4));
	s = _mm_xor_si128(s, _mm_slli_si128(s, 4));
	return _mm_xor_si128(tmp, s);
}
static inline __m128i KeyExpantion256(__m128i Key, __m128i tmp) {
	__m128i s = _mm_xor_si128(Key, _mm_slli_si128(Key, 4));
	tmp = _mm_shuffle_epi32(tmp, _MM_SHUFFLE(2, 2, 2, 2));
	s = _mm_xor_si128(s, _mm_slli_si128(s, 4));
	s = _mm_xor_si128(s, _mm_slli_si128(s, 4));
	return _mm_xor_si128(tmp, s);
}


inline void aesExpand128(void* Key, std::vector<__m128i> & EK128) {
	EK128[0] = _mm_loadu_si128((__m128i*)Key);
	EK128[1] = KeyExpantion128(EK128[0], _mm_aeskeygenassist_si128(EK128[0], 0x01));
	EK128[2] = KeyExpantion128(EK128[1], _mm_aeskeygenassist_si128(EK128[1], 0x02));
	EK128[3] = KeyExpantion128(EK128[2], _mm_aeskeygenassist_si128(EK128[2], 0x04));
	EK128[4] = KeyExpantion128(EK128[3], _mm_aeskeygenassist_si128(EK128[3], 0x08));
	EK128[5] = KeyExpantion128(EK128[4], _mm_aeskeygenassist_si128(EK128[4], 0x010));
	EK128[6] = KeyExpantion128(EK128[5], _mm_aeskeygenassist_si128(EK128[5], 0x20));
	EK128[7] = KeyExpantion128(EK128[6], _mm_aeskeygenassist_si128(EK128[6], 0x40));
	EK128[8] = KeyExpantion128(EK128[7], _mm_aeskeygenassist_si128(EK128[7], 0x80));
	EK128[9] = KeyExpantion128(EK128[8], _mm_aeskeygenassist_si128(EK128[8], 0x1B));
	EK128[10] = KeyExpantion128(EK128[9], _mm_aeskeygenassist_si128(EK128[9], 0x36));
	EK128[11] = _mm_aesimc_si128(EK128[9]);
	EK128[12] = _mm_aesimc_si128(EK128[8]);
	EK128[13] = _mm_aesimc_si128(EK128[7]);
	EK128[14] = _mm_aesimc_si128(EK128[6]);
	EK128[15] = _mm_aesimc_si128(EK128[5]);
	EK128[16] = _mm_aesimc_si128(EK128[4]);
	EK128[17] = _mm_aesimc_si128(EK128[3]);
	EK128[18] = _mm_aesimc_si128(EK128[2]);
	EK128[19] = _mm_aesimc_si128(EK128[1]);
};

inline void aesEncrypt128(void* inBlock, void* outBlock, std::vector<__m128i>& EK128) {
	__m128i in = _mm_loadu_si128((__m128i*) inBlock);
	in = _mm_xor_si128(in, EK128[0]);
	in = _mm_aesenc_si128(in, EK128[1]);
	in = _mm_aesenc_si128(in, EK128[2]);
	in = _mm_aesenc_si128(in, EK128[3]);
	in = _mm_aesenc_si128(in, EK128[4]);
	in = _mm_aesenc_si128(in, EK128[5]);
	in = _mm_aesenc_si128(in, EK128[6]);
	in = _mm_aesenc_si128(in, EK128[7]);
	in = _mm_aesenc_si128(in, EK128[8]);
	in = _mm_aesenc_si128(in, EK128[9]);
	in = _mm_aesenclast_si128(in, EK128[10]);
	_mm_storeu_si128((__m128i*) outBlock, in);
};
inline void aesDecrypt128(void* inBlock, void* outBlock, std::vector<__m128i>& EK128) {
	__m128i in = _mm_loadu_si128((__m128i*) inBlock);
	in = _mm_xor_si128(in, EK128[10]);
	in = _mm_aesdec_si128(in, EK128[11]);
	in = _mm_aesdec_si128(in, EK128[12]);
	in = _mm_aesdec_si128(in, EK128[13]);
	in = _mm_aesdec_si128(in, EK128[14]);
	in = _mm_aesdec_si128(in, EK128[15]);
	in = _mm_aesdec_si128(in, EK128[16]);
	in = _mm_aesdec_si128(in, EK128[17]);
	in = _mm_aesdec_si128(in, EK128[18]);
	in = _mm_aesdec_si128(in, EK128[19]);
	in = _mm_aesdeclast_si128(in, EK128[0]);
	_mm_storeu_si128((__m128i*)outBlock, in);
};

#ifdef __GNUC__
#pragma GCC pop_options
#endif

class RIJNDAEL 
{
public:
	RIJNDAEL() { 
		bIsAES_SET = IsAesNIset();
		if (bIsAES_SET) {
			EK128.reserve(20);
			__m128i t = { 0L, 0L };
			for (int i = 0; i < 20; i++) EK128.push_back(t);
		};
		Clear();
	};
	~RIJNDAEL() { Clear(); };
public:
	inline void Decrypt(void* inBlock, void* outBlock) {
#if !defined _M_IX86
		if (Rounds == 10 && bIsAES_SET) { Decrypt128(inBlock, outBlock); return; }
#endif
		DecryptCommon(inBlock, outBlock);	
	};
	inline void Encrypt(void* inBlock, void* outBlock) {
#if !defined _M_IX86
		if (Rounds == 10 && bIsAES_SET) { 	Encrypt128(inBlock, outBlock); return;	}
#endif
		EncryptCommon(inBlock, outBlock); 
	};
	inline void Initialize(void* pKey, int iKeyLength) {
		Clear();
#if !defined _M_IX86
		if (iKeyLength == 16 && bIsAES_SET) {
			Expand128(pKey);
			Rounds = 10;
			return;
	}
#endif // !!defined _M_IX86
		InitializeCommon(pKey, iKeyLength);
	};

protected:
	void InitializeCommon(void* pkey, int length);
	inline void Expand128(void* Key) {
		aesExpand128(Key, EK128);
	};
	inline void Encrypt128(void* inBlock, void* outBlock) {
		aesEncrypt128(inBlock, outBlock, EK128);
	};
	inline void Decrypt128(void* inBlock, void* outBlock) {
		aesDecrypt128(inBlock, outBlock, EK128);
	};
	void DecryptCommon(void* inBlock, void* outBlock);
	void EncryptCommon(void* inBlock, void* outBlock);


	inline void	 Clear() {
		Rounds = 0;
		ZeroMemory(EK, 52 * sizeof(u32bit));
		ZeroMemory(DK, 52 * sizeof(u32bit));
		ZeroMemory(ME, 32);
		ZeroMemory(MD, 32);
//#if !defined _M_IX86
		if (bIsAES_SET) {
			for (auto&& t : EK128) t = __m128i {0L,0L };
		}

//#endif
		//		ZeroMemory((_UINT8*)EK128, sizeof(EK128));
	};
	//__m128i EK128[20]; // need only 128 bit kewy
	std::vector<__m128i> EK128;

	static u32bit S(u32bit);
    static const u8bit  SE[256], SD[256];
    static const u32bit TE0[256], TE1[256], TE2[256], TE3[256],
                            TD0[256], TD1[256], TD2[256], TD3[256];
	unsigned int Rounds;
	u32bit	EK[52],
            DK[52];
    u32bit	ME[32],
            MD[32];
	bool bIsAES_SET = false;
	
};


/*************************************************
* Rotation Functions                             *
*************************************************/
template<typename T> inline T rotate_left(T input, u32bit rot)
{
	return (T)((input << rot) | (input >> (8 * sizeof(T) - rot)));
}

template<typename T> inline T rotate_right(T input, u32bit rot)
{
	return (T)((input >> rot) | (input << (8 * sizeof(T) - rot)));
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif


#endif // !defined(SECURED_CLASS_LIBRARY_RIJNDAEL_ALGORITHM)
