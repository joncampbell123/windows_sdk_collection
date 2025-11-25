/***
*cinitclk.c - Contains the init code pointer for clock
*
*       Copyright (c) 1987-1992, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       define the entry in initializer table, __pinittime is referenced in
*       clock.c, if clock gets used, then this code will be pulled into
*       XI$C segment.
*
*******************************************************************************/
#include <fltintrn.h>            //PFV definition
#include <internal.h>            //prototype for _inittime

#pragma data_seg(".CRT$XIC")

const PFV __pinittime = _inittime;

