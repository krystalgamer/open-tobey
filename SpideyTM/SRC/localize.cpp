#include "localize.h"

// @Ok
// @Matching
int localize_manager::get_language_id(void) const
{
	return 0;
}

#include "my_assertions.h"
void validate_localize_manager(void)
{
	VALIDATE_SIZE(localize_manager, 0x10);
}

#include "my_patch.h"
void patch_localize_manager(void)
{
	PATCH_PUSH_RET(0x00503A10, localize_manager::get_language_id);
}
