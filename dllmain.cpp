#include <windows.h>
#include "forwards.h"

// @TODO - remove when not needed
#include "SpideyTM/SRC/signals.h"


BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpvReserved ) 
{
    // Perform actions based on the reason for calling.
    switch( fdwReason ) 
    { 
        case DLL_PROCESS_ATTACH:
            break;

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
