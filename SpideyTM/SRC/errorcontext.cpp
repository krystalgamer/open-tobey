#include "global.h"
#include "errorcontext.h"
//P #include "memorycontext.h"

//! DEFINE_AUTO_SINGLETON(error_context)
//DEFINE_SINGLETON(error_context)
//P memory_context g_memory_context;
//P bool memory_context::initialized = false;

static void my_create_errorcontext_instance(void)
{
	error_context** ptr = (error_context**)0x00948410;

	*ptr = NEW error_context;
}

char error_context::error_message[1024];

// @Ok
// @NotMatching - same but offsets are always +4 -4 for some reason
ectx::ectx(const stringx & desc) 
{
  error_context::inst()->push_context(desc); 
}
	
// @Ok
// @NotMatching - same but offsets are always +4 -4 for some reason
ectx::~ectx() 
{
  error_context::inst()->pop_context(); 
}

#include "my_assertions.h"
static void compile_time_assertions()
{
	StaticAssert<sizeof(error_context) == 0x10>::sass();
}

void validate_error_context(void)
{
	VALIDATE_SIZE(error_context, 0x10);

	VALIDATE(error_context, context_stack, 0x4);
}

#include "my_patch.h"
void patch_error_context(void)
{
	PATCH_PUSH_RET(0x0082F1A0, my_create_errorcontext_instance);

	PATCH_PUSH_RET_POLY(0x0082F2B0, error_context::~error_context, "??1error_context@@UAE@XZ");

	PATCH_PUSH_RET(0x007D00C0, error_context::get_context);
	PATCH_PUSH_RET(0x005143E0, error_context::push_context);
}


void patch_ectx(void)
{
	PATCH_PUSH_RET_POLY(0x007CFF00, ectx::ectx, "??0ectx@@QAE@ABVstringx@@@Z");
	PATCH_PUSH_RET_POLY(0x007D0090, ectx::~ectx, "??1ectx@@QAE@XZ");
}
