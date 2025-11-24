/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

#include <windows.h>
#include <winspool.h>
#include <offsets.h>
#include <winsplp.h>
#include <regstr.h>

#include "spltypes.h"
#include "local.h"
#include "localmon.h"
#include "dialogs.h"

HANDLE hInst = NULL;

TCHAR szFILE[]    = TEXT("FILE:");
TCHAR szColon[]   = TEXT(":");
#define IS_FILE_PORT(pName)     !wcsicmp( pName, szFILE )

LPTSTR GetPortName(HWND hWnd);

PINIMONPORT pIniFirstPort = NULL;
CRITICAL_SECTION MonitorSection = {0, 0, 0, 0, 0, 0};
HANDLE hEventNetEnum = NULL;
DWORD dwTimeLastNetEnum = 0;
BOOL bNetInstalled = FALSE;
BOOL bNetEnumThreadRunning = FALSE;

#define NET_ENUM_RETRY_INTERVAL 10000   // 10 seconds
#define NET_ENUM_TIMEOUT        3000    // 3 seconds

#define OUT_QUEUE_SIZE  4096    // 4k out queue
#define WRITE_TOTAL_TIMEOUT     3000    // 3 seconds
#define READ_TOTAL_TIMEOUT      5000    // 5 seconds
#define READ_INTERVAL_TIMEOUT   200     // 0.1 second
#define DEF_DNS_TIMEOUT 15000   // 15 seconds
#define DEF_TX_TIMEOUT  45000   // 45 seconds


// ---------------------------------------------------------------------------
// EnterMonSem
//
// Enter localmon's internal critical section.
//
// ---------------------------------------------------------------------------
VOID
EnterMonSem(
   VOID
)
{
    EnterCriticalSection(&MonitorSection);
}


// ---------------------------------------------------------------------------
// LeaveMonSem
//
// Leave localmon's internal critical section.
//
// ---------------------------------------------------------------------------
VOID
LeaveMonSem(
   VOID
)
{
    LeaveCriticalSection(&MonitorSection);
}


// ---------------------------------------------------------------------------
// CreateMonitorPortEntry
//
// Create a new port data structure and link into the list.
//
// ---------------------------------------------------------------------------
PINIMONPORT
CreateMonitorPortEntry(
    LPTSTR  pPortName,
    LPTSTR  pDeviceDesc
)
{
    DWORD       cb;
    PINIMONPORT    pIniPort, pPort;

    cb = sizeof(INIMONPORT) + wcslen(pPortName)*sizeof(TCHAR) + sizeof(TCHAR);

    EnterMonSem();

    pIniPort = AllocSplMem(cb);

    LeaveMonSem();

    if( pIniPort )
    {
        pIniPort->pName = wcscpy((LPTSTR)(pIniPort+1), pPortName);
        pIniPort->cb = cb;
        pIniPort->pNext = 0;
        pIniPort->signature = IPO_SIGNATURE;

        // pDeviceDesc could be NULL only if it's PP_REMOTE

        if (pDeviceDesc)
        {
            if (*pDeviceDesc)
                pIniPort->pDeviceDesc = AllocSplStr(pDeviceDesc);
            else
                pIniPort->pDeviceDesc = AllocSplStr(pPortName);
        }

        // set the port type status bit
        if (IsCom(pPortName))
            pIniPort->Status = PP_COM;
        else if (IsLpt(pPortName))
            pIniPort->Status = PP_LPT;
        else if (IS_FILE_PORT(pPortName))
            pIniPort->Status = PP_FILE;

#ifdef USE_OVERLAPPED_IO
        if (pIniPort->Status & PP_LPT || pIniPort->Status & PP_COM)
        {
            // manual reset
            pIniPort->ahEvents[0] =
            pIniPort->ovEvent.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            pIniPort->ahEvents[1] =
            pIniPort->ovWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        }
#endif

        if (pPort = pIniFirstPort)
        {
            while (pPort->pNext)
                pPort = pPort->pNext;

            pPort->pNext = pIniPort;
        }
        else
            pIniFirstPort = pIniPort;
    }

    return pIniPort;
}


// ---------------------------------------------------------------------------
// DeleteMonitorPortEntry
//
// Free a port data structure that is no longer needed.
//
// ---------------------------------------------------------------------------
BOOL
DeleteMonitorPortEntry(
    LPTSTR   pPortName
)
{
    DWORD       cb;
    PINIMONPORT    pPort, pPrevPort;

    pPort = pIniFirstPort;
    while (pPort && wcsicmp(pPort->pName, pPortName))
    {
        pPrevPort = pPort;
        pPort = pPort->pNext;
    }

    if (pPort)
    {
        if (pPort->Status & PP_HARDWARE)
        {
            SetLastError(ERROR_INVALID_PRINTER_COMMAND);
            return FALSE;
        }

        cb = sizeof(INIMONPORT) + wcslen(pPortName)*sizeof(TCHAR) + sizeof(TCHAR);

        if (pPort == pIniFirstPort)
        {
            pIniFirstPort = pPort->pNext;
        }
        else
        {
            pPrevPort->pNext = pPort->pNext;
        }

#ifdef USE_OVERLAPPED_IO
        CloseHandle(pPort->ovEvent.hEvent);
        CloseHandle(pPort->ovWrite.hEvent);
#endif

        FreeSplStr(pPort->pDeviceDesc);
        FreeSplMem(pPort, cb);

        return TRUE;
    }

    SetLastError(ERROR_UNKNOWN_PORT);
    return FALSE;
}


// ---------------------------------------------------------------------------
// EnumPhysicalPorts
//
// We enumerate the dynamic registry keys to search devnodes that are in
// "port" or "modem" class to fine the currently physically present ports.
//
// ---------------------------------------------------------------------------
TCHAR szDriverDesc[]    = TEXT(REGSTR_VAL_DRVDESC);
TCHAR szPortName[]      = TEXT(REGSTR_VAL_PORTNAME);
TCHAR szModem[]         = TEXT("Modem");
TCHAR szEnumSlash[]     = TEXT("Enum\\");
TCHAR szClassSlash[]    = TEXT("System\\CurrentControlSet\\Services\\Class\\");
TCHAR szDynaEnum[]      = TEXT(REGSTR_PATH_DYNA_ENUM);
TCHAR szHWKey[]         = TEXT(REGSTR_VAL_HARDWARE_KEY);
TCHAR szClass[]         = TEXT(REGSTR_KEY_CLASS);
#define	LINE_LEN	100

VOID
EnumPhysicalPorts()
{
    HKEY hkEnum;
    HKEY hkRoot;
    HKEY hkDev;
    HKEY hkDriver;
    char szKey[LINE_LEN];
    char szHardware[LINE_LEN];
    char szValue[LINE_LEN];
    char szDriver[LINE_LEN];
    LPSTR pDriver;
    LPSTR pHardware;
    PINIMONPORT pIniPort;

    if (!RegOpenKey(HKEY_DYN_DATA, szDynaEnum, &hkEnum))
    {
        // Enumerate the Dynamic\Enum key

        int iEnum = 0;

        lstrcpy(szHardware, szEnumSlash);
        pHardware = (LPSTR)szHardware + sizeof(szEnumSlash) - 1;

        lstrcpy(szDriver, szClassSlash);
        pDriver = (LPSTR)szDriver + sizeof(szClassSlash) - 1;

        while (!RegEnumKey(hkEnum, iEnum++, szKey, sizeof(szKey)))
        {
            if (!RegOpenKey(hkEnum, szKey, &hkRoot))
            {
	            DWORD cbSize = sizeof(szHardware) - sizeof(szEnumSlash);

	            if (!RegQueryValueEx(hkRoot, szHWKey, NULL, NULL, pHardware, &cbSize))
                {
                    // pHareware is in the format "ROOT\*PNP0500\0000"
                    // szHareware is in the format "Enum\ROOT\*PNP0500\0000"

                    if (!RegOpenKey(HKEY_LOCAL_MACHINE, szHardware, &hkDev))
                    {
	                    cbSize = sizeof(szValue);

	                    if (!RegQueryValueEx(hkDev, szClass, NULL,
		                    NULL, szValue, &cbSize))
                        {
    		                // Is the device class "ports" or "modem"?

	    	                if (!lstrcmpi(szPorts, szValue) ||
	    	                    !lstrcmpi(szModem, szValue))
                            {
        		                // Yes, get the portname

		                        cbSize = sizeof(szKey);

		                        if (!RegQueryValueEx(hkDev, szPortName, NULL,
			                        NULL, szKey, &cbSize))
                                {
                                    // szKey is the portname WITHOUT a colon

                                    lstrcat(szKey, szColon);

                                    if (!FindPortFromList((PINIENTRY)pIniFirstPort, szKey))
                                    {
                                        szValue[0] = 0;

                                        // Get the DriverDesc from
                                        // HKLM\system\currentcontrolset\services\class\"driver"

		                                cbSize = sizeof(szDriver) - lstrlen(szDriver);
        		                        if (!RegQueryValueEx(hkDev, szDriverFile, NULL,
		        	                        NULL, pDriver, &cbSize))
                                        {
                                            // szDriver is the driver key name

                                            if (!RegOpenKey(HKEY_LOCAL_MACHINE, szDriver, &hkDriver))
                                            {
		                                        cbSize = sizeof(szValue);
        		                                RegQueryValueEx(hkDriver, szDriverDesc, NULL,
		        	                                NULL, szValue, &cbSize);

                                                RegCloseKey(hkDriver);
                                            }

                                            *pDriver = 0;
                                        }

                                        // szValue is the friendly portname

                                        if (pIniPort = CreateMonitorPortEntry(szKey, szValue))
                                            pIniPort->Status |= PP_HARDWARE;
                                    }
                                }
                            }
                        }
                        RegCloseKey(hkDev);
                    }
                }
                RegCloseKey(hkRoot);
            }
        }
        RegCloseKey(hkEnum);
    }
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
VOID
EnumWinIniPorts()
{
    LPTSTR   pPorts, pFirstPort;
    DWORD i;
    char szKey[LINE_LEN];

    i = 4096;
    if (!(pPorts = LocalAlloc(LPTR, i)))
    {
        DBGMSG(DBG_ERROR, ("LocalAlloc failed in LocalmonInitializeMonitor.\n"));
        return;
    }

    GetProfileString(szPorts, NULL, szNull, pPorts, i);

    // Remember the beginning so we can free it later
    pFirstPort = pPorts;

    // We now have all the ports
    szKey[0] = 0;
    LoadString(hInst, IDS_UNKNOWN_LOCAL_PORT, szKey, sizeof(szKey));

    while (*pPorts)
    {
        // !!! ??? ---------------------------------------------------
        // Create a port entry if the port name doesn't have one of the
        // "\\" or "LPTx" or "COMx" or "FILE:"
        // -----------------------------------------------------------
        if ((*((LPWORD)pPorts) != 0x5c5c) &&
            !IsCom(pPorts) &&
            !IsLpt(pPorts) &&
            !IS_FILE_PORT(pPorts) &&
            !FindPortFromList((PINIENTRY)pIniFirstPort, pPorts))
        {
            CreateMonitorPortEntry(pPorts, szKey);
        }
        pPorts += lstrlen(pPorts) + 1;
    }

    LocalFree(pFirstPort);
}


// ---------------------------------------------------------------------------
// InitializeMonitorEx
//                                             
// This is the only real entry point that shoudl be exported out of this
// monitor dll. The rest of the entry points are exposed to the caller via
// this routine which initializes the local monitor. 
//
// ---------------------------------------------------------------------------
BOOL
WINAPI
InitializeMonitorEx(LPTSTR pRegPath, LPMONITOR lpMonitor)
{
    DWORD i;
    char szKey[LINE_LEN];
    HANDLE hEnum;
    PINIMONPORT pIniPort;

    InitializeCriticalSection(&MonitorSection);

   EnterMonSem();

    EnumPhysicalPorts();

    // special case FILE: port
    szKey[0] = 0;
    LoadString(hInst, IDS_FILE_PORT, szKey, sizeof(szKey));
    if (pIniPort = CreateMonitorPortEntry((LPSTR)szFILE, szKey))
        pIniPort->Status |= PP_HARDWARE;

    EnumWinIniPorts();

    // initialize the dispatch table

    lpMonitor->pfnEnumPorts = LocalmonEnumPorts;
    lpMonitor->pfnOpenPort  = LocalmonOpenPort;
    lpMonitor->pfnStartDocPort = LocalmonStartDocPort;
    lpMonitor->pfnWritePort = LocalmonWritePort;
    lpMonitor->pfnReadPort  = LocalmonReadPort;
    lpMonitor->pfnEndDocPort= LocalmonEndDocPort;
    lpMonitor->pfnClosePort = LocalmonClosePort;
    lpMonitor->pfnAddPort   = LocalmonAddPort;
    lpMonitor->pfnConfigurePort = LocalmonConfigurePort;
    lpMonitor->pfnDeletePort= LocalmonDeletePort;
    lpMonitor->pfnOpenPortEx = NULL;
    lpMonitor->pfnGetPrinterDataFromPort = LocalmonGetPrinterDataFromPort;
    lpMonitor->pfnSetPortTimeOuts = LocalmonSetPortTimeOuts;

    bNetInstalled = TRUE;

    if (i = (WNetOpenEnum(RESOURCE_GLOBALNET, 0, 0, NULL, &hEnum) == NO_ERROR))
    {
        WNetCloseEnum(hEnum);
    }
    else if (i == ERROR_NO_NETWORK)
        bNetInstalled = FALSE;

   LeaveMonSem();

    return TRUE;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
DWORD
GetPortSize(
    PINIMONPORT pIniPort,
    DWORD   Level
)
{
    DWORD   cb;
    LPSTR pDeviceDesc;

    switch (Level)
    {
    case 1:
        cb=sizeof(PORT_INFO_1) +
            wcslen(pIniPort->pName)*sizeof(TCHAR) + sizeof(TCHAR);
        break;

    case 2:
        pDeviceDesc = (pIniPort->Status & PP_REMOTE) ?
            pIniPort->pRemoteName : pIniPort->pDeviceDesc;

        cb=sizeof(PORT_INFO_2) +
            wcslen(pIniPort->pName)*sizeof(TCHAR) + sizeof(TCHAR) +
            wcslen(pDeviceDesc)*sizeof(TCHAR) + sizeof(TCHAR) +
            wcslen(szLocalPort)*sizeof(TCHAR) + sizeof(TCHAR);

        break;

    default:
        cb = 0;
        break;
    }

    return cb;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
LPBYTE
CopyIniPortToPort(
    PINIMONPORT pIniPort,
    DWORD   Level,
    LPBYTE  pPortInfo,
    LPBYTE   pEnd
)
{
    LPTSTR   SourceStrings[sizeof(PORT_INFO_2)/sizeof(LPTSTR)];
    LPTSTR   *pSourceStrings=SourceStrings;
    DWORD    *pOffsets;

    switch (Level)
    {
    case 1:
        pOffsets = PortInfo1Strings;
        break;

    case 2:
        pOffsets = PortInfo2Strings;
        break;

    default:
        return pEnd;
    }

    switch (Level)
    {
    case 1:
        *pSourceStrings++=pIniPort->pName;
        pEnd = PackStrings(SourceStrings, pPortInfo, pOffsets, pEnd);
        break;

    case 2:
        *pSourceStrings++=pIniPort->pName;
        *pSourceStrings++=szLocalPort;

        // check if it's a redirected LPT port

        *pSourceStrings++ = (pIniPort->Status & PP_REMOTE) ?
            pIniPort->pRemoteName : pIniPort->pDeviceDesc;

        ((PPORT_INFO_2)pPortInfo)->fPortType = PORT_TYPE_WRITE;

        if (pIniPort->Status & PP_REMOTE)
            ((PPORT_INFO_2)pPortInfo)->fPortType |= PORT_TYPE_REDIRECTED;
        else
            if (pIniPort->Status & PP_LPT)
                ((PPORT_INFO_2)pPortInfo)->fPortType |= PORT_TYPE_READ;

        ((PPORT_INFO_2)pPortInfo)->Reserved = 0;


        pEnd = PackStrings(SourceStrings, pPortInfo, pOffsets, pEnd);
        break;
    }

    return pEnd;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
VOID
RemoveNetworkPrintResource()
{
    PINIMONPORT    pPort, pThisPort;

    pPort = pIniFirstPort;
    while (pPort)
    {
        // it's a redirected port

        pThisPort = pPort;
        pPort = pPort->pNext;

        if (pThisPort->Status & PP_REMOTE)
        {
            if (pThisPort->Status & PP_CURRENT)
                pThisPort->Status &= ~PP_CURRENT;
            else
            {
                // this is a old net connection that does not exist anymore

                FreeSplStr(pThisPort->pRemoteName);

                if (pThisPort->Status & PP_HARDWARE)
                {
                    // there's physical hardware for this port, don't delete the entry

                    pThisPort->Status &= ~PP_REMOTE;
                }
                else
                {
                    DeleteMonitorPortEntry(pThisPort->pName);
                }
            }
        }
    }
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
#define NETBUFFERSIZE 4096

DWORD
WINAPI
EnumNetworkPrintResource(
    DWORD dwParam
)
{
    HANDLE        hEnum;
    LPNETRESOURCE lpNR;
    LPNETRESOURCE lpRes;
    PINIMONPORT   pIniPort;
    char szBuf[LINE_LEN];

    if (WNetOpenEnum(RESOURCE_CONNECTED, RESOURCETYPE_PRINT, 0, NULL, &hEnum) ==
        NO_ERROR)
    {
        if (lpNR = (LPNETRESOURCE)LocalAlloc(LPTR, NETBUFFERSIZE))
        {
            while (1)
            {
                DWORD dwSize = NETBUFFERSIZE;
                DWORD dwEntries = (DWORD)-1L;
                DWORD dwResult;

                dwResult = WNetEnumResource(hEnum, &dwEntries,
                    (LPBYTE)lpNR, &dwSize);

                if ((NO_ERROR == dwResult) || (ERROR_MORE_DATA == dwResult))
                {
                    DWORD dwLoop;

                    for (dwLoop = 0, lpRes = lpNR;
                         dwLoop < dwEntries;
                         dwLoop++, lpRes++)
                    {
                        if (lpRes->lpLocalName)
                        {
                            // here's the real work
                            // create a port entry if there isn't one

                           EnterMonSem();

                            if (!(pIniPort =
                                (PINIMONPORT)FindPortFromList((PINIENTRY)pIniFirstPort, lpRes->lpLocalName)))
                            {
                                lstrcpy(szBuf, lpRes->lpLocalName);
                                lstrcat(szBuf, szColon);
                                pIniPort = CreateMonitorPortEntry(
                                    szBuf, NULL);
                            }

                            pIniPort->pRemoteName = AllocSplStr(lpRes->lpRemoteName);
                            pIniPort->Status |= PP_REMOTE | PP_CURRENT;

                           LeaveMonSem();
                        }
                    }

                    if (NO_ERROR == dwResult)
                        break;
                }
                else
                    break;
            }

            LocalFree(lpNR);
        }

        WNetCloseEnum(hEnum);
    }

   EnterMonSem();

    RemoveNetworkPrintResource();

    bNetEnumThreadRunning = 0;

    if (hEventNetEnum)
        SetEvent(hEventNetEnum);

   LeaveMonSem();

    return 0;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
BOOL
WINAPI
LocalmonEnumPorts(
    LPTSTR   pName,
    DWORD   Level,
    LPBYTE  pPorts,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
)
{
    PINIMONPORT pIniPort;
    DWORD   cb;
    LPBYTE  pEnd;
    DWORD   LastError=0;

    if (Level > 2)
    {
        SetLastError(ERROR_INVALID_LEVEL);
        return FALSE;
    }

   EnterMonSem();

    cb = GetTickCount();

    if (bNetInstalled)
    {
#if 0
        RemoveNetworkPrintResource();
        EnumNetworkPrintResource(0);
#else
        if (((cb - dwTimeLastNetEnum) > NET_ENUM_RETRY_INTERVAL) &&
            !bNetEnumThreadRunning &&
            (hEventNetEnum = CreateEvent(NULL, FALSE, FALSE, NULL)))
        {
            HANDLE  ThreadHandle;
            DWORD   ThreadId;

            dwTimeLastNetEnum = cb;

            if (ThreadHandle = CreateThread(NULL, 16*1024,
                            (LPTHREAD_START_ROUTINE)EnumNetworkPrintResource,
                            0, 0, &ThreadId))
            {
                bNetEnumThreadRunning++;
                CloseHandle(ThreadHandle);

                // use this opportinuty to re enumerate physical ports
                // to check if there has been any new ports arrived

                EnumPhysicalPorts();
                EnumWinIniPorts();

               LeaveMonSem();

                if (WaitForSingleObject(hEventNetEnum, NET_ENUM_TIMEOUT) == WAIT_TIMEOUT)
                {
                    // WNetEnum is taking too long, we'll punt

                    DBGMSG(DBG_ERROR, ("localmon!EnumNetworkPrintResource Timeout\n"));
                }

               EnterMonSem();

                CloseHandle(hEventNetEnum);
                hEventNetEnum = NULL;
            }
            else
            {
                // failed to create the enum thread
                CloseHandle(hEventNetEnum);
                EnumNetworkPrintResource(0);
            }
        }
#endif
    }
    else
    {
        if ((cb - dwTimeLastNetEnum) > NET_ENUM_RETRY_INTERVAL)
        {
            dwTimeLastNetEnum = cb;

            // use this opportinuty to re enumerate physical ports
            // to check if there has been any new ports arrived

            EnumPhysicalPorts();
            EnumWinIniPorts();
        }
    }

    cb=0;
    pIniPort=pIniFirstPort;

    while (pIniPort)
    {
        cb+=GetPortSize(pIniPort, Level);
        pIniPort=pIniPort->pNext;
    }

    *pcbNeeded=cb;

    if (cb <= cbBuf)
    {
        pEnd=pPorts+cbBuf;
        *pcReturned=0;

        pIniPort=pIniFirstPort;
        while (pIniPort)
        {
            pEnd = CopyIniPortToPort(pIniPort, Level, pPorts, pEnd);
            switch (Level)
            {
            case 1:
                pPorts+=sizeof(PORT_INFO_1);
                break;

            case 2:
                pPorts+=sizeof(PORT_INFO_2);
                break;
            }
            pIniPort=pIniPort->pNext;
            (*pcReturned)++;
        }
    }
    else
        LastError = ERROR_INSUFFICIENT_BUFFER;

   LeaveMonSem();

    if (LastError)
    {
        SetLastError(LastError);
        return FALSE;
    }

    return TRUE;
}


// ---------------------------------------------------------------------------
// GetIniCommValues
//
// It goes to win.ini [ports] section to get the comm (serial) port
// settings such as
//
// com1:=9600,n,8,1
//
// And build a DCB.
//
// ---------------------------------------------------------------------------
BOOL
GetIniCommValues(
    LPTSTR          pName,
    LPDCB          pdcb
)
{
    COMMCONFIG ccDummy;
    COMMCONFIG *pcc;
    DWORD dwSize;
    char buf[MAX_PATH];
    int len = lstrlen(pName) - 1;
    BOOL ret = FALSE;

    lstrcpy(buf, pName);
    if (buf[len] == ':')
        buf[len] = 0;

    ccDummy.dwProviderSubType = PST_RS232;
    dwSize = sizeof(ccDummy);
    GetDefaultCommConfig(buf, &ccDummy, &dwSize);
    if (pcc = LocalAlloc(LPTR, dwSize))
    {
        pcc->dwProviderSubType = PST_RS232;
        if (GetDefaultCommConfig(buf, pcc, &dwSize))
        {
            *pdcb = pcc->dcb;
            ret = TRUE;
        }
        LocalFree(pcc);
    }

    // this is here just in case GetDefaultCommConfig doesn't work
    // remove after M6
    if (!ret)
    {
        TCHAR IniEntry[20];

        *IniEntry = TEXT('\0');

        GetProfileString( szPorts, pName, szNull, IniEntry, sizeof IniEntry );

        BuildCommDCB((LPTSTR)IniEntry, pdcb);

        ret = TRUE;
    }

    return ret;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
BOOL
WINAPI
LocalmonOpenPort(
    LPTSTR   pName,
    LPHANDLE pHandle
)
{
    PINIMONPORT     pIniPort;

   EnterMonSem();

    pIniPort = (PINIMONPORT)FindPortFromList((PINIENTRY)pIniFirstPort, pName);

    if (pIniPort)
    {
        *pHandle = pIniPort;

       LeaveMonSem();
        return TRUE;
    }
    else
    {
       DBGMSG(DBG_ERROR, ("localmon!OpenPort %s : Failed\n", pName));

       LeaveMonSem();
        return FALSE;
    }
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
int myatoi(LPSTR pch)
{
    int i=0;
    while (*pch >= '0' && *pch <= '9')
        i = i*10 + *pch++ - '0';

    return i;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
VOID
GetIniTimeouts(
    LPSTR pPrinter,
    PINIMONPORT pIniPort
)
{
    char buf[sizeof(PRINTER_INFO_5) + MAX_PATH + MAX_PATH];
    DWORD dwNeeded;
    PPRINTER_INFO_5 pPrinter5 = (PPRINTER_INFO_5)buf;

    if (!OpenPrinter(pPrinter, &(pIniPort->hPrinter), NULL))
        pIniPort->hPrinter = NULL;

    if (pIniPort->hPrinter &&
        GetPrinter(pIniPort->hPrinter, 5, (LPBYTE)buf, sizeof(buf), &dwNeeded))
    {
        pIniPort->dnsTimeout = pPrinter5->DeviceNotSelectedTimeout;
        pIniPort->txTimeout = pPrinter5->TransmissionRetryTimeout;
    }
    else
    {
        pIniPort->dnsTimeout = DEF_DNS_TIMEOUT;
        pIniPort->txTimeout = DEF_TX_TIMEOUT;
    }
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
BOOL
WINAPI
LocalmonStartDocPort(
    HANDLE  hPort,
    LPTSTR  pPrinterName,
    DWORD   JobId,
    DWORD   Level,
    LPBYTE  pDocInfo
)
{
    PINIMONPORT    pIniPort = (PINIMONPORT)hPort;
    char        pPortName[MAX_PATH];
    int         UserSuppliedFileName = 0;
    DCB          dcb;
    COMMTIMEOUTS cto;
    PDOC_INFO_2 pDocInfo2 = (PDOC_INFO_2)pDocInfo;

    UNREFERENCED_PARAMETER( Level );

    // Serialize access to the port
    if (pIniPort->Status & PP_INSTARTDOC)
    {
        SetLastError(ERROR_BUSY);
        return FALSE;
    }

    pIniPort->JobId = JobId;

    // remove trailing colon
    if (pDocInfo2->pOutputFile)
        lstrcpy(pPortName, pDocInfo2->pOutputFile);
    else
        lstrcpy(pPortName, pIniPort->pName);

    if (pPortName[lstrlen(pPortName) - 1] == ':')
    {
        pPortName[lstrlen(pPortName) - 1] = 0;
    }

    if (pDocInfo2->pOutputFile)
        goto IsFileName;

    if (pIniPort->Status & PP_LPT || pIniPort->Status & PP_COM)
    {
#ifdef USE_OVERLAPPED_IO
        pIniPort->hFile = CreateFile(pPortName, GENERIC_WRITE | GENERIC_READ,
                                     0 /*FILE_SHARE_READ*/, NULL, OPEN_ALWAYS,
                                     FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                                     NULL);
#else
        pIniPort->hFile = CreateFile(pPortName, GENERIC_WRITE | GENERIC_READ,
                                     0 /*FILE_SHARE_READ*/, NULL, OPEN_ALWAYS,
                                     FILE_ATTRIBUTE_NORMAL, NULL);
#endif

        if (pIniPort->hFile == INVALID_HANDLE_VALUE)
        {
            DBGMSG(DBG_ERROR, ("LocalmonStartDocPort: CreateFile(%s) FAILED %d\n", pPortName, GetLastError()));
            DBGBREAK();
            goto ErrorExit;
        }

#ifdef USE_OVERLAPPED_IO
        // use 4k out buffer
#ifdef _SETUP_COMM
        SetupComm(pIniPort->hFile, 0, OUT_QUEUE_SIZE);
#else
        pIniPort->pOutBuffer = GlobalAlloc(GMEM_FIXED, OUT_QUEUE_SIZE);
#endif
        SetCommMask(pIniPort->hFile, EV_ERR | EV_PERR | EV_TXEMPTY);
#endif

        SetEndOfFile(pIniPort->hFile);

        GetIniTimeouts(pPrinterName, pIniPort);
    }
    else
    {
        if (Level == 2 && pDocInfo2->dwMode == DI_CHANNEL)
        {
            SetLastError(ERROR_CANTREAD);
            goto ErrorExit;
        }
    }

    if (pIniPort->Status & PP_COM)
    {
        if (GetCommState (pIniPort->hFile, &dcb))
        {
            GetCommTimeouts(pIniPort->hFile, &cto);
            GetIniCommValues(pIniPort->pName, &dcb);
            cto.WriteTotalTimeoutConstant = WRITE_TOTAL_TIMEOUT;
            cto.ReadTotalTimeoutConstant = READ_TOTAL_TIMEOUT;
            cto.ReadIntervalTimeout = READ_INTERVAL_TIMEOUT;

            SetCommState(pIniPort->hFile, &dcb);
            SetCommTimeouts(pIniPort->hFile, &cto);
        }
        else
            DBGMSG( DBG_ERROR, ("ERROR: Failed to set Comm State\n") );
    }
    else if (pIniPort->Status & PP_LPT)
    {
        if (GetCommTimeouts(pIniPort->hFile, &cto))
        {
            cto.WriteTotalTimeoutConstant = WRITE_TOTAL_TIMEOUT;
            cto.ReadTotalTimeoutConstant = READ_TOTAL_TIMEOUT;
            cto.ReadIntervalTimeout = READ_INTERVAL_TIMEOUT;
            SetCommTimeouts(pIniPort->hFile, &cto);
        }
        else
            DBGMSG( DBG_ERROR, ("ERROR: Failed to set Comm State\n") );
    }
    else if (pIniPort->Status & PP_FILE)
    {
        TCHAR FileName[MAX_PATH];

        UserSuppliedFileName = DialogBoxParam( hInst,
                        MAKEINTRESOURCE( DLG_PRINTTOFILE ),
                        NULL, (DLGPROC)PrintToFileDlg,
                        (LPARAM)FileName );

        if( !UserSuppliedFileName || ( UserSuppliedFileName == -1 ) )
            return FALSE;

        pIniPort->hFile = CreateFile(FileName, GENERIC_WRITE,
                                        FILE_SHARE_READ, NULL, OPEN_ALWAYS,
                                        FILE_ATTRIBUTE_NORMAL, NULL);
        SetEndOfFile(pIniPort->hFile);

    }
    else // if pPortName is actually a filename
    {
IsFileName:
        pIniPort->hFile = CreateFile(pPortName, GENERIC_WRITE,
                                FILE_SHARE_READ, NULL, OPEN_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL, NULL);
    }

    if (pIniPort->hFile == INVALID_HANDLE_VALUE)
    {
ErrorExit:
        DBGMSG(DBG_ERROR, ("StartDocPort FAILED %d\n", GetLastError()));
        return FALSE;
    }
    else
    {
        pIniPort->Status |= PP_INSTARTDOC | PP_FIRSTWRITE;
        return TRUE;
    }
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
BOOL
WINAPI
LocalmonReadPort(
    HANDLE hPort,
    LPBYTE pBuffer,
    DWORD  cbBuf,
    LPDWORD pcbRead
)
{
    PINIMONPORT    pIniPort = (PINIMONPORT)hPort;
    BOOL    rc;

    rc = ReadFile(pIniPort->hFile, pBuffer, cbBuf, pcbRead, NULL);

    return rc;
}


// ---------------------------------------------------------------------------
// TransmitTimeout
//
// Returns TRUE if timeour occurs.
//
// ---------------------------------------------------------------------------
BOOL
TransmitTimeout(
    PINIMONPORT pIniPort,
    DWORD timeout
)
{
    DWORD CurTime;

    // 0 timeout meaning infinite retry

    if (!timeout)
        return FALSE;

    CurTime = GetTickCount();

#ifdef USE_OVERLAPPED_IO
    if (pIniPort->dwRetry)
    {
        if ((CurTime - pIniPort->dwRetry) >= timeout)
        {
            // timeout !!!

            pIniPort->dwRetry = 0;
	        return TRUE;
        }
    }
    else
    {
        pIniPort->dwRetry = CurTime;
    }
#else
    if (!pIniPort->dwRetry)
    {
        // we've already tried once

        pIniPort->dwRetry = CurTime - WRITE_TOTAL_TIMEOUT;
    }

    if ((CurTime - pIniPort->dwRetry) >= timeout)
    {
        // timeout !!!

        pIniPort->dwRetry = 0;
	    return TRUE;
    }
#endif

    return FALSE;	// everything is ok
}


#ifdef USE_OVERLAPPED_IO
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
BOOL
WaitForCompleteOrError(
    PINIMONPORT pIniPort
)
{
    DWORD dwTransfer;
    DWORD dwObject;
    LPOVERLAPPED pov;
    DWORD LastError = 0;
    BOOL rc;

    // !!! ???
    // we have to do some of the following thing in EndDocPort as well
    // to make sure the very last write succeeded.

    if (pIniPort->Status & PP_FIRSTWRITE)
        pIniPort->Status &= ~PP_FIRSTWRITE;
    else
    {
        // check if we had a timeout error
        // and if we have to retry the previously unfinished buffer

        if ((pIniPort->dwLastCommError == ERROR_NOT_READY) &&
            pIniPort->cbLastBuffer)
        {
            pIniPort->dwLastCommError = 0;

            rc = WriteFile(pIniPort->hFile, pIniPort->pLastBuffer, pIniPort->cbLastBuffer, &dwTransfer, &(pIniPort->ovWrite));

            if (!rc && (GetLastError() != ERROR_IO_PENDING))
            {
                // this should never happen

                return FALSE;
            }
        }

RetryTransmit:

        // Future optimization:
        // we should probably only call WaitCommEvent only when we need to
        // i.e. when there is not another WaitCommEvent pending

        WaitCommEvent(pIniPort->hFile, &(pIniPort->dwEventMask), &(pIniPort->ovEvent));

        dwObject = WaitForMultipleObjects(2, pIniPort->ahEvents, FALSE, INFINITE);
        dwObject -= WAIT_OBJECT_0;

        // dwObject should be either 0 or 1
        // 0: comm event occured
        // 1: last write has either completed or timeout

        pov = !dwObject ? &(pIniPort->ovEvent) : &(pIniPort->ovWrite);

        if (!GetOverlappedResult(pIniPort->hFile, pov, &dwTransfer, FALSE))
        {
            // this should never happens

            DBGMSG(DBG_ERROR, ("GetOverlappedResult FAILED %x\n", GetLastError()));
            return FALSE;
        }

        if (dwObject == 0)
        {
            // a printer error occured in the previous write

            ClearCommError(pIniPort->hFile, &(pIniPort->dwLastCommError), NULL);

            // these are the possible errors from ClearCommError()
            // ERROR_OUT_OF_PAPER for CE_OOP
            // ERROR_NOT_READY for CE_DNS
            // ERROR_IO_DEVICE for CE_IOE
            // ERROR_xxx for CE_PTO
            // ...

            if (pIniPort->dwLastCommError & CE_OOP)
                LastError = ERROR_OUT_OF_PAPER;
            else if (pIniPort->dwLastCommError & CE_IOE)
                LastError = ERROR_IO_DEVICE;
            else if (pIniPort->dwLastCommError & CE_PTO)
                LastError = ERROR_NOT_READY;
            else if (pIniPort->dwLastCommError & CE_DNS)
            {
                // check DeviceNotSelectedTimeout

                if (TransmitTimeout(pIniPort, pIniPort->dnsTimeout))
                    LastError = ERROR_NOT_READY;
                else
                {
                    Sleep(5000);    // sleep for 5 seconds
                    goto RetryTransmit;
                }
            }
            else
                LastError = ERROR_IO_DEVICE;    // generic error
        }
        else
        {
            // the previous WriteFile has returned
            // though it doesn't mean it has written everything

            ResetEvent(pov->hEvent);

            // we had at least written one byte

            if (dwTransfer)
                pIniPort->dwRetry = 0;

            if (dwTransfer < pIniPort->cbLastBuffer)
            {
                // timeout occured
                // so we didn't finish writing the previous buffer

                pIniPort->dwLastCommError =
                LastError = ERROR_NOT_READY;

                if (dwTransfer)
                {
                    // we had at least written one byte

                    pIniPort->cbLastBuffer -= dwTransfer;
                    pIniPort->pLastBuffer += dwTransfer;
                }
            }
            else
            {
                // we had successfully written everything

                pIniPort->dwLastCommError = 0;
            }
        }
    }

    if (LastError)
    {
        SetLastError(LastError);
        return FALSE;
    }

    return TRUE;
}
#endif


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
BOOL
WINAPI
LocalmonWritePort(
    HANDLE  hPort,
    LPBYTE  pBuffer,
    DWORD   cbBuf,
    LPDWORD pcbWritten
)
{
    PINIMONPORT    pIniPort = (PINIMONPORT)hPort;
    BOOL    rc;


#ifdef USE_OVERLAPPED_IO // ----------------------------------------

    if (!(pIniPort->pLastBuffer))
    {
        // we're not doing overlapped io

        return WriteFile(pIniPort->hFile, pBuffer, cbBuf, pcbWritten, NULL);
    }

    if (!WaitForCompleteOrError(pIniPort))
    {
        // we had an error or timeout
        // return failure and note that we didn't write a thing this time

        *pcbWritten = 0;
        return FALSE;
    }

    // zero size buffer?

    // IMPORTANT NOTE !!!
    // we need to do this checking after WaitForCompleteOrError()
    // spooler calls monitor with zero size buffer before it calls EndDocPort
    // to make sure the very last WritePort is done.

    if (!cbBuf)
    {
        *pcbWritten = 0;
        return TRUE;
    }

    // now we know that we've really completed the last write so we can
    // write this current buffer

    // if the buffer is larger than 4k, only write 4k at a time

    if (cbBuf > OUT_QUEUE_SIZE)
        cbBuf = OUT_QUEUE_SIZE;

#ifdef _SETUP_COMM
    rc = WriteFile(pIniPort->hFile, pBuffer, cbBuf, pcbWritten, &(pIniPort->ovWrite));
#else
    pIniPort->pLastBuffer = pIniPort->pOutBuffer;
    CopyMemory(pIniPort->pLastBuffer, pBuffer, cbBuf);
    rc = WriteFile(pIniPort->hFile, pIniPort->pLastBuffer, cbBuf, pcbWritten, &(pIniPort->ovWrite));
#endif

    if (!rc && (GetLastError() == ERROR_IO_PENDING))
    {
        // pretend that we've succefully written the whole buffer

        SetLastError(ERROR_SUCCESS);
        pIniPort->cbLastBuffer = *pcbWritten = cbBuf;
        rc = TRUE;
    }

#else // We no longer use overlapped I/O ------------------------------------

RetryTransmit:
    rc = WriteFile(pIniPort->hFile, pBuffer, cbBuf, pcbWritten, NULL);

    if (!rc && !*pcbWritten)
    {
        DWORD dwLastCommError;
        DWORD LastError;

        // a printer error occured in the previous write

        ClearCommError(pIniPort->hFile, &dwLastCommError, NULL);

        DBGMSG(DBG_ERROR, ("LocalmonWritePort: WriteFile FAILED %d\n", GetLastError()));

        // ASSERT if it's not CE_OOP or CE_DNS or CE_PTO

        ASSERT((dwLastCommError & (CE_OOP | CE_DNS | CE_PTO)), ("Unknown WriteFile Error.\n"))

        // these are the possible errors from ClearCommError()
        // ERROR_OUT_OF_PAPER for CE_OOP
        // ERROR_NOT_READY for CE_DNS
        // ERROR_IO_DEVICE for CE_IOE
        // ERROR_xxx for CE_PTO
        // ...

        if (dwLastCommError & CE_OOP)
            LastError = ERROR_OUT_OF_PAPER;

        else if (dwLastCommError & (CE_BREAK | CE_IOE))
            LastError = ERROR_IO_DEVICE;

        else if (dwLastCommError & CE_DNS)
        {
            // check DeviceNotSelectedTimeout

            if (TransmitTimeout(pIniPort, pIniPort->dnsTimeout))
                LastError = ERROR_NOT_READY;
            else
                goto RetryTransmit;
        }
        else
        {
            // CE_PTO and
            // other errors
            // check TransmissionRetryTimeout

            if (TransmitTimeout(pIniPort, pIniPort->txTimeout))
                LastError = ERROR_COUNTER_TIMEOUT;
            else
                goto RetryTransmit;
        }

        SetLastError(LastError);
    }
    else
    {
        pIniPort->dwRetry = 0;
        rc = TRUE;
    }
#endif

    return rc;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
BOOL
WINAPI
LocalmonEndDocPort(
    HANDLE   hPort
)
{
    PINIMONPORT    pIniPort = (PINIMONPORT)hPort;

    if (!(pIniPort->Status & PP_INSTARTDOC))
    {
        return TRUE;
    }

#ifdef USE_OVERLAPPED_IO
#ifndef _SETUP_COMM
    if (pIniPort->pLastBuffer)
    {
        GlobalFree(pIniPort->pOutBuffer);
        pIniPort->pLastBuffer = NULL;
    }
#endif
#endif

    if (pIniPort->hPrinter)
    {
        ClosePrinter(pIniPort->hPrinter);
        pIniPort->hPrinter = NULL;
    }

    if (pIniPort->Status & PP_LPT || pIniPort->Status & PP_COM)
        PurgeComm(pIniPort->hFile, PURGE_RXABORT | PURGE_RXCLEAR);

    CloseHandle(pIniPort->hFile);

   EnterMonSem();

    pIniPort->Status &= ~PP_INSTARTDOC;

   LeaveMonSem();

    return TRUE;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
BOOL
WINAPI
LocalmonClosePort(
    HANDLE  hPort
)
{
    // do nothing for localmon
    UNREFERENCED_PARAMETER( hPort );

    return TRUE;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
BOOL
WINAPI
LocalmonDeletePort(
    LPTSTR   pName,
    HWND    hWnd,
    LPTSTR   pPortName
)
{
    BOOL rc;

   EnterMonSem();

    if (rc = DeleteMonitorPortEntry( pPortName ))
        WriteProfileString(szPorts, pPortName, NULL);

   LeaveMonSem();

    return rc;

    UNREFERENCED_PARAMETER( pName );
    UNREFERENCED_PARAMETER( hWnd );
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
BOOL
WINAPI
LocalmonAddPort(
    LPTSTR   pName,
    HWND    hWnd,
    LPTSTR   pMonitorName
)
{
    LPTSTR   pPortName;
    PINIMONPORT pIniPort;
    char buf[60];

    // Get the user to enter a port name:
    pPortName = GetPortName( hWnd );

    if ( pPortName )
    {
       EnterMonSem();

        if ( FindPortFromList( (PINIENTRY)pIniFirstPort, pPortName ) )
        {
           LeaveMonSem();
            Message( hWnd, MSG_ERROR, IDS_WIN32_SPOOLER,
                     IDS_PORTALREADYEXISTS_S, pPortName );
            return FALSE;
        }

       LeaveMonSem();

        if (IsCom(pPortName))
        {
            LocalmonConfigurePort(NULL, hWnd, pPortName);
        }

        buf[0] = 0;
        LoadString(hInst, IDS_UNKNOWN_LOCAL_PORT, buf, sizeof(buf));

        if ( pIniPort = CreateMonitorPortEntry( pPortName, buf ) )
        {
            if (!WriteProfileString(szPorts, pPortName, szNull))
            {
                DeleteMonitorPortEntry(pPortName);
                return FALSE;
            }

            return TRUE;
        }
        else
            return FALSE;
    }

    return FALSE;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
BOOL
WINAPI
LocalmonConfigurePort(
    LPTSTR   pName,
    HWND  hWnd,
    LPTSTR pPortName
)
{
    BOOL ret = TRUE;

    if(IsCom(pPortName))
    {
        COMMCONFIG ccDummy;
        COMMCONFIG *pcc;
        DWORD dwSize;
        char buf[MAX_PATH];
        int len = lstrlen(pPortName) - 1;

        lstrcpy(buf, pPortName);
        if (buf[len] == ':')
            buf[len] = 0;

        ccDummy.dwProviderSubType = PST_RS232;
        dwSize = sizeof(ccDummy);
        GetDefaultCommConfig(buf, &ccDummy, &dwSize);
        if (pcc = LocalAlloc(LPTR, dwSize))
        {
            DWORD LastError = 0;

            pcc->dwProviderSubType = PST_RS232;
            if (GetDefaultCommConfig(buf, pcc, &dwSize))
            {
                if (CommConfigDialog(buf, hWnd, pcc))
                {
                    SetDefaultCommConfig(buf, pcc, dwSize);
                }
                else
                    LastError = ERROR_CANCELLED;
            }
            else
            {
                LastError = ERROR_FILE_NOT_FOUND;
                DBGMSG(DBG_ERROR, ("Localmon: GetDefaultCommConfig failed.\n"));
            }

            if (LastError)
            {
                SetLastError(LastError);
                ret = FALSE;
            }

            LocalFree(pcc);
        }
    }
    else if(IsLpt(pPortName))
    {
        // We are not configuring the LPT ports.
        //DialogBoxParam( hInst, MAKEINTRESOURCE( DLG_CONFIGURE_LPT ),
        //       hWnd, (DLGPROC)ConfigureLPTPortDlg, (LPARAM)pPortName );
        Message( hWnd, MSG_INFORMATION | MB_SETFOREGROUND, IDS_WIN32_SPOOLER,
                 IDS_NOTHING_TO_CONFIGURE );
    }
    else
        Message( hWnd, MSG_INFORMATION | MB_SETFOREGROUND, IDS_WIN32_SPOOLER,
                 IDS_NOTHING_TO_CONFIGURE );

    return ret;
}


// ---------------------------------------------------------------------------
/* GetPortName
 *
 * Puts up a dialog containing a free entry field.
 * The dialog allocates a string for the name, if a selection is made.
 */
// ---------------------------------------------------------------------------
LPTSTR
GetPortName(
    HWND hWnd
)
{
    LPTSTR pPortName;

    if( DialogBoxParam( hInst, MAKEINTRESOURCE(DLG_PORTNAME), hWnd,
                        (DLGPROC)PortNameDlg, (LPARAM)&pPortName ) != IDOK )
    {
        SetLastError(ERROR_CANCELLED);
        pPortName = NULL;
    }

    return pPortName;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
BOOL
WINAPI
LocalmonGetPrinterDataFromPort
(
    HANDLE  hPort,
    DWORD   ControlID,
    LPTSTR  pValueName,
    LPTSTR  lpInBuffer,
    DWORD   cbInBuffer,
    LPTSTR  lpOutBuffer,
    DWORD   cbOutBuffer,
    LPDWORD lpcbReturned
)
{
    PINIMONPORT    pIniPort = (PINIMONPORT)hPort;

    if (!(pIniPort->Status & PP_INSTARTDOC) || pValueName)
    {
        return FALSE;
    }

    return DeviceIoControl(
                    pIniPort->hFile,
                    ControlID,
                    lpInBuffer,
                    cbOutBuffer,
                    lpOutBuffer,
                    cbOutBuffer,
                    lpcbReturned,
                    NULL);
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
BOOL
WINAPI
LocalmonSetPortTimeOuts
(
    HANDLE  hPort,
    LPCOMMTIMEOUTS lpCTO,
    DWORD   reserved    // must be set to 0
)
{
    PINIMONPORT    pIniPort = (PINIMONPORT)hPort;
    COMMTIMEOUTS    cto;

    if (reserved != 0)
        return FALSE;

    if (!(pIniPort->Status & PP_INSTARTDOC))
    {
        return FALSE;
    }

    if (GetCommTimeouts(pIniPort->hFile, &cto))
    {
        cto.ReadTotalTimeoutConstant = lpCTO->ReadTotalTimeoutConstant;
        cto.ReadIntervalTimeout = lpCTO->ReadIntervalTimeout;
        return SetCommTimeouts(pIniPort->hFile, &cto);
    }

    return FALSE;
}
