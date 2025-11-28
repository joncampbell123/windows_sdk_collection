//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       WrapCtl.cpp
//
//  Contents:   ANSI Wrappers for OLE Control Interfaces and APIs.
//
//  History:    01-Jun-94   johnels     Created.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"

#include <initguid.h>
#include "olectlid.h"

#pragma warning(disable: 4243)


IDINTERFACE WrapTranslateControlIID(REFIID riid)
{
	IDINTERFACE idRef = ID_NULL;

	switch (*(ULONG*)&riid)
	{
	case 0x9BFBBC02:
		if (IsEqualIID(riid, IID_IPropertyNotifySink))
			idRef = ID_IPropertyNotifySink;
		break;

	case 0xB196B28F:
		if (IsEqualIID(riid, IID_IClassFactory2))
			idRef = ID_IClassFactory2;
		break;

	case 0xB196B283:
		if (IsEqualIID(riid, IID_IProvideClassInfo))
			idRef = ID_IProvideClassInfo;
		break;

	case 0xB196B284:
		if (IsEqualIID(riid, IID_IConnectionPointContainer))
			idRef = ID_IConnectionPointContainer;
		break;

	case 0xB196B285:
		if (IsEqualIID(riid, IID_IEnumConnectionPoints))
			idRef = ID_IEnumConnectionPoints;
		break;

	case 0xB196B286:
		if (IsEqualIID(riid, IID_IConnectionPoint))
			idRef = ID_IConnectionPoint;
		break;

	case 0xB196B287:
		if (IsEqualIID(riid, IID_IEnumConnections))
			idRef = ID_IEnumConnections;
		break;

	case 0xB196B288:
		if (IsEqualIID(riid, IID_IOleControl))
			idRef = ID_IOleControl;
		break;

	case 0xB196B289:
		if (IsEqualIID(riid, IID_IOleControlSite))
			idRef = ID_IOleControlSite;
		break;

	case 0x742B0E01:
		if (IsEqualIID(riid, IID_ISimpleFrameSite))
			idRef = ID_ISimpleFrameSite;
		break;

	case 0x7FD52380:
		if (IsEqualIID(riid, IID_IPersistStreamInit))
			idRef = ID_IPersistStreamInit;
		break;

	case 0xB196B28B:
		if (IsEqualIID(riid, IID_ISpecifyPropertyPages))
			idRef = ID_ISpecifyPropertyPages;
		break;

	case 0x376BD3AA:
		if (IsEqualIID(riid, IID_IPerPropertyBrowsing))
			idRef = ID_IPerPropertyBrowsing;
		break;

	case 0xB196B28C:
		if (IsEqualIID(riid, IID_IPropertyPageSite))
			idRef = ID_IPropertyPageSite;
		break;

	case 0xB196B28D:
		if (IsEqualIID(riid, IID_IPropertyPage))
			idRef = ID_IPropertyPage;
		break;

	case 0x01E44665:
		if (IsEqualIID(riid, IID_IPropertyPage2))
			idRef = ID_IPropertyPage2;
		break;

	case 0xBEF6E002:
		if (IsEqualIID(riid, IID_IFont))
			idRef = ID_IFont;
		break;

	case 0xBEF6E003:
		if (IsEqualIID(riid, IID_IFontDisp))
			idRef = ID_IFontDisp;
		break;

	case 0x7BF80980:
		if (IsEqualIID(riid, IID_IPicture))
			idRef = ID_IPicture;
		break;

	case 0x7Bf80981:
		if (IsEqualIID(riid, IID_IPictureDisp))
			idRef = ID_IPictureDisp;
		break;
	}

	return idRef;
}


CInterface* WrapAnyControlAFromW(LPUNKNOWN pObjOuter, IDINTERFACE idRef, LPUNKNOWN pObj)
{
	CInterface* pInterface = NULL;

	switch (idRef)
	{
	case ID_IPropertyNotifySink:
		pInterface = (CInterface*)new CPropertyNotifySinkA(pObjOuter, (IPropertyNotifySink *)pObj);
		break;

	case ID_IProvideClassInfo:
		pInterface = (CInterface*)new CProvideClassInfoA(pObjOuter, (IProvideClassInfo *)pObj);
		break;

	case ID_IConnectionPointContainer:
		pInterface = (CInterface*)new CConnectionPointContainerA(pObjOuter, (IConnectionPointContainer *)pObj);
		break;

	case ID_IEnumConnectionPoints:
		pInterface = (CInterface*)new CEnumConnectionPointsA(pObjOuter, (IEnumConnectionPoints *)pObj);
		break;

	case ID_IConnectionPoint:
		pInterface = (CInterface*)new CConnectionPointA(pObjOuter, (IConnectionPoint *)pObj);
		break;

	case ID_IEnumConnections:
		pInterface = (CInterface*)new CEnumConnectionsA(pObjOuter, (IEnumConnections *)pObj);
		break;

	case ID_IOleControl:
		pInterface = (CInterface*)new COleControlA(pObjOuter, (IOleControl *)pObj);
		break;

	case ID_IOleControlSite:
		pInterface = (CInterface*)new COleControlSiteA(pObjOuter, (IOleControlSite *)pObj);
		break;

	case ID_ISimpleFrameSite:
		pInterface = (CInterface*)new CSimpleFrameSiteA(pObjOuter, (ISimpleFrameSite *)pObj);
		break;

	case ID_IPersistStreamInit:
		pInterface = (CInterface*)new CPersistStreamInitA(pObjOuter, (IPersistStreamInit *)pObj);
		break;

	case ID_IClassFactory2:
		pInterface = (CInterface*)new CClassFactory2A(pObjOuter, (IClassFactory2 *)pObj);
		break;

	case ID_ISpecifyPropertyPages:
		pInterface = (CInterface*)new CSpecifyPropertyPagesA(pObjOuter, (ISpecifyPropertyPages *)pObj);
		break;

	case ID_IPerPropertyBrowsing:
		pInterface = (CInterface*)new CPerPropertyBrowsingA(pObjOuter, (IPerPropertyBrowsing *)pObj);
		break;

	case ID_IPropertyPageSite:
		pInterface = (CInterface*)new CPropertyPageSiteA(pObjOuter, (IPropertyPageSite *)pObj);
		break;

	case ID_IPropertyPage:
	case ID_IPropertyPage2:
		pInterface = (CInterface*)new CPropertyPage2A(pObjOuter, (IPropertyPage2 *)pObj);
		break;

	case ID_IFont:
		pInterface = (CInterface*)new CFontA(pObjOuter, (IFont *)pObj);
		break;

	case ID_IPicture:
		pInterface = (CInterface*)new CPictureA(pObjOuter, (IPicture *)pObj);
		break;

	case ID_IFontDisp:
	case ID_IPictureDisp:
		pInterface = (CInterface*)new CDispatchA(pObjOuter, (IDispatch *)pObj);
		break;
	}

	return pInterface;
}


CInterface* WrapAnyControlWFromA(LPUNKNOWN pObjOuter, IDINTERFACE idRef, LPUNKNOWN pObj)
{
	CInterface* pInterface = NULL;

	switch (idRef)
	{
	case ID_IPropertyNotifySink:
		pInterface = (CInterface*)new CPropertyNotifySinkW(pObjOuter, (IPropertyNotifySinkA *)pObj);
		break;

	case ID_IProvideClassInfo:
		pInterface = (CInterface*)new CProvideClassInfoW(pObjOuter, (IProvideClassInfoA *)pObj);
		break;

	case ID_IConnectionPointContainer:
		pInterface = (CInterface*)new CConnectionPointContainerW(pObjOuter, (IConnectionPointContainerA *)pObj);
		break;

	case ID_IEnumConnectionPoints:
		pInterface = (CInterface*)new CEnumConnectionPointsW(pObjOuter, (IEnumConnectionPointsA *)pObj);
		break;

	case ID_IConnectionPoint:
		pInterface = (CInterface*)new CConnectionPointW(pObjOuter, (IConnectionPointA *)pObj);
		break;

	case ID_IEnumConnections:
		pInterface = (CInterface*)new CEnumConnectionsW(pObjOuter, (IEnumConnectionsA *)pObj);
		break;

	case ID_IOleControl:
		pInterface = (CInterface*)new COleControlW(pObjOuter, (IOleControlA *)pObj);
		break;

	case ID_IOleControlSite:
		pInterface = (CInterface*)new COleControlSiteW(pObjOuter, (IOleControlSiteA *)pObj);
		break;

	case ID_ISimpleFrameSite:
		pInterface = (CInterface*)new CSimpleFrameSiteW(pObjOuter, (ISimpleFrameSiteA *)pObj);
		break;

	case ID_IPersistStreamInit:
		pInterface = (CInterface*)new CPersistStreamInitW(pObjOuter, (IPersistStreamInitA *)pObj);
		break;

	case ID_IClassFactory2:
		pInterface = (CInterface*)new CClassFactory2W(pObjOuter, (IClassFactory2A *)pObj);
		break;

	case ID_ISpecifyPropertyPages:
		pInterface = (CInterface*)new CSpecifyPropertyPagesW(pObjOuter, (ISpecifyPropertyPagesA *)pObj);
		break;

	case ID_IPerPropertyBrowsing:
		pInterface = (CInterface*)new CPerPropertyBrowsingW(pObjOuter, (IPerPropertyBrowsingA *)pObj);
		break;

	case ID_IPropertyPageSite:
		pInterface = (CInterface*)new CPropertyPageSiteW(pObjOuter, (IPropertyPageSiteA *)pObj);
		break;

	case ID_IPropertyPage:
	case ID_IPropertyPage2:
		pInterface = (CInterface*)new CPropertyPage2W(pObjOuter, (IPropertyPage2A *)pObj);
		break;

	case ID_IFont:
		pInterface = (CInterface*)new CFontW(pObjOuter, (IFontA *)pObj);
		break;

	case ID_IPicture:
		pInterface = (CInterface*)new CPictureW(pObjOuter, (IPictureA *)pObj);
		break;

	case ID_IFontDisp:
	case ID_IPictureDisp:
		pInterface = (CInterface*)new CDispatchW(pObjOuter, (IDispatchA *)pObj);
		break;
	}

	return pInterface;
}


//////////////////////////////////////////////////////////////////////////////
// Wrappers for IPropertyNotifySink interface

HRESULT WrapIPropertyNotifySinkAFromW(LPPROPERTYNOTIFYSINK pobj, LPPROPERTYNOTIFYSINKA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IPropertyNotifySink, pobj, (LPUNKNOWN *)ppobjA);
}

HRESULT WrapIPropertyNotifySinkWFromA(LPPROPERTYNOTIFYSINKA pobjA, LPPROPERTYNOTIFYSINK * ppobj)
{
	return WrapInterfaceWFromA(ID_IPropertyNotifySink, pobjA, (LPUNKNOWN *)ppobj);
}


//////////////////////////////////////////////////////////////////////////////
// Wrappers for IProvideClassInfo interface

HRESULT WrapIProvideClassInfoAFromW(LPPROVIDECLASSINFO pobj, LPPROVIDECLASSINFOA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IProvideClassInfo, pobj, (LPUNKNOWN *)ppobjA);
}

HRESULT WrapIProvideClassInfoWFromA(LPPROVIDECLASSINFOA pobjA, LPPROVIDECLASSINFO * ppobj)
{
	return WrapInterfaceWFromA(ID_IProvideClassInfo, pobjA, (LPUNKNOWN *)ppobj);
}


//////////////////////////////////////////////////////////////////////////////
// Wrappers for IConnectionPointContainer interface

HRESULT WrapIConnectionPointContainerAFromW(LPCONNECTIONPOINTCONTAINER pobj, LPCONNECTIONPOINTCONTAINERA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IConnectionPointContainer, pobj, (LPUNKNOWN *)ppobjA);
}

HRESULT WrapIConnectionPointContainerWFromA(LPCONNECTIONPOINTCONTAINERA pobjA, LPCONNECTIONPOINTCONTAINER * ppobj)
{
	return WrapInterfaceWFromA(ID_IConnectionPointContainer, pobjA, (LPUNKNOWN *)ppobj);
}


//////////////////////////////////////////////////////////////////////////////
// Wrappers for IEnumConnectionPoints interface

HRESULT WrapIEnumConnectionPointsAFromW(LPENUMCONNECTIONPOINTS pobj, LPENUMCONNECTIONPOINTSA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IEnumConnectionPoints, pobj, (LPUNKNOWN *)ppobjA);
}

HRESULT WrapIEnumConnectionPointsWFromA(LPENUMCONNECTIONPOINTSA pobjA, LPENUMCONNECTIONPOINTS * ppobj)
{
	return WrapInterfaceWFromA(ID_IEnumConnectionPoints, pobjA, (LPUNKNOWN *)ppobj);
}


//////////////////////////////////////////////////////////////////////////////
// Wrappers for IConnectionPoint interface

HRESULT WrapIConnectionPointAFromW(LPCONNECTIONPOINT pobj, LPCONNECTIONPOINTA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IConnectionPoint, pobj, (LPUNKNOWN *)ppobjA);
}

HRESULT WrapIConnectionPointWFromA(LPCONNECTIONPOINTA pobjA, LPCONNECTIONPOINT * ppobj)
{
	return WrapInterfaceWFromA(ID_IConnectionPoint, pobjA, (LPUNKNOWN *)ppobj);
}


//////////////////////////////////////////////////////////////////////////////
// Wrappers for IEnumConnections interface

HRESULT WrapIEnumConnectionsAFromW(LPENUMCONNECTIONS pobj, LPENUMCONNECTIONSA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IEnumConnections, pobj, (LPUNKNOWN *)ppobjA);
}

HRESULT WrapIEnumConnectionsWFromA(LPENUMCONNECTIONSA pobjA, LPENUMCONNECTIONS * ppobj)
{
	return WrapInterfaceWFromA(ID_IEnumConnections, pobjA, (LPUNKNOWN *)ppobj);
}


//////////////////////////////////////////////////////////////////////////////
// Wrappers for IOleControl interface

HRESULT WrapIOleControlAFromW(LPOLECONTROL pobj, LPOLECONTROLA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IOleControl, pobj, (LPUNKNOWN *)ppobjA);
}

HRESULT WrapIOleControlWFromA(LPOLECONTROLA pobjA, LPOLECONTROL * ppobj)
{
	return WrapInterfaceWFromA(ID_IOleControl, pobjA, (LPUNKNOWN *)ppobj);
}


//////////////////////////////////////////////////////////////////////////////
// Wrappers for IOleControlSite interface

HRESULT WrapIOleControlSiteAFromW(LPOLECONTROLSITE pobj, LPOLECONTROLSITEA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IOleControlSite, pobj, (LPUNKNOWN *)ppobjA);
}

HRESULT WrapIOleControlSiteWFromA(LPOLECONTROLSITEA pobjA, LPOLECONTROLSITE * ppobj)
{
	return WrapInterfaceWFromA(ID_IOleControlSite, pobjA, (LPUNKNOWN *)ppobj);
}


//////////////////////////////////////////////////////////////////////////////
// Wrappers for ISimpleFrameSite interface

HRESULT WrapISimpleFrameSiteAFromW(LPSIMPLEFRAMESITE pobj, LPSIMPLEFRAMESITEA * ppobjA)
{
	return WrapInterfaceAFromW(ID_ISimpleFrameSite, pobj, (LPUNKNOWN *)ppobjA);
}

HRESULT WrapISimpleFrameSiteWFromA(LPSIMPLEFRAMESITEA pobjA, LPSIMPLEFRAMESITE * ppobj)
{
	return WrapInterfaceWFromA(ID_ISimpleFrameSite, pobjA, (LPUNKNOWN *)ppobj);
}


//////////////////////////////////////////////////////////////////////////////
// Wrappers for IPersistStreamInit interface

HRESULT WrapIPersistStreamInitAFromW(LPPERSISTSTREAMINIT pobj, LPPERSISTSTREAMINITA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IPersistStreamInit, pobj, (LPUNKNOWN *)ppobjA);
}

HRESULT WrapIPersistStreamInitWFromA(LPPERSISTSTREAMINITA pobjA, LPPERSISTSTREAMINIT * ppobj)
{
	return WrapInterfaceWFromA(ID_IPersistStreamInit, pobjA, (LPUNKNOWN *)ppobj);
}


//////////////////////////////////////////////////////////////////////////////
// Wrappers for IClassFactory2 interface

HRESULT WrapIClassFactory2AFromW(LPCLASSFACTORY2 pobj, LPCLASSFACTORY2A * ppobjA)
{
	return WrapInterfaceAFromW(ID_IClassFactory2, pobj, (LPUNKNOWN *)ppobjA);
}

HRESULT WrapIClassFactory2WFromA(LPCLASSFACTORY2A pobjA, LPCLASSFACTORY2 * ppobj)
{
	return WrapInterfaceWFromA(ID_IClassFactory2, pobjA, (LPUNKNOWN *)ppobj);
}


//////////////////////////////////////////////////////////////////////////////
// Wrappers for ISpecifyPropertyPages interface

HRESULT WrapISpecifyPropertyPagesAFromW(LPSPECIFYPROPERTYPAGES pobj, LPSPECIFYPROPERTYPAGESA * ppobjA)
{
	return WrapInterfaceAFromW(ID_ISpecifyPropertyPages, pobj, (LPUNKNOWN *)ppobjA);
}

HRESULT WrapISpecifyPropertyPagesWFromA(LPSPECIFYPROPERTYPAGESA pobjA, LPSPECIFYPROPERTYPAGES * ppobj)
{
	return WrapInterfaceWFromA(ID_ISpecifyPropertyPages, pobjA, (LPUNKNOWN *)ppobj);
}


//////////////////////////////////////////////////////////////////////////////
// Wrappers for IPerPropertyBrowsing interface

HRESULT WrapIPerPropertyBrowsingAFromW(LPPERPROPERTYBROWSING pobj, LPPERPROPERTYBROWSINGA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IPerPropertyBrowsing, pobj, (LPUNKNOWN *)ppobjA);
}

HRESULT WrapIPerPropertyBrowsingWFromA(LPPERPROPERTYBROWSINGA pobjA, LPPERPROPERTYBROWSING * ppobj)
{
	return WrapInterfaceWFromA(ID_IPerPropertyBrowsing, pobjA, (LPUNKNOWN *)ppobj);
}


//////////////////////////////////////////////////////////////////////////////
// Wrappers for IPropertyPageSite interface

HRESULT WrapIPropertyPageSiteAFromW(LPPROPERTYPAGESITE pobj, LPPROPERTYPAGESITEA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IPropertyPageSite, pobj, (LPUNKNOWN *)ppobjA);
}

HRESULT WrapIPropertyPageSiteWFromA(LPPROPERTYPAGESITEA pobjA, LPPROPERTYPAGESITE * ppobj)
{
	return WrapInterfaceWFromA(ID_IPropertyPageSite, pobjA, (LPUNKNOWN *)ppobj);
}


//////////////////////////////////////////////////////////////////////////////
// Wrappers for IPropertyPage interface

HRESULT WrapIPropertyPageAFromW(LPPROPERTYPAGE pobj, LPPROPERTYPAGEA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IPropertyPage, pobj, (LPUNKNOWN *)ppobjA);
}

HRESULT WrapIPropertyPageWFromA(LPPROPERTYPAGEA pobjA, LPPROPERTYPAGE * ppobj)
{
	return WrapInterfaceWFromA(ID_IPropertyPage, pobjA, (LPUNKNOWN *)ppobj);
}


//////////////////////////////////////////////////////////////////////////////
// Wrappers for IPropertyPage2 interface

HRESULT WrapIPropertyPage2AFromW(LPPROPERTYPAGE2 pobj, LPPROPERTYPAGE2A * ppobjA)
{
	return WrapInterfaceAFromW(ID_IPropertyPage2, pobj, (LPUNKNOWN *)ppobjA);
}

HRESULT WrapIPropertyPage2WFromA(LPPROPERTYPAGE2A pobjA, LPPROPERTYPAGE2 * ppobj)
{
	return WrapInterfaceWFromA(ID_IPropertyPage2, pobjA, (LPUNKNOWN *)ppobj);
}


//////////////////////////////////////////////////////////////////////////////
// Wrappers for IFont interface

HRESULT WrapIFontAFromW(LPFONT pobj, LPFONTA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IFont, pobj, (LPUNKNOWN *)ppobjA);
}

HRESULT WrapIFontWFromA(LPFONTA pobjA, LPFONT * ppobj)
{
	return WrapInterfaceWFromA(ID_IFont, pobjA, (LPUNKNOWN *)ppobj);
}


//////////////////////////////////////////////////////////////////////////////
// Wrappers for IFontDisp interface

HRESULT WrapIFontDispAFromW(LPFONTDISP pobj, LPFONTDISPA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IFontDisp, pobj, (LPUNKNOWN *)ppobjA);
}

HRESULT WrapIFontDispWFromA(LPFONTDISPA pobjA, LPFONTDISP * ppobj)
{
	return WrapInterfaceWFromA(ID_IFontDisp, pobjA, (LPUNKNOWN *)ppobj);
}


//////////////////////////////////////////////////////////////////////////////
// Wrappers for IPicture interface

HRESULT WrapIPictureAFromW(LPPICTURE pobj, LPPICTUREA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IPicture, pobj, (LPUNKNOWN *)ppobjA);
}

HRESULT WrapIPictureWFromA(LPPICTUREA pobjA, LPPICTURE * ppobj)
{
	return WrapInterfaceWFromA(ID_IPicture, pobjA, (LPUNKNOWN *)ppobj);
}


//////////////////////////////////////////////////////////////////////////////
// Wrappers for IPictureDisp interface

HRESULT WrapIPictureDispAFromW(LPPICTUREDISP pobj, LPPICTUREDISPA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IPictureDisp, pobj, (LPUNKNOWN *)ppobjA);
}

HRESULT WrapIPictureDispWFromA(LPPICTUREDISPA pobjA, LPPICTUREDISP * ppobj)
{
	return WrapInterfaceWFromA(ID_IPictureDisp, pobjA, (LPUNKNOWN *)ppobj);
}
