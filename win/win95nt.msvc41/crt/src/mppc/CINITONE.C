/***
*cinitone.c - Contains the init code pointer for onexit
*
*       Copyright (c) 1987-1992, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       define the entry in initializer table, __ponexitinit is referenced in
*       onexit.c, if _onexit gets used, then this code will be pulled into
*       XI$C segment.
*
*******************************************************************************/
#include <fltintrn.h>            //PFV definition
#include <internal.h>            //prototype

#pragma data_seg(".CRT$XIC")

const PFV __ponexitinit = _onexitinit;


