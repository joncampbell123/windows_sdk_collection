//+--------------------------------------------------------------------------
//
//  Copyright (c) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       Globals.cpp
//
//  Contents:   Define and Initialize global variables.
//
//  Functions:  GlobalInit
//
//  History:    15-Jun-94   'Hammer'    Created.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"


//
//  Define clipboard formats constants.
//
CLIPFORMAT  g_cfLinkSourceDescriptor;
CLIPFORMAT  g_cfObjectDescriptor;

#pragma code_seg(".text$initseg")

//+--------------------------------------------------------------------------
//
//  Routine:    GlobalInit
//
//  Synopsis:   Initializes global variables.
//
//  Returns:    TRUE if function successful.
//
//---------------------------------------------------------------------------
BOOL GlobalInit(void)
{
	g_cfLinkSourceDescriptor = RegisterClipboardFormat("Link Source Descriptor");
	g_cfObjectDescriptor     = RegisterClipboardFormat("Object Descriptor");

	return TRUE;
}

#pragma code_seg(".orpc")
