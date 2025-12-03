#include "w32_timer.h"
#include "..\inputmgr.h"

void input_mgr::scan_devices(void)
{
	// @TODO
	typedef void (__fastcall *func_ptr)(input_mgr*);
	func_ptr func = (func_ptr)0x00822870;

	func(this);
	return;
}
