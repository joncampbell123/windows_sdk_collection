//
// MLMusic.cpp
//
// Music logic for MusicLines
//
// Authored by: Jim Geist and Mark Burton
//
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <windows.h>
#include "Debug.h"
#include <objbase.h>
#include <dmusici.h>

#include "MLMusic.h"
#include "DMHelper.h"

long gcMoveCount = 0;
DWORD gdwTime = 0;
DWORD gdwIntensity = 0;
#define MAX_INTENSITY 8

Music::Music()
{
	m_pPerf = NULL;
	m_pLoader = NULL;
	for( int x = 0; x < NUM_TEMPLATES; x++ )
	{
		m_apTemplate[x] = NULL;
	}
	for( x = 0; x < NUM_MAINSEGS; x++ )
	{
		m_apMainSeg[x] = NULL;
	}
	m_pIntroSeg = NULL;
	m_pEndSeg = NULL;
	for( x = 0; x < NUM_PLAYERS; x++ )
	{
		m_pUpSeg[x] = NULL;
		m_pDownSeg[x] = NULL;
		m_pLeftSeg[x] = NULL;
		m_pRightSeg[x] = NULL;
		m_pDirSegState[x] = NULL;
	}
	m_pDeathSeg = NULL;
	m_pWinSeg = NULL;
    m_pBand = NULL;
	m_fPlayingIntro = FALSE;
	m_cRef = 1; // set to 1 so one call to Release() will free this
}

Music::~Music()
{
    if ( m_pBand && m_pPerf )
    {
        m_pBand->Unload(m_pPerf);
    }
	RELEASE(m_pBand);

	if( m_pPerf )
	{
		m_pPerf->CloseDown();
	}
	RELEASE(m_pPerf);

	for( int x = 0; x < NUM_TEMPLATES; x++ )
	{
		RELEASE(m_apTemplate[x]);
	}
	for( x = 0; x < NUM_MAINSEGS; x++ )
	{
		RELEASE(m_apMainSeg[x]);
	}
	RELEASE(m_pComposer);
	RELEASE(m_pIntroSeg);
	RELEASE(m_pEndSeg);
	for( x = 0; x < NUM_PLAYERS; x++ )
	{
		RELEASE(m_pUpSeg[x]);
		RELEASE(m_pDownSeg[x]);
		RELEASE(m_pLeftSeg[x]);
		RELEASE(m_pRightSeg[x]);
		RELEASE(m_pDirSegState[x]);
	}
	RELEASE(m_pDeathSeg);
	RELEASE(m_pWinSeg);

	if( m_pLoader )
	{
		m_pLoader->ClearCache(GUID_DirectMusicAllTypes);
	}
	RELEASE(m_pLoader);
}

BOOL Music::Initialize()
{
	if( m_pPerf )
	{
		return FALSE; // already initialized
	}
	m_pPerf = CreatePerformance();
	if (!m_pPerf)
	{
		return FALSE;
	}

	if (FAILED(m_pPerf->Init(NULL)))
	{
		m_pPerf->Release();
		return FALSE;
	}


	if( SUCCEEDED( m_pPerf->AddPort(NULL)) )
	{
		IDirectMusicGraph* pGraph = CreateGraph();
		if( pGraph )
		{
			DWORD adwPChannels[1];
			adwPChannels[0] = 15;
			pGraph->InsertTool( this, adwPChannels, 1, 0 );
			m_pPerf->SetGraph(pGraph);
			pGraph->Release();

			m_pLoader = CreateLoader();
			if( m_pLoader )
			{
                IDirectMusicObject *pObject = CreateDLSObject(m_pLoader, L"MLines.dls");
                if (!pObject)
                {
                    Trace(0, "Could not load DLS collection!");
                }

                m_pBand = CreateBandFromFile(m_pLoader, L"MLines.bnd");
                if (m_pBand)
                {
                    m_pBand->Download(m_pPerf);
                }
                else
                {
                    Trace(0, "No band!!!");
                }
                RELEASE(pObject);


				m_pComposer = CreateComposer();
				if( m_pComposer )
				{
                    m_pIntroSeg = CreateSegmentFromFile( m_pLoader, L"MLIntro.mid" );
					if( m_pIntroSeg )
					{
						m_pIntroSeg->SetRepeats(-1); // set repeats to infinite
					}
					m_apTemplate[0] = CreateSegmentFromFile( m_pLoader, L"MLinesA.tpl" );
					m_apMainSeg[0] = CreateSegmentFromTemplate( m_pLoader, m_apTemplate[0],
						m_pComposer, L"MLines.sty", L"MLines1.per" );
					m_apMainSeg[1] = CreateSegmentFromTemplate( m_pLoader, m_apTemplate[0],
						m_pComposer, L"MLines.sty", L"MLines2.per" );
					m_apMainSeg[2] = CreateSegmentFromTemplate( m_pLoader, m_apTemplate[0],
						m_pComposer, L"MLines.sty", L"MLines3.per" );
					m_apMainSeg[3] = CreateSegmentFromTemplate( m_pLoader, m_apTemplate[0],
						m_pComposer, L"MLines.sty", L"MLines4.per" );
					m_apMainSeg[4] = CreateSegmentFromFile( m_pLoader, L"MLChaCha.mid" );
					for( int i = 0; i < NUM_MAINSEGS; i++ )
					{
						if( m_apMainSeg[i] )
						{
							m_apMainSeg[i]->SetRepeats(-1); // set repeats to infinite
						}
					}
					
					m_pUpSeg[0] = GetMotif( m_pLoader, L"mlines.sty", L"Go Up L" );
					m_pDownSeg[0] = GetMotif( m_pLoader, L"mlines.sty", L"Go Down L" );
					m_pLeftSeg[0] = GetMotif( m_pLoader, L"mlines.sty", L"Go Left L" );
					m_pRightSeg[0] = GetMotif( m_pLoader, L"mlines.sty", L"Go Right L" );
					m_pUpSeg[1] = GetMotif( m_pLoader, L"mlines.sty", L"Go Up" );
					m_pDownSeg[1] = GetMotif( m_pLoader, L"mlines.sty", L"Go Down" );
					m_pLeftSeg[1] = GetMotif( m_pLoader, L"mlines.sty", L"Go Left" );
					m_pRightSeg[1] = GetMotif( m_pLoader, L"mlines.sty", L"Go Right" );
					for( i = 0; i < NUM_PLAYERS; i++ )
					{
						if( m_pUpSeg[i] )
						{
							m_pUpSeg[i]->SetDefaultResolution( DMUS_SEGF_GRID );
						}
						if( m_pDownSeg[i] )
						{
							m_pDownSeg[i]->SetDefaultResolution( DMUS_SEGF_GRID );
						}
						if( m_pLeftSeg[i] )
						{
							m_pLeftSeg[i]->SetDefaultResolution( DMUS_SEGF_GRID );
						}
						if( m_pRightSeg[i] )
						{
							m_pRightSeg[i]->SetDefaultResolution( DMUS_SEGF_GRID );
						}
					}
					m_pWinSeg = GetMotif( m_pLoader, L"mlines.sty", L"Hooray" );
					m_pDeathSeg = GetMotif( m_pLoader, L"mlines.sty", L"Lose" );
					m_pEndSeg = CreateSegmentFromFile( m_pLoader, L"MLEnd.mid" );
				}
			} 
            else
            {
                MessageBox(NULL,
                           "SDK not properly installed.",
                           "MusicLines",
                           MB_OK | MB_ICONEXCLAMATION);

            }
		}
	}
	if( m_apMainSeg[0] )
	{
		return TRUE;
	}
	return FALSE;
}

// The game logic calls:
// Play( StartLevel, 0 );			at the beginning of the level
// Play( <direction>, <player #> ); when a player makes a move
// Play( Death, <player #> );		when a player dies but there is no winner
// Play( Win, <player #> );			when a player wins by having the longest trail
// Play( EndLevel, 0 );				when both players have died and the level is over
void Music::Play( PlayEvent playEvent, int PlayerNo )
{
	DWORD dwSegF;
	IDirectMusicSegment* pSeg = NULL;
	static int siCount = 0; // wrap around counter for main music

	if( (Up == playEvent) ||
		(Down == playEvent) ||
		(Left == playEvent) ||
		(Right == playEvent) )
	{
		if( m_fPlayingIntro )
		{
			Trace(0, "Switch from intro to real music");

			// if the intro is playing and a player moves,
			// switch to main music.
			// Cue the main music
			m_pPerf->PlaySegment( m_apMainSeg[siCount], DMUS_SEGF_DEFAULT, 0, NULL );
			m_fPlayingIntro = FALSE;
			siCount++; // so a new main segment plays next time
			if( siCount >= NUM_MAINSEGS ) siCount = 0;
		}
		else
		{
			// stop any currently playing up/down/left/right segment
			if( m_pDirSegState[PlayerNo] )
			{
				m_pPerf->Stop( NULL, m_pDirSegState[PlayerNo], 0, 0 );
			}
		}
	}
	// Currently we're not paying attention to player number, but
	// we could code this in if desired.
	switch( playEvent )
	{
	case StartLevel:
		if( FALSE == m_fPlayingIntro )
		{
			pSeg = m_pIntroSeg;
			dwSegF = DMUS_SEGF_QUEUE;
			m_fPlayingIntro = TRUE;
		}
		gcMoveCount = 0;
		break;

	case EndLevel:
		pSeg = m_pEndSeg;
		dwSegF = DMUS_SEGF_DEFAULT;
		break;

	case Up:
		pSeg = m_pUpSeg[PlayerNo];
		dwSegF = DMUS_SEGF_DEFAULT | DMUS_SEGF_SECONDARY;
		break;

	case Down:
		pSeg = m_pDownSeg[PlayerNo];
		dwSegF = DMUS_SEGF_DEFAULT | DMUS_SEGF_SECONDARY;
		break;

	case Left:
		pSeg = m_pLeftSeg[PlayerNo];
		dwSegF = DMUS_SEGF_DEFAULT | DMUS_SEGF_SECONDARY;
		break;

	case Right:
		pSeg = m_pRightSeg[PlayerNo];
		dwSegF = DMUS_SEGF_DEFAULT | DMUS_SEGF_SECONDARY;
		break;

	case Death:
		pSeg = m_pDeathSeg;
		dwSegF = DMUS_SEGF_DEFAULT | DMUS_SEGF_SECONDARY;
		break;

	case Win:
		pSeg = m_pWinSeg;
		dwSegF = DMUS_SEGF_DEFAULT | DMUS_SEGF_SECONDARY;
		break;
	}
	if( pSeg )
	{
		if( (Up == playEvent) ||
			(Down == playEvent) ||
			(Left == playEvent) ||
			(Right == playEvent) )
		{
			m_pPerf->PlaySegment( pSeg, dwSegF, 0, &m_pDirSegState[PlayerNo] );
		}
		else
		{
			m_pPerf->PlaySegment( pSeg, dwSegF, 0, NULL );
		}
		if( playEvent == EndLevel )
		{
			// queue the intro music after the end level music.
			// This call also resets m_fPlayingIntro. Don't worry
			// that this is TRUE while playing the EndLevel music,
			// it's not a big deal unless we decide we always need
			// to hear the entire EndLevel music.
			Play( StartLevel, 0 );
		}
	}
}

/*  IUnknown */
STDMETHODIMP Music::QueryInterface(THIS_ REFIID iid, LPVOID FAR *ppv)
{
	if (iid == IID_IUnknown || iid == IID_IDirectMusicTool)
	{
		*ppv = static_cast<IDirectMusicTool*>(this);
	} else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	reinterpret_cast<IUnknown*>(this)->AddRef();
	return S_OK;
}

STDMETHODIMP_(ULONG) Music::AddRef        (THIS)
{
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) Music::Release       (THIS)
{
    if( 0 == InterlockedDecrement(&m_cRef) )
    {
        delete this;
        return 0;
    }
    return m_cRef;
}

/*  IDirectMusicTool */
STDMETHODIMP Music::Init                 (THIS_ IDirectMusicGraph* pGraph)
{
	// This tool has no need to do any type of initialization.
	return E_NOTIMPL;
}

STDMETHODIMP Music::GetMsgDeliveryType   (THIS_ DWORD* pdwDeliveryType )
{
	// This tool wants messages immediately.
	// This is the default, so returning E_NOTIMPL
	// would work. The other method is to specifically
	// set *pdwDeliveryType to the delivery type, DMUS_PMSGF_IMMEDIATE,
	// DMUS_PMSGF_QUEUE, or DMUS_PMSGF_ATTIME.

	*pdwDeliveryType = DMUS_PMSGF_TOOL_IMMEDIATE;
	return S_OK;
}

STDMETHODIMP Music::GetMediaTypeArraySize(THIS_ DWORD* pdwNumElements )
{
    Trace(0,"GetMediaTypeArraySize");
	// This tool wants control changes (MIDI messages) and curves

	*pdwNumElements = 2;
	return S_OK;
}

STDMETHODIMP Music::GetMediaTypes        (THIS_ DWORD** padwMediaTypes, DWORD dwNumElements)
{
	// Fill in the array padwMediaTypes with the type of
	// messages this tool wants to process. In this case,
	// dwNumElements will be 2, since that is what this
	// tool returns from GetMediaTypeArraySize().

	if( dwNumElements == 2 )
	{
		// set the elements in the array to DMUS_PMSGT_MIDI
		// so we get control changes
		(*padwMediaTypes)[0] = DMUS_PMSGT_MIDI;
		// set DMUS_PMSGT_CURVE so we get curves
		(*padwMediaTypes)[1] = DMUS_PMSGT_CURVE;
		return S_OK;
	}
	else
	{
		// this should never happen
		return E_FAIL;
	}
}

STDMETHODIMP Music::ProcessPMsg          (THIS_ IDirectMusicPerformance* pPerf, DMUS_PMSG* pPMSG)
{
	Trace(0, "Tock\n");
	// Since ProcessPMsg is called from inside
	// the Performance's multi-media timer thread, take as little time
	// inside this function as possible.

	// The Tool is set up to receive messages of type DMUS_PMSGT_MIDI or DMUS_PMSGT_CURVE

	// If the DMUS_PMSGF_TOOL_ATTIME flag is set, this function
	// has already operated on this PMsg and requeued it.
	if( pPMSG->dwFlags & DMUS_PMSGF_TOOL_ATTIME )
	{
        Trace(0, "Tick\n");
		// Atomically increment the counter so the game logic knows
		// to advance the move visually.
		InterlockedIncrement(&gcMoveCount);

		// return DMUS_S_FREE so the original message is freed
		return DMUS_S_FREE;
	}
	else if( pPMSG->dwType == DMUS_PMSGT_MIDI )
	{
		DMUS_MIDI_PMSG* pMIDI = (DMUS_MIDI_PMSG*)pPMSG;

		// only process control changes of type 0x06 and value 0x7f 
		if(((pMIDI->bStatus & 0xf0) == 0xb0 ) &&
			(pMIDI->bByte1 == 0x06) &&
			(pMIDI->bByte2 == 0x7f))
		{
			// Requeue the message to come back at its time stamp.
			// It will then be processed by the previous block of code.
			pPMSG->dwFlags &= ~DMUS_PMSGF_TOOL_IMMEDIATE;
			pPMSG->dwFlags |= DMUS_PMSGF_TOOL_ATTIME;
			return DMUS_S_REQUEUE;
		}
	}
	else if( pPMSG->dwType == DMUS_PMSGT_CURVE )
	{
		DMUS_CURVE_PMSG* pCurve = (DMUS_CURVE_PMSG*)pPMSG;

		// only process curves of type 0x06
		if( pCurve->bCCData == 6 )
		{
			// Requeue the message to come back at its time stamp.
			// It will then be processed by the previous block of code.
			pPMSG->dwFlags &= ~DMUS_PMSGF_TOOL_IMMEDIATE;
			pPMSG->dwFlags |= DMUS_PMSGF_TOOL_ATTIME;
			return DMUS_S_REQUEUE;
		}
	}

	// stamp the message and return DMUS_S_REQUEUE so other midi messages continue to
	// the next tool
	if( pPMSG->pGraph )
	{
		if( SUCCEEDED( pPMSG->pGraph->StampPMsg( pPMSG )))
		{
			return DMUS_S_REQUEUE;
		}
	}
	// we were unable to stamp the tool, so free it
	return DMUS_S_FREE;
}

STDMETHODIMP Music::Flush                (THIS_ IDirectMusicPerformance* pPerf, DMUS_PMSG* pPMSG, REFERENCE_TIME rtTime)
{
	// this tool does not need to flush.
	return E_NOTIMPL;
}
