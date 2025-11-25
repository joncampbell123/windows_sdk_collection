#define WIN32_LEAN_AND_MEAN         // the bare essential Win32 API
#include <windows.h>
#include <httpext.h>

//
// The DLL's name is used to determine which EXE to launch in
// response to a server request.  In LibMain, the path name of
// the DLL is used to determine the EXE name.  The DLL extension
// is stripped, and an EXE extension is added.
//
// This approach avoids problems when passing in the CGI name from
// the client.  No other programs are allowed to run.  The other approach,
// using one copy of this DLL for all Windows CGI apps introduces the
// ability to run anything.
//
// Other approaches to this problem include using the registry
// or some other untouchable configuration file--untouchable from
// the web client that is.
//

//
// DllEntryPoint also allows us to initialize our state variables.
// You might keep state information, as the DLL often remains
// loaded for several client requests.  The server may choose 
// to unload this DLL, and you should save your state to disk,
// and reload it here.  DllEntryPoint is called for both
// loading and unloading.  See the Win32 SDK for more info
// on how DLLs load and unload.
//

TCHAR gszAppName[MAX_PATH];

BOOL WINAPI DllMain (HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpv)
    {
    int nLen;

    switch (dwReason)
        {
        case DLL_THREAD_ATTACH:
        case DLL_PROCESS_ATTACH:
            {

            // Get our DLL's name, abort if this fails
            if (!GetModuleFileName (hinstDLL, gszAppName, MAX_PATH))
                return TRUE; //return FALSE;

            // Strip DLL extension, add EXE extension
            nLen = lstrlen (gszAppName);
            if (nLen > 4)
                {
                if (!lstrcmpi (&gszAppName[nLen - 4], TEXT(".DLL")))
                    {
                    gszAppName[nLen - 4] = 0;
                    lstrcat (gszAppName, TEXT(".EXE"));
                    }
                }

            break;
            }
        }
    return TRUE;
    }

