/***
*shellrtn.c
*
*   Copyright (c) 1989-1995, Microsoft Corporation. All rights reserved.
*
*Purpose:
*
************************************************************************/
#include <macos\segload.h>

void _ShellReturn()
{
        ExitToShell();
}
