// MV2WC.h: interface for the MVL class.
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

#if !defined(AFX_MV2WC_H__0A5E73D6_18E1_4B20_92C4_AC2AD04A9977AES__INCLUDED_)
#define AFX_MV2MEM_H__0A5E73D6_18E1_4B20_92C4_AC2AD04A9977AES__INCLUDED_


//#include "stdio.h"
#include <stdlib.h>
#include <time.h>
#include "../include/mv2base.h"

#ifdef __GNUC__
#pragma GCC visibility push(hidden)
#endif

// AES ENCRYPT CLASS


////////////////////////////////////////////////////////////////////////////////////////////////////
// This class emplaments MV2 algorithm realisation
/////////////////////////////////////////////////////////////////////////////////////////////////
class MVLAES  
{
public:
	CRand654 m_Rand; // random generator
//    int m_RandPos;// for testing Rand position;
//	int GetTestRandom(bool ItEnd = false); // for testing Random value array
  //  _UINT32 m_RandValue
	MVLAES();
	~MVLAES();
	CNoiseRC m_noise; 
    int m_CurStep; //current step
	bool m_MustAbort; // stop 
private:
    bool CheckCompTable(_UINT8 * charSeq); // checking char sequence for repeaat code

    bool InitCodeMem(MV2_BUFFER &mv2Buf, int RestLength); // coding into differents files
	bool InitDecodeMem(MV2_BUFFER &mv2Buf); // decoding from different files
	int GetCurStatus(); // return state of the encryption/decryption process or  last error or OK
    bool SetKey(_UINT8 * srcKey, int szKey); // Set key
	bool SetTable(int newNum); // sets current table for encoding corresponding newNum		
	//bool SetTableDecode(int newNum); // sets current table for decoding corresponding newNum		
public:
    inline bool MV2SetKey(_UINT8 * srcKey, int szKey){       return SetKey(srcKey,szKey);   }
    inline bool MV2Encrypt(MV2_BUFFER &mv2Buf, int corelength){return InitCodeMem(mv2Buf, corelength);}
    inline bool MV2Decrypt(MV2_BUFFER &mv2Buf){return InitDecodeMem(mv2Buf);}
    inline int  MV2GetStatus(){return GetCurStatus();}
    inline int  MV2GetStep(){return m_CurStep;}
    bool MV2CalcRecomendedSizes(_UINT32 cbSource, _UINT32 dwStep, _UINT32 *cbCore, _UINT32 *cbFlags);


private:
    void CreateMvTable(_UINT8 * charSeq, TABLE_FOR_CHAR *inTable ); // transform char sequence into table when encryption run
  //  void CreateMvTableDecode(_UINT8 * charSeq, TABLE_FOR_CHAR *inTable); // transform char sequence into table when decryption run
	int GetNextTable(bool itEnd =false); // generation number for select table from array of keys
	int CodingComp(); // coding into differents parts (round transformation);
	bool DecodingComp(long endByte);// decoding from differents parts (round transformation);
	void CloseAllFile(); // service process for end 
	int m_minStep; // minimum number of step
//	char m_WorkPath[MV2_MAX_PATH]; // directory of tempfiles
    _UINT8 m_TabBuffer[8320]; // memory buffer for array of key
	int m_CurTabNumber; // current trunsformation
    _UINT8 m_cTab[TABLE_LENGTH]; // array for current compression tab
	TABLE_FOR_CHAR m_Tab; //current table
	bool m_IsTableSet; // true if m_TabFile is set
	int m_Status;	// Status last error
    _UINT8 * m_InBuf; // working input buffer
    _UINT8 * m_InBuf1; //working second input buffer
    _UINT8 * m_OutBuf;	// working output buffer
	unsigned int m_MaxCycle; // number of maximum rounds

	_MV2ZAGL m_curZagl[3]; // headers arrays
	CVMBuf * m_inFile[2];  // tempriory files (memory buffers)
	CVMBuf * m_outFile[2]; // tempriory files (memory buffers)
//	HANDLE m_TabFile;	//  handle for sorce file of the table (old)
	CVMBuf * m_sourceFile[2];// files of source input/output (memory buffers)
	CVMBuf * m_tarFile[2]; // files of target input/output (memory buffers)
	
	CVMBuf  m_tmpBuf[5]; // memory imitation for files (memory buffers)


};// End WinClassic

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

#endif // !defined(AFX_MV2WC_H__0A5E73D6_18E1_4B20_92C4_AC2AD04A9977__INCLUDED_)
