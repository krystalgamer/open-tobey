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
	OutputDebugStringA("ERROR RIGHT NOW");
	OutputDebugStringA(fmtp);
	exit(-1);
}

// @TODO
void warning(const stringx& str )
{
	OutputDebugStringA("WARNING BOY");
	OutputDebugStringA(str.c_str());
}

// @TODO
void warning(const char* fmtp, ...)
{
	OutputDebugStringA("WARNING BOY");
	OutputDebugStringA(fmtp);
}
