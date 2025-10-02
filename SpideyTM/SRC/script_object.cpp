#include "script_object.h"

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

