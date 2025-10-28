#include "entity.h"

// @Ok
// @Matching
char* strdupcpp(const char* str)
{
  char* retstr;

  retstr = NEW char[ strlen(str)+1 ];
  strcpy( retstr, str );
  return retstr;
}

#include "my_assertions.h"
#include "my_patch.h"

void patch_str(void)
{
	PATCH_PUSH_RET(0x004EBEA0, strdupcpp);
}
