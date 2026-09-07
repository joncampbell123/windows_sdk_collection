// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1995 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"

#ifdef AFX_OLE3_SEG
#pragma code_seg(AFX_OLE3_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Support for MFC/COM in DLLs

SCODE AFXAPI AfxDllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	SCODE sc;
	*ppv = NULL;

	// search factories defined in the application
	AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
	AfxLockGlobals(CRIT_OBJECTFACTORYLIST);
	for (COleObjectFactory* pFactory = pModuleState->m_factoryList;
		pFactory != NULL; pFactory = pFactory->m_pNextFactory)
	{
		if (pFactory->m_bRegistered != 0 && pFactory->m_clsid == rclsid)
		{
			// found suitable class factory -- query for correct interface
			sc = pFactory->ExternalQueryInterface(&riid, ppv);
			AfxUnlockGlobals(CRIT_OBJECTFACTORYLIST);
			return sc;
		}
	}
	AfxUnlockGlobals(CRIT_OBJECTFACTORYLIST);
#ifdef _AFXDLL
	AfxLockGlobals(CRIT_DYNLINKLIST);
	AfxLockGlobals(CRIT_RUNTIMECLASSLIST);
	// search factories defined in extension DLLs
	AFX_MODULE_PROCESS_STATE* pState = AfxGetModuleProcessState();
	for (CDynLinkLibrary* pDLL = pState->m_libraryList; pDLL != NULL;
		pDLL = pDLL->m_pNextDLL)
	{
		for (pFactory = pDLL->m_factoryList;
			pFactory != NULL; pFactory = pFactory->m_pNextFactory)
		{
			if (pFactory->m_bRegistered != 0 && pFactory->m_clsid == rclsid)
			{
				// found suitable class factory -- query for correct interface
				sc = pFactory->ExternalQueryInterface(&riid, ppv);
				AfxUnlockGlobals(CRIT_RUNTIMECLASSLIST);
				AfxUnlockGlobals(CRIT_DYNLINKLIST);
				return sc;
			}
		}
	}
	AfxUnlockGlobals(CRIT_RUNTIMECLASSLIST);
	AfxUnlockGlobals(CRIT_DYNLINKLIST);
#endif

	// factory not registered -- return error
	return CLASS_E_CLASSNOTAVAILABLE;
}

SCODE AFXAPI AfxDllCanUnloadNow(void)
{
	// return S_OK only if no outstanding objects active
	if (!AfxOleCanExitApp())
		return S_FALSE;

	// check if any class factories with >1 reference count
	AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
	AfxLockGlobals(CRIT_OBJECTFACTORYLIST);
	for (COleObjectFactory* pFactory = pModuleState->m_factoryList;
		pFactory != NULL; pFactory = pFactory->m_pNextFactory)
	{
		if (pFactory->m_dwRef > 1)
		{
			AfxUnlockGlobals(CRIT_OBJECTFACTORYLIST);
			return S_FALSE;
		}
	}
	AfxUnlockGlobals(CRIT_OBJECTFACTORYLIST);
#ifdef _AFXDLL
	AfxLockGlobals(CRIT_DYNLINKLIST);
	AfxLockGlobals(CRIT_OBJECTFACTORYLIST);
	// search factories defined in extension DLLs
	AFX_MODULE_PROCESS_STATE* pState = AfxGetModuleProcessState();
	for (CDynLinkLibrary* pDLL = pState->m_libraryList; pDLL != NULL;
		pDLL = pDLL->m_pNextDLL)
	{
		for (pFactory = pDLL->m_factoryList;
			pFactory != NULL; pFactory = pFactory->m_pNextFactory)
		{
			if (pFactory->m_dwRef > 1)
			{
				AfxUnlockGlobals(CRIT_OBJECTFACTORYLIST);
				AfxUnlockGlobals(CRIT_DYNLINKLIST);
				return S_FALSE;
			}
		}
	}
	AfxUnlockGlobals(CRIT_OBJECTFACTORYLIST);
	AfxUnlockGlobals(CRIT_DYNLINKLIST);
#endif

	TRACE0("Info: AfxDllCanUnloadNow returning S_OK\n");
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
