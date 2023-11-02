#Examples

##Linux

```c
 // .....
 #include "../include/mv2lin.h"
 //  ... some code ...
 
 uint8_t* hKey =  (uint8_t*) iKey; // pointer to key buffer
// .....
	bRes = MV2SetKey(hKey, j);
	if (!bRes) {
		std::cout << "\r\nSET KEY" << j * 8 << " RETURN ERROR!!!\r\n";
			break;
	}

 MV2_BUFFER mv2Buf;
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

```