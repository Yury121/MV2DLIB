#pragma once
#include <intrin.h>
#include <wmmintrin.h>
#ifdef WINDOWS
#include <minwinbase.h>
#endif // WIN32
#include <string.h>

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



 //static int const rcon[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36, 0x6C, 0xD8, 0xAB, 0x4D, 0x9A };
class CAES128NI {
public:
	enum AESLENGTH { AES128 = 16, /*AES192 = 24,*/ AES256 = 32 };
	CAES128NI() { Clear(); };
	~CAES128NI() { Clear(); };
	inline void Clear() {
#ifdef WINDOWS
		ZeroMemory((uint8_t*)EK, sizeof(EK));
#else
		memset((uint8_t*)EK, 0, sizeof(EK));
	};
#endif // WINDOWS
	inline void Expand128(void* Key) {
		EK[0] = _mm_loadu_si128((const __m128i*)Key);
		EK[1] = KeyExpantion128(EK[0], _mm_aeskeygenassist_si128(EK[0], 0x01));
		EK[2] = KeyExpantion128(EK[1], _mm_aeskeygenassist_si128(EK[1], 0x02));
		EK[3] = KeyExpantion128(EK[2], _mm_aeskeygenassist_si128(EK[2], 0x04));
		EK[4] = KeyExpantion128(EK[3], _mm_aeskeygenassist_si128(EK[3], 0x08));
		EK[5] = KeyExpantion128(EK[4], _mm_aeskeygenassist_si128(EK[4], 0x010));
		EK[6] = KeyExpantion128(EK[5], _mm_aeskeygenassist_si128(EK[5], 0x20));
		EK[7] = KeyExpantion128(EK[6], _mm_aeskeygenassist_si128(EK[6], 0x40));
		EK[8] = KeyExpantion128(EK[7], _mm_aeskeygenassist_si128(EK[7], 0x80));
		EK[9] = KeyExpantion128(EK[8], _mm_aeskeygenassist_si128(EK[8], 0x1B));
		EK[10] = KeyExpantion128(EK[9], _mm_aeskeygenassist_si128(EK[9], 0x36));
		EK[11] = _mm_aesimc_si128(EK[9]);
		EK[12] = _mm_aesimc_si128(EK[8]);
		EK[13] = _mm_aesimc_si128(EK[7]);
		EK[14] = _mm_aesimc_si128(EK[6]);
		EK[15] = _mm_aesimc_si128(EK[5]);
		EK[16] = _mm_aesimc_si128(EK[4]);
		EK[17] = _mm_aesimc_si128(EK[3]);
		EK[18] = _mm_aesimc_si128(EK[2]);
		EK[19] = _mm_aesimc_si128(EK[1]);
	};
	inline void Expand256(void* Key) {
		EK[0] = _mm_loadu_si128((const __m128i*)Key);
		EK[1] = _mm_loadu_si128((const __m128i*) ((uint8_t*) Key + 16) );
		EK[2] = KeyExpantion128(EK[0], _mm_aeskeygenassist_si128(EK[1], 0x01));
		EK[3] = KeyExpantion256(EK[1], _mm_aeskeygenassist_si128(EK[2], 0x01));
		EK[4] = KeyExpantion128(EK[2], _mm_aeskeygenassist_si128(EK[3], 0x02));
		EK[5] = KeyExpantion256(EK[3], _mm_aeskeygenassist_si128(EK[4], 0x02));
   		EK[6] = KeyExpantion128(EK[4], _mm_aeskeygenassist_si128(EK[5], 0x04));
		EK[7] = KeyExpantion256(EK[5], _mm_aeskeygenassist_si128(EK[6], 0x04));
		EK[8] = KeyExpantion128(EK[6], _mm_aeskeygenassist_si128(EK[7], 0x08));
		EK[9] = KeyExpantion256(EK[7], _mm_aeskeygenassist_si128(EK[8], 0x08));
		EK[10] = KeyExpantion128(EK[8], _mm_aeskeygenassist_si128(EK[9], 0x10));
		EK[11] = KeyExpantion256(EK[9], _mm_aeskeygenassist_si128(EK[10], 0x10));
		EK[12] = KeyExpantion128(EK[10], _mm_aeskeygenassist_si128(EK[11], 0x20));
		EK[13] = KeyExpantion256(EK[11], _mm_aeskeygenassist_si128(EK[12], 0x20));
		EK[14] = KeyExpantion128(EK[12], _mm_aeskeygenassist_si128(EK[13], 0x40));
	
		EK[15] = _mm_aesimc_si128(EK[13]);
		EK[16] = _mm_aesimc_si128(EK[12]);
		EK[17] = _mm_aesimc_si128(EK[11]);
		EK[18] = _mm_aesimc_si128(EK[10]);
		EK[19] = _mm_aesimc_si128(EK[9]);
		EK[20] = _mm_aesimc_si128(EK[8]);
		EK[21] = _mm_aesimc_si128(EK[7]);
		EK[22] = _mm_aesimc_si128(EK[6]);
		EK[23] = _mm_aesimc_si128(EK[5]);
		EK[24] = _mm_aesimc_si128(EK[4]);
		EK[25] = _mm_aesimc_si128(EK[3]);
		EK[26] = _mm_aesimc_si128(EK[2]);
		EK[27] = _mm_aesimc_si128(EK[1]);
	};
	inline void Initilise(void* key, int iKeyLength) {
		if (iKeyLength == AES128) {
			Expand128(key);  nRaunds = AES128;
		}
		else {
			Expand256(key);  nRaunds = AES256;
		}
	};
	void Encrypt(void* inBlock, void* outBlock) {
		__m128i in = _mm_loadu_si128((__m128i*) inBlock);
		in = _mm_xor_si128(in, EK[0]);
		in = _mm_aesenc_si128(in, EK[1]);
		in = _mm_aesenc_si128(in, EK[2]);
		in = _mm_aesenc_si128(in, EK[3]);
		in = _mm_aesenc_si128(in, EK[4]);
		in = _mm_aesenc_si128(in, EK[5]);
		in = _mm_aesenc_si128(in, EK[6]);
		in = _mm_aesenc_si128(in, EK[7]);
		in = _mm_aesenc_si128(in, EK[8]);
		in = _mm_aesenc_si128(in, EK[9]);
		if (nRaunds == AES128) { 
			in = _mm_aesenclast_si128(in, EK[10]); 
		}
		else {
			in = _mm_aesenc_si128(in, EK[10]);
			in = _mm_aesenc_si128(in, EK[11]);
			in = _mm_aesenc_si128(in, EK[12]);
			in = _mm_aesenc_si128(in, EK[13]);
			in = _mm_aesenclast_si128(in, EK[14]);
		}
		_mm_storeu_si128((__m128i*)outBlock, in);
	};
	void Decrypt(void* inBlock, void* outBlock) {
		__m128i in = _mm_loadu_si128((__m128i*) inBlock);
		if (nRaunds == AES256) {
			in = _mm_xor_si128(in, EK[14]);
			in = _mm_aesdec_si128(in, EK[15]);
			in = _mm_aesdec_si128(in, EK[16]);
			in = _mm_aesdec_si128(in, EK[17]);
			in = _mm_aesdec_si128(in, EK[18]);
			in = _mm_aesdec_si128(in, EK[19]);
			in = _mm_aesdec_si128(in, EK[20]);
			in = _mm_aesdec_si128(in, EK[21]);
			in = _mm_aesdec_si128(in, EK[22]);
			in = _mm_aesdec_si128(in, EK[23]);
			in = _mm_aesdec_si128(in, EK[24]);
			in = _mm_aesdec_si128(in, EK[25]);
			in = _mm_aesdec_si128(in, EK[26]);
			in = _mm_aesdec_si128(in, EK[27]);
		}
		else {
			in = _mm_xor_si128(in, EK[10]);
			in = _mm_aesdec_si128(in, EK[11]);
			in = _mm_aesdec_si128(in, EK[12]);
			in = _mm_aesdec_si128(in, EK[13]);
			in = _mm_aesdec_si128(in, EK[14]);
			in = _mm_aesdec_si128(in, EK[15]);
			in = _mm_aesdec_si128(in, EK[16]);
			in = _mm_aesdec_si128(in, EK[17]);
			in = _mm_aesdec_si128(in, EK[18]);
			in = _mm_aesdec_si128(in, EK[19]);
		}
		in = _mm_aesdeclast_si128(in, EK[0]);
		_mm_storeu_si128((__m128i*)outBlock, in);
	};
protected:
	__m128i EK[28];
	int nRaunds = AES256;
};





void AesEncrypt(uint8_t* in, uint8_t out) {

}