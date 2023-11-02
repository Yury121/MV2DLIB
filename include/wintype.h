#ifndef WINTYPE_H
#define WINTYPE_H

#include <stdlib.h>
#include <memory.h>
#include <malloc.h>
#include <time.h>
#include <chrono>

#ifndef __GNUC__
#include <Windows.h>
//#include <minwinbase.h>
#endif

#define MV2_MAX_PATH 1024


typedef unsigned int _UINT32;
typedef unsigned char _UINT8;
typedef char _INT8;
//typedef void VOID;


typedef unsigned short _UINT16;
typedef short _INT16;
typedef  unsigned long long _UINT64;
typedef  long long _INT64;

//#ifndef __GNUC__

#ifndef _max
#define _max(a,b)    (( (a)>(b) ) ? (a): (b) )
#endif

#ifndef _min
#define _min(a,b)    (( (a)<(b) ) ? (a): (b) )
#endif
//#endif


template<typename Dur>
_UINT64 GetTs() { return _UINT64(std::chrono::duration_cast<Dur>(std::chrono::system_clock::now().time_since_epoch()).count()); }



#ifndef ZeroMemory
#ifdef __GNUC__
#pragma GCC push_options
#pragma GCC optimize ("O0")
inline void ZeroMemory(void* ptr, int size) { memset(ptr, 0, size); }
#pragma GCC pop_options
//#else 
//#include <winbase.h>
#endif

inline bool IsBadReadPtr(void* ptr, int size) {
    if (ptr != 0) {
        if ((_UINT64)ptr + size > 0x00400) {
            return false;
        }
    }
    return true;
}
inline void MoveMemory(void* dest, const void* source, size_t length) {
    memcpy(dest, source, length);
}
inline bool IsBadWritePtr(void* ptr, int size) {
    return IsBadReadPtr(ptr, size);
}

#define Int64ShllMod32(a, b) (((_UINT64)(a)) << (b))
#define Int64ShraMod32(a, b) (((_INT64)(a)) >> (b))
#define Int64ShrlMod32(a, b) (((_UINT64)(a)) >> (b))


#endif // !ZeroMemory

inline _UINT32 GetTickCount0() { return _UINT32(clock()); }
inline _UINT64 GetTickCount1() {    return GetTs<std::chrono::nanoseconds>();}


#endif // WINTYPE_H
