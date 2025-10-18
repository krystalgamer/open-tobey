#include "vm_executable.h"

typedef void (__fastcall *vm_executable_link_ptr)(vm_executable*, int, const script_manager&);

// @TODO
void vm_executable::link(const script_manager& sm)
{
	vm_executable_link_ptr vm_executable_link = (vm_executable_link_ptr)0x007E45C0;
	vm_executable_link(this, 0, sm);
}

#include "my_assertions.h"
void validate_vm_executable(void)
{
	VALIDATE(vm_executable, fullname, 0xC);

	VALIDATE(vm_executable, parms_stacksize, 0x20);

	VALIDATE(vm_executable, static_func, 0x24);

	VALIDATE(vm_executable, buffer, 0x28);
}

#include "my_patch.h"

void patch_vm_executable(void)
{
}
