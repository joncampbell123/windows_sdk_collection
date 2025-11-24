// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE: gdimeta.inl
//
//  PURPOSE: Inline functions
//
//  FUNCTIONS:
//    GetFName         - Get the current file name.
//
//  COMMENTS:
//

//
//  FUNCTION: GetFName(VOID)
//
//  PURPOSE: Get the current file name.
//
//  PARAMETERS:
//    NONE
//
//  RETURN VALUE:
//    The full path name of the current file.
//
//  COMMENTS:
//
//

extern char szFName[];

__inline LPSTR GetFName(VOID)
{
    return szFName;
}


