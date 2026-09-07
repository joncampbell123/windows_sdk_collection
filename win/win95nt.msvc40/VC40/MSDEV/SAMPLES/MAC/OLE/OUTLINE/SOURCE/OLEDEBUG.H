#ifndef __OLEDEBUG_H__
#define __OLEDEBUG_H__

#if qOleDebug

	extern Boolean gOleBreakInterfaceCalls;
	
	#define OleDbgEnterInterface()	{if (gOleBreakInterfaceCalls) Debugger();}

	#define OLEDEBUG_BEGIN
	#define OLEDEBUG_END
	#define OLEDEBUG_BEGIN1(lpsz)
	#define OLEDEBUG_BEGIN2(lpsz)
	#define OLEDEBUG_BEGIN3(lpsz)
	#define OLEDEBUG_BEGIN4(lpsz)
	#define OLEDEBUG_END1
	#define OLEDEBUG_END2
	#define OLEDEBUG_END3
	#define OLEDEBUG_END4

	#define OLEDEBUGAssertSz(condition, str)
#else

	#define OleDbgEnterInterface()

	#define OLEDEBUG_BEGIN
	#define OLEDEBUG_END
	#define OLEDEBUG_BEGIN1(lpsz)
	#define OLEDEBUG_BEGIN2(lpsz)
	#define OLEDEBUG_BEGIN3(lpsz)
	#define OLEDEBUG_BEGIN4(lpsz)
	#define OLEDEBUG_END1
	#define OLEDEBUG_END2
	#define OLEDEBUG_END3
	#define OLEDEBUG_END4
	
	#define OLEDEBUGAssertSz(condition, str)
#endif

#endif
