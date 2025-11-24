/*** 
*misc.cpp
*
*  Copyright (C) 1992-93, Microsoft Corporation.  All Rights Reserved.
*
*Purpose:
*  This module contains a the of IDispatch related helper(s) that
*  are common to both the CPoly and CPoint implementations of IDispatch.
*
*Implementation Notes:
*
*****************************************************************************/

#include "spoly.h"
#include "cpoint.h"
#include "cpoly.h"

#include <stdio.h>

#if _MAC
# include <string.h>
# include <ctype.h>
#endif

int g_fExitOnLastRelease = 0;

unsigned long g_dwPolyCF = 0;
unsigned long g_dwPointCF = 0;

IClassFactory FAR* g_ppolyCF = NULL;
IClassFactory FAR* g_ppointCF = NULL;


void
Assert(int fCond, char FAR* file, int line, char FAR* message)
{
    char * fmt;
    char buf[128];

    if(fCond)
      return;

    fmt = (message == NULL)
      ? "Assertion failed: %s(%d)"
      : "Assertion failed: %s(%d) '%s'";
    sprintf(buf, fmt, file, line, message);

#ifdef _MAC
    DebugStr(c2pstr(buf));
#else
    OutputDebugString(buf);
    DebugBreak();
#endif
}


#ifdef WIN32

//---------------------------------------------------------------------
//           ASCII NLS Wrapper Functions (for Win32)
//---------------------------------------------------------------------

static int WINAPI
CompareStringA(
    LCID lcid,
    DWORD dwFlags,
    LPSTR lpStr1, int cch1,
    LPSTR lpStr2, int cch2)
{
    WCHAR lpwStr1[32], lpwStr2[32];
	
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpStr1, cch1, lpwStr1, 32);
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpStr2, cch2, lpwStr2, 32);
    return(CompareStringW(lcid, dwFlags, lpwStr1, cch1, lpwStr2, cch2));
}

#endif


#ifdef _MAC
pascal int stricmp(char *first, char *last)
{
    unsigned short f, l;

    do{
	f = tolower(*first++);
	l = tolower(*last++);
    }while(f && f == l);

    return f - l;
}
#endif


/***
*HRESULT InitOle(void)
*Purpose:
*  Initialize Ole, and register our class factories.
*
*Entry:
*  None
*
*Exit:
*  None
*
***********************************************************************/
STDAPI
InitOle()
{
    HRESULT hresult;

    if((hresult = OleInitialize(NULL)) != NOERROR)
      goto LError0;

    // Register the CPoint Class Factory
    //
    if((g_ppointCF = CPointCF::Create()) == NULL){
      hresult = ResultFromScode(E_OUTOFMEMORY);
      goto LError1;
    }

    hresult = CoRegisterClassObject(
      CLSID_CPoint,
      g_ppointCF,
      CLSCTX_LOCAL_SERVER,
      REGCLS_MULTIPLEUSE,
      &g_dwPointCF);
    if(hresult != NOERROR)
      goto LError1;

    // Register the CPoly Class Factory.
    //
    if((g_ppolyCF = CPolyCF::Create()) == NULL){
      hresult = ResultFromScode(E_OUTOFMEMORY);
      goto LError1;
    }

    hresult = CoRegisterClassObject(
      CLSID_CPoly,
      g_ppolyCF,
      CLSCTX_LOCAL_SERVER,
      REGCLS_MULTIPLEUSE,
      &g_dwPolyCF);
    if(hresult != NOERROR)
      goto LError1;

    g_ppolyCF->Release();

    g_ppointCF->Release();

    return NOERROR;


LError1:;
    if(g_ppolyCF != NULL)
      g_ppolyCF->Release();

    if(g_ppointCF != NULL)
      g_ppointCF->Release();

    UninitOle();

LError0:;
    return hresult;
}


STDAPI
UninitOle()
{
    // Tell Ole to release our class factories.
    //
    if(g_dwPointCF != 0L)
      CoRevokeClassObject(g_dwPointCF);

    if(g_dwPolyCF != 0L)
      CoRevokeClassObject(g_dwPolyCF);

    OleUninitialize();

    return NOERROR;
}


/***
*HRESULT SPolyGetIDsOfNames(MEMBERDESC*, unsigned int, char**, unsigned int, LCID, long*)
*Purpose:
*  This is the table driven implementation of IDispatch::GetIDsOfNames
*  deferred to by both the CPoint and CPoly objects.
*
*Entry:
*  rgmd = pointer to an array of method descriptors
*  cMethods = number of elements in the array of method descriptors
*  rgszNames = pointer to an array of names
*  cNames = the number of names in the rgszNames array
*  lcid = the callers locale ID
*
*Exit:
*  return value = HRESULT
*  rgdispid = array of name IDs corresponding to the rgszNames array
*    this array will contain -1 for each entry that is not known.
*
***********************************************************************/
STDAPI
SPolyGetIDsOfNames(
    MEMBERDESC FAR* rgmd,
    unsigned int cMethods,
    char FAR* FAR* rgszNames,
    unsigned int cNames,
    LCID lcid,
    DISPID FAR* rgdispid)
{
    HRESULT hresult;
    PARAMDESC FAR* ppd;
    MEMBERDESC FAR* pmd;
    unsigned int iName, iTry, cParams;

    hresult = NOERROR;

    // first lookup the member name.
    //
    for(pmd = rgmd;; ++pmd){
      if(pmd == &rgmd[cMethods])
        goto LMemberNotFound;

#ifdef _MAC
      if(!stricmp(rgszNames[0], pmd->szName))
#else
      if(CompareStringA(
     	 lcid, NORM_IGNORECASE, rgszNames[0], -1, pmd->szName, -1) == 2)
#endif
      {
	rgdispid[0] = pmd->id;
	break;
      }
    }

    // Lookup the named parameters, if there are any.
    if(cNames > 1){
      cParams = pmd->cParams;
      for(iName = 1; iName < cNames; ++iName){
	for(iTry = 0;; ++iTry){
	  if(iTry == cParams){
	    hresult = ResultFromScode(DISP_E_UNKNOWNNAME);
	    rgdispid[iName] = -1;
	    break;
	  }
	  ppd = &pmd->rgpd[iTry];

#ifdef _MAC
          if(!stricmp(rgszNames[iName], ppd->szName))
#else
	  if(CompareStringA(
	     lcid, NORM_IGNORECASE, rgszNames[iName], -1, ppd->szName, -1) == 2)
#endif
	  {
	    // The DISPID for a named parameter is defined to be its
	    // zero based positional index.  This routine assumes that
	    // that PARAMDESC array was declared in correct textual order.
	    //
	    rgdispid[iName] = iTry-1;
	    break;
	  }
        }
      }
    }

    return hresult;

LMemberNotFound:;
    // If the member name is unknown, everything is unknown.
    for(iName = 0; iName < cNames; ++iName)
      rgdispid[iName] = -1;
    return ResultFromScode(DISP_E_UNKNOWNNAME);
}

