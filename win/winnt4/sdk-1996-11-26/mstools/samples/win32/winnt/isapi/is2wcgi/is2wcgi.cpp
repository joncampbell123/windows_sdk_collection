//
// IS2WCGI.CPP
//
// This sample Web Server Application allows a Windows CGI extension
// to run as an ISAPI extension.  The sample translates the inbound
// ISAPI environment into the private profile that the Windows CGI
// app is expecting.  This sample implements Windows CGI version 1.2.
//
// The Windows CGI spec (authored by Robert B. Denny 
// <rdenny@netcom.com>) can be found at:
//
//   http://website.ora.com/wsdocs/32demo/windows-cgi.html
//
// After building this DLL, copy it to the same directory your
// Windows CGI application is in.  Rename the DLL to the same
// name your application has, but keep the DLL extension.
//
// See LibMain for details on why you must rename the DLL.
//

//
// NOTE: Only the 1.2 spec is implemented here.  The capabilities
//       of spec version 1.3 really only include multi-part
//		 MIME types.
//

//
// Uncomment this #define only if the initial part of your WinCGI app's
// output data is missing on the browser.  The WinCGI spec says the
// Web server is responsible for sending all headers, and the WinCGI
// app is only responsible for Web page content, typically HTML.  However,
// most WinCGI apps send header information anyhow, so IS2WCGI pulls
// this header info out of the output file and sends it to 
// ServerSupportFunction.
//

//#define CONFORMANT_HEADER 

//
// Comment this #define when your application expects the Windows CGI
// 1.2 command line instead of the 1.1 command line.
//

#define USE_11_CMD_LINE


#define WIN32_LEAN_AND_MEAN  
#include <windows.h>
#include <httpext.h>

#include "keys.h"

#define WAIT_EXT_TIMEOUT 120000     // 120 secs
extern TCHAR gszAppName[MAX_PATH];

//
// This struct is used to pass profile information,
// file names for temporary files, and an extension
// control block pointer.
//
// One instance of this structure is delcared in
// HttpExtensionProc, and its address is passed to 
// all the helper functions.
//

typedef struct
    {
    // Profile section names
    LPCSTR szCGI;
    LPCSTR szAccept;
    LPCSTR szSystem;
    LPCSTR szExtraHeaders;
    LPCSTR szFormLiteral;
    LPCSTR szFormExternal;
    LPCSTR szFormHuge;
    LPCSTR szFormFile;

    // File and path names
    TCHAR szProfileName[MAX_PATH];
    TCHAR szOutputFileName[MAX_PATH];
    TCHAR szTempDir[MAX_PATH];
    LPCTSTR lpszContentFile;

    // Key list and content file
    HKEYLIST hKeyList;

    // Info given by the www service
    EXTENSION_CONTROL_BLOCK *pECB;
    } WCGIPARAMS, *PWCGIPARAMS;


// Prototypes
void LogError (LPCTSTR lpszError, EXTENSION_CONTROL_BLOCK *pECB);
void FillCGI (PWCGIPARAMS pParam);
void FillAccept (PWCGIPARAMS pParam);
void FillSystem (PWCGIPARAMS pParam);
void FillExtraHeaders (PWCGIPARAMS pParam);
BOOL FillFormData (PWCGIPARAMS pParam);
HANDLE ExecuteChildProc (PWCGIPARAMS pParam);
BOOL GetDataFromFile (PWCGIPARAMS pParam);

//
// Error messages (for the log file)
//

static TCHAR gszInDiskError[]       = TEXT("Error writing inbound client data to disk.");
static TCHAR gszOutDiskError[]      = TEXT("Error reading outbound client data from disk.");
static TCHAR gszBadProc[]           = TEXT("Could not start Windows CGI executable.");
static TCHAR gszNoProcEnd[]         = TEXT("Windows CGI application never terminated.");
static TCHAR gszParseDiskError[]    = TEXT("Error reading data file during form decoding.");
static TCHAR gszContentDiskError[]  = TEXT("Could not save inbound data to content file.");
static TCHAR gszOutOfMemory[]       = TEXT("Memory allocation request failed.");
static TCHAR gszTempFileError[]     = TEXT("Could not create a temporary file.");
static TCHAR gszWriteError[]        = TEXT("Error writing to a temporary file.");
static TCHAR gszNoDataDecoded[]     = TEXT("No data sent by form.");

//
// Debug mode - make "Yes" or "No"
//

static TCHAR gszDebugMode[] = TEXT("No");

//
// Macro is used in Fill routines below.
//

#define MACRO_WriteKey(key,val) WritePrivateProfileString (szSection, key, val, szProfile)
#define MACRO_WriteKeyInt(key,val) wsprintf(szVal,TEXT("%i"),val);WritePrivateProfileString(szSection, key, szVal, szProfile)

//
//  BOOL WINAPI GetExtensionVersion (HSE_VERSION_INFO *pVersionInfo)
//
//  Return the version this server is built for.  See httpext.h for
//  a prototype.  This function is required by the spec.
//
 
BOOL WINAPI GetExtensionVersion (HSE_VERSION_INFO *pVersionInfo)
    {
    // set version to httpext.h version constants
    pVersionInfo->dwExtensionVersion = MAKELONG (HSE_VERSION_MINOR, HSE_VERSION_MAJOR);

    lstrcpyn (pVersionInfo->lpszExtensionDesc,
            TEXT("IIS to WCGI Converter"),
            HSE_MAX_EXT_DLL_NAME_LEN);

    return TRUE;
    } // GetExtensionVersion()


//
//  BOOL WINAPI HttpExtensionProc (EXTENSION_CONTROL_BLOCK *pECB)
//
//  This function does all of the work.  Once called it retrieves all of
//  the environment data from the server via the GetServerVariable
//  server function, reads all of the client data if it's not already
//  available, packs it all up in accordance with the Windows CGI standard,
//  calls the Windows CGI app, and then passes the returned data to the
//  web client.
//

DWORD WINAPI HttpExtensionProc (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WCGIPARAMS param;
    HANDLE hProcess;
    DWORD dwResult;
    BOOL bError;

    memset (&param, 0, sizeof (WCGIPARAMS));

    param.szCGI = "CGI";
    param.szAccept = "Accept";
    param.szSystem = "System";
    param.szExtraHeaders = "Extra Headers";
    param.szFormLiteral = "Form Literal";
    param.szFormExternal = "Form External";
    param.szFormHuge = "Form Huge";

    param.pECB = pECB;


    //
    // Windows CGI 1.2 apps are supposed to be called with a command line of:
    //     <WinCGIAppName> <WinCGIProfilePath>
	//
	// Many WinCGI apps rely on additional parameters of older versions
	// supported by O'Reily's Website server.  So IS2WCGI supports them
	// by running WinCGI apps with the following command line:
	//
	// <AppName> <ProfilePath> <InputFileName> <OutputFileName> <UrlArgs>
	//
	//
    // For a WinCGI app, most of the information comes in the profile ('ini' file).
	// The WinCGI app does its thing and then writes output to <OutputFileName>.
    //
    // This extension has the following tasks:
    //
    //  - Create a temporary file to hold the profile
    //  - Fill all the profile keys
    //  - Collect inbound client data and save it to disk
    //  - Start the child process
    //  - Wait for child to finish
    //  - Write output data back to the web client
    //  - Clean up
    //

    //
    // First, we have to create a temporary file.  We use Win32 APIs.
    //

    // Get path of this module
	int i;
	for (i = lstrlen (gszAppName) - 1 ; i >= 0 ; i--)
		if (gszAppName[i] == '\\')
			break;

	lstrcpy (param.szTempDir, gszAppName);
	if (i >= 0)
		param.szTempDir[i] = 0;

    // Create profile and output file (always req'd by WCGI app)
    if (!GetTempFileName (param.szTempDir, 
                          param.szCGI, 0, 
                          param.szProfileName))
        {
        // Unexpected error creating temp file.  Log an error.
        LogError (gszTempFileError, pECB);

        return HSE_STATUS_ERROR;
        }

    if (!GetTempFileName (param.szTempDir, 
                          param.szCGI, 0, 
                          param.szOutputFileName))
        {
        // Unexpected error creating temp file.  Log an error.
        LogError (gszTempFileError, pECB);
        DeleteFile (param.szProfileName);

        return HSE_STATUS_ERROR;
        }

    //
    // Generate all sections
    //

    FillCGI (&param);
    FillAccept (&param);
    FillSystem (&param);
    FillExtraHeaders (&param);

    bError = !FillFormData (&param);

    if (!bError)
        {
        //
        // Execute child process
        //

        hProcess = ExecuteChildProc (&param);

        if (hProcess == INVALID_HANDLE_VALUE)
            {
            // Process may not exist.  Log an error.
            LogError (gszBadProc, pECB);
            bError = TRUE;
            }

        if (!bError)
            {
            //
            // Allow it to finish - give it WAIT_EXT_TIMEOUT seconds
            //

            dwResult = WaitForSingleObject (hProcess, WAIT_EXT_TIMEOUT);

            if (dwResult == WAIT_FAILED)
                {
                dwResult = GetLastError ();
                if (dwResult == WAIT_TIMEOUT)
                    {
                    // App never finished!  Log an error.
                    LogError (gszNoProcEnd, pECB);
                    bError = TRUE;
                    }

                // else must be WAIT_OBJECT_0, and that's fine (but unusual)
                }

            }


        //
        // Move app output to the web server
        //

        if (!bError)
            {
            if (!GetDataFromFile (&param))
                {
                // Error reading from disk.  Log an error.
                LogError (gszOutDiskError, pECB);
                bError = TRUE;
                }
            }
        }


    //
    // Clean up
    //

    // Delete the temp files made in form decoding
    DWORD dwBufSize;
    LPTSTR lpszDelBuf;
    LPTSTR lpszThisStr;
    LPTSTR lpszNextStr;
    LPTSTR lpszEndOfStr;
    dwBufSize = 0x10000;        // (assume this is big enough for now)
    lpszDelBuf = (LPTSTR) HeapAlloc (GetProcessHeap (), 0, dwBufSize);
    if (lpszDelBuf)
        {
		lpszDelBuf[0] = 0;
        dwBufSize = GetPrivateProfileSection (param.szFormExternal, 
                                              lpszDelBuf, 
                                              dwBufSize, 
                                              param.szProfileName);

        lpszNextStr = lpszDelBuf;
        while (*lpszNextStr)
            {
			//
			// lpszNextStr currently points to a string in the form of:
			// key=string
			//
			// Set lpszNextStr to point to data, then set lpszThisStr to the same
			//
            lpszNextStr = strchr (lpszNextStr, TEXT('='));
			if (!lpszNextStr)
				break;

            lpszNextStr++;
            lpszThisStr = lpszNextStr;

			// Find space delimeter, search backwards
            lpszEndOfStr = lpszNextStr + lstrlen (lpszNextStr);
			while (lpszEndOfStr != lpszNextStr && *lpszEndOfStr != TEXT(' '))
				lpszEndOfStr--;
			
			// Advance string pointer to next key=string
            lpszNextStr += lstrlen (lpszNextStr) + 1;

			// Terminate file name with a zero
			*lpszEndOfStr = 0;
            
            DeleteFile (lpszThisStr);
            }

        HeapFree (GetProcessHeap (), 0, lpszDelBuf);
        }

    // Clean up key list resources & delete content file
	if (param.hKeyList)
		FreeKeyList (param.hKeyList);
	else
		{
		// Remove zero-length content file
		if (param.lpszContentFile)
			{
			DeleteFile (param.lpszContentFile);
			HeapFree (GetProcessHeap (), 0, (LPVOID) param.lpszContentFile);
			}
		}

    // Delete the temp files we made
    DeleteFile (param.szProfileName);
    DeleteFile (param.szOutputFileName);

	return bError ? HSE_STATUS_ERROR : HSE_STATUS_SUCCESS; 
    }


//
// GetVarAndWriteKey obtains an environment variable and saves it
// to the specified key in the profile.  This function cleans
// up the Fill code a lot.
//

void GetVarAndWriteKey (PWCGIPARAMS pParam, LPCTSTR lpszSection,
                        LPCTSTR lpszVar, LPCTSTR lpszKey)
    {
    TCHAR szBuffer[MAX_PATH];
    DWORD dwBufferSize;
    BOOL bReturn;

    // Call server to get environment variable
    dwBufferSize = MAX_PATH;
    bReturn = pParam->pECB->GetServerVariable (pParam->pECB->ConnID,
                                               (LPTSTR) lpszVar,
                                               szBuffer,
                                               &dwBufferSize);

    if (!bReturn)
        {
        // expected symbol is missing
        return;
        }

    // Write variable to profile if data exists
    if (szBuffer[0])
        {
        WritePrivateProfileString (lpszSection,
                                   lpszKey, 
                                   szBuffer, 
                                   pParam->szProfileName);
        }
    }


//
// Fill routines are used to move data from the server's environment
// into the profile string.
//
// FillCGI handles the [CGI] section of the profile.
//

void FillCGI (PWCGIPARAMS pParam)
    {
    // Everything comes in pParam.  We'll use
    // shorter variable names make things readable.

    LPCTSTR szProfile;
    LPCTSTR szSection;
    EXTENSION_CONTROL_BLOCK *ecb;

    szProfile = pParam->szProfileName;
    szSection = pParam->szCGI;
    ecb = pParam->pECB;


    //
    // Write information not kept as a server varaible.
    //

    MACRO_WriteKey (TEXT("Request Method"), ecb->lpszMethod);
    MACRO_WriteKey (TEXT("Query String"),   ecb->lpszQueryString);
    MACRO_WriteKey (TEXT("Logical Path"),   ecb->lpszPathInfo);
    MACRO_WriteKey (TEXT("Physical Path"),  ecb->lpszPathTranslated);
    MACRO_WriteKey (TEXT("CGI Version"),    TEXT("CGI/1.2 (Win)"));


    //
    // Get server variables and write the values to the profile
    //

    GetVarAndWriteKey (pParam, szSection,
                       TEXT("SERVER_PROTOCOL"), TEXT("Request Protocol"));

    GetVarAndWriteKey (pParam, szSection,
                       TEXT("SCRIPT_NAME"), TEXT("Executable Path"));

    GetVarAndWriteKey (pParam, szSection,
                       TEXT("SERVER_SOFTWARE"), TEXT("Server Software"));

    GetVarAndWriteKey (pParam, szSection,
                       TEXT("SERVER_NAME"), TEXT("Server Name"));

    GetVarAndWriteKey (pParam, szSection,
                       TEXT("SERVER_PORT"), TEXT("Server Port"));

    GetVarAndWriteKey (pParam, szSection,
                       TEXT("REMOTE_HOST"), TEXT("Remote Host"));

    GetVarAndWriteKey (pParam, szSection,
                       TEXT("REMOTE_ADDR"), TEXT("Remote Address"));

    GetVarAndWriteKey (pParam, szSection,
                       TEXT("AUTHTEXTYPE"), TEXT("Authentication Method"));

    GetVarAndWriteKey (pParam, szSection,
                       TEXT("REMOTE_USER"), TEXT("Authenticated Username"));

	GetVarAndWriteKey (pParam, szSection,
                       TEXT("HTTP_REFERER"), TEXT("Referer"));



    // Keys not supported:
    //
    // From
    // Server Admin
    // Authentication Realm (goes with Authenticated Username)
    }

void FillAccept (PWCGIPARAMS pParam)
    {
    //
    // Accept section provides info about the client's capabilities.  We use
    // the header information stored in the HTTP_ACCEPT envirnoment variable.
    //
    // The format of this variable is:
    //
    // type/subtype [;opt. parameters] [, type/subtype [;params]] [, ...]
    //
    // For example:
    // */*; q=0.300, audio/x-aiff, audio/basic, image/jpeg, image/gif, text/plain, text/html
    //
    // Windows CGI 1.2 breaks this into the [Accept] section of the profile.
    // The above example becomes:
    //
    // [Accept]
    // */*=q=0.300
    // audio/x-aiff=Yes
    // audio/basic=Yes
    // image/jpeg=Yes
    // image/gif=Yes
    // text/plain=Yes
    // text/html=Yes
    //

    DWORD dwBufferSize;
    BOOL bReturn;
    TCHAR *pChar, *pOpts;
    TCHAR szBuffer[MAX_PATH];

    // We'll use shorter variable names make things readable.
    LPCTSTR szProfile;
    LPCTSTR szSection;
    EXTENSION_CONTROL_BLOCK *ecb;

    szProfile = pParam->szProfileName;
    szSection = pParam->szAccept;
    ecb = pParam->pECB;

    //
    // Get the inbound accept line
    //

    dwBufferSize = MAX_PATH;
    bReturn = ecb->GetServerVariable (ecb->ConnID,
                                      TEXT("HTTP_ACCEPT"),
                                      szBuffer,
                                      &dwBufferSize);

    
    if (!bReturn)
        {
        // expected symbol is missing
        return;
        }

    //
    // Skip leading spaces and grab entire type/subtype[;opts] string
    //

    pChar = strtok (szBuffer, TEXT(" ,"));
    while (pChar)
        {
        pOpts = strchr (pChar, TEXT(';'));  // look for opts, if any

        MACRO_WriteKey (pChar, pOpts == NULL ? TEXT("Yes") : pOpts);

        pChar = strtok (NULL, TEXT(" ,")); // get next type/subtype pair
        }
    }

void FillSystem (PWCGIPARAMS pParam)
    {
    // MACRO_WriteKeyInt buffer
    char szVal[8];

    // Again we'll use shorter variable names make things readable.
    LPCTSTR szProfile;
    LPCTSTR szSection;
    EXTENSION_CONTROL_BLOCK *ecb;

    szProfile = pParam->szProfileName;
    szSection = pParam->szSystem;
    ecb = pParam->pECB;

    //
    // The [System] section must be filled out with GMT Offset, Debug Mode,
    // Output File and Content File.  The Content File key is written in
    // PutDataInFile() below.
    //
    // GMT offset is the number of seconds added to GMT time to reach local
    // time.  For example, PST = GMT - 8 hours; GMT offset would equal 
    // -28,800.  Win32 call GetTimeZoneInformation returns the number of
    // minutes to subtract from GMT (UTC) to get local time.
    //
    // So, GMT Offset = -60*TZI.Bias.
    //
    
    TIME_ZONE_INFORMATION tzi = {0};
    GetTimeZoneInformation (&tzi);
    MACRO_WriteKeyInt (TEXT("GMT Offset"), -60 * tzi.Bias);

    // See top of file for gszDebugMode setting.
    
    MACRO_WriteKey (TEXT("Debug Mode"), gszDebugMode);
    MACRO_WriteKey (TEXT("Output File"), pParam->szOutputFileName);
    }

void FillExtraHeaders (PWCGIPARAMS pParam)
    {
    TCHAR *pChar, *pOpts, *pEnd, *pDash;
    DWORD dwBufferSize;
    TCHAR szBuffer[4096];
    BOOL bReturn;

    // Use shorter variable names make things readable.
    LPCTSTR szProfile;
    LPCTSTR szSection;
    EXTENSION_CONTROL_BLOCK *ecb;

    szProfile = pParam->szProfileName;
    szSection = pParam->szExtraHeaders;
    ecb = pParam->pECB;

    // Any extra HTTP headers go in ALL_HTTP.  We need to parse these out and
    // put them in the [Extra Headers] Section.  The format of the ALL_HTTP
    // variable is:
    //     varname: <varvalue>\r\n{...}\0

    // Retrieve ALL_HTTP

    dwBufferSize = sizeof (szBuffer);
    bReturn = ecb->GetServerVariable (ecb->ConnID,
                                      TEXT("ALL_HTTP"),
                                      szBuffer,
                                      &dwBufferSize);

    if (!bReturn)
        {
        // expected symbol is missing
        return;
        }


    //
    // Find lines, split key/data pair and write them to profile
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
            return;
        if (!*pOpts)
            return;

        pEnd = pOpts;
        while (*pEnd && *pEnd != TEXT('\r') && *pEnd != TEXT('\n'))
            pEnd++;

        *pOpts = 0;     // split strings
        *pEnd = 0;

		//
		// Convert to WebSite syntax.  Since profile keys are case-insensitive,
		// we only have to strip HTTP_ and convert underscores to dashes.
		//

		if (!_strnicmp (pChar, TEXT("HTTP_"), 5))
			{
			// Skip HTTP_
			pChar += 5;

			// Convert underscores to dashes
			for (pDash = pChar ; *pDash ; pDash++)
				{
				if (*pDash == TEXT('_'))
					*pDash = TEXT('-');
				}
			}

        MACRO_WriteKey (pChar, pOpts + 1);
        
        pChar = pEnd + 1;
        }
    }


//
// We have to parse the inbound data in a special way.  The Windows CGI
// spec says that a set of form data keys are to be set up based on
// the inbound data.  Four sections separate the inbound data:
//
//  [Form Literal] holds short, text-based form keys.
//  [Form External] holds information about form keys 255 to 65535 
//                  bytes long, or those that have characters < value 32.
//  [Form Huge] holds information about from keys > 65536 bytes long.
//  [Form File] holds pathnames to files uploaded as form data.
//
// For [Form External], the inbound form key is stored in yet another
// temporary file.  A profile key is added specifying the file name and
// length of the file to the Windows CGI application.
//
// For [Form Huge], the inbound form key is left in the content file,
// and a profile key is added specifying the offset from the start
// of the content file, as well as the length of the form key data.
//

//
// First, a cleanup function for FillFormData.
//

void FFD_Cleanup (HKEYLIST hKeyList, HANDLE hContent, LPBYTE lpbyMem)
    {
    if (hKeyList)
        FreeKeyList (hKeyList);

    if (hContent != INVALID_HANDLE_VALUE)
        CloseHandle (hContent);

    if (lpbyMem)
        HeapFree (GetProcessHeap (), 0, lpbyMem);
    }


BOOL FillFormData (PWCGIPARAMS pParam)
    {
    HANDLE hContent;
    HANDLE hExternalFile;
    HKEYLIST hKeyList, hStepKey;
    LPCTSTR lpszKeyName;
    DWORD dwOffset, dwLength;
    BOOL bHasCtrlChars;
    int nInstance;
    TCHAR szKeyNameBuf[256];
    BYTE byKeyDataBuf[254];
    DWORD dwRead, dwWritten;
    LPBYTE lpbyExternalBuf = NULL;
    BOOL bReturn;
    TCHAR szExternalFileName[MAX_PATH];

    // MACRO_WriteKeyInt buffer
    char szVal[8];

    // Use shorter variable names make things readable.
    EXTENSION_CONTROL_BLOCK *ecb;
    LPCTSTR szSection;
    LPCTSTR szProfile;

    szProfile = pParam->szProfileName;
    ecb = pParam->pECB;

    // Get the data sent as a form
    hKeyList = GetKeyList (ecb);

    // If NULL was returned, an error occured or there is no data
    if (!hKeyList)
		{
		if (ecb->cbTotalBytes == 0)
			{
       		// Make zero-length content file
			pParam->lpszContentFile = (LPCTSTR) HeapAlloc (GetProcessHeap (), 0, MAX_PATH);
			if (!GetTempFileName (pParam->szTempDir, 
								  pParam->szCGI, 0, 
								  (LPTSTR) pParam->lpszContentFile))
				{
				return (int) INVALID_HANDLE_VALUE;
				}

			// Create it
			HANDLE hTemp = CreateFile (pParam->lpszContentFile,
						   GENERIC_READ,
						   0,                       // No sharing mode
						   NULL,                    // Default security attribs
						   CREATE_ALWAYS,
						   FILE_ATTRIBUTE_NORMAL,
						   NULL                     // No template file
						   );
			if (hTemp == INVALID_HANDLE_VALUE)
				return (int) INVALID_HANDLE_VALUE;

			// Then close it
			CloseHandle (hTemp);
			return TRUE;
			}

        return FALSE;
		}

    //
    // Make keys.cpp close the content file, then open our content file here
    //
	CloseContentFile (hKeyList);
    pParam->lpszContentFile = GetContentFile (hKeyList);

    hContent = CreateFile (pParam->lpszContentFile,
                       GENERIC_READ,
                       0,                       // No sharing mode
                       NULL,                    // Default security attribs
                       OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL,   
                       NULL                     // No template file
                       );

    if (hContent == INVALID_HANDLE_VALUE)
        {
        FreeKeyList (hKeyList);
        LogError (gszParseDiskError, ecb);
        return FALSE;
        }


    //
    // Next we step through the keys, determining if they go in
    // [Form Literal], [Form External], [Form Huge], or 
    // [Form File].
    //
	// We use our own handle to the content file because for
	// huge data we just pass the location of the data.
	//

    hStepKey = hKeyList;
    while (hStepKey)
        {
		dwOffset = GetKeyOffset (hStepKey);
        hStepKey = GetKeyInfo (hStepKey, &lpszKeyName, 
                               &dwLength, &bHasCtrlChars, &nInstance);

        //
        // If nInstance > 0, we must generate a new key name.
        //
        if (nInstance > 0)
            {
            wsprintf (szKeyNameBuf, TEXT("%s_%i"), lpszKeyName, nInstance);
            lpszKeyName = szKeyNameBuf;
            }

        //
        // If length < 255 and data is straight text, we put the
        // key in [Form Literal]
        //
        if (dwLength < 255 && !bHasCtrlChars)
            {
            // Move to data in the file
            SetFilePointer (hContent, dwOffset, NULL, FILE_BEGIN);

            // Read it
            bReturn = ReadFile (hContent, byKeyDataBuf, dwLength, &dwRead, NULL);
            if (!bReturn || dwRead != dwLength)
                {
                // Handle abnormal errors
                FFD_Cleanup (hKeyList, hContent, lpbyExternalBuf);
                LogError (gszParseDiskError, ecb);
                return FALSE;
                }

            // Write profile key
            byKeyDataBuf[dwLength] = 0;
            WritePrivateProfileString (pParam->szFormLiteral, 
                                        lpszKeyName, (LPTSTR) byKeyDataBuf,
                                        szProfile);
            }

        //
        // If length < 65536 bytes, we put the data into another
        // temporary file, and we note the new temporary file
        // in [Form External].
        //
        else if (dwLength < 65535)
            {
            // Lazy memory allocation
            if (!lpbyExternalBuf)
                {
                lpbyExternalBuf = (LPBYTE) HeapAlloc (GetProcessHeap (), 0, 65536);

                if (!lpbyExternalBuf)
                    {
                    // Handle abnormal errors
                    FFD_Cleanup (hKeyList, hContent, lpbyExternalBuf);
                    LogError (gszOutOfMemory, ecb);
                    return FALSE;
                    }
                }

            // Move to data in the file
            SetFilePointer (hContent, dwOffset, NULL, FILE_BEGIN);

            // Read it
            bReturn = ReadFile (hContent, lpbyExternalBuf, dwLength, &dwRead, NULL);
            if (!bReturn || dwRead != dwLength)
                {
                // Handle abnormal errors
                FFD_Cleanup (hKeyList, hContent, lpbyExternalBuf);
                LogError (gszParseDiskError, ecb);
                return FALSE;
                }

            // Create another temporary file
            if (GetTempFileName (pParam->szTempDir, 
                                  pParam->szCGI, 0, 
                                  szExternalFileName))
                {
                // Open temp file for writing
                hExternalFile = CreateFile (szExternalFileName,
                                          GENERIC_WRITE,
                                          0,                 // No sharing mode
                                          NULL,              // Default security attribs
                                          CREATE_ALWAYS,
                                          FILE_ATTRIBUTE_NORMAL |
                                          FILE_FLAG_SEQUENTIAL_SCAN,
                                          NULL               // No template file
                                          );
                }
            else
                hExternalFile = INVALID_HANDLE_VALUE;

            // Check for errors
            if (hExternalFile == INVALID_HANDLE_VALUE)
                {
                // Handle abnormal errors
                FFD_Cleanup (hKeyList, hContent, lpbyExternalBuf);
                LogError (gszTempFileError, ecb);
                return FALSE;
                }

            // Write the data to this new file
            bReturn = WriteFile (hExternalFile, lpbyExternalBuf, dwLength, 
                                 &dwWritten, NULL);

            // Check for errors
            if (!bReturn || dwWritten != dwLength)
                {
                FFD_Cleanup (hKeyList, hContent, lpbyExternalBuf);
                CloseHandle (hExternalFile);
                DeleteFile (szExternalFileName);
                LogError (gszWriteError, ecb);
                return FALSE;
                }

            //
            // Close temp file.  See HttpExtensionProc for deletion.
            //
            CloseHandle (hExternalFile);

            //
            // Add key to [Form External] section
            //
            wsprintf ((LPTSTR) byKeyDataBuf, TEXT("%s %u"), 
                      szExternalFileName, dwLength);

            WritePrivateProfileString (pParam->szFormExternal,
                                       lpszKeyName, (LPTSTR) byKeyDataBuf,
                                       szProfile);
            }

        //
        // If length is 65536 or greater, just mark the location of
        // that data within the content file.
        //
        else
            {
            wsprintf ((LPTSTR) byKeyDataBuf, TEXT("%u %u"),
                      dwOffset, dwLength);

            WritePrivateProfileString (pParam->szFormHuge,
                                       lpszKeyName, (LPTSTR) byKeyDataBuf,
                                       szProfile);
            }
        }

    //
    // Cleanup
    //

    CloseHandle (hContent);
    if (lpbyExternalBuf)
        HeapFree (GetProcessHeap (), 0, lpbyExternalBuf);

    // See HttpExtensionProc for cleanup of key list
    pParam->hKeyList = hKeyList;

    //
    // Add to [CGI] and [System] sections to profile
    //
    szSection = pParam->szCGI;
    MACRO_WriteKey (TEXT("Content File"), pParam->lpszContentFile);
    MACRO_WriteKeyInt (TEXT("Content Length"), ecb->cbTotalBytes);
    MACRO_WriteKey (TEXT("Content Type"), ecb->lpszContentType);

    szSection = pParam->szSystem;
    MACRO_WriteKey (TEXT("Content File"), pParam->lpszContentFile);

    return TRUE;
    }


//
// GetShortName returns a pointer to the file name in a path
//

LPCTSTR GetShortName (LPCTSTR lpszFileName)
	{
	int i;

	for (i = lstrlen (lpszFileName) - 1 ; i >= 0 ; i--)
		{
		if (lpszFileName[i] == '\\')
			break;
		}
	i++;

	return (&lpszFileName[i]);
	}

//
// ExecuteChildProc builds a command line string and starts the
// Windows CGI app.  It returns a process handle if successful.
//

HANDLE ExecuteChildProc (PWCGIPARAMS pParam)
    {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    TCHAR szCmdLine[MAX_PATH * 4];
    BOOL bReturn;

#ifdef USE_11_CMD_LINE

	TCHAR szProfile[MAX_PATH];
	LPCTSTR szContent;

    // Build command line for 1.1
	wsprintf (szProfile, ".\\%s", GetShortName (pParam->szProfileName));

	if (pParam->hKeyList)	// content file created in keys.cpp
		szContent = pParam->lpszContentFile;
	else					// content file created in current dir
		szContent = GetShortName (pParam->lpszContentFile);

    wsprintf (szCmdLine, "%s %s %s %s ", 
			  GetShortName (gszAppName),
			  szProfile,
			  szContent,
			  GetShortName (pParam->szOutputFileName));

	if (pParam->pECB->lpszQueryString)
		lstrcat (szCmdLine, pParam->pECB->lpszQueryString);

#else

	// Build command line for 1.2
	wsprintf (szCmdLine, "%s %s",
			  GetShortName (gszAppName),
			  pParam->szProfileName);

#endif

    ZeroMemory (&si, sizeof (STARTUPINFO));

    si.cb = sizeof (STARTUPINFO);

    // Create process, return success or failure
	SetCurrentDirectory (pParam->szTempDir);
    bReturn = CreateProcess (gszAppName, 
                          szCmdLine,
                          NULL,         // default process security attrbs
                          NULL,         // default primary thread security
                          FALSE,        // don't inherit handles
                          CREATE_DEFAULT_ERROR_MODE|CREATE_NEW_PROCESS_GROUP,
                          NULL,			// no environment
                          NULL,			// initial dir
                          &si,			// startup info
                          &pi);			// outbound process info
    
    if (!bReturn)
        return INVALID_HANDLE_VALUE;

    return pi.hProcess;
    }


//
// GetDataFromFile reads the output file and sends all
// data to the web client.
//

BOOL GetDataFromFile (PWCGIPARAMS pParam)
    {
    HANDLE hOutputFile;
    LPBYTE lpbyBuf;
    DWORD dwBufSize;
    DWORD dwRead;
    DWORD dwSend;

    // Once again, readability...
    EXTENSION_CONTROL_BLOCK *ecb;
    ecb = pParam->pECB;

    // Allocate a buffer for moving data
    dwBufSize = 16384;
    lpbyBuf = (LPBYTE) HeapAlloc (GetProcessHeap (), 0, dwBufSize);
    if (!lpbyBuf)
        return FALSE;

    // Open output file
    hOutputFile = CreateFile (pParam->szOutputFileName,
                       GENERIC_READ,
                       0,                       // No sharing mode
                       NULL,                    // Default security attribs
                       OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL                     // No template file
                       );

    // Handle errors
    if (hOutputFile == INVALID_HANDLE_VALUE)
        {
        HeapFree (GetProcessHeap (), 0, lpbyBuf);
        return FALSE;
        }

	//
	// Although the WinCGI spec says that a WinCGI app should let the
	// Web server provide all header information, most WinCGI apps
	// still send header info mixed in the content.  When this is the
	// case, we have to read the output file and send header info
	// to ServerSupportFunction.
	//

#ifndef CONFORMANT_HEADER

	// 1K is plenty for header info
    if (!ReadFile (hOutputFile, lpbyBuf, 1024, &dwRead, NULL))
		dwRead = 0;

    if (dwRead)
        {
		// Determine end of header
		for (dwSend = 0 ; dwSend < dwRead ; dwSend++)
			{
			if (lpbyBuf[dwSend]		== '\r' &&
				lpbyBuf[dwSend + 1]	== '\n' &&
				lpbyBuf[dwSend + 2] == '\r' &&
				lpbyBuf[dwSend + 3] == '\n')
				{
				break;
				}

			else if (lpbyBuf[dwSend] == '\n' &&
					 lpbyBuf[dwSend + 1] == '\n')
				{
				lpbyBuf[dwSend] = '\r';
				lpbyBuf[dwSend + 2] = '\r';
				lpbyBuf[dwSend + 3] = '\n';
				break;
				}
			}

		if (dwSend == dwRead)
			dwRead = 0;
		else
			{
			dwSend += 4;
			dwRead = dwSend;
			lpbyBuf[dwSend] = 0;
			ecb->ServerSupportFunction (ecb->ConnID,
										HSE_REQ_SEND_RESPONSE_HEADER,
										NULL,
										&dwSend,
										(LPDWORD) lpbyBuf);
			}

		SetFilePointer (hOutputFile, dwRead, NULL, FILE_BEGIN);
        }
#endif


    // Loop until all data is read from the file
    do  {
        if (!ReadFile (hOutputFile, lpbyBuf, dwBufSize, &dwRead, NULL))
			break;

        if (dwRead)
            {
            dwSend = dwRead;
            ecb->WriteClient (ecb->ConnID, lpbyBuf, &dwSend, 0);
            if (dwSend != dwRead)
                {
                // Handle communication error
                CloseHandle (hOutputFile);
                HeapFree (GetProcessHeap (), 0, lpbyBuf);
                return FALSE;
                }
            }
        } while (dwRead);

    // Cleanup
    CloseHandle (hOutputFile);                                   
    HeapFree (GetProcessHeap (), 0, lpbyBuf);

    return TRUE;
    }


//
// LogError appends an error string to the application name,
// and then copies a string into the log buffer.  We use this
// function to support UNICODE builds of this DLL.
//

void LogError (LPCTSTR lpszError, EXTENSION_CONTROL_BLOCK *pECB)
    {
    LPTSTR lptsBuf;     // ts Hungarian means 'text string', safe for UNICODE

    // Alloc a buffer big enough for error string.
    // (add three for colon, space and null)
    lptsBuf = (LPTSTR) HeapAlloc (GetProcessHeap (), 0, 
                                    lstrlen (lpszError) + 
                                    lstrlen (gszAppName) + 3);
    if (!lptsBuf)
            return;     // can't log because of error

    // Build the error string
    wsprintf (lptsBuf, TEXT("%s: %s"), gszAppName, lpszError);

#ifdef UNICODE
    WideCharToMultiByte (CP_ACP, WC_COMPOSITECHECK|WC_DEFAULTCHAR,
                        lptsBuf, lstrlen (lptsError) + 1,
                        pECB->lpszLogData, HSE_LOG_BUFFER_LEN);
#else
    lstrcpyn (pECB->lpszLogData, lptsBuf, HSE_LOG_BUFFER_LEN);
#endif

    HeapFree (GetProcessHeap (), 0, lptsBuf);
    }

