#include <windows.h>
#include "forwards.h"

// @TODO - remove when not needed
#include "SpideyTM/SRC/signals.h"

#include "SpideyTM/SRC/my_assertions.h"

#include "my_validations.h"
#include "runtime_version.h"

#ifndef RUNTIME_VERSION
#define RUNTIME_VERSION "LOCAL"

#endif

HMODULE bink_dll;

void runtime_assertions()
{
	validate_stringx();
	validate_string_buf();

	validate_vm_stack();
	validate_vm_thread();

	// @TODO
	//validate_script_object_instance();

	validate_so_data_block();
	validate_vm_symbol();

	validate_os_file();

	do
	{
	} while (FAIL_VALIDATION);
}

void game_patches()
{
	patch_alloc();

	patch_string_buf();
	patch_stringx();

	patch_vm_stack();
	patch_vm_thread();
	patch_vm_symbol();

	patch_so_data_block();
}

void runtime_patches()
{
	LPVOID text_start = (void*)0x401000;
	SIZE_T text_size = 0x896000 - (int)text_start;

	DWORD text_protect;
	VirtualProtect(text_start, text_size, PAGE_EXECUTE_READWRITE, &text_protect);

	game_patches();

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
			SetConsoleTitle("open-tobey - " RUNTIME_VERSION);
			freopen("CONOUT$", "w", stdout);

			puts("open-tobey starting " RUNTIME_VERSION);

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
