#include <memory>

#include "w32_errmsg.h"

int my_atexit(
   void (__cdecl *func )( void )
)
{
	return atexit(func);
}

void *my_malloc(size_t size)
{
	//printf("WHAT - ");
	void* res = malloc(size);
	///puts("WHAT2");
	return res;
}

void my_free(void* block)
{
	//printf("WHY - %08X", block);
	free(block);
	//puts(" - WHY2");
}

void *operator new(size_t size)
{
	return my_malloc(size);
}

void *operator new[](size_t size)
{
	return my_malloc(size);
}

void *operator new(size_t size, unsigned int flags, const char *descript, int line )
{
	return my_malloc(size);
}

void *operator new[](size_t size, unsigned int flags, const char *descript, int line )
{
	return my_malloc(size);
}

void operator delete( void* block )
{
	my_free(block);
}

void operator delete[]( void* block )
{
	my_free(block);
}

// @Ok
// @Note: not matching, don't care there's extra debug stuff that polutes the
// disasm but is not relevant
void *arch_malloc(size_t size)
{
	static unsigned int counter = 0;
	counter++;

	void *result = malloc(size);
	if (!result)
	{
		error("MALLOC FAILED!");
	}

	return result;
}

void arch_free(void *ptr)
{
	free(ptr);
}

#include "..\my_patch.h"

void patch_alloc(void)
{
	PATCH_PUSH_RET(0x0086EEB0, my_free);
	PATCH_PUSH_RET(0x0086EF99, my_malloc);

	PATCH_PUSH_RET(0x0086C4E8, my_atexit);
}
