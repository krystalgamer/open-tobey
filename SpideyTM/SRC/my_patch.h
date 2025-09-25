#pragma once


static inline int* get_thunk_address(void* first,...)
{
	va_list args;
	va_start(args, first);


	int* res = va_arg(args, int*);
	va_end(args);
	return res;
}

#define PATCH_PUSH_RET(addr, dest, reason) {\
	unsigned char *tmp = (unsigned char*)(addr);\
	tmp[0] = 0x68;\
	*(int**)&tmp[1] = get_thunk_address(0, dest);\
	tmp[5] = 0xC3;\
	puts("Hooking " reason " at " #addr);\
}


