// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE:   mixline.c
//
//  PURPOSE:  Enumerate mixer lines and controls and add them to the 
//            TreeView control.  Modify the state of the TreeView control.
//
//  FUNCTIONS:
//    BuildMixerTree   - Gets all mixer lines and adds them to the TreeView
//                       control.
//    GetMixerControls - Gets all controls for a mixer line and adds them to
//                       the TreeView control.
//    ExpandTreeView   - Cause all the nodes and leaves of the TreeView to
//                       be displayed.
//    ExpandBranch     - Expand a single branch of a parent item which has
//                       no parent of its own.
//    ExpandChildren   - Expand all the children of a parent item.
//    ItemHasChild     - Determine if an item has one or more children.
//
//  COMMENTS:
//

#include <windows.h>                // required for all Windows applications.
#include <windowsx.h>
#include <memory.h>

#include "globals.h"                // prototypes specific to this application.
#include "resource.h"
#include "mixline.h"


//
//  FUNCTION: BuildMixerTree(HWND)
//
//  PURPOSE: Gets all mixer lines and adds them to the TreeView control.
//                       
//
//  PARAMETERS:
//    hwnd     - application window handle
//
//  RETURN VALUE:
//    TRUE     - Success.
//    FALSE    - Failure.
//
//  COMMENTS:
//    
//

BOOL BuildMixerTree(HWND hwnd)
{    
	MMRESULT result;
	MIXERLINE mlDest, mlSrc;   
	HTREEITEM htiDest = 0, htiSrc = 0;
	DWORD dwDestination, dwSource;   
	
	SetCapture(GetParent(hwndTreeView));
	SetCursor(LoadCursor(NULL, IDC_WAIT));

	// reset global variables for currently selected item and number of items
	lCurrentItem = MIXITEM_NOSELECTION;
	uMixItems = 0;

	// Clear TreeView Control
	TreeView_DeleteAllItems(hwndTreeView);

	// process each destination
	for (dwDestination = 0; dwDestination < MixerCaps.cDestinations; 
		dwDestination++)
	{       
		// don't write past the end of amixii[]  
		if (uMixItems > MAXMIXITEMS)
		{                          
			ReleaseCapture();
			SetCursor(LoadCursor(NULL, IDC_ARROW));           
			MessageBox(hwnd, "TreeView incomplete; MAXMIXITEMS reached", 
				"MixTree Error", MB_ICONHAND);
			return (FALSE);
		}      
	
	// initialize MIXERLINE structure
	_fmemset((LPBYTE)&mlDest, 0, sizeof(MIXERLINE));               
	    mlDest.cbStruct = sizeof(MIXERLINE);
	    mlDest.dwDestination = dwDestination;   
				   
		// get the destination's line information                   
		result = GetLineInfo(hwnd, &mlDest, MIXER_GETLINEINFOF_DESTINATION);  
		if (result)
		{                          
			ReleaseCapture();
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			return (FALSE);
		}      
		
		// store this item's information  
		amixii[uMixItems].dwItemType      = MIXITEM_MIXERLINE;
		amixii[uMixItems].dwLineID        = mlDest.dwLineID;
   
		// Set up the structure for a text label, and use
		// the ImageList ID iINIFileImage.
		_fmemset((LPBYTE)&tvi, 0, sizeof(TV_ITEM));
		tvi.mask            = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM;
		tvi.iImage          =
		tvi.iSelectedImage  = aImageInfo[IL_DESTLINE].uImageIndex;
		tvi.pszText         = mlDest.szName;  
		tvi.cchTextMax      = MIXER_LONG_NAME_CHARS;      
		tvi.lParam          = uMixItems;     

		// Populate the TreeView Insert Struct
		// The item is the one filled above.
		// Insert it after the last item inserted at this level.
		// And indicate this is a root entry.

		tvins.item         = tvi;
		tvins.hParent      = TVI_ROOT;  
		
		// insert after previous destination if there was one    
		tvins.hInsertAfter = mlDest.dwDestination ? htiDest : TVI_ROOT;       
	
		// Add the item to the tree
		htiDest = TreeView_InsertItem(hwndTreeView, &tvins);
		if (!htiDest)
		{                         
			ReleaseCapture();
			SetCursor(LoadCursor(NULL, IDC_ARROW));           
			MessageBox(hwnd, "TreeView incomplete; unable to insert item", 
				"MixTree Error", MB_ICONHAND);
			return (FALSE);
		}      

		// increment the count
		uMixItems++;
				
		// get all of the destination's controls
		if (!GetMixerControls(hwnd, &mlDest, htiDest))
		{
			// GetMixerControls has already displayed the error message
			ReleaseCapture();
			SetCursor(LoadCursor(NULL, IDC_ARROW));                        
			return (FALSE);
		}
							   
		if (htiControl)
			// cause source lines to be children of the last control
			htiDest = htiControl;      

		// process each of the destination line's sources                                                     
		for (dwSource = 0; dwSource < mlDest.cConnections; dwSource++)
		{   
			// don't write past the end of amixii[]                                        
			if (uMixItems > MAXMIXITEMS)
			{                          
				ReleaseCapture();
				SetCursor(LoadCursor(NULL, IDC_ARROW));           
				MessageBox(hwnd, "TreeView incomplete; MAXMIXITEMS reached", 
					"MixTree Error", MB_ICONHAND);
				return (FALSE);
			}      

	    // initialize the MIXERLINE structure
		_fmemset((LPBYTE)&mlSrc, 0, sizeof(MIXERLINE));               
		mlSrc.cbStruct = sizeof(MIXERLINE);
	    mlSrc.dwSource = dwSource;
	    mlSrc.dwDestination = dwDestination;

			// get the source's line information
			result = GetLineInfo(hwnd, &mlSrc, MIXER_GETLINEINFOF_SOURCE);  
			if (result)
			{                              
				ReleaseCapture();
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				return (FALSE);
			}  
			
			// store this item's information and increment the count
			amixii[uMixItems].dwItemType      = MIXITEM_MIXERLINE;
			amixii[uMixItems].dwLineID        = mlSrc.dwLineID;
	   
			// Set up the structure for a text label, and use
			// the ImageList ID iINIFileImage.
			tvi.mask            = TVIF_TEXT | TVIF_IMAGE | 
								  TVIF_SELECTEDIMAGE | TVIF_PARAM;
			tvi.iImage          =
			tvi.iSelectedImage  = aImageInfo[IL_SRCLINE].uImageIndex;
			tvi.pszText         = mlSrc.szName;  
			tvi.cchTextMax      = MIXER_LONG_NAME_CHARS;      
			tvi.lParam          = uMixItems;     

			// Populate the TreeView Insert Struct
			// The item is the one filled above.
			// Insert it after the last item inserted at this level.
			// And indicate this is a root entry.

			tvins.item         = tvi;
			tvins.hParent      = htiDest;
			tvins.hInsertAfter = mlSrc.dwSource ? htiSrc : htiDest;         
								
			// Add the item to the tree
			htiSrc = TreeView_InsertItem(hwndTreeView, &tvins);
			if (!htiSrc)
			{                          
				ReleaseCapture();
				SetCursor(LoadCursor(NULL, IDC_ARROW));           
				MessageBox(hwnd, "TreeView incomplete; unable to insert item", 
					"MixTree Error", MB_ICONHAND);
				return (FALSE);
			}              
	
			uMixItems++;                  
			
			// get all of the source's controls
			if (!GetMixerControls(hwnd, &mlSrc, htiSrc))
			{
				// GetMixerControls() has already displayed the error message
				ReleaseCapture();
				SetCursor(LoadCursor(NULL, IDC_ARROW));                           
				return (FALSE);
			}
		}
	}

	// initialize TreeView display

	// get handle to root of first branch (which is the root of
	// the entire TreeView control) and remember it
	htiCurrentRoot = TreeView_GetRoot(hwndTreeView);

	if (fDisplayAll)
		// display all destination lines expanded
		ExpandTreeView();
	else
		// display first destination line expanded
		ExpandBranch(htiCurrentRoot);
	   
	ReleaseCapture();
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	return (TRUE);
}


//
//  FUNCTION: GetMixerControls(HWND, LPMIXERLINE, HTREEITEM)
//
//  PURPOSE: Obtains all the controls of a mixer line and inserts items
//           for them into the TreeView control
//
//  PARAMETERS:
//    hwnd        - application window handle.
//    lpMixerLine - pointer to a MIXERLINE structure.
//    htiLine     - handle to the TreeView item for the mixer line.
//
//  RETURN VALUE:
//    TRUE     - Success.
//    FALSE    - Failure.
//
//  COMMENTS:
//    
//
 
BOOL GetMixerControls(
	HWND hwnd,
	LPMIXERLINE lpMixerLine,
	HTREEITEM htiLine)
{    
	MMRESULT result;
	MIXERLINECONTROLS mlcs; 
	HGLOBAL hMem;
	LPSTR lpMem;   
	UINT i;
	int iImage;   
				  
	if (!lpMixerLine->cControls)
		// nothing to do
		return (TRUE);

	// initialize global variable for last control tree item
	// GetMixerLines() uses this to insert source lines into the TreeView
	// control after the destination line's controls.
	htiControl = NULL;

	// initialize the MIXERLINECONTROLS structure                       
	_fmemset((LPBYTE)&mlcs, 0, sizeof(MIXERLINECONTROLS));
	mlcs.cbStruct = sizeof(MIXERLINECONTROLS);
	mlcs.dwLineID = lpMixerLine->dwLineID;
	mlcs.dwControlType = 0;
	mlcs.cControls = lpMixerLine->cControls;           
	mlcs.cbmxctrl = sizeof(MIXERCONTROL);
	
	hMem = GlobalAlloc(GHND, lpMixerLine->cControls * sizeof(MIXERCONTROL));
	if (!hMem)    
	{        
		MessageBox(hwnd, "Insufficient Memory", "MixTree Error", MB_ICONHAND);
		return (FALSE);
	}
	
	lpMem = GlobalLock(hMem);
	if (!lpMem)
	{      
		MessageBox(hwnd, "Memory Error", "MixTree Error", MB_ICONHAND);
		GlobalFree(hMem);
		return (FALSE);
	}    
	
	mlcs.pamxctrl = (LPMIXERCONTROL)lpMem;    
	  
	result = GetLineControls(hwnd, &mlcs, MIXER_GETLINECONTROLSF_ALL); 
	if (result)
	{               
		GlobalUnlock(hMem);
		GlobalFree(hMem);
		return (FALSE);
	}
		  
	// process each control
	for (i = 0; i < lpMixerLine->cControls; i++)
	{ 
		// don't write past the end of amixii[]  
		if (uMixItems > MAXMIXITEMS)
		{                                              
			MessageBox(hwnd, "TreeView incomplete; MAXMIXITEMS reached", 
				"MixTree Error", MB_ICONHAND);
			GlobalUnlock(hMem);
			GlobalFree(hMem);
			return (FALSE);
		}      
 
		// store this item's information and increment the count  
		amixii[uMixItems].dwItemType    = MIXITEM_MIXERCONTROL;
		amixii[uMixItems].dwControlID   = (mlcs.pamxctrl + i)->dwControlID;
	  
		// Set up the structure for a text label, and use
		// the ImageList ID iINIFileImage.
		tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

		// select an image according to the class of the control
		switch ((mlcs.pamxctrl + i)->dwControlType)
		{
			case MIXERCONTROL_CONTROLTYPE_CUSTOM:
				iImage = IL_CUSTOM;
				break;

			case MIXERCONTROL_CONTROLTYPE_SIGNEDMETER:
			case MIXERCONTROL_CONTROLTYPE_PEAKMETER:
			case MIXERCONTROL_CONTROLTYPE_UNSIGNEDMETER:
				iImage = IL_METER;
				break;

			case MIXERCONTROL_CONTROLTYPE_BOOLEAN:
			case MIXERCONTROL_CONTROLTYPE_ONOFF:
			case MIXERCONTROL_CONTROLTYPE_MUTE:
			case MIXERCONTROL_CONTROLTYPE_MONO:
			case MIXERCONTROL_CONTROLTYPE_LOUDNESS:
			case MIXERCONTROL_CONTROLTYPE_STEREOENH:
				iImage = IL_SWITCH;
				break;

			case MIXERCONTROL_CONTROLTYPE_SIGNED:
			case MIXERCONTROL_CONTROLTYPE_UNSIGNED:
				iImage = IL_NUMBER;
				break;

			case MIXERCONTROL_CONTROLTYPE_SLIDER:
			case MIXERCONTROL_CONTROLTYPE_PAN:
			case MIXERCONTROL_CONTROLTYPE_QSOUNDPAN:
				iImage = IL_SLIDER;
				break;

			case MIXERCONTROL_CONTROLTYPE_FADER:
			case MIXERCONTROL_CONTROLTYPE_VOLUME:
			case MIXERCONTROL_CONTROLTYPE_BASS:
			case MIXERCONTROL_CONTROLTYPE_TREBLE:
			case MIXERCONTROL_CONTROLTYPE_EQUALIZER:
				iImage = IL_FADER;
				break;

			case MIXERCONTROL_CONTROLTYPE_MICROTIME:
			case MIXERCONTROL_CONTROLTYPE_MILLITIME:
				iImage = IL_TIME;
				break;

			case MIXERCONTROL_CONTROLTYPE_SINGLESELECT:
			case MIXERCONTROL_CONTROLTYPE_MUX:
			case MIXERCONTROL_CONTROLTYPE_MULTIPLESELECT:
			case MIXERCONTROL_CONTROLTYPE_MIXER:
				iImage = IL_LIST;
				break;

			default:
				// this shouldn't be reached, but just in case...
				iImage = IL_CUSTOM;
				break;
		}

		tvi.iImage          = 
		tvi.iSelectedImage  = aImageInfo[iImage].uImageIndex;                                
		tvi.pszText         = (mlcs.pamxctrl + i)->szName;  
		tvi.cchTextMax      = MIXER_LONG_NAME_CHARS;      
		tvi.lParam          = uMixItems;     

		// Populate the TreeView Insert Struct
		// The item is the one filled above.
		// Insert it after the last item inserted at this level.
		// And indicate this is a root entry.

		tvins.item         = tvi;
		tvins.hParent      = htiLine;
		
		// insert after mixer line item if this is first control
		tvins.hInsertAfter = i ? htiControl : htiLine;     
	
		// Add the item to the tree
		htiControl = TreeView_InsertItem(hwndTreeView, &tvins);
		if (!htiControl)
		{               
			MessageBox(hwnd, "TreeView incomplete; unable to insert item", 
				"MixTree Error", MB_ICONHAND);
			GlobalUnlock(hMem);
			GlobalFree(hMem);
			return (FALSE);
		}      
	
		uMixItems++;   
	}
						   
	GlobalUnlock(hMem);
	GlobalFree(hMem); 
	return (TRUE);
}

//
//  FUNCTION: ExpandTreeView()
//
//  PURPOSE: Expand all of TreeView control's items.
//
//  PARAMETERS:
//    none.
//
//  RETURN VALUE:
//    nothing.
//
//  COMMENTS:
//    
//

void ExpandTreeView()
{  
	HTREEITEM htiNext = TreeView_GetRoot(hwndTreeView);   

	fExpandEnabled = TRUE;
   
	if (ItemHasChild(htiNext))
		ExpandChildren(htiNext); 
  
	while (htiNext = TreeView_GetNextSibling(hwndTreeView, htiNext))
		if (ItemHasChild(htiNext))        
			ExpandChildren(htiNext);                

	fExpandEnabled = FALSE;
}

//
//  FUNCTION: ExpandBranch(HTREEITEM)
//
//  PURPOSE: Expand a single branch of a TreeView control.
//
//  PARAMETERS:
//    htiParent - handle to parent item.
//
//  RETURN VALUE:
//    nothing.
//
//  COMMENTS:
//    
//

void ExpandBranch(HTREEITEM htiParent)
{  
	fExpandEnabled = TRUE;
   
	if (ItemHasChild(htiParent))
		ExpandChildren(htiParent); 

	fExpandEnabled = FALSE; 
}


//
//  FUNCTION: ExpandChildren(HTREEITEM)
//
//  PURPOSE: Expand all of a parent TreeView item's children.
//
//  PARAMETERS:
//    htiParent - handle to parent item.
//
//  RETURN VALUE:
//    nothing.
//
//  COMMENTS:
//    
//

void ExpandChildren(HTREEITEM htiParent)
{
	HTREEITEM htiChild;

	TreeView_Expand(hwndTreeView, htiParent, TVE_EXPAND);                  

	htiChild = TreeView_GetChild(hwndTreeView, htiParent);
	if (ItemHasChild(htiChild))
		ExpandChildren(htiChild); 

	while (htiChild = TreeView_GetNextSibling(hwndTreeView, htiChild))            
		if (ItemHasChild(htiChild))               
			ExpandChildren(htiChild);     
}

// 
//
//  FUNCTION: ItemHasChild(HTREEITEM)
//
//  PURPOSE: determine if a TreeView item has any children.
//
//  PARAMETERS:
//    hti - handle to a TreeView item.
//
//  RETURN VALUE:
//    TRUE     - The item has children.
//    FALSE    - The item has no children.
//
//  COMMENTS:
//    
//

BOOL ItemHasChild(HTREEITEM hti)
{
	 _fmemset((LPBYTE)&tvi, 0, sizeof(TV_ITEM));
	tvi.hItem = hti;
	tvi.mask = TVIF_CHILDREN;
	TreeView_GetItem(hwndTreeView, &tvi);
	return (tvi.cChildren) ? TRUE : FALSE;
}

