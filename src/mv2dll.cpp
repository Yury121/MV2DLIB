// mv2dll.cpp : Defines the entry point for the DLL application.
//


#include <Windows.h>
#include <thread>
#include <mutex>


#include "../include/mvl-aes.h"

inline  MVLAES* GetMV2Object(bool bAdd = false);



//static std::shared_ptr<MVLAES> mv2 { std::make_shared<MVLAES>() };

std::mutex	mm;
static std::vector<std::tuple<std::thread::id, MVLAES *> > vMVL;

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

//static MVLAES * mv2 = NULL;


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	bool bRet = TRUE;
	std::thread::id id = std::this_thread::get_id();
	switch (ul_reason_for_call){ 
		case DLL_PROCESS_ATTACH: 
			vMVL.reserve(20);
//			mv2 = new MVLAES;		
			//if (!mv2) bRet = false;
			break;
		case DLL_THREAD_ATTACH: 
#if 0
			mm.lock();
			if (GetMV2Object() == nullptr) {
//				std::thread::id id = std::this_thread::get_id();
				MVLAES* pMv2 = new MVLAES;
				vMVL.push_back(std::tuple<std::thread::id, MVLAES*>({ id, pMv2 }));
			}
			mm.unlock();
#endif
			break; 
		case DLL_THREAD_DETACH: 
			mm.lock();
			for (int i = 0; i < vMVL.size(); i++) {
				if (std::get<0>(vMVL[i]) == id) {
					delete std::get<1>(vMVL[i]);
					vMVL.erase(vMVL.begin() + i);
					break;
				}
			}
			mm.unlock();
            break; 
		case DLL_PROCESS_DETACH: 
			for (auto&& p : vMVL) {
				if (std::get<1>(p) != nullptr) { delete std::get<1>(p); std::get<1>(p) = nullptr; }
			}
			vMVL.clear();
		default: 
          break; 
	}

    return bRet;
}

extern "C" {

	////////////////////////////////////////////////////////////
	__declspec (dllexport) bool
		MV2SetKey(unsigned char* srcKey, int szKey) {
		MVLAES* mv2 = GetMV2Object(true);
		if (mv2) return mv2->MV2SetKey(srcKey, szKey);
		else return false;

	}

	__declspec (dllexport) bool
		MV2Encrypt(MV2_BUFFER& mv2Buf, long RestLength) { // coding into differents files
		MVLAES* mv2 = GetMV2Object();
		if (mv2) return mv2->MV2Encrypt(mv2Buf, RestLength);
		else return false;
	}
	/////
	__declspec (dllexport)
		bool
		MV2Decrypt(MV2_BUFFER& mv2Buf) { // decoding from different files
		MVLAES* mv2 = GetMV2Object();
		if (mv2) return mv2->MV2Decrypt(mv2Buf);
		else return false;
	}

	__declspec (dllexport)
		int
		MV2GetStatus() {
		MVLAES* mv2 = GetMV2Object();
		if (mv2)return mv2->MV2GetStatus();// GetCurStatus();
		else return -99;
	}

	__declspec (dllexport)
		int
		MV2GetStep() {
		MVLAES* mv2 = GetMV2Object();
		if (mv2) return mv2->m_CurStep;
		else return 0;
	}
	__declspec (dllexport)
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
	__declspec (dllexport)
		bool
		MV2CalcRecomendedSizes(DWORD cbSource, DWORD dwStep, unsigned int* cbCore, unsigned int* cbFlags) {
		MVLAES* mv2 = GetMV2Object();
		if (mv2) return mv2->MV2CalcRecomendedSizes(cbSource, dwStep, (unsigned int * )cbCore, (unsigned int*) cbFlags);
		return 0;
	}

}