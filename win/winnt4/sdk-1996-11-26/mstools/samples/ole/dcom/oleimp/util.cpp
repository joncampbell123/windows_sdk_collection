// ===========================================================================
// File: U T I L . C P P
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
// 
// Copyright 1996 Microsoft Corporation.  All Rights Reserved.
// ===========================================================================

// %%Includes: ---------------------------------------------------------------
#define INC_OLE2
#define STRICT
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"

// ---------------------------------------------------------------------------
// %%Function: UtilAllocMem
// ---------------------------------------------------------------------------
BOOL UtilAllocMem (PVOID *ppv, DWORD cb)
{
	*ppv = malloc (cb);
	return (NULL != *ppv);
}

// ---------------------------------------------------------------------------
// %%Function: UtilFreeMem
// ---------------------------------------------------------------------------
void UtilFreeMem (PVOID pv)
{
	if (NULL != pv)
		free (pv);
}

// ---------------------------------------------------------------------------
// %%Function: GetUserSidFromToken
//  Returns the SID of the user identified by the supplied token.
// ---------------------------------------------------------------------------
BOOL GetUserSidFromToken (HANDLE hToken, PSID *ppSid)
{
	TOKEN_USER *pUser = NULL;
	PSID psidUser = NULL;
	DWORD cbSid;
	DWORD cbRequired;

	if (GetTokenInformation (hToken, TokenUser, NULL, 0, &cbRequired))
		return(FALSE);	// we shouldn't succeed without a buffer...

	if (ERROR_INSUFFICIENT_BUFFER != GetLastError ())
		return(FALSE);

	if (!UtilAllocMem ((PVOID*)&pUser, cbRequired))
		return(FALSE);

	__try  {
		if (!GetTokenInformation (hToken, TokenUser, pUser, cbRequired,
				&cbRequired))
			return(FALSE);

		// Need a buffer with just the SID, so allocate and copy
		cbSid = GetLengthSid (pUser->User.Sid);
		if (!UtilAllocMem ((PVOID*)&psidUser, cbSid))
			return(FALSE);

		if (!CopySid (cbSid, psidUser, pUser->User.Sid))
			return(FALSE);

		*ppSid = psidUser;
		psidUser = NULL;	// so we don't free the sid

		return(TRUE);
	} __finally  {
		UtilFreeMem (psidUser);
		UtilFreeMem (pUser);
	}
}

// ---------------------------------------------------------------------------
// %%Function: GetGroupSidFromToken
//  Returns the SID for the primary group in the supplied token
// ---------------------------------------------------------------------------
BOOL GetGroupSidFromToken (HANDLE hToken, PSID *ppSid)
{
	TOKEN_PRIMARY_GROUP *pGroup = NULL;
	PSID psidGroup = NULL;
	DWORD cbSid;
	DWORD cbRequired;

	if (GetTokenInformation (hToken, TokenPrimaryGroup, NULL, 0, &cbRequired))
		return(FALSE);	// something's really screwy

	if (ERROR_INSUFFICIENT_BUFFER != GetLastError ())
		return(FALSE);

	if (!UtilAllocMem ((PVOID*)&pGroup, cbRequired))
		return(FALSE);

	__try  {
		if (!GetTokenInformation (hToken, TokenPrimaryGroup, pGroup,
				cbRequired, &cbRequired))
			return(FALSE);

		// Need a buffer with just the SID, so allocate and copy
		cbSid = GetLengthSid (pGroup->PrimaryGroup);
		if (!UtilAllocMem ((PVOID*)&psidGroup, cbSid))
			return(FALSE);

		if (!CopySid (cbSid, psidGroup, pGroup->PrimaryGroup))
			return(FALSE);

		*ppSid = psidGroup;
		psidGroup = NULL;	// so we don't free the sid

		return(TRUE);
	} __finally  {
		UtilFreeMem (psidGroup);
		UtilFreeMem (pGroup);
	}
}

// ---------------------------------------------------------------------------
// %%Function: CreateAppSecurityDescriptor
//  Creates a security descriptor that allows the world access and is
// owned by the current user.  AccessCheck is used to verify the SD
// because an SD can be valid without having everything required by
// CoInitializeSecurity.
// ---------------------------------------------------------------------------
BOOL CreateAppSecurityDescriptor(PSECURITY_DESCRIPTOR *ppsd)
{
	PSECURITY_DESCRIPTOR psd = NULL;
	PSID psidUser = NULL, psidGroup = NULL;
	HANDLE hToken;
	GENERIC_MAPPING gm =  {1, 2, 4, 8};	// dummy GM for AccessCheck
	DWORD dwDesired = 1;	// MUST be a value from GM
	BOOL fAccess = FALSE;
	DWORD dwGranted;
	PRIVILEGE_SET ps;
	DWORD cbPriv = sizeof (ps);

	// allocate the security descriptor
	if (!UtilAllocMem ((PVOID*)&psd, SECURITY_DESCRIPTOR_MIN_LENGTH))
		return(FALSE);

	// we have to impersonate ourselves for the AccessCheck later
	if (!ImpersonateSelf(SecurityImpersonation))
		return(FALSE);

	__try  {

		if (!InitializeSecurityDescriptor(psd, SECURITY_DESCRIPTOR_REVISION))
			return(FALSE);

		// give it a NULL DACL -- allow all access
		if (!SetSecurityDescriptorDacl (psd, TRUE, NULL, FALSE))
			return(FALSE);

		// open the token to get the user and group for the new SD
		if (!OpenThreadToken (GetCurrentThread(), TOKEN_QUERY, TRUE, &hToken))
			return(FALSE);

		// set the SD owner		
		if (!GetUserSidFromToken(hToken, &psidUser))
			return(FALSE);
		if (!SetSecurityDescriptorOwner(psd, psidUser, FALSE))
			return(FALSE);

		// set the SD group
		if (!GetGroupSidFromToken(hToken, &psidGroup))
			return(FALSE);
		if (!SetSecurityDescriptorGroup(psd, psidGroup, FALSE))
			return(FALSE);

		// use AccessCheck to verify that we have a good SD
		// using dummy values since we don't really have anything to check
		if (!AccessCheck(psd, hToken, dwDesired, &gm, &ps, &cbPriv,
				&dwGranted, &fAccess))
			return(FALSE);

		*ppsd = psd;
		psd = NULL;			//
		psidUser = NULL;	// don't free these...
		psidGroup = NULL;	//

		return(TRUE);

	} __finally  {
		UtilFreeMem (psd);
		UtilFreeMem (psidUser);
		UtilFreeMem (psidGroup);
		RevertToSelf ();
	}
}

// ---------------------------------------------------------------------------
// %%Function: FreeAppSecurityDescriptor
//  Releases resources used by the security descriptor.
// ---------------------------------------------------------------------------
BOOL FreeAppSecurityDescriptor (PSECURITY_DESCRIPTOR psd)
{
	PSID psid;
	BOOL fDefaulted;

	if (!GetSecurityDescriptorOwner(psd, &psid, &fDefaulted))
		return(FALSE);
	UtilFreeMem (psid);

	if (!GetSecurityDescriptorGroup(psd, &psid, &fDefaulted))
		return(FALSE);
	UtilFreeMem (psid);

	UtilFreeMem (psd);

	return(TRUE);
}

// ---------------------------------------------------------------------------
// %%Function: ErrorMessage
// ---------------------------------------------------------------------------
void ErrorMessage(HWND hwnd, LPCTSTR szFunction, HRESULT hr)
{
	LPTSTR   szMessage;

	if (HRESULT_FACILITY(hr) == FACILITY_WINDOWS)
		hr = HRESULT_CODE(hr);

	if (!FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, hr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //The user default
			(LPTSTR)&szMessage, 0, NULL))
		return;

	if (hwnd == NULL)
		{
		OutputDebugString(szFunction);
		OutputDebugString(TEXT(": "));
		OutputDebugString(szMessage);
		OutputDebugString(TEXT("\n"));
		}
	else
		MessageBox(hwnd, szMessage, szFunction, MB_OK);

	LocalFree(szMessage);
}  // ErrorMessage

// EOF =======================================================================
