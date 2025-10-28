// filespec.cpp
#include "global.h"

#include "filespec.h"

// @Ok
// @Matching
// @Patch - replace empty_string with call to constructor
void filespec::extract(const stringx & src)
{
	int cp1=src.rfind('\\');
	if (cp1!=stringx::npos)
	{
		cp1++;
		path = src.substr(0,cp1);
	}
	else
	{
		path = stringx("");
		cp1=0;
	}

	int cp2 = src.rfind('.');
	if (cp2!=stringx::npos)

	{
		name = src.substr(cp1,cp2-cp1);
		ext = src.substr(cp2,src.length()-cp2);
	}
	else
	{
		name = src.substr(cp1,src.length()-cp1+1);
		ext = stringx("");
	}
}


#include "my_assertions.h"

void validate_filespec(void)
{
	VALIDATE_SIZE(filespec, 0x18);

	VALIDATE(filespec, path, 0x0);
	VALIDATE(filespec, name, 0x8);
	VALIDATE(filespec, ext, 0x10);
}

#include "my_patch.h"

void patch_filespec(void)
{
	PATCH_PUSH_RET(0x0079BD70, filespec::extract);
}
