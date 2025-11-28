// Helper.cpp : Helper functions for DirectMusic
//
// Authored by: Jim Geist and Mark Burton
//
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <direct.h>
#include <dmusici.h>
#include <dmusicf.h>
#include "debug.h"
#include "DMHelper.h"

static BOOL GetSearchPath(WCHAR wszPath[MAX_PATH]);

//
// Create and return the graph object.
// It is the caller's responsibility to release the object.
//
IDirectMusicGraph* CreateGraph(void)
{
	IDirectMusicGraph* pGraph;

	if (FAILED(CoCreateInstance(
			CLSID_DirectMusicGraph,
			NULL,
			CLSCTX_INPROC, 
			IID_IDirectMusicGraph,
			(void**)&pGraph
		)))
	{
		pGraph = NULL;
	}

	return pGraph;
}

//
// Create and return the loader object.
// It is the caller's responsibility to release the object.
//
IDirectMusicLoader* CreateLoader(void)
{
	IDirectMusicLoader* pLoader;

    WCHAR wszPath[MAX_PATH];
    if (!GetSearchPath(wszPath))
    {
        return NULL;
    }

	if (FAILED(CoCreateInstance(
			CLSID_DirectMusicLoader,
			NULL,
			CLSCTX_INPROC, 
			IID_IDirectMusicLoader,
			(void**)&pLoader
		)))
	{
		pLoader = NULL;
	}
	else
	{
		pLoader->SetSearchDirectory( GUID_DirectMusicAllTypes, wszPath, FALSE );
	}

	return pLoader;
}

//
// Create and return the performance object.
// It is the caller's responsibility to release the object.
//
IDirectMusicPerformance* CreatePerformance(void)
{
	IDirectMusicPerformance* pPerf;

	if (FAILED(CoCreateInstance(
			CLSID_DirectMusicPerformance,
			NULL,
			CLSCTX_INPROC, 
			IID_IDirectMusicPerformance,
			(void**)&pPerf
		)))
	{
		pPerf = NULL;
	}

	return pPerf;
}

// Create and return the composer object.
// It is the caller's responsibility to release the object.
IDirectMusicComposer* CreateComposer(void)
{
	IDirectMusicComposer* pComposer;

	if (FAILED(CoCreateInstance(
			CLSID_DirectMusicComposer,
			NULL,
			CLSCTX_INPROC, 
			IID_IDirectMusicComposer,
			(void**)&pComposer
		)))
    {
		pComposer = NULL;
	}
	return pComposer;
}

IDirectMusicSegment* CreateSegmentFromFile(IDirectMusicLoader* pLoader, WCHAR* wszFileName)
{
	DMUS_OBJECTDESC ObjDesc; // Object descriptor for pLoader->GetObject()
	IDirectMusicSegment* pSegment = NULL;

	ObjDesc.guidClass = CLSID_DirectMusicSegment;
	ObjDesc.dwSize = sizeof(DMUS_OBJECTDESC);
	wcscpy( ObjDesc.wszFileName, wszFileName );
	ObjDesc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME;
	pLoader->GetObject( &ObjDesc, IID_IDirectMusicSegment, (void**)&pSegment );
	return pSegment;
}

void RemoveTrackType(IDirectMusicSegment* pSegment, REFGUID rguid)
{
	IDirectMusicTrack* pTrack;
	// remove every track of type rguid from pSegment.
	// In GetTrack(), 0xffffffff indicates to scan all groups in the Segment,
	// and the 0 indicates to get the first track in that group.
	while( SUCCEEDED( pSegment->GetTrack( rguid, 0xffffffff, 0, &pTrack )))
	{
		if( FAILED( pSegment->RemoveTrack( pTrack )))
		{
			// this should never happen
			break;
		}
		pTrack->Release();
	}
}

IDirectMusicSegment* CreateSecSegmentFromFile(IDirectMusicLoader* pLoader, WCHAR* wszFileName)
{
	IDirectMusicSegment* pSegment = CreateSegmentFromFile(pLoader, wszFileName);
	if( pSegment )
	{
		// remove tracks from the Segment, since it is a
		// secondary segment and we don't want it to override 
		// the Primary.
		RemoveTrackType( pSegment, GUID_TempoTrack );
		RemoveTrackType( pSegment, GUID_TimeSigTrack );
		RemoveTrackType( pSegment, GUID_BandTrack );

		// also make sure the repeat count is 0
		pSegment->SetRepeats(0);
	}
	return pSegment;
}

IDirectMusicStyle* GetStyle(IDirectMusicLoader* pLoader, WCHAR* wszStyle)
{
	DMUS_OBJECTDESC ObjDesc; // Object descriptor for pLoader->GetObject()
	IDirectMusicStyle* pStyle = NULL;

	ObjDesc.guidClass = CLSID_DirectMusicStyle;
	ObjDesc.dwSize = sizeof(DMUS_OBJECTDESC);
	wcscpy( ObjDesc.wszFileName, wszStyle );
	ObjDesc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME;
	pLoader->GetObject( &ObjDesc, IID_IDirectMusicStyle, (void**)&pStyle );
	return pStyle;
}

IDirectMusicPersonality* GetPersonality(IDirectMusicLoader* pLoader, WCHAR* wszPersonality)
{
	DMUS_OBJECTDESC ObjDesc; // Object descriptor for pLoader->GetObject()
	IDirectMusicPersonality* pPersonality = NULL;

	ObjDesc.guidClass = CLSID_DirectMusicPersonality;
	ObjDesc.dwSize = sizeof(DMUS_OBJECTDESC);
	wcscpy( ObjDesc.wszFileName, wszPersonality );
	ObjDesc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME;
	pLoader->GetObject( &ObjDesc, IID_IDirectMusicPersonality, (void**)&pPersonality );
	return pPersonality;
}

IDirectMusicSegment* CreateSegmentFromTemplate(IDirectMusicLoader* pLoader, 
											  IDirectMusicSegment* pTemplate,
											  IDirectMusicComposer* pComposer,
											  WCHAR* wszStyle,
											  WCHAR* wszPersonality)
{
	IDirectMusicSegment* pSegment = NULL;
	IDirectMusicStyle* pStyle;
	IDirectMusicPersonality* pPersonality;

	pStyle = GetStyle( pLoader, wszStyle );
	pPersonality = GetPersonality( pLoader, wszPersonality );
	if( pStyle && pPersonality )
	{
		pComposer->ComposeSegmentFromTemplate(
			pStyle,
			pTemplate,
			3, // lowest activity level
			pPersonality,
			&pSegment);
		// The following code will be obsolete once the media files contain
		// this information internally.
		if (pSegment)
		{
			DMUS_TEMPO_PARAM tempo;
			tempo.mtTime = 0;
			tempo.dblTempo = 80;
			pSegment->SetParam( GUID_TempoTrack, 0xFFFFFFFF, 0, tempo.mtTime, &tempo );
			// this isn't the correct generic way to compute time, but
			// works so long as we're in a 4/4 time signature
			tempo.mtTime = DMUS_PPQ * 4 * 2;
			tempo.dblTempo = 100;
			pSegment->SetParam( GUID_TempoTrack, 0xFFFFFFFF, 0, tempo.mtTime, &tempo );
			tempo.mtTime = DMUS_PPQ * 4 * 4;
			tempo.dblTempo = 120;
			pSegment->SetParam( GUID_TempoTrack, 0xFFFFFFFF, 0, tempo.mtTime, &tempo );
			tempo.mtTime = DMUS_PPQ * 4 * 8;
			tempo.dblTempo = 140;
			pSegment->SetParam( GUID_TempoTrack, 0xFFFFFFFF, 0, tempo.mtTime, &tempo );
			tempo.mtTime = DMUS_PPQ * 4 * 14;
			tempo.dblTempo = 160;
			pSegment->SetParam( GUID_TempoTrack, 0xFFFFFFFF, 0, tempo.mtTime, &tempo );
			tempo.mtTime = DMUS_PPQ * 4 * 20;
			tempo.dblTempo = 180;
			pSegment->SetParam( GUID_TempoTrack, 0xFFFFFFFF, 0, tempo.mtTime, &tempo );
			tempo.mtTime = DMUS_PPQ * 4 * 30;
			tempo.dblTempo = 220;
			pSegment->SetParam( GUID_TempoTrack, 0xFFFFFFFF, 0, tempo.mtTime, &tempo );
		}
	}
	RELEASE(pStyle);
	RELEASE(pPersonality);
	return pSegment;
}

IDirectMusicSegment* GetMotif(IDirectMusicLoader* pLoader, 
							 WCHAR* wszStyle,
							 WCHAR* wszMotif)
{
	IDirectMusicStyle* pStyle;
	IDirectMusicSegment* pSegment = NULL;

	pStyle = GetStyle(pLoader, wszStyle);
	if( pStyle )
	{
		pStyle->GetMotif(wszMotif, &pSegment);
		pStyle->Release();
	}
	return pSegment;
}


IDirectMusicBand* CreateBandFromFile(IDirectMusicLoader* pLoader, WCHAR* wszFileName)
{
	DMUS_OBJECTDESC ObjDesc; // Object descriptor for pLoader->GetObject()
	IDirectMusicObject* pObjectSeg = NULL;
	IDirectMusicBand* pBand = NULL;

	ObjDesc.guidClass = CLSID_DirectMusicBand;
	ObjDesc.dwSize = sizeof(DMUS_OBJECTDESC);
	wcscpy( ObjDesc.wszFileName, wszFileName );
	ObjDesc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME;
	pLoader->GetObject( &ObjDesc, IID_IDirectMusicBand, (void**)&pBand );
	return pBand;
}

IDirectMusicObject* CreateDLSObject(IDirectMusicLoader* pLoader, WCHAR *wszFileName)
{
	DMUS_OBJECTDESC ObjDesc; // Object descriptor for pLoader->GetObject()
	IDirectMusicObject* pObjectDLS = NULL;

	ObjDesc.guidClass = CLSID_DirectMusicCollection;
	ObjDesc.dwSize = sizeof(DMUS_OBJECTDESC);
	wcscpy( ObjDesc.wszFileName, wszFileName );
	ObjDesc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME;
	if( SUCCEEDED( pLoader->GetObject( &ObjDesc, IID_IDirectMusicObject, (void**)&pObjectDLS )))
	{
        return pObjectDLS;
	}

	return NULL;
}

// Get registry search path
//
static char szDirectMusicMedia[] = "\\DMusic\\Media";

static BOOL GetSearchPath(WCHAR wszPath[MAX_PATH])
{
	HKEY	hkDirectX;
	BOOL	bRet = FALSE;
	char	szPath[MAX_PATH];
	DWORD	cbPath;


	// Get DirectX SDK search path from the registry
	//
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
					 "Software\\Microsoft\\DirectX",
					 0,							// Reserved
					 KEY_READ,
					 &hkDirectX))
    {
		return FALSE;
	}

	cbPath = sizeof(szPath);
	if (RegQueryValueEx(hkDirectX,
						"DX6SDK Samples Path",
						NULL,					// Reserved
						NULL,					// Type: don't care
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
