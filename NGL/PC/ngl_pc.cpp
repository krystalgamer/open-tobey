#include "..\..\SpideyTM\SRC\types.h"
#include "..\..\SpideyTM\SRC\my_export.h"
#include "ngl_pc.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


// @TODO
void nglPrintf(const char *Format, ...)
{
	OutputDebugStringA("it's me mario");
}
