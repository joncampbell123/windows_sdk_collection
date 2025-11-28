//-----------------------------------------------------------------------------
// File: JoystImm.cpp
//
// Desc: Demonstrates an application which receives immediate 
//       joystick data in exclusive mode via a dialog timer.
//
//
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define INITGUID
#include "JoystImm.h"
#include "resource.h"

//-----------------------------------------------------------------------------
// Global variables for the DirectMusic sample 
//-----------------------------------------------------------------------------
IDirectInput*           g_pDI               = NULL;         
IDirectInputDevice*     g_pJoystick         = NULL;     
IDirectInputDevice2*    g_pJoystickDevice2  = NULL;  // needed to poll joystick
HINSTANCE               g_hInst             = NULL;
BOOL                    g_bActive           = TRUE;     

//-----------------------------------------------------------------------------
// Local function-prototypes
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumJoysticksCallback( LPCDIDEVICEINSTANCE pInst, 
                                     LPVOID lpvContext );




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

    // look for a simple joystick we can use for this sample program.
    hr = g_pDI->EnumDevices( DIDEVTYPE_JOYSTICK,
                             EnumJoysticksCallback,
                             NULL,
                             DIEDFL_ATTACHEDONLY );
    if ( FAILED(hr) ) 
        return hr;

    if ( NULL == g_pJoystick )
    {
        MessageBox( NULL, 
            "Joystick not found", 
            "DirectInput Sample", 
            MB_ICONERROR | MB_OK );
        return E_FAIL;
    }

    // Set the data format to "simple joystick" - a predefined data format 
    //
    // A data format specifies which controls on a device we
    // are interested in, and how they should be reported.
    //
    // This tells DirectInput that we will be passing a
    // DIJOYSTATE structure to IDirectInputDevice::GetDeviceState.
    hr = g_pJoystick->SetDataFormat( &c_dfDIJoystick );
    if ( FAILED(hr) ) 
        return hr;

    // Set the cooperativity level to let DirectInput know how
    // this device should interact with the system and with other
    // DirectInput applications.
    hr = g_pJoystick->SetCooperativeLevel( hDlg, 
                                        DISCL_EXCLUSIVE | DISCL_FOREGROUND);
    if ( FAILED(hr) ) 
        return hr;

    // set the range of the joystick axis
    DIPROPRANGE diprg; 

    diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
    diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
    diprg.diph.dwHow        = DIPH_BYOFFSET; 
    diprg.lMin              = -1000; 
    diprg.lMax              = +1000; 

    diprg.diph.dwObj = DIJOFS_X;    // set the x-axis range
    hr = g_pJoystick->SetProperty( DIPROP_RANGE, &diprg.diph );
    if ( FAILED(hr) ) 
        return hr;

    diprg.diph.dwObj = DIJOFS_Y;    // set the y-axis range
    hr = g_pJoystick->SetProperty( DIPROP_RANGE, &diprg.diph );
    if ( FAILED(hr) ) 
        return hr;

    diprg.diph.dwObj = DIJOFS_Z;    // set the z-axis range
    hr = g_pJoystick->SetProperty( DIPROP_RANGE, &diprg.diph );
    if ( FAILED(hr) ) 
        return hr;

    diprg.diph.dwObj = DIJOFS_RZ;   // set the rudder range
    hr = g_pJoystick->SetProperty( DIPROP_RANGE, &diprg.diph );
    if ( FAILED(hr) ) 
        return hr;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Function: EnumJoysticksCallback
//
// Description: 
//      Called once for each enumerated joystick. If we find one, 
//       create a device interface on it so we can play with it.
//
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumJoysticksCallback( LPCDIDEVICEINSTANCE pInst, 
                                     LPVOID lpvContext )
{
    HRESULT             hr;
    LPDIRECTINPUTDEVICE pDevice;

    // obtain an interface to the enumerated force feedback joystick.
    hr = g_pDI->CreateDevice( pInst->guidInstance, &pDevice, NULL );

    // if it failed, then we can't use this joystick for some
    // bizarre reason.  (Maybe the user unplugged it while we
    // were in the middle of enumerating it.)  So continue enumerating
    if ( FAILED(hr) ) 
        return DIENUM_CONTINUE;

    // we successfully created an IDirectInputDevice.  So stop looking 
    // for another one.
    g_pJoystick = pDevice;

    // query for IDirectInputDevice2 - we need this to poll the joystick 
    pDevice->QueryInterface( IID_IDirectInputDevice2, 
                    (LPVOID *)&g_pJoystickDevice2 );

    return DIENUM_STOP;
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
    // nothing to do if g_pJoystick is NULL
    if (NULL == g_pJoystick)
        return S_FALSE;

    if (g_bActive) 
    {
        // acquire the input device 
        g_pJoystick->Acquire();
    } 
    else 
    {
        // unacquire the input device 
        g_pJoystick->Unacquire();
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
    HRESULT     hr;
    char        szText[128];       // keyboard state text
    DIJOYSTATE  js;          // DirectInput joystick state 
    char*       psz;


    if (NULL != g_pJoystick) 
    {
        hr = DIERR_INPUTLOST;

        // if input is lost then acquire and keep trying 
        while ( DIERR_INPUTLOST == hr ) 
        {
            // poll the joystick to read the current state
            hr = g_pJoystickDevice2->Poll();
            if ( FAILED(hr) )
                return hr;

            // get the input's device state, and put the state in dims
            hr = g_pJoystick->GetDeviceState( sizeof(DIJOYSTATE), &js );

            if ( hr == DIERR_INPUTLOST )
            {
                // DirectInput is telling us that the input stream has
                // been interrupted.  We aren't tracking any state
                // between polls, so we don't have any special reset
                // that needs to be done.  We just re-acquire and
                // try again.
                hr = g_pJoystick->Acquire();
                if ( FAILED(hr) )  
                    return hr;
            }
        }

        if ( FAILED(hr) )  
            return hr;

        // display joystick state to dialog
        wsprintf( szText, "%d", js.lX ); 
        SetWindowText( GetDlgItem( hDlg, IDC_X_AXIS ), szText );

        wsprintf( szText, "%d", js.lY ); 
        SetWindowText( GetDlgItem( hDlg, IDC_Y_AXIS ), szText );

        wsprintf( szText, "%d", js.lZ ); 
        SetWindowText( GetDlgItem( hDlg, IDC_Z_AXIS ), szText );

        wsprintf( szText, "%d", js.lRz ); 
        SetWindowText( GetDlgItem( hDlg, IDC_Z_ROT ), szText );

        wsprintf( szText, "%d", js.lRz ); 
        SetWindowText( GetDlgItem( hDlg, IDC_Z_ROT ), szText );

        wsprintf( szText, "%d", js.rgdwPOV[0] ); 
        SetWindowText( GetDlgItem( hDlg, IDC_POV ), szText );

        // fill up text with which buttons are pressed
        psz = szText;
        for (int i = 0; i < 32; i++)
        {
            if ( js.rgbButtons[ i ] & 0x80 )
            {
                psz += wsprintf(psz, "%02d ", i );
            }
        }
        *psz = 0;   // Terminate the string 

        SetWindowText( GetDlgItem( hDlg, IDC_BUTTONS ), szText );
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
    if (NULL != g_pJoystick) 
    {
        // Unacquire the device one last time just in case 
        // the app tried to exit while the device is still acquired.
        g_pJoystick->Unacquire();

        g_pJoystick->Release();
        g_pJoystick = NULL;
    }

    if (NULL != g_pJoystickDevice2)
    {
        g_pJoystickDevice2->Release();
        g_pJoystickDevice2 = NULL;
    }

    // Release any DirectInput objects.
    if (NULL != g_pDI) 
    {
        g_pDI->Release();
        g_pDI = NULL;
    }

    return S_OK;
}

