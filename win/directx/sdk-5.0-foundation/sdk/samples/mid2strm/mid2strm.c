/*==========================================================================
 *
 *  Copyright (C) 1995-1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File:	mid2strm.c
 *  Content:	Converts a MIDI file into a MDS (MidiStream) File.
 *
 ***************************************************************************/
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <assert.h>

// MIDI file constants
//
#define MThd			0x6468544D				// Start of file
#define MTrk			0x6B72544D				// Start of track

#define MIDI_SYSEX		((BYTE)0xF0)			// SysEx begin
#define MIDI_SYSEXEND	((BYTE)0xF7)			// SysEx begin
#define MIDI_META		((BYTE)0xFF)			// Meta event begin
#define MIDI_META_TEMPO	((BYTE)0x51)
#define MIDI_META_EOT	((BYTE)0x2F)			// End-of-track

#define MIDI_NOTEOFF	((BYTE)0x80)			// + note + velocity
#define MIDI_NOTEON		((BYTE)0x90)			// + note + velocity
#define MIDI_POLYPRESS  ((BYTE)0xA0)			// + pressure (2 bytes)
#define MIDI_CTRLCHANGE ((BYTE)0xB0)			// + ctrlr + value
#define MIDI_PRGMCHANGE	((BYTE)0xC0)			// + new patch
#define MIDI_CHANPRESS	((BYTE)0xD0)			// + pressure (1 byte)
#define MIDI_PITCHBEND	((BYTE)0xE0)			// + pitch bend (2 bytes)

#define CB_STREAMBUF	(4096)					// Size of each stream buffer

#define MIDS_SHORTMSG	(0x00000000)
#define MIDS_TEMPO		(0x01000000)

// Macros for swapping hi/lo-endian data
//
#define WORDSWAP(w)		(((w) >> 8) | \
						(((w) << 8) & 0xFF00))

#define DWORDSWAP(dw) 	(((dw) >> 24) |					\
						(((dw) >> 8) & 0x0000FF00) |	\
						(((dw) << 8) & 0x00FF0000) |	\
						(((dw) << 24) & 0xFF000000))

// In debug builds, TRACKERR will show us where the parser died
//
#ifdef _DEBUG
#define TRACKERR(p,sz) ShowTrackError(p,sz);
#else
#define TRACKERR(p,sz)
#endif


// These structures are stored in MIDI files; they need to be byte
// aligned.
//
#pragma pack(1)

// Chunk header. dwTag is either MTrk or MThd.
//
typedef struct
{
	DWORD			dwTag;					// Type
	DWORD			cbChunk;				// Length (hi-lo)
} MIDICHUNK;

// Contents of MThd chunk.
typedef struct
{
	WORD			wFormat;				// Format (hi-lo)
	WORD			cTrack;					// # tracks (hi-lo)
	WORD			wTimeDivision;			// Time division (hi-lo)
} MIDIFILEHDR;

#pragma pack()

// One event we're reading or writing to a track
//
typedef struct
{
	DWORD			tkEvent;			// Absolute time of event
	BYTE			abEvent[4];			// Event type and parameters if channel msg
	DWORD			cbEvent;			// Of data which follows if meta or sysex
	LPBYTE			pEvent;				// -> Event data if applicable
} MEVENT;	

// Description of a track open for read
//
#define ITS_F_ENDOFTRK	0x00000001

typedef struct
{
	DWORD			fdwTrack;			// Track status
	DWORD			cbTrack;			// Total bytes in track
	DWORD			cbLeft;				// Bytes left unread in track
	LPBYTE			pTrack;				// -> start of track data
	LPBYTE			pTrackPointer;		// -> next byte to read
	DWORD			tkNextEventDue;		// Absolute time of next event in track
	BYTE			bRunningStatus;		// Running status from last channel msg
#ifdef _DEBUG
	DWORD			nTrack;				// # of this track for debugging
#endif
} INTRACKSTATE;

// Description of the input MIDI file
//
typedef struct
{
	DWORD			cbFile;				// Total bytes in file
	LPBYTE			pFile;				// -> entire file in memory
	DWORD			cbLeft;				// Bytes left unread
	LPBYTE			pFilePointer;		// -> next byte to read

	DWORD			dwTimeDivision;		// Original time division
	DWORD			dwFormat;			// Original format
	DWORD			cTrack;				// Track count (specifies apIts size)
	INTRACKSTATE*	apIts;				// -> array of tracks in this file
} INFILESTATE;

// Description of a stream buffer on the output side
//
typedef struct STREAMBUF *PSTREAMBUF;
typedef struct STREAMBUF
{
	LPBYTE			pBuffer;			// -> Start of actual buffer
	DWORD			tkStart;			// Tick time just before first event
	LPBYTE			pbNextEvent;		// Where to write next event
	DWORD			cbLeft; 			// bytes left in buffer
	DWORD			cbLeftUncompressed;	// bytes left when uncompressed
	PSTREAMBUF		pNext;				// Next buffer
} STREAMBUF;

// Description of output stream open for write
//
typedef struct
{
	DWORD			tkTrack;			// Current tick position in track	
	PSTREAMBUF		pFirst;				// First stream buffer
	PSTREAMBUF		pLast;				// Last (current) stream buffer
} OUTSTREAMSTATE;

// Format of structs within a MSD file
//
// 'fmt ' chunk
//

#define MDS_F_NOSTREAMID	0x00000001	// Stream ID's skipped; reader inserts
typedef struct
{
	DWORD			dwTimeFormat;		// Low word == time format in SMF format
	DWORD			cbMaxBuffer;		// Guaranteed max buffer size
	DWORD			dwFlags;			// Format flags
} MIDSFMT;

// 'data' chunk buffer header
//
typedef struct
{
	DWORD			tkStart;			// Absolute tick offset at start of buffer
	DWORD			cbBuffer;			// Bytes in this buffer
} MIDSBUFFER;

// A few globals
//
static HANDLE		hInFile				= INVALID_HANDLE_VALUE;
static HANDLE		hOutFile			= INVALID_HANDLE_VALUE;
static INFILESTATE  ifs;
static OUTSTREAMSTATE ots;
static BOOL			fCompress			= FALSE;

// Messages
//
static char szInitErrMem[]			= "Out of memory.\n";
static char szInitErrInFile[]		= "Read error on input file or file is corrupt.\n";

#ifdef _DEBUG
static char gteBadRunStat[] = 		"Reference to missing running status.";
static char gteRunStatMsgTrunc[] = 	"Running status message truncated";
static char gteChanMsgTrunc[] = 	"Channel message truncated";
static char gteSysExLenTrunc[] = 	"SysEx event truncated (length)";
static char gteSysExTrunc[] = 		"SysEx event truncated";
static char gteMetaNoClass[] =		"Meta event truncated (no class byte)";
static char gteMetaLenTrunc[] = 	"Meta event truncated (length)";
static char gteMetaTrunc[] = 		"Meta event truncated";
#endif

// Prototypes
//
static BOOL 		Init(LPSTR szInFile, LPSTR szOutFile);
static LPBYTE		GetInFileData(DWORD cbToGet);
static void 		Cleanup(void);
static BOOL			BuildNewTracks(void);
static BOOL			WriteStreamBuffers(void);
static BOOL			GetTrackVDWord(INTRACKSTATE* pTs, LPDWORD lpdw);
static BOOL			GetTrackEvent(INTRACKSTATE* pTs, MEVENT *pMe);
static BOOL			AddEventToStream(MEVENT *pMe);
static LPBYTE		GetOutStreamBytes(DWORD tkNow, DWORD cbNeeded, DWORD cbUncompressed);
#ifdef _DEBUG
static void 		ShowTrackError(INTRACKSTATE* pTs, char* szErr);
#endif

void main(int argc, char* argv[])
{
	UINT			idxFnames;

	if (argc < 3)
	{
		fprintf(stderr, "Format is mid2strm [-c] infile outfile\n");
		fprintf(stderr, "-c\tNo-stream-id compression\n");
		exit(1);
	}

	idxFnames = 1;
	if (argv[1][0] == '-')
	{
		++idxFnames;
		if (argv[1][1] == 'c')
			fCompress = TRUE;
	}

	if (!Init(argv[idxFnames], argv[idxFnames+1]))
		exit(1);

	if (!BuildNewTracks())
		exit(1);

	if (!WriteStreamBuffers())
		exit(1);

	// Add cleanup code!!!
	//
	Cleanup();

	exit(0);
}

// Init
// 
// Open the input and output files
// Allocate and read the entire input file into memory
// Validate the input file structure
// Allocate the input track structures and initialize them
// Initialize the output track structures
//
// Return TRUE on success
// Prints its own error message if something goes wrong
//
static BOOL Init(LPSTR szInFile, LPSTR szOutFile)
{
	BOOL			fRet = FALSE;
	DWORD			cbRead;
	LPDWORD			lpdwTag;
	LPDWORD			lpcbHeader;
	DWORD			cbHeader;
	MIDIFILEHDR*	pHeader;
	INTRACKSTATE*	pTs;
	UINT			idx;
	
	// Initialize things we'll try to free later if we fail
	//
	ifs.cbFile = 0;
	ifs.pFile = NULL;
	ifs.apIts = NULL;

	// Attempt to open the input and output files
	//
	hInFile = CreateFile(szInFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hInFile)
	{
		fprintf(stderr, "Could not open \"%s\" for read.\n", szInFile);
		goto Init_Cleanup;
	}

	hOutFile = CreateFile(szOutFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hOutFile)
	{
		fprintf(stderr, "Could not open \"%s\" for write.\n", szOutFile);
		goto Init_Cleanup;
	}

	// Figure out how big the input file is and allocate a chunk of memory big enough
	// to hold the whole thing. Read the whole file in at once.
	//
	if (((UINT)-1) == (ifs.cbFile = GetFileSize(hInFile, NULL)))
	{
		fprintf(stderr, "File system error on input file.\n");
		goto Init_Cleanup;
	}

	if (NULL == (ifs.pFile = GlobalAllocPtr(GPTR, ifs.cbFile)))
	{
		fprintf(stderr, szInitErrMem);
		goto Init_Cleanup;
	}

	if ((!ReadFile(hInFile, ifs.pFile, ifs.cbFile, &cbRead, NULL)) ||
		cbRead != ifs.cbFile)
	{
		fprintf(stderr, szInitErrInFile);
		goto Init_Cleanup;
	}
	
	// Set up to read from the memory buffer. Read and validate
	// - MThd header
	// - size of file header chunk
	// - file header itself
	//	
	ifs.cbLeft = ifs.cbFile;
	ifs.pFilePointer = ifs.pFile;

	if (NULL == (lpdwTag = (LPDWORD)GetInFileData(sizeof(*lpdwTag))) ||
		*lpdwTag != MThd ||
		NULL == (lpcbHeader = (LPDWORD)GetInFileData(sizeof(*lpcbHeader))) ||
		(cbHeader = DWORDSWAP(*lpcbHeader)) < sizeof(MIDIFILEHDR) ||
		NULL == (pHeader = (MIDIFILEHDR*)GetInFileData(cbHeader)))
	{
		fprintf(stderr, szInitErrInFile);
		goto Init_Cleanup;
	}

	// File header is stored in hi-lo order. Swap this into Intel order and save
	// parameters in our native int size (32 bits)
	//
	ifs.dwFormat = (DWORD)WORDSWAP(pHeader->wFormat);
	ifs.cTrack = (DWORD)WORDSWAP(pHeader->cTrack);
	ifs.dwTimeDivision = (DWORD)WORDSWAP(pHeader->wTimeDivision);

	// We know how many tracks there are; allocate the structures for them and parse
	// them. The parse merely looks at the MTrk signature and track chunk length
	// in order to skip to the next track header.
	//
	ifs.apIts = (INTRACKSTATE*)GlobalAllocPtr(GPTR, ifs.cTrack*sizeof(INTRACKSTATE));
	if (NULL == ifs.apIts)
	{
		fprintf(stderr, szInitErrMem);
		goto Init_Cleanup;
	}

	for (idx = 0, pTs = ifs.apIts; idx < ifs.cTrack; ++idx, ++pTs)
	{
		if (NULL == (lpdwTag = (LPDWORD)GetInFileData(sizeof(*lpdwTag))) ||
			*lpdwTag != MTrk ||
			NULL == (lpcbHeader = (LPDWORD)GetInFileData(sizeof(*lpcbHeader))))
		{
			fprintf(stderr, szInitErrInFile);
			goto Init_Cleanup;
		}

		cbHeader = DWORDSWAP(*lpcbHeader);
		pTs->cbTrack = cbHeader;
		pTs->cbLeft = cbHeader;
		pTs->pTrack = GetInFileData(cbHeader);
		if (NULL == pTs->pTrack)
		{
			fprintf(stderr, szInitErrInFile);
			goto Init_Cleanup;
		}

#ifdef _DEBUG
		pTs->nTrack = idx;
#endif
		pTs->pTrackPointer = pTs->pTrack;
		pTs->cbLeft = pTs->cbTrack;
		pTs->fdwTrack = 0;
		pTs->bRunningStatus = 0;

		// Handle bozo MIDI files which contain empty track chunks
		//
		if (!pTs->cbLeft)
		{
			pTs->fdwTrack |= ITS_F_ENDOFTRK;
			continue;
		}


		// We always preread the time from each track so the mixer code can
		// determine which track has the next event with a minimum of work
		//
		if (!GetTrackVDWord(pTs, &pTs->tkNextEventDue))
		{
			fprintf(stderr, szInitErrInFile);
			goto Init_Cleanup;
		}
	}

	ots.tkTrack = 0;
	ots.pFirst = NULL;
	ots.pLast = NULL;

	fRet = TRUE;
		
Init_Cleanup:
	if (!fRet)
		Cleanup();

	return fRet;
}

//
// GetInFileData
//
// Gets the requested number of bytes of data from the input file and returns
// a pointer to them.
// 
// Returns a pointer to the data or NULL if we'd read more than is
// there.
//
static LPBYTE		GetInFileData(DWORD cbToGet)
{
	LPBYTE				pRet;

	if (ifs.cbLeft < cbToGet)
		return NULL;

	pRet = ifs.pFilePointer;

	ifs.cbLeft -= cbToGet;
	ifs.pFilePointer += cbToGet;

	return pRet;
}

//
// Cleanup
//
// Free anything we ever allocated
//
static void 		Cleanup(void)
{
	PSTREAMBUF			pCurr;
	PSTREAMBUF			pNext;

	if (hInFile != INVALID_HANDLE_VALUE) 	CloseHandle(hInFile);
	if (hOutFile != INVALID_HANDLE_VALUE) 	CloseHandle(hOutFile);
	if (ifs.pFile)							GlobalFreePtr(ifs.pFile);
	if (ifs.apIts)							GlobalFreePtr(ifs.apIts);
	

	pCurr = ots.pFirst;
	while (pCurr)
	{
		pNext = pCurr->pNext;
		GlobalFreePtr(pCurr);
		pCurr = pNext;
	}
}

//
// BuildNewTracks
//
// This is where the actual work gets done.
//
// Until all tracks are done,
//  Scan the tracks to find the next due event
//  Figure out where the event belongs in the new mapping
//  Put it there
// Add end of track metas to all new tracks that now have any data
//
// Return TRUE on success
// Prints its own error message if something goes wrong
//
static BOOL			BuildNewTracks(void)
{
	INTRACKSTATE*		pTs;
	INTRACKSTATE*		pTsFound;
	UINT				idx;
	DWORD				tkNext;
	MEVENT			me;
	
	for(;;)
	{
		// Find nearest event due
		//
		pTsFound = NULL;
		tkNext = 0xFFFFFFFFL;

		for (idx = 0, pTs = ifs.apIts; idx < ifs.cTrack; ++idx, ++pTs)
			if ((!(pTs->fdwTrack & ITS_F_ENDOFTRK)) && (pTs->tkNextEventDue < tkNext))
			{
				tkNext = pTs->tkNextEventDue;
				pTsFound = pTs;
			}

		// None found? We must be done
		//
		if (!pTsFound)
			break;

		// Ok, get the event header from that track
		//

		if (!GetTrackEvent(pTsFound, &me))
		{
			fprintf(stderr, "MIDI file is corrupt!\n");
			return FALSE;
		}

		// Don't add end of track event 'til we're done
		//
		if (me.abEvent[0] == MIDI_META && me.abEvent[1] == MIDI_META_EOT)
			continue;
		
		if (!AddEventToStream(&me))
		{
			fprintf(stderr, "Out of memory building tracks.\n");
			return FALSE;
		}
	}	

	return TRUE;
}

//
// WriteStreamBuffers
//
// Write stream buffers into an MDS file (RIFF MIDS format)
//
// Return TRUE on success
// Prints its own error message if something goes wrong
//
#define FOURCC_MIDS	mmioFOURCC('M','I','D','S')
#define FOURCC_fmt  mmioFOURCC('f','m','t',' ')
#define FOURCC_data mmioFOURCC('d','a','t','a')

static BOOL			WriteStreamBuffers(void)
{
	DWORD			cbFmt;
	DWORD			cbData;
	DWORD			cbRiff;
	PSTREAMBUF		psb;
	FOURCC			fcc;
	FOURCC			fcc2;
	MIDSFMT			fmt;
	MIDSBUFFER		data;
	DWORD			cb;
	DWORD			cBuffers;

	// Walk buffer list to find entire size of data chunk
	//
	cbData = sizeof(cBuffers);
	cBuffers = 0;
	for (psb = ots.pFirst; psb; psb = psb->pNext, ++cBuffers)
		cbData += sizeof(MIDSBUFFER) + (CB_STREAMBUF - psb->cbLeft);

	cbFmt = sizeof(fmt);

	// Figure size of entire RIFF chunk
	//
	cbRiff = 
		sizeof(FOURCC) +			// RIFF form type ('MIDS')
		sizeof(FOURCC) +			// Format chunk type ('fmt ')
		sizeof(DWORD) + 			// Format chunk size
		sizeof(MIDSFMT) +			// Format chunk contents
		sizeof(FOURCC) +			// Data chunk type ('data')
		sizeof(DWORD) +				// Data chunk size
		cbData;						// Data chunk contents
		
	fcc = FOURCC_RIFF;
	fcc2 = FOURCC_MIDS;
	if ((!WriteFile(hOutFile, &fcc, sizeof(fcc), &cb, NULL)) ||
		(!WriteFile(hOutFile, &cbRiff, sizeof(cbRiff), &cb, NULL)) ||
		(!WriteFile(hOutFile, &fcc2, sizeof(fcc2), &cb, NULL)))
		return FALSE;


	fmt.dwTimeFormat 	= ifs.dwTimeDivision;
	fmt.cbMaxBuffer		= CB_STREAMBUF;
	fmt.dwFlags 		= 0;
	if (fCompress)
		fmt.dwFlags		|= MDS_F_NOSTREAMID;

	fcc = FOURCC_fmt;
	if ((!WriteFile(hOutFile, &fcc, sizeof(fcc), &cb, NULL)) ||
		(!WriteFile(hOutFile, &cbFmt, sizeof(cbFmt), &cb, NULL)) ||
		(!WriteFile(hOutFile, &fmt, sizeof(fmt), &cb, NULL)))
		return FALSE;

	fcc = FOURCC_data;
	if ((!WriteFile(hOutFile, &fcc, sizeof(fcc), &cb, NULL)) ||
		(!WriteFile(hOutFile, &cbData, sizeof(cbData), &cb, NULL)) ||
		(!WriteFile(hOutFile, &cBuffers, sizeof(cBuffers), &cb, NULL)))
		return FALSE;

	for (psb = ots.pFirst; psb; psb = psb->pNext)
	{
		data.tkStart = psb->tkStart;
		data.cbBuffer = CB_STREAMBUF - psb->cbLeft;

		if ((!WriteFile(hOutFile, &data, sizeof(data), &cb, NULL)) ||
			(!WriteFile(hOutFile, psb->pBuffer, data.cbBuffer, &cb, NULL)))
			return FALSE;
	}
			
	return TRUE;
}

//
// GetTrackVDWord
//
// Attempts to parse a variable length DWORD from the given track. A VDWord
// in a MIDI file
//  (a) is in lo-hi format 
//  (b) has the high bit set on every byte except the last
//
// Returns the DWORD in *lpdw and TRUE on success; else
// FALSE if we hit end of track first. Sets ITS_F_ENDOFTRK
// if we hit end of track.
//
static BOOL			GetTrackVDWord(INTRACKSTATE* pTs, LPDWORD lpdw)
{
	BYTE				b;
	DWORD				dw = 0;

	if (pTs->fdwTrack & ITS_F_ENDOFTRK)
		return FALSE;

	do
	{
		if (!pTs->cbLeft)
		{
			pTs->fdwTrack |= ITS_F_ENDOFTRK;
			return FALSE;
		}

		b = *pTs->pTrackPointer++;
		--pTs->cbLeft;

	 	dw = (dw << 7) | (b & 0x7F);
	} while (b & 0x80);

	*lpdw = dw;

	return TRUE;
}

//
// GetTrackEvent
//
// Fills in the event struct with the next event from the track
//
// pMe->tkEvent will contain the absolute tick time of the event
// pMe->abEvent[0] will contain
//  MIDI_META if the event is a meta event;
//   in this case pMe->abEvent[1] will contain the meta class
//  MIDI_SYSEX or MIDI_SYSEXEND if the event is a SysEx event
//  Otherwise, the event is a channel message and pMe->abEvent[1]
//   and pMe->abEvent[2] will contain the rest of the event.
//
// pMe->cbEvent will contain
//  The total length of the channel message in pMe->abEvent if
//   the event is a channel message
//  The total length of the paramter data pointed to by
//   pMe->pEvent otherwise
//
// pMe->pEvent will point at any additional paramters if the 
//  event is a SysEx or meta event with non-zero length; else
//  it will contain NULL
//
// Returns TRUE on success or FALSE on any kind of parse error
// Prints its own error message ONLY in the debug version
//
// Maintains the state of the input track (i.e. pTs->cbLeft,
// pTs->pTrackPointers, and pTs->bRunningStatus).
//
static BOOL			GetTrackEvent(INTRACKSTATE* pTs, MEVENT *pMe)
{
	BYTE				b;
	UINT				cbEvent;

	pMe->pEvent = NULL;

	// Already at end of track? There's nothing to read.
	//
	if ((pTs->fdwTrack & ITS_F_ENDOFTRK) || !pTs->cbLeft)
		return FALSE;

	// Get the first byte, which determines the type of event.
	//
	b = *pTs->pTrackPointer++;
	--pTs->cbLeft;

	// If the high bit is not set, then this is a channel message
	// which uses the status byte from the last channel message
	// we saw. NOTE: We do not clear running status across SysEx or
	// meta events even though the spec says to because there are
	// actually files out there which contain that sequence of data.
	//
	if (!(b & 0x80))
	{
		// No previous status byte? We're hosed.
		//
		if (!pTs->bRunningStatus)
		{
			TRACKERR(pTs, gteBadRunStat);
			return FALSE;
		}

		pMe->abEvent[0] = pTs->bRunningStatus;
		pMe->abEvent[1] = b;

		b = pMe->abEvent[0] & 0xF0;
		pMe->cbEvent = 2;

		// Only program change and channel pressure events are 2 bytes long;
		// the rest are 3 and need another byte
		//
		if (b != MIDI_PRGMCHANGE && b != MIDI_CHANPRESS)
		{
			if (!pTs->cbLeft)
			{
				TRACKERR(pTs, gteRunStatMsgTrunc);
				pTs->fdwTrack |= ITS_F_ENDOFTRK;
				return FALSE;
			}

			pMe->abEvent[2] = *pTs->pTrackPointer++;
			--pTs->cbLeft;
			++pMe->cbEvent;
		}
	}
	else if ((b & 0xF0) != MIDI_SYSEX)
	{
		// Not running status, not in SysEx range - must be
		// normal channel message (0x80-0xEF)
		//
		pMe->abEvent[0] = b;
		pTs->bRunningStatus = b;
		
		// Strip off channel and just keep message type
		//
		b &= 0xF0;

		cbEvent = (b == MIDI_PRGMCHANGE || b == MIDI_CHANPRESS) ? 1 : 2;
		pMe->cbEvent = cbEvent + 1;

		if (pTs->cbLeft < cbEvent)
		{
			TRACKERR(pTs, gteChanMsgTrunc);
			pTs->fdwTrack |= ITS_F_ENDOFTRK;
			return FALSE;
		}

		pMe->abEvent[1] = *pTs->pTrackPointer++;
		if (cbEvent == 2)
			pMe->abEvent[2] = *pTs->pTrackPointer++;

		pTs->cbLeft -= cbEvent;
	} 
	else if (b == MIDI_SYSEX || b == MIDI_SYSEXEND)
	{
		// One of the SysEx types. (They are the same as far as we're concerned;
		// there is only a semantic difference in how the data would actually
		// get sent when the file is played. We must take care to put the correct
		// event type back on the output track, however.)
		//
		// Parse the general format of:
		//  BYTE 	bEvent (MIDI_SYSEX or MIDI_SYSEXEND)
		//  VDWORD 	cbParms
		//  BYTE   	abParms[cbParms]
		//
		pMe->abEvent[0] = b;
		if (!GetTrackVDWord(pTs, &pMe->cbEvent))
		{
			TRACKERR(pTs, gteSysExLenTrunc);
			return FALSE;
		}
		
		if (pTs->cbLeft < pMe->cbEvent)
		{
			TRACKERR(pTs, gteSysExTrunc);
			pTs->fdwTrack |= ITS_F_ENDOFTRK;
			return FALSE;
		}

		pMe->pEvent = pTs->pTrackPointer;
		pTs->pTrackPointer += pMe->cbEvent;
		pTs->cbLeft -= pMe->cbEvent;
	} 
	else if (b == MIDI_META)
	{
		// It's a meta event. Parse the general form:
		//  BYTE	bEvent	(MIDI_META)
		//	BYTE	bClass
		//  VDWORD	cbParms
		//	BYTE	abParms[cbParms]
		//
		pMe->abEvent[0] = b;

		if (!pTs->cbLeft)
		{
			TRACKERR(pTs, gteMetaNoClass);
			pTs->fdwTrack |= ITS_F_ENDOFTRK;
			return FALSE;
		}

		pMe->abEvent[1] = *pTs->pTrackPointer++;
		--pTs->cbLeft;

		if (!GetTrackVDWord(pTs, &pMe->cbEvent))
		{	
			TRACKERR(pTs, gteMetaLenTrunc);
			return FALSE;
		}

		// NOTE: Perfectly valid to have a meta with no data
		// In this case, cbEvent == 0 and pEvent == NULL
		//
		if (pMe->cbEvent)
		{		
			if (pTs->cbLeft < pMe->cbEvent)
			{
				TRACKERR(pTs, gteMetaTrunc);
				pTs->fdwTrack |= ITS_F_ENDOFTRK;
				return FALSE;
			}

			pMe->pEvent = pTs->pTrackPointer;
			pTs->pTrackPointer += pMe->cbEvent;
			pTs->cbLeft -= pMe->cbEvent;
		}

		if (pMe->abEvent[1] == MIDI_META_EOT)
			pTs->fdwTrack |= ITS_F_ENDOFTRK;
	}
	else
	{
		// Messages in this range are system messages and aren't supposed to
		// be in a normal MIDI file. If they are, we've misparsed or the
		// authoring software is stpuid.
		//
		return FALSE;
	}

	// Event time was already stored as the current track time
	//
	pMe->tkEvent = pTs->tkNextEventDue;

	// Now update to the next event time. The code above MUST properly
	// maintain the end of track flag in case the end of track meta is
	// missing. 
	//
	if (!(pTs->fdwTrack & ITS_F_ENDOFTRK))
	{
		DWORD				tkDelta;

		if (!GetTrackVDWord(pTs, &tkDelta))
			return FALSE;

		pTs->tkNextEventDue += tkDelta;
	}

	return TRUE;
}

//
// AddEventToStream
//
// Put the given event onto the given output track.
// pMe must point to an event filled out in accordance with the
// description given in GetTrackEvent
//
// Returns TRUE on sucess or FALSE if we're out of memory
//
static BOOL			AddEventToStream(MEVENT *pMe)
{
	PDWORD					pdw;		
	DWORD					tkNow;
	DWORD					tkDelta;	
	UINT					cdw;

	tkNow = ots.tkTrack;

	// Delta time is absolute event time minus absolute time
	// already gone by on this track
	//
	tkDelta = pMe->tkEvent - ots.tkTrack;

	// Event time is now current time on this track
	//
	ots.tkTrack = pMe->tkEvent;

	if (pMe->abEvent[0] < MIDI_SYSEX)
	{
		// Channel message. We know how long it is, just copy it. Need 3 DWORD's: delta-t, 
		// stream-ID, event
		// 
		// TODO: Compress with running status
		//

		cdw = (fCompress ? 2 : 3);
		if (NULL == (pdw = (PDWORD)GetOutStreamBytes(tkNow, cdw * sizeof(DWORD), 3 * sizeof(DWORD))))
			return FALSE;

		*pdw++ = tkDelta;
		if (!fCompress)
			*pdw++ = 0;
		*pdw =	(pMe->abEvent[0]) |
				(((DWORD)pMe->abEvent[1]) << 8) |
				(((DWORD)pMe->abEvent[2]) << 16) |
				MIDS_SHORTMSG;

	}
	else if (pMe->abEvent[0] == MIDI_SYSEX || pMe->abEvent[0] == MIDI_SYSEXEND)
	{
		fprintf(stderr, "NOTE: Ignoring SysEx for now.\n");
	}
	else
	{
		// Better be a meta event.
		//  BYTE		bEvent
		//  BYTE 		bClass
		//	VDWORD		cbParms
		//	BYTE		abParms[cbParms]
		//
		assert(pMe->abEvent[0] == MIDI_META);

		// The only meta-event we care about is change tempo
		//
		if (pMe->abEvent[1] != MIDI_META_TEMPO)
			return TRUE;

		assert(pMe->cbEvent == 3);

		cdw = (fCompress ? 2 : 3);
		pdw = (PDWORD)GetOutStreamBytes(tkNow, cdw * sizeof(DWORD), 3 * sizeof(DWORD));
		if (NULL == pdw)
			return FALSE;

		*pdw++ = tkDelta;
		if (!fCompress)
			*pdw++ = (DWORD)-1;
		*pdw =	(pMe->pEvent[2]) |
				(((DWORD)pMe->pEvent[1]) << 8) |
				(((DWORD)pMe->pEvent[0]) << 16) |
				MIDS_TEMPO;
	}

	return TRUE;	
}

//
// GetOutStreamBytes
//
// This function performs the memory management and pseudo-file I/O for output
// tracks.
// 
// We build a linked list of stream buffers as they would exist if they were
// about to be played. Each buffer is CB_STREAMBUF bytes long maximum. They are
// filled as full as possible; events are not allowed to cross buffers.
//
// Returns a pointer to the number of requested bytes or NULL if we're out of memory
//
static LPBYTE		GetOutStreamBytes(DWORD tkNow, DWORD cbNeeded, DWORD cbUncompressed)
{
	LPBYTE					pb;

	// Round request up to the next DWORD boundry. This aligns the final output buffer correctly
	// and allows the above routines to deal with byte-aligned data
	//
	cbNeeded = (cbNeeded + 3) & ~3;
	cbUncompressed = (cbUncompressed + 3) & ~3;

	assert(cbUncompressed >= cbNeeded);

	if (NULL == ots.pLast || cbUncompressed > ots.pLast->cbLeftUncompressed)
	{
		PSTREAMBUF 			pNew;

		pNew = GlobalAllocPtr(GHND, sizeof(*pNew) + CB_STREAMBUF);
		if (NULL == pNew)
			return NULL;
			
		pNew->pBuffer 		= (LPBYTE)(pNew + 1);
		pNew->tkStart		= tkNow;
		pNew->pbNextEvent 	= pNew->pBuffer;
		pNew->cbLeft 		= CB_STREAMBUF;
		pNew->cbLeftUncompressed = CB_STREAMBUF;
		pNew->pNext 		= NULL;

		if (!ots.pLast)
		{
			ots.pFirst = pNew;
			ots.pLast  = pNew;
		}
		else
		{
			ots.pLast->pNext 	= pNew;
			ots.pLast			= pNew;
		}
	}

	// If there's STILL not enough room for the requested block, then an event is bigger than 
	// the buffer size -- this is unacceptable. 
	//
	if (cbNeeded > ots.pLast->cbLeft)
	{
		fprintf(stderr, "NOTE: An event requested %lu bytes of memory; the\n", cbNeeded);
		fprintf(stderr, "      maximum configured buffer size is %lu.\n", (DWORD)CB_STREAMBUF);

		return NULL;
	}

	pb = ots.pLast->pbNextEvent;

	ots.pLast->pbNextEvent += cbNeeded;
	ots.pLast->cbLeft -= cbNeeded;
	ots.pLast->cbLeftUncompressed -= cbUncompressed;

	return pb;
}

#ifdef _DEBUG
static void 			ShowTrackError(INTRACKSTATE* pTs, char* szErr)
{
	fprintf(stderr, "Track %u: %s\n", pTs->nTrack, szErr);		
	fprintf(stderr, "Track offset %lu\n", (DWORD)(pTs->pTrackPointer - pTs->pTrack));
	fprintf(stderr, "Track total %lu  Track left %lu\n", pTs->cbTrack, pTs->cbLeft);
}
#endif
