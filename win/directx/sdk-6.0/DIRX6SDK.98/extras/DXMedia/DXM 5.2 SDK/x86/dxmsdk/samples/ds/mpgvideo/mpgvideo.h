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
//
/******************************Module*Header*******************************\
* Module Name: MpgVideo.h
*
* Prototype Mpeg Video codec
*
\**************************************************************************/
#ifndef _INC_MPGVIDEO_H
#define _INC_MPGVIDEO_H
#include <streams.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <stddef.h>
#include <string.h>

#include "Decoder.h"


// -------------------------------------------------------------------------
// Helper functions that can be used by audio and video codecs.
// -------------------------------------------------------------------------
//

/******************************Public*Routine******************************\
* ByteSwap
*
* Converts dwX from little endian to big endian and vice-versa.
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



// -------------------------------------------------------------------------
// This structure is used to associate Pts time stamps with positions
// within the circular buffer.  Each time we get a media sample with a Pts
// time stamp we add a TIMEPOSITION entry to a queue of time positions.
// Each time we decode an I frame we record the starting and ending position
// of the I frame picture within the circular buffer, this information is
// then used to find a suitable time stamp to associate with the frame.  If
// a suitable time stamp cannot be found or the frame is not an I frame we
// calculate a suitable time code by extrapolation.
// -------------------------------------------------------------------------
//
class CTimePosition {
public:
    CRefTime    m_PtsTimeStamp;
    LPBYTE      m_BufferPosition;

    CTimePosition(CRefTime *TimeStamp, LPBYTE lpPos)
        : m_PtsTimeStamp(*TimeStamp), m_BufferPosition(lpPos) {}
};


// -------------------------------------------------------------------------
// Structure to hold the contents of an Mpeg 1 sequence header.
// -------------------------------------------------------------------------
struct SEQHDR_INFO {
    LONG           lWidth;             //  Native Width in pixels
    LONG           lHeight;            //  Native Height in pixels
    LONG           lvbv;               //  vbv
    REFERENCE_TIME tPictureTime;       //  Time per picture in 100ns units
    LONG           lTimePerFrame;      //  Time per picture in MPEG units
    LONG           dwBitRate;          //  Bits per second
    LONG           lXPelsPerMeter;     //  Pel aspect ratio
    LONG           lYPelsPerMeter;     //  Pel aspect ratio
    DWORD          dwStartTimeCode;    //  First GOP time code (or -1)
    LONG           lActualHeaderLen;   //  Length of valid bytes in raw seq hdr
    BYTE           RawHeader[140];     //  The real sequence header
};


// -------------------------------------------------------------------------
// Quartz Mpeg video codec framework class
//
// -------------------------------------------------------------------------
//
class CMpegVideoOutputPin;
class CMpegVideoInpuPin;

class CMpegVideoCodec
    : public CTransformFilter {

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
    HRESULT SetMediaType(PIN_DIRECTION direction,const CMediaType *pmt);
    HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
    HRESULT StartStreaming(void);
    HRESULT StopStreaming(void);
    HRESULT EndOfStream(void);
    HRESULT EndFlush(void);
    HRESULT AlterQuality(Quality q);


    CMpegVideoCodec(TCHAR *pName, LPUNKNOWN pUnk, HRESULT *pHr);
    ~CMpegVideoCodec();


    //
    // Callback function from low-level deocoder to get an output
    // buffer to draw into.
    //
    LPBYTE GetDecodeBufferAndFormat();

private:
    LPBYTE          m_pSample;
    LONG            m_LenLeftInPacket;

    //
    // Input buffer
    //
    int             m_VBlockSize;
    LPBYTE          m_Buffer;
    LPBYTE          m_BufferStart;
    LPBYTE          m_BufferCurr;
    LONG            m_BufferFullness;
    void            UpdateBufferPosition(LPBYTE lpNewCurrent);


    //
    // Output buffer, for decode I and P frames.
    //
    LPBYTE          m_pFrameBuff;


    //
    // Sequence header of the video stream beging played.
    //
    SEQHDR_INFO     m_seqInfo;



    //
    // Decode statistics, number of I, P, B and skipped frames.
    //
    DWORD           m_dwFramesDecoded[4];
    DWORD           m_dwFramesSkipped[4];
    int             m_PerfDecode;
    int             m_QualMsg;
    int             m_FrameDrawn;
    int             m_FrameType;


    //
    // This is the low level mpeg video decoder that all the work
    //
    DWORD           m_dwLateBy;
    DWORD           m_dwCtrl;
    DWORD           m_dwCtrlDefault;
    DWORD           m_dwQualDefault;
    CVideoDecoder   *m_pVideoDecoder;
    VideoCtrl       m_VideoControl;

    //
    // The decoder calls back to get the colour conversion format
    // and a buffer to decode into.  The callback is static because it
    // does not have a "this" pointer.  hrCallback is used to keep a record
    // of any error that may have occurred during the callback.
    //
    void            SetOutputPinMediaType(const CMediaType *pmt);
    IMediaSample    *m_pOutSample;
    HRESULT         hrCallback;



    //
    // If the input sample contains a sync point then its
    // sample time is correct.  I will use this time to resync my
    // internal clock and reset the count of frames decoded
    // since the previous sync point.  Otherwise, if the sample
    // is not a sync point I use the average time per frame and
    // the count of frames since the last sync point to interpolate
    // a suitable time for the newly decoded frame.
    //
    CRefTime        m_TimeAtLastSyncPoint;
    CRefTime        m_TimeSinceLastSyncPoint;
    CRefTime        m_tStopPrev;     // Previous stop time
    CGenericList<CTimePosition> m_PtsQueue;


    //
    // IP Frame tracking class
    //
    class CNextIP
    {
    public:
        CNextIP();
        BOOL GotFirst();
        void NextRef(DWORD dwTemporalReference);
        BOOL GetTime(REFERENCE_TIME *pt);
        BOOL TimeToDraw() const;
        void Reset();
        void Set(DWORD dwSkipFlag, BOOL bIFrame,
                 BOOL bTimeSet, REFERENCE_TIME t, DWORD dwTemporalReference);

    private:
        DWORD           m_dwSkipFlag;
        BOOL            m_bGotFirst;
        BOOL            m_bTimeSet;
        BOOL            m_bTimeToDraw;
        REFERENCE_TIME  m_t;
        DWORD           m_dwTemporalReference;
    };

    CNextIP         m_NextIP;


    //
    // Remember whether the media type specified packets or payload
    //
    BOOL            m_bPayloadOnly;


    //
    // On 8 bit displays the user can select a gray scale palette
    // alteranative to the default colour palette, this bypasses the
    // colour space conversion code.
    //
    enum MEDIA_FORMATS {MM_411PK, MM_422PK, MM_422SPK, MM_RGB24_DIB,
                        MM_RGB24_DDB, MM_RGB565_DIB, MM_RGB565_DDB,
                        MM_RGB555_DIB, MM_RGB555_DDB, MM_RGB8_DIB,
                        MM_RGB8_DDB, MM_Y_DIB, MM_Y_DDB};

    enum MEDIA_TYPES   {MT_Y41P = 0, MT_YUY2, MT_UYVY, MT_RGB24, MT_RGB565,
                        MT_RGB555, MT_RGB8, MT_COUNT_OF_TYPES = MT_RGB8};

    enum PALETTE_TYPE {COLOUR_PALETTE, GREY_PALETTE};

    PALETTE_TYPE    m_PaletteType;
    BOOL            m_IgnoreQualityMessage;
    DWORD           m_dwOutputFormatDib;
    DWORD           m_dwOutputFormatDdb;

    HRESULT         CopySampleToBuffer(IMediaSample *pSample);
    BOOL            UpdateTimeSyncPoint(LPBYTE lpPicStart, REFERENCE_TIME *Time);
    ptrdiff_t       BuffOffset(LPBYTE Offset);

    HRESULT         DecodeNextPicture();
    void            ResetVideoDecoder();
    void            DecodeUntilBufferEmpty();

    void            InitDestinationVideoInfo(VIDEOINFO *pVI, DWORD Comp, int n);
    void            InitDestinationPalette(VIDEOINFO *pVideoInfo);

    BOOL            ParseSequenceHeader(const BYTE *pbData, LONG lData,
                                        SEQHDR_INFO *pInfo);

    //
    //  Stop time
    //
    CRefTime        m_tStop;
};
#endif
