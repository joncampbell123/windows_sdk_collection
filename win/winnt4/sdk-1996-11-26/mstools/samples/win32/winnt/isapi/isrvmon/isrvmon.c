/*
  isrvmon.c

  This is a Internet Information Server Application that displays 
  performance information about the Microsoft Internet Information Server

  Exports:

    BOOL WINAPI GetExtensionVersion( HSE_VERSION_INFO *pVer )

    As per the ISAPI Spec, this just returns the
    version of the spec that this server was built with.  This
    function is prototyped in httpext.h

    BOOL WINAPI HttpExtensionProc(   EXTENSION_CONTROL_BLOCK *pECB )

    This function does all of the work.

*/
#include <windows.h>
#include <httpext.h>

#define SYS_INFO "SystemInfo"
#define MEMORY_STATUS "MemoryStatus"
#define PROCESSINFO  "ProcessInfo"
#define GET  "get"

BOOL FillInSystemInfo(char *buff);
BOOL FillInMemoryStatus(char *buff);
BOOL FillInProcessInfo(char *buff);

BOOL WINAPI GetExtensionVersion( HSE_VERSION_INFO *pVer )
 {
  pVer->dwExtensionVersion = MAKELONG( HSE_VERSION_MINOR, HSE_VERSION_MAJOR );

  lstrcpyn( pVer->lpszExtensionDesc,
            "Sample Internet Web Server Application",
            HSE_MAX_EXT_DLL_NAME_LEN );

  return TRUE;
 } // GetExtensionVersion()

DWORD WINAPI HttpExtensionProc( EXTENSION_CONTROL_BLOCK *pECB )
 {

    CHAR  buff[4096];
    CHAR  *lpszQuery;
    CHAR  *lpszTemp=NULL;
    DWORD cbQuery;
    BOOL  Success=FALSE;

    pECB->dwHttpStatusCode=0; // 0 Failure

    //
    //  The Query parameter will contain some or all of the following
    //  strings:
    //
    //  SystemInfo
    //  MemoryStatus
    //  ProcessInfo
    //
    //  We have to parse the string, determine which are passed, and do
    //  the appropriate APIs.  Just for fun we will create the html page
    //  first, and then append the appropriate data as we go along.
    //

    //
    //
    //  Note the HTTP header block is terminated by a blank '\r\n' pair,
    //  followed by the document body
    //

    wsprintf( buff,
             "Content-Type: text/html\r\n"
             "\r\n"
             "<head><title>Sample Web Server Application</title></head>\n"
             "<body><h1>Internet Server Performance Characteristics</h1>\n"
             "<hr>\n");


    //
    //  If the request is a GET, then the lpszQueryString member of the ECB
    //  contains the query string.
    //
    //  If the request is a POST, then you have to get all of the data, both
    //  from the lpbData member, and then read the rest of the data via the
    //  ReadClient() call.
    //

    if (!stricmp(pECB->lpszMethod, GET))
        lpszQuery = pECB->lpszQueryString;
    else 
    {

        if(pECB->cbTotalBytes == 0)     // No Query at all
            goto nodata;
        else
        {
            if(!(lpszTemp = (CHAR *)LocalAlloc(LPTR, pECB->cbTotalBytes)))
                return HSE_STATUS_ERROR;

            strcpy(lpszTemp, pECB->lpbData);

            if((cbQuery = pECB->cbTotalBytes - pECB->cbAvailable) > 0)
                pECB->ReadClient(pECB->ConnID, (LPVOID) (lpszTemp + pECB->cbAvailable), &cbQuery);

            lpszQuery = lpszTemp;
        }
    }

    if (strstr(lpszQuery, SYS_INFO))
        Success |= FillInSystemInfo(buff);

    if (strstr(lpszQuery, MEMORY_STATUS))
        Success |= FillInMemoryStatus(buff);

    if (strstr(lpszQuery, PROCESSINFO))
        Success |= FillInProcessInfo(buff);

nodata:
    if (!Success)
        strcat(buff,"<p>No Data Requested");
     {
      DWORD dwLen=lstrlen(buff);

      if ( !pECB->ServerSupportFunction( pECB->ConnID,
                                         HSE_REQ_SEND_RESPONSE_HEADER,
                                         "200 OK",
                                         &dwLen,
                                         (LPDWORD) buff ))
      {
        return HSE_STATUS_ERROR;
      }
     }

    pECB->dwHttpStatusCode=200; // 200 OK

    if(lpszTemp)
        LocalFree(lpszTemp);

    return HSE_STATUS_SUCCESS;

 } // HttpExtensionProc()

CHAR *lpszArchitecture[]={"INTEL", "MIPS", "ALPHA", "PPC"};

BOOL FillInSystemInfo(CHAR *buff)
{
    SYSTEM_INFO si;
    CHAR        tmpbuf[1024];

    GetSystemInfo(&si);

//
// Need to actually handle processor level.  
// Just print it for now as a number
//
    wsprintf(tmpbuf,
             "<h2>System Information</h2>\n"
             "<pre>Type Of Machine:      %s</pre>\n"
             "<pre>Level Of Processor:   %d</pre>\n"
             "<pre>Number Of Processors: %d</pre>\n"
             "<hr>\n",
              lpszArchitecture[si.wProcessorArchitecture],
              si.wProcessorLevel,
              si.dwNumberOfProcessors);

    buff = strcat(buff,tmpbuf);
    
    return TRUE;
} // FillInSystemInfo

BOOL FillInMemoryStatus(CHAR *buff)
{
    MEMORYSTATUS ms;
    CHAR         tmpbuf[1024];

    ms.dwLength = sizeof(MEMORYSTATUS);

    GlobalMemoryStatus(&ms);

    wsprintf(tmpbuf,
             "<h2>Global Memory Status</h2>\n"
             "<pre>Memory Load:               %d%%\n</pre>\n"
             "<pre>Total Physical Memory:     %d MB</pre>\n"
             "<pre>Available Physical Memory: %d MB\n</pre>\n"
             "<pre>Total Page File:           %d MB</pre>\n"
             "<pre>Available Page File:       %d MB\n</pre>\n"
             "<pre>Total Virtual Memory:      %d MB</pre>\n"
             "<pre>Available Virtual Memory:  %d MB</pre>\n"
             "<hr>\n",
             ms.dwMemoryLoad,
             ms.dwTotalPhys/1024/1024,
             ms.dwAvailPhys/1024/1024,
             ms.dwTotalPageFile/1024/1024,
             ms.dwAvailPageFile/1024/1024,
             ms.dwTotalVirtual/1024/1024,
             ms.dwAvailVirtual/1024/1024);
             
    buff = strcat(buff,tmpbuf);

    return TRUE;
} // FillInMemoryStatus

BOOL FillInProcessInfo(CHAR *buff)
{
    LONGLONG    ftCreationTime;
    LONGLONG    ftExitTime;
    LONGLONG    ftKernelTime;
    LONGLONG    ftUserTime;
    LONGLONG    ftCurrentTime;
    LONGLONG    ftElapsedTime;
    SYSTEMTIME  stCurrentTime;
    CHAR        tmpbuf[1024];
    
    buff = strcat(buff,"<h2>Process Information</h2>\n");

    if(!GetProcessTimes(GetCurrentProcess(),
                        (FILETIME *) &ftCreationTime,
                        (FILETIME *) &ftExitTime,
                        (FILETIME *) &ftKernelTime,
                        (FILETIME *) &ftUserTime))
    {
        buff = strcat(buff,"<p>No Process Information Available\n<hr>\n");
        return TRUE;
    }
                            

    GetSystemTime(&stCurrentTime);
    SystemTimeToFileTime(&stCurrentTime,(FILETIME *) &ftCurrentTime);
    ftElapsedTime = (ftCurrentTime - ftCreationTime)/10000;
    ftKernelTime = ftKernelTime/10000;
    ftUserTime = ftUserTime/10000;

    wsprintf(tmpbuf,
             "<pre>ElapsedTime:   %d Day(s)</pre>\n"
             "<pre>               %d Hr</pre>\n"
             "<pre>               %d Min</pre>\n"
             "<pre>               %d Sec</pre>\n"
             "<pre>               %d mSec\n</pre>\n" 
             "<pre>KernelTime:    %d Min</pre>\n"
             "<pre>               %d Sec</pre>\n"
             "<pre>               %d mSec\n</pre>\n" 
             "<pre>UserTime:      %d Min</pre>\n"
             "<pre>               %d Sec</pre>\n"
             "<pre>               %d mSec</pre>\n"
             "<hr>\n",
             (DWORD) (ftElapsedTime/86400000),
             (DWORD) (ftElapsedTime%86400000/3600000),
             (DWORD) (ftElapsedTime%86400000%3600000/60000),
             (DWORD) (ftElapsedTime%86400000%3600000%60000/1000),
             (DWORD) (ftElapsedTime%86400000%3600000%60000%1000),
             (DWORD) (ftKernelTime/60000),
             (DWORD) (ftKernelTime%60000/1000),
             (DWORD) (ftKernelTime%60000%1000),
             (DWORD) (ftUserTime/60000),
             (DWORD) (ftUserTime%60000/1000),
             (DWORD) (ftUserTime%60000%1000));
             
            
    buff = strcat(buff,tmpbuf);
    return TRUE;
} // FillInProcessInfo
