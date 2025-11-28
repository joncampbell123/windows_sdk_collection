//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       Ole2Ansi.h
//
//  Contents:   Internal include file for OLE2ANSI Wrappers.
//
//  History:    01-Jan-94   v-kentc     Created.
//
//---------------------------------------------------------------------------

#ifndef _OLE2ANSI_H_
#define _OLE2ANSI_H_

#pragma warning(disable : 4505)

#include <windows.h>
#include <memory.h>
#include <malloc.h>
#include <string.h>
#include <ole2.h>
#include "olectl.h"

//
//  For each interface that is wrapper, define a simple numeric identifier
//  that can be used more efficiently than the long 128 bit REFIID.
//
#define ID_NULL                         0
#define ID_IUnknown                     1
#define ID_IClassFactory                2
#define ID_IMarshal                     3
#define ID_IEnumString                  4
#define ID_IEnumSTATSTG                 5
#define ID_ILockBytes                   6
#define ID_IStream                      7
#define ID_IStorage                     8
#define ID_IRootStorage                 9
#define ID_IDropTarget                 10
#define ID_IPersistStorage             11
#define ID_IPersistFile                12
#define ID_IBindCtx                    13
#define ID_IMoniker                    14
#define ID_IRunningObjectTable         15
#define ID_IEnumMoniker                16
#define ID_IEnumSTATDATA               17
#define ID_IEnumFORMATETC              18
#define ID_IDataObject                 19
#define ID_IViewObject2                20
#define ID_IAdviseSink2                21
#define ID_IDataAdviseHolder           22
#define ID_IOleCache2                  23
#define ID_IOleCacheControl            24
#define ID_IEnumOLEVERB                25
#define ID_IOleObject                  26
#define ID_IOleClientSite              27
#define ID_IRunnableObject             28
#define ID_IOleItemContainer           29
#define ID_IOleAdviseHolder            30
#define ID_IOleLink                    31
#define ID_IOleInPlaceObject           32
#define ID_IOleInPlaceActiveObject     33
#define ID_IOleInPlaceFrame            34
#define ID_IOleInPlaceSite             35
#define ID_ITypeLib                    36
#define ID_ITypeInfo                   37
#define ID_ITypeComp                   38
#define ID_ICreateTypeLib              39
#define ID_ICreateTypeInfo             40
#define ID_IEnumVARIANT                41
#define ID_IDispatch                   42
#define ID_IStdMarshalInfo             43
#define ID_IPersistStream          44
#define ID_IPersist                45
#define ID_IOleWindow              46
#define ID_IOleInPlaceUIWindow         47
#define ID_IParseDisplayName           48
#define ID_IOleContainer           49
#define ID_IAdviseSink             50
#define ID_IViewObject             51
#define ID_IOleCache               52
#define ID_IBoundObject                            53
#define ID_IBoundObjectSite                        54
#define ID_IPropertyNotifySink             55
#define ID_IProvideClassInfo               56
#define ID_IConnectionPointContainer   57
#define ID_IEnumConnectionPoints           58
#define ID_IConnectionPoint                        59
#define ID_IEnumConnections                        60
#define ID_IOleControl                             61
#define ID_IOleControlSite                         62
#define ID_ISimpleFrameSite                        63
#define ID_IPersistStreamInit              64
#define ID_IClassFactory2                          65
#define ID_ISpecifyPropertyPages           66
#define ID_IPerPropertyBrowsing            67
#define ID_IPropertyPageSite               68
#define ID_IPropertyPage                           69
#define ID_IPropertyPage2                          70
#define ID_IFont                                           71
#define ID_IFontDisp                               72
#define ID_IPicture                                        73
#define ID_IPictureDisp                            74
#ifndef NOERRORINFO
#define ID_IErrorInfo                  75
#define ID_ICreateErrorInfo            76
#define ID_SIZE                77
#else //!NOERRORINFO
#define ID_SIZE                75
#endif //!NOERRORINFO

typedef USHORT IDINTERFACE;
#define ID_ANSIINTERFACE           0x1000
#define ID_MASK       (~ID_ANSIINTERFACE)


#include "Globals.h"
#include "Debug.h"
#include "Trace.h"
#include "Thunk.h"
#include "AnsiComp.h"
#include "WideComp.h"
#include "AnsiStor.h"
#include "WideStor.h"
#include "AnsiMoni.h"
#include "WideMoni.h"
#include "AnsiDvOb.h"
#include "WideDvOb.h"
#include "AnsiOle.h"
#include "WideOle.h"
#include "AnsiDisp.h"
#include "WideDisp.h"
#include "Convert.h"
#include "Wrapper.h"

#include "AnsiCtl.h"
#include "WideCtl.h"
#include "ConvCtl.h"
#include "WrapCtl.h"

#define unreference(x)  (x)

#ifdef _DEBUG_HOOKS
	#pragma code_seg(".orpc")
#endif

#endif
