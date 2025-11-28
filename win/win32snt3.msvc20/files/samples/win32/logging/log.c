/* Microsoft Developer Support Copyright (c) 1993 Microsoft Corporation. */

#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include "messages.h"

#define PERR(bSuccess, api) {if(!(bSuccess)) printf("%s: Error %d from %s \
    on line %d\n", __FILE__, GetLastError(), api, __LINE__);}

#define MAX_MSG_LENGTH 1024
#define MSG_ID_MASK 0x0000FFFF
#define MAX_INSERT_STRS 8

/*********************************************************************
* FUNCTION: addSourceToRegistry(void)                                *
*                                                                    *
* PURPOSE: Add a source name key, message DLL name value, and        *
*          message type supported value to the registry              *
*                                                                    *
* INPUT: source name, path of message DLL                            *
*                                                                    *
* RETURNS: none                                                      *
*********************************************************************/

void addSourceToRegistry(LPSTR pszAppname, LPSTR pszMsgDLL)
{
  HKEY hk;                      /* registry key handle */
  DWORD dwData;
  BOOL bSuccess;

  /* When an application uses the RegisterEventSource or OpenEventLog
     function to get a handle of an event log, the event loggging service
     searches for the specified source name in the registry. You can add a
     new source name to the registry by opening a new registry subkey
     under the Application key and adding registry values to the new
     subkey. */

  /* Create a new key for our application */
  bSuccess = RegCreateKey(HKEY_LOCAL_MACHINE,
      "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\log", &hk);
  PERR(bSuccess == ERROR_SUCCESS, "RegCreateKey");

  /* Add the Event-ID message-file name to the subkey. */
  bSuccess = RegSetValueEx(hk,  /* subkey handle         */
      "EventMessageFile",       /* value name            */
      0,                        /* must be zero          */
      REG_EXPAND_SZ,            /* value type            */
      (LPBYTE) pszMsgDLL,       /* address of value data */
      strlen(pszMsgDLL) + 1);   /* length of value data  */
  PERR(bSuccess == ERROR_SUCCESS, "RegSetValueEx");

  /* Set the supported types flags and addit to the subkey. */
  dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE |
      EVENTLOG_INFORMATION_TYPE;
  bSuccess = RegSetValueEx(hk,  /* subkey handle                */
      "TypesSupported",         /* value name                   */
      0,                        /* must be zero                 */
      REG_DWORD,                /* value type                   */
      (LPBYTE) &dwData,         /* address of value data        */
      sizeof(DWORD));           /* length of value data         */
  PERR(bSuccess == ERROR_SUCCESS, "RegSetValueEx");
  RegCloseKey(hk);
  return;
}

/*********************************************************************
* FUNCTION: reportAnEvent(DWORD dwIdEvent, WORD cStrings,            *
*                         LPTSTR *ppszStrings);                      *
*                                                                    *
* PURPOSE: add the event to the event log                            *
*                                                                    *
* INPUT: the event ID to report in the log, the number of insert     *
*        strings, and an array of null-terminated insert strings     *
*                                                                    *
* RETURNS: none                                                      *
*********************************************************************/

void reportAnEvent(DWORD dwIdEvent, WORD cStrings, LPTSTR *pszStrings)
{
  HANDLE hAppLog;
  BOOL bSuccess;

  /* Get a handle to the Application event log */
  hAppLog = RegisterEventSource(NULL,   /* use local machine      */
      "log");                   /* source name                 */
  PERR(hAppLog, "RegisterEventSource");

  /* Now report the event, which will add this event to the event log */
  bSuccess = ReportEvent(hAppLog,       /* event-log handle            */
      EVENTLOG_ERROR_TYPE,      /* event type                  */
      0,                        /* category zero               */
      dwIdEvent,                /* event ID                    */
      NULL,                     /* no user SID                 */
      cStrings,                 /* number of substitution strings     */
      0,                        /* no binary data              */
      pszStrings,               /* string array                */
      NULL);                    /* address of data             */
  PERR(bSuccess, "ReportEvent");
  DeregisterEventSource(hAppLog);
  return;
}

/*********************************************************************
* FUNCTION: queryEventLog(void)                                      *
*                                                                    *
* PURPOSE: dump out some of the data for each event in the event log *
*                                                                    *
* INPUT: none                                                        *
*                                                                    *
* RETURNS: none                                                      *
*********************************************************************/

void queryEventLog(void)
{
  EVENTLOGRECORD *pevlr;
  BYTE bBuffer[512];            /* hold the event log record raw data */
  DWORD dwRead, dwNeeded, dwThisRecord = 0;
  HANDLE hAppLog;               /* handle to the application log */
  BOOL bSuccess;
  DWORD cRecords;               /* number of event records in the log */
  LONG lSuccess;
  DWORD dwType;
  char szMsgDll[MAX_PATH];      /* the name of the message DLL */
  DWORD dwcbData;
  HKEY hk;
  HINSTANCE hLib;               /* handle to the messagetable DLL */
  char szTemp[MAX_PATH];
  LPTSTR msgBuf;                /* hold text of the error message that we
                                   build */
  DWORD cchDest;
  char *aInsertStrs[MAX_INSERT_STRS];   /* array of pointers to insert
                                           strings */
  int i;
  char *p;

  /* Open the Application log. */
  hAppLog = OpenEventLog(NULL,  /* use local machine */
      "Application");           /* source name       */
  PERR(hAppLog, "OpenEventLog");

  /* Get the number of records in the Application log. */
  bSuccess = GetNumberOfEventLogRecords(hAppLog, &cRecords);
  PERR(bSuccess, "GetNumberOfEventLogRecords");
  printf("There are %d records in the Application log.\n", cRecords);

  pevlr = (EVENTLOGRECORD *) &bBuffer;

  /* Opening the log positions the file pointer for this handle at the
     beginning of the log. Read records sequentially until there are no
     more records. */
  while (bSuccess = ReadEventLog(hAppLog,       /* event-log handle */
          EVENTLOG_FORWARDS_READ |      /* read forward     */
          EVENTLOG_SEQUENTIAL_READ,     /* sequential read  */
          0,                    /* ignored for sequential reads */
          bBuffer,              /* address of buffer            */
          sizeof(bBuffer),      /* size of buffer               */
          &dwRead,              /* count of bytes read          */
          &dwNeeded))           /* bytes in next record         */
  {
    while (dwRead > 0)
    {
      /* Print the event ID, type, and source name. The source name is
         just past the end of the formal structure. */
      printf("%03d  Event ID: 0x%08X ", dwThisRecord++, pevlr->EventID);
      printf("EventType: %d Source: %s\n", pevlr->EventType,
          (LPSTR) ((LPBYTE) pevlr + sizeof(EVENTLOGRECORD)));

      /* From the event log source name, we know the name of the registry
         key to look under for the name of the message DLL that contains
         the messages we need to extract with FormatMessage. So first get
         the event log source name... */
      strcpy(szTemp, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\");
      /* The source name follows the EVENTLOGRECORD structure */
      strcat(szTemp, (char *)((LPBYTE) pevlr + sizeof(EVENTLOGRECORD)));

      /* Now open this key and get the EventMessageFile value, which is
         the name of the message DLL. */
      lSuccess = RegOpenKey(HKEY_LOCAL_MACHINE, szTemp, &hk);
      PERR(lSuccess == ERROR_SUCCESS, "RegOpenKey");
      dwcbData = MAX_PATH;
      bSuccess = RegQueryValueEx(hk,    /* handle of key to query        */
          "EventMessageFile",   /* value name            */
          NULL,                 /* must be NULL          */
          &dwType,              /* address of type value           */
          szTemp,               /* address of value data */
          &dwcbData);           /* length of value data  */
      PERR(bSuccess == ERROR_SUCCESS, "RegQueryValueEx");

      /* Expand environment variable strings in the message DLL path name,
         in case any are there. */
      cchDest = ExpandEnvironmentStrings(szTemp, szMsgDll, MAX_PATH);
      PERR(cchDest != 0, "ExpandEnvironmentStrings");
      PERR(cchDest < MAX_PATH, "ExpandEnvironmentStrings");

      /* Now we've got the message DLL name, load the DLL. */
      hLib = LoadLibraryEx(szMsgDll, NULL, DONT_RESOLVE_DLL_REFERENCES);
      PERR(hLib != NULL, "LoadLibraryEx");
      if (!hLib)
        {
        puts("Can't find or load message DLL... terminating.");
        puts("Message DLL must be in path or in current directory.");
        exit(1);
        }
      /* prepare the array of insert strings for FormatMessage - the
         insert strings are in the log entry. */
      p = (char *)((LPBYTE) pevlr + pevlr->StringOffset);
      for (i = 0; i < pevlr->NumStrings && i < MAX_INSERT_STRS; i++)
      {
        aInsertStrs[i] = p;
        p += strlen(p) + 1;     /* point to next string */
      }

      /* Format the message from the message DLL with the insert strings */
      bSuccess = FormatMessage(
          FORMAT_MESSAGE_FROM_HMODULE | /* get the message from the DLL */
          FORMAT_MESSAGE_ALLOCATE_BUFFER |      /* allocate the msg buffer
                                                   for us */
          FORMAT_MESSAGE_ARGUMENT_ARRAY |       /* lpArgs is an array of
                                                   pointers */
          60,                   /* line length for the mesages */
          hLib,                 /* the messagetable DLL handle */
          pevlr->EventID,       /* message ID */
          MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), /* language ID */
          (LPTSTR) &msgBuf,     /* address of pointer to buffer for
                                   message */
          MAX_MSG_LENGTH,       /* maximum size of the message buffer */
          aInsertStrs);         /* array of insert strings for the message */
      PERR(bSuccess, "FormatMessage");

      /* mask off the actual message number and show it */
      printf("  Message ID: %d  ", pevlr->EventID & MSG_ID_MASK);
      printf("Message: %s\n", msgBuf);

      /* Free the buffer that FormatMessage allocated for us. */
      LocalFree((HLOCAL) msgBuf);

      /* Subtract the size of the event log record we just read */
      dwRead -= pevlr->Length;
      /* Point to the next event log record in the buffer */
      pevlr = (EVENTLOGRECORD *) ((LPBYTE) pevlr + pevlr->Length);

      /* free the message DLL since we don't know if we'll need it again */
      FreeLibrary(hLib);
      RegCloseKey(hk);
    }
    /* reset our event log record pointer back to the beginning of the
       buffer in preparation for reading the next record. */
    pevlr = (EVENTLOGRECORD *) &bBuffer;
  }
  if (GetLastError()!= ERROR_HANDLE_EOF)
    PERR(bSuccess, "ReadEventLog");
  CloseEventLog(hAppLog);
  RegCloseKey(hk);
  return;
}

int main()
{
  char szMsgPath[MAX_PATH];
  char *aInsertStrs[MAX_INSERT_STRS];   /* array of pointers to insert
                                           strings */

// check if running on Windows NT, if not, display notice and terminate
    if( GetVersion() & 0x80000000 )
    {
       MessageBox( NULL,
         "This sample application can only be run on Windows NT.\n"
         "This application will now terminate.",
         "Logging",
         MB_OK | MB_ICONSTOP | MB_SETFOREGROUND );
      return( 1 );
    }

  /* Set the Event-ID message-file name. We assume that the message DLL is
     in the same directory as this application. */
  GetCurrentDirectory(sizeof(szMsgPath), szMsgPath);
  strcat(szMsgPath, "\\messages.dll");
  addSourceToRegistry("log", szMsgPath);

  /* These commands have no insert strings */

  reportAnEvent( MSG_BAD_COMMAND, 0, NULL );
  reportAnEvent( MSG_BAD_PARM1, 0, NULL );
  reportAnEvent( MSG_STRIKE_ANY_KEY, 0, NULL );

  /* Set up our array of insert strings for error message */
  aInsertStrs[0] = "test.c";
  aInsertStrs[1] = "MYTEST";
  reportAnEvent( MSG_CMD_DELETE, /* the message to log */
      2,                         /* number of insert strings */
      aInsertStrs);              /* the array of insert strings */

  /* Set up our array of insert strings for error message */
  aInsertStrs[0] = "47";
  aInsertStrs[1] = "5";
  reportAnEvent( MSG_STRIKE_ANY_KEY, /* the message to log */
      2,                             /* the number of insert strings */
      aInsertStrs);                  /* the array of insert strings */

  queryEventLog();
  return (0);
}
