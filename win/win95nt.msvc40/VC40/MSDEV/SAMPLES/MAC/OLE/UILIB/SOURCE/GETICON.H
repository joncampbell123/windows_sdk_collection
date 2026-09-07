/*
 * GETICON.H
 *
 * This file contains definitions and function prototypes used in geticon.c
 *
 * Copyright (c)1992-1994 Microsoft Corporation, All Right Reserved
 */


#ifndef _GETICON_H
#define _GETICON_H


unsigned int     OleStdGetUserTypeOfClass(REFCLSID rclsid, char *lpszUserType, unsigned int cch, HKEY hKey);
unsigned int     OleStdGetAuxUserType(REFCLSID rclsid, short wAuxUserType,  char *lpszAuxUserType, 
                                      int cch, HKEY hKey);


#endif // _GETICON_H 
