#include "global.h"
#include "vm_stack.h"
#include "script_library_class.h"

// @Ok
// @Matching
vm_stack::vm_stack(int sa, vm_thread * _my_thread)
{
	assert(!(sa&3));
	salloc = sa;
	buffer = NEW char[sa];

	// @Patch - add unitialized bit
	if (sa > 0)
	{
		int stop = sa >> 2;
		assert (sa == stop*4);
		for (int i=0;i<stop;i++) ((unsigned int *)buffer)[i] = UNINITIALIZED_SCRIPT_PARM;
	}

	my_thread = _my_thread;
	SP = buffer;
}

vm_stack::~vm_stack()
{
	delete[] buffer;
}

// @Ok
// @Matching
vm_num_t vm_stack::pop_num()
{
	pop(sizeof(vm_num_t));
	return *(vm_num_t*)SP;
}


// @Ok
// @Matching
bool vm_stack::push(const char* src, int n)
{
	memcpy(SP,src,n);
	move_SP( n );
#if REPORT_OVERFLOW
  // check for stack overflow

  if ( size() > capacity() )
    return false;
#endif
  return true;
}



#include "my_assertions.h"
static void compile_time_assertions()
{
	StaticAssert<sizeof(vm_stack) == 0x10>::sass();
}

void validate_vm_stack(void)
{
	VALIDATE_SIZE(vm_stack, 0x10);

	VALIDATE(vm_stack, salloc, 0x0);
	VALIDATE(vm_stack, buffer, 0x4);
	VALIDATE(vm_stack, SP, 0x8);
	VALIDATE(vm_stack, my_thread, 0xC);
}

void patch_vm_stack(void)
{
	PATCH_PUSH_RET_POLY(0x007E5B90, vm_stack::push, "?push@vm_stack@@QAE_NPBDH@Z");

	PATCH_PUSH_RET_POLY(0x007E5AC0, vm_stack::vm_stack, "??0vm_stack@@QAE@HPAVvm_thread@@@Z");

	PATCH_PUSH_RET(0x007E5B70, vm_stack::pop_num);
}
