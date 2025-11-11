#include "pmesh.h"

// @TODO
void vr_pmesh::shrink_memory_footprint()
{
	typedef void (__fastcall *ptr)(vr_pmesh*);
	ptr func = (ptr)0x005AE3D0;

	func(this);
}

#include "my_assertions.h"

void validate_vr_pmesh(void)
{
	VALIDATE_VAL(VISREP_PMESH, 0);
}

#include "my_patch.h"

void patch_vr_pmesh(void)
{
}
