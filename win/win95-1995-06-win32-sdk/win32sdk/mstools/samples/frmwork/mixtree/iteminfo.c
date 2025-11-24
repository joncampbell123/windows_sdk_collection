// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE:   iteminfo.c
//
//  PURPOSE:  Obtain and display information associated with TreeView items.  
//
//  FUNCTIONS:
//    GetItemInfo      - Query on information about the mixer line or control 
//                       associated with a TreeView item.
//    DisplayItemInfo  - Display the information for a mixer line or control 
//                       associated with a TreeView item.
//    GetComponentTypeString - Retrieve the description of a mixer line's
//                       component type.
//    GetControlTypeString   - Retrieve the description of a mixer control's
//                       control type.
//
//  COMMENTS:
//

#include <windows.h>                // required for all Windows applications.
#include <windowsx.h>
#include <memory.h>

#include "globals.h"                // prototypes specific to this application.
#include "resource.h"
#include "iteminfo.h"


//
//  FUNCTION: GetItemInfo(HWND, long)
//
//  PURPOSE: Get the information for the mixer line or control associated
//           with a TreeView item.
//
//  PARAMETERS:
//    hwnd     - application window handle.
//    lItem    - index into array of MIXITEMINFO structures identifying
//               TreeView item associated with a mixer line or control.
//
//  RETURN VALUE:
//    TRUE     - Success.
//    FALSE    - Failure.
//
//  COMMENTS:
//    
//

BOOL GetItemInfo(
	HWND hwnd,
	long lItem)
{
	MMRESULT result;

	if (lItem == MIXITEM_NOSELECTION)
		return FALSE;

	switch (amixii[lItem].dwItemType)
	{
		case MIXITEM_UNDEFINED:
			MessageBox(hwnd, "Item type undefined", 
					   "MixTree Error", MB_ICONHAND);
			return FALSE;
			break;

		case MIXITEM_MIXERLINE:              
			_fmemset((LPBYTE)&mlCurrent, 0, sizeof(MIXERLINE));               
			mlCurrent.cbStruct = sizeof(MIXERLINE);  
			mlCurrent.dwLineID = amixii[lItem].dwLineID;
				   
			// get the line information                   
			result = GetLineInfo(hwnd, &mlCurrent, MIXER_GETLINEINFOF_LINEID);
			if (result)
				return FALSE;
					
			break;

		case MIXITEM_MIXERCONTROL:
		{
			MIXERLINECONTROLS mlcs;
		
			_fmemset((LPBYTE)&mlcs, 0, sizeof(MIXERLINECONTROLS));
			mlcs.cbStruct = sizeof(MIXERLINECONTROLS); 
			mlcs.dwControlID = amixii[lItem].dwControlID;
			mlcs.cControls = 1;
			mlcs.cbmxctrl = sizeof(MIXERCONTROL);  
			_fmemset((LPBYTE)&mcCurrent, 0, sizeof(MIXERCONTROL));                      
			mlcs.pamxctrl = (LPMIXERCONTROL)&mcCurrent;    
	  
			result = GetLineControls(hwnd, 
						 &mlcs, 
						 MIXER_GETLINECONTROLSF_ONEBYID);
			if (result)
				return (FALSE);
										
			break;
		}

		default:
			MessageBox(hwnd, "Bad item type", "MixTree Error", MB_ICONHAND);
			return FALSE;
	}

	return TRUE;
}

 
//
//  FUNCTION: DisplayItemInfo(HWND, HDC)
//
//  PURPOSE: Display a TreeView item's mixer line or control information.
//
//  PARAMETERS:
//    hwnd     - application window handle.
//    hdc      - handle to display context.
//
//  RETURN VALUE:
//    nothing.    
//
//  COMMENTS:
//    
//

void DisplayItemInfo(
	HWND hwnd,
	HDC hdc)
{
	RECT rc;
	POINT pt;
	TEXTMETRIC tm;
	SIZE size;
	int dx, dy;
	char achBuf[80];
	char achTypeBuf[80];

	HFONT hfOldFont;

	if (!hwnd || !hdc)
	{
		OutputDebugString("Bad hWnd or hDC in DisplayItemInfo\r\n");
		return;
	}

	hfOldFont = SelectObject(hdc, hfTVFont);

	GetClientRect(hwnd, &rc);

	pt.x = (rc.right / 3);
	PatBlt(hdc, 
		   pt.x + 1, 0,
		   rc.right - pt.x - 1, rc.bottom,
		   WHITENESS);
   
	if (lCurrentItem == MIXITEM_NOSELECTION)
		return;

	pt.x += 10;
	pt.y = 0;
	GetTextMetrics(hdc, &tm);
	dy = tm.tmHeight + tm.tmExternalLeading;

	if (lCurrentItem == MIXITEM_NOSELECTION || 
			amixii[lCurrentItem].dwItemType == MIXITEM_UNDEFINED)
		wsprintf((LPSTR)achBuf, 
			(LPSTR)"Select a mixer line or control with the mouse.");
	else
		wsprintf((LPSTR)achBuf, (LPSTR)"Current Selection:");
	TextOut(hdc, pt.x, pt.y, achBuf, strlen(achBuf));

	switch (amixii[lCurrentItem].dwItemType)
	{
		case MIXITEM_MIXERLINE:              
			GetTextExtentPoint(hdc, "Target.vDriverVersion ", 22, &size);
			dx = size.cx;
		   
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"Item type");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= MIXERLINE");
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"cbStruct");                    
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= %lu", 
					 mlCurrent.cbStruct);
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			wsprintf((LPSTR)achBuf,     
					 (LPSTR)"dwDestination");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));
			wsprintf((LPSTR)achBuf,     
					 (LPSTR)"= %lu", 
					 mlCurrent.dwDestination);
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"dwSource");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= %lu", 
					 mlCurrent.dwSource);
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"dwLineID");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));
			wsprintf((LPSTR)achBuf,                     
					 (LPSTR)"= %lXH", 
					 mlCurrent.dwLineID);
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"fdwLine");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= %lXH", 
					 mlCurrent.fdwLine);
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"dwUser");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf)); 
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= %lu", 
					 mlCurrent.dwUser);             
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			GetComponentTypeString(mlCurrent.dwComponentType, 
								   (LPSTR)achTypeBuf,
								   sizeof(achTypeBuf));
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"dwComponentType");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= %s", 
					 (LPSTR)achTypeBuf);
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"cChannels");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= %lu", 
					 mlCurrent.cChannels);
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"cConnections");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= %lu", 
					 mlCurrent.cConnections);
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"cControls");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));  
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= %lu", 
					 mlCurrent.cControls);
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"szShortName");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= %s", 
					 (LPSTR)mlCurrent.szShortName);
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"szName");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf)); 
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= %s", 
					 (LPSTR)mlCurrent.szName);
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"Target.dwType");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf)); 
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= %lu", 
					 mlCurrent.Target.dwType);  
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			if (mlCurrent.Target.dwType == MIXERLINE_TARGETTYPE_UNDEFINED)
				// ignore the remainder of the Target structure
				break;

			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"Target.dwDeviceID");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf)); 
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= %lu", 
					 mlCurrent.Target.dwType);
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"Target.wMid");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf)); 
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= %u", 
					 mlCurrent.Target.wMid);
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"Target.wPid");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf)); 
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= %u", 
					 mlCurrent.Target.wPid);
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"Target.vDriverVersion = %u.%u", 
					 HIBYTE(mlCurrent.Target.vDriverVersion),
					 LOBYTE(mlCurrent.Target.vDriverVersion));
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));
		   
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"szPname");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf)); 
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= %s", 
					 (LPSTR)mlCurrent.Target.szPname);
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));
			break;

		case MIXITEM_MIXERCONTROL:
			GetTextExtentPoint(hdc, "Bounds.dwMaximum ", 17, &size);
			dx = size.cx;

			wsprintf((LPSTR)achBuf, 
				(LPSTR)"Item type");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));  
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= MIXERCONTROL");
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));      

			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"cbStruct");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= %lu", 
					 mcCurrent.cbStruct);
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"dwControlID");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= %lu", 
					 mcCurrent.dwControlID);
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			GetControlTypeString(mcCurrent.dwControlType, 
								 (LPSTR)achTypeBuf,
								 sizeof(achTypeBuf));
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"dwControlType");                                    
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= %s", 
					 (LPSTR)achTypeBuf);
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"fdwControl");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= %lu", 
					 mcCurrent.fdwControl);
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"cMultipleItems");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= %lu", 
					 mcCurrent.cMultipleItems);
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"szShortName");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= %s", 
					 (LPSTR)mcCurrent.szShortName);
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"szName");
			TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));     
			wsprintf((LPSTR)achBuf, 
					 (LPSTR)"= %s", 
					 (LPSTR)mcCurrent.szName);
			TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

			switch (mcCurrent.dwControlType)
			{                                 
				// controls with unsigned bounds 

				case MIXERCONTROL_CONTROLTYPE_CUSTOM :
					// custom control could use either signed or unsigned
					// bounds -- arbitrarily we display it as unsigned
				case MIXERCONTROL_CONTROLTYPE_UNSIGNEDMETER :
				case MIXERCONTROL_CONTROLTYPE_UNSIGNED :
				case MIXERCONTROL_CONTROLTYPE_FADER :
				case MIXERCONTROL_CONTROLTYPE_VOLUME :
				case MIXERCONTROL_CONTROLTYPE_BASS :
				case MIXERCONTROL_CONTROLTYPE_TREBLE :
				case MIXERCONTROL_CONTROLTYPE_EQUALIZER :
				case MIXERCONTROL_CONTROLTYPE_MICROTIME :
				case MIXERCONTROL_CONTROLTYPE_MILLITIME :
					
					// display unsigned Bounds information                    
					wsprintf((LPSTR)achBuf, 
							 (LPSTR)"Bounds.dwMinimum");
					TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));     
					wsprintf((LPSTR)achBuf, 
							 (LPSTR)"= %lu", 
							 mcCurrent.Bounds.dwMinimum);
					TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

					wsprintf((LPSTR)achBuf, 
							 (LPSTR)"Bounds.dwMaximum");
					TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));     
					wsprintf((LPSTR)achBuf, 
							 (LPSTR)"= %lu", 
							 mcCurrent.Bounds.dwMaximum);
					TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));
					break;

				// controls with signed bounds

				case MIXERCONTROL_CONTROLTYPE_PEAKMETER :                
				case MIXERCONTROL_CONTROLTYPE_SIGNEDMETER :
				case MIXERCONTROL_CONTROLTYPE_SIGNED :
				case MIXERCONTROL_CONTROLTYPE_SLIDER :
				case MIXERCONTROL_CONTROLTYPE_PAN :
				case MIXERCONTROL_CONTROLTYPE_QSOUNDPAN :

					// display signed Bounds information
					wsprintf((LPSTR)achBuf, 
							 (LPSTR)"Bounds.lMinimum");
					TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));     
					wsprintf((LPSTR)achBuf, 
							 (LPSTR)"= %li", 
							 mcCurrent.Bounds.lMinimum);
					TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

					wsprintf((LPSTR)achBuf, 
							 (LPSTR)"Bounds.lMaximum");
					TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));     
					wsprintf((LPSTR)achBuf, 
							 (LPSTR)"= %li", 
							 mcCurrent.Bounds.lMaximum);
					TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));

					break;

				// these controls do not use Bounds
				case MIXERCONTROL_CONTROLTYPE_BOOLEAN :
				case MIXERCONTROL_CONTROLTYPE_ONOFF :
				case MIXERCONTROL_CONTROLTYPE_MUTE :
				case MIXERCONTROL_CONTROLTYPE_MONO :
				case MIXERCONTROL_CONTROLTYPE_LOUDNESS :
				case MIXERCONTROL_CONTROLTYPE_STEREOENH :
				case MIXERCONTROL_CONTROLTYPE_SINGLESELECT :
				case MIXERCONTROL_CONTROLTYPE_MUX :
				case MIXERCONTROL_CONTROLTYPE_MULTIPLESELECT :
				case MIXERCONTROL_CONTROLTYPE_MIXER :
				default:                                    
					break;
			}

			if (mcCurrent.dwControlType == MIXERCONTROL_CONTROLTYPE_CUSTOM)
			{              
				wsprintf((LPSTR)achBuf, 
						 (LPSTR)"Metrics.cbCustomData");
				TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));     
				wsprintf((LPSTR)achBuf, 
						 (LPSTR)"= %lu", 
						 mcCurrent.Metrics.cbCustomData);
				TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));
			}
			else
			{              
				wsprintf((LPSTR)achBuf, 
						 (LPSTR)"Metrics.cSteps");
				TextOut(hdc, pt.x, pt.y += dy, achBuf, strlen(achBuf));     
				wsprintf((LPSTR)achBuf, 
						 (LPSTR)"= %lu", 
						 mcCurrent.Metrics.cSteps);
				TextOut(hdc, pt.x + dx, pt.y,  achBuf, strlen(achBuf));
			}

			break;

		case MIXITEM_UNDEFINED:
		default:
			// nothing to display        
			break;
	}

	SelectObject(hdc, hfOldFont);
}


//
//  FUNCTION: GetComponentTypeString(DWORD, LPSTR, int)
//
//  PURPOSE: Get the string describing the given type of mixer component.
//
//  PARAMETERS:
//    dwComponentType - indicates the type of mixer component.
//    lpszBuf         - points to buffer used to receive string.
//    cbBuf           - size of buffer used to receive string.
//
//  RETURN VALUE:
//    nothing.
//    
//  COMMENTS:
//    
//

void GetComponentTypeString(
	DWORD dwComponentType,
	LPSTR lpszBuf,
	int cbBuf)
{
	int i;

	for (i = 0; i < MIXERCOMPONENT_TYPES; i++)
		if (dwComponentType == adwComponentType[i][0])
		{
			LoadString(hInst, adwComponentType[i][1], lpszBuf, cbBuf);
			return;
		}

	// if dwComponentType was not found in the array, buffer is returned empty
}


//
//  FUNCTION: GetControlTypeString(DWORD, LPSTR, int)
//
//  PURPOSE: Get the string describing the given type of mixer control.
//
//  PARAMETERS:
//    dwControlType  - indicates the type of mixer control.
//    lpszBuf        - points to buffer used to receive string.
//    cbBuf          - size of buffer used to receive string.
//
//  RETURN VALUE:
//    nothing.
//    
//  COMMENTS:
//    
//

void GetControlTypeString(
	DWORD dwControlType,
	LPSTR lpszBuf,
	int cbBuf)
{
	int i;

	for (i = 0; i < MIXERCONTROL_TYPES; i++)
		if (dwControlType == adwControlType[i][0])
		{
			LoadString(hInst, adwControlType[i][1], lpszBuf, cbBuf);
			return;
		}

	// if dwControlType was not found in the array, buffer is returned empty
}
