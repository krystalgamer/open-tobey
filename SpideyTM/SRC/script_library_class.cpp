// script_library_class.cpp
#include "global.h"

#include "script_library_class.h"
#include "script_object.h"
#include "vm_stack.h"
#include "vm_thread.h"

#include "oserrmsg.h"

//!DEFINE_AUTO_SINGLETON(slc_manager)
// @Patch - remove
//DEFINE_SINGLETON(slc_manager)

char* strdupcpp(const char* str);

extern void script_strings_deconstruct();


// CLASS script_library_class

// global script library class provides home for global library functions


//script_library_class* slc_global = NULL;


// Constructors

// @TODO - focus on slc_manager first
int sizeof_funcs = sizeof(script_library_class::function_list);

// @Ok
// @Matching
INLINE script_library_class::script_library_class(const char* n,int sz,const char* p)
  : name(n),
    size(sz),
    parent_name(p),
    parent(NULL),
    funcs()
{
  // add myself to the script library class manager
  assert(slc_manager::is_inst());
  slc_manager::inst()->add(this);
}

// constructor provided for searching (see slc_manager::find())

// @Ok
// @Matching
INLINE script_library_class::script_library_class()
  : name(),
    size(0),
    parent_name(NULL),
    parent(NULL),
    funcs()
{
}


// @Ok
// @PartialMatching - stl
INLINE script_library_class::~script_library_class()
{
	for (function_list::iterator i=funcs.begin(); i!=funcs.end(); ++i)
	{
		delete *i;
	}
}



// Methods

// @TODO
const script_library_class::function* script_library_class::find(const char* n) const
{
	// @TODO
	typedef script_library_class::function* (__fastcall *find_ptr)(const script_library_class*, int, const char*);
	find_ptr find_real = (find_ptr)0x007DAB20;
	return find_real(this, 0, n);
}



// SUBCLASS script_library_class::function

// Constructors

script_library_class::function::function(int dummy)
  : name("")
{
}

script_library_class::function::function(script_library_class* slc,const char* n)
{
  // add myself to the given script library class
  name = strdupcpp(n);

  slc->add(this);
}

// @Ok
// @Matching
script_library_class::function::function(const char* n)
{
  name = strdupcpp(n);
  // add myself to the global script library class
  //if(!slc_global) slc_global = NEW script_library_class("_global_slc",0);
  //assert(slc_global);
  GET_SLC_GLOBAL->add(this);
}

// constructor provided for script_library_class::find()

script_library_class::function::function(const stringx& n)
{
  name = strdupcpp(n.c_str());
}

// @Ok
// @Matching
script_library_class::function::~function()
{
  delete[] name;
}



// SINGLETON CLASS slc_manager

// Constructors (not public in a singleton)
// @Ok
// @PartialMatching - stl
slc_manager::slc_manager()
  : classes()
{
}

// @Ok
// @PartialMatching - stl
slc_manager::~slc_manager()
{
	// @Patch - remove KSCULL
	script_strings_deconstruct();
	for (class_list::iterator i=classes.begin(); i!=classes.end(); ++i)
	{
		delete *i;
	}
}


// Methods

// @Ok
// @Matching - it randomly changed
INLINE void slc_manager::add(script_library_class* slc)
{
#ifdef DEBUG
  pair<class_list::iterator,bool> iret = classes.insert( slc );
  assert( iret.second );

#else
  classes.insert( slc );
#endif

}


// @Ok
// @Matching
// call this after static initialization, to establish parent links
void slc_manager::link_hierarchy()
{
  class_list::iterator i = classes.begin();
  class_list::iterator i_end = classes.end();
  for ( ; i!=i_end; ++i )
  {

    // for each class that has a parent_name, find that parent and link the pointer
    script_library_class* c = *i;
    if ( c->parent_name )
      c->parent = find( c->parent_name );

  }
}

// @Ok
// @PartialMatching - STL thread safety but only in the destructor
script_library_class* slc_manager::find(const char* n) const
{
  // set up search argument
  // NOTE: we cannot call a public constructor here, since such would end up
  // calling slc_manager in an attempt to add the library class to the global list
  script_library_class slc;
  slc.name = n;
  class_list::const_iterator cli = classes.find(&slc);
  if (cli == classes.end())
    return NULL;
  return *cli;
}

// @Ok
// @PartialMatching - stl
// destroy library class with given name (if any)
void slc_manager::destroy( const stringx& n )
{
  script_library_class slc;
  slc.name = n;
  class_list::iterator cli = classes.find(&slc);

  if ( cli != classes.end() )
  {
    delete *cli;

    classes.erase( cli );
  }
}

// @Ok
// @Matching
void slc_manager::purge()
{
	script_strings_deconstruct();
	class_list::iterator cli;
	for( cli = classes.begin(); cli != classes.end(); ++cli )
	{
		(*cli)->purge();
	}
}



// CLASS slc_script_object_t supports dummy slc entries for script objects, so
// that they may be referenced and linked at runtime.
// fill in for script language built-in type NUM

// @Ok
// @AlmostMatching - push order different inside the if block
slc_script_object_t::slc_script_object_t(const script_manager& sm,const char* n)
  : script_library_class(n,4,NULL),
    sman(sm)
{
  sobj = sman.find_object(n);
  if (!sobj)
  {
    stringx err;
    err=stringx("script object ")+n+" not found\n";
    error(err.c_str());
  }
}

#ifndef NO_SERIAL_IN
// read a script object value (by id) from a stream
typedef script_object::instance* vm_script_object_t;  // vm_stack data representation

// @Ok
// @Matching
void slc_script_object_t::read_value(chunk_file& fs,char* buf)
{
  // read id
  stringx id;
  serial_in(fs,&id);
  // find script object instance and write value to buffer
  *(vm_script_object_t*)buf = (vm_script_object_t)find_instance(id);
}
#endif

// @Ok
// @Matching
// find named instance of script object
unsigned slc_script_object_t::find_instance(const stringx& n) const
{
  script_object::instance* soinst = sobj->find_instance(n);
  if (!soinst)
  {
    stringx err;
    err="script object instance "+n+" not found";
    error(err.c_str());
  }
  return (unsigned)soinst;
}


// CLASS slc_num_t supports script language built-in type NUM
// @TODO - this might be used be careful
slc_num_t* slc_num = NULL;

// @Ok
// @Matching
slc_num_t::slc_num_t(const char* n,int sz,const char* p)
  : script_library_class(n,sz,p)
{
}

#ifndef NO_SERIAL_IN
// @Ok
// @Matching
// read a num value from a stream
void slc_num_t::read_value(chunk_file& fs,char* buf)
{
  serial_in(fs,(vm_num_t*)buf);
}
#endif



// CLASS slc_str_t supports script language built-in type STR

// @TODO - maybe important
slc_str_t* slc_str = NULL;


// @Ok
// @NotMatching - slightly different code gen but no biggie
slc_str_t::slc_str_t(const char* n,int sz,const char* p)
  : script_library_class(n,sz,p),
	strings(NULL)
{
}

// destructor needed to delete managed strings
// @Ok
// @Matching
slc_str_t::~slc_str_t()
{
	purge();
}

#ifndef NO_SERIAL_IN
// @Ok
// @PartialMatching - stl
// read a str value from a stream
void slc_str_t::read_value(chunk_file& fs,char* buf)
{
	// read NEW stringx and add to managed strings list
	stringx* s = NEW stringx;
	serial_in(fs,s);

	// @Patch - strings is a pointer
	if (!this->strings)
	{
		this->strings = NEW std::vector<stringx*>;
	}


	strings->push_back(s);
	// copy stringx pointer into buffer
	*(vm_str_t*)buf = s;
}
#endif


// @Ok
// @PartialMatching - stl
INLINE void slc_str_t::purge()
{
	// @Patch - strings is a pointer
	if (this->strings)
	{
		for (std::vector<stringx*>::iterator i=strings->begin(); i!=strings->end(); ++i)
		{
			delete *i;
		}

		// @Patch
		delete this->strings;
		this->strings = NULL;
	}
}



///////////////////////////////////////////////////////////////////////////////
// Error message function for script library classes
///////////////////////////////////////////////////////////////////////////////
void verify_parms_integrity( script_library_class::function* the_function,
                             vm_stack* the_stack,
                             unsigned int* parms,
                             int parms_size )
{
  int i;

  for ( i=0; i<parms_size; ++i )
  {

    if ( parms[i] == UNINITIALIZED_SCRIPT_PARM )
    {
      // give bad parameter message <<<
      the_stack->get_thread()->slf_error( stringx("uninitialized parameters in call to ") + the_function->get_name() );
    }
  }
}


#include "my_assertions.h"

static void compile_time_assertions(void)
{
	StaticAssert<sizeof(slc_manager) == 0x10>::sass();
}

void validate_slc_manager(void)
{
	VALIDATE_SIZE(slc_manager, 0x10);

	VALIDATE(slc_manager, classes, 0x4);
}

void validate_script_library_class(void)
{
	VALIDATE_SIZE(script_library_class, 0x24);

	VALIDATE(script_library_class, name, 0x4);
	VALIDATE(script_library_class, size, 0xC);
	VALIDATE(script_library_class, parent_name, 0x10);
	VALIDATE(script_library_class, parent, 0x14);

	VALIDATE(script_library_class, funcs, 0x18);

	VALIDATE_VTABLE(script_library_class, find_instance, 0x1);
	VALIDATE_VTABLE(script_library_class, read_value, 0x2);
	VALIDATE_VTABLE(script_library_class, purge, 0x3);
}

void validate_script_library_class_function(void)
{
	VALIDATE_SIZE(script_library_class::function, 0x8);

	VALIDATE(script_library_class::function, name, 0x4);
}

void validate_slc_script_object_t(void)
{
	VALIDATE_SIZE(slc_script_object_t, 0x2C);

	// @Note - can't validate it causes null de-ref
	//VALIDATE(slc_script_object_t, sman, 0x24); 
	VALIDATE(slc_script_object_t, sobj, 0x28);

	VALIDATE_VTABLE(slc_script_object_t, find_instance, 0x1);
	VALIDATE_VTABLE(slc_script_object_t, read_value, 0x2);
	VALIDATE_VTABLE(slc_script_object_t, purge, 0x3);
}

void validate_slc_num_t(void)
{
}

void validate_slc_str_t(void)
{
	VALIDATE_SIZE(slc_str_t, 0x28);

	VALIDATE(slc_str_t, strings, 0x24);
}

#include "my_patch.h"

void patch_slc_str_t(void)
{
	PATCH_PUSH_RET_POLY(0x007DC2C0, slc_str_t::slc_str_t, "??0slc_str_t@@QAE@PBDH0@Z");

	PATCH_PUSH_RET_POLY(0x007DC820, slc_str_t::read_value, "?read_value@slc_str_t@@UAEXAAVchunk_file@@PAD@Z");
	PATCH_PUSH_RET_POLY(0x007DC9D0, slc_str_t::purge, "?purge@slc_str_t@@UAEXXZ");
}

void patch_slc_num_t(void)
{
	PATCH_PUSH_RET_POLY(0x007DBF70, slc_num_t::slc_num_t, "??0slc_num_t@@QAE@PBDH0@Z");
	PATCH_PUSH_RET_POLY(0x007DC290, slc_num_t::read_value, "?read_value@slc_num_t@@UAEXAAVchunk_file@@PAD@Z");

	VALIDATE_VTABLE(slc_num_t, find_instance, 0x1);
	VALIDATE_VTABLE(slc_num_t, read_value, 0x2);
	VALIDATE_VTABLE(slc_num_t, purge, 0x3);
}

void patch_slc_script_object_t(void)
{
	PATCH_PUSH_RET_POLY(0x007DBEB0, slc_script_object_t::find_instance, "?find_instance@slc_script_object_t@@UBEIABVstringx@@@Z");
	PATCH_PUSH_RET_POLY(0x007DBE40, slc_script_object_t::read_value, "?read_value@slc_script_object_t@@UAEXAAVchunk_file@@PAD@Z");

	PATCH_PUSH_RET_POLY(0x007DBB40, slc_script_object_t::slc_script_object_t, "??0slc_script_object_t@@QAE@ABVscript_manager@@PBD@Z");
}

void patch_script_library_class_function(void)
{
	PATCH_PUSH_RET_POLY(0x007DAE20, script_library_class::function, "??0function@script_library_class@@QAE@PBD@Z");
	PATCH_PUSH_RET_POLY(0x007DABE0, script_library_class::function::operator(), "??Rfunction@script_library_class@@UAE_NAAVvm_stack@@W4entry_t@01@@Z");
}

void patch_slc_manager(void)
{
	PATCH_PUSH_RET_POLY(0x007DB050, slc_manager::slc_manager, "??0slc_manager@@IAE@XZ");

	PATCH_PUSH_RET(0x007DBAC0, slc_manager::purge);

	PATCH_PUSH_RET_POLY(0x007DB6C0, slc_manager::find, "?find@slc_manager@@QBEPAVscript_library_class@@PBD@Z");

	PATCH_PUSH_RET(0x007DB8F0, slc_manager::destroy);
	PATCH_PUSH_RET(0x007DB470, slc_manager::link_hierarchy);
	PATCH_PUSH_RET(0x007DB390, slc_manager::add);
}

void patch_script_library_class(void)
{
	PATCH_PUSH_RET_POLY(0x0052C030, script_library_class::find_instance, "?find_instance@script_library_class@@UBEIABVstringx@@@Z");
	PATCH_PUSH_RET_POLY(0x0052C050, script_library_class::read_value, "?read_value@script_library_class@@UAEXAAVchunk_file@@PAD@Z");
	PATCH_PUSH_RET_POLY(0x0052C070, script_library_class::purge, "?purge@script_library_class@@UAEXXZ");
}
