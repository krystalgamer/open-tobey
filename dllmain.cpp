#include <windows.h>
#include "forwards.h"

// @TODO - remove when not needed
#include "SpideyTM/SRC/signals.h"

#include "SpideyTM/SRC/my_assertions.h"

HMODULE bink_dll;

void runtime_assertions()
{
	validate_stringx();
	validate_string_buf();
}

void runtime_patches()
{
	LPVOID text_start = (void*)0x401000;
	SIZE_T text_size = 0x896000 - (int)text_start;

	DWORD text_protect;
	VirtualProtect(text_start, text_size, PAGE_EXECUTE_READWRITE, &text_protect);

	patch_string_buf();

	DWORD t;
	VirtualProtect(text_start, text_size, text_protect, &t);
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpvReserved ) 
{
    // Perform actions based on the reason for calling.
    switch( fdwReason ) 
    { 
        case DLL_PROCESS_ATTACH:
			AllocConsole();
			freopen("CONOUT$", "w", stdout);

			puts("open-tobey starting");

			bink_dll = GetModuleHandle("binkw32.dll");
			runtime_assertions();
			runtime_patches();
            break;

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
