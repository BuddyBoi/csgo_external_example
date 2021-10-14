#include <iostream>
#include <thread>
#include <Windows.h>
#include <TlHelp32.h>

//Perfect function from guidedhacking. No need to change it or make your own
uintptr_t module_base_address( DWORD procId, const wchar_t* modName )
{
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId );
    if ( hSnap != INVALID_HANDLE_VALUE )
    {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof( modEntry );
        if ( Module32First( hSnap, &modEntry ) )
        {
            do
            {
                if ( !_wcsicmp( modEntry.szModule, modName ) )
                {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while ( Module32Next( hSnap, &modEntry ) );
        }
    }
    CloseHandle( hSnap );
    return modBaseAddr;
}

//Define our offsets in their own namespace as it's good practice
namespace offsets
{
    DWORD local_player = 0xDA344C;
    DWORD flash_max_alpha = 0x1046C;
}

int main()
{
    //Get a window handle to csgo
    HWND hw_csgo = FindWindowA( NULL, "Counter-Strike: Global Offensive" );
    DWORD dw_process_id, dw_client_address;

    //Get csgo's process ID using the handle (No need to make some big function for procid like every other paster does)
    GetWindowThreadProcessId( hw_csgo, &dw_process_id );

    //Get base address of the client module
    DWORD dw_client_address = module_base_address( dw_process_id, L"client.dll" );
    
    //Open a handle for reading a writing
    HANDLE h_csgo = OpenProcess( FILE_ALL_ACCESS, false, dw_process_id );

    //Get local player address
    uintptr_t local_player;
    ReadProcessMemory( h_csgo, (LPCVOID)(dw_client_address + offsets::local_player), &local_player, sizeof( local_player ), nullptr );
    
    //WPM and RPM require defined data types so that memory is not overloaded
    float new_flash = 0.0f;

    //Create a loop so that our feature is always running
    while ( true )
    {
        //Write to the memory address
        //Pro tip. Make a template for reading and writing. Make work quicker and custom structs easier
        WriteProcessMemory( h_csgo, (LPVOID)(local_player + offsets::flash_max_alpha), &new_flash, sizeof( new_flash ), nullptr );
        Sleep( 50 );
    }

    return 0;
}
