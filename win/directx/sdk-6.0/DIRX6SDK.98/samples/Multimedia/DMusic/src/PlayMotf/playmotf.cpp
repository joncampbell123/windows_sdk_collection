//-----------------------------------------------------------------------------
// File: WinMain.cpp
//
// Desc: Plays a Primary Segment and a Motif using DirectMusic
//
//
// Copyright (c) 1997 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define STRICT
#include <objbase.h>
#include <initguid.h>
#include <conio.h>
#include <direct.h>
#include <dmusicc.h>
#include <dmusici.h>
#include "PlayMotf.h"

// Both Borland CBuilder3 and Watcom C++ 11 have "chdir" defined incorrectly
// So the code below changes chdir only for MSVC++.
#if	(defined(_MSC_VER))
#define chdir   _chdir
#endif

//-----------------------------------------------------------------------------
// Global variables for the DirectMusic sample 
//-----------------------------------------------------------------------------
IDirectMusicLoader*      g_pLoader      = NULL;
IDirectMusicPerformance* g_pPerformance = NULL;
IDirectMusicSegment*     g_pSegment     = NULL;
IDirectMusicStyle*       g_pStyle       = NULL;
WCHAR                    g_awszMotifName[9][MAX_PATH];




//-----------------------------------------------------------------------------
// Function: InitDirectMusic
//
// Description: 
//      Initilizes DirectMusic
//
//-----------------------------------------------------------------------------
HRESULT InitDirectMusic( LPSTR lpCmdLine )
{
    HRESULT hr;
    GUID    guid;
    BOOL    bUseCurrentWorkingDir = FALSE;
    DWORD   dwIndex;

    if( lpCmdLine[0] != 0  )  // if there are command line params
    {
        if( chdir( lpCmdLine ) != 0 )
        {
            MessageBox( NULL, 
                "Failed to find directory.", 
                "Error", 
                MB_ICONERROR | MB_OK );
            return E_FAIL;
        }
        bUseCurrentWorkingDir = TRUE;
    }

    // Initialize COM
    hr = CoInitialize(NULL);
    if ( FAILED(hr) )
        return hr;

    // Create loader object
    hr = CoCreateInstance(
            CLSID_DirectMusicLoader,
            NULL,
            CLSCTX_INPROC, 
            IID_IDirectMusicLoader,
            (void**)&g_pLoader
        );
    if ( FAILED(hr) )
        return hr;

    // Create performance object
    hr = CoCreateInstance(
            CLSID_DirectMusicPerformance,
            NULL,
            CLSCTX_INPROC, 
            IID_IDirectMusicPerformance,
            (void**)&g_pPerformance
        );
    if ( FAILED(hr) )
        return hr;

    // Initialize the software synthesizer
    hr = InitializeSynth();
    if ( FAILED(hr) )
        return hr;

    // Load the segment
    hr = LoadSegment( bUseCurrentWorkingDir );
    if ( FAILED(hr) )
        return hr;

    // set the segment to repeat many times
    g_pSegment->SetRepeats(200);

    // Get the Style from the Segment by calling the Segment's GetData()
    // with the data type GUID_StyleTrackStyle.
    // 0xffffffff indicates to look at Tracks in all TrackGroups in the segment.
    // The first 0 indicates to retrieve the Style from the first Track 
    // in the indicated TrackGroup.
    // The second 0 indicates to retrieve the Style from the beginning
    // of the Segment, i.e. time 0 in Segment time.
    // If this Segment was loaded from a Section file, there is only one 
    // Style and it is at time 0.
    // Note that the GetData() call with GUID_StyleTrackStyle assumes the
    // third parameter is the address of a pointer to an IDirectMusicStyle.
    guid = GUID_StyleTrackStyle;
    hr = g_pSegment->GetParam( guid, 0xffffffff, 0, 0, NULL, (void*)&g_pStyle );
    if ( FAILED(hr) )
        return hr;

    // Play the segment and wait. The DMUS_SEGF_BEAT indicates to play on the next beat 
    // if there is a segment currently playing. The first 0 indicates to
    // play (on the next beat from) now.
    // The final NULL means do not return an IDirectMusicSegmentState* in
    // the last parameter.
    g_pPerformance->PlaySegment( g_pSegment, DMUS_SEGF_BEAT, 0, NULL );

    // Get the names of the Motifs from the Style. Styles may have
    // any number of Motifs, but for simplicity's sake only get
    // a maximum of 9 here.
    for( dwIndex = 0; dwIndex < 9; dwIndex++ )
    {
        if( S_OK != g_pStyle->EnumMotif( dwIndex, g_awszMotifName[dwIndex] ))
        {
            g_awszMotifName[dwIndex][0] = 0;
            break;
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Function: PlayMotif
//
// Description: 
//      Play the named Motif from the Style in the Performance
//
//-----------------------------------------------------------------------------
HRESULT PlayMotif( WCHAR* pwszMotifName )
{
    IDirectMusicSegment* pSeg;
    HRESULT              hr;

    // Get the Motif Segment from the Style, setting it to play once
    // through (no repeats.) Check for S_OK specifically, because
    // GetMotif() returns S_FALSE if it doesn't find the Motif.
    hr = g_pStyle->GetMotif( pwszMotifName, &pSeg );

    if( S_OK == hr )
    {
        // Play the segment. The PSF_BEAT indicates to play on the next beat 
        // if there is a segment currently playing. PSF_SECONDARY means to
        // play the segment as a secondary segment, which plays on top of
        // the currently playing primary segment. The first 0 indicates to
        // play (on the next beat from) now.
        // The final NULL means do not return an IDirectMusicSegmentState* in
        // the last parameter.
        g_pPerformance->PlaySegment( pSeg, 
                                     DMUS_SEGF_BEAT | DMUS_SEGF_SECONDARY, 
                                     0, 
                                     NULL );
        pSeg->Release();
    }

    return hr;
}




//-----------------------------------------------------------------------------
// Function: InitializeSynth
//
// Description: 
//      Initialize the software synthesizer into the performance.
//      This function also calls IDirectMusicPerformance::Init to
//      initialize the performance and create the DirectMusic object.
//
//-----------------------------------------------------------------------------
HRESULT InitializeSynth()
{
    HRESULT           hr;
    IDirectMusic*     pDM;
    IDirectMusicPort* pPort = NULL;
    DMUS_PORTPARAMS   dmos;
    GUID              guidSink;

    // Initialize the performance. Have the performance create the
    // DirectMusic object by setting pDM to NULL. It is needed to
    // create the port.
    pDM = NULL;
    hr = g_pPerformance->Init( &pDM );
    if ( FAILED(hr) )
        return hr;
    
    // Create the software synth port.
    // An alternate, easier method is to call
    // pPerf->AddPort(NULL), which automatically
    // creates the synth port, adds it to the
    // performance, and assigns PChannels.
    ZeroMemory( &dmos, sizeof(DMUS_PORTPARAMS) );
    dmos.dwSize = sizeof(DMUS_PORTPARAMS);  
    dmos.dwChannelGroups = 1; // create 1 channel groups on the port
    dmos.dwValidParams = DMUS_PORTPARAMS_CHANNELGROUPS; 

    ZeroMemory( &guidSink, sizeof(GUID) );

    hr = pDM->CreatePort( CLSID_DirectMusicSynth, guidSink, &dmos, &pPort, NULL );
    if ( FAILED(hr) )
        return hr;

    // Succeeded in creating the port. Add the port to the
    // Performance with five groups of 16 midi channels.
    hr = g_pPerformance->AddPort( pPort );
    if ( FAILED(hr) )
        return hr;

    // Assign a block of 16 PChannels to this port.
    // Block 0, port pPort, and group 1 means to assign
    // PChannels 0-15 to group 1 on port pPort.
    // PChannels 0-15 correspond to the standard 16
    // MIDI channels.
    g_pPerformance->AssignPChannelBlock( 0, pPort, 1 );

    // Release the port since the performance now has its own reference.
    pPort->Release();

    // release the DirectMusic object. The performance has its
    // own reference and we just needed it to call CreatePort.
    pDM->Release();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Function: LoadSegment
//
// Description: 
//      Change the loader's current search directory and load the
//      Sample.sgt segment. The segment internally references the
//      Sample.sty style and Sample.dls downloadable sample file. 
//      When the loader loads the segment, it also loads the internally 
//      referenced files from the search directory.
//      If fUseCWD is TRUE, use the current working directory. Otherwise,
//      use the path referenced by the DirectMusic/Media registry key.
//
//-----------------------------------------------------------------------------
HRESULT LoadSegment( BOOL fUseCWD )
{
    HRESULT             hr;
    DMUS_OBJECTDESC     ObjDesc; // Object descriptor for pLoader->GetObject()
    WCHAR               wszDir[_MAX_PATH];

    // Change the loader's current search directory to the
    // application's current working directory or the DirectMusic
    // registry key.
    if( fUseCWD )
    {
        char szDir[_MAX_PATH];
        // Change the loader's current search directory to the
        // application's current working directory.
#ifdef __BORLANDC__
        if( getcwd( szDir, _MAX_PATH ) == NULL )
#else
        if( _getcwd( szDir, _MAX_PATH ) == NULL )
#endif
        {
            // there was an error. Return E_FAIL.
            return E_FAIL;
        }
        MULTI_TO_WIDE( wszDir, szDir );
    }
    else if(!GetSearchPath(wszDir))
    {
        // there was an error. Return E_FAIL.
        return E_FAIL;
    }

    g_pLoader->SetSearchDirectory( GUID_DirectMusicAllTypes, wszDir, FALSE );

    // Now load the segment file.
    // Sections load as type Segment, as do MIDI files, for example.
    ObjDesc.guidClass = CLSID_DirectMusicSegment;
    ObjDesc.dwSize = sizeof(DMUS_OBJECTDESC);
    wcscpy( ObjDesc.wszFileName, L"Sample.sgt" );
    ObjDesc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME;

    hr = g_pLoader->GetObject( &ObjDesc, IID_IDirectMusicSegment, (void**)&g_pSegment );

    return hr;
}




//-----------------------------------------------------------------------------
// Function: FreeDirectMusic
//
// Description: 
//      Releases DirectMusic
//
//-----------------------------------------------------------------------------
HRESULT FreeDirectMusic()
{
    // Release the Segment
    g_pSegment->Release();

    // If there is any music playing, stop it.
    g_pPerformance->Stop( NULL, NULL, 0, 0 );

    // CloseDown and Release the performance object
    g_pPerformance->CloseDown();
    g_pPerformance->Release();

    // Release the loader object
    g_pLoader->Release();

    // Release COM
    CoUninitialize();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Function: GetSearchPath
//
// Description: 
//      Finds and returns the DirectX SDK search path from the registery
//
//-----------------------------------------------------------------------------
BOOL GetSearchPath(WCHAR wszPath[MAX_PATH])
{
    HKEY    hkDirectX;
    BOOL    bRet = FALSE;
    char    szPath[MAX_PATH];
    DWORD   cbPath;


    // Get DirectX SDK search path from the registry
    //
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                     "Software\\Microsoft\\DirectX",
                     0,                         // Reserved
                     KEY_READ,
                     &hkDirectX))
    {
        return FALSE;
    }

    cbPath = sizeof(szPath);
    if (RegQueryValueEx(hkDirectX,
                        "DX6SDK Samples Path",
                        NULL,                   // Reserved
                        NULL,                   // Type: don't care
                        (LPBYTE)szPath,
                        &cbPath) == ERROR_SUCCESS)
    {
        if (cbPath + sizeof(szDirectMusicMedia) > MAX_PATH)
        {
            return FALSE;
        }

        strcat(szPath, szDirectMusicMedia);

        // DirectMusic requires the search path as a wide string
        //
        mbstowcs(wszPath, 
                 szPath,
                 MAX_PATH);
        bRet = TRUE;
    }

    RegCloseKey(hkDirectX);
    return bRet;
}

