#ifndef MY_PANIC_H
#define MY_PANIC_H

#pragma once

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define PANIC_AT TOSTRING(__FILE__) ":" TOSTRING(__LINE__)
#define PANIC { OutputDebugStringA("IMMA GONNA KILLMYSELF" PANIC_AT); __asm { int 3 }; exit(1); }
#define LEAN_PANIC { __asm { int 3 }; }

#endif
