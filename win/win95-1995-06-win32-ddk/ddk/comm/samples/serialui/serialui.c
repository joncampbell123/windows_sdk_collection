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

//---------------------------------------------------------------------------
//
// File: serialui.c
//
// This files contains the DLL entry-points.
//
// Much of this file contains the code that builds the default property dialog
// for serial ports.
//
//---------------------------------------------------------------------------


#include "suiprv.h"     // common headers
#include "serialui.h"
#include "res.h"


#define MAX_PROP_PAGES  1          // Define a reasonable limit

DWORD WINAPI drvSetDefaultCommConfig(LPSTR pszFriendlyName, LPCOMMCONFIG pcc, DWORD dwSize);
DWORD WINAPI drvGetDefaultCommConfig(LPCSTR pszFriendlyName, LPCOMMCONFIG pcc, LPDWORD pdwSize);

//-----------------------------------------------------------------------------------
//  
//-----------------------------------------------------------------------------------


/*----------------------------------------------------------
Purpose: Composes a string of the format "baud,parity,data,stopbit"

Returns: --
Cond:    --
*/
void PRIVATE ComposeModeComString(
    LPCOMMCONFIG pcc,
    LPSTR pszBuffer)
    {
    WIN32DCB FAR * pdcb = &pcc->dcb;
    char chParity;
    LPCSTR pszStop;
    char chFlow;

    static char rgchParity[] = {'n', 'o', 'e', 'm', 's'};
    static LPCSTR rgpszStop[] = {"1", "1.5", "2"};
    
    // Parity
    ASSERT(!pdcb->fParity && NOPARITY == pdcb->Parity || pdcb->fParity);
    ASSERT(0 <= pdcb->Parity && ARRAYSIZE(rgchParity) > pdcb->Parity);

    if (0 <= pdcb->Parity && ARRAYSIZE(rgchParity) > pdcb->Parity)
        {
        chParity = rgchParity[pdcb->Parity];
        }
    else
        {
        chParity = rgchParity[0];   // Safety net
        }

    // Stop bits
    ASSERT(0 <= pdcb->StopBits && ARRAYSIZE(rgpszStop) > pdcb->StopBits);

    if (0 <= pdcb->StopBits && ARRAYSIZE(rgpszStop) > pdcb->StopBits)
        {
        pszStop = rgpszStop[pdcb->StopBits];
        }
    else
        {
        pszStop = rgpszStop[0];   // Safety net
        }

    // Flow control
    if (FALSE != pdcb->fOutX && FALSE == pdcb->fOutxCtsFlow)
        {
        chFlow = 'x';       // XON/XOFF flow control
        }
    else if (FALSE == pdcb->fOutX && FALSE != pdcb->fOutxCtsFlow)
        {
        chFlow = 'p';       // Hardware flow control
        }
    else
        {
        chFlow = ' ';       // No flow control
        }

    wsprintf(pszBuffer, "%ld,%c,%d,%s,%c", pdcb->BaudRate, chParity, pdcb->ByteSize,
        pszStop, chFlow);
    }


/*----------------------------------------------------------
Purpose: Initialize the port info.

Returns: --
Cond:    --
*/
void PRIVATE InitializePortInfo(
    LPCSTR pszFriendlyName,
    LPPORTINFO pportinfo,
    LPCOMMCONFIG pcc)
    {
    ASSERT(pportinfo);
    ASSERT(pcc);

    // Read-only fields
    pportinfo->pcc = pcc;

    BltByte(&pportinfo->dcb, &pcc->dcb, sizeof(pportinfo->dcb));

    lstrcpyn(pportinfo->szFriendlyName, pszFriendlyName, sizeof(pportinfo->szFriendlyName));
    }


/*----------------------------------------------------------
Purpose: Gets a WIN32DCB from the registry.

Returns: One of the ERROR_ values
Cond:    --
*/
DWORD PRIVATE RegQueryDCB(
    HKEY hkey,
    WIN32DCB FAR * pdcb)
    {
    DWORD dwRet = ERROR_BADKEY;
    DWORD cbData;

    ASSERT(pdcb);

    // Does the DCB key exist in the driver key?
    if (ERROR_SUCCESS == RegQueryValueEx(hkey, c_szDCB, NULL, NULL, NULL, &cbData))
        {
        // Yes; is the size in the registry okay?  
        if (sizeof(*pdcb) < cbData)
            {
            // No; the registry has bogus data
            dwRet = ERROR_BADDB;
            }
        else
            {
            // Yes; get the DCB from the registry
            if (ERROR_SUCCESS == RegQueryValueEx(hkey, c_szDCB, NULL, NULL, (LPSTR)pdcb, &cbData))
                {
                if (sizeof(*pdcb) == pdcb->DCBlength)
                    {
                    dwRet = NO_ERROR;
                    }
                else
                    {
                    dwRet = ERROR_BADDB;
                    }
                }
            else
                {
                dwRet = ERROR_BADKEY;
                }
            }
        }

    return dwRet;
    }


/*----------------------------------------------------------
Purpose: Frees the portinfo struct

Returns: --
Cond:    --
*/
void PRIVATE FreePortInfo(
    LPPORTINFO pportinfo)
    {
    if (pportinfo)
        {
        if (pportinfo->pcc)
            LocalFree(LOCALOF(pportinfo->pcc));

        LocalFree(LOCALOF(pportinfo));
        }
    }


/*----------------------------------------------------------
Purpose: Release the data associated with the Port Settings page
Returns: --
Cond:    --
*/
UINT CALLBACK PortSettingsCallback(
    HWND hwnd,
    UINT uMsg,
    LPPROPSHEETPAGE ppsp)
    {
    if (PSPCB_RELEASE == uMsg)
        {
        LPPORTINFO pportinfo = (LPPORTINFO)ppsp->lParam;
        LPCOMMCONFIG pcc;

        ASSERT(pportinfo);

        pcc = pportinfo->pcc;

        if (IDOK == pportinfo->idRet)
            {
            // Save the changes back to the commconfig struct
            BltByte(&pcc->dcb, &pportinfo->dcb, sizeof(pcc->dcb));

            // Are we releasing from the Device Mgr?
            if (IsFlagSet(pportinfo->uFlags, SIF_FROM_DEVMGR))
                {
                // Yes; save the commconfig now as well
                drvSetDefaultCommConfig(pportinfo->szFriendlyName, pcc, pcc->dwSize);

                // Free the portinfo struct only when called from the Device Mgr
                FreePortInfo(pportinfo);
                }
            }
        }

    return TRUE;
    }


/*----------------------------------------------------------
Purpose: Add the port settings page.  

Returns: ERROR_ value

Cond:    --
*/
DWORD PRIVATE AddPortSettingsPage(
    LPPORTINFO pportinfo,
    LPFNADDPROPSHEETPAGE pfnAdd, 
    LPARAM lParam)
    {
    DWORD dwRet = ERROR_NOT_ENOUGH_MEMORY;
    PROPSHEETPAGE   psp;
    HPROPSHEETPAGE  hpage;

    ASSERT(pportinfo);
    ASSERT(pfnAdd);

    // Add the Port Settings property page
    //
    psp.dwSize = sizeof(PROPSHEETPAGE);
    psp.dwFlags = PSP_USECALLBACK;
    psp.hInstance = g_hinst;
    psp.pszTemplate = MAKEINTRESOURCE(IDD_PORTSETTINGS);
    psp.pfnDlgProc = Port_WrapperProc;
    psp.lParam = (LPARAM)pportinfo;
    psp.pfnCallback = PortSettingsCallback;
    
    hpage = CreatePropertySheetPage(&psp);
    if (hpage)
        {
        if (!pfnAdd(hpage, lParam))
            DestroyPropertySheetPage(hpage);
        else
            dwRet = NO_ERROR;
        }
    
    return dwRet;
    }


/*----------------------------------------------------------
Purpose: Function that is called by EnumPropPages entry-point to
         add property pages.

Returns: TRUE on success
         FALSE on failure

Cond:    --
*/
BOOL CALLBACK AddInstallerPropPage(
    HPROPSHEETPAGE hPage, 
    LPARAM lParam)
    {
    PROPSHEETHEADER FAR * ppsh = (PROPSHEETHEADER FAR *)lParam;
 
    if (ppsh->nPages < MAX_PROP_PAGES)
        {
        ppsh->phpage[ppsh->nPages] = hPage;
        ++ppsh->nPages;
        return(TRUE);
        }
    return(FALSE);
    }


/*----------------------------------------------------------
Purpose: Bring up property sheet for a serial port

Returns: ERROR_ value
Cond:    --
*/
DWORD PRIVATE DoProperties(
    LPCSTR pszFriendlyName,
    HWND hwndParent,
    LPCOMMCONFIG pcc)
    {
    DWORD dwRet;
    PROPSHEETHEADER psh;
    HPROPSHEETPAGE hpsPages[MAX_PROP_PAGES];
    LPPORTINFO pportinfo;

    // Initialize the PropertySheet Header
    psh.dwSize = sizeof(psh);
    psh.dwFlags = PSH_PROPTITLE;
    psh.hwndParent = hwndParent;
    psh.hInstance = g_hinst;
    psh.nPages = 0;
    psh.nStartPage = 0;
    psh.phpage = (HPROPSHEETPAGE FAR *)hpsPages;

    // Allocate the working buffer
    //
    pportinfo = (LPPORTINFO)LocalAlloc(LPTR, sizeof(*pportinfo));
    if (pportinfo)
        {
        InitializePortInfo(pszFriendlyName, pportinfo, pcc);
        psh.pszCaption = pportinfo->szFriendlyName;

        dwRet = AddPortSettingsPage(pportinfo, AddInstallerPropPage, (LPARAM)&psh);

        if (NO_ERROR == dwRet)
            {
            // Show the property sheet
            PropertySheet(&psh);

            dwRet = (IDOK == pportinfo->idRet) ? NO_ERROR : ERROR_CANCELLED;
            }

        // Clear the pcc field so FreePortInfo does not prematurely free it,
        // since we did not allocate it.
        pportinfo->pcc = NULL;
        FreePortInfo(pportinfo);
        }
    else
        {
        dwRet = ERROR_NOT_ENOUGH_MEMORY;
        }
    
    return dwRet;
    }


/*----------------------------------------------------------
Purpose: Derives a PORTINFO struct from a device info.

Returns: TRUE on success

Cond:    --
*/
BOOL PRIVATE DeviceInfoToPortInfo(
    LPDEVICE_INFO pdi,
    LPPORTINFO pportinfo)
    {
    BOOL bRet = FALSE;
    COMMCONFIG ccDummy;
    LPCOMMCONFIG pcommconfig;
    DWORD cbSize;
    DWORD cbData;
    char szFriendly[MAXFRIENDLYNAME];
    HKEY hkeyDev;
    HKEY hkeyDrv;

    // Find the device by looking for the device description.  (Note the
    // device description is not always the same as the friendly name.)

    if (FindDeviceByString(c_szPortClass, c_szDeviceDesc, pdi->szDescription, 
        &hkeyDev, &hkeyDrv))
        {
        cbData = sizeof(szFriendly);
        if (ERROR_SUCCESS == RegQueryValueEx(hkeyDev, c_szFriendlyName, NULL, NULL, 
            szFriendly, &cbData))
            {
            ccDummy.dwProviderSubType = PST_RS232;
            cbSize = sizeof(COMMCONFIG);
            drvGetDefaultCommConfig(szFriendly, &ccDummy, &cbSize);

            pcommconfig = (LPCOMMCONFIG)LocalAlloc(LPTR, (UINT)cbSize);
            if (pcommconfig)
                {
                // Get the commconfig from the registry
                pcommconfig->dwProviderSubType = PST_RS232;
                if (NO_ERROR == drvGetDefaultCommConfig(szFriendly, pcommconfig, 
                    &cbSize))
                    {
                    // Initialize the port info from the commconfig
                    InitializePortInfo(szFriendly, pportinfo, pcommconfig);

                    SetFlag(pportinfo->uFlags, SIF_FROM_DEVMGR);
                    bRet = TRUE;
                    }
                else
                    {
                    // Failure
                    LocalFree(LOCALOF(pcommconfig));
                    }

                // pcommconfig is freed in ReleasePortSettingsPage
                }
            }
        RegCloseKey(hkeyDev);
        RegCloseKey(hkeyDrv);
        }

    return bRet;
    }


/*----------------------------------------------------------
Purpose: EnumDevicePropPages entry-point.  This entry-point
         gets called only when the Device Manager asks for 
         additional property pages.  

Returns: TRUE on success
         FALSE if pages could not be added
Cond:    --
*/
BOOL WINAPI EnumPropPages(
    LPDEVICE_INFO pdi, 
    LPFNADDPROPSHEETPAGE pfnAdd, 
    LPARAM lParam)              // Don't touch the lParam value, just pass it on!
    {
    BOOL bRet = FALSE;
    LPPORTINFO pportinfo;

    ASSERT(pdi);
    ASSERT(pfnAdd);

    pportinfo = (LPPORTINFO)LocalAlloc(LPTR, sizeof(*pportinfo));
    if (pportinfo)
        {
        // Convert the device info struct to a portinfo.
        bRet = DeviceInfoToPortInfo(pdi, pportinfo);
        if (bRet)
            {
            AddPortSettingsPage(pportinfo, pfnAdd, lParam);
            }
        else
            {
            // Failed
            FreePortInfo(pportinfo);
            }
        // pportinfo is freed in ReleasePortSettingsPage
        }

    return bRet;
    }


/*----------------------------------------------------------
Purpose: Invokes the serial port configuration dialog.  

Returns: One of the ERROR_ values
Cond:    --
*/
DWORD PRIVATE MyCommConfigDialog(
    HKEY hkeyDev,           // Device (hardware) key
    HKEY hkeyDrv,           // Driver (software) key
    LPCSTR pszFriendlyName,
    HWND hwndOwner,
    LPCOMMCONFIG pcc)
    {
    DWORD dwRet;
    
    // (Wrapper should have checked these first)
    ASSERT(pszFriendlyName);
    ASSERT(pcc);
    ASSERT(sizeof(*pcc) <= pcc->dwSize);

    dwRet = DoProperties(pszFriendlyName, hwndOwner, pcc);

    return dwRet;
    }


/*----------------------------------------------------------
Purpose: Gets the default COMMCONFIG for the specified device.
         This API doesn't require a handle.

Returns: One of the ERROR_ values in winerror.h

Cond:    --
*/
DWORD PRIVATE MyGetDefaultCommConfig(
    HKEY hkeyDev,           // Device (hardware) key
    HKEY hkeyDrv,           // Driver (software) key
    LPCSTR pszFriendlyName,
    LPCOMMCONFIG pcc,
    LPDWORD pdwSize)
    {
    DWORD dwRet;
    
    // (Wrapper should have checked these first)
    ASSERT(pszFriendlyName);
    ASSERT(pcc);
    ASSERT(pdwSize);
    ASSERT(sizeof(*pcc) <= *pdwSize);

    *pdwSize = sizeof(*pcc);

    // Initialize the commconfig structure
    pcc->dwSize = *pdwSize;
    pcc->wVersion = COMMCONFIG_VERSION_1;
    pcc->dwProviderSubType = PST_RS232;
    pcc->dwProviderOffset = 0;
    pcc->dwProviderSize = 0;

    dwRet = RegQueryDCB(hkeyDrv, &pcc->dcb);

    return dwRet;
    }


/*----------------------------------------------------------
Purpose: Sets the default COMMCONFIG for the specified device.
         This API doesn't require a handle.  This function
         strictly modifies the registry.  Use SetCommConfig
         to set the COMMCONFIG of an open device.

         If the dwSize parameter or the dwSize field are invalid 
         sizes (given the dwProviderSubType field in COMMCONFIG), 
         then this function fails.

Returns: One of the ERROR_ return values

Cond:    --
*/
DWORD PRIVATE MySetDefaultCommConfig(
    HKEY hkeyDev,           // Device (hardware) key
    HKEY hkeyDrv,           // Driver (software) key
    LPCSTR pszFriendlyName,
    LPCOMMCONFIG pcc)
    {
    DWORD dwRet;
    DWORD cbData;
    char szKey[8];
    char szValue[MAXSHORTLEN];

    // (Wrapper should have checked these first)
    ASSERT(pszFriendlyName);
    ASSERT(pcc);
    ASSERT(sizeof(*pcc) <= pcc->dwSize);

    ASSERT(0 == pcc->dwProviderSize);
    ASSERT(0 == pcc->dwProviderOffset);

    // Write the DCB to the driver key
    cbData = sizeof(WIN32DCB);
    dwRet = RegSetValueEx(hkeyDrv, c_szDCB, NULL, REG_BINARY, (LPSTR)&pcc->dcb, cbData);

    // For Win 3.1 compatibility, write some info to win.ini
    cbData = sizeof(szKey);
    if (ERROR_SUCCESS == RegQueryValueEx(hkeyDev, c_szPortName, NULL, NULL, szKey, &cbData))
        {
        lstrcat(szKey, ":");

        // Delete the old win.ini entry first
        WriteProfileString(c_szPortClass, szKey, NULL);

        ComposeModeComString(pcc, szValue);
        WriteProfileString(c_szPortClass, szKey, szValue);

        // Send a broadcast proclaiming that the win.ini has changed
        SendMessage((HWND)-1, WM_WININICHANGE, NULL, (LPARAM)c_szPortClass);
        }

    return dwRet;
    }


//-----------------------------------------------------------------------------------
//  Entry-points provided for KERNEL32 APIs
//-----------------------------------------------------------------------------------


/*----------------------------------------------------------
Purpose: Entry point for CommConfigDialog

Returns: standard error value in winerror.h
Cond:    --
*/
DWORD WINAPI drvCommConfigDialog(
    LPCSTR pszFriendlyName,
    HWND hwndOwner,
    LPCOMMCONFIG pcc)
    {
    DWORD dwRet;
    HKEY hkeyDevice;
    HKEY hkeyDriver;

    // We support friendly names (eg, "Communications Port (COM1)") or 
    // portname values (eg, "COM1").

    if (NULL == pszFriendlyName || 
        NULL == pcc)
        {
        dwRet = ERROR_INVALID_PARAMETER;
        }
    // Is the size sufficient?
    else if (sizeof(*pcc) > pcc->dwSize)
        {
        // No
        dwRet = ERROR_INSUFFICIENT_BUFFER;
        }
    else if (FindDevice(c_szPortClass, pszFriendlyName, &hkeyDevice, &hkeyDriver) ||
        FindDeviceByString(c_szPortClass, c_szPortName, pszFriendlyName, &hkeyDevice, &hkeyDriver) ||
        FindDeviceByString(c_szModemClass, c_szPortName, pszFriendlyName, &hkeyDevice, &hkeyDriver))
        {
        dwRet = MyCommConfigDialog(hkeyDevice, hkeyDriver, pszFriendlyName, hwndOwner, pcc);

        RegCloseKey(hkeyDevice);
        RegCloseKey(hkeyDriver);
        }
    else
        {
        dwRet = ERROR_BADKEY;
        }

    return dwRet;
    }


/*----------------------------------------------------------
Purpose: Entry point for GetDefaultCommConfig

Returns: standard error value in winerror.h
Cond:    --
*/
DWORD WINAPI drvGetDefaultCommConfig(
    LPCSTR pszFriendlyName,
    LPCOMMCONFIG pcc,
    LPDWORD pdwSize)
    {
    DWORD dwRet;
    HKEY hkeyDevice;
    HKEY hkeyDriver;

    // We support friendly names (eg, "Communications Port (COM1)") or 
    // portname values (eg, "COM1").

    if (NULL == pszFriendlyName || 
        NULL == pcc || 
        NULL == pdwSize)
        {
        dwRet = ERROR_INVALID_PARAMETER;
        }
    // Is the size sufficient?
    else if (sizeof(*pcc) > *pdwSize)
        {
        // No; return correct value
        dwRet = ERROR_INSUFFICIENT_BUFFER;
        *pdwSize = sizeof(*pcc);
        }
    else if (FindDevice(c_szPortClass, pszFriendlyName, &hkeyDevice, &hkeyDriver) ||
        FindDeviceByString(c_szPortClass, c_szPortName, pszFriendlyName, &hkeyDevice, &hkeyDriver) ||
        FindDeviceByString(c_szModemClass, c_szPortName, pszFriendlyName, &hkeyDevice, &hkeyDriver))
        {
        dwRet = MyGetDefaultCommConfig(hkeyDevice, hkeyDriver, pszFriendlyName, pcc, pdwSize);

        RegCloseKey(hkeyDevice);
        RegCloseKey(hkeyDriver);
        }
    else
        {
        dwRet = ERROR_BADKEY;
        }

    return dwRet;
    }


/*----------------------------------------------------------
Purpose: Entry point for SetDefaultCommConfig

Returns: standard error value in winerror.h
Cond:    --
*/
DWORD WINAPI drvSetDefaultCommConfig(
    LPSTR pszFriendlyName,
    LPCOMMCONFIG pcc,
    DWORD dwSize)           // This is ignored
    {
    DWORD dwRet;
    HKEY hkeyDevice;
    HKEY hkeyDriver;

    // We support friendly names (eg, "Communications Port (COM1)") or 
    // portname values (eg, "COM1").

    if (NULL == pszFriendlyName || 
        NULL == pcc)
        {
        dwRet = ERROR_INVALID_PARAMETER;
        }
    // Is the size sufficient?
    else if (sizeof(*pcc) > pcc->dwSize)
        {
        // No
        dwRet = ERROR_INSUFFICIENT_BUFFER;
        }
    else if (FindDevice(c_szPortClass, pszFriendlyName, &hkeyDevice, &hkeyDriver) ||
        FindDeviceByString(c_szPortClass, c_szPortName, pszFriendlyName, &hkeyDevice, &hkeyDriver) ||
        FindDeviceByString(c_szModemClass, c_szPortName, pszFriendlyName, &hkeyDevice, &hkeyDriver))
        {
        dwRet = MySetDefaultCommConfig(hkeyDevice, hkeyDriver, pszFriendlyName, pcc);

        RegCloseKey(hkeyDevice);
        RegCloseKey(hkeyDriver);
        }
    else
        {
        dwRet = ERROR_BADKEY;
        }

    return dwRet;
    }


