/*
 * RESOURCE.H
 * Freeloader Chapter 6
 *
 * Definitions specifically pertaining to resources.
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */



#ifndef _RESOURCE_H_
#define _RESOURCE_H_


//Menu command identifiers.

#define IDM_EDITSIZETOGRAPHIC   (IDM_CUSTOMEDITMIN)

/*
 * IDs for StatStrip handling of popup menus:  must be in order of menu
 * CLASSRES.H already defines ID_MENUFILE and ID_MENUEDIT for us.
 */

//These are specifically for the StatStrip
#define IDS_STATMESSAGEMIN                  IDS_STANDARDSTATMESSAGEMIN
#define IDS_ITEMMESSAGEEDITSIZETOGRAPHIC    (IDS_CUSTOMSTATMESSAGEMIN+0)
#define IDS_STATMESSAGEMAX                  (IDS_CUSTOMSTATMESSAGEMIN+0)


#endif //_RESOURCE_H_
