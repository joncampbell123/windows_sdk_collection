// Helper.h : Helper functions for DirectMusic
// 
// Authored by: Jim Geist and Mark Burton
//
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _DMUSHELPER_H
#define _DMUSHELPER_H

#include <dmusici.h>

IDirectMusicGraph*			CreateGraph(void);
IDirectMusicLoader*			CreateLoader(void);
IDirectMusicPerformance*	CreatePerformance(void);
IDirectMusicComposer*		CreateComposer(void);
IDirectMusicSegment*		CreateSegmentFromFile(IDirectMusicLoader*, WCHAR*);
IDirectMusicSegment*		CreateSecSegmentFromFile(IDirectMusicLoader*, WCHAR*);
IDirectMusicSegment*		CreateSegmentFromTemplate(IDirectMusicLoader*, 
													  IDirectMusicSegment*,
													  IDirectMusicComposer*,
													  WCHAR* wszStyle,
													  WCHAR* wszPersonality);
IDirectMusicSegment*		GetMotif(IDirectMusicLoader*, 
									 WCHAR* wszStyle,
									 WCHAR* wszMotif);
IDirectMusicBand*           CreateBandFromFile(IDirectMusicLoader*, WCHAR*);
IDirectMusicObject*         CreateDLSObject(IDirectMusicLoader*, WCHAR *);

#define RELEASE(x) if(x) x->Release();

#endif // _DMUSHELPER_H