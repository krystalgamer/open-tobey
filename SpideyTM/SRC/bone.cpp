// @TODO - need interfaces first
#include "bone.h"
#include "my_assertions.h"



// @TODO
void bone::dirty_family(bool parm)
{
	// @TODO
	typedef void (__fastcall *dirty_family_ptr)(bone*, int, bool);
	dirty_family_ptr dirty_func = (dirty_family_ptr)0x004E1570;

	dirty_func(this, 0, parm);
}

void validate_bone(void)
{
	VALIDATE(bone, my_link_interface, 0x5C);
}



#include "my_patch.h"

void patch_bone(void)
{

}
