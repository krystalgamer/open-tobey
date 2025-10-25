#include "vm_executable.h"

#include "oserrmsg.h"


// CLASS vm_executable

// Constructors

// @Ok
// @Matching
vm_executable::vm_executable( script_object* _owner )
:   owner( _owner ),
    name(),
    fullname(),
    parameters(),
    parms_stacksize( 0 ),
    static_func( false ),
    linked( false ),
    buffer( NULL ),
    buffer_len( 0 ),
    strings()
{
}


// @NotOk
// @Note: strange memcpy non inlined
vm_executable::vm_executable(const vm_executable& b)
:   owner( b.owner ),
    name( b.name ),
    fullname( b.fullname ),
    parameters( b.parameters ),
    parms_stacksize( b.parms_stacksize ),
    static_func( b.static_func ),
    linked( b.linked ),
    buffer_len( b.buffer_len ),
    strings( b.strings )
  {
	  // @Note: don't use scary
	  /*
  buffer = NEW unsigned short[buffer_len];
  memcpy(buffer,b.buffer,buffer_len*2);
  */
  }


// @Ok
// @NotMatching - stl stuff
vm_executable::~vm_executable()
{
	// @Patch - clear instead of destroy
	this->clear();
}



// Internal Methods


// @Ok
// @Matching
INLINE void vm_executable::_destroy()
{
	// @Patch - just set to null
	this->buffer = NULL;
}

// @Ok
// @PartialMatching - resize doesn't nullify most fields
INLINE void vm_executable::clear()
{
	parameters.clear();
	parameters.resize(0);

	strings.clear();
	strings.resize(0);

	_destroy();
	linked = false;
}

// @Ok
// @Matching
void vm_executable::_build_fullname()
{
	fullname = name;
	fullname += "(";
	for (parms_list::const_iterator pli=parameters.begin(); pli!=parameters.end(); pli++)
	{
		if (pli != parameters.begin())
			fullname += ",";

		fullname += (*pli)->get_name();
	}
	fullname += ")";
}

unsigned short vm_executable::_string_id(const stringx& s)
  {
	  // @TODO
	  /*
  int idx = 0;
  std::vector<stringx const *>::iterator i=strings.begin();
  for (; i!=strings.end() && **i!=s; i++,idx++) continue;
  if (i == strings.end())
    {
    // string not already registered;
    // register NEW string
    stringx const * sptr;
    sptr = g_world_ptr->get_script_manager()->add_string(s);
    strings.push_back(sptr);
    }
  // return stringx index

  return idx;
  */
	  return 69;
  }


typedef void (__fastcall *vm_executable_link_ptr)(vm_executable*, int, const script_manager&);

// @TODO
void vm_executable::link(const script_manager& sm)
{
	vm_executable_link_ptr vm_executable_link = (vm_executable_link_ptr)0x007E45C0;
	vm_executable_link(this, 0, sm);
}

#include "my_assertions.h"
void validate_vm_executable(void)
{
	VALIDATE_SIZE(vm_executable, 0x3C);

	VALIDATE(vm_executable, owner, 0x0);
	VALIDATE(vm_executable, name, 0x4);

	VALIDATE(vm_executable, fullname, 0xC);

	VALIDATE(vm_executable, parameters, 0x14);

	VALIDATE(vm_executable, parms_stacksize, 0x20);

	VALIDATE(vm_executable, static_func, 0x24);
	VALIDATE(vm_executable, linked, 0x25);

	VALIDATE(vm_executable, buffer, 0x28);

	VALIDATE(vm_executable, buffer_len, 0x2C);

	VALIDATE(vm_executable, strings, 0x30);
}

#include "my_patch.h"

void patch_vm_executable(void)
{
	PATCH_PUSH_RET_POLY(0x007E3EB0, vm_executable::vm_executable, "??0vm_executable@@QAE@PAVscript_object@@@Z");
	PATCH_PUSH_RET_POLY(0x007E4060, vm_executable::~vm_executable, "??1vm_executable@@QAE@XZ");


	PATCH_PUSH_RET(0x007E4370, vm_executable::_build_fullname);

	PATCH_PUSH_RET(0x007E41F0, vm_executable::_destroy);
	PATCH_PUSH_RET(0x007E4210, vm_executable::clear);
}
