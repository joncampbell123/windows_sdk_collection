/**********************************************************************
*   bndbufc.c -- sample program demonstrating two client threads
*        producing to and consuming from a central buffer
*        pool via RPC.
*
*   Frederick Chong
*   Microsoft Developer Support
*   Copyright (c) 1992, 1993 Microsoft Corporation
*
***********************************************************************/


#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <rpc.h>
#include "common.h"
#include "bndbuf.h"

// Function prototypes

void producer();
void consumer();
void produce_item(char * string);
void _CRTAPI1 main(int argc, char *argv[]);

// Global variables

HANDLE hMainThrd;   // Handle to main thread
HANDLE hConThrd;    // Handle to consumer thread
HANDLE hProdThrd;   // Handle to producer thread
LONG   lConThreadId;    // Consumer thread ID
LONG   lProdThreadId;   // Producer thread ID
DWORD  retCode;


/**********************************************************************

void _CRTAPI1 main(int argc, char *argv[])

Creates the producer and consumer thread and then suspend itself.

***********************************************************************/

void _CRTAPI1 main(int argc, char *argv[])
{


    // Create Consumer Thread

    hConThrd = CreateThread(NULL,
                0,
                (LPTHREAD_START_ROUTINE)consumer,
                NULL,
                0,
                (LPDWORD) &lConThreadId );


    // Create producer thread

    hProdThrd = CreateThread(NULL,
                 0,
                 (LPTHREAD_START_ROUTINE)producer,
                 NULL,
                 0,
                 (LPDWORD) &lProdThreadId );


    // Suspend the main thread

    hMainThrd = GetCurrentThread();

    if (SuspendThread(hMainThrd) == -1)
    {
        retCode = GetLastError();
        printf("\nSuspendThread returns %d\n", retCode);
        ExitProcess(0);
    }




}


/***********************************************************************

void producer()

This is the producer thread function. It produces a string of 20 random
characters then make a RPC to insert the string item.

************************************************************************/

void producer()
{
    char item[MAX_SIZE];

    // Delay for awhile before starting
    Sleep(200);

    while (TRUE)
    {
    produce_item(item);     // Produce random string


    // RPC guarded statements

    RpcTryExcept
    {
        insert_buffer(item);    // Put item in buffer
        printf("\nproduce   ---> %s", item);
    }
    RpcExcept(1)
    {
        printf("The RPC runtime library raised exception 0x%lx.\n", RpcExceptionCode());
        printf("Please verify that the server application and \n");
        printf("the locator service have been started.");
    }
    RpcEndExcept

    // End RPC guarded statements

    }
}


/***********************************************************************

void consumer()

This is the consumer thread function. It makes a RPC and then remove
the string item from the central buffer pool.

************************************************************************/


void consumer()
{
    char item[20];

    while (TRUE)
    {

    // RPC guarded statements

    RpcTryExcept
    {
        remove_buffer(item);        // Take item from buffer
        printf("\nconsume   ---> %s", item);
    }

    RpcExcept(1)
    {
        printf("The RPC runtime library raised exception %lx.\n", RpcExceptionCode());
        printf("Please verify that the server application and \n");
        printf("the locator service have been started.");
    }
    RpcEndExcept

    // End RPC guarded statements


    }


}


/***********************************************************************

void produce_item(char *string)

Generates a random string of 20 characters.

************************************************************************/

void produce_item(char *string)
{
    char *tmp;
    int i;

    tmp = string;

    for (i = MAX_SIZE; i > 1; i--)
    {
        *tmp = (rand() % 26) + 'A';
        tmp++;
    }
    strcpy(tmp, "\0");

}



/***********************************************************************
    MIDL allocate and free
***********************************************************************/

void __RPC_FAR * __RPC_API MIDL_user_allocate(size_t len)
{
    return(malloc(len));
}

void __RPC_API MIDL_user_free(void __RPC_FAR * ptr)
{
    free(ptr);
}
