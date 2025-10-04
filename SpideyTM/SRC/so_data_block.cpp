#include "global.h"
#include "so_data_block.h"
#include "script_library_class.h"

#include <memory>


// CLASS so_data_block

// Constructors

so_data_block::so_data_block(const so_data_block& b)
{
	init(b.blocksize);
	memcpy(buffer,b.buffer,blocksize);
}

// @Ok 
// @Matching
so_data_block::so_data_block(int sz)
{
	init(sz);
}

so_data_block::~so_data_block()
  {
    _destroy();
  }


// Methods

// @Ok 
// @Matching
INLINE void so_data_block::init(int sz)
{
	blocksize = sz;
	if (sz)
	{
		buffer = NEW char[sz];

		int stop = sz >> 2;
		assert (sz == stop*4);
		for (int i=0;i<stop;i++) ((unsigned int *)buffer)[i] = UNINITIALIZED_SCRIPT_PARM;
	}
	else
	buffer = NULL;
}

void so_data_block::clear()
  {
  _destroy();
  blocksize = 0;
  }


// Internal Methods

void so_data_block::_destroy()
{
if (buffer)
delete[] buffer;
}

#include "my_patch.h"

void patch_so_data_block(void)
{
	PATCH_PUSH_RET_POLY(0x007E3360, so_data_block::so_data_block, "??0so_data_block@@QAE@H@Z");
}

#include "my_assertions.h"

void validate_so_data_block(void)
{
	VALIDATE_SIZE(so_data_block, 0x8);

	VALIDATE(so_data_block, blocksize, 0x0);
	VALIDATE(so_data_block, buffer, 0x4);
}
