/*****************************************************************************\
*                                                                             *
*    main.c                                                                   *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#if !defined(_MSC_VER) && !defined(THINK_C)
#include "OLine.h"
#endif

#if defined(USEHEADER)
#include "OleHdrs.h"
#endif
#include "Debug.h"
#if qOle
	#define INITGUID
	
	#include "compobj.h"
	#ifndef _PPCMAC
	#include <initguid.h>			// forces our GUIDs to be initialized
	#else
		// forces GUIDs PMAC style, without segments
		#undef DEFINE_GUID
		#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
		 const GUID name = { l, w1, w2, b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 }
	#endif
	#include <coguid.h>
	#include <OleGuid.h>
#endif
#include <SegLoad.h>
#include <Traps.h>
#include "Const.h"
#include "App.h"
#include "OleXcept.h"
#include "Util.h"

extern ApplicationPtr	gApplication;

#ifndef _MSC_VER
#pragma segment mainSeg
#endif
void main(void)
{
	volatile ApplicationPtr		pApp;

	InitDebugging();
	InitVtbls();

	TRY
	{
#if qOle
		pApp = (ApplicationPtr)NewPtrClear(sizeof(OleOutlineAppRec));
#else
		pApp = (ApplicationPtr)NewPtrClear(sizeof(OutlineAppRec));
#endif

		ASSERTCOND(pApp != nil);
		gApplication = pApp;

#if qOleServerApp
		OleOutlineAppInit((OleOutlineAppPtr)pApp, kOutlineServerType);
#elif qOleContainerApp
		OleOutlineAppInit((OleOutlineAppPtr)pApp, kOutlineContainerType);
#else
		OutlineAppInit((OutlineAppPtr)pApp, kOutlineType);
#endif

		ASSERTCOND(pApp->vtbl->m_DoStartupProcPtr != nil);
		(*pApp->vtbl->m_DoStartupProcPtr)(pApp);

#if !defined(THINK_C) && !defined(__MWERKS__)
#if !defined(_PPCMAC)
		UnloadSeg(OutlineAppInit);
#endif
#endif

		// run the event loop
		ASSERTCOND(pApp->vtbl->m_EventLoopProcPtr != nil);
		(*pApp->vtbl->m_EventLoopProcPtr)(pApp);

		ASSERTCOND(pApp->vtbl->m_DisposeProcPtr != nil);
		(*pApp->vtbl->m_DisposeProcPtr)(pApp);
	}
	CATCH
	{
		SysBeep(1);
		SysBeep(1);
		SysBeep(1);
		
		if (pApp)
		{
			ASSERTCOND(pApp->vtbl->m_DisposeProcPtr != nil);
			(*pApp->vtbl->m_DisposeProcPtr)(pApp);
		}

		NO_PROPAGATE;
	}
	ENDTRY
	
	DisposeVtbls();
	FinishDebugging();
}
