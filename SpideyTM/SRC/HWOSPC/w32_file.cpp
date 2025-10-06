#include "w32_file.h"

#define INVLALID_FP INVLAID_HANDLE_VALUE

os_file::os_file()
{
	field_10 = -1;
	flags=0;
	opened=false;
	from_cd = false;
	field_14 = 0;
}

os_file::os_file(const stringx & _name, int _flags)

{
	flags=0;
	opened=false;
	from_cd = false;

	// @Patch
	//io = INVALID_FP;
	open(_name,_flags);

}



/*** destructor ***/
os_file::~os_file()
{
	if (is_open()) 
		close();
}

// @TODO
void os_file::open(const stringx & _name, int _flags)
{
}

// @TODO
void os_file::close()
{
}

#include "..\my_assertions.h"

void validate_os_file()
{
	VALIDATE_SIZE(os_file, 0x18);

	VALIDATE(os_file, name, 0x0);
	VALIDATE(os_file, flags, 0x8);
	VALIDATE(os_file, opened, 0xC);
	VALIDATE(os_file, from_cd, 0xD);

	VALIDATE(os_file, field_10, 0x10);
	VALIDATE(os_file, field_14, 0x14);
}
