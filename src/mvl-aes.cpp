// MV2WC.cpp: implementation of the MVLAES class.
//
//////////////////////////////////////////////////////////////////////
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
#include <math.h>
#include "../include/mvl-aes.h"

#ifndef AVAL_TEST_PROCEDURE
#define STEPCOUNTCONST 16
#else
#define STEPCOUNTCONST 1
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MVLAES::MVLAES()
{//initialization
	m_inFile[0] = 0;
	m_inFile[1] = 0;
	m_outFile[0] = 0;
	m_outFile[1] = 0;
//	m_TabFile = INVALID_HANDLE_VALUE;
	m_tarFile[0] = 0;
	m_tarFile[1] = 0;
	m_sourceFile[0] = 0;
	m_sourceFile[1] = 0;

	m_IsTableSet = false;
	m_CurStep = 0;
	m_OutBuf = new unsigned char[BUFSIZE];
	memset(m_OutBuf,0,BUFSIZE);
	m_InBuf	= new unsigned char[BUFSIZE];
	memset(m_InBuf,0,BUFSIZE);
	m_InBuf1 = new unsigned char[BUFSIZE];
	memset(m_InBuf1,0,BUFSIZE);
	m_Status = MV2ERR_OK;//0; // not init
	m_IsTableSet = false;
	m_MustAbort = false;
	memset(m_cTab,0,sizeof(m_cTab));
	memset((unsigned char *)&m_Tab,0,sizeof(TABLE_FOR_CHAR));

//	memset(m_sCoreName,0, MV2_MAX_PATH);
//	memset(m_sFlagName,0, MV2_MAX_PATH);
//	memset(m_sOutFlagName,0, MV2_MAX_PATH);
//	memset(m_tempString,0, MV2_MAX_PATH);
//	memset(m_sTitle,0, MV2_MAX_PATH);
	memset(&m_curZagl[0],0,sizeof(	_MV2ZAGL));
	memset(&m_curZagl[1],0,sizeof(	_MV2ZAGL));
	memset(&m_curZagl[2],0,sizeof(	_MV2ZAGL));

	m_MaxCycle = 100;
	m_minStep = 4;
	

}

MVLAES::~MVLAES()
{
	CloseAllFile();
	ZeroMemory(m_TabBuffer,sizeof(m_TabBuffer));
	ZeroMemory(m_cTab, TABLE_LENGTH);
//	if (m_TabFile != INVALID_HANDLE_VALUE) CloseHandle(m_TabFile);
	if (m_OutBuf) {
		ZeroMemory(m_OutBuf,BUFSIZE);
		delete m_OutBuf;
	}
	if (m_InBuf) {
		ZeroMemory(m_InBuf, BUFSIZE);
		delete m_InBuf;
	}
	if (m_InBuf1) {
		ZeroMemory(m_InBuf1,BUFSIZE);
		delete m_InBuf1;
	}
}

//////////////////////////////////////////////////////////////////////
// generation number for compression
int MVLAES::GetNextTable(bool itEnd) {
	int RetValue =  m_Rand.GetValue();//rand();
	if (itEnd == false ) {
		RetValue = DefMV2Symb;
	}
	if (RetValue < 0) RetValue = abs(RetValue);

	RetValue = (RetValue>>3)&0x0000001F; // count nomber of maping for this version  < 32
	if (SetTable(RetValue))	return RetValue;
	return -1;
//	return RetValue;
}
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////


//#if 0
void MVLAES::CreateMvTable(_UINT8* charSeq, TABLE_FOR_CHAR *inTable){
	unsigned char N,k,bSave,bShag=0;
	unsigned short i,j;
	for (i=0; i<256; i++) inTable->_symb[i] = inTable->_bits[i] = 0; //clearning
	N =3; //=1 old
	k = (1<<N)+1;
	//for (j = 0; j < 254; j++ ){
	for (j = 0; j < 248; j++ ){
		if ( 0 == --k){	k = 1<<(++N); }
		bSave = bShag<<(8-N);
		inTable->_symb[charSeq[j]] = bSave;
		inTable->_bits[charSeq[j]] = N-1;
		bShag++;
	}
	N = 3;
	bShag =0;
	for (j=248; j<256; j++){
		bSave = bShag<<(8-N);
		inTable->_symb[charSeq[j]] = bSave;
		inTable->_bits[charSeq[j]] = 1; // bits = 1 <>2,3,4,5,6
		bShag++;
		bSave = bShag<<(8-N);
	}
//error version	inTable->_Mixed_par = 0;
	memcpy(&(inTable->_Mixed_par),charSeq+258,sizeof(short));

}
//#endif
//////////////////////////////////////////////////////////////////////
bool MVLAES::SetTable(int newNum){ // sets current table corresponding newNum		
	unsigned int tabNumber = 0;
	if (m_IsTableSet){
		tabNumber = (abs(newNum))&0x0000001f;//%m_tabLength; // calculate real tabnumber
		memcpy(m_cTab,m_TabBuffer+(tabNumber*TABLE_LENGTH), TABLE_LENGTH);
		CreateMvTable(m_cTab, &m_Tab);
		m_CurTabNumber = tabNumber;
	}// end if table select
	else return false;
	return true;
}

//////////////////////////////////////////////////////////////////////
// checking char sequence of repeaat code
bool MVLAES::CheckCompTable(_UINT8* charSeq) {
	bool IsCode[256] = {};
	int i;
//	for (i=0; i<256; i++){	IsCode[i] = false;	} // cleaning IsCode
	for (i=0; i<256; i++){ IsCode[charSeq[i]] = true;}
	for (i=0; i<256; i++){ if(IsCode[i] == false) return false; }
//	if (charSeq[256]==charSeq[257]) return false;
	return true;
}
//////////////////////////////////////////////////////////////////////
/// end New Insert
///////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////SET KEY /////////////////////////////////
bool MVLAES::SetKey(_UINT8* srcKey, int szKey){
	m_IsTableSet = false;
	m_Rand.InitRand654();
	int i;
	switch (szKey){
		case size128bits:
			kexpan128((_UINT32 *)srcKey,m_TabBuffer);
			m_IsTableSet = true;
			break;
		case size256bits:
			kexpan256((_UINT32 *)srcKey,m_TabBuffer);
			m_IsTableSet = true;
			break;
		case size512bits:
			kexpan512B((_UINT32 *)srcKey,m_TabBuffer);
			m_IsTableSet = true;
			break;
		case size1024bits:
			kexpan1024((_UINT32 *)srcKey,m_TabBuffer);
			m_IsTableSet = true;
			break;
		case size2048bits:
			kexpan2048((_UINT32 *)srcKey,m_TabBuffer);
			m_IsTableSet = true;
			break;
		default:
			m_IsTableSet =false;
	}
	if (m_IsTableSet){
		for (i=0; i<32;i++){
			if( !CheckCompTable( m_TabBuffer + TABLE_LENGTH*i)){
				m_IsTableSet = false;
				break;
			}
		}//end checcking loop
	}
/*#ifdef _DEBUG
	HANDLE hKey = CreateFile("save_key.mvt",GENERIC_READ|GENERIC_WRITE,0,0,CREATE_ALWAYS,0,0);
	if (hKey != INVALID_HANDLE_VALUE){
		WriteFile(hKey, m_TabBuffer,TABLE_LENGTH*32,(_UINT32*) &i,0);
		CloseHandle(hKey);
	}
#endif*/

	return m_IsTableSet;
}
//////////////////////////////////////////////////////////////////////
 // return the last error if OK then 0
int MVLAES::GetCurStatus() {	return m_Status;}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
int MVLAES::CodingComp(){//char * inName,char * outCore, char * outFlag ){ // coding into differents buffers
	// return number bits adding to file
//////////////////////////////////////////////////////////////////////////////////////////
// compress inName and write reminder to outName and flags to outFlag by mapping curTab follows
// <length in bits:3>- m_m_outFile[0][1]   <image>-m_outFile[0]
    int cbRead, i,cbWrite, cbWriteFlags, factWrite,Prime;
	unsigned char	
		outByte[2],		//	byte for record to buffer
		inByte[2],		//	byte for input char
		inAdd[2],		//	byte for reminder
		fBit[2],		//	counter free bits in  outByte
		mBit[2],		//	cont number of bits to add into output
		rBit[2];		//	cont number of bits to write
    _UINT8 symb1, symb2;
//	unsigned char q1 =0;
//	unsigned char q2 =0;
	bool IsNormal = true; // => _t1 < _t2


	m_outFile[1]->SetPointer(0,CVMBUF_SET_END);// = fopen(outFlag,"a+b"); // "a+b"!!OPEN FOR ADDING

	m_inFile[0]->SetPointer(0,CVMBUF_SET_BEGIN);// = fopen(inName,"rb");
	m_outFile[0]->SetPointer(0,CVMBUF_SET_END);// = fopen(outCore,"wb");
	// symb1 < symb2
	if (m_Tab._t1 < m_Tab._t2) { symb1 = m_Tab._t1; symb2 = m_Tab._t2;}
	else { symb1 = m_Tab._t2; symb2 = m_Tab._t1; IsNormal = false;}
	unsigned char * OutBuf[2];
	OutBuf[0] = m_OutBuf;
	OutBuf[1] = m_OutBuf + BUFSIZE/2;
	outByte[0] = inByte[0] = inAdd[0] = 0;
	fBit[0] = 8; mBit[0] = rBit[0] =0;
	outByte[1] = inByte[1] = inAdd[1] = 0;
	fBit[1] = 8; mBit[1] = rBit[1] =0;
    _UINT32 dwStartOffset = m_outFile[1]->SetPointer(0,CVMBUF_SET_CURRENT); // get start write position
// Open input and output file
	cbWrite = cbWriteFlags = 0;
	while (m_inFile[0]->ReadData(m_InBuf1,BUFSIZE,(_UINT32 *)&cbRead) )
	{// whan input file not finished
		if ((cbRead == 0)||(m_Status== MV2ERR_KEYINT /*101*/)){	 // check state
			break;
		}
		//read source data from buffer
		Prime = m_Tab._Mixed_par;
		Shuffling(m_InBuf1,m_InBuf, cbRead); // mixed
		for (i=0;i<cbRead; i++){// while buffer not ended
			// take charcter from input buffer
			// find image length from table 
			inByte[0] = inAdd[0] = 1; //prepare receiver
			if (m_Tab._bits[m_InBuf[i]] > 6){// 7){ check length for new version
				m_Status = MV2ERR_KEYINT;// 101; // error table
				break;
			}
			if (m_Tab._bits[m_InBuf[i]] == 1) { // is equal to new 'bbb'
				mBit[0] = 5; // write 5 bits
				inByte[0] = inAdd[0] = 0; // write b'00000'
			}
			else mBit[0] = 7 - m_Tab._bits[m_InBuf[i]];
// For 7 bits -> b'1',6->b'01',5->b'001',4->b'0001',3->b'00001',3->b'00000'
//			mBit[0] = 3; // number bits for writes
			if (fBit[0] >= mBit[0]){ // add new value
				outByte[0] = outByte[0] << mBit[0]; 
				outByte[0] = outByte[0]|inByte[0]; //into  inByte -length
				fBit[0] = fBit[0] - mBit[0]; // number of bits to add to bytes
				if (fBit[0]==0){ // byte is full
					if (cbWriteFlags++ >= (BUFSIZE/2)){ // if the buffer is full, than move buffer to file
						if(!m_outFile[1]->WriteData(OutBuf[0],BUFSIZE/2,(_UINT32 *)&factWrite)){
							m_Status = MV2ERR_WRITE;//77;
							goto MER1;
						}
						if (factWrite != (BUFSIZE/2)){
							m_Status = MV2ERR_WRITE;//77; // Error writing
							goto MER1;
						}
						cbWriteFlags =1;

					}
					OutBuf[0][cbWriteFlags-1] = outByte[0]; // write unsigned char into buffer
					outByte[0] = 0; fBit[0] = 8; // prepare for next part of bits
				}//end unsigned char full
			}
			else{ //fBit > mBit => need to divide on 2 blocks
				rBit[0] = mBit[0]-fBit[0];
				inByte[0] = inByte[0]>>rBit[0]; // remove unnecessary bits
				outByte[0] = outByte[0] << fBit[0];
				outByte[0] = outByte[0]|inByte[0]; // adding size to whole byte
				if (cbWriteFlags++ >= (BUFSIZE/2)){ // if buffer is full, than move to file
					if(!m_outFile[1]->WriteData(OutBuf[0],BUFSIZE/2,(_UINT32 *)&factWrite) ){
						m_Status = MV2ERR_WRITE;// 77;
						goto MER1;
					}
					if (factWrite != (BUFSIZE/2)){
						m_Status = MV2ERR_WRITE;//77; // Error writing
						goto MER1;
					}
					cbWriteFlags =1;
				}
				OutBuf[0][cbWriteFlags-1] = outByte[0]; // write unsigned char into buffer
				outByte[0] = 0; fBit[0] = 8; // prapare for next portion bits
				inAdd[0] = inAdd[0] << (8 - rBit[0]);
				inAdd[0] = inAdd[0] >> (8 - rBit[0]);
				outByte[0] = inAdd[0];
				fBit[0] = 8 - rBit[0];
			}// end add kod to flags file
			// the code image length is writing than will write image
			mBit[1] = m_Tab._bits[m_InBuf[i]]+1; // image length in bits
			inByte[1] = m_Tab._symb[m_InBuf[i]]; // symbol representation
			if (mBit[1]== 2){
				mBit[1] = 3;
			}
			
			inAdd[1] = inByte[1] = inByte[1] >> (8 - mBit[1]);
			if (fBit[1] >= mBit[1]){ // the place in byte is exist, that add value
				outByte[1] = outByte[1] << mBit[1];
				outByte[1] = outByte[1]|inByte[1]; //the length into  inByte 
				fBit[1] = fBit[1] - mBit[1]; // number free bits in bytes
				if (fBit[1] == 0){ // byte is full
					if (cbWrite++ >= (BUFSIZE/2)){ // if buffer is full then move it to disk
//						if(!WriteFile(m_outFile[0],OutBuf[1],BUFSIZE/2,(_UINT32 *)&factWrite,0)){
						if(!m_outFile[0]->WriteData(OutBuf[1],BUFSIZE/2,(_UINT32 *)&factWrite) ){
							m_Status = MV2ERR_WRITE;//77;
							goto MER1;
						}
						if (factWrite != (BUFSIZE/2)){
							m_Status = MV2ERR_WRITE;//77; // Error writing
							goto MER1;
						}
						cbWrite =1;
					}
					OutBuf[1][cbWrite-1] = outByte[1]; // write unsigned char into buffer
					outByte[1] = 0; fBit[1] = 8; // prepare to  next bits portion
				}//end unsigned char full
			}
			else{ //fBit > mBit => we need to divide on 2 blocks
				rBit[1] = mBit[1] - fBit[1];
				inByte[1] = inByte[1]>>rBit[1];
				outByte[1] = outByte[1] << fBit[1];
				outByte[1] = outByte[1] | inByte[1]; // adding to byte size
				if (cbWrite++ >= (BUFSIZE/2)){ // if buffer is full than move to file
					if(!m_outFile[0]->WriteData(OutBuf[1],BUFSIZE/2,(_UINT32 *)&factWrite) ){
						m_Status = MV2ERR_WRITE;//77;
						goto MER1;
					}
					if (factWrite != (BUFSIZE/2)){
						m_Status = MV2ERR_WRITE;//77; // Error writing
						goto MER1;
					}
					cbWrite =1;

				}
				OutBuf[1][cbWrite-1] = outByte[1]; // write unsigned char into buffer
				outByte[1] = 0; fBit[1] = 8; // prepare to  next bits portion
				inAdd[1] = inAdd[1] << (8 - rBit[1]);
				inAdd[1] = inAdd[1] >> (8 - rBit[1]);
				outByte[1] = inAdd[1];
				fBit[1] = (8-rBit[1]);
			}
			// image representation was stored;
		}// end read Buffer
	}// end read input file		
	// write rest if nned
	if (cbWriteFlags >= (BUFSIZE/2)){ // if buffer is full => move to file
//		if(!WriteFile(m_outFile[1],OutBuf[0],BUFSIZE/2,(_UINT32 *)&factWrite,0)){
		if(!m_outFile[1]->WriteData(OutBuf[0],BUFSIZE/2,(_UINT32 *)&factWrite) ){
			m_Status = MV2ERR_WRITE;//77;
			goto MER1;
		}
		if (factWrite != (BUFSIZE/2)){
			m_Status = MV2ERR_WRITE;//77; // Error writing
			goto MER1;
		}
		cbWriteFlags =0;
	}
	inByte[0] = 0;
	if (8-fBit[0]) {// add flags rest
		cbWriteFlags++;
		outByte[0] = (outByte[0] << fBit[0])|inByte[0];
		OutBuf[0][cbWriteFlags-1] = outByte[0]; // write unsigned char into buffer
	}
	if (cbWriteFlags > 0) {
//		if(!WriteFile(m_outFile[1],OutBuf[0],cbWriteFlags,(_UINT32 *)&factWrite,0)){
		if(!m_outFile[1]->WriteData(OutBuf[0],cbWriteFlags,(_UINT32 *)&factWrite) ){
			m_Status = MV2ERR_WRITE;//77;
			goto MER1;
		}
		if (factWrite != cbWriteFlags) {
			m_Status = MV2ERR_WRITE;//77;
			goto MER1;
		}
	
	}
// now work with reminder
	if (cbWrite >= (BUFSIZE/2)){ // if buffer is full => move to file
		if(!m_outFile[0]->WriteData(OutBuf[1],BUFSIZE/2,(_UINT32 *)&factWrite) ){
			m_Status = MV2ERR_WRITE;//77;
			goto MER1;
		}
		if (factWrite != (BUFSIZE/2)){
			m_Status = MV2ERR_WRITE;//77; // Error writing
			goto MER1;
		}
		cbWrite =0;
	}
	if (fBit[1] != 8 ){// adding rest
		cbWrite++;
		inByte [1]= 255 >> (8 - fBit[1]); // right 1111
		outByte[1] = (outByte[1] << fBit[1])|inByte[1];
		OutBuf[1][cbWrite-1] = outByte[1]; // write unsigned char into buffer
	}
	if(!m_outFile[0]->WriteData(OutBuf[1],cbWrite,(_UINT32 *)&factWrite) ){
		m_Status = MV2ERR_WRITE;//77;
		goto MER1;
	}

	return 8-fBit[1]; // return bits number
MER1:
	return 9;
}// end CodingComp
//////////////////////////////////////////////////////////////////////
//			Decoding Comp
//////////////////////////////////////////////////////////////////////
//bool MVLAES::DecodingComp(char * inCore, char * inFlag, char * outName, long endByte ){
bool MVLAES::DecodingComp(long endByte ){
// m_inFile[0] - curent core
// m_sourceFile[1] - curent flag
// endByte =  size of core
// m_outFile[0] - result to decoding
// m_curZagl[0].Reminder - core reminder

// inCore - current coding core, inFlag - current flags inName - outFile
// m_Tab - table for decoding must be setup calling procedure
// m_inFile[0] - current core position,
// m_inFile[1] - current flag;
// m_outFile[0] - out core
    int coreBitSize = 0; // core BitSize to buffer processing
    int  cbAllCoreRead = 0; // all core read
    _UINT32  cbWrite, cbRead,cbReadFlag,factWrite; // length write code and read code
	bool EndProc = false; // inication for end of process
						  // EndProc == true if all core is Reading and reminder processing
	unsigned char * inBuf[2];
	int Prime;
    int counter = 0;
	inBuf[0] = m_InBuf;
	inBuf[1] = m_InBuf+(BUFSIZE/2);
//!!!	if (endByte < 0) endByte = - endByte;
	if (endByte == 0) {
		m_Status = MV2ERR_DECRYPT;//65; // ERROR 65 Bad Decoding
		return false;
	}
// open all file and start
	m_outFile[0]->SetPointer(0,CVMBUF_SET_BEGIN);
// preparing buffers => reading input files
	long  curByte[2],i; 
	unsigned char selByte[2], addByte[2], rBit[2], curBit[2], selLen[2];// fBit[2];
	rBit[0] = rBit[1] = 0; curBit[0] = curBit[1] = 0;
	selLen[1] =0; selLen[0] = 0;
	addByte[1] = addByte[0] = 0;
	bool IsLength = true;
	cbWrite = 0;
	// read Flag
	if (!m_sourceFile[1]->ReadDataAes(inBuf[1],BUFSIZE/2,(_UINT32*)&cbReadFlag) ){
		m_Status = m_sourceFile[1]->GetError(); //GetLastError();
		return false;
	}
	// read Core
	if (!m_inFile[0]->ReadData(inBuf[0],BUFSIZE/2,(_UINT32*)&cbRead) ){
		m_Status = m_inFile[0]->GetError();//GetLastError();
		return false;
	}
	if (cbRead == 0) return false; // 
	cbAllCoreRead += cbRead;
	if (cbAllCoreRead == endByte){
		if (m_curZagl[0].Reminder)
			coreBitSize = cbRead*8 -8 + m_curZagl[0].Reminder + rBit[0]; // real size
		else
			coreBitSize = cbRead*8 + rBit[0]; // real size
	}
	else {
		coreBitSize = 0; // don't check end
	}

	curByte[0] = curByte[1] = 0;
	while(!EndProc){ // 
		if (IsLength) {// select length
			selByte[1] = inBuf[1][curByte[1]]<<curBit[1]; // select curent flag 
			counter = 5; // <=5 bit
			while (curBit[1]++ < 8){
				selLen[1]++;
				if ((selByte[1]&128)){
					IsLength = false;
					break;
				}
				if (selLen[1] == 5){
					selLen[1]++;
					IsLength = false;
					break;
				}
				selByte[1]= selByte[1]<<1;
			}//end while
			if (curBit[1] >=8) {
				curBit[1] = 0;
				if (++curByte[1] >= cbReadFlag){
//					if (!ReadFile(m_sourceFile[1],inBuf[1],BUFSIZE/2,(_UINT32*)&cbReadFlag,0)){
					if (!m_sourceFile[1]->ReadDataAes(inBuf[1],BUFSIZE/2,(_UINT32*)&cbReadFlag) ){
						m_Status = m_sourceFile[1]->GetError();//GetLastError();
						return false;
					}
					if (cbReadFlag > 0){curByte[1] = 0;	}
					else{// end work flag file ends ???
						if (IsLength) {
							m_Status = MV2ERR_ENDFLAGS; //:103; // Decode error - end flags 
							break;
						}//
					}
				}// end ++curByte
			}// end curbit проверка
			if (!IsLength) { // setup length core 
				if (selLen[1] > 6 ){
					m_Status = MV2ERR_DECRYPT;//65; //ERROR 65 Decode error
					break;
				}
				else {
					if (selLen[1] == 6) selLen[0] = 3;
					else selLen[0] = 8 - selLen[1];
				}
/// We must remember flag				selLen[1] = 0;
			}
		}//end select length
		else{// length for core selected => select code
			selByte[0] = inBuf[0][curByte[0]];
			if ((8-curBit[0]) < selLen[0]){ // next unsigned char neaded
				addByte[0] = selByte[0]; rBit[0] = 8-curBit[0]; curBit[0] = 0;
				// now read buffers if is nessiasry
				if (++curByte[0] >= cbRead){
//					if (!ReadFile(m_inFile[0],inBuf[0],BUFSIZE/2,(_UINT32*)&cbRead,0)){
					if (!m_inFile[0]->ReadData(inBuf[0],BUFSIZE/2,(_UINT32*)&cbRead) ){
						m_Status = m_inFile[0]->GetError(); //GetLastError();
						return false;
					}
					if (cbRead > 0){
						curByte[0] = 0;	
						cbAllCoreRead += cbRead;
						if (cbAllCoreRead == endByte){
							if (m_curZagl[0].Reminder)
								coreBitSize = cbRead*8 -8 + m_curZagl[0].Reminder + rBit[0]; // real size
							else
								coreBitSize = cbRead*8 + rBit[0]; // real size
						}
						else {
							coreBitSize = 0;
						}

					}
					else{// end work flag file ends ???
						m_Status = MV2ERR_ENDCORE; // 104; // ERROR 104 End core befor all decoding
						break;
					}
				}// end ++curByte
			}
			else{ // select code
				if (coreBitSize) 
				{ 
					coreBitSize -= selLen[0]; // increase reminder size
				}
				addByte[0] = addByte[0]<<(8-rBit[0]);// if adding previosly preparing addByte
				selByte[0] = selByte[0] << curBit[0];
				selByte[0] = selByte[0]	>> rBit[0];
				selByte[0] = selByte[0] | addByte[0];
				selByte[0] = selByte[0] >> (8-selLen[0]);
				selByte[0] = selByte[0] << (8-selLen[0]);
				curBit[0] += selLen[0] - rBit[0];
				addByte[0] =0; rBit[0] = 0; 
				IsLength = true;
				// select code position
				selByte[0] = (selByte[0]>>(8-selLen[0]));
				if (selLen[0]){
					i = (1<<selLen[0])-8;
					if (selByte[0]< i ) 
						selByte[0] = selByte[0] + (1<<selLen[0]);
				}
				if (selLen[1]==6){ // the same code
					selByte[0] += 248;
				}
				selLen[1] = 0; // clear length
				if (m_Status < 0 ){
					m_OutBuf[cbWrite] = m_cTab[selByte[0]];
					if (++cbWrite >= BUFSIZE) {
						Prime = m_Tab._Mixed_par;
						UnShuffling(m_OutBuf,m_InBuf1,BUFSIZE);
//						if (!WriteFile(m_outFile[0],m_InBuf1,BUFSIZE,(_UINT32*)&factWrite,0)){
						if (!m_outFile[0]->WriteData(m_InBuf1,BUFSIZE,(_UINT32*)&factWrite) ){
							m_Status = m_outFile[0]->GetError(); //GetLastError(); 
							return false;
						}
						if (factWrite != BUFSIZE) return false;//goto MER2;
						cbWrite = 0; 
					}
				}
				// analis to end processing
				if (cbAllCoreRead >= endByte){
					if (coreBitSize <= 0) 
						EndProc = true;
				}
				// end analis to end processing
			}// end code select
		}//end select core
	}
	if (m_Status > 0) 	return false; 
	if ((cbWrite>0)&&(cbWrite <=BUFSIZE)){
		Prime = m_Tab._Mixed_par;
		UnShuffling( m_OutBuf,m_InBuf1, cbWrite);
//		if (!WriteFile(m_outFile[0],m_InBuf1,cbWrite,(_UINT32*)&factWrite,0)){
		if (!m_outFile[0]->WriteData(m_InBuf1,cbWrite,(_UINT32*)&factWrite) ){
			m_Status = m_outFile[0]->GetError();// GetLastError();
			return false;
		}
		if (factWrite != cbWrite) return false;
		cbWrite = 0; 
	}
	if (m_Status > 0) 
		return false; 
	if (cbReadFlag){
		m_sourceFile[1]->SetPointer(curByte[1] - cbReadFlag,CVMBUF_SET_CURRENT);
		if (curBit[1]>0)
//			SetFilePointer(m_sourceFile[1],1,0,FILE_CURRENT);
			m_sourceFile[1]->SetPointer(1,CVMBUF_SET_CURRENT);
	}
	return true;
}// end DecodingComp	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MVLAES::CloseAllFile(){
	int i;
	for (i= 0; i<2; i++){
		m_sourceFile[i] = 0;
		m_inFile[i]=0;
		m_outFile[i] = 0;
		m_tarFile[i]=0;
	}
	for (i = 0; i < 5 ; i++){
		m_tmpBuf[i].Release();
	}
}
//////////////////////////////////////////////////////////////////////
 // coding into differents files
bool MVLAES::InitCodeMem(MV2_BUFFER &mv2Buf, int RestLength){

	_UINT32 dwTemp[8];
	bool bIsExcludeTable = false; //2006-08
	// 2006-06 check initial size
	if (mv2Buf.szSrcBuf < 16) {
		this->m_Status = MV2ERR_SOURCELEN; // initial data is small
		return false;
	}

//STEP 1 check type number of rounds or core length
	_UINT32 MaxStep = 100; // counter rounds
	CVMBuf * tmpcvm = 0; // for changing buffers
	if (mv2Buf.szSrcBuf == 0){
		mv2Buf.RetCode = m_Status = MV2ERR_SOURCELEN;
		return false;
	}
	if (RestLength <= 50) {
		if (RestLength < 0 ){
			mv2Buf.RetCode = m_Status = MV2ERR_INPUTRND;// 22;
			return false;
		}
		else {
			//m_minStep=RestLength;
			MaxStep = RestLength;
			RestLength  = 1;
		}
	}

	int CoreLength = 0;
	m_Status = MV2ERR_WORK; //-1; // processing 
	int cbRead;
	if (!m_IsTableSet){// not table
		mv2Buf.RetCode = m_Status = MV2ERR_KEYNOTSET;// 99; // ERROR 99 Table not set
		return false;
	}
	unsigned long fOffset[100];
	memset(fOffset,-1,sizeof(fOffset)); // array of flag offset
	int factWrite = 0;

	// check access to Buffer;
	if (!IsBadReadPtr(mv2Buf.SrcBuf, mv2Buf.szSrcBuf)){
		// preparing memory
		m_inFile[0] = &m_tmpBuf[0];
		m_outFile[0] = &m_tmpBuf[1];
		m_outFile[1] = &m_tmpBuf[2];
		m_sourceFile[0] = &m_tmpBuf[3];

		factWrite  = mv2Buf.szSrcBuf + (mv2Buf.szSrcBuf>>2) + 2048;
		if (!m_inFile[0]->Alloc(factWrite) ) { 
			mv2Buf.RetCode = m_Status = MV2ERR_MEMALLOC;// 40; // can't allocate memory
			CloseAllFile();
			return false; 
		}
		if (!m_outFile[0]->Alloc(factWrite) ){ 
			mv2Buf.RetCode = m_Status = MV2ERR_MEMALLOC;// 40; // can't allocate memory
			CloseAllFile();
			return false; 
		}
		if (!m_outFile[1]->Alloc(factWrite) ) { 
			mv2Buf.RetCode = m_Status = MV2ERR_MEMALLOC; //40; // can't allocate memory
			CloseAllFile();
			return false; 
		}
		factWrite = 0;

// ROUND 1 - read file and whitening
//		Read seource data
		if (m_sourceFile[0]->AllocExisting(mv2Buf.SrcBuf,mv2Buf.szSrcBuf)) { // 			
			CoreLength = m_sourceFile[0]->GetSize();//(_UINT32 *)&CoreLength); //CoreLength  = flength(m_sourceFile[0]);
			m_minStep = STEPCOUNTCONST;//for testa 16-standart;
		//	if (CoreLength) { // calculate number of loops
		//		m_minStep += (int)(log10((double)CoreLength) * 1.22);
				//if ((_UINT32) m_minStep > MaxStep) m_minStep = MaxStep;
		//	}
			if ((_UINT32) m_minStep > MaxStep)  MaxStep = m_minStep;
			m_CurStep = 0;
			//	preparing for started coding

			m_curZagl[0].Reminder = 0;
			m_curZagl[0].Counter = GetNextTable(true); // load table for rc4 

			m_noise.prepare_key(m_cTab,256); // set rc4 key ????
			
			m_outFile[0]->WriteData((_UINT8 *)&m_curZagl[0],sizeof(_MV2ZAGL),(_UINT32 *)&factWrite) ;
			
			// whitening
			do  {// copy source file to tempfile
				if (!m_sourceFile[0]->ReadData(m_InBuf,BUFSIZE,(_UINT32 *)&cbRead) ){	break;	}
				if (cbRead == 0)	{ break;	}
				m_noise.rc(m_InBuf,cbRead); // write mixing
				if (!m_outFile[0]->WriteData(m_InBuf,cbRead,(_UINT32 *)&factWrite)){CloseAllFile(); m_Status = MV2ERR_WRITE;  return false;}
				if (factWrite != cbRead) {CloseAllFile(); m_Status = MV2ERR_WRITE; return false;}				
			} while (cbRead > 0);
			m_sourceFile[0]->Release(); 
			m_sourceFile[0] = 0;
//END Round 1
			// end preparing 
		}// end open sourse
		else {
			mv2Buf.RetCode = m_Status = MV2ERR_OPEN; // 2; // ERROR 2 => Share violation
			CloseAllFile();
			return false;
		}// end error open source
	}// end if source exist
	else{
		CloseAllFile();
		mv2Buf.RetCode = m_Status = MV2ERR_SRCNOTEXIST;//1; // ERROR = 1; Source file not exist
		return false;
	}// end if source not exist;
// Round 2 and .....
	
	do{ 	// Begin processing
		tmpcvm = m_inFile[0];
		m_inFile[0] = m_outFile[0];//
		m_outFile[0] = tmpcvm;

		m_inFile[0]->SetPointer(0,CVMBUF_SET_BEGIN);
		m_outFile[0]->SetSize(0);
		if (m_Status != MV2ERR_WORK) 
			break; // The error is occured => End working
		fOffset[m_CurStep++] = m_outFile[1]->GetSize();// = m_szFlags;

		// we must define number of transformation of current rounds
		if (bIsExcludeTable){ //2006-08)
			m_curZagl[0].Counter = 17; //fix table
			this->SetTable(17);
		}
		else { 
			m_curZagl[0].Counter = GetNextTable(true); // The mapping will be  loaded fom mapping array
			if (m_inFile[0]->GetSize() < 48) {
				while(m_curZagl[0].Counter == 17)
					m_curZagl[0].Counter = GetNextTable(true);
			}
		}
// run round encryption
//		m_curZagl[2].szCompSize = GetFileSize(m_inFile[0],NULL);//getflength(m_tempString);

		m_outFile[0]->WriteData((_UINT8 *)&m_curZagl[0],sizeof(_MV2ZAGL),(_UINT32 *)&factWrite);
		m_outFile[1]->SetPointer(0,CVMBUF_SET_END); // set current write position
	// start 2006-08 changing
		// 2006-08 //if ((m_curZagl[0].Reminder = CodingComp())>= 8){ break;}
		m_curZagl[0].Reminder = CodingComp();

		bIsExcludeTable = false;
		CoreLength = m_outFile[0]->GetSize();
		if (CoreLength < 16){
			// ????????????
			dwTemp[0] = m_Rand.GetValue(); dwTemp[1] = m_Rand.GetValue();
			dwTemp[2] = m_Rand.GetValue(); dwTemp[3] = m_Rand.GetValue();
			dwTemp[4] = m_Rand.GetValue(); dwTemp[5] = m_Rand.GetValue();
			dwTemp[6] = m_Rand.GetValue(); dwTemp[7] = m_Rand.GetValue();
			m_outFile[0]->SetPointer(0,CVMBUF_SET_END);
			m_outFile[0]->WriteData((_UINT8 *) dwTemp, 32,&dwTemp[0]);
			if (m_CurStep == m_minStep)	m_minStep++;
			if (m_CurStep == MaxStep) MaxStep++;
			
			//if ((_UINT32) m_minStep > MaxStep)  MaxStep = m_minStep;
			bIsExcludeTable = true;
			
		}
	// end 2006-08 changing
		// preparing resalt
		// preparing resalt
		if (m_Status != MV2ERR_WORK) break; // The error is occured => End working
		CoreLength = m_outFile[0]->GetSize();
// write flag's header
		m_outFile[1]->SetPointer(0,CVMBUF_SET_END);
// add rest 
		m_outFile[0]->SetPointer(0,CVMBUF_SET_BEGIN);
		m_outFile[0]->WriteData((_UINT8 *) &m_curZagl[0],sizeof(_MV2ZAGL),(_UINT32 *)&factWrite);
	}while(((CoreLength >= RestLength)&&(m_CurStep < MaxStep))||(m_CurStep < (_UINT32) m_minStep ));
	if (m_Status/*GetCurStatus()*/== MV2ERR_WORK){ // if not error

// Create the cores header
		tmpcvm = m_inFile[0];
		m_inFile[0]->Release();
		m_inFile[0] = m_outFile[0];
		m_outFile[0] = 0;
		m_inFile[0]->SetPointer(0,CVMBUF_SET_BEGIN);
		
		m_inFile[1] = m_outFile[1];//m_inFile[1] = fopen(m_sOutFlagName,"rb");
		m_outFile[1] = 0;

		m_inFile[1]->SetPointer(0,CVMBUF_SET_BEGIN);
		unsigned long EndOffset, CurOffset;
		EndOffset = m_inFile[1]->GetSize();
		int iStep = m_CurStep;
// write output of flags
//		m_tarFile[1] = CreateFile(LPCTSTR(outFlag),GENERIC_READ|GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,0);
		tmpcvm->Release();
		m_tarFile[1] = tmpcvm;
		if (!m_tarFile[1]->AllocExisting(mv2Buf.FlagBuf,mv2Buf.szFlagBuf)){
			CloseAllFile();
			mv2Buf.RetCode = m_Status = MV2ERR_CREATEOUT; // 80; // error create out file;
			return false;
		}
		m_tarFile[1]->SetSize(0);
		m_inFile[0]->GetHash(m_tarFile[1]->m_state.aesKey);
		updatehash(m_TabBuffer, m_tarFile[1]->m_state.aesKey);
		m_tarFile[1]->aes.Initialize(m_tarFile[1]->m_state.aesKey,16);
		memcpy((_UINT8 *) m_tarFile[1]->m_state.dwCounter.ch, m_tarFile[1]->m_state.aesKey,16);
		// write output flags
		_UINT8* tarBuf = nullptr;
		while(iStep-- > 0){
			CurOffset = fOffset[iStep];
			while( CurOffset != EndOffset){
				m_inFile[1]->SetPointer(CurOffset, CVMBUF_SET_BEGIN);
				m_inFile[1]->PseudoReadData(tarBuf, _min(BUFSIZE, EndOffset - CurOffset), (_UINT32*)&cbRead);
				CurOffset += cbRead;
				//if (!m_tarFile[1]->WriteData(m_InBuf,cbRead,(_UINT32 *)&factWrite)) {
				if (!m_tarFile[1]->WriteDataAes(tarBuf, cbRead, (_UINT32*)&factWrite)) { // write Aes
					mv2Buf.RetCode = m_Status = MV2ERR_WRITEFLAG; //79;	
					break;
				}

#if 0
				m_inFile[1]->SetPointer(CurOffset,CVMBUF_SET_BEGIN);
				m_inFile[1]->ReadData(m_InBuf,min(BUFSIZE,EndOffset-CurOffset),(_UINT32 *)&cbRead);
				CurOffset += cbRead;
				//if (!m_tarFile[1]->WriteData(m_InBuf,cbRead,(_UINT32 *)&factWrite)) {
				if (!m_tarFile[1]->WriteDataAes(m_InBuf,cbRead,(_UINT32 *)&factWrite)) { // write Aes
					mv2Buf.RetCode = m_Status = MV2ERR_WRITEFLAG; //79;	
					break;	
				}
#endif
			}
			if (m_Status == MV2ERR_WRITEFLAG) break;
			EndOffset = fOffset[iStep];
		}
		mv2Buf.szFlagBuf = m_tarFile[1]->GetSize();
		m_tarFile[1]->Release();
		m_inFile[1]->Release();
		m_tarFile[1] = 0;
		m_inFile[1] =  0;
//    aes flag encryption 

		m_tarFile[0] = &m_tmpBuf[3];
		m_tarFile[0]->AllocExisting(mv2Buf.CoreBuf,mv2Buf.szCoreBuf);
		m_tarFile[0]->SetSize(0);
//!!!!!!!!!!		WriteFile(m_tarFile[0],&m_curZagl[0],sizeof(_MV2ZAGL),(_UINT32 *)&factWrite,0);
// Difine outputs parts
		// Reading
		// write parts of core
		m_inFile[0]->SetPointer(0,CVMBUF_SET_BEGIN);
		do {
			if( !m_inFile[0]->ReadData(m_InBuf,BUFSIZE,(_UINT32 *)&cbRead) ){	break;		}
			if (!cbRead) break;
			if (!m_tarFile[0]->WriteData(m_InBuf,cbRead,(_UINT32 *)&factWrite)) {		m_Status =  MV2ERR_WRITEFLAG; break;	}
		}while(cbRead > 0);
		mv2Buf.szCoreBuf = m_tarFile[0]->GetSize();
		m_tarFile[0]->Release(); m_tarFile[0] = 0;// The core is created

	}// end status OK
	if (m_Status == MV2ERR_WORK) {mv2Buf.RetCode = m_Status = MV2ERR_OK;}
	CloseAllFile();
	if (m_Status != MV2ERR_OK)	{	mv2Buf.RetCode = m_Status; return false; }
	mv2Buf.RetCode = m_Status;
	return true;
}
//////////////////////////////////////////////////////////////////////
// decoding from different files
//////////////////////////////////////////////////////////////////////
bool MVLAES::InitDecodeMem(MV2_BUFFER & mv2Buf){
 	int CoreLength = 0;
	//char symStep[4];
	int factWrite;
	CVMBuf * tmpmvb;
	m_Status = MV2ERR_DWORK; //-2; // processing 
    _UINT32 cbRead, cbWrite, cbFlagRead;
    _UINT32 flagPointer = 0;
//	long i;
	if (!m_IsTableSet){
		// if key is not set
		m_Status = MV2ERR_KEYNOTSET;//99; // ERROR 99 Table not set
		return false;
	}
	// open source core
		m_inFile[0] = &m_tmpBuf[0];
		m_outFile[0] = &m_tmpBuf[1];
		m_outFile[1] = &m_tmpBuf[2];
		m_sourceFile[1] = &m_tmpBuf[3];
		m_sourceFile[0] = &m_tmpBuf[4];

		cbFlagRead  = mv2Buf.szCoreBuf + mv2Buf.szFlagBuf;
		if (!m_inFile[0]->Alloc(cbFlagRead) ) { 
			mv2Buf.RetCode = m_Status = MV2ERR_MEMALLOC;//40; // can't allocate memory
			CloseAllFile();
			return false; 
		}
		if (!m_outFile[0]->Alloc(cbFlagRead) ){ 
			mv2Buf.RetCode = m_Status = MV2ERR_MEMALLOC;// 40; // can't allocate memory
			CloseAllFile();
			return false; 
		}
		
		cbFlagRead = 0;

		

//	CRYPTO::RIJNDAEL aes;
	AESSTATE state;	
//	m_sourceFile[0] = CreateFile(inCore,GENERIC_READ,0,NULL,OPEN_EXISTING,0,0);
	if (m_sourceFile[0]->AllocExisting(mv2Buf.CoreBuf,mv2Buf.szCoreBuf)){

		CoreLength = mv2Buf.szCoreBuf; // store file size

		// Set AES Decryption

//		MD5Context md5cnt;
		if (CoreLength>0){ // checcking that file exist  = getflength(inCore))

			//	preparing for started coding
			_UINT8* tarBuf = NULL;
			do  {
				//m_sourceFile[0]->ReadData(m_InBuf,BUFSIZE,(_UINT32 *)&cbWrite); //AESKEY
				m_sourceFile[0]->PseudoReadData(tarBuf, BUFSIZE, (_UINT32*)&cbWrite);
  				//if (!m_outFile[0]->WriteData(m_InBuf,cbWrite,(_UINT32 *)&factWrite))	{
				if (!m_outFile[0]->WriteData(tarBuf, cbWrite, (_UINT32*)&factWrite)) {
					m_Status = m_outFile[0]->GetError();  //GetLastError();
					CloseAllFile();
					return false;
				}
			} while (factWrite > 0);
			m_sourceFile[0]->GetHash(state.aesKey);
			updatehash(m_TabBuffer, state.aesKey);
			m_sourceFile[0]->Release();
			if (!m_sourceFile[1]->AllocExisting(mv2Buf.FlagBuf,mv2Buf.szFlagBuf) )	{
				m_Status = m_sourceFile[1]->GetError();//GetLastError();
//MERR2:
				CloseAllFile(); 
				return false;
			}
			memcpy((_UINT8 *) m_sourceFile[1]->m_state.aesKey, (_UINT8 *) state.aesKey, 16);
			m_sourceFile[1]->aes.Initialize(state.aesKey,16);
			memcpy((_UINT8 *) m_sourceFile[1]->m_state.dwCounter.ch, (_UINT8 *) m_sourceFile[1]->m_state.aesKey,16);

			cbFlagRead = m_sourceFile[1]->GetSize(); // Get length of Flag
			// end preparing 
		}// end open sourse
		else {
			m_Status = MV2ERR_SRCNOTEXIST;//1; // ERROR = 1; Source file has zero length
			CloseAllFile();
			return false;
		}// end error open source
	}// end if source exist
	else{
//		m_Status = GetLastError();
		m_Status = MV2ERR_OPEN;//2; // ERROR 2 => Share violation
		CloseAllFile();
		return false;
	}// end if source not exist;
// all preparing cancel
	flagPointer = m_sourceFile[1]->SetPointer(0,CVMBUF_SET_CURRENT); // getCurentPointer
	m_CurStep = 0;
//	Begin main LOOP
	do{ 	// Begin processing
		tmpmvb = m_inFile[0]; m_inFile[0] = m_outFile[0]; m_outFile[0] = tmpmvb;
		m_outFile[0]->SetSize(0);
		if (m_Status != MV2ERR_DWORK /*-2*/) 
			break; // The error is aqured
//		wsprintf(symStep,"%i3",m_CurStep);
		m_CurStep++; // increase  number of step
//	2. Из <title>.mv& - select header to determine current number of mapping from mapping array and service information

		m_inFile[0]->SetPointer(0,CVMBUF_SET_BEGIN); // set core pointer to start
		cbWrite = m_inFile[0]->GetSize(); // read core size
		m_inFile[0]->ReadData((_UINT8 *) &m_curZagl[0],sizeof(_MV2ZAGL),(_UINT32 *)&cbRead); // read core header
		// Sets current Table
		if (!SetTable(m_curZagl[0].Counter)){ // 
			CloseAllFile();
			return false;
		}
		// CREATE out core file	

//	MV2 mapping invertion
		if (m_Status != MV2ERR_DWORK /*-2*/) break;

		if (!DecodingComp(cbWrite -1)){ // send real core size to decodin
			m_Status = MV2ERR_DECRYPT;//65; // !!! ERROR 55  Bad Decoding
			CloseAllFile();
			return false;
		}
		if (m_Status != MV2ERR_DWORK /*-2*/) break;
		// 2006-08
		cbWrite = (long) m_outFile[0]->GetSize();
		if ( (cbWrite > 32)&&(cbWrite< 48)&&(m_curZagl[0].Counter == 17) ){
			m_outFile[0]->SetSize(cbWrite-32);			
		}
		//end 2006-08
		flagPointer = m_sourceFile[1]->SetPointer(0,CVMBUF_SET_CURRENT);
	}while((cbFlagRead > flagPointer)&&(m_CurStep <= m_MaxCycle));
	if (m_CurStep >= STEPCOUNTCONST) {
		if (m_Status == MV2ERR_DWORK){ // -2){ // if not error
			// MV2 process canceled
			tmpmvb = m_inFile[0]; m_inFile[0] = m_outFile[0]; m_outFile[0] = tmpmvb;
			m_outFile[0]->SetSize(0);

			m_inFile[0]->SetPointer( 0,CVMBUF_SET_BEGIN);
			m_inFile[0]->ReadData((_UINT8 *) &m_curZagl[0],sizeof(_MV2ZAGL),(_UINT32 *)&factWrite); //read header
			SetTable(m_curZagl[0].Counter); // set RC$ key (noise key)
			m_noise.prepare_key(m_cTab,256); // prepare rc4 key (noise key)

			m_tarFile[0] = &m_tmpBuf[4];
			if (!m_tarFile[0]->AllocExisting(mv2Buf.SrcBuf,mv2Buf.szSrcBuf)){
				m_Status = m_tarFile[0]->GetError();
				CloseAllFile();
				return false;
			}
			m_tarFile[0]->SetSize(0);
			do {
				if (!m_inFile[0]->ReadData(m_InBuf,BUFSIZE,(_UINT32 *)&cbRead)){
					m_Status = m_inFile[0]->GetError();
					CloseAllFile();
					return false;
				}
				if (cbRead == 0 ) break;
                
				m_noise.rc(m_InBuf,cbRead); // re-noising
                
				if (!m_tarFile[0]->WriteData(m_InBuf,cbRead,(_UINT32 *)&factWrite)){
					m_Status = m_tarFile[0]->GetError();
					CloseAllFile();
					return false;
				}
			}while(cbRead > 0);
			mv2Buf.szSrcBuf = m_tarFile[0]->GetSize();
			m_tarFile[0]->Release();
			m_inFile[0]->Release();
		}
	}
	else {
		m_Status = MV2ERR_ENDFLAGS ;
		CloseAllFile();
		return false;
	}
	if (m_Status == MV2ERR_DWORK/*-2*/) m_Status = MV2ERR_OK; //0;
	mv2Buf.RetCode = m_Status;
	CloseAllFile();
//#endif
	return true;
}// end InitDecode diferent files

bool MVLAES::MV2CalcRecomendedSizes(_UINT32 cbSource, _UINT32 dwStep, _UINT32 *cbCore, _UINT32 *cbFlags)
{// calculate recomended sizes for output buffers
    _UINT32 szCore = cbSource+1;
    _UINT32 szCoreMin = cbSource+1;
    _UINT32 szFlag = 0;
    _UINT32 Count = _max(dwStep,16);
    //int i;

    if (dwStep >50){
        if (szCore <= dwStep) Count =16;
        else {
            Count =0;
            while (szCore >= dwStep){
                Count++;
                if (szCore > 0x00FFFFFF) {
                    szCore = (szCore>>7)*97 + 1;
                }
                else {
                    szCore = 1 + ((szCore*97)>>7);
//					szCoreMin = 1 + ((szCore*96)>>7);
//					if (szCoreMin + 16  <= szCore) szCore = dwStep;//szCoreMin; + 32;

                }
                if (szCore <= 16) szCore += 33;
            }//end while
            if (Count < 16) Count = 16;
            szCore = cbSource+1;
        }
    }
    for (_UINT32 i=0; i < Count; i++){
        szFlag += (szCore>>2)+1;
        if (szCore > 0x00FFFFFF) {
            szCore = szCoreMin = (szCore>>7)*97 + 1;
//			szCoreMin =  (szCoreMin>>7)*96 + 1;
        }
        else {
            if (szCore > 32767) szCore = szCoreMin = ((szCore*97)>>7) + 2;
            else {
                szCore =  ((szCore*99)>>7) + 1;
                szCoreMin = 1 + ((szCoreMin*95)>>7);
            }
//			if (szCoreMin <= 16)  szCore = 16;
//			if ((szCoreMin + szCore)/2 <= 16) szCore = (szCore + szCoreMin)/2;
        //	if ((szCoreMin <= 16)&&(szcoreMin+32 > )) szCore = szCoreMin;
        }
        if (szCoreMin <= 16) {
            szCoreMin += 33;
            Count += 1;
        }
        if (szCore <= 16){
            szCore += 33;
            //Count += 1;
        }
        if (szCoreMin > szCore) szCore = szCoreMin;
    }
    szFlag += Count;// + (cbSource>>4);
    if ((szCore*3/2) < 51) szCore = szCore*2;
    if (dwStep >50) szCore =  dwStep;
    try{
        memcpy(cbCore, &szCore, sizeof(_UINT32));
        memcpy(cbFlags, &szFlag, sizeof(_UINT32));
    }
    catch(...){
        return false;
    }
    return true;
}

///////////////// TESTING FUNCTIONS ///////////////////////////////////////////////////////////////////////////////









