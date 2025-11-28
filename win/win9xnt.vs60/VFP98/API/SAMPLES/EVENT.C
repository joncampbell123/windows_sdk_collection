/*
**
*
* EVENT.C - Sample API routine.
*
* Copyright (c) 1989-1993 Microsoft Corporation as an unpublished
* licensed proprietary work.  All rights reserved.
*
* Description:
*       This library will create an Event Handler which demonstrates when
*       events are generated within FoxPro.  All events are processed, but
*       do get passed on so FoxPro can handle them also.  When a null event
*       is received, a counter is accumulated and once a different event has
*       been received a total of the count is displayed.
*
*       This library creates a file called EVENT.TXT which captures the events
*       and outputs the EventRec for each (except null events).  This file is
*       currently created by using the _Execute() command to SET PRINT TO.  So,
*       if you do anything while in FoxPro which uses the SET PRINT TO command,
*       EVENT.TXT will not capture everything.  NOTE: not all of the information
*       output through this routine for each event is relevant to that event.
*
**
*/
#include <pro_ext.h>


#define nl      _PutChr(0x0a);


int         eventid;            // Our event handler
long        num_nulls=0;        // accumulator of null events



/*      This routine will take a long integer and put it
        into a Value structure which it then prints out.                */

PutInt(unsigned long number)
{
    Value       val;


    val.ev_type = 'I';
    val.ev_width = 10;
    val.ev_long = number;
    _PutValue(&val);


}

/*      This routine checks to see if there were any null events
        processed since the last time we had an event.   If there were,
        then it prints out the number of null events that were processed
        and resets the counter to 0.                                    */

ChkNulls()
{

    if (num_nulls)              // if there were null events processed
    {
        _PutStr("\n\nNumber of Null Events Processed -> ");
        PutInt(num_nulls);
        num_nulls = 0;
    }

}

/*      This routine takes a pointer to a WHandle and a point on the screen
        as a parameter. It takes these parameters and determines in what
        area of the window the point is.  It then prints out which area of
        the window the mouse was pressed down in.                       */


PrintFindWindow(WHandle FAR *window, Point win_point)
{
    int where;


        where = _FindWindow(window, win_point);
        _PutStr("In Window -> ");
        switch (where)
        {
            case inBorder:
               _PutStr("inBorder\n");
               break;
            case inHelp:
               _PutStr("inHelp\n");
               break;
            case inContent:
               _PutStr("inContent\n");
               break;
            case inDrag:
               _PutStr("inDrag\n");
               break;
            case inGrow:
               _PutStr("inGrow\n");
               break;
            case inGoAway:
               _PutStr("inGoAway\n");
               break;
            case inZoom:
               _PutStr("inZoom\n");
               break;
            case inVUpArrow:
               _PutStr("inVUpArrow\n");
               break;
            case inVDownArrow:
               _PutStr("invDownArrow\n");
               break;
            case inVPageUp:
               _PutStr("inVPageUp\n");
               break;
            case inVPageDown:
               _PutStr("inVPageDown\n");
               break;
            case inVThumb:
               _PutStr("inVThumb\n");
               break;
            case inHUpArrow:
               _PutStr("inHUpArrow\n");
               break;
            case inHDownArrow:
               _PutStr("inHDownArrow\n");
               break;
            case inHPageUp:
               _PutStr("inHPageUp\n");
               break;
            case inHPageDown:
               _PutStr("inHPageDown\n");
               break;
            case inHThumb:
               _PutStr("inHThumb\n");
               break;
            case inMenuBar:
               _PutStr("inMenuBar\n");
               break;
        }

}


/*      This procedure will check the which Modifiers were pressed when
        a keyDownEvent was received.  It then will print out the
        combination of the modifiers.                                   */

ChkModifiers(short modifiers)
{

    int         others=FALSE;

        if (modifiers & altKey)         /* The Alt Key was pressed */
        {
                if (modifiers & ctrlKey)     /* The Ctrl Key */
                {
                    _PutStr("Ctrl");
                    others = TRUE;
                }

                if (modifiers & shiftKey)      /* The Shift Key */
                {
                    if (others)
                        _PutChr('+');
                    _PutStr("Shift");
                    others = TRUE;
                }


                if (others)
                    _PutChr('+');
                _PutStr("Alt");


        }
        else
        {
            if (modifiers & ctrlKey)         /* Ctrl w/o Alt */
                if (modifiers & shiftKey)     /* check for Shift */
                    _PutStr("Ctrl+Shift");
                else
                    _PutStr("Ctrl");
            else
                if (modifiers & shiftKey)     /* Shift w/o Alt and Ctrl */
                    _PutStr("Shift");
        }


        _PutChr(0x0a);
        others = FALSE;


}

// Print the Event record's objects to the current output port.

PrintEvent(EventRec FAR *ev, char FAR *whatevent)
{


/*  The following is the definition for the Event Record which is
    defined in pro_ext.h.


        Event record definitions
        typedef struct
        {
        unsigned short      what;            Event code
        Ticks               when;            Ticks since startup
        Point               where;           Mouse location
        unsigned long       message;         Key/window
        unsigned long       misc;            Event dependant misc info
        unsigned long       misc2;           Event dependant misc info
        unsigned char       mbState;         Mouse buttons state
        unsigned char       mcState;         Mouse cond activate state
        unsigned short      modifiers;
        } EventRec;

*/


    ChkNulls();                 // Check for null events
    _PutStr("\n\n*******  New Event ********\n\n");

    _PutStr("What Event -> ");
    _PutStr(whatevent); nl;

    _PutStr("Horizontal mouse location -> ");
    PutInt((long) ev->where.h); nl;

    _PutStr("Vertical mouse location -> ");
    PutInt((long) ev->where.v); nl;

    _PutStr("Event message -> ");
                                        // check if a regular key was pressed
    if (ev->what == keyDownEvent && !(ev->modifiers & shiftCodeMask))
	   _PutChr((char) ev->message);
    else
        PutInt(ev->message); nl;

    _PutStr("Event misc info -> ");
    PutInt(ev->misc); nl;

    _PutStr("Event misc2 info -> ");
    PutInt(ev->misc2); nl;

    if (ev->mbState == leftButton)
        _PutStr("Mouse button state ->  Left mouse button\n");


    _PutStr("Event modifiers -> ");
    ChkModifiers(ev->modifiers);


}


//
// This routine is registered as an event handler.
//

FAR EventHandler(WHandle theWindow, EventRec FAR *ev)
{



    switch(ev->what)
    {
	case nullEvent:
           num_nulls++;
           return NO;

	case mouseDownEvent:
           PrintEvent(ev, "mouseDownEvent");
           PrintFindWindow(&theWindow, ev->where);
           return NO;
           break;

	case keyDownEvent:
           PrintEvent(ev, "keyDownEvent");
           return NO;
           break;

        case activateEvent:
           PrintEvent(ev, "activateEvent");
           return NO;
           break;

        case deactivateEvent:
           PrintEvent(ev, "deactivateEvent");
           return NO;
           break;

        case menuHitEvent:
           PrintEvent(ev, "menuHitEvent");
           return NO;
           break;

        case closeEvent:
           PrintEvent(ev, "closeEvent");
           return NO;
           break;

        case hideEvent:
           PrintEvent(ev, "hideEvent");
           return NO;
           break;

        case showEvent:
           PrintEvent(ev, "showEvent");
           return NO;
           break;

        case hotkeyEvent:
           PrintEvent(ev, "hotkeyEvent");
           return NO;
           break;

        case sizeEvent:
           PrintEvent(ev, "sizeEvent");
           return NO;
           break;

        case zoomEvent:
           PrintEvent(ev, "zoomEvent");
           return NO;
           break;


	default:
	    return NO;
    }
    return YES;
}


/*      This is our procedure which sets up the event handler and creates
        the text file to capture all of the events.                     */

FAR Event()
{

    eventid = _ActivateHandler(EventHandler);
    _Execute("SET PRINT TO EVENT.TXT");
    _Execute("SET PRINT ON");

}



/*      When we unload the library, we cleanup our mess!  First deactivating
        the event handler and then closing our text file.               */

FAR EventExit()
{

    _DeActivateHandler(eventid);
    _Execute("SET PRINT OFF");
    _Execute("SET PRINT TO");

}

FoxInfo myFoxInfo[] = {
	{"EVENT", (FPFI) Event, CALLONLOAD, ""},
	{"EVENTEXIT", (FPFI) EventExit, CALLONUNLOAD, ""}
};

FoxTable _FoxTable = {
    (FoxTable FAR *)0, sizeof(myFoxInfo) / sizeof(FoxInfo), myFoxInfo
};
