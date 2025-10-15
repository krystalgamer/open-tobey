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

extern "C" EXPORT int run_assertions(void)
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

	validate_ini_parser();
	validate_os_developer_options();

	validate_signaller();

	validate_pstring();

	return FAIL_VALIDATION;
}

void runtime_assertions()
{
	int result = run_assertions();

	while(result)
		;
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
	patch_os_file();
	patch_ini_parser();
	patch_os_developer_options();

	patch_signaller();

	patch_pstring();
}

#pragma pack(push,1)
struct Thunk
{
	unsigned char jmp; //always 0xE9
	DWORD offset;
};

struct MyPatch
{
	unsigned char push; //always 0x68
	DWORD finalAddress;
	unsigned char ret; //always 0xC3
};
#pragma pack(pop)

void optimize_thunks()
{
	StaticAssert<sizeof(Thunk) == 5>::sass();
	StaticAssert<sizeof(MyPatch) == 6>::sass();


	Thunk *pThunk = (Thunk*)0x00401005;
	int counter = 0;

	for (int i = 0; ; i++)
	{
		if (pThunk[i].jmp != 0xE9)
		{
			printf("Stopping the optimizer at (%08X):(%d)\n", &pThunk[i], i);
			break;
		}

		DWORD dest = pThunk[i].offset + (DWORD)&pThunk[i] + 5;


		MyPatch *pPatch = (MyPatch*)dest;

		if (pPatch->push != 0x68)
		{
			continue;
		}

		if (pPatch->ret != 0xC3)
		{
			continue;
		}

		pThunk[i].offset = pPatch->finalAddress - (DWORD)&pThunk[i] - 5;
		counter++;
	}

	printf("Optimized %d thunks\n", counter);
}

void runtime_patches()
{
	LPVOID text_start = (void*)0x401000;
	SIZE_T text_size = 0x896000 - (int)text_start;

	DWORD text_protect;
	VirtualProtect(text_start, text_size, PAGE_EXECUTE_READWRITE, &text_protect);

	game_patches();

	optimize_thunks();

	DWORD t;
	VirtualProtect(text_start, text_size, text_protect, &t);
}

void dumper()
{
	/*
	const char **flag_names = (const char**)0x008DE560;
	int num_flags = 102;
	const int* flag_defaults = (const int*)0x008DE6F8;

	for (int i = 0; i < num_flags; i++)
	{
		const char *flag_name = flag_names[i];
		printf("MAC(FLAG_%s, \"%s\", %d)\n", flag_name, flag_name, flag_defaults[i]);
	}
	*/

	/*
	const char **int_names = (const char**)0x008DE8E0;
	int num_ints = 17;
	const int* int_defaults = (const int*)0x008DE928;

	for (int i = 0; i < num_ints; i++)
	{
		const char *int_name = int_names[i];
		printf("MAC(INT_%s, \"%s\", %d)\n", int_name, int_name, int_defaults[i]);
	}
	*/

	/*
	const char **string_names = (const char**)0x008DE890;
	int num_strings = 10;
	const char** string_defaults = (const char**)0x008DE8B8;

	for (int i = 0; i < num_strings; i++)
	{
		const char *string_name = string_names[i];
		printf("MAC(STRING_%s, \"%s\", \"%s\")\n", string_name, string_name, string_defaults[i]);
	}
	*/

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

			if(GetModuleHandle("tobey_validator.exe") != NULL)
			{
				puts("In validator");
				break;
			}

			AllocConsole();
			SetConsoleTitle("open-tobey - " RUNTIME_VERSION);
			freopen("CONOUT$", "w", stdout);


			puts("open-tobey starting " RUNTIME_VERSION);

			bink_dll = GetModuleHandle("binkw32.dll");
			runtime_assertions();
			runtime_patches();

			//dumper();
            break;

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
