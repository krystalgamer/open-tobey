#include "stringx.h"

template<bool b>

struct StaticAssert{};



template<>

struct StaticAssert<true>
{
	static void sass() {}
};

void compile_time_assertions()
{
	StaticAssert<sizeof(stringx) == 8>::sass();
}
