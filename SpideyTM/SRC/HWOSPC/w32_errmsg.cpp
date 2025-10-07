#include "w32_errmsg.h"

// @TODO
void error(const stringx& str)
{
	OutputDebugStringA(str.c_str());
	exit(-1);
}

// @TODO
void error(const char* fmtp, ...)
{
}

// @TODO
void warning(const stringx& str )
{
}
