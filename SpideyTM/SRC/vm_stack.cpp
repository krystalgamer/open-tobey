#include "global.h"
#include "vm_stack.h"

vm_stack::vm_stack(int sa, vm_thread * _my_thread)
  {
  assert(!(sa&3));
  salloc = sa;
  buffer = NEW char[sa];
  SP = buffer;
  my_thread = _my_thread;
  }

vm_stack::~vm_stack()
  {
  delete[] buffer;

  }


// @Ok
// @Matching
bool vm_stack::push( const char* src, int n )
{
	memcpy(SP,src,n);
	move_SP( n );
#if REPORT_OVERFLOW
  // check for stack overflow

  if ( size() > capacity() )
    return false;
#endif
  return true;
}


// @TODO
vm_num_t vm_stack::pop_num()
{
	return 0;
}
