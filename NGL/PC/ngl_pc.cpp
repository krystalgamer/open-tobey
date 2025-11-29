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

// @TODO
void nglSetFogRange(float Near, float Far, float Min, float Max)
{
	typedef void (*func_ptr)(float, float, float, float);
	func_ptr func = (func_ptr)0x008033B0;
	func(Near, Far, Min, Max);
}

// @TODO
void nglSetFogColor(float R, float G, float B)
{
	typedef void (*func_ptr)(float, float, float);
	func_ptr func = (func_ptr)0x00803310;

	func(R, G, B);
}
int nglGetScreenWidth()
{
	typedef int (*func_ptr)();
	func_ptr func = (func_ptr)0x00801E30;
	return func();
}

int nglGetScreenHeight()
{
	typedef int (*func_ptr)();
	func_ptr func = (func_ptr)0x00801E50;
	return func();
}
