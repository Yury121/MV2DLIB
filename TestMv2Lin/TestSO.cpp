#include <cstdio>
#include <memory>
#include <string.h>
#include <iostream>
#include <random>
#include <ctime>
#include <fstream>
#include <thread>

#include "../include/mv2lin.h"

double timedif(timespec& start, timespec& end) {
	double dt1 = 1.0 * double(start.tv_sec) + 0.000000001 * double(start.tv_nsec);
	double dt2 = 1.0 * double(end.tv_sec) + 0.000000001 * double(end.tv_nsec);
	return (dt2 - dt1);	   //sec
}//end diff

#define MEGABYTE 2097152

int RestDecryptExisting(std::string dir = "");
int RestDecryptExisting(std::string dir) {
	int Count = 0;
	std::fstream ifs_key;
	std::fstream ifs_crypt;
	std::string fileKey = dir +  "./keys.bin";
	std::string fileCrypt = dir + "/crypt.bin";
	ifs_key.open(fileKey, std::ios::in | std::ios_base::binary);
	ifs_crypt.open(fileCrypt, std::ios::in | std::ios_base::binary);
	if (!ifs_key.is_open() || !ifs_crypt.is_open()) {
		if (ifs_crypt.is_open()) ifs_crypt.close();
		if (ifs_key.is_open()) ifs_key.close();
		return Count;
	}
	MV2_BUFFER mv2Buf;
	uint8_t hKey[256];
	//	long lRest[] = { 64, 128, 256, 512,1024 };
	//	long lCore = 0;
	bool bRes = false;
	uint32_t szCore, szFlags;
	if (MV2CalcRecomendedSizes(2 * MEGABYTE, 1024, szCore, szFlags)) {
		std::cout << "Recomended core's size (bytes) :" << szCore << "; flag's size: " << szFlags << ";\n";
		//cout << "Used core's size (bytes) :" << 1024 << "; flag's size: " << 2 * MEGABYTE + MEGABYTE / 512 << ";\n";
	};
	memset(&mv2Buf, 0, sizeof(MV2_BUFFER));
	std::unique_ptr<uint8_t[]> ptrCore(new uint8_t[1024]);
	szFlags = ( szFlags >  2 * MEGABYTE + MEGABYTE / 512) ? szFlags : 2 * MEGABYTE + MEGABYTE / 512;
	std::unique_ptr<uint8_t[]> ptrFlags(new uint8_t[szFlags]);
	std::unique_ptr<uint8_t[]> ptrSRC(new uint8_t[2 * MEGABYTE]);
	std::unique_ptr<uint8_t[]> ptrMem(new uint8_t[2 * MEGABYTE]);

	mv2Buf.CoreBuf = ptrCore.get();// (BYTE*) new BYTE[1024];
	if (mv2Buf.CoreBuf) mv2Buf.szCoreBuf = 1024;
	mv2Buf.FlagBuf = ptrFlags.get();// (BYTE*) new BYTE[2 * MEGABYTE + MEGABYTE / 512];
	if (mv2Buf.FlagBuf) mv2Buf.szFlagBuf = szFlags;// 2 * MEGABYTE + MEGABYTE / 512;
	mv2Buf.SrcBuf = ptrSRC.get();// (BYTE*) new BYTE[2 * MEGABYTE];
	if (mv2Buf.SrcBuf) {
		mv2Buf.szSrcBuf = 2 * MEGABYTE;
	}
	memset(mv2Buf.SrcBuf, 0, mv2Buf.szSrcBuf);
	uint8_t* hMem = ptrMem.get();// (BYTE*) new BYTE[2 * MEGABYTE];
	int i = 0, j = 0;
	int k = 31;
	for (j = 16; j <= 256; j = j * 2) {
		k = k >> 1;
		std::cout << "start MV2-Decrypt from file " << j * 8 << " tests:\r\n";
		if (ifs_key.is_open())
			ifs_key.read((char*)hKey, 256);
		for (i = 0; i < 100; i++) {
			bRes = MV2SetKey(&hKey[j * (i & k)], j);
			if (!bRes) { std::cout << "\r\nSET KEY" << j * 8 << " RETURN ERROR: " << MV2GetStatus() << ";\n"; break; }
			ifs_crypt.read((char*)&mv2Buf.szCoreBuf, sizeof(int));
			ifs_crypt.read((char*)mv2Buf.CoreBuf, mv2Buf.szCoreBuf);
			ifs_crypt.read((char*)&mv2Buf.szFlagBuf, sizeof(int));
			ifs_crypt.read((char*)mv2Buf.FlagBuf, mv2Buf.szFlagBuf);
			std::cout << " * " << mv2Buf.szCoreBuf << " / " << mv2Buf.szFlagBuf << " ";
			bRes = MV2Decrypt(mv2Buf);
			if (!bRes) { std::cout << "\r\nDECRYPTION RETURN ERROR!!! CODE: " << MV2GetStatus() << " Steps: " << MV2GetStep() << "\r\n"; }
			else { Count++; }
			if (i == 0) {
				memcpy(ptrMem.get(), mv2Buf.SrcBuf, mv2Buf.szSrcBuf);
			}
			else {
				if (memcmp(ptrMem.get(), mv2Buf.SrcBuf, mv2Buf.szSrcBuf) != 0) {
					std::cout << "\r\nERROR!! decrypted and source are not equal\r\n";
					break;
				}
			}
			std::cout.flush();
		};
		std::cout << "\nKey size = " << j * 8 << " b, successfully decrypted" << Count << " tuples\n";

	}
	if (ifs_crypt.is_open()) ifs_crypt.close();
	if (ifs_key.is_open()) ifs_key.close();

	return Count;
}

int TestMv2(std::string dir = "");
int TestMv2(std::string dir ) {
	std::fstream ifs_key;
	std::fstream ifs_crypt;
	timespec tstart, tstop;
	if (!dir.empty()) {
		//check file exist 
		ifs_key.open("keys.bin", std::ios::out | std::ios::in | std::ios_base::binary);
		ifs_crypt.open("crypt.bin", std::ios::out | std::ios::in | std::ios_base::binary);
		if (!ifs_key.is_open() || !ifs_crypt.is_open()) {
			if (ifs_crypt.is_open()) ifs_crypt.close();
			if (ifs_key.is_open()) ifs_key.close();
			ifs_key.open("keys.bin", std::ios::out | std::ios_base::binary);
			ifs_crypt.open("crypt.bin", std::ios::out | std::ios_base::binary);
		}
		else {
			//file exist
			if (ifs_crypt.is_open()) ifs_crypt.close();
			if (ifs_key.is_open()) ifs_key.close();
		}
		if (!ifs_key.is_open() || !ifs_crypt.is_open()) {
			std::cout << "can't write output results\n";
			if (ifs_crypt.is_open()) ifs_crypt.close();
			if (ifs_key.is_open()) ifs_key.close();
		}
	}
	double dTime, dKeyTime, dDecTime;
	int i, j;

	//std::unique_ptr<uint8_t[]> hMem{ std::make_unique<uint8_t[]>(MEGABYTE) };
	uint8_t* hMem = nullptr;
	//rnd.InitRand654();
	uint32_t* pDword = nullptr;
	float fAvrCore, fAvrFlags;

	MV2_BUFFER mv2Buf;
	int nRetCode = 0;
	//	char * Key = _T("1234567890123456789012345678901234567890123456789012345678901234567890");
	int  iKey[64] = {};
	uint8_t* hKey = (uint8_t*)iKey;// [256] ;
	long lRest[] = { 64, 128, 256, 512,1024, 2048 };
	long lCore = 0;

	timespec_get(&tstart, TIME_UTC);
	std::srand(int(tstart.tv_nsec));

	bool bRes = false;
	for (i = 0; i < 64; i++) { iKey[i] = std::rand(); }
	memset(&mv2Buf, 0, sizeof(MV2_BUFFER));
	std::unique_ptr<uint8_t[]> ptrCore(new uint8_t[1024]);
	std::unique_ptr<uint8_t[]> ptrFlags(new uint8_t[2 * MEGABYTE + MEGABYTE / 512]);
	std::unique_ptr<uint8_t[]> ptrSRC(new uint8_t[2 * MEGABYTE]);
	std::unique_ptr<uint8_t[]> ptrMem(new uint8_t[2 * MEGABYTE]);

	mv2Buf.CoreBuf = ptrCore.get();// (BYTE*) new BYTE[1024];
	if (mv2Buf.CoreBuf) mv2Buf.szCoreBuf = 1024;
	mv2Buf.FlagBuf = ptrFlags.get();// (BYTE*) new BYTE[2 * MEGABYTE + MEGABYTE / 512];
	if (mv2Buf.FlagBuf) mv2Buf.szFlagBuf = 2 * MEGABYTE + MEGABYTE / 512;
	mv2Buf.SrcBuf = ptrSRC.get();// (BYTE*) new BYTE[2 * MEGABYTE];
	if (mv2Buf.SrcBuf) {
		mv2Buf.szSrcBuf = 2 * MEGABYTE;
	}
	memset(mv2Buf.SrcBuf, 0, mv2Buf.szSrcBuf);
	hMem = ptrMem.get();// (BYTE*) new BYTE[2 * MEGABYTE];

	for (j = 16; j <= 256; j *= 2) {
		std::cout << "\r\nFilling memory Id:" << std::this_thread::get_id() << "\r\n";
		pDword = (uint32_t*)mv2Buf.SrcBuf;

		for (i = 0; i < MEGABYTE / 2; i++) { pDword[i] = std::rand(); }
		std::cout << "start MV2-" << j * 8 << " tests: " << std::this_thread::get_id() << "\r\n";
		dTime = dKeyTime = dDecTime = 0;

		for (i = 0; i < 100; i++) {
			std::cout.flush();
			lCore = lRest[(j + i) % 5];
			timespec_get(&tstart, TIME_UTC);
			bRes = MV2SetKey(hKey, j);
			timespec_get(&tstop, TIME_UTC);
			if (!bRes) {
				std::cout << "\r\nSET KEY" << j * 8 << " RETURN ERROR!!! Id:" << std::this_thread::get_id() << "\r\n";
				break;
			}
			dKeyTime += timedif(tstart, tstop);

			std::cout << "*";
			// encryption
			mv2Buf.szSrcBuf = 2 * MEGABYTE;
			mv2Buf.szCoreBuf = 1024;
			mv2Buf.szFlagBuf = 2 * MEGABYTE + MEGABYTE / 512;
			timespec_get(&tstart, TIME_UTC);
			bRes = MV2Encrypt(mv2Buf, 1024);
			//bRes = MV2Encrypt(mv2Buf,lCore);
			timespec_get(&tstop, TIME_UTC);
			std::cout <<"." << std::this_thread::get_id() << ". "  << mv2Buf.szCoreBuf << "/" << MV2GetStep() << "  ";
			dTime += timedif(tstart, tstop);
			if (!bRes) {
				std::cout << "\r\nENCRYPTION RETURN ERROR!!! CODE: " << MV2GetStatus() << " Steps: " << MV2GetStep() << "\r\n";
				break;
			}
			// decryption
			memcpy(hMem, mv2Buf.SrcBuf, 2 * MEGABYTE);
			memset(mv2Buf.SrcBuf, 1, mv2Buf.szSrcBuf);
			mv2Buf.szSrcBuf = 2 * MEGABYTE;
			timespec_get(&tstart, TIME_UTC);
			bRes = MV2Decrypt(mv2Buf);
			timespec_get(&tstop, TIME_UTC);
			if (!bRes) {
				std::cout << "\r\nDECRYPTION RETURN ERROR!!! CODE: " << MV2GetStatus() << " Steps: " << MV2GetStep() << "\r\n";
				break;
			}
			dDecTime += timedif(tstart, tstop);
			if (memcmp(hMem, mv2Buf.SrcBuf, 2 * MEGABYTE) == 0)		std::cout << "!";
			else {
				std::cout << "\r\nERROR!! decrypted and source are not equal\r\n";
				break;
			}
		}//end loop for key
		std::cout << "\r\n" << i << " tests for 4 MB data was executed (" << std::this_thread::get_id() << ")\r\n";
		std::cout << "key set time is: " << dKeyTime << " sec; ";
		if (dKeyTime != 0.0) std::cout << "speed is " << 1.0 * i / dKeyTime << " keys/sec;";
		std::cout << "\r\nencryption time is " << dTime << " sec; Speed is " << 4.0 * i / dTime << " MB/sec; \r\n";
		std::cout << "decryption time is " << dDecTime << " sec; Speed is " << 4.0 * i / dDecTime << " MB/sec; \r\n";

		memcpy(hKey, mv2Buf.SrcBuf, 256);
	}//end loop for keylength
	MV2Release();
	if (ifs_crypt.is_open()) ifs_crypt.close();
	if (ifs_key.is_open()) ifs_key.close();
	return 0;
  }//end TestMv2

  int test() {  return TestMv2();  	  }

int main(int argc, char* argv[])
{
    printf("hello from %s!\n", "TestMv2Lin");

	std::string dir = "";
	if (argc > 1) dir = argv[1];


	std::string str = (IsAESProc()) ? " AES supported\n" : " AES not supported\n";
	std::cout << "Test MV2!" << str;
	printf("hello from %s!\n", "mv2lib");
	//dir = "/mnt/d/Worker/MV2/Bin/x64/Release/";
	//RestDecryptExisting(dir);
	std::thread f1(test);
	std::thread f2(test);

	f1.join();
	f2.join();
#if 0
	std::fstream ifs_key;
	std::fstream ifs_crypt;
	//check file exist 
	ifs_key.open("keys.bin", std::ios::out | std::ios::in | std::ios_base::binary);
	ifs_crypt.open("crypt.bin", std::ios::out | std::ios::in | std::ios_base::binary);
	if (!ifs_key.is_open() || !ifs_crypt.is_open()) {
		if (ifs_crypt.is_open()) ifs_crypt.close();
		if (ifs_key.is_open()) ifs_key.close();
		ifs_key.open("keys.bin", std::ios::out | std::ios_base::binary);
		ifs_crypt.open("crypt.bin", std::ios::out | std::ios_base::binary);
	}
	else {
		//file exist
		if (ifs_crypt.is_open()) ifs_crypt.close();
		if (ifs_key.is_open()) ifs_key.close();
	}
	if (!ifs_key.is_open() || !ifs_crypt.is_open()) {
		std::cout << "can't write output results\n";
		if (ifs_crypt.is_open()) ifs_crypt.close();
		if (ifs_key.is_open()) ifs_key.close();
	}


	timespec tstart, tstop;

	std::string str = (IsAESProc()) ? " AES supported\n" : " AES not supported\n";
	std::cout << "Test MV2!" << str;
	printf("hello from %s!\n", "mv2lib");

	double dTime, dKeyTime, dDecTime;
	int i, j;

	//std::unique_ptr<uint8_t[]> hMem{ std::make_unique<uint8_t[]>(MEGABYTE) };
	uint8_t* hMem = nullptr;
	//rnd.InitRand654();
	uint32_t* pDword = nullptr;
	float fAvrCore, fAvrFlags;

	MV2_BUFFER mv2Buf;
	int nRetCode = 0;
	//	char * Key = _T("1234567890123456789012345678901234567890123456789012345678901234567890");
	int  iKey[64] = {};
	uint8_t* hKey = (uint8_t*)iKey;// [256] ;
	long lRest[] = { 64, 128, 256, 512,1024, 2048 };
	long lCore = 0;

	timespec_get(&tstart, TIME_UTC);
	std::srand(int(tstart.tv_nsec));

	bool bRes = false;
	for (i = 0; i < 64; i++) { iKey[i] = std::rand(); }
	memset(&mv2Buf, 0, sizeof(MV2_BUFFER));
	std::unique_ptr<uint8_t[]> ptrCore(new uint8_t[1024]);
	std::unique_ptr<uint8_t[]> ptrFlags(new uint8_t[2 * MEGABYTE + MEGABYTE / 512]);
	std::unique_ptr<uint8_t[]> ptrSRC(new uint8_t[2 * MEGABYTE]);
	std::unique_ptr<uint8_t[]> ptrMem(new uint8_t[2 * MEGABYTE]);

	mv2Buf.CoreBuf = ptrCore.get();// (BYTE*) new BYTE[1024];
	if (mv2Buf.CoreBuf) mv2Buf.szCoreBuf = 1024;
	mv2Buf.FlagBuf = ptrFlags.get();// (BYTE*) new BYTE[2 * MEGABYTE + MEGABYTE / 512];
	if (mv2Buf.FlagBuf) mv2Buf.szFlagBuf = 2 * MEGABYTE + MEGABYTE / 512;
	mv2Buf.SrcBuf = ptrSRC.get();// (BYTE*) new BYTE[2 * MEGABYTE];
	if (mv2Buf.SrcBuf) {
		mv2Buf.szSrcBuf = 2 * MEGABYTE;
	}
	memset(mv2Buf.SrcBuf, 0, mv2Buf.szSrcBuf);
	hMem = ptrMem.get();// (BYTE*) new BYTE[2 * MEGABYTE];

	std::cout << "\r\nFilling memory\r\n";
	pDword = (uint32_t*)mv2Buf.SrcBuf;
	for (i = 0; i < MEGABYTE / 2; i++) {
		pDword[i] = std::rand();
	}
	for (j = 16; j <= 256; j *=  2) {
		std::cout << "start MV2-" << j * 8 << " tests:\r\n";
		dTime = dKeyTime = dDecTime = 0;

		for (i = 0; i < 100; i++) {
			std::cout.flush();
			lCore = lRest[(j + i) % 5];
			timespec_get(&tstart, TIME_UTC);
			bRes = MV2SetKey(hKey, j);
			timespec_get(&tstop, TIME_UTC);
			if (!bRes) {
				std::cout << "\r\nSET KEY" << j * 8 << " RETURN ERROR!!!\r\n";
				break;
			}
			dKeyTime += timedif(tstart, tstop);
			
			std::cout << "*";
			// encryption
			mv2Buf.szSrcBuf = 2 * MEGABYTE;
			mv2Buf.szCoreBuf = 1024;
			mv2Buf.szFlagBuf = 2 * MEGABYTE + MEGABYTE / 512;
			timespec_get(&tstart, TIME_UTC);
			bRes = MV2Encrypt(mv2Buf, 1024);
			//bRes = MV2Encrypt(mv2Buf,lCore);
			timespec_get(&tstop, TIME_UTC);
			std::cout << ". " << mv2Buf.szCoreBuf << "/" << MV2GetStep() << "  ";
			dTime += timedif(tstart, tstop);
			if (!bRes) {
				std::cout << "\r\nENCRYPTION RETURN ERROR!!! CODE: " << MV2GetStatus() << " Steps: " << MV2GetStep() << "\r\n";
				break;
			}
			// decryption
			memcpy(hMem, mv2Buf.SrcBuf, 2 * MEGABYTE);
			memset(mv2Buf.SrcBuf, 1, mv2Buf.szSrcBuf);
			mv2Buf.szSrcBuf = 2 * MEGABYTE;
			timespec_get(&tstart, TIME_UTC);
			bRes = MV2Decrypt(mv2Buf);
			timespec_get(&tstop, TIME_UTC);
			if (!bRes) {
				std::cout << "\r\nDECRYPTION RETURN ERROR!!! CODE: " << MV2GetStatus() << " Steps: " << MV2GetStep() << "\r\n";
				break;
			}
			dDecTime += timedif(tstart, tstop);
			if (memcmp(hMem, mv2Buf.SrcBuf, 2 * MEGABYTE) == 0)		std::cout << "!";
			else {
				std::cout << "\r\nERROR!! decrypted and source are not equal\r\n";
				break;
			}
		}//end loop for key
		std::cout << "\r\n" << i << " tests for 1 MB data was executed\r\n";
		std::cout << "key set time is: " << dKeyTime << " sec; ";
		if (dKeyTime != 0.0) std::cout << "speed is " << 1.0 * i / dKeyTime << " keys/sec;";
		std::cout << "\r\nencryption time is " << dTime << " sec; Speed is " << 4.0 * i / dTime << " MB/sec; \r\n";
		std::cout << "decryption time is " << dDecTime << " sec; Speed is " << 4.0 * i / dDecTime << " MB/sec; \r\n";

		std::cout << "\r\nFilling memory\r\n";
		for (i = 0; i < MEGABYTE / 4; i++) {
			pDword[i] = std::rand();
		}
		memcpy(hKey, hMem, 256);
	}//end loop for keylength

	if (ifs_crypt.is_open()) ifs_crypt.close();
	if (ifs_key.is_open()) ifs_key.close();
#endif


    return 0;
}