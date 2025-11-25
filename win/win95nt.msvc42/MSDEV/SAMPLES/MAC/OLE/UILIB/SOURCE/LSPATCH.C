#ifndef _MSC_VER
#include <SetUpA4.h>
#include <Types.h>
#include <Memory.h>

UniversalProcPtr	gOldPack0 = NULL;

pascal void LSPatch(short setIt, Point theCell, ListHandle theList, short theSelector);

pascal void main()
{
	RememberA0();							// Set up A4
	SetUpA4();								// For globals
	
	gOldPack0 = GetToolTrapAddress(_Pack0);
	SetToolTrapAddress((UniversalProcPtr)LSPatch, _Pack0);
	
	RestoreA4();							// Restore A4 before we go
}
	
pascal void LSPatch(short setIt, Point theCell, ListHandle theList, short theSelector)
{
	long mainPtr;
	long oldA4;
	
	mainPtr = (long)main;					// Get pointer to main
	mainPtr -= 38;							// move back through prolog
		
	if (0x5c == theSelector)				// LSetSelect Check
	{
		asm
		{
			move.l	A4, oldA4
			move.l	mainPtr,A4
		}
		
		SetToolTrapAddress(gOldPack0, _Pack0);	// Put back old trap
		asm
		{
			move.l	oldA4, A4
		}
		return;
	}
		
	asm
	{
		move.l 	A4, oldA4
		move.l	mainPtr,A4
		move.l	gOldPack0, A0
		move.l	oldA4,A4
		unlk	A6
		jmp		(A0)
	}
}
#endif // _MSC_VER
