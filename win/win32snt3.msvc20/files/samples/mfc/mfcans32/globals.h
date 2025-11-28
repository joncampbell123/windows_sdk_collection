//+--------------------------------------------------------------------------
//
//  Copyright (c) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       Globals.h
//
//  Contents:   Define and Initialize global variables.
//
//  History:    15-Jun-94   'Hammer'    Created.
//
//---------------------------------------------------------------------------


//
//  Define global macros.
//
#define OBJDESC_CF(cf) \
	((cf) == g_cfLinkSourceDescriptor || (cf) == g_cfObjectDescriptor)


//
//  Define clipboard formats constants.
//
extern CLIPFORMAT g_cfLinkSourceDescriptor;
extern CLIPFORMAT g_cfObjectDescriptor;


//
//  Define defined functions.
//
extern BOOL GlobalInit(void);
