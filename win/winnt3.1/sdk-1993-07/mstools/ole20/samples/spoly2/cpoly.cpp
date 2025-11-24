/*** 
*cpoly.cpp - Implementation of the CPoly object.
*
*  Copyright (C) 1992, Microsoft Corporation.  All Rights Reserved.
*  Information Contained Herein Is Proprietary and Confidential.
*
*Purpose:
*  This module implements the CPoly class. This class implements a
*  number of methods and properties, and exposes them via IDispatch.
*
*  This module is intended as a sample implementation of the IDispatch
*  interface, and its purpose is to demonstrate how an object can
*  expose methods and properties for programatic and cross-process
*  access via the IDispatch interface.
*
*Implementation Notes:
*
*****************************************************************************/

#include <windows.h>
#include <ole2.h>
#if !defined(WIN32)
#include <olenls.h>
#endif
#include <dispatch.h>

#include "statbar.h"
#include "cenumpt.h"

#include "spoly.h"
#include "cpoint.h"
#include "cpoly.h"


// Adjust for WIN16/WIN32 synchronization
#if defined(WIN32) && !defined(OLE2FINAL)
#define E_FAIL E_UNSPEC
#endif


extern HWND g_hwndClient;

extern CStatBar FAR* g_psb;

extern BOOL g_fExitOnLastRelease;


// our global list of polygons.
//
POLYLINK FAR* g_ppolylink = (POLYLINK FAR*)NULL;

// global count of polygons.
//
int g_cPoly = 0;


CPoly::CPoly()
{
    m_refs = 0;
    m_xorg = 0;
    m_yorg = 0;
    m_width = 0;
    m_cPoints = 0;

    m_red   = 0;
    m_green = 0;
    m_blue  = 0;

    m_ptinfo = NULL;

    m_ppointlink = NULL;
    m_ppointlinkLast = NULL;
}


/***
*CPoly *CPoly::Create(void)
*Purpose:
*  Create an instance of a CPoly object, and add it to the global
*  list of polygons.
*
*Entry:
*  None
*
*Exit:
*  returns a polygon object, NULL the allocation failed.
*
***********************************************************************/
CPoly FAR*
CPoly::Create()
{
    HRESULT hresult;
    CPoly FAR* ppoly;
    ITypeInfo FAR* ptinfo;
    POLYLINK FAR* ppolylink;
extern INTERFACEDATA NEAR g_idataCPoly;


    if((ppolylink = new FAR POLYLINK) == (POLYLINK FAR*)NULL)
      return (CPoly FAR*)NULL;

    if((ppoly = new FAR CPoly()) == (CPoly FAR*)NULL)
      return (CPoly FAR*)NULL;

    ppoly->AddRef();

    // create and attach its TypeInfo
    //
    hresult = CreateDispTypeInfo(&g_idataCPoly, LOCALE_SYSTEM_DEFAULT, &ptinfo);
    if(hresult != NOERROR)
      goto LError0;

    ppoly->m_ptinfo = ptinfo;

    // push the new polygon onto the front of the polygon list.
    //
    ++g_cPoly;

    ppolylink->ppoly = ppoly;

    ppolylink->next = g_ppolylink;
    g_ppolylink = ppolylink;

    SBprintf(g_psb, "#poly = %d", g_cPoly);

    return ppoly;


LError0:;
    ppoly->Release();

    return NULL;
}


//---------------------------------------------------------------------
//                     IUnknown Methods
//---------------------------------------------------------------------


STDMETHODIMP
CPoly::QueryInterface(REFIID riid, void FAR* FAR* ppv)
{
    if(riid != IID_IUnknown)
      if(riid != IID_IDispatch)
        return ResultFromScode(E_NOINTERFACE);

    *ppv = this;
    AddRef();
    return NOERROR;
}


STDMETHODIMP_(ULONG)
CPoly::AddRef()
{
    return ++m_refs;
}


STDMETHODIMP_(ULONG)
CPoly::Release()
{
    POLYLINK FAR* FAR* pppolylink, FAR* ppolylinkDead;

    if(--m_refs == 0){
      Reset(); // release all CPoints

      // remove ourselves from the global list of polygons
      //
      for( pppolylink = &g_ppolylink;
	  *pppolylink != NULL;
	   pppolylink = &(*pppolylink)->next)
      {
	if((*pppolylink)->ppoly == this){
	  ppolylinkDead = *pppolylink;
	  *pppolylink = (*pppolylink)->next;
	  delete ppolylinkDead;
	  break;
	}
      }

      if(m_ptinfo != NULL){
	m_ptinfo->Release();
      }

      --g_cPoly;

#ifndef _MAC
      SBprintf(g_psb, "#poly = %d", g_cPoly);
#endif

      delete this;
      return 0;
    }
    return m_refs;
}


//---------------------------------------------------------------------
//                     IDispatch Methods
//---------------------------------------------------------------------

STDMETHODIMP
CPoly::GetTypeInfoCount(UINT FAR* pctinfo)
{
    // This object has a single *introduced* interface
    //
    *pctinfo = 1;

    return NOERROR;
}


STDMETHODIMP
CPoly::GetTypeInfo(UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
    if(itinfo != 0)
      return ResultFromScode(DISP_E_BADINDEX);

    m_ptinfo->AddRef();
    *pptinfo = m_ptinfo;

    return NOERROR;
}


/***
*HRESULT CPoly::GetIDsOfNames(char**, UINT, LCID, LONG*)
*Purpose:
*  This method translates the given array of names to a corresponding
*  array of DISPIDs.
*
*  This method deferrs to a common implementation shared by
*  both the CPoly and CPoint objects. See the description of
*  'SPolyGetIDsOfNames()' in dispimpl.cpp for more information.
*
*Entry:
*  rgszNames = pointer to an array of names
*  cNames = the number of names in the rgszNames array
*  lcid = the callers locale ID
*
*Exit:
*  return value = HRESULT
*  rgdispid = array of DISPIDs corresponding to the rgszNames array
*    this array will contain -1 for each entry that is not known.
*
***********************************************************************/
STDMETHODIMP
CPoly::GetIDsOfNames(
    REFIID riid,
    char FAR* FAR* rgszNames,
    UINT cNames,
    LCID lcid,
    DISPID FAR* rgdispid)
{
    if(riid != IID_NULL)
      return ResultFromScode(DISP_E_UNKNOWNINTERFACE);

    return DispGetIDsOfNames(m_ptinfo, rgszNames, cNames, rgdispid);
}


/***
*HRESULT CPoly::Invoke(...)
*Purpose:
*  Dispatch a method or property request for objects of type CPoly.
*
*  see the IDispatch document for more information, and a general
*  description of this method.
*
*Entry:
*  dispidMember = the DISPID of the member being requested
*
*  riid = reference to the interface ID of the interface on this object
*    that the requested member belongs to. IID_NULL means to interpret
*    the member as belonging to the implementation defined "default"
*    or "primary" interface.
*
*  lcid = the caller's locale ID
*
*  wFlags = flags indicating the type of access being requested
*
*  pdispparams = pointer to the DISPPARAMS struct containing the
*    requested members arguments (if any) and its named parameter
*    DISPIDs (if any).
*
*Exit:
*  return value = HRESULT
*   see the IDispatch spec for a description of possible success codes.
*
*  pvarResult = pointer to a caller allocated VARIANT containing
*    the members return value (if any).
*
*  pexcepinfo = caller allocated exception info structure, this will
*    be filled in only if an exception was raised that must be passed
*    up through Invoke to an enclosing handler.
*
*  puArgErr = pointer to a caller allocated UINT, that will contain the
*    index of the offending argument if a DISP_E_TYPEMISMATCH error
*    was returned indicating that one of the arguments was of an
*    incorrect type and/or could not be reasonably coerced to a proper
*    type.
*
***********************************************************************/
STDMETHODIMP
CPoly::Invoke(
    DISPID dispidMember,
    REFIID riid,
    LCID lcid,
    WORD wFlags,
    DISPPARAMS FAR* pdispparams,
    VARIANT FAR* pvarResult,
    EXCEPINFO FAR* pexcepinfo,
    UINT FAR* puArgErr)
{
    if(riid != IID_NULL)
      return ResultFromScode(DISP_E_UNKNOWNINTERFACE);

    return DispInvoke(
      this, m_ptinfo,
      dispidMember, wFlags, pdispparams,
      pvarResult, pexcepinfo, puArgErr);
}


//---------------------------------------------------------------------
//                     Introduced Methods
//---------------------------------------------------------------------


/***
*void CPoly::Draw(void)
*Purpose:
*  Draw the polygon, using the current x/y origin and line width
*  properties.
*
*Entry:
*  None
*
*Exit:
*  None
*
***********************************************************************/
void METHODCALLTYPE EXPORT
CPoly::Draw()
{
    POINTLINK FAR* ppointlinkFirst, FAR* ppointlink;

    if((ppointlinkFirst = m_ppointlink) == (POINTLINK FAR*)NULL)
      return;

#ifdef _MAC /* { */

#else /* }{ */

    HDC hdc;
    RECT rect;
    short xorg, yorg;
    HPEN hpen, hpenOld;

    GetClientRect(g_hwndClient, &rect);
    xorg = m_xorg + (short) rect.left;
    yorg = m_yorg + (short) rect.top;

    hdc = GetDC(g_hwndClient);
    hpen = CreatePen(PS_SOLID, m_width, RGB(m_red, m_green, m_blue));
    hpenOld = SelectObject(hdc, hpen);

#ifdef WIN32
    MoveToEx(hdc,
      xorg + ppointlinkFirst->ppoint->m_x,
      yorg + ppointlinkFirst->ppoint->m_y, NULL);
#else
    MoveTo(hdc,
      xorg + ppointlinkFirst->ppoint->m_x,
      yorg + ppointlinkFirst->ppoint->m_y);
#endif

    for(ppointlink = ppointlinkFirst->next;
	ppointlink != (POINTLINK FAR*)NULL;
	ppointlink = ppointlink->next)
    {
      LineTo(hdc,
	xorg + ppointlink->ppoint->m_x,
	yorg + ppointlink->ppoint->m_y);
    }

    LineTo(hdc,
      xorg + ppointlinkFirst->ppoint->m_x,
      yorg + ppointlinkFirst->ppoint->m_y);

    SelectObject(hdc, hpenOld);
    DeleteObject(hpen);

    ReleaseDC(g_hwndClient, hdc);

#endif /* } */
}


/***
*void CPoly::Reset(void)
*Purpose:
*  Release all points referenced by this poly.
*
*Entry:
*  None
*
*Exit:
*  None
*
***********************************************************************/
void METHODCALLTYPE EXPORT
CPoly::Reset()
{
    POINTLINK FAR* ppointlink, FAR* ppointlinkNext;

    for(ppointlink = m_ppointlink;
	ppointlink != (POINTLINK FAR*)NULL;
	ppointlink = ppointlinkNext)
    {
      ppointlinkNext = ppointlink->next;
      ppointlink->ppoint->Release();
      delete ppointlink;
    }

    m_cPoints = 0;
    m_ppointlink = NULL;
    m_ppointlinkLast = NULL;
}


/***
*HRESULT CPoly::AddPoint(short, short)
*Purpose:
*  Add a CPoint with the given coordinates to the end of our current
*  list of points.
*
*Entry:
*  x,y = the x and y coordinates of the new point.
*
*Exit:
*  return value = HRESULT
*
***********************************************************************/
HRESULT METHODCALLTYPE
CPoly::AddPoint(short x, short y)
{
    CPoint FAR* ppoint;
    POINTLINK FAR* ppointlink;

    ppoint = CPoint::Create();
    if(ppoint == (CPoint FAR*)NULL)
      return ResultFromScode(E_OUTOFMEMORY);

    ppoint->SetX(x);
    ppoint->SetY(y);

    ppointlink = new FAR POINTLINK;
    if(ppointlink == (POINTLINK FAR*)NULL){
      delete ppoint;
      return ResultFromScode(E_OUTOFMEMORY);
    }

    ppointlink->ppoint = ppoint;
    ppointlink->next = (POINTLINK FAR*)NULL;

    if(m_ppointlinkLast == (POINTLINK FAR*)NULL){
      m_ppointlink = m_ppointlinkLast = ppointlink;
    }else{
      m_ppointlinkLast->next = ppointlink;
      m_ppointlinkLast = ppointlink;
    }

    ++m_cPoints;

    return NOERROR;
}


/***
*IUnknown FAR* CPoly::EnumPoints(void);
*Purpose:
*  Return and enumerator for the points in this polygon.
*
*Entry:
*  None
*
*Exit:
*  return value = HRESULT
*
*  *ppenum = pointer to an IEnumVARIANT for the points in this polygon
*
***********************************************************************/
IUnknown FAR* METHODCALLTYPE EXPORT
CPoly::EnumPoints()
{
    UINT i;
    HRESULT hresult;
    VARIANT var;
    SAFEARRAY FAR* psa;
    IUnknown FAR* punk;
    CEnumPoint FAR* penum;
    POINTLINK FAR* ppointlink;
    SAFEARRAYBOUND rgsabound[1];

    rgsabound[0].lLbound = 0;
    rgsabound[0].cElements = m_cPoints;

    psa = SafeArrayCreate(VT_VARIANT, 1, rgsabound);
    if(psa == NULL){
      hresult = ResultFromScode(E_OUTOFMEMORY);
      goto LError0;
    }

    ppointlink = m_ppointlink;
    for(i = 0; i < m_cPoints; ++i){
      LONG ix[1];

      if(ppointlink == NULL){
        // this indicates an internal consistency error.
	// (this test should probably be an assertion)
        hresult = ResultFromScode(E_FAIL);
        goto LError1;
      }

      V_VT(&var) = VT_DISPATCH;
      hresult = ppointlink->ppoint->QueryInterface(
	IID_IDispatch, (void FAR* FAR*)&V_DISPATCH(&var));
      if(hresult != NOERROR)
        goto LError1;

      ix[0] = i;
      SafeArrayPutElement(psa, ix, &var);

      ppointlink = ppointlink->next;
    }

    hresult = CEnumPoint::Create(psa, &penum);
    if(hresult != NOERROR)
      goto LError1;

    // ownership of the array has passed to the enumerator
    psa = NULL;

    hresult = penum->QueryInterface(IID_IUnknown, (void FAR* FAR*)&punk);
    if(hresult != NOERROR)
      goto LError2;

    penum->Release();

    return punk;

LError2:;
    penum->Release();

LError1:;
    // destroy the array if we were not successful creating the enumerator.
    if(psa != NULL)
      SafeArrayDestroy(psa);

LError0:;
    return NULL;
}


short METHODCALLTYPE EXPORT
CPoly::GetXOrigin()
{
    return m_xorg;
}

void METHODCALLTYPE EXPORT
CPoly::SetXOrigin(short x)
{
    m_xorg = x;
}

short METHODCALLTYPE EXPORT
CPoly::GetYOrigin()
{
    return m_yorg;
}

void METHODCALLTYPE EXPORT
CPoly::SetYOrigin(short y)
{
    m_yorg = y;
}

short METHODCALLTYPE EXPORT
CPoly::GetWidth()
{
    return m_width;
}


void METHODCALLTYPE EXPORT
CPoly::SetWidth(short width)
{
    m_width = width;
}

short METHODCALLTYPE EXPORT
CPoly::get_red()
{
    return m_red;
}

void METHODCALLTYPE EXPORT
CPoly::set_red(short red)
{
    m_red = red;
}

short METHODCALLTYPE EXPORT
CPoly::get_green()
{
    return m_green;
}

void METHODCALLTYPE EXPORT
CPoly::set_green(short green)
{
    m_green = green;
}

short METHODCALLTYPE EXPORT
CPoly::get_blue()
{
    return m_blue;
}

void METHODCALLTYPE EXPORT
CPoly::set_blue(short blue)
{
    m_blue = blue;
}


/***
*void CPoly::Dump(void)
*Purpose:
*  Output a debug dump of this instance.
*
*Entry:
*  None
*
*Exit:
*  None
*
***********************************************************************/
void METHODCALLTYPE EXPORT
CPoly::Dump()
{
#ifdef _MAC

    // REVIEW: implement for the mac

#else

    char buffer[80];
    POINTLINK FAR* ppointlink;

    wsprintf(buffer, "CPoly(0x%x) =\n", (int)this);
    OutputDebugString(buffer);

    wsprintf(buffer,
      "    xorg = %d, yorg = %d, width = %d, rgb = {%d,%d,%d}\n    points = ",
      m_xorg, m_yorg, m_width,
      get_red(),
      get_green(),
      get_blue());

    OutputDebugString(buffer);

    for(ppointlink = m_ppointlink;
	ppointlink != (POINTLINK FAR*)NULL;
	ppointlink = ppointlink->next)
    {
      wsprintf(buffer, "{%d,%d}",
        ppointlink->ppoint->GetX(),
        ppointlink->ppoint->GetY());
      OutputDebugString(buffer);

      wsprintf(buffer, " ");
      OutputDebugString(buffer);
    }
    wsprintf(buffer, "\n");
    OutputDebugString(buffer);

#endif
}

/***
*void CPoly::PolyDraw(void)
*Purpose:
*  Draw all polygons.
*
*Entry:
*  None
*
*Exit:
*  None
*
***********************************************************************/
void
CPoly::PolyDraw()
{
    POLYLINK FAR* polylink;

    for(polylink = g_ppolylink;
	polylink != (POLYLINK FAR*)NULL;
	polylink = polylink->next)
    {
      polylink->ppoly->Draw();
    }
}


/***
*void PolyTerm(void)
*Purpose:
*  Release all polygons.
*
*Entry:
*  None
*
*Exit:
*  None
*
***********************************************************************/
void
CPoly::PolyTerm()
{
    POLYLINK FAR* ppolylink;
    POLYLINK FAR* ppolylinkNext;

    for(ppolylink = g_ppolylink;
	ppolylink != (POLYLINK FAR*)NULL;
	ppolylink = ppolylinkNext)
    {
      ppolylinkNext = ppolylink->next;
      ppolylink->ppoly->Release();
      delete ppolylink;
    }
}


/***
*void PolyDump(void)
*Purpose:
*  Invoke the debug Dump() method on all polygons were currently
*  holding on to.
*
*Entry:
*  None
*
*Exit:
*  None
*
***********************************************************************/
void
CPoly::PolyDump()
{
    POLYLINK FAR* ppolylink;

    if(g_ppolylink == (POLYLINK FAR*)NULL){
      OutputDebugString("\t(none)\n");
      return;
    }

    for(ppolylink = g_ppolylink;
	ppolylink != (POLYLINK FAR*)NULL;
	ppolylink = ppolylink->next)
    {
      ppolylink->ppoly->Dump();
    }
}
