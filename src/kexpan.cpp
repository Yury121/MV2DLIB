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

static const _UINT32 t[8] = { 0x726a8f3b, 0xe69a3b5c, 0xd3c71fe5, 0xab3c73d2, 0x4d3a8eb3, 0x0396d6e8, 0x3d4c2f7a, 0x9ee27cf3 };

 inline _UINT32 lcshft(_UINT32 dw, int i){ // left cycle shift
	return ((dw >> (32-i))|(dw<<i));
}
 inline _UINT32 g_main(_UINT32 x3, _UINT32 x2, _UINT32 x1, _UINT32 x0, _UINT32 t){
	return ((((x3^x2)&x1)^x2) + t + x0);
}
inline  _UINT32 g1(_UINT32 x2, _UINT32 x1, _UINT32 x0){
	return ((x2&x1)^(x1&x0)^(x0&x2));
}

// keyexpan_b
void kexpan512B(_UINT32 * key, _UINT8 * Table){
//	HANDLE hFile = INVALID_HANDLE_VALUE;
    int i,j;
	_UINT8 tmp;
	_UINT32 kx[256];
	_UINT8   * p;//[32*256]; // pointer to table
	p = Table; 	//p[1] = p[0] + 260; 	p[2] = p[1] + 260; 	p[3] = p[2] + 260;
	_UINT32 s[16];
	_UINT32 k,x, dwWrite;
	k =	x = dwWrite = 0;
	memset((_UINT8 *)kx,0, 256*sizeof(_UINT32));
	memset((_UINT8 *)s, 0, 16*sizeof(_UINT32));
	memset(Table, 0, 260*32);
	// preparing
	for (i = 0; i < 16; i++ ){ //0
		kx[i] = key[i]; //s[i] = 0;
//		p[0][i] = p[1][i] = p[2][i] = p[3][i] = i;
	}//0
	for (i=16; i< 256; i++){//0
		s[0] = lcshft ( g_main(kx[i-4],kx[i-3],kx[i-2],kx[i-1], t[0]),3);
		s[1] = lcshft ( g_main(kx[i-8],kx[i-7],kx[i-6],kx[i-5], t[1]),3);
		s[2] = lcshft ( g_main(kx[i-12],kx[i-11],kx[i-10],kx[i-9], t[2]),3);
		s[3] = lcshft ( g_main(kx[i-16],kx[i-15],kx[i-14],kx[i-13], t[3]),3);
		s[4] = lcshft ( g_main(kx[i-6],kx[i-5],kx[i-4],kx[i-3], t[4]),13);
		s[5] = lcshft ( g_main(kx[i-10],kx[i-9],kx[i-8],kx[i-7], t[5]),13);
		s[6] = lcshft ( g_main(kx[i-14],kx[i-13],kx[i-12],kx[i-11], t[6]),13);
		s[7] = lcshft ( g_main(kx[i-16],kx[i-15],kx[i-2],kx[i-1], t[7]),13);

		s[8] = lcshft ( g1(s[2],s[3], s[4]),2 );
		s[9] = lcshft ( g1(s[0],s[5], s[6]),11 );
		s[10]= lcshft ( g1(s[1],lcshft(s[2],14), s[7]),13 );
		s[11]= lcshft ( g1( lcshft(s[3],6),lcshft(s[4],4), lcshft( s[5],12)),9 );
		s[12]= lcshft ( g1(lcshft(s[0],7),lcshft(s[1],17), lcshft(s[6],20)),3 );
		s[13]= lcshft ( g1(lcshft(s[2],10),lcshft(s[4],12), lcshft(s[7],16)),7 );
		s[14]= lcshft ( g1(lcshft(s[0],13),lcshft(s[3],7), lcshft(s[5],11)),16 );
		s[15]= lcshft ( g1(lcshft(s[1],5),lcshft(s[6],12), lcshft(s[7],10)),5 );

		s[3] = s[8]^s[9]^s[10]^s[11]^s[12]^s[13]^s[14]^s[15];
		s[3] = s[3] + lcshft(s[3],11);
		kx[i] = s[3]^lcshft(s[3],5);
		
	}//0
	for (i=0; i< 256; i++){//0
		kx[i] = kx[i]^(kx[(i+23)&0x000000ff] + kx[kx[(i+109)&0x000000ff]&0x000000ff]);
		for (j = 0; j<32; j++)	p[j*260+i] = i;
	}//0
	//permutation
	x = 0;
	for (i=255; i >0; i--){//0
		x = kx[i] + lcshft(kx[(i+113)&0x000000ff],11);
		for (j=0; j < 32; j++){//1
			k = (x&0x000000ff)%(i+1);

//			hMem.Add(k);

			tmp = p[j*260+k];//[j][k];
			p[j*260+k] = p[j*260+i]; 
			p[j*260+i] = tmp;
			//x = x*(x>>1) + (x>>17);
			s[0] = lcshft( g1(s[0],s[2],kx[(i+j+47)&0x000000ff]), (j+3)&0x0000000f);
			s[7] = lcshft( g1(s[7],s[1],kx[(i+j+59)&0x000000ff]), (j+3)&0x0000000f);
			s[4] = lcshft( g1(s[4],s[3],kx[(i+j+67)&0x000000ff]), (j+3)&0x0000000f);
			s[6] = lcshft( g1(s[6],s[5],kx[(i+j+73)&0x000000ff]), (j+3)&0x0000000f);
			s[2] = lcshft( g1(s[2],s[1],kx[(i+j+83)&0x000000ff]), (j+3)&0x0000000f);
			s[3] = lcshft( g1(s[3],s[5],kx[(i+j+97)&0x000000ff]), (j+3)&0x0000000f);
			s[5] = lcshft( g1(s[5],s[1],kx[(i+j+103)&0x000000ff]), (j+3)&0x0000000f);
			s[1] = lcshft( g1(s[1],s[0],kx[(i+j+109)&0x000000ff]), (j+3)&0x0000000f);
			s[3] -= s[0];
			s[6] += s[2];
			s[7] = s[7]^s[5];
			s[1] -= s[4];
			s[0] -= s[7];
			s[4] = s[4]^s[6];
			s[2] += s[3];
			s[5] = s[3]+s[1];
			s[1] = lcshft(s[7],11);
			s[3] = s[3]^lcshft(s[6],17);
			s[2] = s[2] - lcshft(s[5],13);
			s[0] += lcshft(s[4], (s[3]^0x0000000f));
			s[1] = s[1]^s[3];
			s[0] = s[0]^lcshft(s[2],5);
			x = (x*(x>>1)+(x>>17))^lcshft(kx[(i+j+127)&0x000000ff]&0x000000ff,j&0x0000000f)^(s[1]+s[0]);
		}//end for j//1
	}//end for i;//0
}
#if 0 // change
/// KeyExpantion 256

void kexpan256(_UINT32 * key, _UINT8 * Table){
//	HANDLE hFile = INVALID_HANDLE_VALUE;
    int i,j;
	_UINT8 tmp;
	_UINT32 kx[256];
	_UINT8   * p;//[32*256]; // pointer to table
	p = Table; 	//p[1] = p[0] + 260; 	p[2] = p[1] + 260; 	p[3] = p[2] + 260;
	_UINT32 s[16];
	_UINT32 k,x, dwWrite;
	k =	x = dwWrite = 0;
	memset((_UINT8 *)kx,0, 256*sizeof(_UINT32));
	memset((_UINT8 *)s, 0, 16*sizeof(_UINT32));
	memset(Table, 0, 260*32);
	// preparing
	for (i = 0; i < 8; i++ ){ //0
		kx[i] = key[i]; //s[i] = 0;
//		p[0][i] = p[1][i] = p[2][i] = p[3][i] = i;
	}//0
	for (i=8; i< 256; i++){//0
		s[0] = lcshft ( g_main(kx[i-4],kx[i-3],kx[i-2],kx[i-1], t[0]),13);
		s[1] = lcshft ( g_main(kx[i-8],kx[i-7],kx[i-6],kx[i-5], t[1]),13);
		s[2] = lcshft ( g_main(kx[i-6],kx[i-5],kx[i-4],kx[i-3], t[2]),13);
		s[3] = lcshft ( g_main(kx[i-1],kx[i-2],kx[i-8],kx[i-7], t[3]),13);
		s[4] = lcshft ( g_main(kx[i-8],kx[i-6],kx[i-4],kx[i-2], t[4]),13);
		s[5] = lcshft ( g_main(kx[i-7],kx[i-5],kx[i-3],kx[i-1], t[5]),7);
		s[6] = t[6] + ((kx[i-7]&kx[i-4])^(kx[i-6]&kx[i-2])^(kx[i-5]&kx[i-3])^(kx[i-2]&kx[i-1])^kx[i-1]);
        s[7] = t[7] + ( ((( kx[i-8]&kx[i-6]& kx[i-4]&kx[i-2])^(kx[i-8]&kx[i-7] )^( kx[i-6]&kx[i-5] )^( kx[i-5]&kx[i-4] )^kx[i-3] )&kx[i-2])^kx[i-4]);
		s[8] = lcshft ( g1(s[2],s[3], s[4]),2 );
		s[9] = lcshft ( g1(s[0],s[5], s[6]),11 );
		s[10]= lcshft ( g1(s[1],lcshft(s[2],14), s[7]),13 );
		s[11]= lcshft ( g1( lcshft(s[3],6),lcshft(s[4],4), lcshft( s[5],12)),9 );
		s[12]= lcshft ( g1(lcshft(s[0],7),lcshft(s[1],17), lcshft(s[6],20)),3 );
		s[13]= lcshft ( g1(lcshft(s[2],10),lcshft(s[4],12), lcshft(s[7],16)),7 );
		s[14]= lcshft ( g1(lcshft(s[0],13),lcshft(s[3],7), lcshft(s[5],11)),16 );
		s[15]= lcshft ( g1(lcshft(s[1],5),lcshft(s[6],12), lcshft(s[7],10)),5 );

		s[3] = s[8]^s[9]^s[10]^s[11]^s[12]^s[13]^s[14]^s[15];
		s[3] += lcshft(s[3],11);
		kx[i] = s[3]^lcshft(s[3],5);
		
	}//0
	for (i=0; i< 256; i++){//0
		kx[i] = kx[i]^(kx[(i+23)&0x000000ff] + kx[kx[(i+109)&0x000000ff]&0x000000ff]);
		for (j = 0; j<32; j++)	p[j*260+i] = i;
	}//0
	x = 0;
	for (i=255; i >0; i--){//0
		x = kx[i] + lcshft(kx[(i+113)&0x000000ff],11);
		for (j=0; j < 32; j++){//1
			k = (x&0x000000ff)%(i+1);
//			hMem.Add(k);
			tmp = p[j*260+k];//[j][k];
			p[j*260+k] = p[j*260+i]; 
			p[j*260+i] = tmp;
			//x = x*(x>>1) + (x>>17);
			s[0] = lcshft( g1(s[0],s[2],kx[(i+j+47)&0x000000ff]), (j+3)&0x0000000f);
			s[7] = lcshft( g1(s[7],s[1],kx[(i+j+59)&0x000000ff]), (j+3)&0x0000000f);
			s[4] = lcshft( g1(s[4],s[3],kx[(i+j+67)&0x000000ff]), (j+3)&0x0000000f);
			s[6] = lcshft( g1(s[6],s[5],kx[(i+j+73)&0x000000ff]), (j+3)&0x0000000f);
			s[2] = lcshft( g1(s[2],s[1],kx[(i+j+83)&0x000000ff]), (j+3)&0x0000000f);
			s[3] = lcshft( g1(s[3],s[5],kx[(i+j+97)&0x000000ff]), (j+3)&0x0000000f);
			s[5] = lcshft( g1(s[5],s[1],kx[(i+j+103)&0x000000ff]), (j+3)&0x0000000f);
			s[1] = lcshft( g1(s[1],s[0],kx[(i+j+109)&0x000000ff]), (j+3)&0x0000000f);
#if 0 //??????????????????????
			s[3] = s[3] - s[0];
			s[6] = s[6] + s[2];
			s[7] = s[7]^s[5];
			s[1] = s[1] - s[4];
			s[0] = s[0] - s[7];
			s[4] = s[4]^s[6];
			s[2] += s[3];
			s[5] = s[3]+s[1];
			s[3] -= s[0];
#endif
			s[3] -= s[0];
			s[6] += s[2];
			s[7] = s[7]^s[5];
			s[1] -= s[4];
			s[0] -= s[7];
			s[4] = s[4]^s[6];
			s[2] += s[3];
			s[5] = s[3]+s[1];
			s[1] = lcshft(s[7],11);
			s[3] = s[3]^lcshft(s[6],17);
			s[2] = s[2] - lcshft(s[5],13);
			s[0] += lcshft(s[4], (s[3]^0x0000000f));
			s[1] = s[1]^s[3];
			s[0] = s[0]^lcshft(s[2],5);
			x = (x*(x>>1)+(x>>17))^lcshft(kx[(i+j+127)&0x000000ff]&0x000000ff,j&0x0000000f)^(s[1]+s[0]);
		}//end for j//1
	}//end for i;//0

}
#endif

/// KeyExpantion 256

void kexpan256(_UINT32 * key, _UINT8 * Table){
    int i,j;
    _UINT8 tmp;
    _UINT32 kx[256];
    _UINT8   * p;//[32*256]; // pointer to table
    p = Table; 	//p[1] = p[0] + 260; 	p[2] = p[1] + 260; 	p[3] = p[2] + 260;
    _UINT32 s[16];
    _UINT32 k,x, dwWrite;
    k =	x = dwWrite = 0;
    memset((_UINT8 *)kx,0, 256*sizeof(_UINT32));
    memset((_UINT8 *)s, 0, 16*sizeof(_UINT32));
    memset(Table, 0, 260*32);
    // preparing
    for (i = 0; i < 8; i++ ){ //0
        kx[i] = key[i]; //s[i] = 0;
//		p[0][i] = p[1][i] = p[2][i] = p[3][i] = i;
    }//0
    for (i=8; i< 256; i++){//0
        s[0] = lcshft ( g_main(kx[i-4],kx[i-3],kx[i-2],kx[i-1], t[0]),13);
        s[1] = lcshft ( g_main(kx[i-8],kx[i-7],kx[i-6],kx[i-5], t[1]),13);
        s[2] = lcshft ( g_main(kx[i-6],kx[i-5],kx[i-4],kx[i-3], t[2]),13);
        s[3] = lcshft ( g_main(kx[i-1],kx[i-2],kx[i-8],kx[i-7], t[3]),13);
        s[4] = lcshft ( g_main(kx[i-8],kx[i-6],kx[i-4],kx[i-2], t[4]),13);
        s[5] = lcshft ( g_main(kx[i-7],kx[i-5],kx[i-3],kx[i-1], t[5]),7);
        s[6] = t[6] + ((kx[i-7]&kx[i-4])^(kx[i-6]&kx[i-2])^(kx[i-5]&kx[i-3])^(kx[i-2]&kx[i-1])^kx[i-1]);
        s[7] = t[7] + ((kx[i-8]&kx[i-6]&kx[i-4]&kx[i-2]^(kx[i-8]&kx[i-7])^(kx[i-6]&kx[i-5])^(kx[i-5]&kx[i-4])^kx[i-3]&kx[i-2])^kx[i-4]);
        s[8] = lcshft ( g1(s[2],s[3], s[4]),2 );
        s[9] = lcshft ( g1(s[0],s[5], s[6]),11 );
        s[10]= lcshft ( g1(s[1],lcshft(s[2],14), s[7]),13 );
        s[11]= lcshft ( g1( lcshft(s[3],6),lcshft(s[4],4), lcshft( s[5],12)),9 );
        s[12]= lcshft ( g1(lcshft(s[0],7),lcshft(s[1],17), lcshft(s[6],20)),3 );
        s[13]= lcshft ( g1(lcshft(s[2],10),lcshft(s[4],12), lcshft(s[7],16)),7 );
        s[14]= lcshft ( g1(lcshft(s[0],13),lcshft(s[3],7), lcshft(s[5],11)),16 );
        s[15]= lcshft ( g1(lcshft(s[1],5),lcshft(s[6],12), lcshft(s[7],10)),5 );

        s[3] = s[8]^s[9]^s[10]^s[11]^s[12]^s[13]^s[14]^s[15];
        s[3] += lcshft(s[3],11);
        kx[i] = s[3]^lcshft(s[3],5);

    }//0
    for (i=0; i< 256; i++){//0
        kx[i] = kx[i]^(kx[(i+23)&0x000000ff] + kx[kx[(i+109)&0x000000ff]&0x000000ff]);
        for (j = 0; j<32; j++)	p[j*260+i] = i;
    }//0
    x = 0;
    for (i=255; i >0; i--){//0
        x = kx[i] + lcshft(kx[(i+113)&0x000000ff],11);
        for (j=0; j < 32; j++){//1
            k = (x&0x000000ff)%(i+1);
//			hMem.Add(k);
            tmp = p[j*260+k];//[j][k];
            p[j*260+k] = p[j*260+i];
            p[j*260+i] = tmp;
            //x = x*(x>>1) + (x>>17);
            s[0] = lcshft( g1(s[0],s[2],kx[(i+j+47)&0x000000ff]), (j+3)&0x0000000f);
            s[7] = lcshft( g1(s[7],s[1],kx[(i+j+59)&0x000000ff]), (j+3)&0x0000000f);
            s[4] = lcshft( g1(s[4],s[3],kx[(i+j+67)&0x000000ff]), (j+3)&0x0000000f);
            s[6] = lcshft( g1(s[6],s[5],kx[(i+j+73)&0x000000ff]), (j+3)&0x0000000f);
            s[2] = lcshft( g1(s[2],s[1],kx[(i+j+83)&0x000000ff]), (j+3)&0x0000000f);
            s[3] = lcshft( g1(s[3],s[5],kx[(i+j+97)&0x000000ff]), (j+3)&0x0000000f);
            s[5] = lcshft( g1(s[5],s[1],kx[(i+j+103)&0x000000ff]), (j+3)&0x0000000f);
            s[1] = lcshft( g1(s[1],s[0],kx[(i+j+109)&0x000000ff]), (j+3)&0x0000000f);
#if 0 //??????????????????????
            s[3] = s[3] - s[0];
            s[6] = s[6] + s[2];
            s[7] = s[7]^s[5];
            s[1] = s[1] - s[4];
            s[0] = s[0] - s[7];
            s[4] = s[4]^s[6];
            s[2] += s[3];
            s[5] = s[3]+s[1];
            s[3] -= s[0];
#endif
            s[3] -= s[0];
            s[6] += s[2];
            s[7] = s[7]^s[5];
            s[1] -= s[4];
            s[0] -= s[7];
            s[4] = s[4]^s[6];
            s[2] += s[3];
            s[5] = s[3]+s[1];
            s[1] = lcshft(s[7],11);
            s[3] = s[3]^lcshft(s[6],17);
            s[2] = s[2] - lcshft(s[5],13);
            s[0] += lcshft(s[4], (s[3]^0x0000000f));
            s[1] = s[1]^s[3];
            s[0] = s[0]^lcshft(s[2],5);
            x = (x*(x>>1)+(x>>17))^lcshft(kx[(i+j+127)&0x000000ff]&0x000000ff,j&0x0000000f)^(s[1]+s[0]);
        }//end for j//1
    }//end for i;//0

}


// keyexpansion 1024
void kexpan1024(_UINT32 * key, _UINT8 * Table){
//	HANDLE hFile = INVALID_HANDLE_VALUE;
    int i,j;
	_UINT8 tmp;
	_UINT32 kx[256];
	_UINT8   * p;//[32*256]; // pointer to table
	p = Table; 	//p[1] = p[0] + 260; 	p[2] = p[1] + 260; 	p[3] = p[2] + 260;
	_UINT32 s[16];
	_UINT32 k,x, dwWrite;
	k =	x = dwWrite = 0;
	memset((_UINT8 *)kx,0, 256*sizeof(_UINT32));
	memset((_UINT8 *)s, 0, 16*sizeof(_UINT32));
	memset(Table, 0, 260*32);
	// preparing
	for (i = 0; i < 32; i++ ){ //0
		kx[i] = key[i];// s[i] = 0;
//		p[0][i] = p[1][i] = p[2][i] = p[3][i] = i;
	}//0
	for (i=32; i< 256; i++){//0
		s[0] = lcshft ( g_main(kx[i-4],kx[i-3],kx[i-2],kx[i-1], t[0]),3);
		s[1] = lcshft ( g_main(kx[i-8],kx[i-7],kx[i-6],kx[i-5], t[1]),3);
		s[2] = lcshft ( g_main(kx[i-12],kx[i-11],kx[i-10],kx[i-9], t[2]),3);
		s[3] = lcshft ( g_main(kx[i-16],kx[i-15],kx[i-14],kx[i-13], t[3]),3);
		s[4] = lcshft ( g_main(kx[i-6],kx[i-5],kx[i-4],kx[i-3], t[4]),13);
//		s[5] = lcshft ( g_main(kx[i-10],kx[i-9],kx[i-8],kx[i-8], t[5]),13);
//		s[6] = lcshft ( g_main(kx[i-14],kx[i-13],kx[i-12],kx[i-12], t[6]),13);
		s[5] = lcshft ( g_main(kx[i-10],kx[i-9],kx[i-8],kx[i-7], t[5]),13);
		s[6] = lcshft ( g_main(kx[i-14],kx[i-13],kx[i-12],kx[i-11], t[6]),13);

		s[7] = lcshft ( g_main(kx[i-16],kx[i-15],kx[i-2],kx[i-1], t[7]),13);
	
		s[14] = g1(kx[i-14],kx[i-13], kx[i-12]);

		s[8] = lcshft( g1(kx[i-32],kx[i-31], kx[i-30]) + g1(s[2],s[3],s[4]), 2);
		s[9] = lcshft( g1(kx[i-29],kx[i-28], kx[i-27]) + g1(s[0],s[3],s[4]), 11);
		s[10]= lcshft ( g1(kx[i-26],kx[i-25],kx[i-24])+ g1(s[1], lcshft(s[2],14), s[7]), 13);
		s[11]= lcshft ( g1(kx[i-23],kx[i-22],kx[i-21])+ g1(lcshft(s[3],6), lcshft(s[4],4),lcshft(s[5],12) ), 9);
		s[12]= lcshft ( g1(kx[i-20],kx[i-19],kx[i-18])+ g1(lcshft(s[0],7), lcshft(s[1],17),lcshft(s[6],20) ), 3);
		s[13]= lcshft ( g1(kx[i-17],kx[i-32],kx[i-21])+ g1(lcshft(s[2],10), lcshft(s[4],12),lcshft(s[7],16) ), 7);
		s[14]= lcshft ( g1(kx[i-17],kx[i-16],kx[i-15])+ g1(lcshft(s[0],13), lcshft(s[3],7),lcshft(s[5],11) ), 16);
		s[15]= lcshft ( g1(kx[i-16],kx[i-31],kx[i-1])+ g1(lcshft(s[1],5), lcshft(s[6],12),lcshft(s[7],10) ), 5);

		s[3] = s[8]^s[9]^s[10]^s[11]^s[12]^s[13]^s[14]^s[15];
		s[3] = s[3] + lcshft(s[3],11);
		kx[i] = s[3]^lcshft(s[3],5);
		
	}//0
	for (i=0; i< 256; i++){//0
		kx[i] = kx[i]^(kx[(i+23)&0x000000ff] + kx[kx[(i+109)&0x000000ff]&0x000000ff]);
		for (j = 0; j<32; j++)	p[j*260+i] = i;
	}//0
	x = 0;
	for (i=255; i >0; i--){//0
		x = kx[i] + lcshft(kx[(i+113)&0x000000ff],11);
		for (j=0; j < 32; j++){//1
			k = (x&0x000000ff)%(i+1);
//			hMem.Add(k);
			tmp = p[j*260+k];//[j][k];
			p[j*260+k] = p[j*260+i]; 
			p[j*260+i] = tmp;
			//x = x*(x>>1) + (x>>17);
			s[0] = lcshft( g1(s[0],s[2],kx[(i+j+47)&0x000000ff]), (j+3)&0x0000000f);
			s[7] = lcshft( g1(s[7],s[1],kx[(i+j+59)&0x000000ff]), (j+3)&0x0000000f);
			s[4] = lcshft( g1(s[4],s[3],kx[(i+j+67)&0x000000ff]), (j+3)&0x0000000f);
			s[6] = lcshft( g1(s[6],s[5],kx[(i+j+73)&0x000000ff]), (j+3)&0x0000000f);
			s[2] = lcshft( g1(s[2],s[1],kx[(i+j+83)&0x000000ff]), (j+3)&0x0000000f);
			s[3] = lcshft( g1(s[3],s[5],kx[(i+j+97)&0x000000ff]), (j+3)&0x0000000f);
			s[5] = lcshft( g1(s[5],s[1],kx[(i+j+103)&0x000000ff]), (j+3)&0x0000000f);
			s[1] = lcshft( g1(s[1],s[0],kx[(i+j+109)&0x000000ff]), (j+3)&0x0000000f);
			s[3] = s[3] - s[0];
			s[6] = s[6] + s[2];
			s[7] = s[7]^s[5];
			s[1] = s[1] - s[4];
			s[0] = s[0] - s[7];
			s[4] = s[4]^s[6];
			s[2] += s[3];
			s[5] = s[3]+s[1];
			
//			s[3] -= s[0];
			s[1] = lcshft(s[7],11);
			s[3] = s[3]^lcshft(s[6],17);
			s[2] = s[2] - lcshft(s[5],13);
			s[0] += lcshft(s[4], (s[3]^0x0000000f));
			s[1] = s[1]^s[3];
			s[0] = s[0]^lcshft(s[2],5);
			x = (x*(x>>1)+(x>>17))^lcshft(kx[(i+j+127)&0x000000ff]&0x000000ff,j&0x0000000f)^(s[1]+s[0]);
		}//end for j//1
	}//end for i;//0

}

//2048

void kexpan2048(_UINT32 * key, _UINT8 * Table){
//	HANDLE hFile = INVALID_HANDLE_VALUE;
    int i,j;
	_UINT8 tmp;
	_UINT32 kx[256];
	_UINT8   * p;//[32*256]; // pointer to table
	p = Table; 	//p[1] = p[0] + 260; 	p[2] = p[1] + 260; 	p[3] = p[2] + 260;
	_UINT32 s[16];
	_UINT32 k,x, dwWrite;
	k =	x = dwWrite = 0;
	memset((_UINT8 *)kx,0, 256*sizeof(_UINT32));
	memset((_UINT8 *)s, 0, 16*sizeof(_UINT32));
	memset(Table, 0, 260*32);
	// preparing
	for (i = 0; i < 64; i++ ){ //0
		kx[i] = key[i];// s[i] = 0;
//		p[0][i] = p[1][i] = p[2][i] = p[3][i] = i;
	}//0
	for (i=64; i< 256; i++){//0
		
		s[0] = lcshft ( (kx[i-5]^kx[i-4]^kx[i-3]^kx[i-2])+t[0]+kx[i-1],3);
		s[1] = lcshft ( (kx[i-10]^kx[i-9]^kx[i-8]^kx[i-7])+t[1]+kx[i-6],3);
		s[2] = lcshft ( (kx[i-15]^kx[i-14]^kx[i-13]^kx[i-12])+t[2]+kx[i-11],3);
		s[3] = lcshft ( (kx[i-20]^kx[i-19]^kx[i-18]^kx[i-17])+t[3]+kx[i-16],3);
		s[4] = lcshft ( (kx[i-25]^kx[i-24]^kx[i-23]^kx[i-22])+t[4]+kx[i-21],13);
		s[5] = lcshft ( (kx[i-30]^kx[i-29]^kx[i-28]^kx[i-27])+t[5]+kx[i-26],13);
		s[6] = lcshft ( (kx[i-35]^kx[i-34]^kx[i-33]^kx[i-32])+t[6]+kx[i-31],13);
		s[7] = lcshft ( (kx[i-40]^kx[i-39]^kx[i-38]^kx[i-37])+t[7]+kx[i-36],13);
	
		s[8] = lcshft ( g1(kx[i-43],kx[i-42],kx[i-41])+g1(s[2],s[3],s[4]),2);
		s[9] = lcshft ( g1(kx[i-46],kx[i-45],kx[i-44])+g1(s[0],s[5],s[6]),11);
		s[10]= lcshft ( g1(kx[i-49],kx[i-48],kx[i-47])+ g1(s[1], lcshft(s[2],14), s[7]), 13);
		s[11]= lcshft ( g1(kx[i-52],kx[i-51],kx[i-50])+ g1(lcshft(s[3],6), lcshft(s[4],4),lcshft(s[5],12) ), 9);
		s[12]= lcshft ( g1(kx[i-55],kx[i-54],kx[i-53])+ g1(lcshft(s[0],7), lcshft(s[1],17),lcshft(s[6],20) ), 3);
		s[13]= lcshft ( g1(kx[i-58],kx[i-57],kx[i-56])+ g1(lcshft(s[2],10), lcshft(s[4],12),lcshft(s[7],16) ), 7);
		s[14]= lcshft ( g1(kx[i-61],kx[i-60],kx[i-59])+ g1(lcshft(s[0],13), lcshft(s[3],7),lcshft(s[5],11) ), 16);
		s[15]= lcshft ( g1(kx[i-64],kx[i-63],kx[i-62])+ g1(lcshft(s[1],5), lcshft(s[6],12),lcshft(s[7],10) ), 5);

		s[3] = s[8]^s[9]^s[10]^s[11]^s[12]^s[13]^s[14]^s[15];
		s[3] = s[3] + lcshft(s[3],11);
		kx[i] = s[3]^lcshft(s[3],5);
		
	}//0
	for (i=0; i< 256; i++){//0
		kx[i] = kx[i]^(kx[(i+23)&0x000000ff] + kx[kx[(i+109)&0x000000ff]&0x000000ff]);
		for (j = 0; j<32; j++)	p[j*260+i] = i;
	}//0
	x = 0;
	for (i=255; i >0; i--){//0
		x = kx[i] + lcshft(kx[(i+113)&0x000000ff],11);
		for (j=0; j < 32; j++){//1
			k = (x&0x000000ff)%(i+1);
//			hMem.Add(k);
			tmp = p[j*260+k];//[j][k];
			p[j*260+k] = p[j*260+i]; 
			p[j*260+i] = tmp;
			//x = x*(x>>1) + (x>>17);
			s[0] = lcshft( g1(s[0],s[2],kx[(i+j+47)&0x000000ff]), (j+3)&0x0000000f);
			s[7] = lcshft( g1(s[7],s[1],kx[(i+j+59)&0x000000ff]), (j+3)&0x0000000f);
			s[4] = lcshft( g1(s[4],s[3],kx[(i+j+67)&0x000000ff]), (j+3)&0x0000000f);
			s[6] = lcshft( g1(s[6],s[5],kx[(i+j+73)&0x000000ff]), (j+3)&0x0000000f);
			s[2] = lcshft( g1(s[2],s[1],kx[(i+j+83)&0x000000ff]), (j+3)&0x0000000f);
			s[3] = lcshft( g1(s[3],s[5],kx[(i+j+97)&0x000000ff]), (j+3)&0x0000000f);
			s[5] = lcshft( g1(s[5],s[1],kx[(i+j+103)&0x000000ff]), (j+3)&0x0000000f);
			s[1] = lcshft( g1(s[1],s[0],kx[(i+j+109)&0x000000ff]), (j+3)&0x0000000f);
			s[3] = s[3] - s[0];
			s[6] = s[6] + s[2];
			s[7] = s[7]^s[5];
			s[1] = s[1] - s[4];
			s[0] = s[0] - s[7];
			s[4] = s[4]^s[6];
			s[2] += s[3];
			s[5] = s[3]+s[1];
			
//			s[3] -= s[0];
			s[1] = lcshft(s[7],11);
			s[3] = s[3]^lcshft(s[6],17);
			s[2] = s[2] - lcshft(s[5],13);
			s[0] += lcshft(s[4], (s[3]^0x0000000f));
			s[1] = s[1]^s[3];
			s[0] = s[0]^lcshft(s[2],5);
			x = (x*(x>>1)+(x>>17))^lcshft(kx[(i+j+127)&0x000000ff]&0x000000ff,j&0x0000000f)^(s[1]+s[0]);
		}//end for j//1
	}//end for i;//0

}
/// KeyExpation 128
void kexpan128(_UINT32 * key, _UINT8 * Table){
//	HANDLE hFile = INVALID_HANDLE_VALUE;
	int i,j;
	_UINT8 tmp;
	_UINT32 kx[256];
	_UINT8   * p;//[32*256]; // pointer to table
	p = Table; 	//p[1] = p[0] + 260; 	p[2] = p[1] + 260; 	p[3] = p[2] + 260;
	_UINT32 s[16];
	_UINT32 k,x, dwWrite;
	k =	x = dwWrite = 0;
	memset((_UINT8 *)kx,0, 256*sizeof(_UINT32));
	memset((_UINT8 *)s, 0, 16*sizeof(_UINT32));
	memset(Table, 0, 260*32);
	// preparing
	for (i = 0; i < 4; i++ ){ //0
		kx[i] = key[i]; 
//		p[0][i] = p[1][i] = p[2][i] = p[3][i] = i;
	}//0
	for (i=4; i< 256; i++){//0
		s[0] = g1(kx[i-1],kx[i-2], t[0]);
		s[1] = g1(lcshft (kx[i-1],3),kx[i-3], t[1]);
		s[2] = g1(lcshft (kx[i-1],7),kx[i-4], t[2]);
		s[3] = g1(lcshft (kx[i-2],3),lcshft(kx[i-3],3), t[3]);
		s[4] = g1(lcshft (kx[i-2],7),lcshft(kx[i-4],3), t[4]);
		s[5] = g1(lcshft (kx[i-3],7),lcshft(kx[i-4],7), t[5]);
		s[6] = g1( kx[i-1],lcshft(kx[i-2],11), lcshft(kx[i-3],17) ) + lcshft(t[6],(s[0]+s[1]+s[2])&0x000000ff);
        s[7] = g1( lcshft(kx[i-1],2), lcshft(kx[i-2],17), kx[i-4] ) + lcshft(t[6],(s[3]+s[4]+s[5])&0x000000ff);
		s[8] = lcshft ( g1(s[2],s[3], s[4]),2 );
		s[9] = lcshft ( g1(s[0],s[5], s[6]),11 );
		s[10]= lcshft ( g1(s[1],lcshft(s[2],14), s[7]),13 );
		s[11]= lcshft ( g1( lcshft(s[3],6),lcshft(s[4],4), lcshft( s[5],12)),9 );
		s[12]= lcshft ( g1(lcshft(s[0],7),lcshft(s[1],17), lcshft(s[6],20)),3 );
		s[13]= lcshft ( g1(lcshft(s[2],10),lcshft(s[4],12), lcshft(s[7],16)),7 );
		s[14]= lcshft ( g1(lcshft(s[0],13),lcshft(s[3],7), lcshft(s[5],11)),16 );
		s[15]= lcshft ( g1(lcshft(s[1],5),lcshft(s[6],12), lcshft(s[7],10)),5 );

		s[3] = s[8]^s[9]^s[10]^s[11]^s[12]^s[13]^s[14]^s[15];
		s[3] = s[3] + lcshft(s[3],11);
		kx[i] = s[3]^lcshft(s[3],5);
		
	}//0
	for (i=0; i< 256; i++){//0
		kx[i] = kx[i]^(kx[(i+23)&0x000000ff] + kx[kx[(i+109)&0x000000ff]&0x000000ff]);
		for (j = 0; j<32; j++)	p[j*260+i] = i;
	}//0
	x = 0;
	for (i=255; i >0; i--){//0
		x = kx[i] + lcshft(kx[(i+113)&0x000000ff],11);
		for (j=0; j < 32; j++){//1
			k = (x&0x000000ff)%(i+1);
//			hMem.Add(k);
			tmp = p[j*260+k];//[j][k];
			p[j*260+k] = p[j*260+i]; 
			p[j*260+i] = tmp;
			//x = x*(x>>1) + (x>>17);
			s[0] = lcshft( g1(s[0],s[2],kx[(i+j+47)&0x000000ff]), (j+3)&0x0000000f);
			s[7] = lcshft( g1(s[7],s[1],kx[(i+j+59)&0x000000ff]), (j+3)&0x0000000f);
			s[4] = lcshft( g1(s[4],s[3],kx[(i+j+67)&0x000000ff]), (j+3)&0x0000000f);
			s[6] = lcshft( g1(s[6],s[5],kx[(i+j+73)&0x000000ff]), (j+3)&0x0000000f);
			s[2] = lcshft( g1(s[2],s[1],kx[(i+j+83)&0x000000ff]), (j+3)&0x0000000f);
			s[3] = lcshft( g1(s[3],s[5],kx[(i+j+97)&0x000000ff]), (j+3)&0x0000000f);
			s[5] = lcshft( g1(s[5],s[1],kx[(i+j+103)&0x000000ff]), (j+3)&0x0000000f);
			s[1] = lcshft( g1(s[1],s[0],kx[(i+j+109)&0x000000ff]), (j+3)&0x0000000f);
			s[3] -= s[0];
			s[6] += s[2];
			s[7] = s[7]^s[5];
			s[1] -= s[4];
			s[0] -= s[7];
			s[4] = s[4]^s[6];
			s[2] += s[3];
			s[5] = s[3]+s[1];
			s[1] = lcshft(s[7],11);
			s[3] = s[3]^lcshft(s[6],17);
			s[2] = s[2] - lcshft(s[5],13);
			s[0] += lcshft(s[4], (s[3]^0x0000000f));
			s[1] = s[1]^s[3];
			s[0] = s[0]^lcshft(s[2],5);
			x = (x*(x>>1)+(x>>17))^lcshft(kx[(i+j+127)&0x000000ff]&0x000000ff,j&0x0000000f)^(s[1]+s[0]);
		}//end for j//1
	}//end for i;//0

}




