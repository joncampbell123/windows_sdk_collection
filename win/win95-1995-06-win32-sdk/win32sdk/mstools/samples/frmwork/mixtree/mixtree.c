// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE:   mixtree.c
//
//  PURPOSE:  Implement the windows procedure for the main application
//    windows.  Also implement the generic message and command dispatchers.
//
//  FUNCTIONS:
//    WndProc         - Processes messages for the main window.
//    MsgCommand      - Handle the WM_COMMAND messages for the main window.
//    MsgDestroy      - Handles the WM_DESTROY message by calling 
//                      PostQuitMessage().
//    MsgSize         - Handles WM_SIZE messages for main window; resizes
//                      TreeView control.
//    MsgPaint        - Handles WM_PAINT messages for main window.
//    MsgNotify       - Handles WM_NOTIFY messages for main window sent by
//                      TreeView control.
//    MsgChangeBranch - Handles private message sent when user has selected
//                      a different branch of the TreeView control
//    CmdFill         - Fills the TreeView control.
//    CmdDisplayAll   - Allows all destination lines to be displayed at once.
//    CmdCollapseAny  - Allows the children of any item to be collapsed.
//    CmdExit         - Handles the file exit command by calling destory 
//                      window on the main window.
//    CreateImageList - Create an Image List.
//    CreateTreeView  - Create a TreeViewControl.
//
//  COMMENTS:
//

#include <windows.h>                // required for all Windows applications.
#include <windowsx.h>
#include "globals.h"                // prototypes specific to this application.

// FUNCTION PROTOTYPES

HWND CreateTreeView(HWND hwndParent, int iID, DWORD dwStyle);

HIMAGELIST CreateImageList(HWND hwnd);

// Main window message table definition.
MSD rgmsd[] =
{
	{WM_COMMAND,      MsgCommand},
	{WM_DESTROY,      MsgDestroy},
	{WM_CREATE,       MsgCreate},
	{WM_SIZE,         MsgSize},
	{WM_PAINT,        MsgPaint},
	{WM_NOTIFY,       MsgNotify},
	{PM_CHANGEBRANCH, MsgChangeBranch}
};

MSDI msdiMain =
{
	sizeof(rgmsd) / sizeof(MSD),
	rgmsd,
	edwpWindow
};

// Main window command table definition.
CMD rgcmd[] =
{
	{IDM_EXIT,        CmdExit},
	{IDM_ABOUT,       CmdAbout},
	{IDM_FILL,        CmdFill},
	{IDM_DISPLAYALL,  CmdDisplayAll},
	{IDM_COLLAPSEANY, CmdCollapseAny}
};

CMDI cmdiMain =
{
	sizeof(rgcmd) / sizeof(CMD),
	rgcmd,
	edwpWindow
};


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  PARAMETERS:
//    hwnd     - window handle
//    uMessage - message number
//    wparam   - additional information (dependant on message number)
//    lparam   - additional information (dependant on message number)
//
//  RETURN VALUE:
//    The return value depends on the message number.  If the message
//    is implemented in the message dispatch table, the return value is
//    the value returned by the message handling function.  Otherwise,
//    the return value is the value returned by the default window procedure.
//
//  COMMENTS:
//    Call the DispMessage() function with the main window's message dispatch
//    information (msdiMain) and the message specific information.
//

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
	return DispMessage(&msdiMain, hwnd, uMessage, wparam, lparam);
}


//
//  FUNCTION: MsgCommand(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handle the WM_COMMAND messages for the main window.
//
//  PARAMETERS:
//    hwnd     - window handle
//    uMessage - WM_COMMAND (Unused)
//    GET_WM_COMMAND_ID(wparam, lparam)   - Command identifier
//    GET_WM_COMMAND_HWND(wparam, lparam) - Control handle
//
//  RETURN VALUE:
//    The return value depends on the message number.  If the message
//    is implemented in the message dispatch table, the return value is
//    the value returned by the message handling function.  Otherwise,
//    the return value is the value returned by the default window procedure.
//
//  COMMENTS:
//    Call the DispCommand() function with the main window's command dispatch
//    information (cmdiMain) and the command specific information.
//

LRESULT MsgCommand(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
	return DispCommand(&cmdiMain, hwnd, wparam, lparam);
}

//
//  FUNCTION: MsgCreate(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Creates TreeView Control, and posts "Fill" message.
//
//  PARAMETERS:
//
//    hwnd      - Window handle  (Unused)
//    uMessage  - Message number (Unused)
//    wparam    - not used
//    lparam    - not used
//
//  RETURN VALUE:
//
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//

LRESULT MsgCreate(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
	// Create the TreeView control with lines only
	hwndTreeView = CreateTreeView(hwnd, 
								  0, 
								  TVS_HASLINES | 
								  // TVS_HASBUTTONS | TVS_LINESATROOT  |
								  WS_VISIBLE | WS_CHILD | WS_BORDER);
  
	if (hwndTreeView)
	{
		// set global variable for application menu handle
		hMenu = GetMenu(hwnd);

		// initialize some menu items in the application menu
		CheckMenuItem(hMenu, IDM_DISPLAYALL,  fDisplayAll  ? 
			MF_CHECKED : MF_UNCHECKED);
		CheckMenuItem(hMenu, IDM_COLLAPSEANY, fCollapseAny ? 
			MF_CHECKED : MF_UNCHECKED);

		// get handle of font used in TreeView control so we can use it, too      
		hfTVFont = (HFONT)SendMessage(hwndTreeView, WM_GETFONT, 0, 0);
	
		// fill the TreeView control
		PostMessage(hwnd, WM_COMMAND, IDM_FILL, 0);
	}
	else
		// couldn't create TreeView control so exit
		PostMessage(hwnd, WM_COMMAND, IDM_EXIT, 0);

	return 0;
}

//
//  FUNCTION: MsgDestroy(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Calls PostQuitMessage(), cleans up image list.
//
//  PARAMETERS:
//
//    hwnd      - Window handle  (Unused)
//    uMessage  - Message number (Unused)
//    wparam    - Extra data     (Unused)
//    lparam    - Extra data     (Unused)
//
//  RETURN VALUE:
//
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//

LRESULT MsgDestroy(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
	// Clean up the image list.

	if (hImageList)
		ImageList_Destroy(hImageList);

	PostQuitMessage(0);
	return 0;
}

//
//  FUNCTION: MsgSize(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Resizes TreeView Control
//
//  PARAMETERS:
//
//    hwnd      - Window handle  (Unused)
//    uMessage  - Message number (Unused)
//    wparam    - Not used.
//    lparam    - Not used.
//
//  RETURN VALUE:
//
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//

LRESULT MsgSize(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
	RECT rc;    // Client area rect

	// Figure the new client area

	GetClientRect(hwnd, &rc);

	// Resize the TreeView control

	if (hwndTreeView)
		SetWindowPos(hwndTreeView,
					 NULL,
					 0, 0,
					 rc.right / 3, rc.bottom,
					 SWP_NOZORDER);

	return 0;
}

//
//  FUNCTION: MsgPaint(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Paints text info area to right of TreeView control
//
//  PARAMETERS:
//
//    hwnd      - Window handle  (Unused)
//    uMessage  - Message number (Unused)
//    wparam    - Not used.
//    lparam    - Not used.
//
//  RETURN VALUE:
//
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//

LRESULT MsgPaint(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;

	BeginPaint(hwnd, &ps);  
	DisplayItemInfo(hwnd, ps.hdc);
	EndPaint(hwnd, &ps);

	return 0;
}
  

//
//  FUNCTION: MsgNotify(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handles notifications from TreeView Control
//
//  PARAMETERS:
//
//    hwnd      - Window handle  (Unused)
//    uMessage  - Message number (Unused)
//    wparam    - control id
//    lparam    - points to an NMHDR (Notify Message Header) which 
//                is overloaded with a NM_TREEVIEW structure:
//     
//
//  RETURN VALUE:
//
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//
LRESULT MsgNotify(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{           
	switch (((LPNMHDR)lparam)->code)   
	{
		case TVN_ITEMEXPANDING: 
		{
			HTREEITEM htiNew = ((LPNM_TREEVIEW)lparam)->itemNew.hItem;

			// take action if the new selection is not the current root and
			// View.Display All has not been selected
			if (htiNew && htiNew != htiCurrentRoot && !fDisplayAll)
			{                          
				// see if the new item is the root of its branch
				if (!TreeView_GetParent(hwndTreeView, htiNew))                    
					// it's the root; collapse previous branch, expand this one
					PostMessage(hwnd, PM_CHANGEBRANCH, (UINT)htiNew, 0L);                                                                                   
			}

	    // only allow an item to expand while building the tree or switching destinations
			// unless View.Collapse Any has been selected
			if (!fExpandEnabled && !fCollapseAny)
				// don't allow item to expand  
				return 1;    

			break;
		}

		case TVN_SELCHANGED:
		{
			TV_ITEM tviNew = ((LPNM_TREEVIEW)lparam)->itemNew;       
			HDC hdc;        
					  
			// remember the current selection if there is one
			lCurrentItem = tviNew.hItem ? tviNew.lParam : MIXITEM_NOSELECTION;

			// display the current item's info
			GetItemInfo(hwnd, lCurrentItem);            
			hdc = GetDC(hwnd);
			DisplayItemInfo(hwnd, hdc);
			ReleaseDC(hwnd, hdc);           
		
			break;
		}

		default:
			break;
	}

	return 0;
}

//
//  FUNCTION: MsgChangeBranch(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handles message sent when user selects a different branch of
//           the TreeView control, collapsing the previous branch and
//           expanding the new one.
//
//  PARAMETERS:
//
//    hwnd      - Window handle  (Unused)
//    uMessage  - Message number (Unused)
//    wparam    - HTREEITEM indicating root item of new branch
//    lparam    - (Unused)
//
//     
//
//  RETURN VALUE:
//
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//
LRESULT MsgChangeBranch(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{      
	// enable expansion of TreeView control
	fExpandEnabled = TRUE;
	   
	// collapse the branch of the current root
	TreeView_Expand(hwndTreeView, htiCurrentRoot, TVE_COLLAPSE);

	// make the new selection the current root
	htiCurrentRoot = (HTREEITEM)wparam;

	// expand the branch of the new current root
	ExpandBranch(htiCurrentRoot);

	// defeat expansion of TreeView control
	fExpandEnabled = FALSE;

	return 0;
}


//
//  FUNCTION: CmdFill(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Fill TreeView Control will the lines and controls.
//
//  PARAMETERS:
//    hwnd     - window handle
//    wCommand - IDM_FILL
//    wNotify  - Not Used.
//    hwndCtrl - Not Used.
//
//  RETURN VALUE:
//    Always 0.
//
//  COMMENTS:
//    This calls the BuildMixerTree() function in MIXLINE.C to fill the
//    TreeView control. The message to invoke this function is posted
//    at startup in MsgCreate().

LRESULT CmdFill(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
	BuildMixerTree(hwnd);   
					 
	return 0;
}


//
//  FUNCTION: CmdDisplayAll(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Enables the ability to display all destination lines 
//           simultaneously.
//
//  PARAMETERS:
//    hwnd     - window handle
//    wCommand - IDM_DISPLAYALL
//    wNotify  - Not Used.
//    hwndCtrl - Not Used.
//
//  RETURN VALUE:
//    Always 0.
//
//  COMMENTS:
//
//

LRESULT CmdDisplayAll(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
	// toggle the flag
	fDisplayAll ^= TRUE;

	// reset the current root to the root of the TreeView control
	htiCurrentRoot = TreeView_GetRoot(hwndTreeView);

	if (fDisplayAll)   
		// display all destination lines expanded
		ExpandTreeView(); 
	else 
	{
		HTREEITEM htiRoot = htiCurrentRoot;
	
		fExpandEnabled = TRUE;
	   
		// display first destination line expanded
		ExpandBranch(htiCurrentRoot);
	   
		// display all other destination lines collapsed
		while (htiRoot = TreeView_GetNextSibling(hwndTreeView, htiRoot))            
			TreeView_Expand(hwndTreeView, htiRoot, TVE_COLLAPSE);     
		  
		fExpandEnabled = FALSE;
	}

	CheckMenuItem(hMenu, IDM_DISPLAYALL, fDisplayAll ? MF_CHECKED : MF_UNCHECKED);
	return 0;
}


//
//  FUNCTION: CmdCollapseAny(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Enables the ability to collapse the children of any item.
//
//  PARAMETERS:
//    hwnd     - window handle
//    wCommand - IDM_COLLAPSEANY
//    wNotify  - Not Used.
//    hwndCtrl - Not Used.
//
//  RETURN VALUE:
//    Always 0.
//
//  COMMENTS:
//    
//

LRESULT CmdCollapseAny(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
	// toggle the flag
	fCollapseAny ^= TRUE;

	CheckMenuItem(hMenu, IDM_COLLAPSEANY, fCollapseAny ? MF_CHECKED : MF_UNCHECKED);
	return 0;
}


//
//  FUNCTION: CmdExit(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Exit the application.
//
//  PARAMETERS:
//    hwnd     - The window.
//    wCommand - IDM_EXIT (unused)
//    wNotify  - Notification number (unused)
//    hwndCtrl - NULL (unused)
//
//  RETURN VALUE:
//    Always returns 0 - command handled.
//
//  COMMENTS:
//
//

LRESULT CmdExit(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
	DestroyWindow(hwnd);
	return 0;
}

//
//  FUNCTION: CreateImageList(HWND hwnd)
//
//  PURPOSE: Create an ImageList for use with the TreeView.
//
//  PARAMETERS:
//    hwnd - handle of parent window.
//
//  RETURN VALUE:
//    On Success: Handle to the Image List
//    On Failure: NULL
//
//  COMMENTS:
//    The bitmaps for the image list are stored in the resources
//    of this application. For this example, each bitmap is a
//    separate resource. The background (mask) color of the bitmaps
//    is white - RGB (255,255,255), which is what the image list
//    will automagically use as a mask as explained below.
//
//    Even though the bitmaps in the resources are 32x32 pixels,
//    we are only using the upper left 20x12 portion, as this is the
//    minimum bounding rectangle to fit the little teensy pictures.
//
//    To change the size of the images, edit both the bitmaps,
//    and change the parameters to ImageList_Create.
//

HIMAGELIST CreateImageList(HWND hwnd)
{
	HIMAGELIST hIL;       // Handle to ImageList
	HBITMAP    hBitmap;   // Handle to a bitmap
	int i;

	// Create the Image List
	hIL = ImageList_Create(20,     // X size of one image
						   12,     // Y size of one image
						   TRUE,   // Masked images
						   IL_MAXIMAGES, // number of images in list
						   0);     // No "growing" bitmaps in this list.

	if (!hIL) 
		return NULL;
   
	// put bitmap resource identifiers into info array
	aImageInfo[IL_DESTLINE].uBitmapID = IDB_DESTLINE;
	aImageInfo[IL_SRCLINE].uBitmapID  = IDB_SRCLINE;
	aImageInfo[IL_CUSTOM].uBitmapID   = IDB_CUSTOM;
	aImageInfo[IL_METER].uBitmapID    = IDB_METER;
	aImageInfo[IL_SWITCH].uBitmapID   = IDB_SWITCH;
	aImageInfo[IL_NUMBER].uBitmapID   = IDB_NUMBER;
	aImageInfo[IL_SLIDER].uBitmapID   = IDB_SLIDER;
	aImageInfo[IL_FADER].uBitmapID    = IDB_FADER;
	aImageInfo[IL_TIME].uBitmapID     = IDB_TIME;
	aImageInfo[IL_LIST].uBitmapID     = IDB_LIST;  

	// Add Each bitmap to the ImageList
	for (i = 0; i < IL_MAXIMAGES; i++)
	{    
		// ImageList_AddMasked will add the bitmap, and treat every
		// pixel that is (in this example) white as a "transparent" pixel,
		// since we specified TRUE for fMask in the above call to
		// ImageList_Create.

		hBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(aImageInfo[i].uBitmapID));
		if (hBitmap)
		{
			aImageInfo[i].uImageIndex = 
				ImageList_AddMasked(hIL, hBitmap, RGB(255,255,255));
			DeleteObject (hBitmap);
		}
	}
  
	// Paranoid checking to make sure everything was added successfully.
	if (ImageList_GetImageCount(hIL) < IL_MAXIMAGES)
	{
		ImageList_Destroy(hIL);
		hIL = NULL;

		for (i = 0; i < IL_MAXIMAGES; i++)
			aImageInfo[i].uImageIndex = 0;
	}

	// Success! 
	return hIL;
}

//
//  FUNCTION: CreateTreeView(HWND hwndParent, int iID, DWORD dwStyle)
//
//  PURPOSE: Create a TreeView control
//
//  PARAMETERS:
// 
//    hwndParent - Parent of the TreeView.
//    iID        - ID of the TreeView.
//    dwStyle    - Style bits of the TreeView.
//
//  RETURN VALUE:
//    On Success: Handle to the TreeView
//    On Failure: NULL
//
//  COMMENTS:
//
//    The call to InitCommonControls is required once per application
//    lifetime. It is OK to move this to init.c, but for the
//    cut-and-pastibility of this sample, the call is placed here.
//

HWND CreateTreeView(HWND hwndParent, int iID, DWORD dwStyle)
{
	RECT rc;      // Client rect of parent
	HWND hwnd;    // Handle to TreeView

	// This registers the TreeView class.
	InitCommonControls();

	// Get the client area of the parent.
	GetClientRect(hwndParent, &rc);

	// Create the TreeView control.
	hwnd = CreateWindowEx(0,                        // Ex style
						  WC_TREEVIEW,              // Class
						  "",                       // Dummy Text
						  dwStyle,                  // Style
						  0, 0,                     // Use all of
						  rc.right, rc.bottom,      // client area.
						  hwndParent,               // Parent
						  (HMENU)iID,               // ID
						  hInst,                    // Instance
						  NULL);                    // No extra

	// Create the ImageList for this TreeView
	hImageList = CreateImageList(hwnd);

	// Attach ImageList to TreeView
	if (hImageList && hwnd)
		TreeView_SetImageList(hwnd, hImageList, 0);

	// Return the window handle
	return hwnd;
}
