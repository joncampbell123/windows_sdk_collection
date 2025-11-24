/*** 
*clsid.h
*
*  Copyright (C) 1992, Microsoft Corporation.  All Rights Reserved.
*  Information Contained Herein Is Proprietary and Confidential.
*
*Purpose:
*  This file defines the CLSIDs
*
*Implementation Notes:
*
*****************************************************************************/

DEFINE_OLEGUID(CLSID_CPoly,	0x00020462, 0, 0);
DEFINE_OLEGUID(CLSID_CPoint,	0x00020463, 0, 0);

#ifdef INITGUID
# ifdef _MAC
DEFINE_GUID(GUID_NULL, 0L, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
DEFINE_OLEGUID(IID_IDispatch,		0x00020400L, 0, 0);
DEFINE_OLEGUID(IID_IEnumVARIANT,	0x00020404L, 0, 0);
DEFINE_OLEGUID(IID_IUnknown, 		0x00000000L, 0, 0);
DEFINE_OLEGUID(IID_IClassFactory,	0x00000001L, 0, 0);
# endif
#endif


