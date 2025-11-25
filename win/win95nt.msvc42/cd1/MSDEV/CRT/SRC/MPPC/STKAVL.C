/***
*assert.c - Display a message and abort
*
*       Copyright (c) 1988-1991, Microsoft Corporation. All rights reserved.
*
*Purpose:
*
*******************************************************************************/

#include <macos\memory.h>
#include <malloc.h>

size_t _stackavail()
        {
        return StackSpace();
        }
