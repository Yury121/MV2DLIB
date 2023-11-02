// TestDll.cpp : Defines the entry point for the console application.
//


#ifdef _DEBUG
//#define new DEBUG_NEW
#endif

#include <iostream>
#include <Windows.h>
#include "../include/mv2dll.h"
#include <math.h>
#include <memory>
#include <fstream>

#include <intrin.h>
#include <thread>


class timer
{
protected:
	LARGE_INTEGER	frequency;
	LARGE_INTEGER	counterStart;
	LARGE_INTEGER	counterEnd;
public:
	timer() { QueryPerformanceFrequency(&frequency); }
	void start() { QueryPerformanceCounter(&counterStart); }
	void stop() { QueryPerformanceCounter(&counterEnd); }
	double seconds() { return (double)((counterEnd.QuadPart - counterStart.QuadPart) / (double)frequency.QuadPart); }
	double microseconds() { return 1000000.0 * (double)((counterEnd.QuadPart - counterStart.QuadPart) / (double)frequency.QuadPart); }
	double miliseconds() { return 1000.0 * (double)((counterEnd.QuadPart - counterStart.QuadPart) / (double)frequency.QuadPart); }
};





using namespace std;
//#define MEGABYTE 1048576
#define MEGABYTE 2097152
//#define MEGABYTE 524288
//#define MEGABYTE 1024

class CRand654 {
public:
	CRand654() {
		srand((unsigned)time(NULL));
		T1 = rand() * GetTickCount();
		T2 = rand(); T2 = T2 << 15; T2 += rand(); T2 *= rand();
		T3 = GetTickCount() * rand(); T3 = T3 << (rand() % 13); T3 = T3 * rand();
	}
	void InitRand654() {
		srand((unsigned)time(NULL));
		T1 = rand() * GetTickCount();
		T2 = rand(); T2 = T2 << 15; T2 += rand(); T2 *= rand();
		T3 = GetTickCount() * rand(); T3 = T3 << (rand() % 13); T3 = T3 * rand();
	}
	DWORD GetValue() {
		ULONGLONG L1, L2, L3, L4;
		if ((T1 == 0) || (T2 == 0) || (T3 == 0)) {
			srand((unsigned)time(NULL) + GetTickCount());
			if (T1 == 0)	T1 = (GetTickCount() + rand()) * rand() + rand();
			if (T2 == 0)	T2 = (GetTickCount() + rand()) * rand() + rand();
			if (T3 == 0)	T3 = (GetTickCount() + rand()) * rand() + rand();
		}
		L1 = T1; L2 = T2; L3 = T3;
		L4 = L1 + L2 + L3;
		L4 = Int64ShllMod32(L4, 13);
		T1 = L2;
		T2 = L3;
		L4 = L4 % ((DWORD)0xFFFFFFFB);//4294967291
		T3 = L4;
		return T3;
	}

private:
	DWORD T1;
	DWORD T2;
	DWORD T3;
};

void TestCore1024(bool bSave = false);


void TestCore1024(bool bSave) {
	timer t1;
	double dTime, dKeyTime, dDecTime;
	CRand654 rnd;
	int i, j;
	BYTE* hMem = NULL;
	rnd.InitRand654();
	DWORD* pDword = NULL;
	//float fAvrCore, fAvrFlags;
	std::fstream ifs_key;
	std::fstream ifs_crypt;
	if (bSave) {
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
	else {
		std::cout << "Not write output results\n";
	}
	MV2_BUFFER mv2Buf;
	BYTE hKey[256];
	//	long lRest[] = { 64, 128, 256, 512,1024 };
	//	long lCore = 0;
	bool bRes = false;
	unsigned int szCore, szFlags;
	if (MV2CalcRecomendedSizes(2 * MEGABYTE, 1024, &szCore, &szFlags)) {
		cout << "Recomended core's size (bytes) :" << szCore << "; flag's size: " << szFlags << ";\n";
		//cout << "Used core's size (bytes) :" << 1024 << "; flag's size: " << 2 * MEGABYTE + MEGABYTE / 512 << ";\n";
	};

	for (i = 0; i < 64; i++) { j = rnd.GetValue();	memcpy(hKey + 4 * i, &j, 4); }
	memset(&mv2Buf, 0, sizeof(MV2_BUFFER));
	std::unique_ptr<BYTE[]> ptrCore(new BYTE[1024]);
	szFlags = max(szFlags, 2 * MEGABYTE + MEGABYTE / 512);
	std::unique_ptr<BYTE[]> ptrFlags(new BYTE[szFlags]);
	std::unique_ptr<BYTE[]> ptrSRC(new BYTE[2 * MEGABYTE]);
	std::unique_ptr<BYTE[]> ptrMem(new BYTE[2 * MEGABYTE]);

	mv2Buf.CoreBuf = ptrCore.get();// (BYTE*) new BYTE[1024];
	if (mv2Buf.CoreBuf) mv2Buf.szCoreBuf = 1024;
	mv2Buf.FlagBuf = ptrFlags.get();// (BYTE*) new BYTE[2 * MEGABYTE + MEGABYTE / 512];
	if (mv2Buf.FlagBuf) mv2Buf.szFlagBuf = szFlags;// 2 * MEGABYTE + MEGABYTE / 512;
	mv2Buf.SrcBuf = ptrSRC.get();// (BYTE*) new BYTE[2 * MEGABYTE];
	if (mv2Buf.SrcBuf) {
		mv2Buf.szSrcBuf = 2 * MEGABYTE;
	}
	memset(mv2Buf.SrcBuf, 0, mv2Buf.szSrcBuf);
	hMem = ptrMem.get();// (BYTE*) new BYTE[2 * MEGABYTE];

	cout << "\r\nFilling memory\r\n";
	pDword = (DWORD*)mv2Buf.SrcBuf;
	for (i = 0; i < MEGABYTE / 2; i++) { pDword[i] = rnd.GetValue(); }
	int k = 31;
	for (j = 16; j <= 256; j = j * 2) {
		cout << "start MV2-" << j * 8 << " tests:\r\n";
		dTime = dKeyTime = dDecTime = 0;
		k = k >> 1;
		if (ifs_key.is_open()) {
			ifs_key.write((char*)hKey, 256);
			ifs_key.flush();
		}
		for (i = 0; i < 100; i++) {
			//	lCore = lRest[(j + i) % 5];
			t1.start();
			bRes = MV2SetKey(&hKey[j * (i & k)], j);
			t1.stop();
			if (!bRes) {
				cout << "\r\nSET KEY" << j * 8 << " RETURN ERROR!!!\r\n";
				break;
			}
			dKeyTime += t1.seconds();
			cout << "*";
			// encryption
			mv2Buf.szSrcBuf = 2 * MEGABYTE;
			mv2Buf.szCoreBuf = 1024;
			mv2Buf.szFlagBuf = szFlags;// 2 * MEGABYTE + MEGABYTE / 512;
			t1.start();
			bRes = MV2Encrypt(mv2Buf, 1024);
			//bRes = MV2Encrypt(mv2Buf,lCore);
			t1.stop();
			cout << ". " << mv2Buf.szCoreBuf << "/" << MV2GetStep() << "  ";
			dTime += t1.seconds();
			if (!bRes) {
				cout << "\r\nENCRYPTION RETURN ERROR!!! CODE: " << MV2GetStatus() << " Steps: " << MV2GetStep() << "\r\n";
				break;
			}
			if (ifs_crypt.is_open()) {
				ifs_crypt.write((char*)&mv2Buf.szCoreBuf, sizeof(int));
				ifs_crypt.write((char*)mv2Buf.CoreBuf, mv2Buf.szCoreBuf);
				ifs_crypt.write((char*)&mv2Buf.szFlagBuf, sizeof(int));
				ifs_crypt.write((char*)mv2Buf.FlagBuf, mv2Buf.szFlagBuf);
			}
			// decryption
			memcpy(hMem, mv2Buf.SrcBuf, 2 * MEGABYTE);
			memset(mv2Buf.SrcBuf, 1, mv2Buf.szSrcBuf);
			mv2Buf.szSrcBuf = 2 * MEGABYTE;
			t1.start();
			bRes = MV2Decrypt(mv2Buf);
			t1.stop();
			if (!bRes) {
				cout << "\r\nDECRYPTION RETURN ERROR!!! CODE: " << MV2GetStatus() << " Steps: " << MV2GetStep() << "\r\n";
				break;
			}
			dDecTime += t1.seconds();
			if (memcmp(hMem, mv2Buf.SrcBuf, 2 * MEGABYTE) == 0)		cout << "!" << std::this_thread::get_id() << "_ ";
			else {
				cout << "\r\nERROR!! decrypted and source files are not equal\r\n";
				break;
			}
		}//end loop for key
		cout << "\r\n" << i << " tests for 4 MB data was executed ("<< std::this_thread::get_id() <<")\r\n";
		cout << "key set time is: " << dKeyTime << " sec; ";
		if (dKeyTime != 0.0) cout << "speed is " << 1.0 * i / dKeyTime << " keys/sec;";
		cout << "\r\nencryption time is " << dTime << " sec; Speed is " << 4.0 * i / dTime << " MB/sec; \r\n";
		cout << "decryption time is " << dDecTime << " sec; Speed is " << 4.0 * i / dDecTime << " MB/sec; \r\n";

		cout << "\r\nFilling memory\r\n";
		for (i = 0; i < MEGABYTE / 2; i++) {
			pDword[i] = rnd.GetValue();
		}
		memcpy(hKey, hMem, 256);
	}//end loop for keylength
	if (ifs_crypt.is_open()) ifs_crypt.close();
	if (ifs_key.is_open()) ifs_key.close();

};

int RestDecryptExisting(string dir = "");
int RestDecryptExisting(string dir) {
	int Count = 0;
	std::fstream ifs_key;
	std::fstream ifs_crypt;
	std::string fileKey = dir + "./keys.bin";
	std::string fileCrypt = dir + "./crypt.bin";
	ifs_key.open(fileKey, std::ios::in | std::ios_base::binary);
	ifs_crypt.open(fileCrypt, std::ios::in | std::ios_base::binary);
	if (!ifs_key.is_open() || !ifs_crypt.is_open()) {
		if (ifs_crypt.is_open()) ifs_crypt.close();
		if (ifs_key.is_open()) ifs_key.close();
		return Count;
	}
	MV2_BUFFER mv2Buf;
	BYTE hKey[256];
	//	long lRest[] = { 64, 128, 256, 512,1024 };
	//	long lCore = 0;
	bool bRes = false;
	unsigned int  szCore, szFlags;
	if (MV2CalcRecomendedSizes(2 * MEGABYTE, 1024, &szCore, &szFlags)) {
		cout << "Recomended core's size (bytes) :" << szCore << "; flag's size: " << szFlags << ";\n";
		//cout << "Used core's size (bytes) :" << 1024 << "; flag's size: " << 2 * MEGABYTE + MEGABYTE / 512 << ";\n";
	};
	memset(&mv2Buf, 0, sizeof(MV2_BUFFER));
	std::unique_ptr<BYTE[]> ptrCore(new BYTE[1024]);
	szFlags = max(szFlags, 2 * MEGABYTE + MEGABYTE / 512);
	std::unique_ptr<BYTE[]> ptrFlags(new BYTE[szFlags]);
	std::unique_ptr<BYTE[]> ptrSRC(new BYTE[2 * MEGABYTE]);
	std::unique_ptr<BYTE[]> ptrMem(new BYTE[2 * MEGABYTE]);

	mv2Buf.CoreBuf = ptrCore.get();// (BYTE*) new BYTE[1024];
	if (mv2Buf.CoreBuf) mv2Buf.szCoreBuf = 1024;
	mv2Buf.FlagBuf = ptrFlags.get();// (BYTE*) new BYTE[2 * MEGABYTE + MEGABYTE / 512];
	if (mv2Buf.FlagBuf) mv2Buf.szFlagBuf = szFlags;// 2 * MEGABYTE + MEGABYTE / 512;
	mv2Buf.SrcBuf = ptrSRC.get();// (BYTE*) new BYTE[2 * MEGABYTE];
	if (mv2Buf.SrcBuf) {
		mv2Buf.szSrcBuf = 2 * MEGABYTE;
	}
	memset(mv2Buf.SrcBuf, 0, mv2Buf.szSrcBuf);
	BYTE* hMem = ptrMem.get();// (BYTE*) new BYTE[2 * MEGABYTE];
	int i = 0, j = 0;
	int k = 31;
	for (j = 16; j <= 256; j = j * 2) {
		k = k >> 1;
		cout << "start MV2-Decrypt from file " << j * 8 << " tests:\r\n";
		ifs_key.read((char*)hKey, 256);
		for (i = 0; i < 100; i++) {
			bRes = MV2SetKey(&hKey[j * (i & k)], j);
			if (!bRes) { cout << "\r\nSET KEY" << j * 8 << " RETURN ERROR: " << MV2GetStatus() << ";\n"; break; }
			ifs_crypt.read((char*)&mv2Buf.szCoreBuf, sizeof(int));
			ifs_crypt.read((char*)mv2Buf.CoreBuf, mv2Buf.szCoreBuf);
			ifs_crypt.read((char*)&mv2Buf.szFlagBuf, sizeof(int));
			ifs_crypt.read((char*)mv2Buf.FlagBuf, mv2Buf.szFlagBuf);
			std::cout << " * " << mv2Buf.szCoreBuf << " / " << mv2Buf.szFlagBuf << " ";
			bRes = MV2Decrypt(mv2Buf);
			if (!bRes) { cout << "\r\nDECRYPTION RETURN ERROR!!! CODE: " << MV2GetStatus() << " Steps: " << MV2GetStep() << "\r\n"; }
			else { Count++; }
			if (i == 0) {
				memcpy(ptrMem.get(), mv2Buf.SrcBuf, mv2Buf.szSrcBuf);
			}
			else {
				if (memcmp(ptrMem.get(), mv2Buf.SrcBuf, mv2Buf.szSrcBuf) != 0) {
					cout << "\r\nERROR!! decrypted and source are not equal\r\n";
					break;
				}
			}
		};
		std::cout << "\nKey size = " << j * 8 << " b, successfully decrypted " << Count << " tuples\n";

	}
	MV2Release();
	if (ifs_crypt.is_open()) ifs_crypt.close();
	if (ifs_key.is_open()) ifs_key.close();

	return Count;
}

void test() { TestCore1024(false); }




int main(int argc, TCHAR* argv[], TCHAR* envp[])
{

	int cpuInfo[4] = { -1 };
	__cpuid(cpuInfo, 1);
	std::string str = (cpuInfo[2] & 0x02000000) ? " AES supported\n" : " AES not supported\n";
	std::cout << "Test MV2!" << str;
	int nRetCode = 0;


	std::string dir = "";
	if (argc > 1) dir = argv[1];
	RestDecryptExisting(dir);
	//TestCore1024(dir.empty() ? false : true);

	std::thread f1(test);
	std::thread f2(test);

	f1.join();
	f2.join();


//	std::thread([]() { });
	//std::thread([]() {TestCore1024(false); });




	return nRetCode;
}
