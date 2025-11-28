//-----------------------------------------------------------------------------
// File: KeybdImm.cpp
//
// Desc: Demonstrates an application which receives immediate 
//       keyboard data in non-exclusive mode via a dialog timer.
//
//
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define INITGUID
#include "KeybdImm.h"
#include "resource.h"

//-----------------------------------------------------------------------------
// Global variables for the DirectMusic sample 
//-----------------------------------------------------------------------------
IDirectInput*           g_pDI       = NULL;         
IDirectInputDevice*     g_pKeyboard = NULL;     
HINSTANCE               g_hInst     = NULL;
BOOL                    g_bActive   = TRUE;     




//-----------------------------------------------------------------------------
// Function: InitDirectInput
//
// Description: 
//      Initialize the DirectInput variables.
//
//-----------------------------------------------------------------------------
HRESULT InitDirectInput( HWND hDlg )
{
    HRESULT hr;

    // Register with the DirectInput subsystem and get a pointer
    // to a IDirectInput interface we can use.
    hr = DirectInputCreate( g_hInst, DIRECTINPUT_VERSION, &g_pDI, NULL );
    if ( FAILED(hr) ) 
        return hr;

    // Obtain an interface to the system keyboard device.
    hr = g_pDI->CreateDevice( GUID_SysKeyboard, &g_pKeyboard, NULL );
    if ( FAILED(hr) ) 
        return hr;

    // Set the data format to "keyboard format" - a predefined data format 
    //
    // A data format specifies which controls on a device we
    // are interested in, and how they should be reported.
    //
    // This tells DirectInput that we will be passing an array
    // of 256 bytes to IDirectInputDevice::GetDeviceState.
    hr = g_pKeyboard->SetDataFormat( &c_dfDIKeyboard );
    if ( FAILED(hr) ) 
        return hr;

    // Set the cooperativity level to let DirectInput know how
    // this device should interact with the system and with other
    // DirectInput applications.
    hr = g_pKeyboard->SetCooperativeLevel( hDlg, 
                                        DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    if ( FAILED(hr) ) 
        return hr;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Function: SetAcquire
//
// Description: 
//      Acquire or unacquire the keyboard, depending on if the app is active
//       Input device must be acquired before the GetDeviceState is called
//
//-----------------------------------------------------------------------------
HRESULT SetAcquire( HWND hDlg )
{
    char szText[128];
    HWND hDlgText;

    // nothing to do if g_pKeyboard is NULL
    if (NULL == g_pKeyboard)
        return S_FALSE;

    if (g_bActive) 
    {
        // acquire the input device 
        g_pKeyboard->Acquire();
    } 
    else 
    {
        // update the dialog text 
        strcpy( szText, "Unacquired" );
        hDlgText = GetDlgItem( hDlg, IDC_KEYBD_STATE );
        SetWindowText( hDlgText, szText );

        // unacquire the input device 
        g_pKeyboard->Unacquire();
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Function: UpdateInputState
//
// Description: 
//      Get the input device's state and display it.
//
//-----------------------------------------------------------------------------
HRESULT UpdateInputState( HWND hDlg )
{
    char szOldText[128];    // previous keyboard state text
    char szNewText[128];    // current  keyboard state text
    HWND hDlgText;          // handle to static text box
    int  i;

    if (NULL != g_pKeyboard) 
    {
        BYTE diks[256];         // DirectInput keyboard state buffer 
        HRESULT hr;

        hr = DIERR_INPUTLOST;

        // if input is lost then acquire and keep trying 
        while ( DIERR_INPUTLOST == hr ) 
        {
            // get the input's device state, and put the state in dims
            hr = g_pKeyboard->GetDeviceState( sizeof(diks), &diks );

            if ( hr == DIERR_INPUTLOST )
            {
                // DirectInput is telling us that the input stream has
                // been interrupted.  We aren't tracking any state
                // between polls, so we don't have any special reset
                // that needs to be done.  We just re-acquire and
                // try again.
                hr = g_pKeyboard->Acquire();
                if ( FAILED(hr) )  
                    return hr;
            }
        }

        if ( FAILED(hr) )  
            return hr;

        // build the new status string.
        // display the index values of the keys that are down
        char *psz = szNewText;
        for (i = 0; i < 256; i++) 
        {
            if (diks[i] & 0x80) 
            {
                psz += wsprintf(psz, "%02x ", i);
            }
        }
        *psz = 0;   // Terminate the string 

        // if anything changed then repaint - avoid flicker
        hDlgText = GetDlgItem( hDlg, IDC_KEYBD_STATE );
        GetWindowText( hDlgText, szOldText, 255 );

        if ( 0 != lstrcmp( szOldText, szNewText ) ) 
        {
            // set the text on the dialog
            SetWindowText( hDlgText, szNewText );
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Function: FreeDirectInput
//
// Description: 
//      Initialize the DirectInput variables.
//
//-----------------------------------------------------------------------------
HRESULT FreeDirectInput()
{
    // Unacquire and release any DirectInputDevice objects.
    if (NULL != g_pKeyboard) 
    {
        // Unacquire the device one last time just in case 
        // the app tried to exit while the device is still acquired.
        g_pKeyboard->Unacquire();

        g_pKeyboard->Release();
        g_pKeyboard = NULL;
    }

    // Release any DirectInput objects.
    if (NULL != g_pDI) 
    {
        g_pDI->Release();
        g_pDI = NULL;
    }

    return S_OK;
}

