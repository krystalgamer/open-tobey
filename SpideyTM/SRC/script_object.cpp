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
// @NotMatching - list thread safety
void script_object::instance::kill_thread(const vm_executable* ex, const vm_thread* thr)
{
	thread_list::iterator ti = threads.begin();
	while (ti != threads.end())
	{
		vm_thread* t = *ti;
		assert(t != NULL);
		if ( t->get_instance()==this && t->get_executable()==ex && t != thr)
		{
			// found matching thread (same script object instance, same function);
			// kill it dead
			t->remove_from_local_region();
			t->remove_from_local_character();
			//krPrintf("killing thread %s %s\n", t->ex->get_fullname().c_str(), t->inst->get_name().c_str());
			delete t;
			ti = threads.erase( ti );  // erase returns next value for iterator
		}
		else
		{
			++ti;
		}
	}
}

// @Ok
// @NotMatching - list thread safety
void script_object::instance::run(bool ignore_suspended)
{
#if defined(TARGET_PC) && !defined(BUILD_BOOTABLE)
	if(g_script_debugger_running)
	{
		g_sl_debugger.set_new_instance(this, threads.size());
	}
#endif

	thread_list::iterator i = threads.begin();

	while ( i != threads.end() )
	{
		vm_thread* t = *i;
		assert(t != NULL);
		if ( ignore_suspended || !t->is_suspended() )
		{
			// execute thread (will run until finished or interrupted)
#if _CONSOLE_ENABLE && 0
			console_process("console_log c:\\console.txt", 0);

			console_log("execing thread %s %s\n", t->ex->get_fullname().c_str(), t->inst->get_name().c_str());
			console_process("console_log end", 0);
#endif

			if ( t->run() )
			{
				// thread has asked to be killed
				t->remove_from_local_region();
				t->remove_from_local_character();

				//krPrintf("removing thread %s %s\n", t->ex->get_fullname().c_str(), t->inst->get_name().c_str());
				delete t;

				i = threads.erase( i );  // erase returns next value for iterator
			}
			else
			{
				++i;
			}
		}
		else
		{
			++i;
		}
	}
}



// @Ok
// @NotMatching - list thread safety
void script_object::instance::run_single_thread( vm_thread* t, bool ignore_suspended )
{
	if ( ignore_suspended || !t->is_suspended() )
	{
		// execute thread (will run until finished or interrupted)
		if ( t->run() )
		{
			// thread has asked to be killed
			t->remove_from_local_region();
			t->remove_from_local_character();
			delete t;
			// remove thread from list

			//krPrintf("I shouldn't be called - thread %s %s\n", t->ex->get_fullname().c_str(), t->inst->get_name().c_str());

			thread_list::iterator i = threads.begin();

			for ( ; i!=threads.end(); ++i )
			{
				if ( *i == t )
				{
					threads.erase( i );  // erase returns next value for iterator
					break;
				}
			}
		}
	}
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

// @Ok
// @Matching
bool script_object::instance::thread_exists(vm_thread* thread) const
{
	thread_list::const_iterator i = threads.begin();
	thread_list::const_iterator i_end = threads.end();
	for ( ; i!=i_end; ++i )
	{
		if((*i) == thread)
			return(true);
	}

	return(false);
}

// @Ok
// @Matching
bool script_object::instance::thread_exists(unsigned int thread_id) const
{

	thread_list::const_iterator i = threads.begin();
	thread_list::const_iterator i_end = threads.end();
	for ( ; i!=i_end; ++i )
	{
		if((*i)->thread_id == thread_id)
			return(true);
	}


	return(false);

}


// @Ok
// @Matching
bool script_object::instance::thread_exists(vm_thread* thread, unsigned int thread_id) const
{
	thread_list::const_iterator i = threads.begin();
	thread_list::const_iterator i_end = threads.end();

	for ( ; i!=i_end; ++i )
	{
		if((*i) == thread)
		return((*i)->thread_id == thread_id);
	}

	return(false);
}


// @Ok
// @Matching
void script_object::link(const script_manager& sm)
{
	std::vector<vm_executable*>::iterator xi;
	for ( xi=funcs.begin(); xi!=funcs.end(); ++xi )
	{
		(*xi)->link( sm );
	}
}


// @TODO
void script_object::_add( instance* inst )
{
#ifndef BUILD_BOOTABLE
	pair<instance_name_list::iterator,bool> iret = instance_names.insert( inst->get_name() );
	if ( !iret.second )
	{
		error( "Duplicate script instance name " + inst->name );
	}
#endif
	instances.push_back( inst );
}

// @Ok
// @Matching
bool script_object::has_threads() const
{
	instance_list::const_iterator i = instances.begin();
	instance_list::const_iterator i_end = instances.end();
	for ( ; i!=i_end; ++i )
	{
		instance* inst = *i;
		if ( inst->has_threads() )
		{
			return true;
		}
	}

	return false;
}


#include "my_assertions.h"
static void compile_time_assertions()
{
	StaticAssert<sizeof(vm_thread) == 0x48>::sass();
	StaticAssert<sizeof(script_object::instance) == 0x18>::sass();
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

void validate_script_object(void)
{
	VALIDATE(script_object, funcs, 0x20);
	VALIDATE(script_object, instances, 0x2C);
}

#include "my_patch.h"

void patch_script_object(void)
{
	PATCH_PUSH_RET(0x007DF950, script_object::has_threads);
	PATCH_PUSH_RET(0x007DEED0, script_object::link);
}

void patch_script_object_instance(void)
{
	PATCH_PUSH_RET(0x007DE8C0, script_object::instance::suspend);
	PATCH_PUSH_RET(0x007DE900, script_object::instance::unsuspend);

	PATCH_PUSH_RET(0x007DE940, script_object::instance::clear_callback_references);
	PATCH_PUSH_RET(0x007DE980, script_object::instance::dump_threads);

	// @TODO when more of this is done - vm_thread
	//PATCH_PUSH_RET_POLY(0x007DE340, script_object::instance::add_thread, "?add_thread@instance@script_object@@QAEPAVvm_thread@@PBVvm_executable@@@Z");
	//PATCH_PUSH_RET_POLY(0x007DE420, script_object::instance::add_thread, "?add_thread@instance@script_object@@QAEPAVvm_thread@@PBVvm_executable@@PBD@Z");
	//PATCH_PUSH_RET_POLY(0x007DE540, script_object::instance::add_thread, "?add_thread@instance@script_object@@QAEPAVvm_thread@@PAVscript_callback@@PBVvm_executable@@PBD@Z");
	//PATCH_PUSH_RET(0x007DE800, script_object::instance::run_single_thread);
	//PATCH_PUSH_RET(0x007DE6A0, script_object::instance::kill_thread);
	//PATCH_PUSH_RET(0x007DE740, script_object::instance::run);

	PATCH_PUSH_RET_POLY(0x007DE9F0, script_object::instance::thread_exists, "?thread_exists@instance@script_object@@QBE_NPAVvm_thread@@@Z");
	PATCH_PUSH_RET_POLY(0x007DEA30, script_object::instance::thread_exists, "?thread_exists@instance@script_object@@QBE_NI@Z");
	PATCH_PUSH_RET_POLY(0x007DEA70, script_object::instance::thread_exists, "?thread_exists@instance@script_object@@QBE_NPAVvm_thread@@I@Z");

}
