//*******************************************************************************************
//
// Filename : Pch.h
//	
//				Common header file 
//
// Copyright (c) 1994 - 1996 Microsoft Corporation. All rights reserved
//
//*******************************************************************************************

#pragma warning(disable: 4001 4100 4201 4204 4209 4214 4226)
#pragma warning(disable: 4514 4699 4705)

#define STRICT 
#include <windows.h>

#pragma warning(disable: 4001 4100 4201 4204 4209 4214 4226)
#pragma warning(disable: 4514 4699 4705)
// Need to turn off warnings again after windows.h

#include <windowsx.h>
#include <shlobj.h>
#include <shlguid.h>

#include "dpda.h"		
#include "Cabobj.h"
#include "Cabvw2.h"
#include "Cabp.h"

#define FIELDOFFSET(type, field)    ((int)(&((type NEAR*)1)->field)-1)
