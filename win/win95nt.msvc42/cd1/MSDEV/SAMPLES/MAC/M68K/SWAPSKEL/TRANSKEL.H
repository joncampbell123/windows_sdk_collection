/*  TransSkel 2.0 Quick Reference */

SkelApple(StringPtr aboutTitle, ProcPtr aboutProc);
SkelBackground(ProcPtr p);
SkelClobber(void);
SkelDialog(DialogPtr theDialog, ProcPtr pEvent, ProcPtr pClose, ProcPtr pClobber);
SkelDlogMask(short mask);
SkelEventHook(Boolean (*p)());
SkelEventMask(short mask);
SkelGetBackground(ProcPtr *p);
SkelGetDlogMask(short *mask);
SkelGetEventHook(Boolean (**p)());
SkelGetEventMask(short *mask);
SkelGrowBounds(WindowPtr theWind, short hLo, short vLo, short hHi, short vHi);
SkelInit(short noMasters, GrowZoneProcPtr growZoneProc);
SkelMain(void);
SkelMenu(MenuHandle theMenu, ProcPtr pSelect, ProcPtr pClobber, Boolean drawBar);
SkelRmveDlog(DialogPtr theDialog);
SkelRmveMenu(MenuHandle theMenu);
SkelRmveWind(WindowPtr theWind);
SkelWhoa(void);
SkelWindow (WindowPtr theWind, ProcPtr pMouse, ProcPtr pKey, ProcPtr pUpdate, 
				ProcPtr pActivate, ProcPtr pClose, ProcPtr pClobber, ProcPtr pIdle, 
				Boolean frontOnly);
