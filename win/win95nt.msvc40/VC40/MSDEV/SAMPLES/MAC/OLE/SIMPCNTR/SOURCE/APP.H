#ifndef _MSC_VER
#pragma once
#endif

#ifndef _CAPPLICATION_H_
#define _CAPPLICATION_H_

#ifndef _WINDOWS_H_
#include "CWindow.h"
#endif

#define kExtraStackSpace	8096
#define	kExtraMasters		   2

#define	kMenuBar			 128

#define kAboutAlert			 129

#define	mObject				 100

#define mApple				 200
#define 	iAbout		 	   1

#define mFile			 	 201
#define 	iNew		 	   1
#define		iClose			   3
#define 	iQuit			   5

#define mEdit				 202
#define 	iInsertObject	   1
#define		iVerbs			   3

#define 	MIN_WINDOW_X	 100
#define 	MIN_WINDOW_Y	 100

class CApplication
{
public:

	CWindow *			m_window;
	Boolean				m_fDone;
	Boolean				m_fInForeground;
	short				m_nWndCount;
	Boolean				m_fHasVerbs;
	
	LPMESSAGEFILTER		m_pIMessageFilter;
		
	CApplication(void);
	~CApplication(void);
	
	void			InitApplication		(void);
	void			InitToolbox			(void);
	void 			SetupMenus			(void);

	void			DisposeApplication	(void);

	void			OleInitApplication	(void);
	void			OleCleanup			(void);

	void 			EventLoop			(void);
	void 			AdjustMenus			(void);

	void 			HandleEvent			(EventRecord event);
	void			HandleMouseDown		(EventRecord event);
	void			HandleMouseUp		(void);
	void			HandleKeyDown		(EventRecord event);
	void			HandleActivate		(EventRecord event);
	void			HandleUpdate		(EventRecord event);
	void			HandleDiskEvent		(EventRecord event);
	void 			HandleOSEvent		(EventRecord event);
	void			HandleNullEvent		(void);
	void 			HandleMenuCommand	(long cmd);

	void			AddVerbs			(void);
	SCODE			CallDoVerb			(long verb);

	void 			BringToForeground	(Boolean fWait);

	void 			CreateNewDocument	(void);
};

extern CApplication *	gApp;
extern char				gAppName[];


void InitToolbox(void);

#endif // _CAPPLICATION_H_
