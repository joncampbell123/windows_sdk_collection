//////////////////////////////////////////////////////////////////////////////
//
// File:        FDAUDIO.CPP
// Contents:    Audio related routines for full-duplex device usage
//
// Description: This file includes functionality for enumerating devices &
//              formats, initializing devices and buffers, and the CALLBACK
//              procedures for dealing with full duplex.
//

#include "fdfilter.h"
#include "fdaudio.h"
#include "filter.h"

static BOOL IsWaveDeviceInList( UINT, WAVEINCAPS * );
static BOOL IsDSDeviceInList( LPGUID );


//////////////////////////////////////////////////////////////////////////////
//
//
//
//
BOOL InitSoundDevices( BOOL fAllowFormatCancel )
    {
    if( NULL == gpwfxOutput )
        {
        gpwfxOutput = new WAVEFORMATEX;
        if( NULL == gpwfxOutput )
            goto ISD_Fail;
        ZeroMemory( gpwfxOutput, sizeof(WAVEFORMATEX));
        gpwfxOutput->wFormatTag = WAVE_FORMAT_PCM;
        DPF( 3, "Created gpwfxOutput" );
        DPWFX( 3, gpwfxOutput );
        }

    if( NULL == gpwfxInput )
        {
        gpwfxInput = new WAVEFORMATEX;
        if( NULL == gpwfxInput )
            goto ISD_Fail;
        ZeroMemory( gpwfxInput, sizeof(WAVEFORMATEX));
        gpwfxInput->wFormatTag = WAVE_FORMAT_PCM;
        DPF( 3, "Created gpwfxInput" );
        DPWFX( 3, gpwfxInput );
        }

    EnumDSDevices();
    if( NULL == gpdsddOutputDevices )
        {
        MessageBox( ghMainWnd, "No DirectSound devices are available!", gszAppName, MB_OK );
        goto ISD_Fail;
        }

    EnumWaveDevices();
    if( NULL == gpwddInputDevices )
        {
        MessageBox( ghMainWnd, "No waveIn devices are available!", gszAppName, MB_OK );
        goto ISD_Fail;
        }

    if( !DialogBox( ghInst, MAKEINTRESOURCE(IDD_DEVICES),
                    ghMainWnd, (DLGPROC)SelectDevicesDlgProc ))
        goto ISD_Fail;

    if( !InitPrimarySoundBuffer())
        goto ISD_Fail;

    if( !DialogBoxParam( ghInst, MAKEINTRESOURCE(IDD_FORMATS),
                            ghMainWnd, (DLGPROC)SelectFormatsDlgProc,
                            (LPARAM)fAllowFormatCancel ))
        goto ISD_Fail;

    return TRUE;

ISD_Fail:
    return FALSE;
    }


BOOL ReOpenSoundDevices( BOOL fAllowFormatCancel )
    {
    if( NULL == gpwfxOutput )
        {
        gpwfxOutput = new WAVEFORMATEX;
        if( NULL == gpwfxOutput )
            goto ROSD_Fail;
        ZeroMemory( gpwfxOutput, sizeof(WAVEFORMATEX));
        gpwfxOutput->wFormatTag = WAVE_FORMAT_PCM;
        DPF( 3, "Created gpwfxOutput" );
        DPWFX( 3, gpwfxOutput );
        }

    if( NULL == gpwfxInput )
        {
        gpwfxInput = new WAVEFORMATEX;
        if( NULL == gpwfxInput )
            goto ROSD_Fail;
        ZeroMemory( gpwfxInput, sizeof(WAVEFORMATEX));
        gpwfxInput->wFormatTag = WAVE_FORMAT_PCM;
        DPF( 3, "Created gpwfxInput" );
        DPWFX( 3, gpwfxInput );
        }

    EnumDSDevices();
    if( NULL == gpdsddOutputDevices )
        {
        MessageBox( ghMainWnd, "No DirectSound devices are available!", gszAppName, MB_OK );
        goto ROSD_Fail;
        }

    EnumWaveDevices();
    if( NULL == gpwddInputDevices )
        {
        MessageBox( ghMainWnd, "No waveIn devices are available!", gszAppName, MB_OK );
        goto ROSD_Fail;
        }

    if( !DialogBox( ghInst, MAKEINTRESOURCE(IDD_DEVICES),
                    ghMainWnd, (DLGPROC)SelectDevicesDlgProc ))
        goto ROSD_Fail;

    StopBuffers();
    CloseWaveDevice();
    DestroyBuffers();
    CloseDSDevice();

    if( !InitPrimarySoundBuffer())
        goto ROSD_Fail;

    if( !DialogBoxParam( ghInst, MAKEINTRESOURCE(IDD_FORMATS),
                            ghMainWnd, (DLGPROC)SelectFormatsDlgProc,
                            (LPARAM)fAllowFormatCancel ))
        goto ROSD_Fail;

    return TRUE;

ROSD_Fail:
    return FALSE;
    }


//////////////////////////////////////////////////////////////////////////////
//
//
//
//
BOOL IsWaveDeviceInList( UINT iDev, WAVEINCAPS *pwic )
    {
    PWAVEDEVICEDESC pwdd = gpwddInputDevices;

    while( pwdd )
        {
        if( pwdd->uID == iDev && pwdd->dwFormats == pwic->dwFormats
            && pwdd->nChannels == pwic->wChannels
            && !lstrcmp(pwdd->szDeviceDesc, pwic->szPname))
            return TRUE;

        pwdd = pwdd->pNext;
        }
    return FALSE;
    }


//////////////////////////////////////////////////////////////////////////////
//
//
//
//
BOOL IsDSDeviceInList( LPGUID lpguDevice )
    {
    PDSDEVICEDESC pdsdd = gpdsddOutputDevices;

    while( pdsdd )
        {
        // This works because operator== is overloaded for GUIDS
        if( NULL == lpguDevice )
            {
            if( pdsdd->guDevice == GUID_NULL )
                return TRUE;
            }
        else if( pdsdd->guDevice == *lpguDevice )
            return TRUE;

        pdsdd = pdsdd->pNext;
        }
    return FALSE;
    }


//////////////////////////////////////////////////////////////////////////////
// EnumDSDevices()
//
//    Enumerates the DirectSound devices with the help of DirectSoundEnumerate
// and DSEnumProc.  Adds entries to a global list about each device.
//
BOOL EnumDSDevices( void )
    {
    DirectSoundEnumerate((LPDSENUMCALLBACK)DSEnumProc, NULL );
    return TRUE;
    }


//////////////////////////////////////////////////////////////////////////////
// EnumWaveDevices()
//
//    This function enumerates the waveIn devices and builds a list of them.
//
BOOL EnumWaveDevices( void )
    {
    UINT            nDevices, iDev;
    PWAVEDEVICEDESC pwddNew;
    WAVEINCAPS      wiCaps;

    nDevices = waveInGetNumDevs();
    if( !nDevices )
        return FALSE;

    for( iDev = 0; iDev < nDevices; iDev++ )
        {
        if( MMSYSERR_NOERROR != waveInGetDevCaps( iDev, &wiCaps, sizeof(wiCaps)))
            continue;

        if( !IsWaveDeviceInList( iDev, &wiCaps ))
            {
            if(( pwddNew = new WAVEDEVICEDESC ) == NULL )
                continue;
            pwddNew->uID = iDev;
            pwddNew->dwFormats = wiCaps.dwFormats;
            pwddNew->nChannels = wiCaps.wChannels;
            lstrcpy( pwddNew->szDeviceDesc, wiCaps.szPname );

            pwddNew->pNext = gpwddInputDevices;
            gpwddInputDevices = pwddNew;
            }
        }

    if( NULL == gpwddInputDevices )
        return FALSE;

    return TRUE;
    }


//////////////////////////////////////////////////////////////////////////////
// InitPrimarySoundBuffer()
//
//    Creates and initializes the primary sound buffer for the application.
// We need the primary buffer in order to get the 3D listener interface and
// also to select output format type.
//
BOOL InitPrimarySoundBuffer( void )
    {
    HRESULT         dsrval;
    DSBUFFERDESC    dsbd;

    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC));
    
    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;

    if( FAILED( dsrval = DirectSoundCreate( &gpdsddOut->guDevice, &gpds, NULL )))
        {
        DPF( 0, "Couldn't open DirectSound device (%s)", TranslateDSError(dsrval));
        goto IPSB_ExitError;
        }

    if( FAILED( dsrval = gpds->SetCooperativeLevel( ghMainWnd, DSSCL_PRIORITY )))
        {
        DPF( 0, "Couldn't get PRIORITY cooperative level (%s)", TranslateDSError(dsrval));
        goto IPSB_ExitError;
        }

    if( FAILED( dsrval = gpds->CreateSoundBuffer( &dsbd, &gpdsbPrimary, NULL )))
        {
        DPF( 0, "Couldn't create primary buffer (%s)", TranslateDSError(dsrval));
        goto IPSB_ExitError;
        }

    return TRUE;

IPSB_ExitError:
    if( NULL != gpdsbPrimary )
        {
        DPF( 1, "Releasing Primary in InitPrimarySoundBuffer() error cleanup" );
        gpdsbPrimary->Release();
        gpdsbPrimary = NULL;
        }
    if( NULL != gpds )
        {
        DPF( 1, "Releasing DSound object in InitPrimarySoundBuffer() error cleanup" );
        gpds->Release();
        gpds= NULL;
        }

    return FALSE;
    }


//////////////////////////////////////////////////////////////////////////////
// DSEnumProc()
//
//   DirectSoundEnumerate() callback procedure which fills the DSDEVICEDESC list
// with data about available devices.
//
BOOL CALLBACK DSEnumProc( LPGUID lpguDevice, LPSTR lpszDesc,
                                LPSTR lpszDrvName, LPVOID lpContext )
    {
    PDSDEVICEDESC   pdsddNew;

    if( !IsDSDeviceInList( lpguDevice ))
        {
        if(( pdsddNew = new DSDEVICEDESC ) == NULL )
            {
            return TRUE;
            }

        ZeroMemory( pdsddNew, sizeof(DSDEVICEDESC));

        if( NULL != lpguDevice )
            pdsddNew->guDevice = *lpguDevice;
        else
            pdsddNew->guDevice = GUID_NULL;

        if(( pdsddNew->pszDeviceDesc = new char[lstrlen(lpszDesc)+1]) == NULL )
            {
            delete pdsddNew;
            return TRUE;
            }
        lstrcpy( pdsddNew->pszDeviceDesc, lpszDesc );

        pdsddNew->pNext = gpdsddOutputDevices;
        gpdsddOutputDevices = pdsddNew;
        }

    return TRUE;
    }


//////////////////////////////////////////////////////////////////////////////
//
//
//
//
void ClearDSDeviceList( void )
    {
    PDSDEVICEDESC   pdsddCur, pdsddTemp;

    pdsddCur = gpdsddOutputDevices;

    while( pdsddCur )
        {
        if( NULL != pdsddCur->pszDeviceDesc )
            delete[] pdsddCur->pszDeviceDesc;

        pdsddTemp = pdsddCur->pNext;
        delete pdsddCur;
        pdsddCur = pdsddTemp;
        }
    gpdsddOutputDevices = NULL;
    }


//////////////////////////////////////////////////////////////////////////////
//
//
//
//
void ClearWaveDeviceList( void )
    {
    PWAVEDEVICEDESC pwddCur, pwddTemp;

    pwddCur = gpwddInputDevices;

    while( pwddCur )
        {
        pwddTemp = pwddCur->pNext;
        delete pwddCur;
        pwddCur = pwddTemp;
        }
    gpwddInputDevices = NULL;
    }


//////////////////////////////////////////////////////////////////////////////
//
//
//
//
void FillInputDeviceCombo( HWND hCombo )
    {
    PWAVEDEVICEDESC pwdd = gpwddInputDevices;
    int             idx;

    if( NULL == hCombo )
        return;

    while( pwdd )
        {
        idx = ComboBox_InsertString( hCombo, -1, pwdd->szDeviceDesc );
        ComboBox_SetItemData( hCombo, idx, pwdd );
        pwdd = pwdd->pNext;
        }
    if( NULL != gpwddInputDevices )
        ComboBox_SetCurSel( hCombo, 0 );
    }


//////////////////////////////////////////////////////////////////////////////
//
//
//
//
void FillOutputDeviceCombo( HWND hCombo )
    {
    PDSDEVICEDESC   pdsdd = gpdsddOutputDevices;
    int             idx;

    if( NULL == hCombo )
        return;

    while( pdsdd )
        {
        idx = ComboBox_InsertString( hCombo, -1, pdsdd->pszDeviceDesc );
        ComboBox_SetItemData( hCombo, idx, pdsdd );
        pdsdd = pdsdd->pNext;
        }
    if( NULL != gpdsddOutputDevices )
        ComboBox_SetCurSel( hCombo, 0 );
    }


//////////////////////////////////////////////////////////////////////////////
//
//
//
//
void ScanAvailableDSFormats( void )
    {
    WAVEFORMATEX    wfx;
    HRESULT         dsrval;
    HCURSOR         hCursor;
    int             i;

    DPF( 3, "Scanning %u DirectSound formats for availability", NUM_FORMATCODES );
    if( NULL == gpds || NULL == gpdsbPrimary )
        {
        for( i = 0; i < NUM_FORMATCODES; i++ )
            aOutputFormats[i].fEnabled = FALSE;
        return;
        }

    // This might take a second or two, so throw up the hourglass
    hCursor = GetCursor();
    SetCursor( LoadCursor( NULL, IDC_WAIT ));

    ZeroMemory( &wfx, sizeof(wfx));
    wfx.wFormatTag = WAVE_FORMAT_PCM;
 
    for( i = 0; i < NUM_FORMATCODES; i++ )
        {
        FormatCodeToWFX( aOutputFormats[i].dwCode, &wfx );

        if( FAILED( dsrval = gpdsbPrimary->SetFormat( &wfx )))
            {
            DPF( 5, "Failed with SetFormat() for %u format", aOutputFormats[i].dwCode );
            aOutputFormats[i].fEnabled = FALSE;
            }
        else
            {
            DPF( 5, "Succeeded with SetFormat() for %u format", aOutputFormats[i].dwCode );
            aOutputFormats[i].fEnabled = TRUE;
            }
        }
    SetCursor( hCursor );
    }


//////////////////////////////////////////////////////////////////////////////
//
//
//
//
void ScanAvailableWaveFormats( void )
    {
    WAVEFORMATEX    wfx;
    HCURSOR         hCursor;
    MMRESULT        mmrval;
    int             i;

    DPF( 3, "Scanning %u waveIn formats for availability", NUM_FORMATCODES );

    // This might take a second or two, so throw up the hourglass
    hCursor = GetCursor();
    SetCursor( LoadCursor( NULL, IDC_WAIT ));

    for( i = 0; i < NUM_FORMATCODES; i++ )
        aOutputFormats[i].fEnabled = FALSE;

    ZeroMemory( &wfx, sizeof(wfx));
    wfx.wFormatTag = WAVE_FORMAT_PCM;
 
    for( i = 0; i < NUM_FORMATCODES; i++ )
        {
        FormatCodeToWFX( aInputFormats[i].dwCode, &wfx );

        if(( mmrval = waveInOpen( &ghWaveIn, gpwddIn->uID, &wfx,
                                    (DWORD)WaveInCallback,
                                    0, CALLBACK_FUNCTION )) != MMSYSERR_NOERROR )
            {
            DPF( 5, "Failed with waveInOpen() for %u format", aInputFormats[i].dwCode );
            aInputFormats[i].fEnabled = FALSE;
            }
        else
            {
            DPF( 5, "Succeeded with waveInOpen() for %u format", aInputFormats[i].dwCode );
            aInputFormats[i].fEnabled = TRUE;
            waveInClose( ghWaveIn );
            }
        }

    SetCursor( hCursor );
    }


//////////////////////////////////////////////////////////////////////////////
//
//
//
//
void CloseDSDevice( void )
    {
    if( NULL != gpdsbOutput )
        {
        gpdsbOutput->Release();
        gpdsbOutput = NULL;
        }
    if( NULL != gpdsbPrimary )
        {
        gpdsbPrimary->Release();
        gpdsbPrimary = NULL;
        }
    if( NULL != gpds )
        {
        gpds->Release();
        gpds = NULL;
        }
    }


//////////////////////////////////////////////////////////////////////////////
//
//
//
//
void CloseWaveDevice( void )
    {
    if( NULL == ghWaveIn )
        return;

    waveInReset( ghWaveIn );
    EnterCriticalSection( &gcsBufferData );
    if( gnBuffersOut )
        WaitForSingleObject( ghCallbackFinished, INFINITE );
    LeaveCriticalSection( &gcsBufferData );
    waveInClose( ghWaveIn );
    ghWaveIn = NULL;
    }


//////////////////////////////////////////////////////////////////////////////
//
//
//
//
BOOL InitBuffers( void )
    {
    HRESULT         dsrval;
    int             i;

    if( gfBuffersInitialized )
        return TRUE;
    if( NULL == ghWaveIn )
        return FALSE;

    EnterCriticalSection( &gcsBufferData );
    if( NULL == gawhHeaders )
        {
        if(( gawhHeaders = new WAVEHDR[NUM_BUFFERS] ) == NULL )
            goto Abort_InitBuffers;
        ASSERT( NULL == gpbBufferData );
        }
    if( NULL == gpbBufferData )
        {
        gcbBufferSize = max( 4096, gpwfxInput->nAvgBytesPerSec / 8 );
        gcbBufferSize -= gcbBufferSize % gpwfxInput->nBlockAlign;
        DPF( 1, "Set input buffer size to %lu bytes", gcbBufferSize );

        if(( gpbBufferData = new BYTE[NUM_BUFFERS*gcbBufferSize] ) == NULL )
            goto Abort_InitBuffers;
        }

    ZeroMemory( gawhHeaders, sizeof(WAVEHDR)*NUM_BUFFERS );
    for( i = 0; i < NUM_BUFFERS; i++ )
        {
        gawhHeaders[i].dwBufferLength = gcbBufferSize;
        gawhHeaders[i].lpData = (LPSTR)gpbBufferData + i*gcbBufferSize;
        gawhHeaders[i].dwUser = i;
        if( MMSYSERR_NOERROR != waveInPrepareHeader( ghWaveIn,
                                            &gawhHeaders[i], sizeof(WAVEHDR)))
            goto Abort_InitBuffers;
        if( MMSYSERR_NOERROR != waveInAddBuffer( ghWaveIn,
                                            &gawhHeaders[i], sizeof(WAVEHDR)))
            goto Abort_InitBuffers;
        gnBuffersOut++;
        }

    ZeroMemory( &gdsbdOutput, sizeof(gdsbdOutput));
    gdsbdOutput.dwSize = sizeof(gdsbdOutput);
    gdsbdOutput.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS;
    gdsbdOutput.dwBufferBytes = NUM_BUFFERS * gcbBufferSize / 2;
    // We want the format of this secondary to match the format of the input
    // data.  This lets us blindly copy the data out and DSound worries about
    // matching the bits and channels to our desired primary buffer format.
    gdsbdOutput.lpwfxFormat = gpwfxInput;

    if( FAILED( dsrval = gpds->CreateSoundBuffer( &gdsbdOutput, &gpdsbOutput, NULL )))
        {
        DPF( 0, "Unable to create sound buffer for output (%s)", TranslateDSError(dsrval));
        goto Abort_InitBuffers;
        }

    InterlockedExchange( &gfBuffersInitialized, TRUE );
    LeaveCriticalSection( &gcsBufferData );
    return TRUE;

Abort_InitBuffers:
    // TODO: If there were any buffers put out, we must Reset to get them back
    ASSERT( DestroyBuffers());
    LeaveCriticalSection( &gcsBufferData );
    return FALSE;
    }


//////////////////////////////////////////////////////////////////////////////
//
//
//
//
BOOL StartBuffers( void )
    {
    if( NULL == ghWaveIn || NULL == gpdsbOutput )
        return FALSE;

    waveInStart( ghWaveIn );
    
    // Rewind the output buffer, fill it with silence, and play it
    gpdsbOutput->SetCurrentPosition( 0 );
    WriteSilenceToOutput( 0, gdsbdOutput.dwBufferBytes );
    gpdsbOutput->Play( 0, 0, DSBPLAY_LOOPING );
    return TRUE;
    }


//////////////////////////////////////////////////////////////////////////////
//
//
//
//
BOOL StopBuffers( void )
    {
    if( NULL == ghWaveIn || NULL == gpdsbOutput )
        return FALSE;

    waveInStop( ghWaveIn );
    gpdsbOutput->Stop();
    return TRUE;
    }


//////////////////////////////////////////////////////////////////////////////
//
//
//
//
BOOL DestroyBuffers( void )
    {
    if( NULL != ghWaveIn )
        return FALSE;

    if( NULL != gawhHeaders )
        {
        delete[] gawhHeaders;
        gawhHeaders = NULL;
        }
    if( NULL != gpbBufferData )
        {
        delete[] gpbBufferData;
        gpbBufferData = NULL;
        }
    if( NULL != gpdsbOutput )
        {
        gpdsbOutput->Release();
        gpdsbOutput = NULL;
        }
    InterlockedExchange( &gfBuffersInitialized, FALSE );
    return TRUE;
    }


//////////////////////////////////////////////////////////////////////////////
//
//
//
//
BOOL WriteSilenceToOutput( DWORD dwStart, DWORD cbLength )
    {
    PBYTE   pb1, pb2;
    DWORD   cb1, cb2;

    if(( !dwStart && !cbLength ) || NULL == gpdsbOutput
                        || NULL == gdsbdOutput.lpwfxFormat )
        return FALSE;

    if( SUCCEEDED( gpdsbOutput->Lock( dwStart, cbLength, &pb1, &cb1, &pb2, &cb2, 0 )))
        {
        FillMemory( pb1, cb1, (gdsbdOutput.lpwfxFormat->wBitsPerSample == 8) ? 128 : 0 );
        if( NULL != pb2 && cb2 )
            FillMemory( pb2, cb2, (gdsbdOutput.lpwfxFormat->wBitsPerSample == 8) ? 128 : 0 );

        gpdsbOutput->Unlock( pb1, cb1, pb2, cb2 );
        return TRUE;
        }

    return FALSE;
    }


//////////////////////////////////////////////////////////////////////////////
//
//
//
//
void CALLBACK WaveInCallback( HWAVEIN hwi, UINT uMsg, DWORD dwInstance,
                                            DWORD dwParam1, DWORD dwParam2 )
    {
    static      DWORD dwWritePosition = 0xFFFFFFFF;
    PWAVEHDR    pwhBuffer;
    HRESULT     dsrval;

    switch( uMsg )
        {
        case WIM_DATA:
            pwhBuffer = (PWAVEHDR)dwParam1;
            // If there's no data in the buffer, we must be getting it back
            // so we can shutdown
            EnterCriticalSection( &gcsBufferData );

            if( pwhBuffer->dwBytesRecorded == 0 )
                {
                waveInUnprepareHeader( ghWaveIn, pwhBuffer, sizeof(WAVEHDR));
                // Don't delete the buffer space or the WAVEHDR, because we
                // have allocated them all in an array and will delete them
                // when all buffers are returned and the device is closed
                if( !--gnBuffersOut )
                    {
                    SetEvent( ghCallbackFinished );
                    dwWritePosition = 0xFFFFFFFF;
                    }
                }
            else
                {
                PBYTE   pb1, pb2;
                DWORD   cb1, cb2;
                BOOL    fRestoredBuffer = FALSE;

                // TODO: Send the data to the filter

                // TODO: Copy the data into the DirectSound buffer
                if( NULL != gpdsbOutput )
                    {
                    DWORD dwStatus;
                    if( FAILED( dsrval = gpdsbOutput->GetStatus( &dwStatus )))
                        {
                        DPF( 0, "Unable to get buffer status. (%s)", TranslateDSError(dsrval));
                        goto WIC_SkipDataProcessing;
                        }
                    if( dwStatus & DSBSTATUS_BUFFERLOST )
                        {
                        if( FAILED( dsrval = gpdsbOutput->Restore()))
                            {
                            DPF( 0, "Couldn't Restore output buffer. (%s)", TranslateDSError(dsrval));
                            goto WIC_SkipDataProcessing;
                            }
                        dwWritePosition = 0;
                        fRestoredBuffer = TRUE;
                        }
                    else
                        {
                        if( dwWritePosition == 0xFFFFFFFF )
                            {
                            DWORD       dwPlay, dwWrite;
                        
                            gpdsbOutput->GetCurrentPosition( &dwPlay, &dwWrite );
                            // For our safety, put the write position twice as far ahead as
                            // what DirectSound says is safe
//                          dwWritePosition = 3 * dwWrite - dwPlay;
//                          dwWritePosition = dwWrite + gcbBufferSize;
                            dwWritePosition = gdsbdOutput.dwBufferBytes + dwPlay - gcbBufferSize;
                            while( dwWritePosition >= gdsbdOutput.dwBufferBytes )
                                dwWritePosition -= gdsbdOutput.dwBufferBytes;
                            }
                        }

                    if( pwhBuffer->dwBytesRecorded > gcbBufferSize )
                        DPF( 1, "Unexpectedly large buffer -- %lu bytes (gcbBufferSize = %lu)",
                             pwhBuffer->dwBytesRecorded, gcbBufferSize );

                    if( SUCCEEDED( gpdsbOutput->Lock( dwWritePosition,
                                                        pwhBuffer->dwBytesRecorded,
                                                        &pb1, &cb1, &pb2, &cb2, 0 )))
                        {
                        if( NULL != pb1 && NULL != gpFilter )
                            {
                            gpFilter->Transform( (PBYTE)pwhBuffer->lpData, cb1, pb1 );
                            }
                        if( cb2 && NULL != pb2 && NULL != gpFilter )
                            {
                            gpFilter->Transform( (PBYTE)pwhBuffer->lpData + cb1,
                                        pwhBuffer->dwBytesRecorded - cb1, pb2 );
                            }
                        gpdsbOutput->Unlock( pb1, cb1, pb2,
                                            pwhBuffer->dwBytesRecorded - cb1 );
                        dwWritePosition += pwhBuffer->dwBytesRecorded;
                        while( dwWritePosition >= gdsbdOutput.dwBufferBytes )
                            dwWritePosition -= gdsbdOutput.dwBufferBytes;
                        }
                    if( fRestoredBuffer )
                        {
                        gpdsbOutput->SetCurrentPosition( 0 );
                        if( FAILED( dsrval = gpdsbOutput->Play( 0, 0, DSBPLAY_LOOPING )))
                            DPF( 0, "Unable to restart restored buffer. (%s)", TranslateDSError(dsrval));
                        dwWritePosition = 0xFFFFFFFF;
                        }
                    }
WIC_SkipDataProcessing:
                // Spit the buffer back out
                pwhBuffer->dwBytesRecorded = 0;
                waveInAddBuffer( ghWaveIn, pwhBuffer, sizeof(WAVEHDR));
                }
            LeaveCriticalSection( &gcsBufferData );

            break;
        }
    }



