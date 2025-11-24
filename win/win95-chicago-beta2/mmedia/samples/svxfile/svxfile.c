/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1994 - 1995  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/
//--------------------------------------------------------------------------;
//
//  File: Svxfile.c
//
//  Abstract:
//      An implementation in C of an AVI File Handler to read 
//      Amiga SVX digital audio files as if they wre an AVI file with
//      one audio stream.
//
//      The sample WAVEFILE was used as a base for this sample code.
//      Routines that were changed in order to add functionality for
//      Amiga SVX digital audio files are noted below.
//
//  Contents:
//      LoadUnicodeString()
//
//      ReverseWORD()                  Added for SVXFile
//      ReverseDWORD()                 Added for SVXFile
//
//      SVXFileCreate()
//
//      SVXUnknownQueryInterface()
//      SVXUnknownAddRef()
//      SVXUnknownRelease()            Modified for SVXFile
//      SVXFileQueryInterface()
//      SVXFileAddRef()
//      SVXFileRelease()
//      SVXPersistQueryInterface()
//      SVXPersistAddRef()
//      SVXPersistRelease()
//      SVXStreamQueryInterface()
//      SVXStreamAddRef()
//      SVXStreamRelease()
//
//      FileName()
//      SVXFileOpen()                  Modified for SVXFile
//      SVXFileGetStream()
//      SVXFileDeleteStream()
//      SVXFileSave()
//      SVXFileCreateStream()
//      SVXFileWriteData()
//      SVXFileReadData()
//      SVXFileEndRecord()
//      SVXFileInfo()
//
//      SVXStreamCreate()
//      SVXStreamFindSample()
//      SVXStreamReadFormat()
//      SVXStreamInfo()
//      SVXStreamSetInfo()
//      SVXInterleaveChannels()        Added for SVXFile    
//      SVXStreamRead()                Modified for SVXFile
//      SVXStreamSetFormat()           Modified for SVXFile
//      SVXUninterleaveChannels()      Added for SVXFile    
//      SVXStreamWrite()               Modified for SVXFile
//      SVXStreamDelete()
//      SVXStreamReadData()
//      SVXStreamWriteData()
//      SVXFileReserved()
//      SVXStreamReserved()
//      SVXPersistGetClassID()         Modified for SVXFile
//      SVXPersistIsDirty()
//      SVXPersistLoad()               Modified for SVXFile
//      SVXPersistSave()
//      SVXPersistSaveCompleted()
//      SVXPersistGetCurFile()
//
//--------------------------------------------------------------------------;

#define INC_OLE2
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <msacm.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <ctype.h>
#include <vfw.h>
#include "MulDiv32.h"
#include "svxfile.h"

#define formtypeWAVE            mmioFOURCC('W', 'A', 'V', 'E')
#define ckidWAVEFORMAT          mmioFOURCC('f', 'm', 't', ' ')
#define ckidWAVEDATA	        mmioFOURCC('d', 'a', 't', 'a')
#define ckidForm                mmioFOURCC('F', 'O', 'R', 'M')
#define ckidSVX                 mmioFOURCC('8', 'S', 'V', 'X')
#define ckidVHDR                mmioFOURCC('V', 'H', 'D', 'R')
#define ckidCHAN                mmioFOURCC('C', 'H', 'A', 'N')
#define ckidBODY                mmioFOURCC('B', 'O', 'D', 'Y')
#define SVX_IFF_BITSPERSAMPLE   8
#define SVX_IFF_STEREO          6

LPSTR FAR FileName(LPCSTR lszPath);

//
// Function prototypes and Vtbl for the Unknown interface
//
STDMETHODIMP SVXUnknownQueryInterface(LPUNKNOWN pu, REFIID iid, void FAR* FAR* ppv);
STDMETHODIMP_(ULONG) SVXUnknownAddRef(LPUNKNOWN pu);
STDMETHODIMP_(ULONG) SVXUnknownRelease(LPUNKNOWN pu);

IUnknownVtbl UnknownVtbl = {
    SVXUnknownQueryInterface,
    SVXUnknownAddRef,
    SVXUnknownRelease
};

//
// Function prototypes and Vtbl for the AVIFile interface
//
STDMETHODIMP SVXFileQueryInterface(PAVIFILE pf, REFIID iid, void FAR* FAR* ppv);
STDMETHODIMP_(ULONG) SVXFileAddRef(PAVIFILE pf);
STDMETHODIMP_(ULONG) SVXFileRelease(PAVIFILE pf);
STDMETHODIMP SVXFileInfo(PAVIFILE pf, AVIFILEINFOW FAR *pfi, LONG lSize);
STDMETHODIMP SVXFileGetStream(PAVIFILE pf, PAVISTREAM FAR * ppavi, DWORD fccType, LONG lParam);
STDMETHODIMP SVXFileCreateStream(PAVIFILE pf, PAVISTREAM FAR *ppstream, AVISTREAMINFOW FAR *psi);
STDMETHODIMP SVXFileWriteData(PAVIFILE pf, DWORD ckid, LPVOID lpData, LONG cbData);
STDMETHODIMP SVXFileReadData(PAVIFILE pf, DWORD ckid, LPVOID lpData, LONG FAR *lpcbData);
STDMETHODIMP SVXFileEndRecord(PAVIFILE pf);
STDMETHODIMP SVXFileDeleteStream(PAVIFILE pf, DWORD fccType, LONG lParam);


IAVIFileVtbl FileVtbl = {
    SVXFileQueryInterface,
    SVXFileAddRef,
    SVXFileRelease,
    SVXFileInfo,
    SVXFileGetStream,
    SVXFileCreateStream,
    SVXFileWriteData,
    SVXFileReadData,
    SVXFileEndRecord,
    SVXFileDeleteStream
};


STDMETHODIMP SVXPersistQueryInterface(LPPERSISTFILE pf, REFIID iid, void FAR* FAR* ppv);
STDMETHODIMP_(ULONG) SVXPersistAddRef(LPPERSISTFILE pf);
STDMETHODIMP_(ULONG) SVXPersistRelease(LPPERSISTFILE pf);
STDMETHODIMP SVXPersistGetClassID (LPPERSISTFILE ppf, LPCLSID lpClassID);
STDMETHODIMP SVXPersistIsDirty (LPPERSISTFILE ppf);
STDMETHODIMP SVXPersistLoad (LPPERSISTFILE ppf,
			      LPCOLESTR lpszFileName, DWORD grfMode);
STDMETHODIMP SVXPersistSave (LPPERSISTFILE ppf,
			      LPCOLESTR lpszFileName, BOOL fRemember);
STDMETHODIMP SVXPersistSaveCompleted (LPPERSISTFILE ppf,
				       LPCOLESTR lpszFileName);
STDMETHODIMP SVXPersistGetCurFile (LPPERSISTFILE ppf,
				    LPOLESTR FAR * lplpszFileName);


IPersistFileVtbl PersistVtbl = {
	SVXPersistQueryInterface,
	SVXPersistAddRef,
	SVXPersistRelease,
	SVXPersistGetClassID,
	SVXPersistIsDirty,
	SVXPersistLoad,
	SVXPersistSave,
	SVXPersistSaveCompleted,
	SVXPersistGetCurFile
};


//
// Function prototypes and Vtbl for the AVIStream interface
//
STDMETHODIMP SVXStreamQueryInterface(PAVISTREAM ps, REFIID riid, LPVOID FAR* ppvObj);
STDMETHODIMP SVXStreamCreate(PAVISTREAM ps, LONG lParam1, LONG lParam2);
STDMETHODIMP_(ULONG) SVXStreamAddRef(PAVISTREAM ps);
STDMETHODIMP_(ULONG) SVXStreamRelease(PAVISTREAM ps);
STDMETHODIMP SVXStreamInfo(PAVISTREAM ps, AVISTREAMINFOW FAR * psi, LONG lSize);
STDMETHODIMP_(LONG) SVXStreamFindSample(PAVISTREAM ps, LONG lPos, LONG lFlags);
STDMETHODIMP SVXStreamReadFormat(PAVISTREAM ps, LONG lPos, LPVOID lpFormat, LONG FAR *lpcbFormat);
STDMETHODIMP SVXStreamSetFormat(PAVISTREAM ps, LONG lPos, LPVOID lpFormat, LONG cbFormat);
STDMETHODIMP SVXStreamRead(PAVISTREAM ps, LONG lStart, LONG lSamples, LPVOID lpBuffer, LONG cbBuffer, LONG FAR * plBytes,LONG FAR * plSamples);
STDMETHODIMP SVXStreamWrite(PAVISTREAM ps, LONG lStart, LONG lSamples, LPVOID lpData, LONG cbData, DWORD dwFlags, LONG FAR *plSampWritten, LONG FAR *plBytesWritten);
STDMETHODIMP SVXStreamDelete(PAVISTREAM ps, LONG lStart, LONG lSamples);
STDMETHODIMP SVXStreamReadData(PAVISTREAM ps, DWORD fcc, LPVOID lp,LONG FAR *lpcb);
STDMETHODIMP SVXStreamWriteData(PAVISTREAM ps, DWORD fcc, LPVOID lp,LONG cb);
STDMETHODIMP SVXStreamSetInfo(PAVISTREAM ps, AVISTREAMINFOW FAR * psi, LONG lSize);

IAVIStreamVtbl StreamVtbl = {
    SVXStreamQueryInterface,
    SVXStreamAddRef,
    SVXStreamRelease,
    SVXStreamCreate,
    SVXStreamInfo,
    SVXStreamFindSample,
    SVXStreamReadFormat,
    SVXStreamSetFormat,
    SVXStreamRead,
    SVXStreamWrite,
    SVXStreamDelete,
    SVXStreamReadData,
    SVXStreamWriteData,
    SVXStreamSetInfo
};

#if !defined UNICODE

int LoadUnicodeString(HINSTANCE hinst, UINT wID, LPWSTR lpBuffer, int cchBuffer)
{
    char    ach[128];
    int	    i;

    i = LoadString(hinst, wID, ach, sizeof(ach));

    if (i > 0)
	MultiByteToWideChar(CP_ACP, 0, ach, -1, lpBuffer, cchBuffer);

    return i;
}

#else
#define LoadUnicodeString   LoadString
#endif


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

/*	-	-	-	-	-	-	-	-	*/

UINT	uUseCount;	// the reference count for our objects
UINT	uLockCount;	// our lock count for LockServer

HGLOBAL hOtherChannelDeferredBuffer = NULL; // for dealing with stereo files
BYTE _huge *lpOtherChannelDeferredBuffer;

/*	-	-	-	-	-	-	-	-	*/

//--------------------------------------------------------------------------;
//
//  UWORD ReverseWORD
//
//  Description:
//      Converts an INTEL word to a MOTOROLA word and vice
//      versa.
//
//  Arguments:
//      UWORD number:
//
//  Return (UWORD):
//
//--------------------------------------------------------------------------;

UWORD ReverseWORD(UWORD number)
{
UWORD   lo;
   
   lo = number & 0x00FF; 
   lo = lo << 8;
   
   number = number >> 8 | lo;
   
   return(number);

} // ReverseWORD()

//--------------------------------------------------------------------------;
//
//  DWORD ReverseDWORD
//
//  Description:
//      Converts an INTEL double word to a MOTOROLA double word and vice
//      versa.
//
//  Arguments:
//      DWORD number:
//
//  Return (DWORD):
//
//--------------------------------------------------------------------------;

DWORD ReverseDWORD(DWORD number)
{ 
DWORD   midhi,
		midlo,
		lo;
   
   midhi = number & 0x00FF0000; 
   midhi = midhi >> 8;

   midlo = number & 0x0000FF00; 
   midlo = midlo << 8;

   lo = number & 0x000000FF; 
   lo = lo << 24;
   
   number = number >> 24 | midhi | midlo | lo;
   
   return(number);
} // ReverseDWORD()


//--------------------------------------------------------------------------;
//
//  HRESULT SVXFileCreate
//
//  Description:
//      Create a new instance.  Since this is a C implementation we have to 
//      allocate space for our structure ourselves.
//
//  Arguments:
//      IUnknown FAR* pUnknownOuter:
//
//      REFIID riid:
//
//  Return (HRESULT):
//
//--------------------------------------------------------------------------;

HRESULT SVXFileCreate(
	IUnknown FAR*	pUnknownOuter,
	REFIID		riid,
	void FAR* FAR*	ppv)
{
	IUnknown FAR*	pUnknown;
	LPWAVESTUFF	pWaveStuff;
	HRESULT	hresult;

	// Allocate space for our structure
	pWaveStuff = (LPWAVESTUFF)GlobalAllocPtr(GMEM_MOVEABLE,
		sizeof(WAVESTUFF));
	if (!pWaveStuff)
		return ResultFromScode(E_OUTOFMEMORY);

	// Initialize the Vtbls
	pWaveStuff->AVIFile = &FileVtbl;
	pWaveStuff->AVIStream = &StreamVtbl;
	pWaveStuff->Unknown = &UnknownVtbl;
    pWaveStuff->Persist = &PersistVtbl;

	// Set up our controlling object 
	pUnknown = (IUnknown FAR *)&pWaveStuff->Unknown;
	if (pUnknownOuter)
		pWaveStuff->pUnknownOuter = pUnknownOuter;
	else
		pWaveStuff->pUnknownOuter =(IUnknown FAR *)&pWaveStuff->Unknown;

	// Initial the things in our structure
	pWaveStuff->refs = 0;
	pWaveStuff->hmmio = NULL;
	pWaveStuff->lpFormat = NULL;
	pWaveStuff->cbFormat = 0L;
	pWaveStuff->fDirty = FALSE;
	pWaveStuff->extra.lp = NULL;
	pWaveStuff->extra.cb = 0L;

	// Call our Query interface to increment our ref count and get a
	// pointer to our interface to return.
	hresult = pUnknown->lpVtbl->QueryInterface(pUnknown, riid, ppv);

	if (FAILED(GetScode(hresult)))
		GlobalFreePtr(pWaveStuff);
	return hresult;
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXUnknownQueryInterface
//
//  Description:
//      Query interface from all three interfaces comes here.  We support the
//      Unknown interface, AVIStream and AVIFile.
//
//  Arguments:
//      LPUNKNOWN pu:
//
//      REFIID iid:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXUnknownQueryInterface(
	LPUNKNOWN	pu,
	REFIID		iid,
	void FAR* FAR*	ppv)
{
	// Get a pointer to our structure
	LPWAVESTUFF pWaveStuff = WAVESTUFF_FROM_UNKNOWN(pu);

	if (IsEqualIID(iid, &IID_IUnknown))
		*ppv = &pWaveStuff->Unknown;
	else if (IsEqualIID(iid, &IID_IAVIFile))
		*ppv = &pWaveStuff->AVIFile;
	else if (IsEqualIID(iid, &IID_IAVIStream))
		*ppv = &pWaveStuff->AVIStream;
	else if (IsEqualIID(iid, &IID_IPersistFile))
		*ppv = &pWaveStuff->Persist;
    else
		return ResultFromScode(E_NOINTERFACE);
	pu->lpVtbl->AddRef(pu);
	return NOERROR;
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP_(ULONG) SVXUnknownAddRef
//
//  Description:
//      Increase our reference count.  AddRef for all three interfaces comes here.
//
//  Arguments:
//      LPUNKNOWN pu:
//
//  Return (STDMETHODIMP_(ULONG)):
//
//--------------------------------------------------------------------------;

STDMETHODIMP_(ULONG) SVXUnknownAddRef(
	LPUNKNOWN	pu)
{
	// Get a pointer to our structure
	LPWAVESTUFF pWaveStuff = WAVESTUFF_FROM_UNKNOWN(pu);

	uUseCount++;
	return ++pWaveStuff->refs;
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP_(ULONG) SVXUnknownRelease
//
//  Description:
//      Decrease our reference count.  Release for all three interfaces 
//      comes here.  This is where we write out the header data for the 
//      SVX file.
//
//  Arguments:
//      LPUNKNOWN pu:
//
//  Return (STDMETHODIMP_(ULONG)):
//
//--------------------------------------------------------------------------;

STDMETHODIMP_(ULONG) SVXUnknownRelease(LPUNKNOWN pu)
{
    // Get a pointer to our structure
    LPWAVESTUFF p = WAVESTUFF_FROM_UNKNOWN(pu);

    uUseCount--;

    //
    // Ref count is zero.  Close the file.  If we've been writing to it, it's
    // clean-up time!
    //
    if (!--p->refs) 
    {
	    LONG lRet = AVIERR_OK;
	
    	if (p->fDirty) 
        {
	        MMCKINFO ckFORM;
	        MMCKINFO ck;
            VHDR     vhdr;

            //
            // Now write out all of that data that we have been storing up for 
            // stereo files
            //
            if (2 == p->lpFormat->nChannels)
            {
                mmioSeek(p->hmmio, p->ckData.dwDataOffset + p->avistream.dwLength,
                        SEEK_SET);

                if (mmioWrite(p->hmmio, (HPSTR) lpOtherChannelDeferredBuffer, 
                        p->avistream.dwLength) != (LONG)p->avistream.dwLength)
		            goto ERROR_CANNOT_WRITE;	// cannot write file, probably

            }

	        mmioSeek(p->hmmio, 0, SEEK_SET);

            //
            // Write out the 'FORM' chunk id
            //
            ckFORM.ckid = ckidForm;
            if (mmioWrite(p->hmmio, (HPSTR)&ckFORM.ckid, sizeof(ckFORM.ckid))
                    != sizeof(ckFORM.ckid))
		        goto ERROR_CANNOT_WRITE;	// cannot write file, probably
            
            //
            // Compute the size of the file and write it out
            //
            if (1 == p->lpFormat->nChannels)
                ckFORM.cksize = ReverseDWORD(p->ckData.cksize + 
                                    (2 * sizeof(ck.ckid)) + sizeof(DWORD) +
                                    sizeof(VHDR));
            else
                ckFORM.cksize = ReverseDWORD(p->ckData.cksize +
                                    (2 * sizeof(ck.ckid)) + sizeof(DWORD) +
                                    sizeof(VHDR) + sizeof(ck.ckid) +
                                    (2 * sizeof(DWORD)) );

            if (mmioWrite(p->hmmio, (HPSTR)&ckFORM.cksize, 
                    sizeof(ckFORM.cksize)) != sizeof(ckFORM.cksize))
		        goto ERROR_CANNOT_WRITE;	// cannot write file, probably
                
            //
            // Write out the '8SVX' chunk id
            //
            ck.ckid = ckidSVX;
            if (mmioWrite(p->hmmio, (HPSTR)&ck.ckid, sizeof(ck.ckid))
                    != sizeof(ck.ckid))
		        goto ERROR_CANNOT_WRITE;	// cannot write file, probably

            //
            // Write out the 'VHDR' chunk id
            //
            ck.ckid = ckidVHDR;
            if (mmioWrite(p->hmmio, (HPSTR)&ck.ckid, sizeof(ck.ckid))
                    != sizeof(ck.ckid))
		        goto ERROR_CANNOT_WRITE;	// cannot write file, probably
                
            //
            // Write out the size and VHDR structure
            //
            ck.cksize = ReverseDWORD(sizeof(VHDR));
            if (mmioWrite(p->hmmio, (HPSTR)&ck.cksize, sizeof(ck.cksize))
                    != sizeof(ck.cksize))
		        goto ERROR_CANNOT_WRITE;	// cannot write file, probably

            vhdr.oneShotHiSamples  = ReverseDWORD(p->ckData.cksize);
            vhdr.repeatHiSamples   = 0L;
            vhdr.samplesPerHiCycle = 0L;
            vhdr.samplesPerSec     = ReverseWORD((UWORD)(p->lpFormat->nSamplesPerSec));
            vhdr.ctOctave          = 1;
            vhdr.sCompression      = 0;
            vhdr.volume            = 0x10000L;
            if (mmioWrite(p->hmmio, (HPSTR)&vhdr, sizeof(vhdr)) 
                    != sizeof(vhdr))
		        goto ERROR_CANNOT_WRITE;	// cannot write file, probably
                
            //
            // Take care of the 'CHAN' chunk if needed
            //
            if (1 != p->lpFormat->nChannels)
            {
                DWORD dwStereo;

                ck.ckid = ckidCHAN;
                if (mmioWrite(p->hmmio, (HPSTR)&ck.ckid, sizeof(ck.ckid))
                        != sizeof(ck.ckid))
		            goto ERROR_CANNOT_WRITE;	// cannot write file, probably

                ck.cksize = ReverseDWORD(sizeof(DWORD));
                if (mmioWrite(p->hmmio, (HPSTR)&ck.cksize, sizeof(ck.cksize))
                        != sizeof(ck.cksize))
    		        goto ERROR_CANNOT_WRITE;	// cannot write file, probably

                dwStereo = ReverseDWORD(SVX_IFF_STEREO);
                if (mmioWrite(p->hmmio, (HPSTR)&dwStereo, sizeof(dwStereo))
                        != sizeof(dwStereo))
		            goto ERROR_CANNOT_WRITE;	// cannot write file, probably

            }

            //
            // Write out the 'BODY' chunk id and size
            //
            ck.ckid = ckidBODY;
            if (mmioWrite(p->hmmio, (HPSTR)&ck.ckid, sizeof(ck.ckid))
                    != sizeof(ck.ckid))
                goto ERROR_CANNOT_WRITE;	// cannot write file, probably

            ck.cksize = ReverseDWORD(p->ckData.cksize);
            if (mmioWrite(p->hmmio, (HPSTR)&ck.cksize, sizeof(ck.cksize))
                    != sizeof(ck.cksize))
                goto ERROR_CANNOT_WRITE;	// cannot write file, probably

	        if (mmioFlush(p->hmmio, 0) != 0)
		        goto ERROR_CANNOT_WRITE;
    	}

	    goto success;

        ERROR_CANNOT_WRITE:
	    lRet = AVIERR_FILEWRITE;

success:
	    if (p->hmmio)
	        mmioClose(p->hmmio, 0);

	    if (p->lpFormat)
	        GlobalFreePtr(p->lpFormat);

        if (NULL != hOtherChannelDeferredBuffer)
        {
            if (GlobalUnlock(hOtherChannelDeferredBuffer) 
                    || GlobalFree(hOtherChannelDeferredBuffer))
		        goto ERROR_CANNOT_WRITE;

            hOtherChannelDeferredBuffer = NULL;
        }

	    // Free the memory for our structure.
	    GlobalFreePtr(p);
	    return 0;
    }

    return p->refs;
}


//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXFileQueryInterface
//
//  Description:
//      Use our controlling object to call QueryInterface on Unknown
//
//  Arguments:
//      PAVIFILE pf:
//
//      REFIID iid:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXFileQueryInterface(
	PAVIFILE	pf,
	REFIID		iid,
	void FAR* FAR*	ppv)
{
	// Get a pointer to our structure
	LPWAVESTUFF pWaveStuff = WAVESTUFF_FROM_FILE(pf);

	return pWaveStuff->pUnknownOuter->lpVtbl->QueryInterface(
		pWaveStuff->pUnknownOuter, iid, ppv);
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP_(ULONG) SVXFileAddRef
//
//  Description:
//      Use our controlling object to call AddRef on Unknown
//
//  Arguments:
//      PAVIFILE pf:
//
//  Return (STDMETHODIMP_(ULONG)):
//
//--------------------------------------------------------------------------;

STDMETHODIMP_(ULONG) SVXFileAddRef(
	PAVIFILE	pf)
{
	// Get a pointer to our structure
	LPWAVESTUFF pWaveStuff = WAVESTUFF_FROM_FILE(pf);

	return pWaveStuff->pUnknownOuter->lpVtbl->AddRef(
		pWaveStuff->pUnknownOuter);
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP_(ULONG) SVXFileRelease
//
//  Description:
//      Use our controlling object to call Release on Unknown
//
//  Arguments:
//      PAVIFILE pf:
//
//  Return (STDMETHODIMP_(ULONG)):
//
//--------------------------------------------------------------------------;

STDMETHODIMP_(ULONG) SVXFileRelease(
	PAVIFILE	pf)
{
	// Get a pointer to our structure
	LPWAVESTUFF pWaveStuff = WAVESTUFF_FROM_FILE(pf);

	return pWaveStuff->pUnknownOuter->lpVtbl->Release(
		pWaveStuff->pUnknownOuter);
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXPersistQueryInterface
//
//  Description:
//      Use our controlling object to call QueryInterface on Unknown
//
//  Arguments:
//      LPPERSISTFILE ppf:
//
//      REFIID iid:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXPersistQueryInterface(
	LPPERSISTFILE	ppf,
	REFIID		iid,
	void FAR* FAR*	ppv)
{
	// Get a pointer to our structure
	LPWAVESTUFF pWaveStuff = WAVESTUFF_FROM_PERSIST(ppf);

	return pWaveStuff->pUnknownOuter->lpVtbl->QueryInterface(
		pWaveStuff->pUnknownOuter, iid, ppv);
}


//--------------------------------------------------------------------------;
//
//  STDMETHODIMP_(ULONG) SVXPersistAddRef
//
//  Description:
//      Use our controlling object to call AddRef on Unknown
//
//  Arguments:
//      ULONG:
//
//  Return (STDMETHODIMP_(ULONG)):
//
//--------------------------------------------------------------------------;

STDMETHODIMP_(ULONG) SVXPersistAddRef(
	LPPERSISTFILE	ppf)
{
	// Get a pointer to our structure
	LPWAVESTUFF pWaveStuff = WAVESTUFF_FROM_PERSIST(ppf);

	return pWaveStuff->pUnknownOuter->lpVtbl->AddRef(
		pWaveStuff->pUnknownOuter);
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP_(ULONG) SVXPersistRelease
//
//  Description:
//      Use our controlling object to call Release on Unknown
//
//  Arguments:
//      ULONG:
//
//  Return (STDMETHODIMP_(ULONG)):
//
//--------------------------------------------------------------------------;

STDMETHODIMP_(ULONG) SVXPersistRelease(
	LPPERSISTFILE	ppf)
{
	// Get a pointer to our structure
	LPWAVESTUFF pWaveStuff = WAVESTUFF_FROM_PERSIST(ppf);

	return pWaveStuff->pUnknownOuter->lpVtbl->Release(
		pWaveStuff->pUnknownOuter);
}


//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXStreamQueryInterface
//
//  Description:
//      Use our controlling object to call QueryInterface on Unknown
//
//  Arguments:
//      PAVISTREAM ps:
//
//      REFIID iid:
//
//      void FAR* FAR* ppv:        
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXStreamQueryInterface(
	PAVISTREAM	ps,
	REFIID		iid,
	void FAR* FAR*	ppv)
{
	// Get a pointer to our structure
	LPWAVESTUFF pWaveStuff = WAVESTUFF_FROM_STREAM(ps);

	return pWaveStuff->pUnknownOuter->lpVtbl->QueryInterface(
		pWaveStuff->pUnknownOuter, iid, ppv);
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP_(ULONG) SVXStreamAddRef
//
//  Description:
//      Use our controlling object to call AddRef on Unknown
//
//  Arguments:
//      PAVISTREAM ps:
//
//  Return (STDMETHODIMP_(ULONG)):
//
//--------------------------------------------------------------------------;

STDMETHODIMP_(ULONG) SVXStreamAddRef(
	PAVISTREAM	ps)
{
	// Get a pointer to our structure
	LPWAVESTUFF pWaveStuff = WAVESTUFF_FROM_STREAM(ps);

	return pWaveStuff->pUnknownOuter->lpVtbl->AddRef(
		pWaveStuff->pUnknownOuter);
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP_(ULONG) SVXStreamRelease
//
//  Description:
//      Use our controlling object to call Release on Unknown
//
//  Arguments:
//      PAVISTREAM ps:
//
//  Return (STDMETHODIMP_(ULONG)):
//
//--------------------------------------------------------------------------;

STDMETHODIMP_(ULONG) SVXStreamRelease(PAVISTREAM	ps)
{
	// Get a pointer to our structure
	LPWAVESTUFF pWaveStuff = WAVESTUFF_FROM_STREAM(ps);

	return pWaveStuff->pUnknownOuter->lpVtbl->Release(
		pWaveStuff->pUnknownOuter);
}

/*	-	-	-	-	-	-	-	-	*/

#define SLASH(c)     ((c) == '/' || (c) == '\\')

/*--------------------------------------------------------------+
| FileName  - return a pointer to the filename part of szPath   |
|             with no preceding path.                           |
+--------------------------------------------------------------*/
LPSTR FAR FileName(LPCSTR lszPath)
{
    LPCSTR   lszCur;

    for (lszCur = lszPath + lstrlen(lszPath); lszCur > lszPath && !SLASH(*lszCur) && *lszCur != ':';)
	
    lszCur = AnsiPrev(lszPath, lszCur);

    if (lszCur == lszPath)
    	return (LPSTR)lszCur;
    else
	    return (LPSTR)(lszCur + 1);
}


//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXFileOpen
//
//  Description:
//      The Open Method for our File interface - Open a WAVE file
//
//  Arguments:
//      PAVIFILE pf:
//
//      LPCSTR szFile:
//
//      UINT mode:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXFileOpen(PAVIFILE pf, LPCSTR szFile, UINT mode)
{
    LPWAVESTUFF     p = WAVESTUFF_FROM_FILE(pf);
    UINT	        ui;
    char	        ach[80];
    LPVHDR          lpVHDR = NULL;

    // !!! Assumptions about the AVIFILE.DLL (which calls us):
    // We will only see READWRITE mode, never only WRITE mode.

    // force the share flags to the 'correct' values
    // If we're writing, use Exclusive mode.  If we're reading, use DenyWrite.
    if (mode & OF_READWRITE) 
    {
	    mode = (mode & ~(MMIO_SHAREMODE)) | OF_SHARE_EXCLUSIVE;
    } 
    else 
    {
	    mode = (mode & ~(MMIO_SHAREMODE)) | OF_SHARE_DENY_WRITE;
    }

    //
    // try to open the actual file, first with share, then without.  
    // You may need to use specific flags in order to open a file
    // that's already open by somebody else.
    //

    // If the first attempt fails, no system error box, please.
    ui = SetErrorMode(SEM_NOOPENFILEERRORBOX);
    p->hmmio = mmioOpen((LPSTR) szFile, NULL, MMIO_ALLOCBUF | mode);
    if (!p->hmmio && ((mode & MMIO_RWMODE) == OF_READ)) 
    {
	    // if the open fails, try again without the share flags.
	    mode &= ~(MMIO_SHAREMODE);
	    p->hmmio = mmioOpen((LPSTR) szFile, NULL, MMIO_ALLOCBUF | mode);
    }
    SetErrorMode(ui);
    
    //
    // Now set up our structure
    //
    p->mode = mode;
    
    if (!p->hmmio)
        goto error;

    _fmemset(&p->avistream, 0, sizeof(p->avistream));

// If this is defined, we pretend that the data is at FPSHACK "frames"
// per second in the main header, otherwise we use the sample
// rate of the audio, which looks somewhat strange in MPlayer.
#define FPSHACK	    1000
    
    _fmemset(&p->avihdr, 0, sizeof(p->avihdr));

#ifdef FPSHACK
    //
    // Initialize our AVIFILEHEADER
    //
    p->avihdr.dwRate = FPSHACK;
    p->avihdr.dwScale = 1;
#endif
    
    p->avihdr.dwStreams = 1;
    LoadUnicodeString(ghModule, IDS_FILETYPE, p->avihdr.szFileType,
    	sizeof(p->avihdr.szFileType));
    
    //
    // Initialize our AVISTREAMHEADER
    //
    LoadString(ghModule, IDS_STREAMNAME, ach, sizeof(ach));
#if !defined UNICODE
	{
	    char    achTemp[64];

	    wsprintf(achTemp, ach, FileName(szFile));

	    MultiByteToWideChar(CP_ACP, 0, achTemp, -1,
			p->avistream.szName, 64);
	}
#else
    wsprintf(p->avistream.szName, ach, FileName(szFile));
#endif

    if (mode & OF_CREATE) 
    {	// Brand new file
	    p->avistream.fccType = streamtypeAUDIO;
	    p->avistream.fccHandler = 0;
	    p->avistream.dwFlags = 0;
	    p->avistream.wPriority = 0;
	    p->avistream.wLanguage = 0;
	    p->avistream.dwInitialFrames = 0;
	    p->avistream.dwScale = 0;
	    p->avistream.dwRate = 0;
	    p->avistream.dwStart = 0;
	    p->avistream.dwLength = 0;
	    p->avistream.dwSuggestedBufferSize = 0;
	    p->avistream.dwSampleSize = 0;
	
	    p->fDirty = TRUE;
	
    } 
    else 
    {	
	    //
        // do the actual reading of the SVX IFF File
        //
	    MMCKINFO        ck;
        DWORD           cbFORM;
        DWORD           cbVHDR;
        DWORD           cbBODY;
        DWORD           dwRead;
        WORD            nChannelsRead = 1;  // Set this to default to 1

	    //
	    //  Read FORM chunk, if it opens we know that we have an IFF file
	    //
        ck.ckid = ckidForm;
	    if (mmioDescend(p->hmmio, &ck, NULL, MMIO_FINDCHUNK) != 0)
	        goto error;

        //
        //  We need to reverse the cksize to Intel format
        //
        cbFORM = ReverseDWORD(ck.cksize);

        if (mmioRead(p->hmmio, (HPSTR) &dwRead, sizeof(dwRead)) 
                != sizeof(dwRead))
            goto error;

        //
        //  Check to see if this is an IFF sound file
        //
	    if (dwRead != ckidSVX)
	        goto error;

        //
        //  Now get the VHDR from the IFF file
        //
        ck.ckid = ckidVHDR;

	    //
	    //  Read VHDR chunk to get the size of the structure
	    //
	    if (mmioDescend(p->hmmio, &ck, NULL, MMIO_FINDCHUNK) != 0)
	        goto error;

        //
        //  We need to reverse the cksize to Intel format
        //
        cbVHDR = ReverseDWORD(ck.cksize);

        //
        //  Allocate VHDR and read into structure
        //
        lpVHDR = (LPVHDR) GlobalAllocPtr(GMEM_MOVEABLE, cbVHDR);
	    if (lpVHDR == NULL)
	        goto error;
        
        if (mmioRead(p->hmmio,
		        (HPSTR) lpVHDR,
		        (LONG)cbVHDR) != (LONG)cbVHDR)
	        goto error;

        //
        //  Abort if it has weirdo data in the header
        //
        if ((lpVHDR->ctOctave > 1) || (lpVHDR->sCompression != 0))
            goto error;
            
        //
	    //  Read until I find the BODY chunk.  If I find the CHAN chunk in
        //  between then read the data in there.
	    //
        while (cbFORM + 2 * sizeof(FOURCC) > 
                (DWORD)mmioSeek(p->hmmio, 0, SEEK_CUR))
        {
            DWORD dwckSize;

            //
            //  Read chunk id and size
            //
            if (mmioRead(p->hmmio, (HPSTR) &dwRead, sizeof(dwRead)) 
                    != sizeof(dwRead))
                goto error;

            if (mmioRead(p->hmmio, (HPSTR) &dwckSize, sizeof(dwckSize)) 
                    != sizeof(dwckSize))
                goto error;

            //
            //  Deal with the chunks that I want
            //
            switch (dwRead)
            {
                case ckidCHAN:
                {
                    //
                    //  We don't care about the size of this chunk, we just 
                    //  want the data
                    //  
                    if (mmioRead(p->hmmio, (HPSTR) &dwRead, sizeof(dwRead)) 
                            != sizeof(dwRead))
                        goto error;

                    //
                    //  Set channels to stereo if I find a 6 in the CHAN 
                    //  chunk of the IFF
                    //
                    if (SVX_IFF_STEREO == ReverseDWORD(dwRead))
                        nChannelsRead = 2;
                    else
                        nChannelsRead = 1;

                }
                break;
                
                case ckidBODY:
                {
                    //
                    //  We need to reverse the cksize to Intel format
                    //
                    cbBODY = ReverseDWORD(dwckSize);

                    p->ckData.ckid         = ckidBODY;
                    p->ckData.cksize       = cbBODY;
                    p->ckData.dwDataOffset = mmioSeek(p->hmmio, 0, SEEK_CUR);

                }
                break;

                //
                //  Keep searching for chunks
                //
                default:
	                mmioSeek(p->hmmio, ReverseDWORD(dwckSize), SEEK_CUR);
            }
        }

        //
        //  This file is always going to be converted to PCM so only allocate
        //  the PCMWAVEFORMAT structure.  I will stuff this structure with
        //  the IFF data that I read.
        //
        p->cbFormat = sizeof(PCMWAVEFORMAT);
	    p->lpFormat = (LPWAVEFORMATEX) GlobalAllocPtr(GMEM_MOVEABLE, p->cbFormat);

	    if (p->lpFormat == NULL)
	        goto error;

        //
        //  Copy info from VHDR structure to PCMWAVEFORMAT structure
        //
        p->lpFormat->wFormatTag      = WAVE_FORMAT_PCM;
        p->lpFormat->nChannels       = nChannelsRead;
        p->lpFormat->nSamplesPerSec  = ReverseWORD(lpVHDR->samplesPerSec);
        p->lpFormat->nAvgBytesPerSec = p->lpFormat->nSamplesPerSec *
                                       p->lpFormat->nChannels;
        p->lpFormat->nBlockAlign     = p->lpFormat->nChannels;

        p->lpFormat->wBitsPerSample  = SVX_IFF_BITSPERSAMPLE;
        p->lpFormat->cbSize          = 0L;

	    
        //
        //  Done using this, might as well get rid of it now
        //
        GlobalFreePtr(lpVHDR);


	    p->fDirty = FALSE;
	
	    p->avistream.fccType = streamtypeAUDIO;
	    p->avistream.fccHandler = 0;
	    p->avistream.dwFlags = 0;
	    p->avistream.wPriority = 0;	       
	    p->avistream.wLanguage = 0;	       
	    p->avistream.dwInitialFrames = 0;       
	    p->avistream.dwScale = p->lpFormat->nBlockAlign;
	    p->avistream.dwRate = p->lpFormat->nAvgBytesPerSec;
	    p->avistream.dwStart = 0;
	    p->avistream.dwLength = p->ckData.cksize / p->lpFormat->nBlockAlign;
	    p->avistream.dwSuggestedBufferSize = 0;
	    p->avistream.dwSampleSize = p->lpFormat->nBlockAlign;
	
#ifdef FPSHACK
	    p->avihdr.dwLength =
                    MulDiv32(p->avistream.dwLength * FPSHACK,
				    p->avistream.dwScale,
				    p->avistream.dwRate);
#else
	    p->avihdr.dwScale = 1;
	    p->avihdr.dwRate = p->lpFormat->wf.nSamplesPerSec;
	    p->avihdr.dwLength =
                    MulDiv32(p->ckData.cksize, p->lpFormat->nSamplesPerSec,
			    p->lpFormat->nAvgBytesPerSec);
			 
#endif

    }
    
    //
    // all done return success.
    //
    return ResultFromScode(0); // success
    
error:
	if (lpVHDR)
	    GlobalFreePtr(lpVHDR);

    return ResultFromScode(AVIERR_FILEREAD);
} 


//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXFileGetStream
//
//  Description:
//      Get a stream from the file... Each SVX file has exactly 1 audio 
//      stream.
//
//  Arguments:
//      PAVIFILE pf:
//
//      PAVISTREAM FAR *ppavi:
//
//      DWORD fccType:
//
//      LONG lParam:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXFileGetStream
(
    PAVIFILE        pf,
	PAVISTREAM FAR  *ppavi,
	DWORD           fccType,
	LONG            lParam
)
{
    int             iStreamWant;
    // Get a pointer to our structure
    LPWAVESTUFF p = WAVESTUFF_FROM_FILE(pf);

    iStreamWant = (int)lParam;

    if (p->lpFormat == NULL)
	return ResultFromScode(AVIERR_BADPARAM);
    
    // We only support one stream
    if (iStreamWant != 0)
        return ResultFromScode(AVIERR_BADPARAM);

    // We only support audio streams
    if (fccType && fccType != streamtypeAUDIO)
	    return ResultFromScode(AVIERR_BADPARAM);

    // increase the reference count
    p->AVIStream->AddRef((PAVISTREAM)&p->AVIStream);
    
    // Return a pointer to our stream Vtbl
    *ppavi = (PAVISTREAM) &(p->AVIStream);

    return ResultFromScode(AVIERR_OK);
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXFileDeleteStream
//
//  Description:
//
//
//  Arguments:
//      PAVIFILE pf:
//
//      DWORD fccType:
//
//      LONG lParam:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXFileDeleteStream(PAVIFILE pf, DWORD fccType, LONG lParam)
{
	int iStreamWant;
	// Get a pointer to our structure
	LPWAVESTUFF p = WAVESTUFF_FROM_FILE(pf);

	iStreamWant = (int)lParam;

	if (p->lpFormat == NULL)
		return ResultFromScode(AVIERR_BADPARAM);
	
	// We only support one stream
	if (iStreamWant != 0)
		return ResultFromScode(AVIERR_BADPARAM);

	// We only support audio streams
	if (fccType && fccType != streamtypeAUDIO)
		return ResultFromScode(AVIERR_BADPARAM);


	GlobalFreePtr(p->lpFormat);
	p->lpFormat = NULL;

	return NOERROR;
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXFileSave
//
//  Description:
//      We don't support the Save Method of the File Interface (We don't save)
//
//  Arguments:
//      PAVIFILE pf:
//
//      LPCSTR szFile:
//
//      AVICOMPRESSOPTIONS FAR *lpOptions:
//
//      AVISAVECALLBACK lpfnCallback:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXFileSave
(   
    PAVIFILE                pf,
	LPCSTR                  szFile,
	AVICOMPRESSOPTIONS FAR  *lpOptions,
	AVISAVECALLBACK         lpfnCallback
)
{
    return ResultFromScode(AVIERR_UNSUPPORTED);
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXFileCreateStream
//
//  Description:
//      Method to create a stream in a SVX file.  We only support this for 
//      blank SVX files.
//
//  Arguments:
//      PAVIFILE pf:
//
//      PAVISTREAM FAR *ppstream:
//
//      AVISTREAMINFOW FAR *psi:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXFileCreateStream(
    PAVIFILE pf,
	PAVISTREAM FAR *ppstream,
	AVISTREAMINFOW FAR *psi)
{
    // Get a pointer to our structure
    LPWAVESTUFF p = WAVESTUFF_FROM_FILE(pf);

    // We can't add a second stream to a file
    if (p->lpFormat)
	    return ResultFromScode(AVIERR_UNSUPPORTED);

    // We only like audio....
    if (psi->fccType != streamtypeAUDIO)
	    return ResultFromScode(AVIERR_UNSUPPORTED);
    
    // Increase our reference count.
    p->AVIStream->AddRef((PAVISTREAM)&p->AVIStream);

    p->cbFormat = 0;
    p->lpFormat = NULL;

    // Return a pointer to our stream Vtbl.
    *ppstream = (PAVISTREAM) &(p->AVIStream);
    
    return ResultFromScode(AVIERR_OK);
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXFileWriteData
//
//  Description:
//      The WriteData Method of the File interface
//
//  Arguments:
//      PAVIFILE pf:
//
//      DWORD ckid:
//
//      LPVOID lpData:
//
//      LONG cbData:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXFileWriteData(PAVIFILE pf,
		DWORD ckid,
		LPVOID lpData,
		LONG cbData)
{
    return ResultFromScode(AVIERR_UNSUPPORTED);
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXFileReadData
//
//  Description:
//      The ReadData Method of the File interface
//
//  Arguments:
//      PAVIFILE pf:
//
//      DWORD ckid:
//
//      LPVOID lpData:
//
//      LONG FAR *lpcbData:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXFileReadData
(
    PAVIFILE    pf,
    DWORD       ckid,
    LPVOID      lpData,
    LONG FAR    *lpcbData
)
{
    return ResultFromScode(AVIERR_UNSUPPORTED);
}


//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXFileEndRecord
//
//  Description:
//      The EndRecord Method of the File interface.. this doesn't need to do
//      anything.. (no concept of interleaving or packaging streams)
//
//  Arguments:
//      PAVIFILE pf:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXFileEndRecord(PAVIFILE pf)
{
    return ResultFromScode(AVIERR_OK);
}


//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXFileInfo
//
//  Description:
//      The Info Method of the File interface
//
//  Arguments:
//      PAVIFILE pf:
//
//      AVIFILEINFOW FAR *pfi:
//
//      LONG lSize:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXFileInfo
(
    PAVIFILE            pf,
	AVIFILEINFOW FAR    *pfi,
	LONG                lSize
)
{
    // Get a pointer to our structure
    LPWAVESTUFF p = WAVESTUFF_FROM_FILE(pf);

    // Return an AVIFILEHEADER.
    hmemcpy(pfi, &p->avihdr, min(lSize, sizeof(p->avihdr)));
    return 0;
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXStreamCreate
//
//  Description:
//      The Create Method of the Stream interface. We can't create streams 
//      that aren't attached to the file.
//
//  Arguments:
//      PAVISTREAM ps:
//
//      LONG lParam1:
//
//      LONG lParam2:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXStreamCreate
(
	PAVISTREAM	ps,
	LONG        lParam1, 
    LONG        lParam2
)
{
    return ResultFromScode(AVIERR_UNSUPPORTED);
}


//--------------------------------------------------------------------------;
//
//  STDMETHODIMP_(LONG) SVXStreamFindSample
//
//  Description:
//      The FindSample Method of the Stream interface
//
//
//  Arguments:
//      PAVISREAM ps:
//
//      LONG lPos:
//
//      LONG lFlags:
//
//  Return (STDMETHODIMP_(LONG)):
//
//--------------------------------------------------------------------------;

STDMETHODIMP_(LONG) SVXStreamFindSample
(
	PAVISTREAM	ps,
	LONG        lPos, 
    LONG        lFlags
)
{
    if (lFlags & FIND_FORMAT) 
    {
        if ((lFlags & FIND_NEXT) && lPos > 0)
            return -1;
        else
            return 0;
    }

    return lPos;    
}


//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXStreamReadFormat
//
//  Description:
//      The ReadFormat Method of the Stream interface
//
//  Arguments:
//      PAVISTREAM ps:
//
//      LONG lPos:
//
//      LPVOID lpFormat:
//
//      LONG FAR *lpcbFormat:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXStreamReadFormat
(
	PAVISTREAM	ps,
	LONG        lPos, 
    LPVOID      lpFormat, 
    LONG FAR    *lpcbFormat
)
{
    // Get a pointer to our structure
    LPWAVESTUFF p = WAVESTUFF_FROM_STREAM(ps);

    // No buffer to fill in, this means return the size needed.
    if (lpFormat == NULL || *lpcbFormat == 0) 
    {
        *lpcbFormat = p->cbFormat;
	    return 0;
    }

    // Give them the WAVE format.
    hmemcpy(lpFormat, p->lpFormat, p->cbFormat);

    // Our buffer is too small
    if (*lpcbFormat < p->cbFormat)
        return ResultFromScode(AVIERR_BUFFERTOOSMALL);
    
    *lpcbFormat = p->cbFormat;

    return 0;
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXStreamInfo
//
//  Description:
//      The Info Method of the Stream interface
//
//  Arguments:
//      PAVISTREAM ps:
//
//      AVISTREAMINFOW FAR * psi:
//
//      LONG lSize:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXStreamInfo(
	PAVISTREAM	ps,
	AVISTREAMINFOW FAR * psi,
	LONG lSize)
{
    // Get a pointer to our structure
    LPWAVESTUFF p = WAVESTUFF_FROM_STREAM(ps);

    // give them an AVISTREAMINFO
    hmemcpy(psi, &p->avistream, min(lSize, sizeof(p->avistream)));
    return 0;
}

STDMETHODIMP SVXStreamSetInfo(PAVISTREAM ps, AVISTREAMINFOW FAR * psi, LONG lSize)
{
	return ResultFromScode(AVIERR_UNSUPPORTED);
}

///////////////////////////////////////////////////////////////////////////



//--------------------------------------------------------------------------;
//
//  void SVXInterleaveChannels
//
//  Description:
//      In the SVX IFF format stereo files are implemented by a continuous
//      stream for each channel.  This function will take the two buffers
//      and alternate samples from each buffer to put them in the RIFF
//      WAVE format on interleaving channels.
//
//  Arguments:
//      HPSTR lpMainBuffer:
//
//      LONG cbMainBuffer:
//
//      HPSTR lpOtherBuffer:
//
//      LONG cbOtherBuffer:
//
//  Return (void):
//
//--------------------------------------------------------------------------;

void SVXInterleaveChannels
(
    HPSTR   lpMainBuffer,
    LONG    cbMainBuffer,   
    HPSTR   lpOtherBuffer,
    LONG    cbOtherBuffer
)
{
    LONG    i,  // Current position in the other buffer
            j,  // Current position in the main buffer
            k;  // Current position in the completed buffer

    i = cbOtherBuffer - 1L;
    j = (cbMainBuffer / 2L) - 1L;
    k = cbMainBuffer - 1L;

    //
    //  Decrement main and other buffer postions by one and completed buffer
    //  position by 2
    //
    for (; i >= 0 ; i--, j--, k = k - 2)
    {
        *(lpMainBuffer + k)       = *(lpOtherBuffer + i) ^ 128;  // Copy from other
        *(lpMainBuffer + (k - 1)) = *(lpMainBuffer + j) ^ 128; // Copy from main
    }

}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXStreamRead
//
//  Description:
//      The Read Method for the Stream Interface - Read some wave data
//      This is where the retrieving of audio data from the SVX file is done.
//
//	    invalid lPos return error
//
//	    if lPos + lSamples is invalid trim lSamples to fit.
//
//	    lpBuffer == NULL
//
//		    cbBuffer == 0 && lSamples > 0
//			    return size of lSamples sample.
//		    else
//			    return the exactly the number of bytes and sample
//			    you would have read if lpBuffer was not zero.
//
//		    NOTE return means fill in *plBytes and *plSamples.
//
//	    lpBuffer != NULL
//
//		    lSamples == -1      read convenient amount (just fill buffer)
//		    lSamples == 0       fill buffer with as many samples that will fit.
//		    lSamples >  0       read lSamples (or as much will fit in cbBuffer)
//
//		    fill in *plBytes   with bytes actualy read
//		    fill in *plSamples with samples actualy read
//
//  Arguments:
//      PAVISTREAM ps:
//
//      LONG lStart:
//
//      LONG lSamples:
//
//      LPVOID lpBuffer:
//
//      LONG cbBuffer:
//
//      LONG FAR * plBytes:
//
//      LONG FAR * plSamples:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXStreamRead
(
    PAVISTREAM ps,
    LONG       lStart,
    LONG       lSamples,
    LPVOID     lpBuffer,
    LONG       cbBuffer,
    LONG FAR * plBytes,
    LONG FAR * plSamples
)
{
    // Get a pointer to our structure
    LPWAVESTUFF p = WAVESTUFF_FROM_STREAM(ps);

    LONG          lSampleSize;
    LONG          lSeek;
    LONG          lRead;

    // Invalid position
    if (lStart < 0 || lStart > (LONG) p->avistream.dwLength) 
    {
ack:
	    if (plBytes)
	        *plBytes = 0;

	    if (plSamples)
	        *plSamples = 0;

	    return 0;
    }
    
    // Can't read quite this much data
    if (lSamples + lStart > (LONG) p->avistream.dwLength)
    	lSamples = p->avistream.dwLength - lStart;
    
    lSampleSize = p->avistream.dwSampleSize;

    // We have fixed-length samples, if lSamples is zero, just fill the buffer.
    if (lSamples > 0)
	    lSamples = min(lSamples, cbBuffer / lSampleSize);
    else
	    lSamples = cbBuffer / lSampleSize;

    //
    // a NULL buffer means return the size buffer needed to read
    // the given sample.
    //
    if (lpBuffer == NULL || cbBuffer == 0) 
    {
	    if (plBytes)
	        *plBytes =  lSamples * lSampleSize;

	    if (plSamples)
	        *plSamples = lSamples;

	    return 0;
    }

    // Buffer too small!
    if (cbBuffer < lSampleSize)
        goto ack;

    // Seek and read

    cbBuffer = lSamples * lSampleSize;

    //
    // If this is a stereo file I have to read the left channel and then the
    // right channel and then interleave the two buffers together
    //
    if (2 == p->lpFormat->nChannels)
    {
        LPVOID  lpOtherChannelBuffer;    


        lSeek = p->ckData.dwDataOffset + (lSampleSize / 2) * lStart;
        lRead = lSamples * (lSampleSize / 2);

        //
        //  Get the first channel of audio
        //
        if (mmioSeek(p->hmmio, lSeek, SEEK_SET) != lSeek)
            goto ack;

        if (mmioRead(p->hmmio, (HPSTR) lpBuffer, lRead) != lRead)
	        goto ack;

        //
        // dwLength is the offset to the other channel of data
        //
        lSeek = p->ckData.dwDataOffset + (p->avistream.dwLength) 
                    + (lSampleSize / 2) * lStart;
        lRead = lSamples * (lSampleSize / 2);

        //
        //  I need a temporary buffer to hold the other channel
        //
	    lpOtherChannelBuffer = (HPSTR) 
                                GlobalAllocPtr(GMEM_MOVEABLE, lRead);
        if (NULL == lpOtherChannelBuffer)
            goto ack;

        //
        //  Get the second channel of audio
        //
        if (mmioSeek(p->hmmio, lSeek, SEEK_SET) != lSeek)
            goto ack;

        if (mmioRead(p->hmmio, (HPSTR) lpOtherChannelBuffer, lRead) != lRead)
	        goto ack;

        SVXInterleaveChannels((HPSTR)lpBuffer, cbBuffer,
                           (HPSTR)lpOtherChannelBuffer, lRead);

	    GlobalFreePtr(lpOtherChannelBuffer);

        //
        //  Reset lRead to the actual value read for both channels
        //
        lRead = lSamples * 2;

    }
    else  // we have a mono file, lSampleSize should be 1
    {
        LONG   i;

        lSeek = p->ckData.dwDataOffset + lSampleSize * lStart;
        lRead = lSamples * lSampleSize;
    
        if (mmioSeek(p->hmmio, lSeek, SEEK_SET) != lSeek)
	        goto ack;

        if (mmioRead(p->hmmio, (HPSTR) lpBuffer, lRead) != lRead)
	        goto ack;

        //
        // Switch the sign on all of the samples
        //
        for (i = 0; i < lRead; i++)
        {
            *((HPSTR)lpBuffer+i) = *((HPSTR)lpBuffer+i) ^ 128;
        }
    }

    //
    // success return number of bytes and number of samples read
    //
    if (plSamples)
        *plSamples = lSamples;

    if (plBytes)
        *plBytes = lRead;

    return ResultFromScode(AVIERR_OK);
}


//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXStreamSetFormat
//
//  Description:
//      The SetFormat Method of the Stream interface	
//      - called on an empty SVX file before writing data to it.
//
//
//  Arguments:
//      PAVISTREAM ps:
//
//      LONG lPos:
//
//      LPVOID lpFormat:
//
//      LONG cbFormat:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXStreamSetFormat(
	PAVISTREAM ps, LONG lPos,LPVOID lpFormat,LONG cbFormat)
{
    // Get a pointer to our structure
    LPWAVESTUFF p = WAVESTUFF_FROM_STREAM(ps);

    // We can only do this to an empty wave file
    if (p->lpFormat) 
    {
	    if (cbFormat != p->cbFormat ||
	        _fmemcmp(lpFormat, p->lpFormat, (int) cbFormat))
	        return ResultFromScode(AVIERR_UNSUPPORTED);
	
	    return NOERROR;
    }
    
    // Go ahead and set the format!

    p->cbFormat = cbFormat;
    p->lpFormat = (LPWAVEFORMATEX) GlobalAllocPtr(GMEM_MOVEABLE, cbFormat);

    if (p->lpFormat == NULL)
    	return ResultFromScode(AVIERR_MEMORY);

    hmemcpy(p->lpFormat, lpFormat, cbFormat);

    //
    //  Data offset is different based on whether or not the sample is
    //  stereo.
    //
    if (1 == p->lpFormat->nChannels)
        p->ckData.dwDataOffset = (7 * sizeof(DWORD)) + sizeof(VHDR);
    else  // need channel chunk
        p->ckData.dwDataOffset = (10 * sizeof(DWORD)) + sizeof(VHDR);

    p->ckData.cksize = 0;
    p->avistream.dwScale = p->lpFormat->nBlockAlign;	       
    p->avistream.dwRate = p->lpFormat->nAvgBytesPerSec;
    p->avistream.dwLength = 0;
    p->avistream.dwSampleSize = p->lpFormat->nBlockAlign;

#ifndef FPSHACK
    p->avihdr.dwScale = 1;
    p->avihdr.dwRate = p->lpFormat->wf.nSamplesPerSec;
#endif
    return ResultFromScode(AVIERR_OK);
}

//--------------------------------------------------------------------------;
//
//  void SVXUninterleaveChannels
//
//  Description:
//      We need to take the interleaved channels of a RIFF WAVE file and
//      put them in two continuous streams on data.
//
//  Arguments:
//      HPSTR lpMainBuffer:
//
//      LONG cbMainBuffer:
//
//      HPSTR lpOtherBuffer:
//
//      LONG cbOtherBuffer:
//
//  Return (void):
//
//--------------------------------------------------------------------------;

void SVXUninterleaveChannels
(
    HPSTR   lpMainBuffer,
    LONG    cbMainBuffer,   
    HPSTR   lpOtherBuffer,
    LONG    cbOtherBuffer
)
{
    LONG    i,  // Current position in the other buffer
            j,  // Current position in the main buffer
            k;  // Current position in the condensed buffer

    i = 0L;
    j = 0L;
    k = 0L;

    //
    //  Decrement main and other buffer postions by one and completed buffer
    //  position by 2
    //
    for (; i < cbOtherBuffer ; i++, j = j + 2, k++)
    {
        *(lpMainBuffer + k)  = *(lpMainBuffer + j) ^ 128;  // Copy from other
        *(lpOtherBuffer + i) = *(lpMainBuffer + (j+1)) ^ 128; // Copy from main
    }

}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXStreamWrite
//
//  Description:
//      The Write Method of the Stream interface.  This is where we write out
//      the data for the SVX file.  The header info will be written out in the
//      SVXUnknownRelease function.
//
//  Arguments:
//      PAVISTREAM ps:
//
//      LONG lStart:
//
//      LONG lSamples:
//
//      LPVOID lpData:
//
//      LONG cbData:
//
//      DWORD dwFlags:
//
//      LONG FAR *plSampWritten:
//
//      LONG FAR *plBytesWritten:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXStreamWrite
(
    PAVISTREAM  ps,
 	LONG        lStart,
 	LONG        lSamples,
 	LPVOID      lpData,
 	LONG        cbData,
 	DWORD       dwFlags,
 	LONG FAR    *plSampWritten,
 	LONG FAR    *plBytesWritten
)
{
    // Get a pointer to our structure
    LPWAVESTUFF p = WAVESTUFF_FROM_STREAM(ps);

    // ACM Conversion Stuff
    HACMSTREAM      has;
    ACMSTREAMHEADER ash;
    WAVEFORMATEX    wfxSVX;
    DWORD           dwDstSize;
    HGLOBAL         hDstMem = NULL;
    LPBYTE          pbDstMem,
                    lpTmp = NULL;


    if ((p->mode & (OF_WRITE | OF_READWRITE)) == 0)
    	return ResultFromScode(AVIERR_READONLY);

    //
    // Now check if the bits per sample is something that SVX8 doesn't support
    // It only supports 8-bit formats, so this should be easy.
    //
    if (SVX_IFF_BITSPERSAMPLE != (int)((LONG)p->lpFormat->nAvgBytesPerSec /
                                 p->lpFormat->nSamplesPerSec /
                                 p->lpFormat->nChannels) * 8)
    {

        //
        //  Get an 8-bit format, that's all that I care about
        //
        wfxSVX.wBitsPerSample = SVX_IFF_BITSPERSAMPLE; 
        if (MMSYSERR_NOERROR != acmFormatSuggest(NULL, p->lpFormat, &wfxSVX,
                                    sizeof(wfxSVX), ACM_FORMATSUGGESTF_WBITSPERSAMPLE))
        {
            return ResultFromScode(AVIERR_CANTCOMPRESS);
        }

        if (MMSYSERR_NOERROR != acmStreamOpen(&has, NULL, p->lpFormat, &wfxSVX,
                                            NULL, 0L, 0L, ACM_STREAMOPENF_NONREALTIME))
        {
            return ResultFromScode(AVIERR_CANTCOMPRESS);
        }

        if (MMSYSERR_NOERROR != acmStreamSize(has, (DWORD)cbData, &dwDstSize, 
                                                ACM_STREAMSIZEF_SOURCE))
        {
            return ResultFromScode(AVIERR_CANTCOMPRESS);
        }

        if (!(hDstMem = GlobalAlloc(GMEM_FIXED | GMEM_SHARE, dwDstSize)))
        {
            return ResultFromScode(AVIERR_MEMORY);
        }

        if (!(pbDstMem = (LPBYTE)GlobalLock(hDstMem)))
        {
            return ResultFromScode(AVIERR_MEMORY);
        }
    
        //
        //  Setup the stream header for conversion
        // 
        ash.cbStruct      = sizeof(ash);
        ash.fdwStatus     = 0L;
        ash.pbSrc         = lpData;
        ash.cbSrcLength   = cbData;
        ash.pbDst         = pbDstMem;
        ash.cbDstLength   = dwDstSize;

        if (MMSYSERR_NOERROR != acmStreamPrepareHeader(has,&ash,0L))
        {
            return ResultFromScode(AVIERR_CANTCOMPRESS);
        }
        
        if (MMSYSERR_NOERROR != acmStreamConvert(has,&ash,
                        ACM_STREAMCONVERTF_START | ACM_STREAMCONVERTF_END))
        {
            return ResultFromScode(AVIERR_CANTCOMPRESS);
        }

        if (MMSYSERR_NOERROR != acmStreamUnprepareHeader(has,&ash,0L))
        {
            return ResultFromScode(AVIERR_CANTCOMPRESS);
        }

        if (MMSYSERR_NOERROR != acmStreamClose(has,0L))
        {
            return ResultFromScode(AVIERR_CANTCOMPRESS);
        }

        //
        //  Now make sure that we are using the new 8-bit data
        //  Save lpTmp so we can reassign later
        //
        lpTmp  = lpData;
        lpData = (LPVOID)pbDstMem;
        cbData = dwDstSize;

        //
        // Set the dwSampleSize to nChannels since we are converting
        // the data to 8-bit
        //
        p->avistream.dwSampleSize = p->lpFormat->nChannels;

    }

    // < 0 means "at end"
    if (lStart < 0)
    {
        //
        // We are using the same case for both stereo and mono because
        // dwLength is in samples and for stereo the end of the first 
        // channel is equal to the position dwLength points to.
        //
        lStart = p->avistream.dwStart + p->avistream.dwLength;
    }

#if 0 // !!! don't check for too long - why not?
    if (lStart > (LONG) (p->avistream.dwStart + p->avistream.dwLength))
	    return ResultFromScode(AVIERR_BADPARAM);
#endif

    p->fDirty = TRUE;


    //
    // Write out the two separate channels for IFF
    // 
    if (2 == p->lpFormat->nChannels)
    {
        LPVOID lpOtherChannelBuffer;

        //
        //  I need a temporary buffer to hold the other channel
        //
	    lpOtherChannelBuffer = (HPSTR) 
                                GlobalAllocPtr(GMEM_MOVEABLE, lSamples);
        if (NULL == lpOtherChannelBuffer)
    	    return ResultFromScode(AVIERR_MEMORY);

        SVXUninterleaveChannels((HPSTR)lpData, cbData,
                             (HPSTR)lpOtherChannelBuffer, lSamples);

        //
        // Seek to the write position for the first channel of audio
        //
        mmioSeek(p->hmmio, p->ckData.dwDataOffset + lStart * 
            (p->avistream.dwSampleSize / 2), SEEK_SET);

        if (mmioWrite(p->hmmio, (HPSTR) lpData, lSamples) != lSamples)
	        return ResultFromScode(AVIERR_FILEWRITE);

        //
        // Instead of writing out the second channel of audio here we defer
        // the write until the release because we don't want the first
        // channel writing over the second channel
        //
        if (p->avistream.dwLength < (DWORD)(lStart + lSamples))
        {
            if (NULL != hOtherChannelDeferredBuffer)
            {
                hOtherChannelDeferredBuffer = 
                    GlobalReAlloc(hOtherChannelDeferredBuffer, 
                                    lStart + lSamples, GMEM_MOVEABLE);
            }
            else
            {
                hOtherChannelDeferredBuffer = 
                    GlobalAlloc(GMEM_MOVEABLE, lStart + lSamples);
            }

            lpOtherChannelDeferredBuffer = 
                (BYTE _huge *)GlobalLock(hOtherChannelDeferredBuffer);
        }

        //
        // Put the new data for the other channel in the temp buffer at the
        // position specified by lStart
        //
        hmemcpy((BYTE _huge *)(lpOtherChannelDeferredBuffer + lStart * 
                (p->avistream.dwSampleSize / 2)), lpOtherChannelBuffer, 
                lSamples);

	    GlobalFreePtr(lpOtherChannelBuffer);
    }
    else
    {
        LONG    i;

        //
        //  Since we don't write out anything but 8-bit don't multiply
        //  by  p->avistream.dwSampleSize
        //
        mmioSeek(p->hmmio, p->ckData.dwDataOffset + lStart, SEEK_SET);

        //
        // Switch the sign on all of the samples
        //
        for (i = 0; i < cbData; i++)
        {
            *((HPSTR)lpData+i) = *((HPSTR)lpData+i) ^ 128;
        }

        if (mmioWrite(p->hmmio, (HPSTR) lpData, cbData) != cbData)
	        return ResultFromScode(AVIERR_FILEWRITE);
    }

    //
    //  Recompute the length -- if we wrote more samples than were there 
    //  the total length is increased
    //
    p->avistream.dwLength = max((LONG) p->avistream.dwLength,
	                        lStart + lSamples);

    p->ckData.cksize = max(p->ckData.cksize, 
                            lStart * p->avistream.dwSampleSize + cbData);

#ifdef FPSHACK
	p->avihdr.dwLength =
                MulDiv32(p->avistream.dwLength * FPSHACK,
				  p->avistream.dwScale,
				  p->avistream.dwRate);
#else
	p->avihdr.dwLength =
                MulDiv32(p->ckData.cksize, p->lpFormat->nSamplesPerSec,
			 p->lpFormat->nAvgBytesPerSec);
			 
#endif

    if (NULL != hDstMem)
    {
        if (GlobalUnlock(hDstMem) || GlobalFree(hDstMem))
            return ResultFromScode(AVIERR_MEMORY);
    }

    if (NULL != lpTmp)
        lpData = lpTmp;
    
    if (plSampWritten)
    	*plSampWritten = lSamples;

    if (plBytesWritten)
	    *plBytesWritten = cbData;
    
    return ResultFromScode(AVIERR_OK);
}

//
//
//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXStreamDelete
//
//  Description:
//      The Delete Method of the Stream interface - we don't cut from 
//      wave files
//
//  Arguments:
//      PAVISTREAM ps:
//
//      LONG lStart:
//
//      LONG lSamples:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXStreamDelete(PAVISTREAM ps, LONG lStart,LONG lSamples)
{
    return ResultFromScode(AVIERR_UNSUPPORTED);
}


//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXStreamReadData
//
//  Description:
//      We don't support ReadData for the Stream Interface
//
//  Arguments:
//      PAVISTREAM ps:
//
//      DWORD fcc:
//
//      LPVOID lp:
//
//      LONG FAR *lpcb:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXStreamReadData(PAVISTREAM ps,
		DWORD fcc, LPVOID lp, LONG FAR *lpcb)
{
    return ResultFromScode(AVIERR_UNSUPPORTED);
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXStreamWriteData
//
//  Description:
//      We don't support WriteData for the Stream Interface
//
//  Arguments:
//      PAVISTREAM ps:
//
//      DWORD fcc:
//
//      LPVOID lp:
//
//      LONG cb:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXStreamWriteData(PAVISTREAM ps, DWORD fcc, LPVOID lp, LONG cb)
{
    return ResultFromScode(AVIERR_UNSUPPORTED);
}



STDMETHODIMP SVXFileReserved(PAVIFILE pf)
{
    return ResultFromScode(AVIERR_UNSUPPORTED);
}

STDMETHODIMP SVXStreamReserved(PAVISTREAM ps)
{
    return ResultFromScode(AVIERR_UNSUPPORTED);
}


// *** IPersist methods ***

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXPersistGetClassID
//
//  Description:
//
//
//  Arguments:
//      LPPERSISTFILE ppf:
//
//      LPCLSID lpClassID:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXPersistGetClassID (LPPERSISTFILE ppf, LPCLSID lpClassID)
{
    // Get a pointer to our structure
    LPWAVESTUFF pfile = WAVESTUFF_FROM_PERSIST(ppf);

    hmemcpy(lpClassID, &CLSID_AVISVXFileReader, sizeof(CLSID));
    return NOERROR;
}

// *** IPersistFile methods ***

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXPersistIsDirty
//
//  Description:
//
//
//  Arguments:
//      LPPERSISTFILE ppf:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXPersistIsDirty (LPPERSISTFILE ppf)
{
    // Get a pointer to our structure
    LPWAVESTUFF pfile = WAVESTUFF_FROM_PERSIST(ppf);

    return pfile->fDirty ? NOERROR : ResultFromScode(S_FALSE);
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXPersistLoad
//
//  Description:
//
//
//  Arguments:
//      LPPERSISTFILE ppf:
//
//      LPCOLESTR lpszFileName:
//
//      DWORD grfMode:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXPersistLoad (LPPERSISTFILE ppf,
			      LPCOLESTR lpszFileName, DWORD grfMode)
{
    // Get a pointer to our structure
    LPWAVESTUFF pfile = WAVESTUFF_FROM_PERSIST(ppf);


#if !defined UNICODE
    char    achTemp[256];

    // Internally, we're using ANSI, but this interface is defined
    // to always accept UNICODE under WIN32, so we have to convert.
    WideCharToMultiByte(CP_ACP, 0, lpszFileName, -1,
			achTemp, sizeof(achTemp), NULL, NULL);
#else
    #define achTemp	lpszFileName
#endif

    return SVXFileOpen((PAVIFILE) &pfile->AVIFile, achTemp, (UINT) grfMode);
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXPersistSave
//
//  Description:
//
//
//  Arguments:
//      LPPERSISTFILE ppf:
//
//      LPCOLESTR lpszFileName:
//
//      BOOL fRemember:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXPersistSave (LPPERSISTFILE ppf,
			      LPCOLESTR lpszFileName, BOOL fRemember)
{
	// Get a pointer to our structure
	LPWAVESTUFF pfile = WAVESTUFF_FROM_PERSIST(ppf);


    return ResultFromScode(E_FAIL);
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXPersistSaveCompleted
//
//  Description:
//
//
//  Arguments:
//      LPPERSISTFILE ppf:
//
//      LPCOLESTR lpszFileName:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXPersistSaveCompleted (LPPERSISTFILE ppf,
				       LPCOLESTR lpszFileName)
{
	// Get a pointer to our structure
	LPWAVESTUFF pfile = WAVESTUFF_FROM_PERSIST(ppf);


    return NOERROR;
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXPersistGetCurFile
//
//  Description:
//
//
//  Arguments:
//      LPPERSISTFILE ppf:
//
//      LPOLESTR FAR * lplpszFileName:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXPersistGetCurFile (LPPERSISTFILE ppf,
				    LPOLESTR FAR * lplpszFileName)
{
    // Get a pointer to our structure
    LPWAVESTUFF pfile = WAVESTUFF_FROM_PERSIST(ppf);

    return ResultFromScode(E_FAIL);
}
