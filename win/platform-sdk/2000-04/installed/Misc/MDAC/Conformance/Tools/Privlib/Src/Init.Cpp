//--------------------------------------------------------------------
// Microsoft OLE DB Test
//
// Copyright 1995-1999 Microsoft Corporation.  
//
// @doc 
//
// @module INIT.CPP | 	This module is used to compile the OLE DB 
//						header files with DBINITCONSTANTS defined, 
//						This causes the OLE DB constants to be initialized
//						only once in the private library dll.
//
// @comm
// Special Notes...:	(OPTIONAL NOTES FOR SPECIAL CIRCUMSTANCES)
//
// <nl><nl>
// Revision History:<nl>
//	
//	[00] MM-DD-YY	EMAIL_NAME	ACTION PERFORMED... <nl>
//	[01] 10-16-95	Microsoft	Created <nl>
//	[02] 12-01-96	Microsoft	Updated for release <nl>
//
// @head3 INIT Elements|
//
// @subindex INIT
//
//---------------------------------------------------------------------------

#include "oledb.h"
#include "oledberr.h"
#include "transact.h"
#include <msdadc.h>
