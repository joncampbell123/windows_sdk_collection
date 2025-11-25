// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993 - 1996  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE:   mixinfo.c
//
//  PURPOSE:  Obtain information about mixer lines and controls.
//
//  FUNCTIONS:
//    InitMixer          - Mixer services initialization code.
//    GetLineInfo        - Get information for a mixer line.
//    GetLineControls    - Get information for one or more mixer controls.
//
//
//  COMMENTS:
//

#include <windows.h>                // required for all Windows applications.
#include <windowsx.h>

#include "globals.h"                // prototypes specific to this application.
#include "resource.h"
#include "mixinfo.h"


//
//  FUNCTION: InitMixer(HWND)
//
//  PURPOSE: Mixer services initialization code.
//
//  PARAMETERS:
//    hwnd - Application window handle.
//
//  RETURN VALUE:
//    TRUE  - Success
//    FALSE - Initialization failed
//
//  COMMENTS:
//
//    This function is called at application initialization time.  It
//    opens a mixer device and sets a global variable for the handle
//    to the mixer device.  If no mixer device is found, then MixTree
//    will simulate a mixer device instead.
//
//

BOOL InitMixer(HWND hwnd)
{
    MMRESULT result;
    char achMsg[80];
	char achBuff[80], achTitle[80];

    // query for the number of mixer devices in the system
    uNumMixers = mixerGetNumDevs();

    if (uNumMixers)
    {
	// Note - this code just deals with a single mixer device!
	// It uses mixer device id 0 regardless of how many mixer
	// devices are actually present.
	uMixerID = 0;

	// open the mixer device without using any callback mechanism
	result = mixerOpen(&hMixer, uMixerID, 0, 0, 0);
	if (result)
	{
        LoadString(hInst, IDS_ERR_OPEN_MIXER_DEVICE, achBuff, sizeof(achBuff));
        LoadString(hInst, IDS_MIXTREE_ERROR, achTitle, sizeof(achTitle));
        wsprintf((LPSTR)achMsg, (LPSTR)achBuff, result);
        MessageBox(hwnd, achMsg, achTitle, MB_ICONHAND);
	    return (FALSE);
	}

	// query the mixer's capabilities
	result = mixerGetDevCaps(uMixerID, &MixerCaps, sizeof(MIXERCAPS));
	if (result)
	{
        LoadString(hInst, IDS_ERR_GET_MIXER_CAPABILITIES, achBuff, sizeof(achBuff));
        LoadString(hInst, IDS_MIXTREE_ERROR, achTitle, sizeof(achTitle));
        wsprintf((LPSTR)achMsg, (LPSTR)achBuff, result);
        MessageBox(hwnd, achMsg, achTitle, MB_ICONHAND);
	    return (FALSE);
	}

    }
    else
    {
	// provide simulated mixer capabilities
	strcpy(MixerCaps.szPname, "Simulated Mixer Device");
	MixerCaps.cDestinations = SIMDESTLINES;
    }

    // display the mixer device name in the window title
    wsprintf((LPSTR)achMsg, (LPSTR)"MixTree - %s", MixerCaps.szPname);
    SetWindowText(hwnd, achMsg);
		
    return (TRUE);
}


//
//  FUNCTION: GetLineInfo(HWND, LPMIXERLINE, DWORD)
//
//  PURPOSE: Get the information for a mixer line.
//
//  PARAMETERS:
//    hwnd     - application window handle.
//    lpml     - address of a MIXERLINE structure to be filled with
//               information about a mixer line.
//    dwFlags  - flag indicating the method used to retrieve information.
//
//  RETURN VALUE:
//    A return value of 0 indicates success.  Any non-zero return value
//    indicates an error in the mixerGetLineInfo() call.
//
//  COMMENTS: If hMixer = 0, no actual mixer device is present so data from
//    the simulated mixer is returned.
//
//

MMRESULT GetLineInfo(
    HWND hwnd,
    LPMIXERLINE lpml,
    DWORD dwFlags)
{
	char achMsg[80];
    MMRESULT result = 0;
	char achBuff[80], achTitle[80];

    if (hMixer)
    {
	// call mixer services to fill a MIXERLINE structure with info

		result = mixerGetLineInfo(hMixer, lpml, dwFlags);
		if (result)
		{
			LoadString(hInst, IDS_ERR_CALLING_MIXERGETLINEINFO, achBuff, sizeof(achBuff));
			LoadString(hInst, IDS_MIXTREE_ERROR, achTitle, sizeof(achTitle));
			wsprintf((LPSTR)achMsg, (LPSTR)achBuff, result);
			MessageBox(hwnd, achMsg, achTitle, MB_ICONHAND);
		}
    }
    else
    {
	// fill the MIXERLINE structure with simulated mixer info

	switch (dwFlags)
	{
	    case MIXER_GETLINEINFOF_DESTINATION:
		_fmemcpy(lpml,
			 &SimLine[lpml->dwDestination],
			 sizeof(MIXERLINE));
		break;

	    case MIXER_GETLINEINFOF_SOURCE:
		_fmemcpy(lpml,
			 &SimLine[SimSourceID[lpml->dwDestination]
					     [lpml->dwSource]],
			 sizeof(MIXERLINE));
		break;

	    case MIXER_GETLINEINFOF_LINEID:
		_fmemcpy(lpml,
			 &SimLine[lpml->dwLineID],
			 sizeof(MIXERLINE));
		break;

	    default:
		OutputDebugString("GetLineInfo: bad dwFlags\r\n");
		return 1;
	}
    }

    return result;
}


//
//  FUNCTION: GetLineControls(HWND, LPMIXERLINECONTROLS, DWORD)
//
//  PURPOSE: Get the information for one or more mixer controls.
//
//  PARAMETERS:
//    hwnd     - application window handle.
//    lpmlcs   - address of a MIXERLINECONTROLS referencing one or more
//               MIXERLINECONTROL structures to be filled with information
//               about the associated mixer controls.
//    dwFlags  - flag indicating the method used to retrieve information.
//
//  RETURN VALUE:
//    A return value of 0 indicates success.  Any non-zero return value
//    indicates an error in the mixerGetLineControls() call.
//
//  COMMENTS: If hMixer = 0, no actual mixer device is present so data from
//    the simulated mixer is returned.
//
//

MMRESULT GetLineControls(
    HWND hwnd,
    LPMIXERLINECONTROLS lpmlcs,
    DWORD dwFlags)
{
    UINT i;
	char achMsg[80];
    MMRESULT result = 0;
	char achBuff[80], achTitle[80];

    if (hMixer)
    {
	// call mixer services to fill one or more MIXERLINECONTROL structures
	// associated with the MIXERLINECONTROLS structure

		result = mixerGetLineControls(hMixer, lpmlcs, dwFlags);
		if (result)
		{
			LoadString(hInst, IDS_ERR_CALLING_MIXERGETLINECONTROLS, achBuff, sizeof(achBuff));
			LoadString(hInst, IDS_MIXTREE_ERROR, achTitle, sizeof(achTitle));
			wsprintf((LPSTR)achMsg, (LPSTR)achBuff, result);
			MessageBox(hwnd, achMsg, achTitle, MB_ICONHAND);
		}
		}
    else
    {
	// fill one or more MIXERLINECONTROL structures associated with the
	// MIXERLINECONTROLS structure with simulated mixer info

	switch (dwFlags)
	{
	    case MIXER_GETLINECONTROLSF_ALL:
		for (i = 0; i < lpmlcs->cControls; i++)
		    _fmemcpy(&(lpmlcs->pamxctrl[i]),
			&SimLineControl[SimLineControlID[lpmlcs->dwLineID][i]],
			sizeof(MIXERCONTROL));
		break;

	    case MIXER_GETLINECONTROLSF_ONEBYID:
		_fmemcpy(lpmlcs->pamxctrl,
			 &SimLineControl[lpmlcs->dwControlID],
			 sizeof(MIXERCONTROL));
		break;

	    default:
		// other flags not supported
		OutputDebugString("GetLineControls: bad dwFlags\r\n");
		return 1;
	}
    }

    return result;
}
