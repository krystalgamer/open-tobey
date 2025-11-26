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

// @Ok
// @Matching
extern "C" __declspec(naked) void _ftol()
{
	__asm
	{
		fld     st(0)
		fistp   [esp-8]
		fild    [esp-8]
		mov     edx, dword ptr [esp-4]

		mov     eax, dword ptr [esp-8]
		test    eax, eax

		jz short loc_82F8B6

		loc_82F886:
		fsubp   st(1), st

		test    edx, edx
		jns     short loc_82F8A4
		fstp    dword ptr [esp-8]
		mov     ecx, dword ptr [esp-8]

		xor     ecx, 80000000h
		add     ecx, 7FFFFFFFh
		adc     eax, 0
		retn

		loc_82F8A4:

		fstp    dword ptr [esp-8]
		mov     ecx, dword ptr [esp-8]
		add     ecx, 7FFFFFFFh

		sbb     eax, 0
		retn


		loc_82F8B6:

		test    edx, 7FFFFFFFh
		jnz     short loc_82F886
		fstp    st
		fstp    st
		retn
	}
}

extern "C" EXPORT int run_assertions(void)
{
	validate_stringx();
	validate_string_buf();

	validate_vm_stack();
	validate_vm_thread();

	validate_script_object_instance();

	validate_so_data_block();
	validate_vm_symbol();

	validate_os_file();

	validate_ini_parser();
	validate_os_developer_options();

	validate_signaller();

	validate_pstring();

	validate_vm_executable();
	validate_script_object();

	validate_error_context();

	validate_signal_callback();
	validate_script_callback();
	validate_code_callback();

	validate_signal();
	validate_signal_manager();
	validate_gated_signal();

	validate_script_manager();

	validate_text_file();

	validate_file_manager();
	validate_slc_manager();
	validate_script_library_class();

	validate_filespec();

	validate_slc_script_object_t();
	validate_slc_num_t();
	validate_slc_str_t();

	validate_hires_clock_t();
	validate_region();
	validate_light_source();
	validate_light_properties();

	validate_link_interface();
	validate_bone();

	validate_vector4d();
	validate_vector3d();

	validate_po();
	validate_matrix4x4();

	validate_entity_manager();
	validate_entity_id();
	validate_entity();

	validate_vr_pmesh();
	validate_visual_rep();

	validate_movement_info();
	validate_damage_interface();

	validate_bounded_attribute();
	validate_frame_info();

	validate_colgeom();
	validate_bone_interface();
	validate_entity_interface();

	validate_sound_interface();

	validate_shared_sound_group();
	validate_sound_group();

	validate_sg_entry();

	validate_game();
	validate_app();

	validate_motion_trail_info();
	validate_mt();

	validate_wds();

	validate_color();
	validate_entity_preload_pair();

	validate_camera();
	validate_marky_camera();

	validate_entity_anim();
	validate_entity_anim_tree();
	validate_ent_time_limit();
	
	validate_mcs();
	validate_controller();

	validate_box_trigger_interface();
	validate_convex_box();
	validate_bounding_box();

	validate_trigger();
	validate_point_trigger();
	validate_trigger_manager();

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

	patch_script_object_instance();

	patch_script_object();
	patch_error_context();
	patch_ectx();

	patch_signal_callback();
	patch_script_callback();
	patch_code_callback();

	patch_signal();

	patch_signal_manager();
	patch_gated_signal();
	patch_script_manager();

	patch_vm_executable();

	patch_text_file();
	patch_file_manager();
	patch_slc_manager();
	patch_script_library_class();

	patch_filespec();
	patch_str();
	patch_script_library_class_function();

	patch_slc_script_object_t();
	patch_slc_num_t();
	patch_slc_str_t();

	patch_hires_clock_t();
	patch_region();

	patch_light_source();
	patch_light_properties();

	patch_link_interface();
	patch_bone();

	patch_vector4d();
	patch_vector3d();

	patch_po();
	patch_matrix4x4();

	patch_entity_manager();
	patch_entity_id();
	patch_entity();

	patch_vr_pmesh();
	patch_visual_rep();

	patch_movement_info();

	patch_damage_interface();

	patch_frame_info();
	patch_colgeom();

	patch_sound_interface();
	patch_shared_sound_group();
	patch_sound_group();

	patch_sg_entry();

	patch_game();
	patch_app();

	patch_motion_trail_info();

	patch_wds();
	patch_color();

	patch_entity_preload_pair();
	patch_camera();

	patch_marky_camera();
	patch_entity_anim();
	patch_entity_anim_tree();

	patch_mcs();
	patch_controller();

	patch_box_trigger_interface();
	patch_convex_box();
	patch_bounding_box();

	patch_trigger();
	patch_point_trigger();
	patch_trigger_manager();
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
	
	/*
	const char **flavors = (const char**)0x008C4440;
	for (int i = 0; i < 29; i++)
	{
		const char *flavor = flavors[i];
		printf("ENTITY_%s,\n", flavor);
	}
	*/


	/*
	const char **entity_signals = (const char**)0x008C44F8;
	for (int i = 0; entity_signals[i] ; i++)
	{
		const char *cur = entity_signals[i];
		printf("MAC( %s, \"%s\" )\n", &cur[8], cur);
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

			dumper();
            break;

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
