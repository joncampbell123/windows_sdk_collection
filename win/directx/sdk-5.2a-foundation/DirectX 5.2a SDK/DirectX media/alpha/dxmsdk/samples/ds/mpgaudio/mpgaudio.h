//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;

/******************************Module*Header*******************************\
* Module Name: MpgAudio.h
*
* Prototype Mpeg Audio codec
*
*
\**************************************************************************/
#ifndef _INC_MPGAUDIO_H
#define _INC_MPGAUDIO_H
#include <streams.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <stddef.h>
#include <string.h>

#include "decoder.h"


class CMpegAudioCodec : public CTransformFilter {
public:
    //
    // --- Com stuff ---
    //
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN, HRESULT *);
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);
    DECLARE_IUNKNOWN;


    //
    // --- CTransform overrides ---
    //
    HRESULT Receive(IMediaSample *pSample);
    HRESULT CheckInputType(const CMediaType* mtIn);
    HRESULT CheckTransform(const CMediaType* mtIn, const CMediaType* mtOut);
    HRESULT DecideBufferSize(IMemAllocator * pAllocator,
                             ALLOCATOR_PROPERTIES * pProperties);
    HRESULT StartStreaming();
    HRESULT StopStreaming();
    HRESULT SetMediaType(PIN_DIRECTION direction,const CMediaType *pmt);
    HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
    HRESULT EndOfStream(void);
    HRESULT EndFlush(void);

    CMpegAudioCodec(TCHAR *pName, LPUNKNOWN pUnk, HRESULT *pHr);
    ~CMpegAudioCodec();

private:
    //  Serialize access to the output pin
    long            m_FrameSize;        // Frame input size (bytes)
    long            m_FrameSizeOutput;  // Frame output size (bytes)

    LPBYTE          m_lpStart;
    LPBYTE          m_lpCurr;
    LPBYTE          m_lpEnd;
    BOOL            m_bPayloadOnly;

    enum            {MAX_FRAMES_PER_OUTPUT_SAMPLE = 4};
    enum            {AUDIO_BUFF_SIZE = (1024 * 8)};

    DWORD           m_dwCtrl;
    AudioCtrl       m_AudioControl;
    CAudioDecoder   *m_pAudioDecoder;

    CRefTime        m_TimePerFrame;
    CRefTime        m_TimeAtLastSyncPoint;
    CRefTime        m_TimeSinceLastSyncPoint;

    int             m_FreqDiv;
    int             m_PrefChan;
    int             m_Quality;
    int             m_QuarterInt;
    int             m_WordSize;

    BYTE            m_Buffer[AUDIO_BUFF_SIZE];

    void    ProcessDiscontiuity(IMediaSample *pSample);
    void    ProcessSyncPoint(IMediaSample *pSample, BYTE *pSrc);
    HRESULT DeliverSample(IMediaSample *pOutSample, CRefTime &TimeDecoded,
                          int iSampleSize);

    void    ResetAudioDecoder();
    BOOL    LookForSyncWord();
    int     Padding();
    void    GetNextPacketChunk(LPBYTE &lpPacket,
                               long &LenLeftInBuffer, long &LenLeftInPacket);

    CRefTime        m_tStop;
    MPEG1WAVEFORMAT m_Format;

};


// -------------------------------------------------------------------------
// Helper functions that can be used by audio and video codecs.
// -------------------------------------------------------------------------
//

/******************************Public*Routine******************************\
* ByteSwap
*
* Converts dwX from little endian to big endian and vice-versa.
*
*
\**************************************************************************/
__inline DWORD
ByteSwap(
    DWORD dwX
    )
{
#ifdef _X86_
    _asm    mov     eax, dwX
    _asm    bswap   eax
    _asm    mov     dwX, eax

    return dwX;
#else
    return _lrotl(((dwX & 0xFF00FF00) >> 8) | ((dwX & 0x00FF00FF) << 8), 16);
#endif
}

LPBYTE SkipToPacketData(LPBYTE pSrc, long &LenLeftInPacket);
int GetDecoderInteger(const TCHAR *pKey,int iDefault);

#endif
