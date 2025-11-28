/*
**
*
* MODIFIER.C - Sample API routine.
*
* Copyright (c) 1989-1993 Microsoft Corporation as an unpublished
* licensed proprietary work.  All rights reserved.
*
* Description:
*		This library establishes an Event Handler
* 		which will display which Key Code should be
*		used when a key is pressed.  This is
*		dependent upon the modifier keys used during
*		the key press.
*
**
 */



#include <pro_ext.h>

int         eventid;


//
// This routine is registered as an event handler.
// But only cares about keyDownEvents which it then
// passes on to FoxPro.
//

FAR EventHandler(WHandle theWindow, EventRec FAR *ev)
{



    switch(ev->what)
    {

	case keyDownEvent:
            if (ev->modifiers & shiftCodeMask)   /* A modifier was pressed. */
            {
                if (ev->modifiers & altKey)      /* Alt Key */

                    _PutStr("Alt Key Code should be used.\n");

                else
                    if (ev->modifiers & ctrlKey) /* Ctrl Key */

                        _PutStr("Ctrl Key Code should be used.\n");

                    else
                        if (ev->modifiers & shiftKey)    /* Shift Key */

                            _PutStr("Shift Key Code should be used.\n");
            }
            else                                 /* Normal Key press */
                _PutStr("Regular Key Code should be used.\n");

            return NO;
            break;



        default:
	    return NO;
    }
    return YES;
}

FAR Modifier()
{

    eventid = _ActivateHandler(EventHandler);

}



FAR ModifierExit()
{

    _DeActivateHandler(eventid);

}


FoxInfo myFoxInfo[] = {
	{"MODIFIER", (FPFI) Modifier, CALLONLOAD, ""},
	{"MODIFIEREXIT", (FPFI) ModifierExit, CALLONUNLOAD, ""}
};

FoxTable _FoxTable = {
    (FoxTable FAR *)0, sizeof(myFoxInfo) / sizeof(FoxInfo), myFoxInfo
};
