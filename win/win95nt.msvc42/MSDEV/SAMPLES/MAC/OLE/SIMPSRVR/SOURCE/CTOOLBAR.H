#ifndef _MSC_VER
#pragma once
#endif

#include "CWindow.h"

#define		kToolbarPict	128

#define		NO_TOOL			 -1
#define		LINE			  0
#define		SQUARE			  1
#define		CIRCLE			  2

#define		TOOL_Y			 21
#define		TOOL_X			 24

class CToolbar : public CWindow
{
public:

	PicHandle	m_PicHandle;
	short		m_Tool;
	Rect		m_updateRect;

	CToolbar(void);
	~CToolbar(void);

	virtual OSErr		InitToolbar			(void);

	virtual void		AdjustCursor		(void);
	virtual void		DrawContents		(void);

	virtual	void		OnWindowResize		(void); 
	virtual void		HandleContentClick	(Point pt);
};
