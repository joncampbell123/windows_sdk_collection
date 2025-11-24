/*
 *  Regdb.h  
 *
 *  Header file for regdb.c - registration database query functions
 *
 *    (c) Copyright Microsoft Corp. 1992 All Rights Reserved
 */


STDAPI_(BOOL) OleStdGetMiscStatusOfClass(REFCLSID, HKEY, DWORD FAR *);
STDAPI_(CLIPFORMAT) OleStdGetDefaultFileFormatOfClass(
        REFCLSID        rclsid, 
        HKEY            hKey
);
