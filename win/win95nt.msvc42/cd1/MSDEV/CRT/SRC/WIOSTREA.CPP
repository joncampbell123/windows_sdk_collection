// wiostream -- initialize standard wide streams
#include <fstream>
#include <iostream>
#include <new>

		// OBJECT DECLARATIONS
int _Winit::_Init_cnt = -1;
static wfilebuf wfin(_Noinit);
static wfilebuf wfout(_Noinit);
static wfilebuf wferr(_Noinit);
wistream wcin(_Noinit);
wostream wcout(_Noinit);
wostream wcerr(_Noinit);
wostream wclog(_Noinit);

_Winit::_Winit()
	{	// initialize standard wide streams first time
	bool doinit;
		{_Lockit _Lk;
		if (0 <= _Init_cnt)
			++_Init_cnt, doinit = false;
		else
			_Init_cnt = 1, doinit = true; }
	if (doinit)
		{	// initialize standard wide streams
		new (&wfin) wfilebuf(stdin);
		new (&wfout) wfilebuf(stdout);
		new (&wferr) wfilebuf(stderr);
		new (&wcin) wistream(&wfin, true);
		new (&wcout) wostream(&wfout, true);
		wcin.tie(&wcout);
		new (&wcerr) wostream(&wferr, true);
		wcerr.tie(&wcout);
		wcerr.setf(ios_base::unitbuf);
		new (&wclog) wostream(&wferr, true);
		wclog.tie(&wcout);
		}
	}

_Winit::~_Winit()
	{	// flush standard wide streams last time
	bool doflush;
		{_Lockit _Lk;
		if (--_Init_cnt == 0)
			doflush = true;
		else
			doflush = false; }
	if (doflush)
		{	// flush standard wide streams
		wcout.flush();
		wcerr.flush();
		wclog.flush();
		}
	}

/*
 * Copyright (c) 1994 by P.J. Plauger.  ALL RIGHTS RESERVED. 
 * Consult your license regarding permissions and restrictions.
 */
