#include "my_assertions.h"
#include <cstdio>

int FAIL_VALIDATION = 0;

void validate_i32(int cur, int expected, const char *name, int line)
{
	if (expected != cur)
	{
		FAIL_VALIDATION = 1;
		printf("[!] Expected %d but got %d: %s:%d\n", expected, cur, name, line);
	}
}

void validate_class(int cur, int expected, const char *cls, const char *member){


	if (expected != cur){

		FAIL_VALIDATION = 1;

		printf("[!] Expected %X but got %X, for %s->%s\n", expected, cur, cls, member);
	}
	else{

#ifdef _VALIDATE_SUCCESS
		printf("Success: Expected %X  for %s->%s\n", expected, cls, member);
#endif
	}

	fflush(stdout);
}

void validate_size(int cur, int expected, const char *name)
{
	if (expected != cur){
		FAIL_VALIDATION = 1;
		printf("[!] Expected %X but got %X, size of %s\n", expected, cur, name);
	}
	else{

#ifdef _VALIDATE_SUCCESS
		printf("[!] SUGG my dicgg\n");
#endif
	}

	fflush(stdout);
}

#include <stdarg.h>

int* get_thunk_address(void* first,...)
{
	va_list args;
	va_start(args, first);

	int* res = va_arg(args, int*);
	va_end(args);
	return res;
}

#pragma pack(push,1)
struct ThiscallThunk
{
	unsigned short movEax;
	unsigned char jmp;
	unsigned char modrm;
};

#pragma pack(pop)

void validate_vtable_index(
		unsigned int expected_index,
		const int* known_address,
		const char *cls,
		const char *name)
{
	const ThiscallThunk *pThunk = reinterpret_cast<const ThiscallThunk*>(known_address);

	if (pThunk->movEax != 0x018B)
	{
		FAIL_VALIDATION = 1;
		printf("[!] Expected mov eax, [ecx] at %08X - for %s->%s\n", known_address, cls, name);
		return;
	}

	if (pThunk->jmp != 0xFF)
	{
		FAIL_VALIDATION = 1;
		printf("[!] Expected jmp at %08X - for %s->%s\n", known_address, cls, name);
		return;
	}

	unsigned int cur_index = -1;
	if (pThunk->modrm == 0x60)
	{
		// 8-bit displacement
		cur_index = *reinterpret_cast<const unsigned char*>(&pThunk[1]);
	}
	else if (pThunk->modrm == 0xA0)
	{
		// 32-bit displacement
		cur_index = *reinterpret_cast<const unsigned int*>(&pThunk[1]);
	}
	else
	{
		FAIL_VALIDATION = 1;
		printf("[!] Unexpected modrm byte at %08X - for %s->%s\n", known_address, cls, name);
		return;
	}

	cur_index /= 4;
	if (cur_index != expected_index)
	{
		FAIL_VALIDATION = 1;

		printf("[!] Invalid index for %s->%s, expected %u but got %u - %08X\n", cls, name, expected_index, cur_index, known_address);
	}


	fflush(stdout);
}

void validate_val(int a, int b, const char* name)
{
	if (a != b)
	{
		printf("[!] Different value for %s, got %d expected %d\n", name, a, b);
		fflush(stdout);
	}
}
