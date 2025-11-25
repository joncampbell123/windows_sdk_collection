// Microsoft Developer Support Copyright (c) 1995-1996 Microsoft Corporation.
/*
version 1.0 (initial release)
version 1.1 (3.51 release)
  - added support for replaceable parameter strings ("%%xxx" codes)
  - corrected processing of registry API return codes and errors
  - added support for PrimaryModule key - default message file
  - added support for multiple message files per message
  - can now dump arbitrary log from command line
version 1.2
  - fixed bug with multiple paths ('path;path' format) for message DLL
  - display event type (error, warning, etc.) as string instead of number
  - dumps account names from SID in eventlog record
version 1.3
  - added category ID support
  - added logging of the callers Sid
version 1.4
  - added support for remote logging. Easier than I thought. :-)
  - removed FormatMessage call to insert message strings - do it manually
    like the Event Viewer to avoid unwanted formatting of the message string

ToDo: this app isn't super robust and will not cleanly recover from all
  possible problems, though I do try to handle the major, common problems.
  Robustness left as an excercise for the reader. :-) */

#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <process.h>
#include "messages.h"

#define PERR(bSuccess, api) {if (!(bSuccess)) printf("\n%s: Error %d from %s \
    on line %d\n", __FILE__, GetLastError(), api, __LINE__);}

#define REG_ERR(lRegError, api) {if (lRegError!=ERROR_SUCCESS) printf("%s: \
    Error %d from %s on line %d\n", __FILE__, lRegError, api, __LINE__);}

#define MAX_MSG_LENGTH 1024
#define MSG_ID_MASK 0x0000FFFF
/* MACHINE should either be NULL for local logging or a UNC name of a remote
   machine, i.e. "\\\\Server1" */
#define MACHINE NULL
/* if MACHINE != NULL, REMOTE_MSG_DLL_PATH must have a UNC path to the proper
   directory containing messages.dll. The machine must have access to
   this remote directory via the UNC pathname given */
#define REMOTE_MSG_DLL_PATH "\\\\ericsa2\\d$\\ntwork\\eventlog"

/*********************************************************************
* FUNCTION: addSourceToRegistry(LPSTR pszLogName, LPSTR pszSrcName,  *
*                               LPSTR pszMsgDLL)                     *
*                                                                    *
* PURPOSE: Add a source name key, message DLL name value, and        *
*          'message types supported' value to the registry           *
*                                                                    *
* INPUT: name of log (Application, System, or Security),             *
*        source name, path of message DLL                            *
*                                                                    *
* RETURNS: none                                                      *
*********************************************************************/

void addSourceToRegistry(LPSTR pszLogName, LPSTR pszSrcName, LPSTR pszMsgDLL)
{
  HKEY hk, hklm;          // registry key handle
  DWORD dwData;
  TCHAR szTemp[MAX_PATH];
  LONG lRegError;         // return value from registry APIs

  /* connect to remote registry (or local if MACHINE == NULL) */
  lRegError = RegConnectRegistry(MACHINE, HKEY_LOCAL_MACHINE, &hklm);
  REG_ERR(lRegError, "RegConnectRegistry");
  /* When a process uses the RegisterEventSource or OpenEventLog function to
     get a handle of an event log, the event logging service searches for the
     specified source name in the registry. You can add a new source name to
     the registry by opening a new registry subkey under the EventLog key and
     adding registry values to the new subkey. */
  // Create a new key for our source
  sprintf(szTemp, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s\\%s",
      pszLogName, pszSrcName);
  lRegError = RegCreateKey(hklm, szTemp, &hk);
  REG_ERR(lRegError, "RegCreateKey");
  RegCloseKey(hklm);
  // Add the Event-ID message-file name to the subkey.
  lRegError = RegSetValueEx(hk,  // subkey handle
      "EventMessageFile",        // value name
      0,                         // must be zero
      REG_EXPAND_SZ,             // value type
      (LPBYTE) pszMsgDLL,        // address of value data
      strlen(pszMsgDLL) + 1);    // length of value data
  REG_ERR(lRegError, "RegSetValueEx");

  /* Add the Category message-file name to the subkey. These category strings
     correspond to the fwCategory parameter passed into ReportEvent(). */
  lRegError = RegSetValueEx(hk,  // subkey handle
      "CategoryMessageFile",     // value name
      0,                         // must be zero
      REG_EXPAND_SZ,             // value type
      (LPBYTE) pszMsgDLL,        // address of value data
      strlen(pszMsgDLL) + 1);    // length of value data
  REG_ERR(lRegError, "RegSetValueEx");

  /* Add the replaceable parameter message-file name to the subkey. This is
     used to replace %%nnn values in messages with the appropriate string */
  lRegError = RegSetValueEx(hk,  // subkey handle
      "ParameterMessageFile",    // value name
      0,                         // must be zero
      REG_EXPAND_SZ,             // value type
      (LPBYTE) pszMsgDLL,        // address of value data
      strlen(pszMsgDLL) + 1);    // length of value data
  REG_ERR(lRegError, "RegSetValueEx");

  // Set the supported types flags and add it to the subkey.
  dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE |
    EVENTLOG_INFORMATION_TYPE;
  lRegError = RegSetValueEx(hk,  // subkey handle
      "TypesSupported",          // value name
      0,                         // must be zero
      REG_DWORD,                 // value type
      (LPBYTE) &dwData,          // address of value data
      sizeof(DWORD));            // length of value data
  REG_ERR(lRegError, "RegSetValueEx");

  // Set the number of categories supported
  dwData = 3;
  lRegError = RegSetValueEx(hk,  // subkey handle
      "CategoryCount",           // value name
      0,                         // must be zero
      REG_DWORD,                 // value type
      (LPBYTE) &dwData,          // address of value data
      sizeof(DWORD));            // length of value data
  REG_ERR(lRegError, "RegSetValueEx");
  RegCloseKey(hk);
  return;
} // addSourceToRegistry

/*********************************************************************
* FUNCTION: reportAnEvent(LPSTR pszSrcName, DWORD dwIdEvent,         *
*                         WORD wCategory, WORD cStrings,             *
*                         LPTSTR *pszStrings);                       *
*                                                                    *
* PURPOSE: add the event to the event log                            *
*                                                                    *
* INPUT: source name, the event ID to report in the log,             *
*        the category, the number of insert strings, and an array of *
*        null-terminated insert strings                              *
*                                                                    *
* RETURNS: none                                                      *
*********************************************************************/

void reportAnEvent(LPSTR pszSrcName, DWORD dwIdEvent, WORD wCategory,
     WORD cStrings, LPTSTR * pszStrings)
{
  #define SID_BUF_SIZE 1024
  HANDLE hLog;
  HANDLE hAccessToken;
  PTOKEN_USER ptgUser;
  BYTE UserBuf[SID_BUF_SIZE];
  DWORD cbUserBufSize = SID_BUF_SIZE;
  BOOL bSuccess;

  // get the current process access token
  bSuccess = OpenProcessToken(
            GetCurrentProcess(),
            TOKEN_QUERY,
            &hAccessToken);
  PERR(bSuccess, "OpenProcessToken");

  ptgUser = (PTOKEN_USER) UserBuf;

  // get the TOKEN_USER structure from the access token, which contains
  // the Sid representing the caller
  bSuccess = GetTokenInformation(
            hAccessToken,
            TokenUser,
            ptgUser,
            cbUserBufSize,
            &cbUserBufSize
            );
  PERR(bSuccess, "GetTokenInformation");

  CloseHandle(hAccessToken);

  // Get a handle to the event log
  hLog = RegisterEventSource(MACHINE,  // use machine defined elsewhere
      pszSrcName);                  // source name
  PERR(hLog, "RegisterEventSource");

  // Now report the event, which will add this event to the event log
  bSuccess = ReportEvent(hLog,  // event-log handle
      EVENTLOG_ERROR_TYPE,      // event type
      wCategory,                // category
      dwIdEvent,                // event ID
      ptgUser->User.Sid,        // Sid representing caller
      cStrings,                 // number of substitution strings
      0,                        // no binary data
      pszStrings,               // string array
      NULL);                    // address of data
  PERR(bSuccess, "ReportEvent");

  DeregisterEventSource(hLog);

  return;
} // reportAnEvent

/*********************************************************************
* FUNCTION: insertMsgString(HKEY hk, PTCHAR szMsg, WORD wNumStrings  *
*     PTCHAR szInsStrings);                                          *
*                                                                    *
* PURPOSE: replace "%xxx" parameters in message strings with         *
*          appropriate value from eventlog insert strings            *
*                                                                    *
* INPUT: message to scan for "%xxx" parameters, strings to insert,   *
*        and the number of strings to insert                         *
*                                                                    *
* RETURNS: the string resulting from the string replacement          *
*          operation. This memory must be freed by the caller via    *
*          free().                                                   *
*********************************************************************/

// size of replaceable parameter strings - should fit a DWORD as a string
#define PARAM_SIZE (16)

PTCHAR insertMsgString(HKEY hk, PTCHAR szMsg, WORD wNumStrings,
    PTCHAR szInsStrings)
{
  BOOL bSuccess;
  DWORD dwType;    // for RegQueryValueEx
  LONG lRegError;  // return value from registry APIs
  // paths to ParameterMessageFile
  TCHAR szParamMsgPath[MAX_PATH], szTemp[MAX_PATH];
  DWORD dwcbData;
  HINSTANCE hLib;   // handle to DLL containing ParameterMessageFile
  DWORD dwID;       // message ID
  LPTSTR msgBuf;
  PTCHAR szTempMsg;  // the message with replaced "%" or "%%" insert strings
  PTCHAR p1 = szMsg, p2 = szMsg; // temp pointers to walk the string
  PTCHAR szInsStr;   // point to insert strings in the EVENTLOGRECORD struct
  DWORD cchDest;
  int i;

  szTempMsg = malloc(MAX_MSG_LENGTH);
  szTempMsg[0] = 0;
  while (p2)
  {
    p2 = strchr(p1, '%');
    if (!p2)  // "%" not found, just copy the rest of the string and quit
    {
      strcat(szTempMsg, p1);
      continue;
    }
    strncat(szTempMsg, p1, p2 - p1);  // copy up to "%"
    /* is this a special "%%" string? If so, replace with language-dependant
       insert string from the ParameterMessageFile from registry. If it's not
       a "%%" string, skip down to else clause to replace as normal insert
       strings from the EVENTLOGRECORD structure */
    if (p2[1] == '%') // yup, it's a "%%" parameter string
    {
      p2 += 2;          // point to number, past the "%%"
      dwID = atoi(p2);  // get the message ID following the "%%"
      if (!dwID)        // it's not a number, copy "%%" as-is
      {
        strcat(szTempMsg, "%%");
        p1 = p2;
        continue;
      }
      else
        p1 = p2 + strspn(p2, "0123456789"); // point past number

      /* now look up the string corresponding to the message ID and append
         it into our temp string */
      dwcbData = MAX_PATH;
      lRegError = RegQueryValueEx(hk,  // handle of key to query
          "ParameterMessageFile",      // value name
          NULL,                        // must be NULL
          &dwType,                     // address of type value
          szTemp,                      // path to replaceable parameter message file
          &dwcbData);                  // length of value data
      REG_ERR(lRegError, "RegQueryValueEx");

      /* Expand environment variable strings in the message DLL path name, in
         case any are there */
      cchDest = ExpandEnvironmentStrings(szTemp, szParamMsgPath, MAX_PATH);
      PERR(cchDest != 0, "ExpandEnvironmentStrings");
      PERR(cchDest < MAX_PATH, "ExpandEnvironmentStrings");

      hLib = LoadLibraryEx(szParamMsgPath, NULL, DONT_RESOLVE_DLL_REFERENCES);
      PERR(hLib != NULL, "LoadLibraryEx");
      bSuccess = FormatMessage(
          FORMAT_MESSAGE_FROM_HMODULE |     // get the message from the DLL
          FORMAT_MESSAGE_ALLOCATE_BUFFER |  // allocate the msg buffer
          FORMAT_MESSAGE_ARGUMENT_ARRAY |   // lpArgs is an array of pointers
          FORMAT_MESSAGE_IGNORE_INSERTS |   // don't do any formatting
          70,                     // line length for the mesages
          hLib,                   // the messagetable DLL handle
          dwID,                   // message ID
          LANG_USER_DEFAULT,      // language ID
          (LPTSTR) &msgBuf,       // address of pointer to buffer for message
          PARAM_SIZE,             // maximum size of the message buffer
          NULL);                  // array of insert strings for the message
      PERR(bSuccess, "FormatMessage");
      FreeLibrary(hLib);
      strcat(szTempMsg, msgBuf);
      LocalFree((HLOCAL) msgBuf);
    }
    else //a normal insert string, look up in EVENTLOGRECORD structure
    {
      p2++;            // point to number, past the "%"
      dwID = atoi(p2);  // get the message ID following the "%"
      if (!dwID)        // it's not a number, copy "%" as-is
      {
        strcat(szTempMsg, "%");
        p1 = p2;
        continue;
      }
      else
        p1 = p2 + strspn(p2, "0123456789"); // point past number
      szInsStr = szInsStrings;
      for (i = 1; i < (int) dwID; i++) // find string number dwID
      {
        if (i > wNumStrings) // handle bogus string number
        {
          puts("**Error! Insert string ID too large");
          continue;
        }
        szInsStr += (strlen(szInsStr) + 1); // point to next string
      }
      strcat(szTempMsg, szInsStr);
    } //else
  } //while
  return (szTempMsg);  // caller must free()!
} // insertMsgString

/*********************************************************************
* FUNCTION: lookupStringFromMsgDll(HKEY hk, DWORD dwEventID,         *
*     WORD wNumStrings, PTCHAR szStrings, PTCHAR szMsgDllPath);      *
*                                                                    *
* PURPOSE: look up a string from a message file                      *
*                                                                    *
* INPUT: root of the key containing the eventlog info for this       *
*        eventlog source, the ID of the string to look up, the       *
*        number of replacement strings, the replacement strings      *
*        from the EVENTLOGRECORD structure, and the list of message  *
*        DLLs to search for the message                              *
*                                                                    *
* RETURNS: the string string from the message file. This string      *
*          should be freed by the caller                             *
*********************************************************************/

PTCHAR lookupStringFromMsgDll(HKEY hk, DWORD dwEventID, WORD wNumStrings,
    PTCHAR szStrings, PTCHAR szMsgDllPath)
{
  DWORD cchDest;
  PTCHAR pMsgDll;   // one of the message DLLs in the list
  PTCHAR pNextDll;  // pointer to next DLL in the list
  HINSTANCE hLib;  // handle to the messagetable DLL
  LPTSTR msgBuf;   // hold text of the error message that we build
  PTCHAR p;
  TCHAR szTempMsg[MAX_MSG_LENGTH];
  BOOL bSuccess;
  TCHAR szMsgDllPathEx[MAX_PATH];  // after environment string expansion

  /* Expand environment variable strings in the message DLL path name, in
     case any are there, such as %systemroot% */
  cchDest = ExpandEnvironmentStrings(szMsgDllPath, szMsgDllPathEx, MAX_PATH);
  PERR(cchDest != 0, "ExpandEnvironmentStrings");
  PERR(cchDest < MAX_PATH, "ExpandEnvironmentStrings");

  // The list is delimited by ';' chars, check each DLL for the message
  pMsgDll = szMsgDllPathEx;  // point to beginning of DLL list
  while (pMsgDll)
  {
    pNextDll = strchr(szMsgDllPathEx, ';');
    if (pNextDll)
    {
      *pNextDll = 0;  // replace ';' with 0 to null terminate the name
      pNextDll++;     // point to the next name in the list
    }

    // Now we've got a message DLL name, load the DLL.
    hLib = LoadLibraryEx(pMsgDll, NULL, DONT_RESOLVE_DLL_REFERENCES);
    PERR(hLib != NULL, "LoadLibraryEx");
    if (!hLib)
      continue;

    /* retrieve the message from the messagetable DLL. For the language
    identifier, use the values we defined in the LanguageNames statements in
    the .mc file. If no LanguageNames values were defined in the .mc file, as
    in this case, use the LANG_USER_DEFAULT macro defined in winnt.h */
    bSuccess = FormatMessage(
        FORMAT_MESSAGE_FROM_HMODULE |     // get the message from the DLL
        FORMAT_MESSAGE_ALLOCATE_BUFFER |  // allocate the msg buffer for us
        FORMAT_MESSAGE_ARGUMENT_ARRAY |   // lpArgs is an array of pointers
        FORMAT_MESSAGE_IGNORE_INSERTS |   // just get the message
        70,                 // line length for the mesages
        hLib,               // the messagetable DLL handle
        dwEventID,          // message ID
        LANG_USER_DEFAULT,  // language ID
        (LPTSTR) &msgBuf,   // address of pointer to buffer for message
        MAX_MSG_LENGTH,     // maximum size of the message buffer
        NULL);              // array of insert strings for the message
    // if the message was not found, try the next DLL in the list
    if (!bSuccess && GetLastError() == ERROR_MR_MID_NOT_FOUND && pNextDll)
    {
      FreeLibrary(hLib);
      pMsgDll = pNextDll;  // point to next DLL in list (or NULL if no more)
      continue;            /* if the msg doesn't exist, and we have more
                              DLL's to check, continue down the list */
    }
    PERR(bSuccess, "FormatMessage");

    // if everything is ok, continue on, otherwise proceed to next event
    if (bSuccess)
    {
      /* process insert strings. Call twice - insert strings may contain
         special "%%" language-dependant replaceable parameter strings */
      strcpy(szTempMsg, msgBuf);
      LocalFree((HLOCAL) msgBuf);
      p = insertMsgString(hk, szTempMsg, wNumStrings, szStrings);
      strcpy(szTempMsg, p);
      free(p);  // free buffer allocated by insertParamString()
      p = insertMsgString(hk, szTempMsg, wNumStrings, szStrings);
      strcpy(szTempMsg, p);
      //don't free p again, it will be returned to the caller to free.
    } // if
    else
      p = NULL;
    pMsgDll = pNextDll;  // point to next DLL in list (or NULL if no more)
    // free the message DLL since we don't know if we'll need it again
    FreeLibrary(hLib);
  } // while
  return(p);  // p will be freed by our caller
} // lookupStringFromMsgDll

/*********************************************************************
* FUNCTION: queryEventLog(HANDLE hEventLog, LPCTSTR pszLogName,      *
*           LPCTSTR pszSourceName)                                   *
*                                                                    *
* PURPOSE: dump out some of the data for each event in the specified *
*          event log                                                 *
*                                                                    *
* INPUT: handle of the open eventlog (or NULL if not open), the name *
*        of the log to dump (application, system, security),         *
*        and the name of an eventlog source within that log (or NULL *
*        if not available, but required if hLog is NULL)             *
*                                                                    *
* RETURNS: none                                                      *
*                                                                    *
* NOTES: Add functionality to get category strings from category     *
*        DLL and display them along with the message string          *
*********************************************************************/

void queryEventLog(HANDLE hEventLog, LPCTSTR pszLogName, LPCTSTR pszSourceName)
{
  EVENTLOGRECORD *pevlr;
  BYTE bBuffer[1024 * 64];    // hold the event log record raw data
  DWORD dwRead, dwNeeded, dwThisRecord = 0;
  HANDLE hLog;                // handle to the log
  BOOL bSuccess;
  DWORD cRecords;             // number of event records in the log
  LONG lRegError;
  DWORD dwType;
  DWORD dwcbData;
  HKEY hklm, hk;
  TCHAR szTemp[MAX_PATH];
  TCHAR szPrimaryMsgFile[64];  // name of primary (default) message file
  PTCHAR p;

  if (!hEventLog)
  {
    hLog = OpenEventLog(NULL,  // use local machine
        pszSourceName);        // source name - assumed to be valid
    PERR(hLog, "OpenEventLog");
  }
  else
    hLog = hEventLog;

  // Get the number of records in the log.
  bSuccess = GetNumberOfEventLogRecords(hLog, &cRecords);
  PERR(bSuccess, "GetNumberOfEventLogRecords");
  printf("There are %d records in the %s log.\n", cRecords, pszLogName);

  /* If the eventlog key contains a PrimaryModule value, get the default
     messagefile from it. This is the default EventMessageFile if an
     EventMessageFile value doesn't exist for a given source. */
  sprintf(szTemp, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s",
      pszLogName);
  lRegError = RegConnectRegistry(MACHINE, HKEY_LOCAL_MACHINE, &hklm);
  REG_ERR(lRegError, "RegConnectRegistry");
  lRegError = RegOpenKeyEx(hklm, szTemp, 0, KEY_READ, &hk);
  REG_ERR(lRegError, "RegOpenKeyEx");
  RegCloseKey(hklm);
  dwcbData = MAX_PATH;
  lRegError = RegQueryValueEx(hk,  // handle of key to query
      "PrimaryModule",             // value name
      NULL,                        // must be NULL
      &dwType,                     // address of type value
      szPrimaryMsgFile,            // path to primary (default) message DLL
      &dwcbData);                  // length of value data
  if (lRegError == ERROR_FILE_NOT_FOUND)  // the key doesn't exist
    szPrimaryMsgFile[0] = 0;       // so we can test for it later
  else
    REG_ERR(lRegError, "RegQueryValueEx");
  RegCloseKey(hk);
  /* if the key exists, get the default EventMessageFile from the specified
     subkey */
  if (lRegError == ERROR_SUCCESS)
  {
    strcat(szTemp, "\\");
    strcat(szTemp, szPrimaryMsgFile);
    lRegError = RegConnectRegistry(MACHINE, HKEY_LOCAL_MACHINE, &hklm);
    REG_ERR(lRegError, "RegConnectRegistry");
    lRegError = RegOpenKeyEx(hklm, szTemp, 0, KEY_READ, &hk);
    REG_ERR(lRegError, "RegOpenKeyEx");
    RegCloseKey(hklm);
    dwcbData = MAX_PATH;
    lRegError = RegQueryValueEx(hk,  // handle of key to query
        "EventMessageFile",          // value name
        NULL,                        // must be NULL
        &dwType,                     // address of type value
        szPrimaryMsgFile,            // path(s) to message DLL
        &dwcbData);                  // length of value data
    REG_ERR(lRegError, "RegQueryValueEx");
    RegCloseKey(hk);
  }

  pevlr = (EVENTLOGRECORD *) &bBuffer;
  /* Opening the log positions the file pointer for this handle at the
     beginning of the log. Read records sequentially until there are no more
     records. */
  while (bSuccess = ReadEventLog(hLog,  // event-log handle
          EVENTLOG_FORWARDS_READ |      // read forward
          EVENTLOG_SEQUENTIAL_READ,     // sequential read
          0,                            // ignored for sequential reads
          bBuffer,                      // address of buffer
          sizeof(bBuffer),              // size of buffer
          &dwRead,                      // count of bytes read
          &dwNeeded))                   // bytes in next record
  {
    while (dwRead > 0)
    {
      /* Print the event ID, type, source name, and category. The source name
         is just past the end of the formal structure. */
      printf("\n%03d  Event ID: 0x%08X  EventType: ", dwThisRecord++,
          pevlr->EventID);
      switch (pevlr->EventType)
      {
        case EVENTLOG_ERROR_TYPE:
          printf("EVENTLOG_ERROR_TYPE");
          break;
        case EVENTLOG_WARNING_TYPE:
          printf("EVENTLOG_WARNING_TYPE");
          break;
        case EVENTLOG_INFORMATION_TYPE:
          printf("EVENTLOG_INFORMATION_TYPE");
          break;
        case EVENTLOG_AUDIT_SUCCESS:
          printf("EVENTLOG_AUDIT_SUCCESS");
          break;
        case EVENTLOG_AUDIT_FAILURE:
          printf("EVENTLOG_AUDIT_FAILURE");
          break;
        default:
          printf("UNKNOWN");
          break;
      }

      printf("\nSource: %s", (LPSTR) ((LPBYTE) pevlr + sizeof(EVENTLOGRECORD)));
      // mask off the actual message number and show it
      printf("  Message ID: %d", pevlr->EventID & MSG_ID_MASK);

      /* From the event log source name, we know the name of the registry key
         to look under for the list of the message DLLs that may contain the
         message we need to extract with FormatMessage. Get the event log
         source name list... */
      // The source name follows the EVENTLOGRECORD structure
      sprintf(szTemp, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s\\%s",
          pszLogName, (PTCHAR) ((LPBYTE) pevlr + sizeof(EVENTLOGRECORD)));
      /* Now open this key and get the EventMessageFile value, which is the
         list of the message DLLs for that source */
      lRegError = RegConnectRegistry(MACHINE, HKEY_LOCAL_MACHINE, &hklm);
      REG_ERR(lRegError, "RegConnectRegistry");
      lRegError = RegOpenKeyEx(hklm, szTemp, 0, KEY_READ, &hk);
      REG_ERR(lRegError, "RegOpenKeyEx");
      RegCloseKey(hklm);
      // first, look up the path(s) to the category message DLL
      dwcbData = MAX_PATH;
      lRegError = RegQueryValueEx(hk,  // handle of key to query
          "CategoryMessageFile",       // value name
          NULL,                        // must be NULL
          &dwType,                     // address of type value
          szTemp,                      // path to category message DLL
          &dwcbData);                  // length of value data
        REG_ERR(lRegError, "RegQueryValueEx");

      // get the string from the category DLL
      p = lookupStringFromMsgDll(hk, pevlr->EventCategory, 0, NULL, szTemp);
      printf("  Category: %s", p);

      // print out the user's account name if the SID was logged in the record
      if (pevlr->UserSidLength)
      {
        UCHAR szAccountName[64], szDomainName[64];
        DWORD cbAccountSize, cbDomainSize;
        SID_NAME_USE snu;
        PSID psid;

        /* get the user name associated with the Security identifier (SID)
           in the event log record */
        psid = (PSID)((LPBYTE) pevlr + pevlr->UserSidOffset);
        cbAccountSize = sizeof(szAccountName);
        cbDomainSize = sizeof(szDomainName);
        bSuccess = LookupAccountSid(NULL, psid, szAccountName, &cbAccountSize,
            szDomainName, &cbDomainSize, &snu);
        PERR(bSuccess, "LookupAccountSid");
        printf("  User: %s", bSuccess ? szAccountName : "NA");
      }

      // now look up the path(s) to the message DLL to get the message text
      dwcbData = MAX_PATH;
      lRegError = RegQueryValueEx(hk,  // handle of key to query
          "EventMessageFile",          // value name
          NULL,                        // must be NULL
          &dwType,                     // address of type value
          szTemp,                      // path(s) to message DLL
          &dwcbData);                  // length of value data
      // if the message file isn't there, and a default exists, use default
      if (lRegError == ERROR_FILE_NOT_FOUND && szPrimaryMsgFile[0])
        strcpy(szTemp, szPrimaryMsgFile);
      else
        REG_ERR(lRegError, "RegQueryValueEx");

      /* get the message from the message DLL, replacing parameters and
         replaceable parameter strings as appropriate */
      p = lookupStringFromMsgDll(hk, pevlr->EventID, pevlr->NumStrings,
          (PTCHAR) ((LPBYTE) pevlr + pevlr->StringOffset), szTemp);
      printf("\nMessage: %s\n", p);

      // Subtract the size of the event log record we just read
      dwRead -= pevlr->Length;
      // Point to the next event log record in the buffer
      pevlr = (EVENTLOGRECORD *) ((LPBYTE) pevlr + pevlr->Length);
      RegCloseKey(hk);  // close key for this event source
    } // while
    /* reset our event log record pointer back to the beginning of the buffer
       in preparation for reading the next record. */
    pevlr = (EVENTLOGRECORD *) &bBuffer;
  } // while
  if (GetLastError() != ERROR_HANDLE_EOF)
    PERR(bSuccess, "ReadEventLog");
  RegCloseKey(hk);
  if (!hEventLog)  // if we had to open the event log, close it
  {
    bSuccess = CloseEventLog(hLog);
    PERR(bSuccess, "CloseEventLog");
  }
  return;
} // queryEventLog

void usage()
{
  puts("log [logname] [backuplog]>");
  puts("[logname] is an optional eventlog category such as Application, Security,"
      " or System."
      "[backuplog] is the filename of a backup log file to view. If this\n"
      "parameter is specified, [logname] must be given as well.");
  return;
} // usage()

void notifyChange(LPCTSTR logSource)
{
  BOOL bSuccess;
  HANDLE hEventLog, hEvent;
  DWORD dwWaitResult;

  hEvent = CreateEvent(NULL,    // address of security attributes
      FALSE,                    // no manual reset
      FALSE,                    // create as not signaled
      NULL);                    // event name
  hEventLog = OpenEventLog(MACHINE,  // open on machine defined elsewhere
      logSource);                 // event log source name
  PERR(hEventLog, "OpenEventLog");
  bSuccess = NotifyChangeEventLog(hEventLog,
      hEvent);
  PERR(bSuccess, "NotifyChangeEventLog");
  dwWaitResult = WaitForSingleObject(hEvent, INFINITE);
  if (dwWaitResult != WAIT_FAILED)
    printf("**Notification: %s has changed\n", logSource);
  else
    PERR(dwWaitResult == WAIT_FAILED, "EventLogNotifyProc");
  CloseHandle(hEvent);
  CloseHandle(hEventLog);
  return;
} // notifyChange

#define LOGNAME "Application"
#define LOGSOURCE "LogSample"

int main(int argc, char *argv[])
{
  TCHAR szMsgPath[MAX_PATH];
  PTCHAR aInsertStrs[16];  // array of pointers to insert strings
  TCHAR szTemp[MAX_PATH];
  BOOL bSuccess;
  HANDLE hEventLog;
  OSVERSIONINFO osv;

  // Check to make sure we are running on Windows NT
  osv.dwOSVersionInfoSize = sizeof(osv);
  bSuccess = GetVersionEx(&osv);
  if (osv.dwPlatformId != VER_PLATFORM_WIN32_NT)
  {
    MessageBox(NULL, "Sorry, this application requires Windows NT.\n"
        "This application will now terminate.",
        "Error", MB_OK);
    return (1);
  }

  if (argc == 1)  // no args, use default defines
  {
    /* set up a notification event thread that will print a message when the
       first message is added to LOGSOURCE. This will only work for local
       logging. */
    if (MACHINE == NULL)
      _beginthread(notifyChange, 0, LOGSOURCE);

    /* Set the Event-ID message-file name. For local logging, we assume that
       the message DLL is in the same directory as this application. For
       remote logging, this sample will access the message DLL over the net
       via a UNC name for simplicity. This will be very slow when reading a
       large number of events from the eventlog. */
    if (MACHINE == NULL)
      GetCurrentDirectory(sizeof(szMsgPath), szMsgPath);
    else
      strcpy(szMsgPath, REMOTE_MSG_DLL_PATH);
    strcat(szMsgPath, "\\messages.dll");
    addSourceToRegistry(LOGNAME, LOGSOURCE, szMsgPath);

    // clear the event log
    hEventLog = OpenEventLog(MACHINE,  // open on machine defined elsewhere
        LOGSOURCE);                 // event log source name
    PERR(hEventLog, "OpenEventLog");
    bSuccess = ClearEventLog(hEventLog, NULL);
    PERR(bSuccess, "ClearEventLog");
    // can't use the log handle now that it's been cleared, we must close it
    bSuccess = CloseEventLog(hEventLog);
    PERR(bSuccess, "CloseEventLog");

    // Set up our array of insert strings for our error message
    reportAnEvent(LOGSOURCE,  // source name
        MSG_BAD_COMMAND,      // the message to log
        CAT_1,                // category
        0,                    // number of insert strings
        aInsertStrs);         // the array of insert strings
    reportAnEvent(LOGSOURCE,  // source name
        MSG_BAD_PARM1,        // the message to log
        CAT_3,                // category
        0,                    // number of insert strings
        aInsertStrs);         // the array of insert strings
    reportAnEvent(LOGSOURCE,  // source name
        MSG_STRIKE_ANY_KEY,   // the message to log
        CAT_2,                // category
        0,                    // number of insert strings
        aInsertStrs);         // the array of insert strings
    aInsertStrs[0] = "2,567";
    aInsertStrs[1] = "10";
    reportAnEvent(LOGSOURCE,  // source name
        MSG_RETRYS,           // the message to log
        CAT_1,                // category
        2,                    // number of insert strings
        aInsertStrs);         // the array of insert strings
    strcpy(szTemp, "%%");
    itoa(SPROCKETS, szTemp + 2, 10);  // concatenate the value
    aInsertStrs[0] = "foo.dat";
    aInsertStrs[1] = szTemp;
    reportAnEvent(LOGSOURCE,    // source name
        MSG_FILE_BAD_CONTENTS,  // the message to log
        CAT_1,                  // category
        2,                      // number of insert strings
        aInsertStrs);           // the array of insert strings
    aInsertStrs[0] = "a:";
    reportAnEvent(LOGSOURCE,    // source name
        MSG_INSERT_DISK,        // the message to log
        CAT_1,                  // category
        1,                      // number of insert strings
        aInsertStrs);           // the array of insert strings
    queryEventLog(NULL, LOGNAME, LOGSOURCE);  // query the log
  }
  else
  {  // we've got parameters
    if (argc > 1 && !strcmpi(argv[1], "Application") ||
        !strcmpi(argv[1], "Security") || !strcmpi(argv[1], "System"))
    {
      if (argc > 2)  // got a backup log name
      {
        hEventLog = OpenBackupEventLog(NULL, argv[2]); // must be local
        if (hEventLog)
        {
          queryEventLog(hEventLog, argv[1], NULL);  // dump the backup log
          bSuccess = CloseEventLog(hEventLog);
          PERR(bSuccess, "CloseEventLog");
        }
        else
        {  // 2nd arg was bad
          printf("can't open backup eventlog %s\n", argv[2]);
          PERR(hEventLog, "OpenBackupEventLog");
          usage();
        }
      }
      else
      {  // only got logname, dump it
        HKEY hk, hklm;
        FILETIME ft;
        LONG lRegError;
        DWORD cchName = sizeof(szTemp);

        // get arbitrary subkey of param to be used to open event log
        sprintf(szTemp, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s",
            argv[1]);
        lRegError = RegConnectRegistry(MACHINE, HKEY_LOCAL_MACHINE, &hklm);
        REG_ERR(lRegError, "RegConnectRegistry");
        lRegError = RegOpenKeyEx(hklm, szTemp, 0, KEY_READ, &hk);
        REG_ERR(lRegError, "RegOpenKeyEx");
        RegCloseKey(hklm);
        lRegError = RegEnumKeyEx(hk, 0, szTemp, &cchName, NULL,
            NULL, NULL, &ft);
        REG_ERR(lRegError, "RegEnumKeyEx");
        queryEventLog(NULL, argv[1], szTemp);  // query the log
      }
    }
    else  // first arg was bad
      usage();
  }  // else
  return (0);
}
