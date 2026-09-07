#include "hdrs.h"


#define kNoEvents			0

ASSERTDATA;


Boolean TrapExists(short theTrap)
{
	TrapType	theTrapType;

	theTrapType = GetTrapType(theTrap);
	if ((theTrapType == ToolTrap) && ((theTrap &= 0x07FF) >= NumToolboxTraps()))
		theTrap = (short) _Unimplemented;

	return NGetTrapAddress((short) _Unimplemented, ToolTrap) != NGetTrapAddress(theTrap, theTrapType);
}


TrapType GetTrapType(short theTrap)
{
	/* OS traps start with A0, Tool with A8 or AA. */
	if ((theTrap & 0x0800) == 0)					/* per D.A. */
		return OSTrap;
	else
		return ToolTrap;
}


short NumToolboxTraps(void)
{
	if (NGetTrapAddress( (short) _InitGraf, ToolTrap) == NGetTrapAddress( (short) 0xAA6E, ToolTrap))
		return 0x200;
	else
		return 0x400;
}


long GetGestaltResult(OSType gestaltSelector)
{
	long	gestaltResult;

	if (Gestalt(gestaltSelector, &gestaltResult) == noErr)
		return gestaltResult;
	else
		return 0;
}


Boolean IsSystem7(void)
{
	SysEnvRec	theEnv;
	OSErr		err;

	err = SysEnvirons(curSysEnvVers,&theEnv);

	return (err == noErr && theEnv.systemVersion < 0x700);
}


Boolean IsFrontProcess(void)
{
	ProcessSerialNumber	psnFront, 
						psnCurrent;
	Boolean				fReturn;

	GetCurrentProcess(&psnCurrent);
	GetFrontProcess(&psnFront);

	SameProcess(&psnFront, &psnCurrent, &fReturn);

	return fReturn;
}


void EnDisItem(MenuHandle hMenu, short theItem, Boolean enable)
{
	ASSERT(hMenu != NULL);

	if (enable)
		EnableItem(hMenu, theItem);
	else
		DisableItem(hMenu, theItem);
}


void GetGlobalMouse(Point *pt)
{
	EventRecord	event;

	OSEventAvail(kNoEvents, &event);

	*pt = event.where;
}


pascal void WriteToOleReg (char * strKey, char * strValue)
{
	static Boolean 	fNoErr = true;
	SCODE 			sc;

	if (!fNoErr)
		return;

	sc = RegSetValue(HKEY_CLASSES_ROOT, strKey, REG_SZ, strValue, 0);
	if (sc != S_OK)
	{
		FailScode("\pRegSetValue()", sc);
		fNoErr = false;
	}
}
