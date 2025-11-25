// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1995-1996  Microsoft Corporation.  All Rights Reserved.
//
// PURPOSE:
//    Contains public declarations for the EditCtls module.
//

BOOL InitEditCtls(HWND hWndParent);
void SizeEditCtls();
void SetFocusEditCtls();

void WriteToDisplayCtl(LPSTR lpNewString, DWORD dwSizeofNewString);
BOOL PostWriteToDisplayCtl(LPSTR lpNewString, DWORD dwSizeofNewString);


