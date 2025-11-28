//-----------------------------------------------------------------------------
// File: dxver.cpp
//
// Desc: Windows code for GetDXVersion() sample
//
//       This code calls GetDXVersion and displays the results
//
//
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <windows.h>

//-----------------------------------------------------------------------------
// External function-prototypes
//-----------------------------------------------------------------------------
extern void GetDXVersion(LPDWORD pdwDXVersion, LPDWORD pdwDXPlatform);

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and pops
//       up a message box with the results of the GetDXVersion call
//-----------------------------------------------------------------------------
int PASCAL
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpszCmdLine,
        int nCmdShow)
{
    char    szBuff[256];
    DWORD   dwDXVersion;
    DWORD   dwDXPlatform;

    GetDXVersion(&dwDXVersion, &dwDXPlatform);
    switch (dwDXPlatform)
    {
        case VER_PLATFORM_WIN32_WINDOWS:
            strcpy(szBuff,"OS:\tWindows 9x\n");
            break;
        case VER_PLATFORM_WIN32_NT:
            strcpy(szBuff,"OS:\tWindows NT\n");
            break;
        default:
            strcpy(szBuff,"Error!\n");
    }
    switch (dwDXVersion)
    {
        case 0x000:
            strcat(szBuff,"Dx:\tNo DirectX installed");
            break;
        case 0x100:
            strcat(szBuff,"Dx:\tDirectX 1");
            break;
        case 0x200:
            strcat(szBuff,"Dx:\tDirectX 2");
            break;
        case 0x300:
            strcat(szBuff,"Dx:\tDirectX 3");
            break;
        case 0x500:
            strcat(szBuff,"Dx:\tDirectX 5");
            break;
        case 0x600:
            strcat(szBuff,"Dx:\tDirectX 6 or better");
            break;
        default:
            strcat(szBuff,"Unknown version of DirectX installed.");
    }
    MessageBox(NULL, szBuff, "DirectX Version:", MB_OK | MB_ICONINFORMATION);
    return 0;
}

