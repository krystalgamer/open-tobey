#include "region.h"

typedef void (__fastcall *remove_from_local_thread_ptr)(region*, int, vm_thread*);

// @TODO
void region::remove_local_thread(vm_thread * thr)
{
	remove_from_local_thread_ptr remove_from_local_thread_f = (remove_from_local_thread_ptr)0x0050F8B0;
	remove_from_local_thread_f(this, 0, thr);
}
