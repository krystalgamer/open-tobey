#include "vm_symbol.h"

// @NotOk
// there's a SEH handler for some reason
vm_symbol::vm_symbol()
:   type_name(),
	name(),
	offset(0)
{
}

// @Ok
// @Matching
vm_symbol::vm_symbol(const vm_symbol& b)
:   type_name(b.type_name),
    name(b.name),
    offset(b.offset)
{
}


#include "my_assertions.h"

void validate_vm_symbol(void)
{
	VALIDATE_SIZE(vm_symbol, 0x14);

	VALIDATE(vm_symbol, type_name, 0x0);
	VALIDATE(vm_symbol, name, 0x8);
	VALIDATE(vm_symbol, offset, 0x10);
}

#include "my_patch.h"

void patch_vm_symbol(void)
{
	PATCH_PUSH_RET_POLY(0x007E65E0, vm_symbol::vm_symbol, "??0vm_symbol@@QAE@XZ");
	PATCH_PUSH_RET_POLY(0x007E6610, vm_symbol::vm_symbol, "??0vm_symbol@@QAE@ABV0@@Z");
}
