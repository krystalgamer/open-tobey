#include "global.h"
#include "vm_executable.h"

#include "vm_executable.h"
#include "opcodes.h"
#include "script_object.h"
#include "script_library_class.h"
//!#include "character.h"
#include "wds.h"
#include "signals.h"

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


// @TODO - replace this when all usages are patched
bool preserved_old_stored_buf;
#define GET_PRESERVED_OLD_STORED_BUF (*reinterpret_cast<bool*>(0x00944BBC));
//#define GET_PRESERVED_OLD_STORED_BUF preserved_old_stored_buf

// @Ok
// @Matching
void vm_executable::link(const script_manager& sm)
{
	// @Patch - added this early return
	if (preserved_old_stored_buf)
	{
		this->linked = 1;
		return;
	}

	  if ( !linked )  // cannot perform link more than once!
    {
    linked = true;
    unsigned short opword;
    opcode_t op;
    opcode_arg_t argtype;
    unsigned short dsize;
    for (unsigned short* PC=buffer; PC<buffer+buffer_len; )
      {
      // decode operation word (opcode,argtype,dsize flag)
      opword = *PC++;

      op = opcode_t(opword >> 8);
      argtype = opcode_arg_t(opword & OP_ARGTYPE_MASK);
      dsize = 4;
      if (opword & OP_DSIZE_FLAG)
        {
        // next word is dsize
        dsize = *PC++;
        }
      // link SDR, LFR, SFR, and CLV references to appropriate addresses
      switch (argtype)

        {
        case OP_ARG_NULL:   // no argument
          break;
        case OP_ARG_NUM:    // constant numeric value (4 bytes)

        case OP_ARG_NUMR:   // constant numeric value used in reverse position (4 bytes; only applies to non-commutative operations)
          PC += 2;
          break;

        case OP_ARG_STR:    // constant string value (4 bytes)
          {
          // first word is string_id, second is unused until after link

          unsigned short id = *PC++;
          PC++;
          // run-time value of argument is pointer to string
          unsigned addr = int(strings[id]);
          // value must be stored as consecutive words
          *(PC-2) = addr >> 16;
          *(PC-1) = addr & 0x0000FFFF;
          }
          break;

        case OP_ARG_WORD:   // constant integer value (2 bytes)

        case OP_ARG_PCR:    // PC-relative address (2 bytes)
        case OP_ARG_SPR:    // SP-relative address (2 bytes)
        case OP_ARG_POPO:   // stack contents (pop) plus offset (2 bytes)

          PC++;
          break;
        case OP_ARG_SDR:    // static data member reference (4 bytes)
          {
          // first word is script object string_id
          unsigned short id = *PC++;
          script_object* so = sm.find_object(*strings[id]);
          assert(so);
          // second word is static data offset
          unsigned short offset = *PC++;
          // run-time address of reference is address of script_object static
          // data block plus given offset
          assert( offset < so->get_static_data_size() );
          unsigned addr = int(so->get_static_data_buffer() + offset);

          // value must be stored as consecutive words
          *(PC-2) = addr >> 16;
          *(PC-1) = addr & 0x0000FFFF;
          }
          break;
        case OP_ARG_SFR:    // function member reference (4 bytes)
          {

          // first word is script object string_id
          unsigned short id = *PC++;
          script_object* so = sm.find_object(*strings[id]);
          assert(so);

          // second word is script function index
          unsigned short idx = *PC++;
          // run-time address of reference is address of given member function
          vm_executable& ex = (vm_executable&)so->get_func( idx );
          // make sure the referenced function gets linked (necessary because
          // we now support partial linking of script executable modules)
          ex.link( sm );
          unsigned addr = int(&ex);
          // value must be stored as consecutive words
          *(PC-2) = addr >> 16;
          *(PC-1) = addr & 0x0000FFFF;
          }
          break;
        case OP_ARG_LFR:    // script library function reference (4 bytes)

          {
          // first word is library class string_id
          unsigned short id = *PC++;

		  // @Patch - dunno why this is here
		   if (id == 0x317)
		   {
			   __asm
			   {
				   int 3;
			   }
		   }
          const script_library_class* slc = slc_manager::inst()->find(*strings[id]);
          if (!slc)
            {
            stringx err;
            err="library class "+*strings[id]+" not found";
            error(err.c_str());
            }
          // second word is library function string_id

          id = *PC++;
          // run-time address of reference is address of given library function
          unsigned addr = int(slc->find(*strings[id]));
          if (!addr)
            {
            stringx err;
            err=stringx("library function ")+slc->get_name()+"::"+*strings[id]+" not found";
            error(err.c_str());
            }
          // value must be stored as consecutive words

          *(PC-2) = addr >> 16;
          *(PC-1) = addr & 0x0000FFFF;
          }
          break;
        case OP_ARG_CLV:    // class value reference (4 bytes)
          {
          // first word is library class string_id
          unsigned short id = *PC++;
          const script_library_class* slc = slc_manager::inst()->find(*strings[id]);

          if (!slc)
            {
            stringx err;
            err="library class "+*strings[id]+" not found";
            error(err.c_str());
            }
          // second word is class value string_id
          id = *PC++;
          // run-time address of reference is class instance (must be 4 bytes)
          assert(slc->get_size()==4);
          //((stringx *)strings[id])->to_upper();  // this is a string for entities, so it needed to be this all along...
          unsigned addr = slc->find_instance(*strings[id]);
          // value must be stored as consecutive words
          *(PC-2) = addr >> 16;
          *(PC-1) = addr & 0x0000FFFF;
          }
          break;
        case OP_ARG_SIG:    // global signal value (2 bytes)
        case OP_ARG_PSIG:   // member signal value (2 bytes)
          {
          // word is string_id of signal name
          unsigned short id = *PC++;
          // run-time value of argument is a non-unique 2-byte signal index local to the signaller
		  // @Patch - make copy instead of ref
          const stringx signame = stringx(*strings[id]);
          id = signal_manager::inst()->get_id( signame );
          *(PC-1) = id;
          }
          break;
        default:
          assert(0);
        }
      }
    }
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
	PATCH_PUSH_RET(0x007E45C0, vm_executable::link);
}
