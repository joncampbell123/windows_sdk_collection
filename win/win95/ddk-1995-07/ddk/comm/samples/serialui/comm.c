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
// File: comm.c
//
//  This files contains all common utility routines
//
//---------------------------------------------------------------------------

#include "suiprv.h"     // common headers


//-----------------------------------------------------------------------------------
//  Port mapping functions
//-----------------------------------------------------------------------------------

#define CPORTPAIR   8

/*----------------------------------------------------------
Purpose: Device enumerator callback.  Adds another device to the
         map table.

Returns: TRUE to continue enumeration
Cond:    --
*/
BOOL WINAPI PortMap_Add(
    HKEY hkeyDev,
    HKEY hkeyDrv,
    LPARAM lParam)
    {
    BOOL bRet = FALSE;
    LPPORTMAP pmap = (LPPORTMAP)lParam;
    LPPORTPAIR ppair;
    DWORD cbData;
    int cb;
    int cbUsed;
    char sz[LINE_LEN];

    // Time to reallocate the table?
    cb = LocalSize(LOCALOF(pmap->rgports));
    cbUsed = pmap->cports * sizeof(*ppair);
    if (cbUsed >= cb)
        {
        // Yes
        cb += (CPORTPAIR * sizeof(*ppair));

        if (!MyLocalReAlloc((LPVOID FAR *)&pmap->rgports, cbUsed, cb))
            {
            goto Leave;
            }
        }

    ppair = &pmap->rgports[pmap->cports++];

    cbData = sizeof(sz);
    if (OK == RegQueryValueEx(hkeyDev, (LPSTR)c_szPortName, NULL, NULL, sz, &cbData))
        {
        lstrcpy(ppair->szPortName, sz);

        cbData = sizeof(sz);
        if (OK == RegQueryValueEx(hkeyDev, (LPSTR)c_szFriendlyName, NULL, NULL, sz, &cbData))
            {
            lstrcpy(ppair->szFriendlyName, sz);
            bRet = TRUE;
            }
        }

    if (!bRet)
        {
        *ppair->szPortName = 0;
        *ppair->szFriendlyName = 0;
        }

Leave:
    return bRet;
    }


/*----------------------------------------------------------
Purpose: Creates a port map

Returns: TRUE on success
Cond:    --
*/
BOOL PUBLIC PortMap_Create(
    LPPORTMAP FAR * ppmap)
    {
    LPPORTMAP pmap;

    pmap = (LPPORTMAP)LocalAlloc(LPTR, sizeof(*pmap));
    if (pmap)
        {
        // Initially alloc 8 entries
        pmap->rgports = (LPPORTPAIR)LocalAlloc(LPTR, CPORTPAIR*sizeof(*pmap->rgports));
        if (pmap->rgports)
            {
            // Fill the map table
            EnumerateDevices(c_szPortClass, PortMap_Add, (LPARAM)pmap);
            }
        else
            {
            // Error
            LocalFree(LOCALOF(pmap));
            pmap = NULL;
            }
        }

    *ppmap = pmap;

    return (NULL != pmap);
    }


/*----------------------------------------------------------
Purpose: Returns the friendly name given the port name

Returns: see above or
         NULL if the port name is not found
Cond:    --
*/
LPCSTR PUBLIC PortMap_GetFriendly(
    LPPORTMAP pmap,
    LPCSTR pszPortName)
    {
    LPPORTPAIR pport = pmap->rgports;
    int cports = pmap->cports;
    int i;

    for (i = 0; i < cports; i++, pport++)
        {
        if (0 == lstrcmpi(pszPortName, pport->szPortName))
            return pport->szFriendlyName;
        }
    return NULL;
    }


/*----------------------------------------------------------
Purpose: Returns the port name given the friendly name 

Returns: see above or
         NULL if the friendly name is not found
Cond:    --
*/
LPCSTR PUBLIC PortMap_GetPortName(
    LPPORTMAP pmap,
    LPCSTR pszFriendlyName)
    {
    LPPORTPAIR pport = pmap->rgports;
    int cports = pmap->cports;
    int i;

    for (i = 0; i < cports; i++, pport++)
        {
        if (0 == lstrcmpi(pszFriendlyName, pport->szFriendlyName))
            return pport->szPortName;
        }
    return NULL;
    }


/*----------------------------------------------------------
Purpose: Frees a port map

Returns: --
Cond:    --
*/
void PUBLIC PortMap_Free(
    LPPORTMAP pmap)
    {
    if (pmap)
        {
        if (pmap->rgports)
            LocalFree(LOCALOF(pmap->rgports));

        LocalFree(LOCALOF(pmap));
        }
    }


//-----------------------------------------------------------------------------------
//  Generic registry enumerator callback mechanism
//-----------------------------------------------------------------------------------

#pragma data_seg(DATASEG_READONLY)

char const FAR c_szPathClass[] = REGSTR_PATH_CLASS "\\";
char const FAR c_szDriver[] = REGSTR_VAL_DRIVER;
char const FAR c_szRegstrPathEnum[] = REGSTR_PATH_ENUM;

#pragma data_seg()


/*----------------------------------------------------------
Purpose: Enumerates the HKEY_LOCAL_MACHINE branch and calls
         pfnDevice for each device that is found that matches
         the specified class.

         pfnDevice can terminate the enumeration by returning FALSE.

Returns: TRUE if at least one device was found
Cond:    --
*/
BOOL PUBLIC EnumerateDevices(
    LPCSTR pszClass,
    ENUMDEVICEPROC pfnDevice,
    LPARAM lParam)
    {
    BOOL bRet = FALSE;
    HKEY hkeyEnum;
    HKEY hkeyRoot;
    HKEY hkeyID;
    HKEY hkeyDev;
    HKEY hkeyDriver;
    char szKey[LINE_LEN];
    char szName[LINE_LEN];

    if (OK == RegOpenKey(HKEY_LOCAL_MACHINE, c_szRegstrPathEnum, &hkeyEnum))
        {
        // Enumerate the Enum branch
        int iEnum = 0;
        while (OK == RegEnumKey(hkeyEnum, iEnum++, szKey, sizeof(szKey)))
            {
            if (OK == RegOpenKey(hkeyEnum, szKey, &hkeyRoot))
                {
                // Enumerate the Root branch (the enumerators)
                int iRoot = 0;
                while (OK == RegEnumKey(hkeyRoot, iRoot++, szKey, sizeof(szKey)))
                    {
                    if (OK == RegOpenKey(hkeyRoot, szKey, &hkeyID))
                        {
                        // Enumerate the hardware ID branch
                        int iID = 0;
                        while (OK == RegEnumKey(hkeyID, iID++, szKey, sizeof(szKey)))
                            {
                            if (OK == RegOpenKey(hkeyID, szKey, &hkeyDev))
                                {
                                DWORD cbData = sizeof(szName);

                                // Does the class match?
                                if (OK == RegQueryValueEx(hkeyDev, c_szClass, NULL,
                                    NULL, szName, &cbData) &&
                                    0 == lstrcmpi(pszClass, szName))
                                    {
                                    // Yes; open the software key
                                    lstrcpy(szName, c_szPathClass);
                                    cbData = sizeof(szName);
                                    if (OK == RegQueryValueEx(hkeyDev, (LPSTR)c_szDriver, 
                                            NULL, NULL, &szName[lstrlen(szName)], 
                                            &cbData) && 
                                        OK == RegOpenKey(HKEY_LOCAL_MACHINE, 
                                            szName, &hkeyDriver))
                                        {
                                        BOOL bContinue = pfnDevice(hkeyDev, hkeyDriver, lParam);

                                        RegCloseKey(hkeyDriver);
                                        bRet = TRUE;

                                        // Continue?
                                        if (!bContinue)
                                            {
                                            // No
                                            RegCloseKey(hkeyDev);
                                            RegCloseKey(hkeyID);
                                            RegCloseKey(hkeyRoot);
                                            RegCloseKey(hkeyEnum);
                                            goto Leave;
                                            }
                                        }
                                    }
                                RegCloseKey(hkeyDev);
                                }
                            }
                        RegCloseKey(hkeyID);
                        }
                    }
                RegCloseKey(hkeyRoot);
                }
            }
        RegCloseKey(hkeyEnum);
        }

Leave:
    return bRet;
    }


/*----------------------------------------------------------
Purpose: Enumerates the HKEY_LOCAL_MACHINE branch and finds the
         device matching the given class and value.  If there
         are duplicate devices that match both criteria, only the
         first device is returned. 

         Returns TRUE if the device was found.

         If this function returns TRUE, *phKeyDev and *phKeyDrv 
         are set to the opened device and driver keys.  Otherwise
         they are NULL.

         The caller must call RegCloseKey for *phKeyDev and *phKeyDrv.

Returns: see above
Cond:    --
*/
BOOL PUBLIC FindDeviceByString(
    LPCSTR pszClass,
    LPCSTR pszValueName,
    LPCSTR pszValue,
    HKEY FAR * phKeyDev,        // Pointer to returned device key
    HKEY FAR * phKeyDrv)        // Pointer to returned driver key
    {
    BOOL bRet = FALSE;
    HKEY hkeyEnum;
    HKEY hkeyRoot;
    HKEY hkeyID;
    HKEY hkeyDev;
    HKEY hkeyDriver;
    char szKey[LINE_LEN];
    char szName[LINE_LEN];

    if (OK == RegOpenKey(HKEY_LOCAL_MACHINE, c_szRegstrPathEnum, &hkeyEnum))
        {
        // Enumerate the Enum branch
        int iEnum = 0;
        while (OK == RegEnumKey(hkeyEnum, iEnum++, szKey, sizeof(szKey)))
            {
            if (OK == RegOpenKey(hkeyEnum, szKey, &hkeyRoot))
                {
                // Enumerate the Root branch (the enumerators)
                int iRoot = 0;
                while (OK == RegEnumKey(hkeyRoot, iRoot++, szKey, sizeof(szKey)))
                    {
                    if (OK == RegOpenKey(hkeyRoot, szKey, &hkeyID))
                        {
                        // Enumerate the hardware ID branch
                        int iID = 0;
                        while (OK == RegEnumKey(hkeyID, iID++, szKey, sizeof(szKey)))
                            {
                            if (OK == RegOpenKey(hkeyID, szKey, &hkeyDev))
                                {
                                DWORD cbData = sizeof(szName);

                                // Does the class match?
                                if (OK == RegQueryValueEx(hkeyDev, 
                                                          c_szClass, 
                                                          NULL,
                                                          NULL, 
                                                          szName, 
                                                          &cbData) &&
                                    0 == lstrcmpi(pszClass, szName))
                                    {
                                    // Yes; does the value match?
                                    cbData = sizeof(szName);
                                    if (OK == RegQueryValueEx(hkeyDev, 
                                                              pszValueName, 
                                                              NULL,
                                                              NULL, 
                                                              szName, 
                                                              &cbData) &&
                                        0 == lstrcmpi(pszValue, szName))
                                        {
                                        // Yes; open the software key
                                        lstrcpy(szName, c_szPathClass);
                                        cbData = sizeof(szName);
                                        if (OK == RegQueryValueEx(hkeyDev, 
                                                                  c_szDriver, 
                                                                  NULL, 
                                                                  NULL, 
                                                                  &szName[lstrlen(szName)],
                                                                  &cbData) && 
                                            OK == RegOpenKey(HKEY_LOCAL_MACHINE, 
                                                             szName, 
                                                             &hkeyDriver))
                                            {
                                            // Close all the keys except for
                                            // the device and driver keys and
                                            // exit
                                            *phKeyDev = hkeyDev;
                                            *phKeyDrv = hkeyDriver;

                                            RegCloseKey(hkeyID);
                                            RegCloseKey(hkeyRoot);
                                            RegCloseKey(hkeyEnum);
                                            bRet = TRUE;
                                            goto Leave;
                                            }
                                        }
                                    }
                                RegCloseKey(hkeyDev);
                                }
                            }
                        RegCloseKey(hkeyID);
                        }
                    }
                RegCloseKey(hkeyRoot);
                }
            }
        RegCloseKey(hkeyEnum);
        }

Leave:
    return bRet;
    }


/*----------------------------------------------------------
Purpose: Performs a local realloc my way

Returns: TRUE on success
Cond:    --
*/
BOOL PUBLIC MyLocalReAlloc(
    LPVOID FAR * ppv,
    int cbOld,
    int cbNew)
    {
    LPVOID pv = (LPVOID)LocalAlloc(LPTR, cbNew);

    if (LOCALOF(pv))
        {
        BltByte(pv, *ppv, min(cbOld, cbNew));
        LocalFree(LOCALOF(*ppv));
        *ppv = pv;
        }

    return (NULL != pv);
    }


/*----------------------------------------------------------
Purpose: Get a string from the resource string table.  
Returns: Ptr to string
Cond:    --
*/
LPSTR PUBLIC SzFromIDS(
    UINT ids,               // resource ID
    LPSTR pszBuf,           // buffer
    int cbBuf)              // size of buffer
    {
    ASSERT(pszBuf);
    ASSERT(cbBuf > 0);

    *pszBuf = NULL_CHAR;
    LoadString(g_hinst, ids, pszBuf, cbBuf);
    return pszBuf;
    }
