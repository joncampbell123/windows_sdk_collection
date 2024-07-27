/*==========================================================================

HotHelp	-- Code which manages the window list

 		$Author:   tqn  $
 		$Revision:   1.7  $
 		$Date:   22 Sep 1987 17:19:06  $

===========================================================================*/
#include <windows.h>

typedef struct  
	{
	HWND	hWnd;
	short	titleSize;
	char	szTitle[1];
	}
	WINTITLE;
typedef WINTITLE *PWINTITLE;

PWINTITLE	*WindowList;
short		DestWindow;
short		NumTitles;
short		OurWindow;
char		*pWindowTitle = NULL;
char		TitleLen = 0;

extern HANDLE	hWoaInst;
extern HWND	hTTYWnd;
extern short	GetFSwitch();

/*------------------------------------------------------------------

NextDestWindow() 
PrevDestWindow() 
	
Purpose		Sets DestWindow to the next or previous window
		in WindowList.

Result		DestWindow will be updated.

---------------------------------------------------------------------*/
PrevDestWindow()
	{
	DestWindow = DestWindow ? DestWindow - 1 : NumTitles - 1;
	}

NextDestWindow()
	{
	DestWindow = ++DestWindow % NumTitles;
	}

	
	
/*-------------------------------------------------------------------

SetWindowList()

Purpose		Sets the WindowList with a list of top windows
                and there titles
                
Notes		WinList is a singly linked list. EnumWndFn will
		be replaced by GetWindow calls.

---------------------------------------------------------------------*/
void SetWindowList()
	{
	register short	i;
	HWND		hWnd, 
			hLastWnd, 
			hFirstWnd;
	short		titleSize; 
	LPSTR		lpTitleTemp;	/* ~~tqn */
	PWINTITLE	pList;
	short		listSize;		
	
	/* delete the current list */
	for( i = NumTitles - 1; i >= 0; i--)
			LocalFree( (HANDLE) WindowList[i] );
	LocalFree( (HANDLE)WindowList );

	/* Allocate a small WindowList */
	listSize = sizeof( PWINTITLE * );
	WindowList = (PWINTITLE *) LocalAlloc( LPTR, listSize );

	/* fill up the window list */
	i = 0;
	for( hWnd = GetWindow( hTTYWnd, GW_HWNDLAST ); hWnd; hWnd = GetNextWindow( hWnd, GW_HWNDPREV ))

		/* Make sure the Window is normal */
		if( IsWindowVisible( hWnd ) && IsWindowEnabled( hWnd ) )
			{
			titleSize = GetWindowTextLength( hWnd );

			/* Test for ourselves */
			if( hWnd == hTTYWnd )
				{
				OurWindow = DestWindow = i;
				TitleLen = titleSize;
				titleSize += 32; 	/* Add space for add ons */
				}

			/* Create a WINTITLE structure for the window */
			pList = (PWINTITLE) LocalAlloc( LPTR, sizeof(WINTITLE) + titleSize+1);
			if( pList == NULL )
				break;
				
			lpTitleTemp = (LPSTR) MAKELONG(0, GlobalAlloc(GMEM_LOWER+GMEM_ZEROINIT,
								      (DWORD)(titleSize+1)));
			if( lpTitleTemp == NULL)				
				{ LocalFree( (HANDLE) pList);
				  break;
				}

			/* Fill fields */
			pList->hWnd = hWnd;
			GetWindowText( hWnd, lpTitleTemp, 1000);
			AnsiToOem( lpTitleTemp, (LPSTR)pList->szTitle );
			pList->titleSize = titleSize;
			GlobalFree( (HANDLE) HIWORD(lpTitleTemp));
	
			/* Grow the WindowList */
			listSize += sizeof( PWINTITLE *);
			WindowList = (PWINTITLE *) LocalReAlloc( (HANDLE)WindowList, listSize, LMEM_MOVEABLE);
			if( WindowList == NULL )
				break;
			
			/* Put the new entry in the Window list */	
			WindowList[i] = pList;
	
			/* Increment the number of titles */	
			++i;
			}
			
	/* finish up */
	pWindowTitle = &(WindowList[OurWindow]->szTitle[0]);
	NumTitles = i;
	}

	
/*-----------------------------------------------------------------------

ActivateNextWindow 

Purpose		Activates the destination window.

Note		The flags in fSwitch are examined to determine the type of
		switch is needed.
  
-------------------------------------------------------------------------*/
ActivateNextWindow()
	{
	/* type of switch? */
	if( GetFSwitch() & 2)
		{
		/* TAB: Open iconic windows */
		BringWindowToTop( WindowList[DestWindow]->hWnd );
		if( IsIconic( WindowList[DestWindow]->hWnd ) )
			PostMessage( WindowList[DestWindow]->hWnd, WM_SYSCOMMAND, SC_RESTORE, 0L);
		}
	else
		{
		/* ESC: Use SYSCOMMAND to switch */
		PostMessage( hTTYWnd, WM_SYSCOMMAND, GetFSwitch() & 4 ? SC_PREVWINDOW : SC_NEXTWINDOW, 0x1bL);
		PostMessage( hTTYWnd, WM_SYSKEYUP, VK_ESCAPE, 0x80380001 );
		}
	}
