/*
 *  Regdb.h  
 *
 *  Header file for regdb.c - registration database query functions
 *
 *    (c) Copyright Microsoft Corp. 1992-1994 All Rights Reserved
 */

#ifndef _REGDB_H_
#define _REGDB_H_

STDAPI_(unsigned long) OleStdGetMiscStatusOfClass(REFCLSID, HKEY, unsigned long  *);
STDAPI_(ResType) OleStdGetDefaultFileFormatOfClass(
        REFCLSID        rclsid, 
        HKEY            hKey
);
 
#endif //_REGDB_H_
