/*-------------------------------------------------------------------------------------------------------
	
	Error Context


	Provides a stack of strings that describe the current operating context of the program.
	The entire list of strings are printed out when an error occurs to better provide information on 
	where the error occurred.

	For convenience, make a local ectx object in your function to describe the operation of the function.
	When the function exits, the ectx object will be destroyed and the context removed.

	Be careful that you always remove your context.

-------------------------------------------------------------------------------------------------------*/
#ifndef ERROR_CONTEXT
#define ERROR_CONTEXT

#include "singleton.h"
#include "stringx.h"

#include <vector>

class error_context : public singleton
{
  private:
	  std::vector<stringx> context_stack;

	public:
		// @Patch - provide the already existing instance
		static inline error_context* inst()
		{
			return *reinterpret_cast<error_context**>(0x00948410);
		}

		enum { ECTX_STACK_SIZE = 64 };

		// @Ok
		// @Matching
		EXPORT error_context() 
		{
			context_stack.reserve(ECTX_STACK_SIZE);
		}

		// @Ok
		// @NotMatching - weird resize implementation
		EXPORT ~error_context()
		{
			context_stack.resize(0);
		}

		EXPORT void push_context( const stringx & context ) 
		{ 
			assert( context_stack.size() < ECTX_STACK_SIZE );
			context_stack.push_back(context); 
		}

		EXPORT inline void pop_context() { context_stack.pop_back(); }

		// @Patch - introduced static variable
		EXPORT static char error_message[1024];

		// @Ok
		// @Matching
		// @Patch - changed from stringx to const char*
		EXPORT const char* get_context() const
		{
			error_message[0] = '\0';
			for ( std::vector<stringx>::const_iterator it = context_stack.begin(); it != context_stack.end(); ++it )
			{
				strcat(error_message, "[");
				strcat(error_message, it->c_str());
				strcat(error_message, "]->");
			}

			return error_message;

		}

    inline std::vector<stringx> const & get_stack() { return context_stack; }

	friend void validate_error_context(void);
	friend void patch_error_context(void);
};

class ectx
{
	public:
		EXPORT ectx(const stringx & desc);

		EXPORT ~ectx();

};

#endif
