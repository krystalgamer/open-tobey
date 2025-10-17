#include "vm_executable.h"


#include "my_assertions.h"
void validate_vm_executable(void)
{
	VALIDATE(vm_executable, fullname, 0xC);

	VALIDATE(vm_executable, parms_stacksize, 0x20);
	VALIDATE(vm_executable, buffer, 0x28);
}

#include "my_patch.h"

void patch_vm_executable(void)
{
}
