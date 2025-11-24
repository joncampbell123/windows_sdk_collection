/************************************************************************
*
*   bndbufs.c - sample program demonstrating the server initialization of
*       the distributed bounded buffer solution
*
*   Frederick Chong
*   Microsoft Developer Support
*   Copyright (c) 1992-1995 Microsoft Corporation
*
*************************************************************************/


#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <rpc.h>
#include "common.h"
#include "bndbuf.h"


// Synchronization primitives to shared buffer

extern HANDLE hMutex;       // Handle to mutex
extern HANDLE hEmptySem;    // Handle to empty semaphore
extern HANDLE hFullSem;     // Handle to full semaphore


// This is the shared buffer

extern char buffer[MAX_ITEM][MAX_SIZE];


DWORD retCode;


/************************************************************************

void Usage(char * pszProgramName)

prints out the command line options for starting the server

*************************************************************************/

void Usage(char * pszProgramName)
{
    fprintf(stderr, "Usage:  %s\n", pszProgramName);
    fprintf(stderr, " -p protocol_sequence\n");
    fprintf(stderr, " -e endpoint\n");
    ExitProcess(1);
}


/*************************************************************************

void main(int argc, char *argv[])

Parses the command line arguments which allow the user to override the
default protocol sequence and endpoint. It then initializes the
synchronization primitives to be used for the central buffer pool.
Finally, it registers its interface with the name service and then
listens for an incoming call.

**************************************************************************/

void main(int argc, char *argv[])
{

    RPC_STATUS status;
    RPC_BINDING_VECTOR * pBindingVector;

    unsigned char * pszEndpoint         = "\\pipe\\boundbuf";
    unsigned char * pszProtocolSequence     = "ncacn_np";

    int i;
    
    // This program only runs on Windows NT
    if (GetVersion() & 0x80000000)
    {
        MessageBox(NULL,
            "This application cannot run on Windows 3.1 or Windows 95\n"
            "This application will now terminate.",
            "BndBuf",
            MB_OK | MB_ICONSTOP | MB_SETFOREGROUND);
        return;        
    }
    

    // allow the user to override settings with command line switches
    for (i = 1; i < argc; i++)
    {
    if ((*argv[i] == '-') || (*argv[i] == '/'))
    {
        switch (tolower(*(argv[i]+1)))
        {
        case 'p':  // protocol sequence
            pszProtocolSequence = argv[++i];
            break;
        case 'e':
            pszEndpoint = argv[++i];
            break;
        case 'h':
        case '?':
        default:
            Usage(argv[0]);
        }
     }

        else
        Usage(argv[0]);
    }




    // Create semaphores and mutex for synchronization

    hMutex = CreateMutex(NULL,
             FALSE,
             "Buffer_Mutex");

    if (hMutex == NULL)
    {
        retCode = GetLastError();
        printf("\nCreateMutex returns error %d\n", retCode);
        ExitProcess(0);
    }


    if ((hEmptySem = CreateSemaphore(NULL,
                    MAX_ITEM,
                    MAX_ITEM,
                    "Empty_Sem")) == NULL)

    {
        retCode = GetLastError();
        printf("\nCreateSemaphore for empty returns error %d\n", retCode);
        ExitProcess(0);
    }


    if ((hFullSem = CreateSemaphore(NULL,
                    0,
                    MAX_ITEM,
                    "Full_Sem")) == NULL)

    {
        retCode = GetLastError();
        printf("\nCreateSemaphore for full returns error %d\n", retCode);
        ExitProcess(0);
    }



// Initialize RPC Interface server


    status = RpcServerUseProtseqEp((unsigned char *)pszProtocolSequence,
                   MAXCALLS,
                   pszEndpoint,
                   0);
    printf("RpcServerUseProtseqEp returned %d\n", status);
    if (status)
    ExitProcess(2);


    status = RpcServerRegisterIf(bndbufh_ServerIfHandle,
                 0,
                 0);
    printf("RpcServerRegisterIf returned %d\n", status);
    if (status)
    ExitProcess(2);


    status = RpcServerInqBindings(&pBindingVector);
    printf("RpcServerInqBindings returned %d\n", status);
    if (status)
    ExitProcess(2);


    status = RpcNsBindingExport(RPC_C_NS_SYNTAX_DEFAULT,    /* name syntax */
                "/.:/Boundbuf_sample",      /* name */
                bndbufh_ServerIfHandle,
                pBindingVector,
                NULL);
    printf("RpcNsBindingExport returned %d\n", status);
    if (status)
    ExitProcess(2);


    printf("Calling RpcServerListen\n");
    status = RpcServerListen(1,
                 MAXCALLS,
                 0);
    printf("RpcServerListen returned %d\n", status);
    if (status)
    ExitProcess(2);

}


/**************************************************************************
        MIDL allocate and free
***************************************************************************/

void __RPC_FAR * __RPC_API MIDL_user_allocate(size_t len)
{
    return(malloc(len));
}

void __RPC_API MIDL_user_free(void __RPC_FAR * ptr)
{
    free(ptr);
}
