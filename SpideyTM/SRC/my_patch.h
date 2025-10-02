#pragma once


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
extern HMODULE bink_dll;
static inline int* get_thunk_address(void* first,...)
{
	va_list args;
	va_start(args, first);


	int* res = va_arg(args, int*);
	va_end(args);
	return res;
}

#define PATCH_PUSH_RET(addr, dest) {\
	unsigned char *tmp = (unsigned char*)(addr);\
	tmp[0] = 0x68;\
	*(int**)&tmp[1] = get_thunk_address(0, dest);\
	tmp[5] = 0xC3;\
	puts("Hooking " #dest " at " #addr);\
}

#define PATCH_PUSH_RET_POLY(addr, dest, func_mangled) {\
	unsigned char *tmp = (unsigned char*)(addr);\
	tmp[0] = 0x68;\
	*(int**)&tmp[1] = (int*)GetProcAddress(bink_dll, func_mangled);\
	tmp[5] = 0xC3;\
	puts("Hooking " #dest " at " #addr);\
}
