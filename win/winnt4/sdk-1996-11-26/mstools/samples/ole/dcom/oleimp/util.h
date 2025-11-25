// ===========================================================================
// File: U T I L . H
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
// 
// Copyright 1996 Microsoft Corporation.  All Rights Reserved.
// ===========================================================================

BOOL UtilAllocMem (PVOID *ppv, DWORD cb);
void UtilFreeMem (PVOID pv);
BOOL GetUserSidFromToken (HANDLE hToken, PSID *ppSid);
BOOL GetGroupSidFromToken (HANDLE hToken, PSID *ppSid);
BOOL CreateAppSecurityDescriptor(PSECURITY_DESCRIPTOR *ppsd);
BOOL FreeAppSecurityDescriptor (PSECURITY_DESCRIPTOR psd);
void ErrorMessage(HWND hwnd, LPCTSTR szFunction, HRESULT hr);

// EOF =======================================================================
