#define WIN32_LEAN_AND_MEAN         // the bare essential Win32 API
#include <windows.h>
#include <ctype.h>                  // for isprint()
#include <httpext.h>

#include "keys.h"
#include "html.h"

// prototypes
void SendVariables (EXTENSION_CONTROL_BLOCK *pECB);
void HexDumper (EXTENSION_CONTROL_BLOCK *pECB, LPBYTE lpbyBuf, DWORD dwLength);
void WhoAmI (EXTENSION_CONTROL_BLOCK *pECB);

//
// DllMain allows us to initialize our state variables.
// You might keep state information, as the DLL often remains
// loaded for several client requests.  The server may choose 
// to unload this DLL, and you should save your state to disk,
// and reload it here.  DllMain is called for both loading
// and unloading.  See the Win32 SDK for more info on how
// DLLs load and unload.
//
// An important part of building any DLL is deciding how the
// entry point is going to function.  You may not want an
// entry point at all.  Or, you might call your entry point
// DllMain and add a link option 
//
// -entry:DllMainCRTStartup$(DLLENTRY)
//
// The makefile symbol DLLENTRY is defined in ntwin32.mak, 
// which comes with Win32 SDK and Visual C++.
//
// You might choose to use Visual C++ 2.x or 4.x--go into the
// project settings, link tab, output and specify a the entry
// symbol DllMainCRTStartup$(DLLENTRY).
//
// By the way, $(DLLENTRY) currently resolves to @12, which is
// the ordinal of _CRT_INIT, the C Runtime initializer.
//

BOOL WINAPI DllMain (HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpv)
    {
    // Nothing to do here
    return (TRUE);
    }


//
//  BOOL WINAPI GetExtensionVersion (HSE_VERSION_INFO *pVersionInfo) 
//
//  Return the version this server is built for.  See <httpext.h> for
//  a prototype.  This function is required by the spec.
//
 
BOOL WINAPI GetExtensionVersion (HSE_VERSION_INFO *pVersionInfo)
    {
    // set version to httpext.h version constants
    pVersionInfo->dwExtensionVersion = MAKELONG (HSE_VERSION_MINOR, HSE_VERSION_MAJOR);

    lstrcpyn ((LPTSTR) pVersionInfo->lpszExtensionDesc,
            TEXT("FORMDUMP - A Form Decoder and Dumper"),
            HSE_MAX_EXT_DLL_NAME_LEN);

    return TRUE;
    } // GetExtensionVersion()


//
//  Our entry point:
//
//  BOOL WINAPI HttpExtensionProc (EXTENSION_CONTROL_BLOCK *pECB)
//
//  This function pulls in all inbound data.  A reply page is built
//  so the user can see how forms appear in our key list.
//  This function is also required by the spec.
//

DWORD WINAPI HttpExtensionProc (EXTENSION_CONTROL_BLOCK *pECB)
    {
    HKEYLIST hKeyList;
    TCHAR szMsg[128];
    DWORD dwWritten;

    // Get the keys sent by the client
    hKeyList = GetKeyList (pECB);

    // Send content type
    TCHAR str[] = TEXT("Content-type: text/html\r\n\r\n");
    dwWritten = sizeof (str);
    pECB->ServerSupportFunction (pECB->ConnID,
                                 HSE_REQ_SEND_RESPONSE_HEADER,
                                 NULL,
                                 &dwWritten,
                                 (LPDWORD) str);


    // Create a basic HTML page
    HtmlCreatePage (pECB, TEXT("FormDump.dll Reply"));
    HtmlHeading (pECB, 1, TEXT("Data Available via ISAPI"));
	HtmlHorizontalRule (pECB);

	//
	// Send each form field
	//

    HtmlHeading (pECB, 2, TEXT("Form Fields"));

    if (!hKeyList)
        {
		// Report no data/error
        HtmlBold (pECB, TEXT("No form fields sent"));
		HtmlWriteText (pECB, TEXT(" (or error decoding keys)"));
		HtmlEndParagraph (pECB);
        }
    else
        {
        HKEYLIST hKey;

        // Print a quick overview
        HtmlWriteTextLine (pECB, TEXT("The form you submitted data to just called"));
        HtmlWriteTextLine (pECB, TEXT("the Internet Information Server extension"));
        HtmlWriteTextLine (pECB, TEXT("FormDump.dll.  Here is a listing of what was"));
        HtmlWriteTextLine (pECB, TEXT("received and what variables inside FormDump"));
        HtmlWriteTextLine (pECB, TEXT("have the data."));
        HtmlEndParagraph (pECB);


        // Loop through all of the keys
        hKey = hKeyList;
        while (hKey)
            {
            // Details about the key
            LPCTSTR lpszKeyName;
            DWORD dwLength;
            BOOL bHasCtrlChars;
            int nInstance;
            HKEYLIST hLastKey;

            // We get info, and hKey points to next key in list
            hLastKey = hKey;    //keep this for later
            hKey = GetKeyInfo (hKey, &lpszKeyName, &dwLength,
                                &bHasCtrlChars, &nInstance);

            
            // Build web page
            HtmlBold (pECB, TEXT("Form Field Name (lpszKeyName): "));
            HtmlWriteText (pECB, lpszKeyName);
            HtmlLineBreak (pECB);

            HtmlBold (pECB, TEXT("Length of Data (dwLength): "));
            wsprintf (szMsg, TEXT("%u"), dwLength);
            HtmlWriteText (pECB, szMsg);
            HtmlLineBreak (pECB);

            HtmlBold (pECB, TEXT("Data Has Control Characters (bHasCtrlChars): "));
            wsprintf (szMsg, TEXT("%u"), bHasCtrlChars);
            HtmlWriteText (pECB, szMsg);
            HtmlLineBreak (pECB);

            HtmlBold (pECB, TEXT("Instance of Form Field (nInstance): "));
            wsprintf (szMsg, TEXT("%u"), nInstance);
            HtmlWriteText (pECB, szMsg);

            if (dwLength)
                {
                HtmlLineBreak (pECB);
                HtmlBold (pECB, TEXT("Data Sent for Field:"));
                HtmlLineBreak (pECB);
                
                HexDumper (pECB, GetKeyBuffer (hLastKey), dwLength);
                }

            HtmlEndParagraph (pECB);
            }
        
        // Clean up
        FreeKeyList (hKeyList);
        }

	HtmlHorizontalRule (pECB);


	//
	// Get user name from SID and return it in the page
	//

	HtmlHeading (pECB, 2, TEXT("Security Context for HttpExtensionProc Thread"));
	WhoAmI (pECB);
	HtmlHorizontalRule (pECB);


	//
	// Display all server variables
	//

	HtmlHeading (pECB, 2, TEXT("Server Variables"));
    HtmlWriteTextLine (pECB, TEXT("Below is a list of all variables available via"));
	HtmlWriteTextLine (pECB, TEXT("GetServerVariable ISAPI API.  Much of this"));
	HtmlWriteTextLine (pECB, TEXT("information comes from the browser HTTP header."));
    HtmlEndParagraph (pECB);

	// Send server variables obtained from the HTTP header
	SendVariables (pECB);


	// Finish up...
    HtmlEndPage (pECB);

    return HSE_STATUS_SUCCESS;
    }


//
// Put the inbound data in a hex dump format
//

void HexDumper (EXTENSION_CONTROL_BLOCK *pECB, LPBYTE lpbyBuf, DWORD dwLength)
    {
    DWORD dwSize;
    TCHAR szLine[80];
    TCHAR szHex[3];
    DWORD i;
    DWORD dwPos = 0;

    HtmlBeginPreformattedText (pECB);

    while (dwLength)
        {
        // Take min of 16 or dwLength
        dwSize = min (16, dwLength);

        // Build text line
        wsprintf (szLine, TEXT("  %04X "), dwPos);

        for (i = 0 ; i < dwSize ; i++)
            {
            wsprintf (szHex, TEXT("%02X"), lpbyBuf[i]);
            lstrcat (szLine, szHex);
            lstrcat (szLine, TEXT(" "));
            }

        // Add spaces for short lines
        while (i < 16)
            {
            lstrcat (szLine, TEXT("   "));
            i++;
            }

        // Add ASCII chars
        for (i = 0 ; i < dwSize ; i++)
            {
            if (isprint (lpbyBuf[i]))
                {
                wsprintf (szHex, TEXT("%c"), lpbyBuf[i]);
                lstrcat (szLine, szHex);
                }
            else
                {
                lstrcat (szLine, TEXT("."));
                }
            }

        // Write data to web page
        HtmlWriteTextLine (pECB, szLine);

        // Advance positions
        dwLength -= dwSize;
        dwPos += dwSize;
        lpbyBuf += dwSize;
        }

    HtmlEndPreformattedText (pECB);
    }


//
// Dump a server variable
//

void DumpVariable (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR szName)
	{
    DWORD dwBufferSize;
    TCHAR szBuffer[4096];
	BOOL bReturn;

    dwBufferSize = sizeof (szBuffer);
    bReturn = pECB->GetServerVariable (pECB->ConnID,
									   (LPTSTR) szName,
									   szBuffer,
									   &dwBufferSize);

	if (!bReturn || !szBuffer[0])
		return;

	HtmlWriteText (pECB, szName);
	HtmlWriteText (pECB, TEXT ("="));
	HtmlWriteText (pECB, szBuffer);
	HtmlLineBreak (pECB);
	}


//
// Send all server variables (they came in the HTTP header)
//

void SendVariables (EXTENSION_CONTROL_BLOCK *pECB)
	{
    TCHAR *pChar, *pOpts, *pEnd;
    DWORD dwBufferSize;
    TCHAR szBuffer[4096];
	BOOL bReturn;

	//
	// Dump the standard variables
	//

	DumpVariable (pECB, "AUTH_TYPE");
	DumpVariable (pECB, "CONTENT_LENGTH");
	DumpVariable (pECB, "CONTENT_TYPE");
	DumpVariable (pECB, "GATEWAY_INTERFACE");
	DumpVariable (pECB, "PATH_INFO");
	DumpVariable (pECB, "PATH_TRANSLATED");
	DumpVariable (pECB, "QUERY_STRING");
	DumpVariable (pECB, "REMOTE_ADDR");
	DumpVariable (pECB, "REMOTE_HOST");
	DumpVariable (pECB, "REMOTE_USER");
	DumpVariable (pECB, "REQUEST_METHOD");
	DumpVariable (pECB, "SCRIPT_NAME");
	DumpVariable (pECB, "SERVER_NAME");
	DumpVariable (pECB, "SERVER_PORT");
	DumpVariable (pECB, "SERVER_PROTOCOL");
	DumpVariable (pECB, "SERVER_SOFTWARE");
	DumpVariable (pECB, "AUTH_PASS");


	//
	// Dump any others (in ALL_HTTP)
	//

    dwBufferSize = sizeof (szBuffer);
    bReturn = pECB->GetServerVariable (pECB->ConnID,
									  TEXT("ALL_HTTP"),
									  szBuffer,
									  &dwBufferSize);

    if (bReturn)
        {
		//
		// Find lines, split key/data pair and write them as output
		//

		pChar = szBuffer;
		while (*pChar)
			{
			if (*pChar == TEXT('\r') || *pChar == TEXT ('\n'))
				{
				pChar++;
				continue;
				}

			pOpts = strchr (pChar, TEXT(':'));  // look for separator
			if (!pOpts)
				break;
			if (!*pOpts)
				break;

			pEnd = pOpts;
			while (*pEnd && *pEnd != TEXT('\r') && *pEnd != TEXT('\n'))
				pEnd++;

			*pOpts = 0;     // split strings
			*pEnd = 0;

			//
			// pChar points to variable name, pOpts + 1 points to variable val
			//

			HtmlWriteText (pECB, pChar);
			HtmlWriteText (pECB, TEXT ("="));
			HtmlWriteText (pECB, pOpts + 1);
			HtmlLineBreak (pECB);
        
			pChar = pEnd + 1;
			}
		}

	HtmlEndParagraph (pECB);
	HtmlHorizontalRule (pECB);
	}


//
// Get the user SID, lookup the account name and display it
//

void WhoAmI (EXTENSION_CONTROL_BLOCK *pECB)
	{
	HANDLE hToken;
	PTOKEN_USER pTokenUser;
	BYTE byBuf[1024];
	DWORD dwLen;
	TCHAR szName[256], szDomain[256];
	DWORD dwNameLen, dwDomainLen;
	SID_NAME_USE eUse;

	if (!OpenThreadToken (GetCurrentThread (), TOKEN_QUERY, TRUE, &hToken))
		{
		DWORD dwError = GetLastError ();
		HtmlBold (pECB, "OpenThreadToken failed. ");
		HtmlPrintf (pECB, "Error code=%u", dwError);
		HtmlEndParagraph (pECB);
		return;
		}

	pTokenUser = (PTOKEN_USER) byBuf;
	if (!GetTokenInformation (hToken, TokenUser, pTokenUser, sizeof (byBuf), &dwLen))
		{
		DWORD dwError = GetLastError ();
		CloseHandle (hToken);

		HtmlBold (pECB, "GetTokenInformation failed. ");
		HtmlPrintf (pECB, "Error code=%u dwLen=%u", dwError, dwLen);
		HtmlEndParagraph (pECB);
		return;
		}

	dwNameLen = sizeof (szName);
	dwDomainLen = sizeof (szDomain);
	if (!LookupAccountSid (NULL, pTokenUser->User.Sid, 
						   szName, &dwNameLen,
						   szDomain, &dwDomainLen, &eUse))
		{
		DWORD dwError = GetLastError ();
		CloseHandle (hToken);

		HtmlBold (pECB, "LookupAccountSid failed. ");
		HtmlPrintf (pECB, "Error code=%u dwNameLen=%u dwDomainLen=%u", 
					dwError, dwNameLen, dwDomainLen);
		HtmlEndParagraph (pECB);
		return;
		}

	HtmlBold (pECB, "Domain: ");
	HtmlWriteText (pECB, szDomain);
	HtmlLineBreak (pECB);
	HtmlBold (pECB, "User: ");
	HtmlWriteText (pECB, szName);
	HtmlEndParagraph (pECB);

	CloseHandle (hToken);
	}
