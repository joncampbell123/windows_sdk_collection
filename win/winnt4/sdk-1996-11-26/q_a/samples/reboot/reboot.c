// ************************************************************************
//
//                      Microsoft Developer Support
//             Copyright (c) 1992-1996 Microsoft Corporation
//
// ************************************************************************
// MODULE    : Reboot.C
// PURPOSE   : A Reboot Win32 Console Application Source File
// FUNCTIONS :
//  main                - application-defined entry point
//  SetCurrentPrivilege - enables/disables a privilege for caller
//  DisplayError        - displays an API error message
// COMMENTS  : none.
// ************************************************************************

#define   STRICT               // enable strict typing
#include <windows.h>           // required for all Windows applications
#include <stdio.h>             // printf()

#ifdef UNICODE

#define MYMAIN wmain // Unicode main
#else

#define MYMAIN main  // Ansi main
#endif

// internal function prototypes
// --------------------------------------------------------------------------
BOOL
SetCurrentPrivilege(
    LPTSTR MachineName,     // target machine name
    LPCTSTR Privilege,      // Privilege to enable/disable
    BOOL bEnablePrivilege   // to enable or disable privilege
    );

void
DisplayError(
    LPSTR
    );

// ************************************************************************
// FUNCTION : main( int, char** )
// PURPOSE  : main is called by the true application entry point
//            mainCRTStartup right after the C Run-Time libaries are
//            initialized when the executable is first loaded.  That is,
//            main is the application-defined entry point.
// COMMENTS : none.
// ************************************************************************
int _cdecl
MYMAIN(int argc, LPTSTR argv[])
{
    LPTSTR lpPrivilege;
    int iReturnValue;   // return value from main()

    // if Win32s or Windows 95, display notice and terminate
    if(GetVersion() & 0x80000000) {
        MessageBoxA(
            NULL,
            "This application cannot run on Windows 3.1 or Windows 95.\n"
            "This application will now terminate.",
            "REBOOT",
            MB_OK | MB_ICONSTOP | MB_SETFOREGROUND
            );

        return 1;
    }

    if(argc != 2) {
        printf("Reboot Program, Version 1.1\n");
        printf("Copyright (c) 1992-1996 Microsoft Corp.\n");

        printf("\nSyntax: reboot \\\\MachineName OR . for local machine\n");
        return 1;
    }

    lpPrivilege = SE_SHUTDOWN_NAME;

    //
    // enable the appropriate privilege
    //
    if(!SetCurrentPrivilege(argv[1], lpPrivilege, TRUE)) {
        DisplayError("SetCurrentPrivilege");
        return 2;
    }

    printf("Attempting to shutdown %s\n", argv[1]);

    //
    // attempt to shutdown the target machine
    // since we use a non-zero shutdown timeout, we could abort the shutdown
    // via the Win32 AbortSystemShutdown() API
    //
    if(!InitiateSystemShutdown(
        argv[1], // machine name
        NULL,    // dialog box message
        10,      // shutdown timeout
        TRUE,    // force apps closed
        TRUE     // reboot
        )) {

        DisplayError("InitiateSystemShutdown");
        iReturnValue = 2; // error
    }
    else {
        printf( "InitiateSystemShutdown successful.\n");
        iReturnValue = 0; // success
    }

    //
    // disable the privilege
    //
    SetCurrentPrivilege(argv[1], lpPrivilege, FALSE);

    return iReturnValue;
}

//
// FUNCTION : SetCurrentPrivilege
// PURPOSE  : Enables or disables a privilege in the calling process, based
//            on whether the caller has this privilege on the target machine
// COMMENTS : none.
//
BOOL
SetCurrentPrivilege(
    LPTSTR MachineName,     // target machine name
    LPCTSTR Privilege,      // Privilege to enable/disable
    BOOL bEnablePrivilege   // to enable or disable privilege
    )
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    LUID luid;
    TOKEN_PRIVILEGES tpPrevious;
    DWORD cbPrevious=sizeof(TOKEN_PRIVILEGES);
    BOOL bSuccess=FALSE; // assume this function will fail

    //
    // obtain the Luid of the privilege from the target computer
    //
    if(!LookupPrivilegeValue(MachineName, Privilege, &luid)) return FALSE;

    //
    // open our access token, which we read and modify later
    //
    if(!OpenProcessToken(
            GetCurrentProcess(),
            TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES,
            &hToken
            )) return FALSE;

    //
    // first pass.  get current privilege setting
    //
    tp.PrivilegeCount           = 1;
    tp.Privileges[0].Luid       = luid;
    tp.Privileges[0].Attributes = 0;

    AdjustTokenPrivileges(
            hToken,
            FALSE,
            &tp,
            sizeof(TOKEN_PRIVILEGES),
            &tpPrevious,
            &cbPrevious
            );

    if(GetLastError() == ERROR_SUCCESS) {
        //
        // second pass.  set privilege based on previous setting
        //
        tpPrevious.PrivilegeCount     = 1;
        tpPrevious.Privileges[0].Luid = luid;

        if(bEnablePrivilege)
            tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
        else
            tpPrevious.Privileges[0].Attributes ^= (SE_PRIVILEGE_ENABLED &
                tpPrevious.Privileges[0].Attributes);

        AdjustTokenPrivileges(
                hToken,
                FALSE,
                &tpPrevious,
                cbPrevious,
                NULL,
                NULL
                );

        if (GetLastError() == ERROR_SUCCESS) bSuccess=TRUE;
    }

    CloseHandle(hToken);

    return bSuccess;
}

//
// FUNCTION : DisplayError(LPSTR, DWORD)
// PURPOSE  : Displays the error text associated with GetLastError()
// COMMENTS : none.
//
void
DisplayError(LPSTR szAPI)
{
    LPSTR MessageBuffer;
    DWORD dwBufferLength;

    fprintf(stderr,"%s error!\n", szAPI);

    if(dwBufferLength=FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM,
                NULL,
                GetLastError(),
                GetUserDefaultLangID(),
                (LPSTR) &MessageBuffer,
                0,
                NULL
                ))
    {
        DWORD dwBytesWritten;

        //
        // Output message string on stderr
        //
        WriteFile(
                GetStdHandle(STD_ERROR_HANDLE),
                MessageBuffer,
                dwBufferLength,
                &dwBytesWritten,
                NULL
                );

        //
        // free the buffer allocated by the system
        //
        LocalFree(MessageBuffer);
    }
}
