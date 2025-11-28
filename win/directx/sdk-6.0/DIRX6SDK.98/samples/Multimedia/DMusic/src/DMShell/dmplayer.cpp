// CDMPlayer.cpp : implementation file
//

#include <objbase.h>
#include <initguid.h>
#include <direct.h>
#include <dmusicc.h>
#include <dmusici.h>
#include "main.h"

static BOOL GetSearchPath(WCHAR wszPath[MAX_PATH]);

/////////////////////////////////////////////////////////////////////////////
// Members

CDMPlayer::CDMPlayer(HWND hWnd)
{
	m_hWnd = hWnd;
	m_bIsPlaying = FALSE;
	m_bIsInitialized = FALSE;
	m_pComposer = NULL;
	m_pLoader = NULL;
	m_pDirectMusic = NULL;
	m_pPerformance = NULL;
	m_pPort = NULL;

	NullMediaPointers();
	
	//Initialize COM
	if ( FAILED( CoInitialize(NULL) ))
		return;

	// Create the loader
	if ( FAILED(CoCreateInstance(CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC, 
		IID_IDirectMusicLoader, (void**)&m_pLoader)))
		return;

	//Set the search directory
	WCHAR wzSearchDir[MAX_PATH];
    if ( !GetSearchPath(wzSearchDir) )
        return;

	m_pLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, wzSearchDir, FALSE );

	// Create the performance object
	if (FAILED(CoCreateInstance(CLSID_DirectMusicPerformance, NULL,
		CLSCTX_INPROC, IID_IDirectMusicPerformance, (void**)&m_pPerformance)))
		return;

	// Initialize the composer
	if (FAILED(::CoCreateInstance(CLSID_DirectMusicComposer, NULL, CLSCTX_INPROC, 
		IID_IDirectMusicComposer, (void**)&m_pComposer)))
		return;

	// Initialize the software synthesizer
	if( InitializeSynth() == FALSE )
		return;

	m_bIsInitialized = TRUE;
	
	// Load the necessary scheme media
	if( !LoadScheme(0))
	{
        MessageBox(m_hWnd, "Unable to load the media for the default scheme.",
			"DirectMusic Shell: Initialization Error", MB_OK | MB_ICONEXCLAMATION );
		return;
	}

	// Start Playing
	m_pDirectMusic->Activate(m_hWnd,TRUE);
	PlaySection(m_hWnd);
	m_bIsPlaying = TRUE;
}

CDMPlayer::~CDMPlayer()
{
	ReleaseMediaPointers();

	if (m_pPerformance)
		m_pPerformance->Stop( NULL, NULL, 0, 0 );

	if (m_pLoader)
		m_pLoader->ClearCache( GUID_DirectMusicAllTypes );

	if (m_pLoader)
		m_pLoader->Release();

	if (m_pPort)
	{
		if (m_pPerformance)
			m_pPerformance->RemovePort(m_pPort);
		m_pPort->Release();
	}
	if (m_pDirectMusic)
		m_pDirectMusic->Release();
	
	if (m_pComposer)
		m_pComposer->Release();

	if (m_pPerformance)
		m_pPerformance->Release();

	CoUninitialize();
}

// PlayEvent is the point of input for all of the shell events.
// Most events just play motifs. The application open/close/switch events
// trigger new primary segments.
void CDMPlayer::PlayEvent(WPARAM wEventType, LPARAM lEventData)
{
	if (!m_bIsPlaying)
		return;

	if (!m_bIsInitialized)
		return;

	// Deal with the events that switch between primary segments
	switch (wEventType)
	{
		case DME_APPOPEN:
			PlaySection( HWND(lEventData) );
			PlayMotif(DME_APPOPEN);
			return;

		case DME_APPSWITCH:
			PlaySection( HWND(lEventData) );
			return;

		case DME_APPCLOSE:
			m_TemplateMapper.DeleteWin( HWND(lEventData) );
			PlayMotif(DME_WINDOWCLOSE);
			return;
	}

	// If it got this far, play the motif corresponding to the event.
	PlayMotif(wEventType);
}


// Initialize the software synthesizer into the performance.
// This function also calls IDirectMusicPerformance::Init to
// initialize the performance and create the DirectMusic object.
BOOL CDMPlayer::InitializeSynth()
{
	// Initialize the performance. Have the performance create the
	// DirectMusic object by setting pDM to NULL. It is needed to
	// create the port.
	if( FAILED( m_pPerformance->Init( &m_pDirectMusic )))
		return FALSE;

	DMUS_PORTPARAMS dmos;
	ZeroMemory(&dmos, sizeof(DMUS_PORTPARAMS));
	// create 1 channel group on the port
	dmos.dwChannelGroups = 1;
	// set the dwChannelGroups field as valid
	dmos.dwValidParams = DMUS_PORTPARAMS_CHANNELGROUPS;
	dmos.dwSize = sizeof(DMUS_PORTPARAMS);

	m_pDirectMusic->Activate(NULL, FALSE);

	if ( FAILED(m_pDirectMusic->CreatePort(CLSID_DirectMusicSynth,
		CLSID_DirectMusicSynthSink, &dmos, &m_pPort, NULL)))
	{
		return FALSE;
	}
	if( SUCCEEDED( m_pPerformance->AddPort( m_pPort)))
	{
		m_pPerformance->AssignPChannelBlock( 0, m_pPort, 1 );
		return TRUE;
	}
	return FALSE;
}

BOOL CDMPlayer::SelectOutputPort(WORD wPortIndex)
{
	if (!m_bIsInitialized)
		return FALSE;

	// Stop the performance and release the current port
	BOOL bStatus = m_bIsPlaying;
	m_bIsPlaying = FALSE;
	if (m_pPort && m_pPerformance)
	{
		m_pPerformance->Invalidate(0, 0);
		Stop();
		m_pPerformance->RemovePort(m_pPort);
	}
	if (m_pPort)
	{
		m_pPort->Release();
		m_pPort = NULL;
	}

	// Enumerate the new port to get its GUID
	HRESULT hr;
	DMUS_PORTCAPS dmpc;
	GUID guidSynthGUID;
	dmpc.dwSize = sizeof(DMUS_PORTCAPS);
	
	hr = m_pDirectMusic->EnumPort(wPortIndex, &dmpc);

	// Open the new port
	if(SUCCEEDED(hr) && hr != S_FALSE)
	{
		CopyMemory(&guidSynthGUID, &dmpc.guidPort, sizeof(GUID));
		
		DMUS_PORTPARAMS dmos;
		ZeroMemory(&dmos, sizeof(dmos));
		// create 1 channel group on the port
		dmos.dwChannelGroups = 1;
		// set the dwChannelGroups field as valid
		dmos.dwValidParams = DMUS_PORTPARAMS_CHANNELGROUPS;
		dmos.dwSize = sizeof(DMUS_PORTPARAMS);

		hr = m_pDirectMusic->CreatePort(guidSynthGUID,
			GUID_NULL, &dmos, &m_pPort, NULL);

		if (FAILED (hr) )
			return FALSE;

		// Add it to the performance
		if( SUCCEEDED( m_pPerformance->AddPort( m_pPort)))
		{
			m_pPerformance->AssignPChannelBlock( 0, m_pPort, 1 );
			if (m_pBand)
			{
				m_pBand->Download(m_pPerformance);
			}
			ResetMidiStream();
			if (bStatus)
				Start();

			return TRUE;
		}
	}
	return FALSE;
}

// Returns VALID_PORT for valid output ports
// Returns INVALID_PORT for input or invalid ports
// Returns SELECTED_PORT for the port that is currently selected
WORD CDMPlayer::EnumOutputPort(WORD wPortIndex, PWSTR pwzPortName, WORD wBufferLength)
{
	if (!m_bIsInitialized)
		return INVALID_PORT;

	HRESULT hr;
	DMUS_PORTCAPS dmpc;
	GUID guidEnumPort;
	dmpc.dwSize = sizeof(DMUS_PORTCAPS);
	
	hr = m_pDirectMusic->EnumPort(wPortIndex, &dmpc);

	if(SUCCEEDED(hr) && hr != S_FALSE)
	{
		if (dmpc.dwClass == DMUS_PC_OUTPUTCLASS)
		{
			//Get string length and account for the ending NULL character
			WORD wStrLength = wcslen(dmpc.wszDescription) + 1;
			if (wStrLength > wBufferLength)
			{
				wStrLength = wBufferLength - 1;			    //leave room for null character
				*(pwzPortName + (wBufferLength - 1) ) = 0;	//change the last character to NULL
															//realizing that there are wSize charaters
															//counting from 0, not 1
			}
			wcsncpy(pwzPortName, dmpc.wszDescription, wStrLength);
			guidEnumPort = dmpc.guidPort;
			if( m_pPort )
			{
				hr = m_pPort->GetCaps(&dmpc);
				if(SUCCEEDED(hr) && hr != S_FALSE)
				{
					if (dmpc.guidPort == guidEnumPort)
						return SELECTED_PORT;
				}
			}
			return VALID_PORT;
		}
		else
			return INVALID_PORT;
	}
	else
		return INVALID_PORT;
}

// This routine loads the style and band
// there is a hard coded assumption that the band name is "Default"
BOOL CDMPlayer::LoadStyle(WCHAR* pwzFileName)
{
	DMUS_OBJECTDESC ObjectDescript;
	ObjectDescript.dwSize = sizeof(DMUS_OBJECTDESC);
	ObjectDescript.guidClass = CLSID_DirectMusicStyle;
	wcscpy(ObjectDescript.wszFileName, pwzFileName );
	ObjectDescript.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME ;

    if (SUCCEEDED(m_pLoader->GetObject( &ObjectDescript, IID_IDirectMusicStyle,
		(void**)&m_pStyle)))
	{
		BSTR bstr = SysAllocString(L"Default");
		m_pStyle->GetBand(bstr, &m_pBand) ;
		SysFreeString(bstr);
		if (m_pBand)
		{
			m_pBand->Download(m_pPerformance);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CDMPlayer::LoadPersonality(WCHAR* pwzFileName)
{
	DMUS_OBJECTDESC ObjectDescript;
	ObjectDescript.dwSize = sizeof(DMUS_OBJECTDESC);
	ObjectDescript.guidClass = CLSID_DirectMusicPersonality;
	wcscpy(ObjectDescript.wszFileName, pwzFileName);
	ObjectDescript.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME;
    if (SUCCEEDED(m_pLoader->GetObject( &ObjectDescript, IID_IDirectMusicPersonality,
		(void**)&m_pPersonality)))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CDMPlayer::LoadTemplate(DWORD dwIndex, WCHAR* pwzName)
{ 	
	if (m_pTemplates[dwIndex])
		m_pTemplates[dwIndex]->Release();

	DMUS_OBJECTDESC ObjectDescript;
	ObjectDescript.dwSize = sizeof(DMUS_OBJECTDESC);
	ObjectDescript.guidClass = CLSID_DirectMusicSegment;
	wcscpy(ObjectDescript.wszFileName, pwzName);
	ObjectDescript.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME;
    if (SUCCEEDED(m_pLoader->GetObject( &ObjectDescript, IID_IDirectMusicSegment,
		(void**)&m_pTemplates[dwIndex])))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CDMPlayer::ComposeSection(DWORD dwIndex)
{ 	
	m_pComposer->ComposeSegmentFromTemplate(
		m_pStyle,
		m_pTemplates[dwIndex],
		1,
		m_pPersonality,
		&m_pPrimarySegments[dwIndex]);
	if (m_pPrimarySegments[dwIndex])
	{
		m_pPrimarySegments[dwIndex]->SetRepeats(999);
		m_pPrimarySegments[dwIndex]->SetParam(
			GUID_BandTrack_Disable_Auto_Download, 
			0xFFFFFFFF, 0, 0, NULL) ;
		return TRUE;
	}
	return FALSE;
}

void CDMPlayer::PlaySection(HWND hWnd)
{
	WORD wTemplateNum = m_TemplateMapper.GetTemplate( hWnd );

	if (wTemplateNum > NUM_TEMPLATES)
		return;

	if (m_pPrimarySegments[wTemplateNum] )
	{
		m_pPerformance->PlaySegment( 
			m_pPrimarySegments[wTemplateNum], DMUS_SEGF_BEAT , 0, NULL);
	}
}

//	Load the dls set that the band needs, but just leave it in the cache.
//	Then, it will be referenced and loaded by the bands in the styles and sections.
BOOL CDMPlayer::LoadDLS(WCHAR* sFileName)
{
	// No need to load GM.DLS. The filename will be blank in this case
	if ( wcscmp(sFileName, L"") == 0 )
		return TRUE;
	
	DMUS_OBJECTDESC ObjectDescript;
	ObjectDescript.dwSize = sizeof(DMUS_OBJECTDESC);
	ObjectDescript.guidClass = CLSID_DirectMusicCollection;
	wcscpy(ObjectDescript.wszFileName, sFileName);
	ObjectDescript.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME ;
	IDirectMusicObject* pObject = NULL;
	if (SUCCEEDED(m_pLoader->GetObject(&ObjectDescript, IID_IDirectMusicObject,
        (void**)&pObject)))
	{
		pObject->Release();
		return TRUE;
	}
	return FALSE;
}

void CDMPlayer::PlayMotif(WORD wMotif)
{
	if (wMotif >= DME_MAXEVENTNUM)
		return;

	// Throw away redundant events that occur in the same beat.
	// The purpose is to prevent more than one motif of any given
	// type from triggering in the same beat.
	MUSIC_TIME mtNow, mtNowBeat;
	m_pPerformance->GetTime(NULL, &mtNow);
	mtNowBeat = mtNow/DMUS_PPQ;
	if (mtNowBeat == m_mtPreviousEventBeatNumber[wMotif])
		return;
	m_mtPreviousEventBeatNumber[wMotif] = mtNowBeat;

	//Play the motif
	if (m_pMotifs[wMotif] && m_pPerformance)
	{
		m_pPerformance->PlaySegment(m_pMotifs[wMotif],
			(DMUS_SEGF_SECONDARY | DMUS_SEGF_BEAT), 0, NULL);
	}
}

BOOL CDMPlayer::LoadScheme(WORD wSchIndx)
{
	if (!LoadDLS(Schemes[wSchIndx].DLSName) )
		return FALSE;
	if (!LoadStyle(Schemes[wSchIndx].StyleName) )
		return FALSE;
	if (!LoadPersonality(Schemes[wSchIndx].PersonalityName) )
		return FALSE;

	WORD x;
	
	for ( x = 0; x < NUM_TEMPLATES ; x++)
	{
		if (wcscmp(Schemes[wSchIndx].TemplateNames[x], L"") != 0)
			if ( LoadTemplate( x, Schemes[wSchIndx].TemplateNames[x] ) )
				ComposeSection( x );
	}

	for ( x = 0; x < DME_MAXEVENTNUM ; x++)
	{
		if ( Schemes[wSchIndx].MotifNames[x] )
			if (wcscmp(Schemes[wSchIndx].MotifNames[x], L"") != 0)
				m_pStyle->GetMotif(Schemes[wSchIndx].MotifNames[x], &m_pMotifs[x]);
	}

	return TRUE;
}

BOOL CDMPlayer::SelectNewScheme(WORD wSchemeNum)
{
	if (!m_bIsInitialized)
		return FALSE;

	ReleaseMediaPointers();
	NullMediaPointers();
	BOOL bReturn = LoadScheme(wSchemeNum);

	ResetMidiStream();
	
	if (m_bIsPlaying)
		PlaySection(m_hWnd);

	return bReturn;
}

BOOL CDMPlayer::GetSchemeName(WORD wSchemeNum, PWSTR pwzSchemeName, WORD wBufferLength)
{
	if (wSchemeNum >= NUM_SCHEMES)
		return FALSE;
	WORD wStrLength = wcslen(Schemes[wSchemeNum].FriendlySchemeName) + 1; //grab null char also
	if (wStrLength > wBufferLength)
	{
		wStrLength = wBufferLength - 1;					//leave room for null character
		*(pwzSchemeName + (wBufferLength - 1) ) = 0;	//change the last character to NULL
														//realizing that there are wSize charaters
														//counting from 0, not 1
	}
	wcsncpy(pwzSchemeName, Schemes[wSchemeNum].FriendlySchemeName, wStrLength);
	return TRUE;
}

void CDMPlayer::Start()
{
	if (!m_bIsInitialized)
		return;

	PlaySection(m_hWnd);
	m_bIsPlaying = TRUE;
}

void CDMPlayer::Stop()
{
	if (m_pPerformance)
		m_pPerformance->Stop( NULL, NULL, 0, 0 );

	ResetMidiStream();
	m_bIsPlaying = FALSE;
}

BOOL CDMPlayer::IsPlaying()
{
	return m_bIsPlaying;
}

void CDMPlayer::NullMediaPointers()
{
	for (int x = 0; x < NUM_TEMPLATES; x++)
		m_pPrimarySegments[x] = NULL;

	m_pPersonality = NULL;

	m_pStyle = NULL;

	for (x = 0; x < DME_MAXEVENTNUM; x++)
		m_pMotifs[x] = NULL;

	for (x = 0; x < NUM_TEMPLATES; x++)
		m_pTemplates[x] = NULL;

	m_pBand = NULL;

	for (x = 0; x < DME_MAXEVENTRANGE; x++)
		m_mtPreviousEventBeatNumber[x] = 0;

}

void CDMPlayer::ReleaseMediaPointers()
{
	for (int x = 0; x < DME_MAXEVENTNUM; x++)
	{
		if (m_pMotifs[x])
			m_pMotifs[x]->Release();
	}
	
	if (m_pPersonality)
		m_pPersonality->Release();
	
	for (x = 0; x < NUM_TEMPLATES; x++)
	{
		if (m_pPrimarySegments[x])
			m_pPrimarySegments[x]->Release();
	}
	
	if (m_pStyle)
		m_pStyle->Release();
	
	for (x = 0; x < NUM_TEMPLATES; x++)
	{
		if (m_pTemplates[x])
			m_pTemplates[x]->Release();
	}

	if (m_pBand)
	{
		m_pBand->Unload(m_pPerformance);
		m_pBand->Release();
	}
}

void CDMPlayer::ResetMidiStream()
{
	if (!m_pPerformance)
		return;

	DMUS_MIDI_PMSG* pMidi;
	DMUS_PMSG* pEvent;
	for (int x = 0; x < 16; x ++ )
	{
		if( SUCCEEDED( m_pPerformance->AllocPMsg( sizeof(DMUS_MIDI_PMSG),
			(DMUS_PMSG**)&pMidi )))
		{
			ZeroMemory( pMidi, sizeof(DMUS_MIDI_PMSG) );
			pMidi->bStatus = 0xb0 | x;
			pMidi->bByte1 = 0x79;			//Reset all controllers
			pMidi->bByte2 = 0x00;
			pMidi->dwType = DMUS_PMSGT_MIDI;
			pEvent = (DMUS_PMSG*)pMidi;
			pEvent->dwFlags = DMUS_PMSGF_REFTIME;
			m_pPerformance->SendPMsg( pEvent );
		}
		if( SUCCEEDED( m_pPerformance->AllocPMsg( sizeof(DMUS_MIDI_PMSG),
			(DMUS_PMSG**)&pMidi )))
		{
			ZeroMemory( pMidi, sizeof(DMUS_MIDI_PMSG) );
			pMidi->bStatus = 0xb0 | x;
			pMidi->bByte1 = 0x7b;			//All notes off
			pMidi->bByte2 = 0x00;
			pMidi->dwType = DMUS_PMSGT_MIDI;
			pEvent = (DMUS_PMSG*)pMidi;
			pEvent->dwFlags = DMUS_PMSGF_REFTIME;
			m_pPerformance->SendPMsg( pEvent );
		}
	}
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

