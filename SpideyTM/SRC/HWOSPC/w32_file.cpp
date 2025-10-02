#include "w32_file.h"

#define INVLALID_FP INVLAID_HANDLE_VALUE

os_file::os_file()
{
	flags=0;
	opened=false;
	from_cd = false;
	io = INVALID_FP;
}

os_file::os_file(const stringx & _name, int _flags)

{
	flags=0;
	opened=false;
	from_cd = false;
	io = INVALID_FP;
	open(_name,_flags);

}



/*** destructor ***/
os_file::~os_file()
{
	if (is_open()) 
		close();
}
