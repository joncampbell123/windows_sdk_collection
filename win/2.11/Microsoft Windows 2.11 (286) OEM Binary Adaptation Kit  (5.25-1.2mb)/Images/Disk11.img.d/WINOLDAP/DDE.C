/*==========================================================================

DDE -- Dynamic Data Exchange module for WinOldAp.

Description	WinOldAp's DDE interface provides a way for a Windows
		application called a shell to control an old MS-DOS
		application. The shell can do anything a user can
		do. For example, type in commands and cut and paste.
		In addition, a shell can add marcos and menus.

		This file contains all the code which handles the DDE
		protocal. In addtion, it contains code which handle the
		WinOldApInfo, WindowInfo and OldApState objects.
		Menus, macros and command parsing is handled elsewhere.

		This module is code in C to meet schedule and enhance
		readability. It follows the Microsoft Windows naming
		conventions with additional rule that local variables
		always start with a lower case letter. Global variables
		don't follow any convention after all this is WinOldAp.
		In addition, all exported global functions start with DDE.


See Also	The menu, macro, and parser code.

History 	$Author:   rcp  $
		$Revision:   1.14  $
		$Date:   11 Nov 1987 17:07:24  $

===========================================================================*/
#include <windows.h>
#include "dde.h"
#define MAX_STR_LEN	40
#define BAD_BITS	0x95





/*===================================================================
 *
 * Global functions
 *
 */

/*  Exported functions
 */
EXPORT void	DDEModuleInit ();	    /* One time DDE initialization  */
EXPORT void	DDEStartUp ();		    /* DDE initialization	    */
EXPORT void	DDEExit ();		    /* App terminated event	    */
EXPORT void	DDESwitchIn (); 	    /* User switched in 	    */
EXPORT void	DDESwitchOut ();	    /* User switched out	    */
EXPORT void	DDEFinishMacro (WORD);	    /* Called when macro is finised */

/*  Function imported from other WinOldAp modules
 */
IMPORT WORD	ExecuteMacro( WORD );	    /* Parser module		    */

IMPORT WORD	UpdateMacro( LPSTR );	    /* Macro module		    */
IMPORT long	GetMacroInfo( WORD, LPSTR );
IMPORT long	GetMacroList( WORD, LPSTR );
IMPORT long	SetMacroSize( WORD );

IMPORT WORD	UpdateMenu( LPSTR );	    /* Menu module		    */
IMPORT long	GetMenuInfo( WORD, LPSTR );
IMPORT long	GetMenuList( WORD, LPSTR );
IMPORT void	SetMenuSize( WORD );
IMPORT void	DisableGrabber( void );

IMPORT void	ProcessMessage2();	    /* TTYFUNC			    */

IMPORT void	OpenDirect();		    /* TTYTASK			    */

short far pascal lstrlen( LPSTR );	    /* Kernel string functions	    */
short far pascal lstrcpy( LPSTR, LPSTR );
short far pascal lstrcat( LPSTR, LPSTR );





/*===================================================================
 *
 * Global data
 *
 */
IMPORT WORD	wBinaryFormat;		    /* "Binary" format              */
IMPORT	char	*szAppName;		    /* Application Name 	    */
IMPORT	char	InBuf[];		    /* Command line passed	    */
IMPORT	HWND	hTTYWnd;		    /* WinOldAp's windows           */
IMPORT	WNDCLASS TTYClass;		    /* A WNDCLASS structure	    */
IMPORT	HANDLE	hWoaInst;		    /* WinOldAp's instance handle   */
IMPORT	short	OldApState;		    /* Set when the app has term.   */
IMPORT	BYTE	pifBehavior;		    /* Indicates the type of app    */
IMPORT	WORD	pifFB;			    /* The Behavoir and Flags combo */
IMPORT	WORD	IconIDLen;		    /* Number of ticks in the icon  */
IMPORT	WORD	fIcon;			    /* Current state of the Window  */
IMPORT	WORD	wWoaExitCode;		    /* Exit code for WOA	    */
IMPORT	WORD	wReturnCode;		    /* Return code for the App	    */
IMPORT	BOOL	fWindowsEnabled;	    /* Indicates wether windows is
					       around			    */

	WORD	wMenuSize = DM_MENU;	    /* The size requested by the    */
	WORD	wMacroSize = DM_MACRO;	    /*	 shells for menus and macros*/
	WORD	fUserLock = 0;		    /*	action can take place	    */


/*--- DDE MessageHandlers -------
 */
void	(*DDEMessageHandlers[NUM_DDE_MESSAGES])(WORD, long) =
		{
		Initiate,
		IdentifyTerminate,
		Advise,
		Unadvise,
		InitiateAck,
		Data,
		Request,
		Poke,
		Execute
		};




/*======================================================================
 *
 * Local data
 *
 */

/*---Message Varibles------------
 */
LOCAL long	    lPassParam;

/*---DDE Conversation state------
 */
LOCAL CONVSTATE     convState;		    /* Maintained by GetNextAction */
LOCAL HWND	    hShellWnd;		    /* Set by Initiate */
LOCAL WORD	    lastUserAction = UF_STARTED;
LOCAL WORD	    fPassedUserLock = 0;

/*---Update Links----------------
 */
LOCAL PLINK	    activeLinks = NULL;     /* List of active advisories    */
LOCAL ATOM	    aStateObj = NULL;	    /* "OldApState" atom            */

/*---Requesting shell attributes--
 */
LOCAL SHELL	    *activeShells = NULL;   /* List of all of current shells */
LOCAL SHELL	    *oldShells = NULL;	    /* Shells which allready had control */
LOCAL short	    highestPriority;	    /* Current highest shell		    */



/*---Objects-------------
 */
OBJ		    namedObjs[NUM_NAMED_OBJS] =
			{
			{"OldApState", GetOldApState},
			{"WinOldApInfo", GetWoaInfo},
			{"MacroList", GetMacroList},
			{"MenuList", GetMenuList},
			{"WindowInfo", GetWindowInfo},
			{"SysItems", GetSysItems},
			{"Topics", GetTopics},
			{"Formats", GetFormats}
			};

UOBJ		     updateObjs[ NUM_UPDATE_OBJS] =
			{
			{"Macro", UpdateMacro},
			{"Menu", UpdateMenu},
			{"OldApState", UpdateOldApState},
			{"WindowInfo", UpdateWindowInfo}
			};

/*-- Identify variables ----------
 */
LOCAL ATOM	    aShell,
		    aTopic;

/*-- Shell Information variables --
 */
LOCAL ATOM	    aShellInfo;

/*-- WoaInfo variables ------------
 */
LOCAL ATOM	    aWoaInfo = 0;
LOCAL HANDLE	    hWoaInfo = NULL;
LOCAL WORD	    wNumActive;
LOCAL WORD	    wNumGranted;
LOCAL short	    commandLineSize;
LOCAL char	    *szCommandLine;
LOCAL BOOL	    fCommandLineValid;

/*-- Posted Data variables --------
 */
LOCAL ATOM	    aDataAck;
LOCAL HANDLE	    hDataAck;
LOCAL long	    lAckParam;

/*-- Execute variables ------------
 */
LOCAL BYTE	    fExecuteMacro;

/*-- WindowInfo -------------------
 */
LOCAL HICON	    hIcon;

/*-- System Conversation ----------
 */
LOCAL HWND	    hSystemWnd = NULL;
LOCAL BOOL	    bSystemClass;
LOCAL char	    szSysItems[] = "SysItems\tTopics\tFormats";
LOCAL char	    szFormats[] = "Binary";
LOCAL char	    szSys[] = "System";
LOCAL PSHELL	    systemShells = NULL;	/* Shells in the system topic */

/*-- Strings ----------------------
 */
LOCAL char	    szWoa[] = "WinOldAp";
LOCAL char	    szShell[] = "Shell";
LOCAL char	    szBinary[] = "Binary";

/*======================================================================
 *
 * Identify Conversations Functions
 *
 *	In the order they are called.
 *
 */


/*------------------------------------------------------------------------

DDEStartUp()

Purpose     Does all the initialization when DDE started.

Results     The hSystemWnd is created for the System conversation.
	    OldApState is not set to SF_NOT_STARTED.

Notes	    This function introduces a new state for OldAp called StartUp.
	    The StartUp state is entered after a WinOldAp window is created
	    but before the old application is exec'ed. The startup state
	    is left when:

		1) There no DDE shells which respond to our topic.
		2) An execute command was sent by a shell.
		3) The shell which is set for control doesn't exist
		   anymore. (Exceptional condition)
		4) A WM_CLOSE message was recieved. (Exceptional condition)

	    When the startup state is left, the old applicaiton is
	    exec'ed, unless a WM_CLOSE message was received. In this case,
	    Winoldap exits.

--------------------------------------------------------------------------*/
void DDEStartUp()
    {
    MSG  msg;

     /* Create the system window for the system conversation. This
       window will never be made visible. It will be deleted
       when the DDEExit is called.
     */
    if( bSystemClass )
	hSystemWnd = CreateWindow(  (LPSTR) szWoa,
				(LPSTR) szWoa,
				WS_OVERLAPPED,
				0, 0, 0, 0,
				NULL,
				NULL,
				hWoaInst,
				NULL
				);

    /* Identify ourselves to any shells out there.
     */
    StartIdentify();

    /* Wait for a reason to leave the start up state.
     */
    while( OldApState == SF_NOT_STARTED )/* TTYClose and Execute can change OldApState */
	{
	if( convState != Identify &&
	    (activeShells == NULL || !IsWindow( activeShells->hShellWnd ))
	  )
	    /* Our master died: continue on with our duties and start up */
	    {
	    convState = None;
	    break;
	    }
	ProcessMessageTwo();
	}
    }


/*-------------------------------------------------------------------------

StartIdentify( )

Purpose     Starts an identify conversation with any shells out there.

Notes	    This function is called when an old application starts
	    and when a control conversation terminates.

---------------------------------------------------------------------------*/
LOCAL void StartIdentify()
    {
    PSHELL  pShell, pNext;

    /* Clear activeShells */
    for( pShell = activeShells; pShell; pShell = pNext )
	{
	pNext = pShell->next;
	DeleteTuple( (PTUPLE *)&activeShells, (PTUPLE)pShell );
	}

    /* Start an identify conversation with szAppName as the topic
     */
    DDEMessageHandlers[DDE_ACK] = InitiateAck;
    DDEMessageHandlers[DDE_TERMINATE] = IdentifyTerminate;
    aTopic = GlobalAddAtom( (LPSTR)szAppName );
    aShell = GlobalAddAtom( (LPSTR)szShell );
    SendMessage( -1, WM_DDE_INITIATE, hTTYWnd, MAKELONG( aShell, aTopic) );
    GlobalDeleteAtom( aTopic );
    GlobalDeleteAtom( aShell );

    /* If no shells responds, return
     */
    if( activeShells == NULL )
	return;
    /* Shells responded, get information about each shell.
     */
    RequestShellInfo();
    }


/*-------------------------------------------------------------------------

InitiateAck( hSenderWnd, lParam )

Purpose     Handles an WM_DDE_ACK message the initiate message.

Parameters  hSenderWnd - Handle of the shell window.
	    lParam - The lParam of the message.

Notes	    I rely on the shells to be well behaved and process their
	    requests in order as specified in the DDE document.

---------------------------------------------------------------------------*/
void InitiateAck( hSenderWnd, lParam )
HWND	hSenderWnd;
long	lParam;
    {
    SHELL   *pShell;
    ACTION  action;

    /* Delete the ACK atoms */
    GlobalDeleteAtom( LOWORD(lParam) );
    GlobalDeleteAtom( HIWORD(lParam) );

    /* Initiate ACK: If the shell hasn't already had control,
		    add the shell to the list of requesting shells
		    and update the action.
    */
    if( !FindTuple( (PTUPLE)oldShells, hSenderWnd ) )
	{
	pShell = (PSHELL) AddTuple( (PTUPLE *)&activeShells, hSenderWnd );
	if( pShell )
	    action = GetNextAction( pShell );
	else
	    /* Out of local space */
	    PostMessage( hSenderWnd, WM_DDE_TERMINATE, hTTYWnd, 0L );
	}
    else
       /* This shell has already had control and shouldn't get it
	  again. */
	PostMessage( hSenderWnd, WM_DDE_TERMINATE, hTTYWnd, 0L );
    }


/*----------------------------------------------------------------------

RequestShellInfo()

Purpose     Requests a ShellInfo structure from all of the active shells.

Notes	    This also has the responsiblity of deleting the WoaInfo
	    data structure and atom.

See Also    PokeWoaInfo() and TerminateAllConv()

------------------------------------------------------------------------*/
LOCAL void RequestShellInfo()
    {
    SHELL	*pShell;

    /* Reset the menu and macro sizes
     */
    wMenuSize = DM_MENU;
    wMacroSize = DM_MACRO;

    /* Send an "ShellInfo" request to everybody. If the responding
       data message requests an ACK, the transaction atom will be
       deleted by the shell. Consequently, each transaction needs its
       own atom. Otherwise, the atom must be deleted by WinOldAp.
       Data and IdentifyTerminate delete the atom.

       Negative ACKs to a "ShellInfo" request should not happen
       since all shells will respond to a shellinfo request.
       Just in case, we will terminate the conversation if this happens.
     */
    DDEMessageHandlers[DDE_ACK] = IdentifyTerminate;
    for( pShell = activeShells; pShell; pShell = pShell->next )
	{
	aShellInfo = GlobalAddAtom( (LPSTR)"ShellInfo" );
	PostMessage( pShell->hShellWnd, WM_DDE_REQUEST, hTTYWnd, MAKELONG( wBinaryFormat, aShellInfo ) );
	}
    }

/*-------------------------------------------------------------------------

Data( hSenderWnd, lParam )

Purpose     Handles data messages. Data messages are only sent by the
	    shell during the identify conversation. The only data
	    messages which WinOldAp understands is "ShellInfo."

Parameters  hSenderWnd - Handle of the shell window.
	    lParam - The lParam of the message.

Notes	    Data will process the message according to the header
	    mode word. If appropriate the transaction atom is deleted
	    here.

---------------------------------------------------------------------------*/
void Data( hSenderWnd, lParam )
HWND	hSenderWnd;
long	lParam;
    {
    register SHELL	*pShell;
    SHELL		*pNewShell;
    LPSHELLINFO 	lpShellInfo;
    WORD		wMode;
    short		shellPriority;

    /* Make sure about the message is ok. If it isn't, we need to
       terminate the conversation
     */
    lpShellInfo = (LPSHELLINFO) GlobalLock( LOWORD( lParam ) );
    if( !lpShellInfo || lpShellInfo->wFormat != wBinaryFormat )
	{
	IdentifyTerminate( hSenderWnd, 0L );
	return;
	}

    /* Prioritize (0-11) this shell according to the following order:

	- Exclusive, Permanent, execing, specific	(11 highest)
	- Exclusive, Permanent, execing, general
	- Exclusive, Permanent, specific
	- Exclusive, Permanent, general
	  (No Exclusive and Temporary shells)
	- Temporary, general
	- Temporary, specific
	- Temporary, execing, general
	- Temporary, execing, specific
	- Permanent, execing, specific
	- Permanent, execing, general
	- Permanent, specific
	- Permanent, general				(0 lowest)
     */
    pShell = (PSHELL) FindTuple( (PTUPLE)activeShells, hSenderWnd );
    pShell->priority = lpShellInfo->wAttributes & AF_SPECIFIC;
    if( lpShellInfo->hInstance == hWoaInst )
	pShell->priority |= 2;
    if( lpShellInfo->wAttributes & AF_TEMPORARY )
	pShell->priority ^= 7;
    if( lpShellInfo->wAttributes & AF_NOTEMPS )
	pShell->priority += 8;

    /* If this shell has a higher prority than the current highest,
	 keep track of it as the highest
     */
    if( activeShells->priority < pShell->priority )
	{
	/* Move the shell to the top of the list */
	pNewShell = (PSHELL) AddTuple( (PTUPLE *)&activeShells, hSenderWnd );
	if( pNewShell )
	    {
	    pNewShell->subState = pShell->subState;
	    pNewShell->priority = pShell->priority;
	    DeleteTuple( (PTUPLE *)&activeShells, (PTUPLE)pShell );
	    }
	}

    /* Adjust the macro and menu size requirements
     */
    wMacroSize += lpShellInfo->wMacroSize;
    wMenuSize += lpShellInfo->wMenuSize;

    /* Unlock the shared link.
     */
    wMode = lpShellInfo->wMode;
    GlobalUnlock( LOWORD( lParam ));

    /* Respond according to the header mode.
     */
    if( wMode & MF_ACKREQUIRED )
	PostAck( MAKELONG( POSITIVE_ACK, HIWORD( lParam ) ) );
    else
	{
	if( wMode & MF_RELEASE )
	    GlobalFree( (HANDLE) LOWORD( lParam ) );
	GlobalDeleteAtom( aShellInfo );
	}

    /* Process the message according to GetNextAction
     */
    if( GetNextAction( pShell ) == PokeInfo )
	PokeWoaInfo();
    }

/*----------------------------------------------------------------------

PokeWoaInfo ()

Purpose     Pokes a WoaInfo data structure to all of the active shells.

Notes	    Since all of the ShellInfo message have been received,
	    wMenuSize and wMacroSize are valid. We can now inform
	    the Menu and Macro manager of their values.

See Also    RequestShellInfo() and TerminateAllConv()

------------------------------------------------------------------------*/
LOCAL void PokeWoaInfo()
    {
    SHELL	*pShell;

    /* Inform the menu and macro managers of the requested
       size values.
     */
    SetMacroSize( wMacroSize );
    SetMenuSize( wMenuSize );


    /* Setup a WOAINFO data structure and atom.  This structure
       will be deleted by TerminateAllConv which is also called by both
       WoaInfoAck and IdentifyTerminate when no more reponses are needed.
       I count on the shells being well behaved and not deleting
       this structure.
     */
    pShell = activeShells;
    if( pShell != NULL )
	{
	aWoaInfo = GlobalAddAtom( (LPSTR)"WinOldApInfo" );
	hWoaInfo = (HANDLE) GetDDEObject(aWoaInfo, MF_ACKREQUIRED, wBinaryFormat);

	/* Set up the Ack handler */
	DDEMessageHandlers[DDE_ACK] = WoaInfoAck;

	/* Post the WoaInfo to all the shells */
	for( ; pShell; pShell = pShell->next )
	    {
	    if( hWoaInfo )
		PostMessage( pShell->hShellWnd, WM_DDE_POKE,
			     hTTYWnd, MAKELONG( hWoaInfo, aWoaInfo ) );
	    else
		/* No memory for posting WinOldAp Info: we need to terminate
		   this conversation in a negative manner. To save code size,
		   I will let IdentifyTerminate handle this */
		IdentifyTerminate( pShell->hShellWnd, 0L );
	    }
	}
    }


/*-------------------------------------------------------------------------

WoaInfoAck( hSenderWnd, lParam )

Purpose     Handles an WM_DDE_ACK message the poke of WOAINFO.

Parameters  hSenderWnd - Handle of the shell window.
	    lParam - The lParam of the message.

Notes	    I rely on the shells to be well behaved and process their
	    requests in order as specified in the DDE document.

---------------------------------------------------------------------------*/
void WoaInfoAck( hSenderWnd, lParam )
HWND	hSenderWnd;
long	lParam;
    {
    SHELL   *pShell;

    /* Is it positive or negative?
     */
    if( LOWORD( lParam ) & ACK_BIT )
	{
	/* Positive Ack: Terminate the conversation keeping the shell
			 on the active list
	 */
	pShell = (PSHELL) FindTuple( (PTUPLE)activeShells, hSenderWnd );
	if( GetNextAction( pShell ) == TerminateAll )
	    TerminateAllConv();
	}
    else
       /* Negative Ack: The shell does not like the WoaInfo.
			Delete the shell from the list of active
			shells and terminate the conversation.
			IdentifyTerminate will do this. */
	IdentifyTerminate( hSenderWnd, 0L );
    }

/*----------------------------------------------------------------------

TerminateAllConv()

Purpose     Terminates the identify conversation with all of the active
	    shells. Frees all global resouces belonging to WOA and the
	    identify converstation.

Notes

See Also    RequestShellInfo() and PokeWoaInfo()

------------------------------------------------------------------------*/
LOCAL void TerminateAllConv()
    {
    SHELL	*pShell;
    SHELL	*pNext;

    /* Delete the WinOldAp Info structure */
    if( hWoaInfo != NULL )
	{
	GlobalFree( hWoaInfo );
	GlobalDeleteAtom( aWoaInfo );
	hWoaInfo = NULL;
	}

    /* Terminate the converation with the active shells */
    convState = None;
    for( pShell = activeShells; pShell; pShell = pNext )
	{
	/* If waiting for ShellInfo, then delete the aShellInfo atom. */
	if( pShell->subState == WaitForData )
	    GlobalDeleteAtom( aShellInfo );

	PostMessage( pShell->hShellWnd, WM_DDE_TERMINATE, hTTYWnd, 0L);
	pNext = pShell->next;
	}
    }

/*---------------------------------------------------------------------

IdentifyTerminate( hShellWnd, lParam );

Purpose     Handles the any terminate message sent to WinOldAp during
	    the Identify conversation.

	    This is an abort terminate sent by the shell.
	    In this case we need to delete the shell from the
	    active shells list and terminate the associated conversation.
	    We also need to check if there any action which needs to be
	    perform as a result of this.

Parameters  hSenderWnd - Handle of the shell window.
	    lParam - The lParam of the message.

-----------------------------------------------------------------------*/
void IdentifyTerminate( hSenderWnd, lParam )
HWND	hSenderWnd;
long	lParam;
    {
    SHELL   *pShell;
    ACTION  action;

    pShell = (PSHELL) FindTuple( (PTUPLE)activeShells, hSenderWnd );
    action = GetNextAction( pShell );

    /* If waiting for ShellInfo, then delete the aShellInfo atom. */
    if( pShell->subState == WaitForData )
	GlobalDeleteAtom( aShellInfo );

    /* Delete the shell structure */
    DeleteTuple( (PTUPLE *)&activeShells, (PTUPLE)pShell );

    /* If no more shells are out there, end conversation. */
    if( activeShells == NULL )
	convState = None;

    /* Execute the action. */
    switch( action )
    {
    case PokeInfo:
	PokeWoaInfo();
	break;

    case TerminateAll:
	TerminateAllConv();	/* Deletes the WoaInfo structure */
	break;
    }
    PostMessage( hSenderWnd, WM_DDE_TERMINATE, hTTYWnd, 0L );
    }



/*---------------------------------------------------------------------

GetNextAction( pShell ) : action

Purpose     GetNextAction looks at the sub state of pShell and determines
	    what action if any needs to be taken.

Parameters  pShell - pointer to a SHELL structure.

Result	    An ACTION enumurated type indicating the action which
	    needs to be performed.

Notes	    GetNextAction syncronizes the conversation between all of
	    the shells.

-----------------------------------------------------------------------*/
LOCAL ACTION GetNextAction( pPassedShell )
SHELL *pPassedShell;
    {
    SHELL *pShell;

    /* Find out what sub state we are in */
    switch( pPassedShell->subState )
	{
	case Init:
	    convState = Identify;
	    pPassedShell->subState = WaitForData;
	    return NoAction;

	case WaitForData:
	    pPassedShell->subState = WaitForAck;

	    /* If there are any other datas out there: NoAction
	       Here we also count the number of active shells
	       for wNumActive which could be referenced in the
	       following control conversation. */
	    wNumActive = 0;
	    for( pShell = activeShells; pShell; pShell = pShell->next )
		{
		wNumActive++;
		if( pShell->subState == WaitForData )
		    return NoAction;
		}

	    /* No more data messages out there: Poke WoaInfo
	       Note also that if we are here wNumActive will be valid. */
	    return PokeInfo;

	case WaitForAck:
	    pPassedShell->subState = ReadyForTerm;

	    /* If there are any other acks out there: NoAction */
	    for( pShell = activeShells; pShell; pShell = pShell->next )
		if( pShell->subState == WaitForAck )
		    return NoAction;

	    /* No more acks out there: RequestShellInfo */
	    return TerminateAll;

	/* Normally, no action is requested when the shell is in this
	   state. */
	case ReadyForTerm:
	    return NoAction;
	}
    }







/*==================================================================
 *
 * Control Conversation Functions
 *
 */

/*-----------------------------------------------------------------------

Initiate( hSenderWnd, lParam )

Purpose     Handles the WM_DDE_INITIATE messages. Initiate messages
	    are sent by a shell to start a control conversation. WinOldAp
	    will only respond if the topic is matches its application and
	    if there are no other converstaions active.

Parameters  hSenderWnd - Handle of the shell window.
	    lParam - The lParam of the message.

-------------------------------------------------------------------------*/
EXPORT void Initiate( hSenderWnd, lParam )
HWND	hSenderWnd;
long	lParam;
    {
    ATOM    aTopic, aServer;

    /* Only respond to the highest priority shell. Handle only one
       control conversation at a time
     */
    if( ( convState == None ) &&
	( OldApState < SF_TERMINATED ) &&
	( activeShells == NULL || activeShells->hShellWnd == hSenderWnd ) &&
	( LOWORD(lParam) == GlobalFindAtom((LPSTR)szWoa) || LOWORD(lParam) == NULL ) &&
	( HIWORD(lParam) == GlobalFindAtom((LPSTR)szAppName) || HIWORD(lParam) == NULL )
      )
	{
	/* This is it! Setup the message handlers */
	DDEMessageHandlers[DDE_ACK] = ControlAck;
	DDEMessageHandlers[DDE_TERMINATE] = ControlTerminate;

	/* Make sure this shell is not used again */
	if( AddTuple( (PTUPLE *)&oldShells, hSenderWnd ) )
	    {
	    /* Increase the NumGranted count */
	    wNumGranted++;

	    /* Update conversation state */
	    hShellWnd = hSenderWnd;
	    convState = Control;

	    /* Re add our topic and server atoms. The shell's ACK handler
	       will delete them.
	     */
	    aTopic = GlobalAddAtom( (LPSTR)szAppName );
	    aServer = GlobalAddAtom( (LPSTR)szWoa );

	    /* Acknowledge */
	    SendMessage( hSenderWnd, WM_DDE_ACK, hTTYWnd, MAKELONG( aServer, aTopic ) );
	    }
	else
	    DeleteTuple( (PTUPLE *)&activeShells, FindTuple( (PTUPLE) activeShells, hSenderWnd ) );
	}
    }

/*---------------------------------------------------------------------

ControlTerminate( hShellWnd, lParam );

Purpose     Handles the any terminate message sent to WinOldAp during
	    the Control conversation.

Parameters  hSenderWnd - Handle of the shell window.
	    lParam - The lParam of the message.

-----------------------------------------------------------------------*/
void ControlTerminate( hSenderWnd, lParam )
HWND	hSenderWnd;
long	lParam;
    {
    convState = None;

    /* If the app hasn't terminated start a new identify conversation. */
    if( OldApState < SF_TERMINATED )
	StartIdentify();
    }



/*-------------------------------------------------------------------------

Execute( hSenderWnd, lParam )

Purpose     Handles and WM_DDE_EXECUTE message. This message is sent
	    by the shell old application.

Parameters  hSenderWnd - Handle of the shell window.
	    lParam - The lParam of the message.

Notes	    If a command is passed, Execute forms a ExecuteMacro with
	    the command and hands it off to ExecuteMacro. It is necessary to
	    form a macro with the command because the macro memory is
	    present in extended mode, while global memory is not.

	    While the ExecuteMacro is being executed, the DDE module is
	    in a busy state where all hot-link updates are postponed until
	    the busy state is exited. This ensures
	    the synchronous behavior of WinOldAp.

--------------------------------------------------------------------------*/
EXPORT	void Execute( hSenderWnd, lParam )
HWND	hSenderWnd;
long	lParam;
    {
    short   commandSize;
    MACRO   *pMacro;
    WORD    wStatus;
    short   macroId;

    /* Allow the application to run */
    fUserLock = 0;


    /* If a command is sent, add the command as a macro. */
    if( HIWORD( lParam ) )
	{
	/* Get the command line */
	commandSize = GlobalSize( HIWORD( lParam ) );

	/* Create a macro structure using GlobalAlloc. If I used LocalAlloc
	   I would have to lock down the whole data segment when
	   I passed the macro to UpdateMacro. Note that I cannot
	   delete the command string because I need to return it in the
	   ACK.
	 */
	pMacro = (MACRO *) LocalAlloc( LPTR, sizeof(HEADER) + sizeof(MACRO) + commandSize);
	if( pMacro != NULL )
	    {
	    pMacro->wAction = MODIFY_MACRO;
	    pMacro->wMacroID = EXECUTE_MACROID;
	    lstrcpy( (LPSTR)pMacro->szMacro, GlobalLock(HIWORD(lParam)) );
	    GlobalUnlock( HIWORD(lParam) );

	    /* Add the macro to the macro list*/
	    wStatus = UpdateMacro( (LPSTR)pMacro );

	    /* Free the macro structure used for the update */
	    LocalFree( (HANDLE)pMacro );

	    /* If macro add was successful, then Execute the macro.
	       ExecuteMacro returns immediately. When an Int16
	       call is made the macro will be executed. No ACK will
	       be sent now. We must wait for the macro to finish
	       executing.
	     */
	    if( wStatus == NO_ERROR )
		{
		DisableGrabber();			/* Woa convention */
		ExecuteMacro( EXECUTE_MACROID );
		fExecuteMacro = TRUE;			/* Remember */
		hDataAck = HIWORD( lParam );
		}
	    else
		PostAck( MAKELONG(wStatus, HIWORD(lParam)));
	    }
	else
	    PostAck( MAKELONG(ERR_NO_MEMORY, HIWORD(lParam)));
	}
    else
	PostAck( MAKELONG(POSITIVE_ACK, HIWORD(lParam)));

    /* Now, we must give the application the input focus. How we do
       this depends on OldApState
     */
    if( OldApState == SF_NOT_STARTED )
	/* Exec the old app. DDEStartup is waiting for OldApState to
	   change. This code must come after UpdateMacro.
	 */
	OldApState = SF_RUNNING;
    else
	/* We have already started. If we are a bad app,
	   we need to open up the icon.
	   (DDE messages are only sent when a bad is susspended).
	*/
	{
	if( pifBehavior & BAD_BITS )
	    OpenDirect();		/* Bypass DDESwitchIn */
	}

    }

/*----------------------------------------------------------------------

DDEFinishMacro( wMacroID )

Purpose     Called whenever a macro is finished executing.

Parameters  wMacroID - The macro ID

Notes	    We need to check for two conditions: One, the macro
	    was executed as a part of a Execute message in which
	    case an ACK must be sent to the shell; Two, a link
	    is established on the macro in which case a notify message
	    is posted.

------------------------------------------------------------------------*/
EXPORT void DDEFinishMacro( wMacroID )
WORD	wMacroID;
    {
    MACRO    macro;

    if( wMacroID == EXECUTE_MACROID && fExecuteMacro )
	{
	fExecuteMacro = FALSE;
	/* This was a execute macro. Release the Macro and post
	   an ACK.
	*/
	macro.wMacroID = EXECUTE_MACROID;
	macro.wAction = MODIFY_MACRO;
	macro.szMacro[0] = '\0';
	UpdateMacro( (LPSTR)&macro );
	PostAck( MAKELONG( POSITIVE_ACK, hDataAck ) );

	/* Events may have occured while the macro was executing.
	   Post any pending link updates.
	*/
	PostPendingEvents();
	}

    else
	/* This was not done while executing a macro,
	   a link might be established on this event. */
	PostEvent( wMacroID );
    }


/*----------------------------------------------------------------------

Poke( hShellWnd, lParam )

Purpose     Handles pokes of macros and menus. An ACK message is posted
	    to the shell indicating the success of the operation.

Parameters  hSenderWnd - Handle of the shell window.
	    lParam - The lParam of the message.

------------------------------------------------------------------------*/
EXPORT void Poke( hSenderWnd, lParam )
HWND	hSenderWnd;
long	lParam;

    {
    LPHEADER	lpData; 	    /* General data header */
    WORD	wStatus;	    /* Valid status */
    WORD	(*pUpdate)(LPSTR);  /* Pointer to the update routine */
    PUOBJ	pObj;		    /* Pointer to an update object */
    WORD	wMode;

    /* Save */
    lPassParam = lParam;

    /* Get the data structure */
    lpData = (LPHEADER) GlobalLock( LOWORD(lParam) );
    wMode = lpData->wMode;

    /* Find out type of data structure was poked.
     */
    if( lpData->wFormat == wBinaryFormat )
	{
	wStatus = ERR_INVALID_OBJECT;
	for( pObj = updateObjs; pObj < &updateObjs[NUM_UPDATE_OBJS]; pObj++ )
	    if( HIWORD(lParam) == GlobalFindAtom((LPSTR)pObj->szName) )
		{
		/* Update the object. UpdateObj refers to an update routine.
		 */
		wStatus = (*pObj->updateObj)((LPSTR)lpData + sizeof(HEADER));
		if( wStatus == SUCCESS )
		    wStatus = POSITIVE_ACK;
		break;
		}
	}
    else
	wStatus = ERR_INVALID_FORMAT;


    /* Release the data structure */
    GlobalUnlock( LOWORD(lParam) );

    /* Respond according to the header */
    if( wMode & MF_ACKREQUIRED )
	PostAck( MAKELONG( wStatus, HIWORD(lParam)) );
    else
	{
	if( wMode & MF_RELEASE )
	    GlobalFree( (HANDLE) LOWORD( lParam ) );
	GlobalDeleteAtom( HIWORD( lParam ) );
	}
    }



/*---------------------------------------------------------------------

Request( hSenderWnd, lParam )

Purpose     Handles the WM_DDE_REQUEST messages. A data or an ack message
	    will be issued in reponse.

Parameters  hSenderWnd - Handle of the shell window.
	    lParam - The lParam of the message.

Notes

------------------------------------------------------------------------*/
EXPORT void Request( hSenderWnd, lParam )
HWND	hSenderWnd;
long	lParam;
    {
    register unsigned message;
    register WORD     loWord;
    DWORD	      lObject;


    /* Make sure the binary format is requested */
    if( LOWORD(lParam) != wBinaryFormat )
	{
	message = WM_DDE_ACK;
	loWord = ERR_INVALID_FORMAT;
	}

    /* Get the object */
    else
	{
	lObject = GetDDEObject( HIWORD(lParam), MF_RELEASE, wBinaryFormat );
	if( LOWORD( lObject ) )
	    {
	    message = WM_DDE_DATA;
	    loWord = LOWORD( lObject );
	    }
	else
	    {
	    message = WM_DDE_ACK;
	    loWord = HIWORD( lObject );
	    }
	}

    PostMessage( hSenderWnd, message, hTTYWnd, MAKELONG( loWord, HIWORD(lParam) ));
    }



/*----------------------------------------------------------------------

Advise( hSenderWnd, lParam )

Purpose     Sets up an update link on a particular data item.

Parameters  hSenderWnd - Handle of the shell window.
	    lParam - The lParam of the message.

Notes	    If successful, the activeLinks list will have a new
	    entry.

See Also    Unadvise and PostEvent.

------------------------------------------------------------------------*/
EXPORT void Advise( hSenderWnd, lParam )
HWND	hSenderWnd;
long	lParam;
    {
    ATOM    aLink;
    PLINK   pLink;
    LPOPTIONS lpOptions;
    WORD    wStatus;
    short   i;
    DWORD   lObject;

    /* Lock the Options header. */
    lpOptions = (LPOPTIONS) GlobalLock( LOWORD(lParam) );

    /* If we have allready set an advisory or the format requested
       is wrong, then NACK. */
    if( lpOptions->wFormat != wBinaryFormat )
	wStatus = ERR_INVALID_FORMAT;
    else if( FindTuple((PTUPLE)&activeLinks, HIWORD(lParam) ) )
	wStatus = ERR_INVALID_OBJECT;

    /* Find out what type of atom it is */
    else if( HIWORD( lParam ) < 0x800 )


	/* Macro: does it exist? */
	if( LOWORD(lObject = (HANDLE) GetDDEObject( HIWORD(lParam), 0, 0))  )
	    {
	    GlobalFree( LOWORD(lObject) );
	    pLink = (PLINK) AddTuple( (PTUPLE *)&activeLinks, HIWORD(lParam));
	    if( pLink )
		{
		aLink = GlobalAddAtom( MAKEINTATOM( HIWORD(lParam)) );
		pLink->wOptions = lpOptions->wOptions;
		wStatus = POSITIVE_ACK;
		}
	    else
		wStatus = ERR_NO_MEMORY;
	    }
	else
	    wStatus = HIWORD(lObject);

    else

	/* OldAp State object: */
	if( HIWORD(lParam) == GlobalFindAtom( (LPSTR)namedObjs[STATE_OBJ].szName ) )
	    {

	    /* We have found a match: Add an advisory to the
	       active list. */
	    aStateObj = GlobalAddAtom( (LPSTR)namedObjs[STATE_OBJ].szName );
	    pLink = (PLINK) AddTuple( (PTUPLE *)&activeLinks, aStateObj );
	    if( pLink )
		{
		pLink->wOptions = lpOptions->wOptions & ~MF_EVENT;
		/* Indicate our success */
		wStatus = POSITIVE_ACK;
		}
	    else
		{
		GlobalDeleteAtom( aStateObj );
		wStatus = ERR_NO_MEMORY;
		}
	    }
	else
	    wStatus = ERR_INVALID_OBJECT;


    /* Release the Options header */
    GlobalUnlock( LOWORD(lParam) );
    GlobalFree( LOWORD(lParam) );

    /* Acknowledge */
    PostAck( MAKELONG( wStatus, HIWORD( lParam )) );
    }



/*-----------------------------------------------------------------------

Unadvise( hSenderWnd, lParam)

Purpose     Handles a WM_DDE_UNADVISE message.

Parameters  hSenderWnd - Handle of the shell window.
	    lParam - The lParam of the message.

See Also    Advise and PostEvent.

-------------------------------------------------------------------------*/
EXPORT void Unadvise( hSenderWnd, lParam )
HWND	hSenderWnd;
long	lParam;
    {
    PLINK   pLink;
    WORD    wStatus;

    /* Delete one link */
    if( LOWORD(lParam) != NULL )
	{
	pLink = (PLINK) FindTuple( (PTUPLE)activeLinks, LOWORD(lParam));
	if( pLink )
	    {
	    GlobalDeleteAtom(pLink->aLink);
	    DeleteTuple( (PTUPLE *)&activeLinks, (PTUPLE)pLink);
	    wStatus = POSITIVE_ACK;
	    }
	else
	    wStatus = ERR_INVALID_OBJECT;
	}

    /* Delete all links */
    else
	{
	wStatus = ERR_INVALID_OBJECT;
	while( activeLinks != NULL )
	    {
	    GlobalDeleteAtom(activeLinks->aLink);
	    DeleteTuple( (PTUPLE *)&activeLinks, (PTUPLE)activeLinks);
	    wStatus = POSITIVE_ACK;
	    }
	}

    /* Send an acknowledgement */
    PostAck( MAKELONG( wStatus, LOWORD(lParam) ));
    }


/*-------------------------------------------------------------------------

ControlAck( hSenderWnd, lParam )

Purpose     Handles the acknowledge messages during the Control
	    conversation.

Parameters  hSenderWnd - Handle of the shell window.
	    lParam - The lParam of the message.

---------------------------------------------------------------------------*/
void ControlAck( hSenderWnd, lParam )
HWND	hSenderWnd;
long	lParam;
    {
    SHELL   *pShell;
    ACTION  action;

    /* Data Ack: This ack is sent during the control conversation.
     */
    if( HIWORD(lParam) == aDataAck )
	{
	GlobalDeleteAtom( aDataAck );
	GlobalFree( hDataAck );
	}
    }



/*-----------------------------------------------------------------------

PostEvent( eventAtom )

Purpose     If an advisory has been set on the passed event, post an
	    event data message to the controlling shell. This function
	    is used for all events.

Notes	    This function may be called when Windows is disabled
	    (ie. bad apps). In this case we cannot post the event
	    immediately. Rather, we set a bit in the link structure
	    to remind us that this event occured. The event will be posted
	    when PostPendingEvents is called.

See also    PostPendingEvents()

-------------------------------------------------------------------------*/
LOCAL void PostEvent( eventAtom )
ATOM	eventAtom;
    {
    PLINK   pLink;
    char    szTopic[MAX_STR_LEN];

    /* Is a link set ? */
    pLink = (PLINK) FindTuple( (PTUPLE)activeLinks, eventAtom );
    if( pLink && convState == Control )
	{
	/* Yes: If windows is not enabled, we must wait for DDESwitchOut...
	 */
	if( !fWindowsEnabled || fExecuteMacro )
	    pLink->wOptions |= MF_EVENT;

	/*	else, we can post the notification immediately.
	 */
	else
	    {
	    pLink->wOptions &= ~MF_EVENT;
	    aDataAck = pLink->aLink;
	    if( pLink->wOptions & MF_NODATA )
		hDataAck = NULL;
	    else
		{
		GlobalGetAtomName( aDataAck, (LPSTR)szTopic, MAX_STR_LEN);
		GlobalAddAtom( (LPSTR)szTopic );
		hDataAck = (HANDLE) GetDDEObject( aDataAck, pLink->wOptions & MF_ACKREQUIRED ? MF_ACKREQUIRED : MF_RELEASE, wBinaryFormat );
		}
	    PostMessage( hShellWnd, WM_DDE_DATA, hTTYWnd, MAKELONG(hDataAck, aDataAck));
	    ProcessMessageTwo();
	    }
	}

    }



/*-----------------------------------------------------------------------

PostPendingEvents( )

Purpose     Try to post any pending events which may have occured but were
	    not posted.

Notes	    This function call PostEvent which may fial. In which case,
	    the events will be posted latter. This function does not
	    post events in the chronological order they occured. This may
	    be considered a defect.

	    This function may or may not be called when Windows is enabled.

-------------------------------------------------------------------------*/
LOCAL void PostPendingEvents( )
    {
    register PLINK	pLink;

    for( pLink = activeLinks; pLink != NULL; pLink = pLink->next )
	if( pLink->wOptions & MF_EVENT )
	    PostEvent( pLink->aLink );
    }




/*======================================================================
 *
 * Control and Identify Functions
 *
 */

/*------------------------------------------------------------------

DDEModuleInit()

Purpose 	Performs all the one time initialization needed for the
		DDE conversations. The system conversation provides a
		context for the DDE conversation. It lists the topics
		formats of the control conversation.

Results 	The "WoaSys" class is registered. The "Binary" format
		is registered.

--------------------------------------------------------------------*/
void DDEModuleInit()
    {
    /* Register the "Binary" clipboard format */
    wBinaryFormat = RegisterClipboardFormat( (LPSTR)szBinary );

    /* Use the already defined TTYClass structure */
    TTYClass.lpfnWndProc = SystemWndProc;
    TTYClass.hInstance = hWoaInst;
    TTYClass.lpszClassName = (LPSTR)szWoa;

    /* Register the class */
    bSystemClass = RegisterClass( &TTYClass );
    }


/*-----------------------------------------------------------------------

DDESwitchIn()

Purpose    Called when the user tries to activate an the old ap. Notifies
	   the shell of this event.

-------------------------------------------------------------------------*/
EXPORT void DDESwitchIn()
    {
    /* This might be called when as a part of a DDE Execute macro
       command in which case it is not a user action and should
       not be reported to the shell
     */
    lastUserAction = UF_SWITCHED_IN;
    PostEvent( aStateObj );
    }

/*-----------------------------------------------------------------------

DDESwitchOut()

Purpose    Called when the user deactivates the old ap. Notifies
	   the shell of this and any other stored events.

Note	   The several events could have happened while a bad app was
	   running:

	     - An Execute command could have finished, so an Ack
	       must be posted.
	     - Several objects (ie macros) on which update links
	       have been set could have changed. In this, case
	       update message need to be posted.


-------------------------------------------------------------------------*/
EXPORT void DDESwitchOut()
    {
    register PLINK   pLink;

    /* First, post any pending acks. */
    if( lAckParam != 0 )
	PostAck( lAckParam );

    /* Next, post pending events */
    PostPendingEvents();	/* This will succede */

    /* Next, post this event */
    lastUserAction = UF_SWITCHED_OUT;
    PostEvent( aStateObj );

    /* Next, make sure that fUserLock is consistent with fPassedUserLock.
       They become inconsitent when a command is executed */
    fUserLock = fPassedUserLock;

    }

/*------------------------------------------------------------------------

DDEExit()

Purpose     Called when the OldAp exited. Terminates any
	    DDE conversations where we are in.

Notes	    This routine should only be called when Windows
	    is enabled.

--------------------------------------------------------------------------*/
EXPORT void DDEExit()
    {
    PSHELL  pShell;
    MSG     msg;

    /* Identify Conversation Active: We are a client. We must
       Terminate all of the conversations we have active. Allow the post
       quit message.
     */
    if( convState == Identify )
	TerminateAllConv();

    /* Control Conversation Active: We are the server. I cannot quit
       until the shell lets me.
     */
    else if ( convState == Control )
	{
	/* post any pending acks */
	if( lAckParam != 0 )
	    PostAck( lAckParam );

	/* Next, post pending events */
	PostPendingEvents();

	/* Advise the shell */
	lastUserAction = UF_TERMINATED;
	PostEvent( aStateObj );

	/* Abort the conversation */
	PostMessage( hShellWnd, WM_DDE_TERMINATE, hTTYWnd, 0L);

	/* Wait for the conversation to terminate. */
	while( convState == Control )
	    ProcessMessageTwo();
	}

    /* Terminate the system converation with the active shells
     */
	if( hSystemWnd != NULL )	/* make sure the window was created */
	    {
	    for( pShell = systemShells; pShell; pShell = pShell->next )
		PostMessage( pShell->hShellWnd, WM_DDE_TERMINATE, hSystemWnd, 0L);

	    /* Wait for each shell to respond with a terminate */
	    while( systemShells != NULL  )
		ProcessMessageTwo();

	    /* Destroy hSystemWnd
	     */
	    DestroyWindow( hSystemWnd );
	    hSystemWnd = NULL;
	    }
	}

/*------------------------------------------------------------------------

PostAck( lParam )

Purpose    Posts an ACK Message to the shell. If Windows is enabled,
	   the message is sent immediately. Otherwise, lAckParam is
	   set with lParam and the message is posted during a switch
	   out.

Parameters lParam - the lParam to be posted. Must not be 0.

--------------------------------------------------------------------------*/
LOCAL void PostAck( lParam )
long	lParam;
    {
    /* Is Windows enabled ? */
    if( fWindowsEnabled )
	{
	if( IsWindow( hShellWnd ) && convState == Control )
	    PostMessage( hShellWnd, WM_DDE_ACK, hTTYWnd, lParam );
	lAckParam = 0;
	}
    else
	lAckParam = lParam;
    }


/*--------------------------------------------------------------------

ProcessMessageTwo()

Purpose    Calls ProcessMessage2 in a C compatible manner.

----------------------------------------------------------------------*/
LOCAL void ProcessMessageTwo()
    {
    register x, y;

    ProcessMessage2();	/* Warning!! This call destroys SI and DI */
    }





/*========================================================================
 *
 * System conversation functions
 *
 */



/*-------------------------------------------------------------------

SystemWndProc( ... )

Purpose 	Window proc for the system window. The system window
		is a invisible window used for the "System" DDE topic.
		All the processing needed for the "System" converstation
		is done here.

Notes		The system conversation only allows items to be requested.
		The system server can handle multiple clients.

---------------------------------------------------------------------*/
LONG FAR PASCAL SystemWndProc( hWindow, message, wParam, lParam )
HWND		hWindow;
unsigned	message;
WORD		wParam;
LONG		lParam;
    {
    HANDLE hList;
    ATOM   aTopic, aServer;

    /* Respond only to DDE messages */
    switch( message )
	{
	case WM_DDE_INITIATE:

	    /* The "System" server can handle multiple clients.
	     */
	    if( ( OldApState != SF_TERMINATED ) &&
		( LOWORD(lParam) == GlobalFindAtom((LPSTR)szWoa) || LOWORD(lParam) == NULL ) &&
		( HIWORD(lParam) == GlobalFindAtom((LPSTR)szSys) || HIWORD(lParam) == NULL )
	      )
		{
		/* Keep track of this shell */
		if( AddTuple( (PTUPLE *)&systemShells, wParam ) )
		    {
		    /* Re add our topic and server atoms. The shell's ACK handler
		       will delete them. */
		    aTopic = GlobalAddAtom( (LPSTR)szSys );
		    aServer = GlobalAddAtom( (LPSTR)szWoa );

		    /* Acknowledge */
		    SendMessage( wParam, WM_DDE_ACK, hWindow, MAKELONG( aServer, aTopic ) );
		    }
		}
	    break;

	case WM_DDE_TERMINATE:
	    /* Remove the shell from the list of active system
	       conversationalists.
	     */
	    DeleteTuple( (PTUPLE *)&systemShells, FindTuple( (PTUPLE) systemShells, wParam ) );
	    break;

	case WM_DDE_REQUEST:
	    hList = GetDDEObject( HIWORD(lParam), MF_RELEASE, CF_TEXT );
	    PostMessage( wParam, WM_DDE_DATA, hWindow, MAKELONG( hList, HIWORD(lParam) ) );
	    break;

	case WM_DDE_POKE:
	case WM_DDE_ADVISE:
	case WM_DDE_UNADVISE:
	case WM_DDE_EXECUTE:
	    /* An illeagel remessage has be sent. Respond with a
	       negative ACK */
	    PostMessage( wParam, WM_DDE_ACK, hWindow, MAKELONG( NEGATIVE_ACK, HIWORD(lParam) ) );
	    break;

	default:
	    DefWindowProc( hWindow, message, wParam, lParam );
	    break;
	}
    }



/*=======================================================================
 *
 * Object Functions
 *
 */

/*-----------------------------------------------------------------------

GetDDEObject( aObject, wMode, cfFormat ) : lObject

Purpose     Returns a handle to a data object which is identified by
	    aObject. The mode flag of the object is set to wMode and
	    the wFormat field contains the "Binary" format.

Parameters  aObject  - ATOM
	    wMode    - WORD
	    cfFormat - Clipboard format

Result	    The LOWORD contains the object handle
	    if aObject exsist. If aObject doesn't exist the HIWORD
	    contains an error code.

------------------------------------------------------------------------*/
LOCAL long GetDDEObject( aObject, wMode, cfFormat )
ATOM	aObject;
WORD	wMode;
WORD	cfFormat;
    {
    POBJ    pObject;
    long    (*getObj)();
    DWORD   dwGetObjRet;
    HANDLE  hData;
    WORD    errorCode;
    LPSTR   lpData;

    /* Use getObj as a flag to indicate a match
     */
    getObj = NULL;

    /* Check for Macros
     */
    if( aObject >= 0x400 && aObject < 0x800 )
	getObj = GetMacroInfo;

    /* Check for Menus
     */
    else if( aObject >= 0x800 && aObject < 0xC00 )
	getObj = GetMenuInfo;

    /* Check for Named objects
     */
    else
	for( pObject = namedObjs; pObject < &namedObjs[NUM_NAMED_OBJS]; pObject++ )
	    if( aObject == GlobalFindAtom((LPSTR)(pObject->szName)) )
		{
		getObj = pObject->getObj;
		break;
		}

    if( getObj != NULL )
	{

	/* Match found: getObj is valid.
	 */
	dwGetObjRet = (WORD) (*getObj)(aObject, (LPSTR)NULL);
	if( LOWORD(dwGetObjRet) != 0 )
	    {
	    hData = GlobalAlloc(GMEM_OBJECT, (long)LOWORD(dwGetObjRet)+sizeof(HEADER));
	    if( hData != NULL )
		{
		lpData = GlobalLock( hData );
		((LPHEADER)lpData)->wMode = wMode;
		((LPHEADER)lpData)->wFormat = cfFormat;
		(*getObj)(aObject, lpData + sizeof(HEADER) );
		GlobalUnlock( hData );
		errorCode = NO_ERROR;
		}
	    else
		/* hData is NULL: No memory is available
		 */
		errorCode = ERR_NO_MEMORY;
	    }
	else
	    {
	    /* Couldn't get the object: use the returned error code
	     */
	    hData = NULL;
	    errorCode = HIWORD( dwGetObjRet );
	    }
	}
    else
	{
	hData = NULL;
	errorCode = ERR_INVALID_OBJECT;
	}

    return MAKELONG( hData, errorCode);
    }



/*----------------------------------------------------------------------

GetOldApState( dummy, lpOldApState ) : wSize

Purpose     Renders the current OldAp state object.

Parameters  dummy -	   The atom corresponding to the object

	    lpOldApState - pointer to the object which needs to be
			   filled. If NULL, don't copy the object.

Return	    LOWORD - wSize of the object.
	    HIWORD - Error code.

Notes	    This function is called twice to render the object. First,
	    to get the object size. Second, to get a copy of the object.

------------------------------------------------------------------------*/
LOCAL long GetOldApState( dummy, lpState )
WORD dummy;
LPSTR lpState;
    {
    STATE    oaState;

    /* If a pointer is passed, copy the state information. */
    if( lpState != NULL )
	{
	/* Setup a temparary oaState */
	((LPSTATE)lpState)->wOAState = OldApState;
	((LPSTATE)lpState)->wLastUserAction = lastUserAction;
	((LPSTATE)lpState)->bUserLock = fPassedUserLock;
	if( OldApState == SF_CLOSED )
	    ((LPSTATE)lpState)->wExitCode = wReturnCode == -1 ? wWoaExitCode | 0x8000 : wReturnCode;
	}

    return MAKELONG( sizeof(STATE), NO_ERROR);
    }


/*----------------------------------------------------------------------

GetWoaInfo( dummy, lpInfo ) : wSize

Purpose     Renders the current WoaInfo object.

Parameters  dummy -  The atom corresponding to the object.

	    lpInfo - pointer to a data structure which needs to be
		     filled. If NULL, don't copy the object.

Return	    LOWORD - wSize of the object.
	    HIWORD - Error code.

Notes	    This function is called twice to render the object. First,
	    to get the object size. Second, to get a copy of the object.

------------------------------------------------------------------------*/
LOCAL long GetWoaInfo( dummy, lpInfo )
WORD dummy;
LPSTR lpInfo;
    {
    WOAINFO    woaInfo;

    /* Make sure the command line is valid */
    if( !fCommandLineValid )
	{
	commandLineSize = lstrlen((LPSTR)InBuf+2) + 1;
	szCommandLine = (char *) LocalAlloc( LPTR, commandLineSize );
	if( szCommandLine == NULL )
	    {
	    szCommandLine = "";
	    commandLineSize = 1;
	    }
	else
	    lstrcpy((LPSTR)szCommandLine, (LPSTR)InBuf+2);
	fCommandLineValid = TRUE;
	}

    /* If a pointer is passed, copy the state information. */
    if( lpInfo != NULL )
	{
	/* Setup a temparary woaInfo */
	((LPWOAINFO)lpInfo)->wNumActive = wNumActive;
	((LPWOAINFO)lpInfo)->wNumGranted = wNumGranted;
	((LPWOAINFO)lpInfo)->wNumOther = IconIDLen;
	((LPWOAINFO)lpInfo)->hInstance = hWoaInst;
	((LPWOAINFO)lpInfo)->fPifBehavior = pifFB;

	/* Copy over the CommandLine Info */
	lstrcpy((LPSTR)((LPWOAINFO)lpInfo)->szCommandLine, (LPSTR)szCommandLine);
	}

    return MAKELONG(sizeof(WOAINFO) + commandLineSize, 0);
    }


/*----------------------------------------------------------------------

GetWindowInfo( dummy, lpInfo ) : wSize

Purpose     Renders the current WindowInfo object.

Parameters  dummy -  The atom corresponding to the object.

	    lpInfo - pointer to a data structure which needs to be
		     filled. If NULL, don't copy the object.

Return	    LOWORD - wSize of the object.
	    HIWORD - Error code.

Notes	    This function is called twice to render the object. First,
	    to get the object size. Second, to get a copy of the object.

------------------------------------------------------------------------*/
LOCAL long GetWindowInfo( dummy, lpInfo )
WORD dummy;
LPSTR lpInfo;
    {
    short	size;
    short	i;
    LPSTR	lpIcon;

    /* Calcuate size: hIcon allready has WINDOWINFO size allocated
       for it.
     */
    size = hIcon == NULL ? sizeof(WINDOWINFO) : (short) GlobalSize( hIcon );

    /* If a pointer is passed, copy the window information.
     */
    if( lpInfo != NULL )
	{
	/* Setup fAction */
	((LPWINDOWINFO)lpInfo)->fAction = IsWindowVisible( hTTYWnd ) ? WI_SHOW : WI_HIDE;
	((LPWINDOWINFO)lpInfo)->fAction |= hIcon == NULL ? WI_DEFAULTICON : WI_PASSEDICON;

	/* Copy over the downloaded Icon */
	if( hIcon != NULL )
	    {
	    lpInfo = (LPSTR)((LPWINDOWINFO)lpInfo + 1);
	    lpIcon = GlobalLock( hIcon );
	    for( i = size - sizeof(WINDOWINFO); i > 0 ;i-- )
		*lpInfo++ = *lpIcon++;
	    GlobalUnlock( hIcon );
	    }
	}


    return MAKELONG( size, NO_ERROR);
    }

/*---------------------------------------------------------------------

GetSysItems( dummy, lpList )

Purpose     Renders the SysItems list.

Parameters  dummy -  The atom corresponding to the object.

	    lpList - pointer to a data structure which needs to be
		     filled. If NULL, don't copy the object.

Return	    LOWORD - wSize of the object.
	    HIWORD - Error code.

Notes	    This function is called twice to render the object. First,
	    to get the object size. Second, to get a copy of the object.

-----------------------------------------------------------------------*/
LOCAL long GetSysItems( dummy, lpList )
WORD dummy;
LPSTR lpList;
    {
    /* If a pointer is passed, copy the syslist. */
    if( lpList != NULL )
	lstrcpy( lpList, (LPSTR)szSysItems );

    return MAKELONG( sizeof(szSysItems), NO_ERROR);
    }

/*---------------------------------------------------------------------

GetTopics( dummy, lpList )

Purpose     Renders the Topics list.

Parameters  dummy -  The atom corresponding to the object.

	    lpList - pointer to a data structure which needs to be
		     filled. If NULL, don't copy the object.

Return	    LOWORD - wSize of the object.
	    HIWORD - Error code.

Notes	    This function is called twice to render the object. First,
	    to get the object size. Second, to get a copy of the object.

-----------------------------------------------------------------------*/
LOCAL long GetTopics( dummy, lpList )
WORD dummy;
LPSTR lpList;
    {
    /* If a pointer is passed, copy the syslist. */
    if( lpList != NULL )
	{
	lstrcpy( lpList, (LPSTR)szAppName );
	lstrcat( lpList, (LPSTR)"\t" );
	lstrcat( lpList, (LPSTR)szSys );
	}

    return MAKELONG( lstrlen( (LPSTR)szAppName) + 8, NO_ERROR);
    }

/*---------------------------------------------------------------------

GetFormats( dummy, lpList )

Purpose     Renders the Formats list.

Parameters  dummy -  The atom corresponding to the object.

	    lpList - pointer to a data structure which needs to be
		     filled. If NULL, don't copy the object.

Return	    LOWORD - wSize of the object.
	    HIWORD - Error code.

Notes	    This function is called twice to render the object. First,
	    to get the object size. Second, to get a copy of the object.

-----------------------------------------------------------------------*/
LOCAL long GetFormats( dummy, lpList )
WORD dummy;
LPSTR lpList;
    {

   /* If a pointer is passed, copy the syslist. */
    if( lpList != NULL )
	{
	lstrcpy( lpList, (LPSTR)szFormats );
	}

    return MAKELONG( sizeof(szFormats), NO_ERROR);
    }


/*-----------------------------------------------------------------------

UpdateOldApState( lpState )

Purpose     Update the OldAp state structure. fUserLock is the only field
	    which the shell can update.

-------------------------------------------------------------------------*/
WORD UpdateOldApState( lpState )
LPSTR	lpState;
    {
    fUserLock = fPassedUserLock = (BYTE) ((LPSTATE)lpState)->bUserLock;

    return SUCCESS;
    }

/*-----------------------------------------------------------------------

UpdateWindowInfo( lpState )

Purpose     Update the WindowInfo structure.

-------------------------------------------------------------------------*/
WORD UpdateWindowInfo( lpInfo )
LPSTR	lpInfo;
    {
    register WORD fAction;
    short	  iconSize;
    LPSTR	  lpIcon;
    WORD	  wResult;

    wResult = SUCCESS;
    fAction = ((LPWINDOWINFO)lpInfo)->fAction;

    /* Set the icon information
     */
    if( fAction & (WI_PASSEDICON | WI_DEFAULTICON)  )
	{
	/* Delete any downloaded icons */
	if( hIcon )
	    {
	    GlobalFree( hIcon );
	    hIcon = NULL;
	    }

	/* Copy the passed icon */
	if( fAction & WI_PASSEDICON )
	    {
	    /* Create an ICON */
	    iconSize = (short) GlobalSize( LOWORD(lPassParam) );
	    hIcon = GlobalAlloc( GMEM_MOVEABLE, (long)iconSize );
	    if( hIcon != NULL )
		{
		lpIcon =  GlobalLock( hIcon );
		lpInfo = (LPSTR)((LPWINDOWINFO)lpInfo + 1);
		for( ; iconSize > 0; iconSize--)
		    *lpIcon++ = *lpInfo++;
		GlobalUnlock( hIcon );
		}
	    else
		wResult = ERR_NO_MEMORY;
	    }

	/* Redraw the icon */
	InvalidateRect( hTTYWnd, (LPRECT)NULL, TRUE );
	UpdateWindow( hTTYWnd );
	}

    /* Set the window visiblity
     */
    if( fAction & WI_HIDE )
	ShowWindow( hTTYWnd, SW_HIDE );

    if( fAction & WI_SHOW )
	ShowWindow( hTTYWnd, SW_SHOW );

    return wResult;
    }



/*========================================================================
 *
 * Tuple (linked list) Functions
 *
 */


/*-----------------------------------------------------------------------

AddTuple( tupleList, index ) : pTuple

Purpose     Create and add a 2-tuple to a tuple list.

Parameters  tupleList - pointer to a tuple list.
	    index - field which is search for by FindTuple.

Notes	    Tuples are used for links and shell lists. Tuples contains
	    two fields. The index is used for search. The term tuples
	    comes from relational databases and was the best name
	    I could think of.


-------------------------------------------------------------------------*/
LOCAL TUPLE *AddTuple( tupleList, index )
PTUPLE	    *tupleList;
WORD	    index;
    {
    register TUPLE  *pTuple;

    pTuple = (TUPLE *) LocalAlloc( LPTR, sizeof( TUPLE ) );
    if( pTuple )
	{
	pTuple->next = *tupleList;
	if( *tupleList )
	    (*tupleList)->prev = pTuple;
	*tupleList = pTuple;
	pTuple->index = index;
	}
    return pTuple;
    }

/*-----------------------------------------------------------------------

DeleteTuple( tupleList, pTuple )

Purpose     Delete a 2-tuple from a tuple list.

Parameters  tupleList - pointer to a tuple list.
	    pTuple - the tuple to be deleted.

See Also    AddTuple and FindTuple.

-------------------------------------------------------------------------*/
LOCAL void  DeleteTuple( tupleList, pTuple )
PTUPLE	*tupleList;
register TUPLE *pTuple;
    {
    if( pTuple->next )
	pTuple->next->prev = pTuple->prev;
    if( pTuple == *tupleList )
	*tupleList = pTuple->next;
    else
	pTuple->prev->next = pTuple->next;
    LocalFree( (HANDLE)pTuple );
    }

/*-----------------------------------------------------------------------

FindTuple( tupleList, index ) : pTuple

Purpose     Delete a 2-tuple from a tuple list.

Parameters  tupleList - a tuple list.
	    index - field which is search for.

Result	    NULL if the tuple is not found.

See Also    AddTuple and FindTuple.

-------------------------------------------------------------------------*/
LOCAL TUPLE *FindTuple( tupleList, index )
PTUPLE	    tupleList;
WORD	    index;
    {
    register TUPLE *pTuple;

    /* Find shell */
    for( pTuple = tupleList; pTuple; pTuple = pTuple->next )
	if( pTuple->index == index )
	    return pTuple;

    /* if no shell found return */
	return NULL;
    }


