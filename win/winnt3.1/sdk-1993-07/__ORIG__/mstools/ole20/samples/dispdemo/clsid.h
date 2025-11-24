/*** 
*clsid.h
*
*  Copyright (C) 1992, Microsoft Corporation.  All Rights Reserved.
*  Information Contained Herein Is Proprietary and Confidential.
*
*Purpose:
*  This file defines the CLSIDs referenced by the IDispatch demo app.
*
*Implementation Notes:
*
*****************************************************************************/

DEFINE_OLEGUID(CLSID_CPoly,	0x00020462, 0, 0);
DEFINE_OLEGUID(CLSID_CPoint,	0x00020463, 0, 0);

DEFINE_OLEGUID(CLSID_CPoly2,	0x00020464, 0, 0);
DEFINE_OLEGUID(CLSID_CPoint2,	0x00020465, 0, 0);

#ifdef _MAC
DEFINE_OLEGUID(CLSID_TMS,	0x00020206, 0, 0);
DEFINE_OLEGUID(CLSID_TMSi,	0x00020207, 0, 0);
DEFINE_OLEGUID(CLSID_SR2,	0x00010001, 0, 0);
#endif

#ifdef INITGUID
# ifdef _MAC
DEFINE_GUID(GUID_NULL, 0L, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
DEFINE_OLEGUID(IID_IDispatch,	0x00020400L, 0, 0);
DEFINE_OLEGUID(IID_IUnknown, 	0x00000000L, 0, 0);
# endif
#endif


