/*************************************************************************
** 
**    OLE 2 Utility Code
**    
**    enumfetc.c
**    
**    Private definitions, structures, types, and function prototypes for the
**    CEnumFormatEtc implementation of the IEnumFORMATETC interface.
**    This file is part of the OLE 2.0 User Interface support library.
**    
**    (c) Copyright Microsoft Corp. 1990 - 1994 All Rights Reserved
**
*************************************************************************/

#ifndef __ENUMFETC_H__
#define __ENUMFETC_H__

#ifndef RC_INVOKED
#pragma message ("INCLUDING ENUMFETC.H from " __FILE__)
#endif  /* RC_INVOKED */

void EnumFORMATETCInitInterface(void);  
STDAPI_(LPENUMFORMATETC)        
  OleStdEnumFmtEtc_Create(unsigned short wCount, LPFORMATETC lpEtc);

#endif // __ENUMFETC_H__
