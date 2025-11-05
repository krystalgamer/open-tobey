// @TODO - need interfaces first
#include "bone.h"
#include "my_assertions.h"



void validate_bone(void)
{
	VALIDATE(bone, my_link_interface, 0x5C);
}



#include "my_patch.h"

void patch_bone(void)
{

}
