/*++

Copyright (c) 1996 Microsoft Corporation

Module Name:

    httpauth.h

Abstract:

    Functions for authentication sequence ( Basic & NTLM )

Revision History:

--*/

BOOL AddAuthorizationHeader(PSTR pch, PSTR pchSchemes, PSTR pchAuthData, PSTR pchUserName, PSTR pchPassword, BOOL *pfNeedMoreData );
BOOL InitAuthorizationHeader();
void TerminateAuthorizationHeader();
BOOL IsInAuthorizationSequence();
BOOL ValidateAuthenticationMethods( PSTR pszMet, PSTR pszPref );
