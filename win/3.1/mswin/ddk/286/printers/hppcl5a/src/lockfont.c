/**[f******************************************************************
* lockfont.c -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
  
// history
// 27 apr 89    peterbe     Tabs are 8 spaces, other format cleanup.
  
/**************************************************************************/
/******************************   lockfont   ******************************/
/*
*  This module contains a short routine for locking down the fontSummary
*  data structure -- it should be included in modules that need to use
*  it.  Including it removes the chance that windows will thrash in a new
*  module just to execute this (very small) utility.
*/
  
/*  lockFontSummary
*
*  Lock down the fontSummary data structure.
*/
static LPFONTSUMMARYHDR lockFontSummary(LPDEVICE);
static LPFONTSUMMARYHDR lockFontSummary(lpDevice)
LPDEVICE lpDevice;
{
#ifdef DEBUG_FUNCT
    DB(("Entering LockFontSummary\n"));
#endif
    if (lpDevice->epLPFntSum)
        return ((LPFONTSUMMARYHDR)lpDevice->epLPFntSum);
  
    if (lpDevice->epHFntSum)
    {
        lpDevice->epLPFntSum = GlobalLock(lpDevice->epHFntSum);
    }
  
    #ifdef DEBUG
    if (!lpDevice->epLPFntSum)
    {
        DBMSG(("***FAILED TO LOCK FONTSUMMARY: epHFntSum=%d\n",
        (HANDLE)lpDevice->epHFntSum));
    }
    #endif
  
#ifdef DEBUG_FUNCT
    DB(("Exiting LockFontSummary\n"));
#endif
    return ((LPFONTSUMMARYHDR)lpDevice->epLPFntSum);
}
  
/*  unlockFontSummary
*
*  Unlock the fontSummary struct.
*/
static void unlockFontSummary(LPDEVICE);
static void unlockFontSummary(lpDevice)
LPDEVICE lpDevice;
{
    if (lpDevice->epLPFntSum && lpDevice->epHFntSum)
    {
        GlobalUnlock(lpDevice->epHFntSum);
        lpDevice->epLPFntSum = 0L;
    }
    #ifdef DEBUG
    else
    {
        DBMSG(
        ("unlockFontSummary(): fontSummary already unlocked, lp=%lp, h=%d\n",
        lpDevice->epLPFntSum, (HANDLE)lpDevice->epHFntSum));
    }
    #endif
}
