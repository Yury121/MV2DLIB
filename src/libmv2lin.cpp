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


#include <cstdio>
#include <memory>
#include <iostream>
#include <random>
#include <ctime>
#include <tuple>
#include <thread>
#include <mutex>
#include "../include/mvl-aes.h"

//static std::shared_ptr<MVLAES> mv2 { std::make_shared<MVLAES>() };


std::mutex	mm;
static std::vector<std::tuple<std::thread::id, MVLAES*> > vMVL;

inline  MVLAES* GetMV2Object(bool bAdd = false);

inline MVLAES* GetMV2Object(bool bAdd) {
	std::thread::id id = std::this_thread::get_id();
	MVLAES* pMv2 = nullptr;
	mm.lock();
	for (auto&& p : vMVL) {
		if (std::get<0>(p) == id) { 
			pMv2 = std::get<1>(p);
			mm.unlock();
			return pMv2;
		}
	}
	if (bAdd) {// add object;
		pMv2 = new MVLAES;
		vMVL.push_back(std::tuple<std::thread::id, MVLAES*>({ id, pMv2 }));
	}
	mm.unlock();
	return  pMv2;
}


extern "C" {
	bool MV2SetKey(unsigned char* srcKey, int szKey) {
		MVLAES* mv2 = GetMV2Object(true);
		if (mv2) return mv2->MV2SetKey(srcKey, szKey);
		else return false;
	}
	bool MV2Encrypt(MV2_BUFFER& mv2Buf, long RestLength) { // coding into differents files
		MVLAES* mv2 = GetMV2Object();
		if (mv2) return mv2->MV2Encrypt(mv2Buf, RestLength);
		else return false;
	}
	/////
	bool MV2Decrypt(MV2_BUFFER& mv2Buf) { // decoding from different files
		MVLAES* mv2 = GetMV2Object();
		if (mv2) return mv2->MV2Decrypt(mv2Buf);
		else return false;
	}

	int MV2GetStatus() {
		MVLAES* mv2 = GetMV2Object();
		if (mv2 )return mv2->MV2GetStatus();// GetCurStatus();
		else return -99;
	}

	int MV2GetStep() {
		MVLAES* mv2 = GetMV2Object();
		if (mv2) return mv2->m_CurStep;
		else return 0;
	};

	bool MV2CalcRecomendedSizes(uint32_t cbSource, uint32_t dwStep, uint32_t& cbCore, uint32_t& cbFlags) {
		MVLAES* mv2 = GetMV2Object();
		return (mv2 != nullptr) ? mv2->MV2CalcRecomendedSizes(cbSource, dwStep, &cbCore, &cbFlags) : false;
	};

	void MV2Release() {
		std::thread::id id = std::this_thread::get_id();
		mm.lock();
		for (int i = 0; i < vMVL.size(); i++) {
			if (std::get<0>(vMVL[i]) == id) {
				delete std::get<1>(vMVL[i]);
				std::get<1>(vMVL[i]) = nullptr;
				vMVL.erase(vMVL.begin() + i);
				break;
			}
		}
		mm.unlock();
	}

	bool IsAESProc() { return IsAesNIset(); }
}

//#if 0

void __attribute__((constructor)) mv2lib_init()
{
#ifndef NDEBUG		
	std::thread::id id = std::this_thread::get_id();
	printf("Id = %d", id);
	//std::cout << "Id: " << std::this_thread::get_id() << "\n";

#endif // !NDEBUG

	vMVL.reserve(20);
};

void __attribute__((destructor)) mv2lib_fini()
{
	MVLAES * pMv2 = nullptr;
	for (int i = 0; i < vMVL.size(); i++) {
		delete std::get<1>(vMVL[i]);
	}
	vMVL.clear();

};
//#endif