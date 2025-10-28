#pragma once

#ifndef MY_PATCH_H
#define MY_PATCH_H


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
extern HMODULE bink_dll;
int* get_thunk_address(void* first,...);

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
	int *varAddr = (int*)GetProcAddress(bink_dll, func_mangled);\
	if (!varAddr) { OutputDebugStringA("DEAD FOR " #func_mangled); exit(69); }\
	*(int**)&tmp[1] = varAddr;\
	tmp[5] = 0xC3;\
	puts("Hooking " #dest " at " #addr);\
}

#define NOP_MEMORY(addr, num) {\
	memset((void*)addr, 0x90909090, num);\
}

#define PATCH_CALL_ADDR(addr, dest) {\
	unsigned char *tmp = (unsigned char*)(addr);\
	tmp[0] = 0xE8;\
	int destAddr = (int)get_thunk_address(0, dest);\
	destAddr -= addr;\
	destAddr -= 5;\
	*(int*)&tmp[1] = destAddr;\
	puts("Adding call " #dest " at " #addr);\
}

#define PATCH_CALL_ADDR_POLY(addr, poly) {\
	unsigned char *tmp = (unsigned char*)(addr);\
	tmp[0] = 0xE8;\
	int destAddr = (int)GetProcAddress(bink_dll, poly);\
	destAddr -= addr;\
	destAddr -= 5;\
	*(int*)&tmp[1] = destAddr;\
	puts("Adding call " #poly " at " #addr);\
}

#endif
