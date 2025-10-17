#include "script_object.h"
#include "vm_thread.h"

#define VM_STACKSIZE 0x180

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

// @Ok
// @NotMatching - the thread-safety on lists
INLINE vm_thread* script_object::instance::add_thread(const vm_executable* ex)
{
	vm_thread* nt = NEW vm_thread(this,ex,VM_STACKSIZE);
  assert(nt != NULL);
  threads.push_back(nt);
  // if this script object instance is suspended, we must suspend any thread
  // that is added here
  if ( suspended )
    nt->set_suspended( true );

  return nt;
}

// @Ok
// @Matching
vm_thread* script_object::instance::add_thread( const vm_executable* ex, const char* parms )
{
	vm_thread* nt = add_thread( ex );
	assert(nt != NULL);

	if ( parms && !nt->get_data_stack().push( parms, ex->get_parms_stacksize() ) )
	{
		error( get_name() + ": stack overflow spawning " + ex->get_fullname() );
	}

	nt->PC = ex->get_start();
	return nt;
}

// @Ok
// @Matching
// spawn a NEW thread via the given event callback
vm_thread* script_object::instance::add_thread( script_callback* cb, const vm_executable* ex, const char* parms )
{
	vm_thread* nt = NEW vm_thread( this, ex, VM_STACKSIZE, cb );
	assert(nt != NULL);
	threads.push_back( nt );
	// if this script object instance is suspended, we must suspend any thread
	// that is added here
	if ( suspended )
	{
		nt->set_suspended( true );
	}
	if ( parms && !nt->get_data_stack().push( parms, ex->get_parms_stacksize() ) )
	{
		error( get_name() + ": stack overflow spawning " + ex->get_fullname() );
	}

	nt->PC = ex->get_start();
	return nt;
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

// @Ok
// @Matching
// for bug purposes; fixes a dangling pointer
void script_object::instance::clear_callback_references( script_callback *remove_me )
{
	thread_list::const_iterator i = threads.begin();
	thread_list::const_iterator i_end = threads.end();
	for ( ; i!=i_end; ++i )
	{
		vm_thread* t = *i;

		assert(t != NULL);

		if (t->my_callback == remove_me)
		  t->my_callback = NULL;
	}

}

// @Ok
// @Matching
// for debugging purposes; dump information on all threads to a file
void script_object::instance::dump_threads( host_system_file_handle outfile ) const
{

	thread_list::const_iterator i = threads.begin();
	thread_list::const_iterator i_end = threads.end();
	for ( ; i!=i_end; ++i )
	{
		vm_thread* t = *i;

		if (!t->is_suspended())
		{

#if THREAD_PROFILING
			host_fprintf( outfile, "%s %s %f %i\n", name.c_str(), t->get_executable()->get_name().c_str(), t->prof_runtime, t->prof_opcount );
#else
			host_fprintf( outfile, "%s %s\n", name.c_str(), t->get_executable()->get_name().c_str() );
#endif
		}
	}
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

	PATCH_PUSH_RET(0x007DE940, script_object::instance::clear_callback_references);
	PATCH_PUSH_RET(0x007DE980, script_object::instance::dump_threads);

	// @TODO when more of this is done
	//PATCH_PUSH_RET_POLY(0x007DE340, script_object::instance::add_thread, "?add_thread@instance@script_object@@QAEPAVvm_thread@@PBVvm_executable@@@Z");
	//PATCH_PUSH_RET_POLY(0x007DE420, script_object::instance::add_thread, "?add_thread@instance@script_object@@QAEPAVvm_thread@@PBVvm_executable@@PBD@Z");
	//PATCH_PUSH_RET_POLY(0x007DE540, script_object::instance::add_thread, "?add_thread@instance@script_object@@QAEPAVvm_thread@@PAVscript_callback@@PBVvm_executable@@PBD@Z");
}
