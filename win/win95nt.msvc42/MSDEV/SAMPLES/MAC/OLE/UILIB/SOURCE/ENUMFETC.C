/*************************************************************************
**
**    OLE 2 Utility Code
**
**    enumfetc.c
**
**    This file contains a standard implementation of IEnumFormatEtc
**    interface.
**    This file is part of the OLE 2.0 User Interface support library.
**
**    (c) Copyright Microsoft Corp. 1990 - 1994 All Rights Reserved
**
*************************************************************************/


#if defined(__MWERKS__) && !defined(__powerc)
#pragma pointers_in_D0
#endif

#if defined(USEHEADER)
#include "OleHeaders.h"
#endif

#if defined(_MSC_VER) && defined(__powerc)
#include <msvcmac.h>
#endif

#define STRICT  1

#include <ole2.h>

#include "ole2ui.h"
#include "uidebug.h"

OLEDBGDATA

typedef struct tagOleStdEnumFmtEtc {
  IEnumFORMATETCVtbl * lpVtbl;
  unsigned long m_dwRefs;      /* referance count */
  unsigned short m_wIndex;       /* current index in list */
  unsigned short m_wCount;       /* how many items in list */
  LPFORMATETC m_lpEtc;  /* list of formatetc */
} OLESTDENUMFMTETC, * LPOLESTDENUMFMTETC;

void  OleStdEnumFmtEtc_Destroy(LPOLESTDENUMFMTETC pEtc);

STDMETHODIMP OleStdEnumFmtEtc_QueryInterface(
        LPENUMFORMATETC lpThis, REFIID riid, void* * ppobj);
STDMETHODIMP_(unsigned long)  OleStdEnumFmtEtc_AddRef(LPENUMFORMATETC lpThis);
STDMETHODIMP_(unsigned long)  OleStdEnumFmtEtc_Release(LPENUMFORMATETC lpThis);
STDMETHODIMP  OleStdEnumFmtEtc_Next(LPENUMFORMATETC lpThis, unsigned long celt,
                                  LPFORMATETC rgelt, unsigned long * pceltFetched);
STDMETHODIMP  OleStdEnumFmtEtc_Skip(LPENUMFORMATETC lpThis, unsigned long celt);
STDMETHODIMP  OleStdEnumFmtEtc_Reset(LPENUMFORMATETC lpThis);
STDMETHODIMP  OleStdEnumFmtEtc_Clone(LPENUMFORMATETC lpThis,
                                     LPENUMFORMATETC * ppenum);


static IEnumFORMATETCVtbl gEnumFORMATETCVtbl;

/////////////////////////////////////////////////////////////////////////////

#ifndef _MSC_VER
#pragma segment EnumFmtEtcSeg
#else
#pragma code_seg("EnumFmtEtcSeg", "SWAPPABLE")
#endif
STDAPI_(void) EnumFORMATETCInitInterface(void)
{
	IEnumFORMATETCVtbl* p;

	p = &gEnumFORMATETCVtbl;
	
    p->QueryInterface 	= OleStdEnumFmtEtc_QueryInterface;
    p->AddRef 			= OleStdEnumFmtEtc_AddRef;
    p->Release			= OleStdEnumFmtEtc_Release;
    p->Next				= OleStdEnumFmtEtc_Next;
    p->Skip				= OleStdEnumFmtEtc_Skip;
    p->Reset			= OleStdEnumFmtEtc_Reset;
    p->Clone			= OleStdEnumFmtEtc_Clone;
}


#ifndef _MSC_VER
#pragma segment EnumFmtEtcSeg
#else
#pragma code_seg("EnumFmtEtcSeg", "SWAPPABLE")
#endif
STDAPI_(LPENUMFORMATETC)
  OleStdEnumFmtEtc_Create(unsigned short wCount, LPFORMATETC lpEtc)
//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
{
  LPMALLOC lpMalloc=NULL;
  LPOLESTDENUMFMTETC lpEF=NULL;
  unsigned long dwSize;
  unsigned short i;
  HRESULT hRes;

  hRes = CoGetMalloc(MEMCTX_TASK, &lpMalloc);
  if (hRes != NOERROR) {
    return NULL;
  }

  lpEF = (LPOLESTDENUMFMTETC)lpMalloc->lpVtbl->Alloc(lpMalloc,
                                                 sizeof(OLESTDENUMFMTETC));
  if (lpEF == NULL) {
    goto errReturn;
  }

  lpEF->lpVtbl = &gEnumFORMATETCVtbl;
  lpEF->m_dwRefs = 1;
  lpEF->m_wCount = wCount;
  lpEF->m_wIndex = 0;

  dwSize = sizeof(FORMATETC) * lpEF->m_wCount;

  lpEF->m_lpEtc = (LPFORMATETC)lpMalloc->lpVtbl->Alloc(lpMalloc, dwSize);
  if (lpEF->m_lpEtc == NULL) {
    goto errReturn;
  }

  for (i=0; i<wCount; i++) {
    OleStdCopyFormatEtc(
            (LPFORMATETC)&(lpEF->m_lpEtc[i]), (LPFORMATETC)&(lpEtc[i]));
  }

  return (LPENUMFORMATETC)lpEF;

errReturn:
  if (lpEF != NULL) {
    lpMalloc->lpVtbl->Free(lpMalloc, lpEF);
  }

  if (lpMalloc != NULL) {
    lpMalloc->lpVtbl->Release(lpMalloc);
  }

  return NULL;

} /* OleStdEnumFmtEtc_Create()
   */


#ifndef _MSC_VER
#pragma segment EnumFmtEtcSeg
#else
#pragma code_seg("EnumFmtEtcSeg", "SWAPPABLE")
#endif
void
  OleStdEnumFmtEtc_Destroy(LPOLESTDENUMFMTETC lpEF)
//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
{
    LPMALLOC lpMalloc=NULL;
    unsigned short i;

    if (lpEF != NULL) {

        if (CoGetMalloc(MEMCTX_TASK, &lpMalloc) == NOERROR) {

            /* OLE2NOTE: we MUST free any memory that was allocated for
            **    TARGETDEVICES contained within the FORMATETC elements.
            */
            for (i=0; i<lpEF->m_wCount; i++) {
                OleStdFree(lpEF->m_lpEtc[i].ptd);
            }

            if (lpEF->m_lpEtc != NULL) {
                lpMalloc->lpVtbl->Free(lpMalloc, lpEF->m_lpEtc);
            }

            lpMalloc->lpVtbl->Free(lpMalloc, lpEF);
            lpMalloc->lpVtbl->Release(lpMalloc);
        }
    }
} /* OleStdEnumFmtEtc_Destroy()
   */


#ifndef _MSC_VER
#pragma segment EnumFmtEtcSeg
#else
#pragma code_seg("EnumFmtEtcSeg", "SWAPPABLE")
#endif
STDMETHODIMP
  OleStdEnumFmtEtc_QueryInterface(
                LPENUMFORMATETC lpThis, REFIID riid, void* * ppobj)
//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
{
  LPOLESTDENUMFMTETC lpEF;

	lpEF = (LPOLESTDENUMFMTETC)lpThis;
	*ppobj = NULL;
	
	if (IsEqualIID(riid,&IID_IUnknown) || IsEqualIID(riid,&IID_IEnumFORMATETC)) {
    	*ppobj = (void*)lpEF;
    	OleStdEnumFmtEtc_AddRef(lpThis);
    	return NOERROR;
  	}

	*ppobj = NULL;
	return ResultFromScode(E_NOINTERFACE);
	
} /* OleStdEnumFmtEtc_QueryInterface()
   */


#ifndef _MSC_VER
#pragma segment EnumFmtEtcSeg
#else
#pragma code_seg("EnumFmtEtcSeg", "SWAPPABLE")
#endif
STDMETHODIMP_(unsigned long)
  OleStdEnumFmtEtc_AddRef(LPENUMFORMATETC lpThis)
//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
{
	LPOLESTDENUMFMTETC lpEF;
	unsigned long	uRet;
	
	lpEF = (LPOLESTDENUMFMTETC)lpThis;
	
	uRet = lpEF->m_dwRefs++;

	return uRet;
	
} /* OleStdEnumFmtEtc_AddRef()
   */


#ifndef _MSC_VER
#pragma segment EnumFmtEtcSeg
#else
#pragma code_seg("EnumFmtEtcSeg", "SWAPPABLE")
#endif
STDMETHODIMP_(unsigned long)
  OleStdEnumFmtEtc_Release(LPENUMFORMATETC lpThis)
//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
{
  LPOLESTDENUMFMTETC lpEF;
  unsigned long dwRefs;

  lpEF = (LPOLESTDENUMFMTETC)lpThis;
  dwRefs = --lpEF->m_dwRefs;

  if (dwRefs == 0)
    OleStdEnumFmtEtc_Destroy(lpEF);

  return dwRefs;

} /* OleStdEnumFmtEtc_Release()
   */


#ifndef _MSC_VER
#pragma segment EnumFmtEtcSeg
#else
#pragma code_seg("EnumFmtEtcSeg", "SWAPPABLE")
#endif
STDMETHODIMP
  OleStdEnumFmtEtc_Next(LPENUMFORMATETC lpThis, unsigned long celt, LPFORMATETC rgelt,
                      unsigned long * pceltFetched)
//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
{
  LPOLESTDENUMFMTETC lpEF;
  unsigned long i;
  unsigned short wOffset;
  HRESULT hrErr;

  lpEF = (LPOLESTDENUMFMTETC)lpThis;
  i = 0;

  if (rgelt == NULL) {
  	hrErr = ResultFromScode(E_INVALIDARG);
    goto exit;
  }

  while (i < celt) {
    wOffset = lpEF->m_wIndex + (unsigned short)i;

    if (wOffset < lpEF->m_wCount) {
      OleStdCopyFormatEtc(
            (LPFORMATETC)&(rgelt[i]), (LPFORMATETC)&(lpEF->m_lpEtc[wOffset]));
      lpEF->m_wIndex ++;
      i++;
    }else{
      break;
    }
  }

  if (pceltFetched != NULL) {
    *pceltFetched = i;
  }

  if (i != celt) {
    hrErr = ResultFromScode(S_FALSE);
    goto exit;
  }

	hrErr = NOERROR;
	
exit:

  return hrErr;

} /* OleStdEnumFmtEtc_Next()
   */


#ifndef _MSC_VER
#pragma segment EnumFmtEtcSeg
#else
#pragma code_seg("EnumFmtEtcSeg", "SWAPPABLE")
#endif
STDMETHODIMP
  OleStdEnumFmtEtc_Skip(LPENUMFORMATETC lpThis, unsigned long celt)
//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
{
  LPOLESTDENUMFMTETC lpEF;
  unsigned long i;
  unsigned short wOffset;
  HRESULT hrErr;

  lpEF = (LPOLESTDENUMFMTETC)lpThis;
  i = 0;

  while (i < celt) {
    wOffset = lpEF->m_wIndex + (unsigned short)i;

    if (wOffset < lpEF->m_wCount) {
      lpEF->m_wIndex ++;
      i++;
    }else{
      break;
    }
  }

  if (i != celt) {
    hrErr = ResultFromScode(S_FALSE);
    goto exit;
  }

  hrErr = NOERROR;

exit:

  return hrErr;

} /* OleStdEnumFmtEtc_Skip()
   */


#ifndef _MSC_VER
#pragma segment EnumFmtEtcSeg
#else
#pragma code_seg("EnumFmtEtcSeg", "SWAPPABLE")
#endif
STDMETHODIMP
  OleStdEnumFmtEtc_Reset(LPENUMFORMATETC lpThis)
//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
{
  LPOLESTDENUMFMTETC lpEF;

  lpEF = (LPOLESTDENUMFMTETC)lpThis;
  lpEF->m_wIndex = 0;

  return NOERROR;

} /* OleStdEnumFmtEtc_Reset()
   */


#ifndef _MSC_VER
#pragma segment EnumFmtEtcSeg
#else
#pragma code_seg("EnumFmtEtcSeg", "SWAPPABLE")
#endif
STDMETHODIMP
  OleStdEnumFmtEtc_Clone(LPENUMFORMATETC lpThis, LPENUMFORMATETC * ppenum)
//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
{
  LPOLESTDENUMFMTETC lpEF;
  HRESULT	hrErr;

  lpEF = (LPOLESTDENUMFMTETC)lpThis;

  if (ppenum == NULL) {
    hrErr = ResultFromScode(E_INVALIDARG);
    goto exit;
  }

  *ppenum = OleStdEnumFmtEtc_Create(lpEF->m_wCount, lpEF->m_lpEtc);

  hrErr = (*ppenum != NULL ? NOERROR : ResultFromScode(E_OUTOFMEMORY));

exit:

	return hrErr;

} /* OleStdEnumFmtEtc_Clone()
   */

