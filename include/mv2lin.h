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

#pragma once

#define MV2_DEFINITION_532121_INCLUDED_

#define BUFSIZE 4096
#define DefMV2Symb 2345678
#define TABLE_LENGTH 260

#pragma GCC visibility push(default)

enum KeySize {
	sizeZero = 0,
	size256bits = 32,
	size512bits = 64,
	size1024bits = 128,
	size2048bits = 256,
	sizeLast
};
enum MV2ERROR {
	MV2ERR_WORK = -1,
	MV2ERR_DWORK = -2,
	MV2ERR_OK = 0,
	MV2ERR_SRCNOTEXIST = 1,
	MV2ERR_OPEN = 2,
	MV2ERR_INPUTRND = 22,
	MV2ERR_MEMALLOC = 40,
	MV2ERR_DECRYPT = 65,
	MV2ERR_WRITE = 77,
	MV2ERR_WRITEFLAG = 79,
	MV2ERR_CREATEOUT = 80,
	MV2ERR_KEYNOTSET = 99,
	MV2ERR_KEYINT = 101,
	MV2ERR_ENDFLAGS = 103,
	MV2ERR_ENDCORE = 104,
	MV2ERR_KEY = 105
};

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


extern "C" {
	bool MV2SetKey(unsigned char* srcKey, int szKey);
	bool MV2Encrypt(MV2_BUFFER& mv2Buf, long RestLength);
	/////
	bool MV2Decrypt(MV2_BUFFER& mv2Buf);
	int MV2GetStatus();
	int MV2GetStep();
	bool MV2CalcRecomendedSizes(uint32_t cbSource, uint32_t dwStep, uint32_t& cbCore, uint32_t& cbFlags);
	bool IsAESProc();
	void MV2Release();
}
#pragma GCC visibility pop
