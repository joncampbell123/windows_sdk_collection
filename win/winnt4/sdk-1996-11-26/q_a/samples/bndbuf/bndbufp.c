/**********************************************************************
*
*   bndbufp.c - Contains the exported RPC interfaces for inserting and
*		removing strings from the central buffer pool.
*
*   Frederick Chong
*   Microsoft Developer Support
*   Copyright (c) 1992-1996 Microsoft Corporation
*
***********************************************************************/


#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <rpc.h>
#include "common.h"
#include "bndbuf.h"


// Functions prototype

void enter_item(char *input);
void remove_item(char *output);


// external variables

extern char buffer[MAX_ITEM][MAX_SIZE];     // Global buffer pool
extern HANDLE hMutex;			    // Handle to mutex
extern HANDLE hEmptySem;		    // Handle to empty semaphore
extern HANDLE hFullSem;			    // Handle to full semaphore
extern int pro_buf_pos; 		    // Producer buffer position
extern int con_buf_pos; 		    // Consumer buffer position


/*************************************************************************

void insert_buffer(unsigned char *item)

Synchronizes access to the central buffer pool for inserting a string.

**************************************************************************/

void insert_buffer(unsigned char *item)
{

    WaitForSingleObject(hEmptySem, INFINITE);	    // Decrease empty count
    WaitForSingleObject(hMutex, INFINITE);	    // Acquire Mutex
    enter_item(item);				    // Put item in buffer
    ReleaseMutex(hMutex);			    // Release Mutex
    ReleaseSemaphore(hFullSem, 1, NULL);	    // Increment full count

}



/*************************************************************************

void remove_buffer(unsigned char *item)

Synchronizes access to the the central buffer pool for removing a string.

**************************************************************************/

void remove_buffer(unsigned char *item)
{

    WaitForSingleObject(hFullSem, INFINITE);	    // Decrease full count
    WaitForSingleObject(hMutex, INFINITE);	    // Acquire Mutex
    remove_item(item);				    // Take item from buffer
    ReleaseMutex(hMutex);			    // Release Mutex
    ReleaseSemaphore(hEmptySem, 1, NULL);	    // Increment full count

}



/*************************************************************************

void enter_item(char *input)

Puts a string in the centralized buffer pool

We maintain two buffer pointers separately for the producer
and the consumer so that they can keep track of where they
are in the buffer queue.

		-----------------------------
Producer -->	|   |	|   ...     |	|   | --> Consumer
		|   |	|	    |	|   |
		-----------------------------
			^	    ^
			|	    |
		     current	    current
		     producer	    consumer
		     pointer	    pointer

**************************************************************************/


void enter_item(char *input)
{

    strncpy(buffer[pro_buf_pos], input, MAX_SIZE);
    printf("\nproduce    ---> %s", buffer[pro_buf_pos]);
    pro_buf_pos = ((++pro_buf_pos) % MAX_ITEM);
}


/*************************************************************************

void remove_item(char *output)

Removes a string from the centralized buffer pool.

**************************************************************************/

void remove_item(char *output)
{

    strncpy(output, buffer[con_buf_pos], 20);
    printf("\nconsume    ---> %s", output);
    con_buf_pos = ((++con_buf_pos) % MAX_ITEM);

}
