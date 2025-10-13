#include "w32_file.h"

#include "..\ini_parser.h"

#define INVLALID_FP INVLAID_HANDLE_VALUE
char os_file::root_dir[MAX_DIR_LEN];
char os_file::pre_root_dir[MAX_DIR_LEN];
bool os_file::system_locked = false;

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

// @Ok
static inline void check_system_locked( const stringx& name )
{
	if (os_file::is_system_locked() && !os_developer_options::inst()->is_flagged(os_developer_options::FLAG_MOVE_EDITOR))
	{
		warning( name + ": os_file system is locked; no file access allowed" );
	}
}

// @Ok
// @Matching
void os_file::open(const stringx & _name, int _flags)
{
	check_system_locked( _name );

	this->name = _name;

	if (this->name[1] != ':' &&this->name[0] != '\\')
	{
		{
			this->name = os_file::root_dir + this->name;
		}
	}


	DWORD desiredAccess = -1;
	DWORD creationDisposition = -1;
	DWORD sharedMode = -1;
	this->flags = _flags;
	if (_flags & FILE_READ)
	{
		desiredAccess = GENERIC_READ;
		creationDisposition = OPEN_EXISTING;
		sharedMode = FILE_SHARE_READ;
	}
	else if (_flags & FILE_WRITE)
	{
		desiredAccess = GENERIC_WRITE;
		creationDisposition = CREATE_ALWAYS;
		sharedMode = 0;
	}
	else if (_flags & FILE_MODIFY)
	{
		desiredAccess = GENERIC_WRITE | GENERIC_READ;
		creationDisposition = OPEN_ALWAYS;
		sharedMode = 0;
	}
	else if (_flags & FILE_APPEND)
	{
		desiredAccess = GENERIC_WRITE;
		creationDisposition = OPEN_ALWAYS;
		sharedMode = 0;
	}


	this->file_handle = CreateFileA(
			this->name.c_str(),
			desiredAccess,
			sharedMode,
			0,
			creationDisposition,
			FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_NORMAL,
			0);

	if (this->file_handle == INVALID_HANDLE_VALUE)
	{
		this->opened = 0;
	}
	else
	{
		this->opened = 1;
		this->from_cd = 0;
	}
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

// @Ok
// @Matching
int os_file::read(void *data, int bytes)
{
	DWORD numRead; 
	if (!ReadFile(this->file_handle, data, bytes, &numRead, 0))
	{
		error(stringx("Something weird about file ") + this->name);
	}

	this->from_cd = numRead < bytes;
	return numRead;
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
bool os_file::file_exists(const stringx& name)
{
	stringx dir_name;

	if (name[1] == ':' || name[0] == '\\')
	{
		dir_name = name;
	}
	else
	{
		dir_name = os_file::root_dir + name;
	}

	return (GetFileAttributesA(dir_name.c_str()) & FILE_ATTRIBUTE_DIRECTORY);
}

// @Ok
// @NotMatching - missing the other part of system check
bool os_file::directory_exists(const stringx &name)
{
	check_system_locked(name);


	stringx dir_name;

	if (name[1] == ':' || name[0] == '\\')
	{
		dir_name = name;
	}
	else
	{
		dir_name = os_file::root_dir + name;
	}

	DWORD ret = GetFileAttributesA(dir_name.c_str());

	if (ret == -1)
	{
		return false;
	}

	return ret & FILE_ATTRIBUTE_DIRECTORY;
}

// @Ok
// @Matching
bool os_file::is_file_newer(const stringx& file1, const stringx& file2)
{
	return false;
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

// @Ok
// @Matching
void host_fclose(host_system_file_handle handle)
{
	fclose(handle);
}

// @Ok
// @Matching
int host_read( host_system_file_handle fp, void* buf, int len )
{
	return fread(buf, 1, len, fp);
}

// @Ok
// @Matching
int host_write( host_system_file_handle fp, void const * buf, int len )
{
	return fwrite(buf, 1, len, fp);
}

// @Ok
// @Matching
int host_fseek( host_system_file_handle fp, int offset, host_seek_mode_t mode )
{
	int origin = 0;
	
	switch (mode)
	{
		case HOST_CUR:
			origin = SEEK_CUR;
			break;
		case HOST_BEGIN:
			origin = SEEK_SET;
			break;
		case HOST_END:
			origin = SEEK_END;
			break;
	}

	return fseek(fp, offset, origin);
}

#include <sys/stat.h>

// @Ok
// @Matching
int host_get_size( host_system_file_handle fp )
{
	struct _stat v2; // [esp+0h] [ebp-24h] BYREF

	if (_fstat(fp->_file, &v2) )
	{
		return -1;
	}

	return v2.st_size;
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
	PATCH_PUSH_RET_POLY(0x007F4630, os_file::os_file, "??0os_file@@QAE@ABVstringx@@H@Z");
	PATCH_PUSH_RET_POLY(0x007F4680, os_file::~os_file, "??1os_file@@QAE@XZ");
	
	PATCH_PUSH_RET(0x007F46D0, os_file::open);
	PATCH_PUSH_RET(0x007F4850, os_file::close);
	PATCH_PUSH_RET(0x007F48A0, os_file::read);
	PATCH_PUSH_RET(0x007F4940, os_file::write);
	PATCH_PUSH_RET(0x007F4980, os_file::get_size);
	PATCH_PUSH_RET(0x007F4A70, os_file::try_unmap_file);
	PATCH_PUSH_RET(0x007F4AB0, os_file::set_fp);


	PATCH_PUSH_RET(0x007F4B30, os_file::set_root_dir);
	PATCH_PUSH_RET(0x007F4B60, os_file::get_root_dir);

	PATCH_PUSH_RET(0x007F4B80, os_file::set_pre_root_dir);
	PATCH_PUSH_RET(0x007F4BB0, os_file::get_pre_root_dir);
	
	PATCH_PUSH_RET(0x007F4BD0, os_file::file_exists);
	PATCH_PUSH_RET(0x007F4C70, os_file::directory_exists);
	PATCH_PUSH_RET(0x007F4D60, os_file::is_file_newer);

	PATCH_PUSH_RET(0x007F4D80, host_fopen);
	PATCH_PUSH_RET(0x007F4E20, host_fclose);
	PATCH_PUSH_RET(0x007F4E40, host_read);
	PATCH_PUSH_RET(0x007F4E70, host_write);
	PATCH_PUSH_RET(0x007F4EA0, host_fseek);
	PATCH_PUSH_RET(0x007F4EF0, host_get_size);
}
