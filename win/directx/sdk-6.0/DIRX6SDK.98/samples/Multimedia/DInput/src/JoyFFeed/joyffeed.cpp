//-----------------------------------------------------------------------------
// File: JoyFFeed.cpp
//
// Desc: Demonstrates an application which sets a force feedback raw force 
//       determined by the user.
//
//
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define INITGUID
#include <math.h>
#include "JoyFFeed.h"
#include "resource.h"

//-----------------------------------------------------------------------------
// Global variables for the DirectMusic sample 
//-----------------------------------------------------------------------------
IDirectInput*           g_pDI       = NULL;         
IDirectInputDevice2*    g_pJoystick = NULL;
IDirectInputEffect*     g_pEffect   = NULL;
HINSTANCE               g_hInst     = NULL;
BOOL                    g_bActive   = TRUE;
int                     g_nXForce;
int                     g_nYForce;

//-----------------------------------------------------------------------------
// Local function-prototypes
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumFFJoysticksCallback( LPCDIDEVICEINSTANCE pInst, 
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

    // look for a force feedback joystick we can use for this
    // sample program.
    hr = g_pDI->EnumDevices( DIDEVTYPE_JOYSTICK,
                             EnumFFJoysticksCallback,
                             NULL,
                             DIEDFL_ATTACHEDONLY | DIEDFL_FORCEFEEDBACK );
    if ( FAILED(hr) ) 
        return hr;

    if ( NULL == g_pJoystick )
    {
        MessageBox( NULL, 
            "Force feedback joystick not found", 
            "DirectInput Sample", 
            MB_ICONERROR | MB_OK );
        return E_FAIL;
    }

    // Set the data format to "simple joystick" - a predefined data format 
    //
    // A data format specifies which controls on a device we
    // are interested in, and how they should be reported.
    //
    //  This tells DirectInput that we will be passing a
    // DIJOYSTATE structure to IDirectInputDevice2::GetDeviceState.
    // Even though we won't actually do it in this sample.
    // But setting the data format is important so that
    // the DIJOFS_* values work properly.
    hr = g_pJoystick->SetDataFormat( &c_dfDIJoystick );
    if ( FAILED(hr) ) 
        return hr;

    // Set the cooperativity level to let DirectInput know how
    // this device should interact with the system and with other
    // DirectInput applications.
    //
    // Exclusive access is required in order to perform force feedback.
    hr = g_pJoystick->SetCooperativeLevel( hDlg, 
                                        DISCL_EXCLUSIVE | DISCL_FOREGROUND);
    if ( FAILED(hr) ) 
        return hr;

    // Since we will be playing force feedback effects,
    // we should disable the auto-centering spring.
    DIPROPDWORD dipdw;
    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;
    dipdw.dwData = FALSE;

    hr = g_pJoystick->SetProperty( DIPROP_AUTOCENTER, &dipdw.diph );
    if ( FAILED(hr) ) 
        return hr;

    // This application needs only one effect:  Applying raw forces.
    DIEFFECT eff;
    DWORD rgdwAxes[2] = { DIJOFS_X, DIJOFS_Y };
    LONG rglDirection[2] = { 0, 0 };
    DICONSTANTFORCE cf = { 0 };

    eff.dwSize = sizeof(DIEFFECT);
    eff.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
    eff.dwDuration = INFINITE;
    eff.dwSamplePeriod = 0;
    eff.dwGain = DI_FFNOMINALMAX;
    eff.dwTriggerButton = DIEB_NOTRIGGER;
    eff.dwTriggerRepeatInterval = 0;
    eff.cAxes = 2;
    eff.rgdwAxes = rgdwAxes;
    eff.rglDirection = rglDirection;
    eff.lpEnvelope = 0;
    eff.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
    eff.lpvTypeSpecificParams = &cf;

    // create the prepared effect
    hr = g_pJoystick->CreateEffect( GUID_ConstantForce,
                                    &eff,
                                    &g_pEffect,
                                    NULL );
    if ( FAILED(hr) ) 
        return hr;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Function: EnumFFJoysticksCallback
//
// Description: 
//      Called once for each enumerated force feedback joystick.
//      If we find one, create a device interface on it so we can
//      play with it.
//
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumFFJoysticksCallback( LPCDIDEVICEINSTANCE pInst, 
                                       LPVOID lpvContext )
{
    HRESULT                 hr;
    LPDIRECTINPUTDEVICE     pDevice;
    LPDIRECTINPUTDEVICE2    pDevice2;

    // obtain an interface to the enumerated force feedback joystick.
    hr = g_pDI->CreateDevice( pInst->guidInstance, &pDevice, NULL );

    // if it failed, then we can't use this joystick for some
    // bizarre reason.  (Maybe the user unplugged it while we
    // were in the middle of enumerating it.)  So continue enumerating
    if ( FAILED(hr) ) 
        return DIENUM_CONTINUE;

    // query for IDirectInputDevice2
    hr = pDevice->QueryInterface( IID_IDirectInputDevice2, (LPVOID *)&pDevice2);
    pDevice->Release(); // done with old interface now

    if ( FAILED(hr) ) 
    {
        return DIENUM_CONTINUE;  // try again
    }

    // we successfully created an IDirectInputDevice2.  So stop looking 
    // for another one.
    g_pJoystick = pDevice2;

    return DIENUM_STOP;
}




//-----------------------------------------------------------------------------
// Function: SetAcquire
//
// Description: 
//      Acquire or unacquire the mouse, depending on if the app is active
//      Input device must be acquired before the GetDeviceState is called
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

        if (NULL != g_pEffect) 
        {       
            g_pEffect->Start( 1, 0 ); // start the effect
        }

    } 
    else 
    {
        // unacquire the input device 
        g_pJoystick->Unacquire();
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
    // Release any DirectInputEffect objects.
    if (NULL != g_pEffect) 
    {
        g_pEffect->Release();
        g_pEffect = NULL;
    }

    // Unacquire and release any DirectInputDevice objects.
    if (NULL != g_pJoystick) 
    {
        // Unacquire the device one last time just in case 
        // the app tried to exit while the device is still acquired.
        g_pJoystick->Unacquire();

        g_pJoystick->Release();
        g_pJoystick = NULL;
    }

    // Release any DirectInput objects.
    if (NULL != g_pDI) 
    {
        g_pDI->Release();
        g_pDI = NULL;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Function: SetJoyForcesXY
//
// Description: 
//      Apply the X and Y forces to the effect we prepared.
//
//-----------------------------------------------------------------------------
HRESULT SetJoyForcesXY()
{
    HRESULT hr;

    // Modifying an effect is basically the same as creating
    // a new one, except you need only specify the parameters
    // you are modifying; the rest are left alone.
    LONG rglDirection[2] = { g_nXForce, g_nYForce };

    DICONSTANTFORCE cf;
    DIEFFECT eff;
    cf.lMagnitude = (DWORD)sqrt( (double)g_nXForce * (double)g_nXForce +
                                 (double)g_nYForce * (double)g_nYForce );

    eff.dwSize = sizeof(DIEFFECT);
    eff.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
    eff.cAxes = 2;
    eff.rglDirection = rglDirection;
    eff.lpEnvelope = 0;
    eff.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
    eff.lpvTypeSpecificParams = &cf;

    // now set the new parameters and start the effect immediately.
    hr = g_pEffect->SetParameters( &eff, 
                            DIEP_DIRECTION |
                            DIEP_TYPESPECIFICPARAMS |
                            DIEP_START );

    return hr;
}
