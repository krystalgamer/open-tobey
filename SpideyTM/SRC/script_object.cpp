#include "script_object.h"
#include "vm_thread.h"

script_object::instance::instance(const stringx& n,int sz)
: name( n ),

  data( sz ),
  threads(),
  suspended( false )
{
}

// @TODO
void script_object::instance::kill_thread(const vm_executable* ex)
{
}

// @TODO
vm_thread* script_object::instance::add_thread(const vm_executable* ex)
{
	return NULL;
}

// @TODO
vm_thread* script_object::instance::add_thread( const vm_executable* ex, const char* parms )
  {
	  return NULL;
  }

// @TODO
// spawn a NEW thread via the given event callback
vm_thread* script_object::instance::add_thread( script_callback* cb, const vm_executable* ex, const char* parms )
  {
	  return NULL;
  }

// @TODO
void script_object::instance::clear_callback_references( script_callback *remove_me )
{
}

// @TODO
// return pointer to executable that corresponds to given PC (NULL if not found)
const vm_executable* script_manager::find_function_by_address( const unsigned short* PC ) const
{
  return NULL;
}

// @Ok
// @Matching
void script_object::instance::suspend()
{
  thread_list::iterator i = threads.begin();
  thread_list::iterator i_end = threads.end();
  for ( ; i!=i_end; ++i )
    {
    vm_thread* t = *i;
    assert(t != NULL);
    t->set_suspended( true );
    }

  suspended = true;
}

// @Ok
// @Matching
void script_object::instance::unsuspend()
{
	  thread_list::iterator i = threads.begin();
  thread_list::iterator i_end = threads.end();
  for ( ; i!=i_end; ++i )
    {
    vm_thread* t = *i;
    assert(t != NULL);
    t->set_suspended( false );

    }
  suspended = false;
}

#include "my_assertions.h"
static void compile_time_assertions()
{
	StaticAssert<sizeof(vm_thread) == 0x48>::sass();
}


void validate_script_object_instance(void)
{
	VALIDATE_SIZE(script_object::instance, 0x18);
	VALIDATE_SIZE(std::list<vm_thread*>, 0x4);

	VALIDATE(script_object::instance, name, 0x0);
	VALIDATE(script_object::instance, data, 0x8);
	VALIDATE(script_object::instance, threads, 0x10);
	VALIDATE(script_object::instance, suspended, 0x14);
}


#include "my_patch.h"

void patch_script_object_instance(void)
{
	PATCH_PUSH_RET(0x007DE8C0, script_object::instance::suspend);
	PATCH_PUSH_RET(0x007DE900, script_object::instance::unsuspend);
}
