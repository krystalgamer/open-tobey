#include "w32_file.h"

#define INVLALID_FP INVLAID_HANDLE_VALUE
char os_file::root_dir[MAX_DIR_LEN];
char os_file::pre_root_dir[MAX_DIR_LEN];

// @Ok
// @Mataching
os_file::os_file()
{
	file_handle = INVALID_HANDLE_VALUE;
	flags=0;
	opened=false;
	from_cd = false;
	file_ptr = 0;
}

// @Ok
// @Matching
os_file::os_file(const stringx & _name, int _flags)
{
	this->flags = 0;
	this->opened = 0;
	this->from_cd = 0;
	this->file_ptr = 0;
	this->file_handle = INVALID_HANDLE_VALUE;

	open(_name,_flags);
}



/*** destructor ***/
// @Ok
// @Matching
os_file::~os_file()
{
	if (is_open()) 
		close();
}

// @TODO
void os_file::open(const stringx & _name, int _flags)
{
}

// @Ok
// @Matching
INLINE void os_file::close()
{
	if (this->file_ptr)
	{
		UnmapViewOfFile(this->file_ptr);
		this->file_ptr = 0;
	}

	CloseHandle(this->file_handle);
	this->file_handle = INVALID_HANDLE_VALUE;
	this->opened = 0;
	this->from_cd = 1;
}

// @Ok - reuse of stack as local
// @Matching
int os_file::write(void * data, int bytes)
{
	DWORD written;
	WriteFile(this->file_handle, data, bytes, &written, 0);
	return written;
}

// @Ok
// @Matching
int os_file::get_size()
{
	if(!is_open())
	{
		return -1;
	}

	return GetFileSize(this->file_handle, 0);
}

bool os_file::try_unmap_file(int)
{
	if (file_ptr)
	{
		UnmapViewOfFile(file_ptr);
		file_ptr = 0;
		return true;
	}

	return false;
}

// @Ok
// @Matching
void os_file::set_fp(int pos, filepos_t base)
{
	int method = 0;
	switch (base)
	{
    case FP_BEGIN:
		method = FILE_BEGIN;
		break;
    case FP_CURRENT:
		method = FILE_CURRENT;
		break;
    case FP_END:
		method = FILE_END;
		break;
	}
	
	SetFilePointer(this->file_handle, pos, 0, method);
}

// @Ok
// @Matching
void os_file::set_root_dir(const stringx & dir)
{
	assert( dir.size()<MAX_DIR_LEN );
	strcpy( root_dir, dir.c_str() );
}

// @Ok
// @Matching
void os_file::set_pre_root_dir(const stringx& dir)
{
	assert( dir.size()<MAX_DIR_LEN );
	strcpy( pre_root_dir, dir.c_str() );
}

// @Ok
// @Matching
host_system_file_handle host_fopen(const char* fname, host_fopen_flags_t flags)
{
	char open_flags[3];
	switch (flags & HOST_RWA_MASK)
	{
		case HOST_READ:
			open_flags[0] = 'r';
			break;
		case HOST_WRITE:
			open_flags[0] = 'w';
			break;
		case HOST_APPEND:
			open_flags[0] = 'a';
			break;
	}

	switch (flags & HOST_TB_MASK)
	{
		case HOST_TEXT:
			open_flags[1] = 't';
			break;
		case HOST_BINARY:
			open_flags[1] = 'b';
			break;
	}

	open_flags[2] = '\0';

	return fopen(fname, open_flags);
}

#include "..\my_assertions.h"

void validate_os_file()
{
	VALIDATE_SIZE(os_file, 0x18);

	VALIDATE(os_file, name, 0x0);
	VALIDATE(os_file, flags, 0x8);
	VALIDATE(os_file, opened, 0xC);
	VALIDATE(os_file, from_cd, 0xD);

	VALIDATE(os_file, file_handle, 0x10);
	VALIDATE(os_file, file_ptr, 0x14);
}

#include "..\my_patch.h"

void patch_os_file()
{
	PATCH_PUSH_RET_POLY(0x007F45F0, os_file::os_file, "??0os_file@@QAE@XZ");
	// @TODO - when open is done
	//PATCH_PUSH_RET_POLY(0x007F4630, os_file::os_file, "??0os_file@@QAE@ABVstringx@@H@Z");
	PATCH_PUSH_RET_POLY(0x007F4680, os_file::~os_file, "??1os_file@@QAE@XZ");
	
	PATCH_PUSH_RET(0x007F4940, os_file::write);
	PATCH_PUSH_RET(0x007F4980, os_file::get_size);

	PATCH_PUSH_RET(0x007F4AB0, os_file::set_fp);


	// @TODO - replace root dir stuff
	

	PATCH_PUSH_RET(0x007F4D80, host_fopen);
	PATCH_PUSH_RET(0x007F4850, os_file::close);
	PATCH_PUSH_RET(0x007F4A70, os_file::try_unmap_file);
}
