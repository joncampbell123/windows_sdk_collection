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
* Module Name: MpgVideo.cpp
*
* Implements a prototype Mpeg Video Software codec.  It just consumes
* the passed in packets.
*
\**************************************************************************/
#include "MpgVideo.h"

// define the GUIDs for streams and my CLSID in this file
#include <initguid.h>
#include "mpegUids.h"
#include "palette.h"


// setup data

const AMOVIESETUP_MEDIATYPE
psudIpPinTypes[] = { { &MEDIATYPE_Video                // clsMajorType
                     , &MEDIASUBTYPE_MPEG1Packet  }    // clsMinorType
                   , { &MEDIATYPE_Video                // clsMajorType
                     , &MEDIASUBTYPE_MPEG1Payload } }; // clsMinorType

const AMOVIESETUP_MEDIATYPE
sudOpPinTypes = { &MEDIATYPE_Video      // clsMajorType
                , &MEDIASUBTYPE_NULL }; // clsMinorType

const AMOVIESETUP_PIN
psudPins[] = { { L"Input"            // strName
               , FALSE               // bRendered
               , FALSE               // bOutput
               , FALSE               // bZero
               , FALSE               // bMany
               , &CLSID_NULL         // clsConnectsToFilter
               , L"Output"           // strConnectsToPin
               , 2                   // nTypes
               , psudIpPinTypes }    // lpTypes
             , { L"Output"           // strName
               , FALSE               // bRendered
               , TRUE                // bOutput
               , FALSE               // bZero
               , FALSE               // bMany
               , &CLSID_NULL         // clsConnectsToFilter
               , L"Input"            // strConnectsToPin
               , 1                   // nTypes
               , &sudOpPinTypes } }; // lpTypes



const AMOVIESETUP_FILTER
sudMPEGVideo = { &CLSID_CMpegFrameworkVideoCodec // clsID
               , L"MPEG Framework Audio Codec"   // strName
               , 0x00600000                      // dwMerit
               , 2                               // nPins
               , psudPins };                     // lpPin

/* -------------------------------------------------------------------------
** list of class ids and creator functions for class factory
** -------------------------------------------------------------------------
*/
CFactoryTemplate g_Templates[] =
{
    {L"MPEG Framework Video Codec"
    , &CLSID_CMpegFrameworkVideoCodec
    , CMpegVideoCodec::CreateInstance
    , NULL
    , &sudMPEGVideo }
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);


const TCHAR chRegistryKey[] =
    TEXT("Software\\Microsoft\\Multimedia\\ActiveMovie Filters\\Sample MPEG Video Decoder");


/******************************Public*Routine******************************\
* GetDecoderInt
*
*
*
* History:
*
*
\**************************************************************************/
int
GetDecoderInt(
    const TCHAR *pKey,
    int iDefault
    )
{
    HKEY hKey;
    LONG lRet;
    int  iRet = iDefault;

    lRet = RegOpenKey(HKEY_CURRENT_USER, chRegistryKey, &hKey);
    if (lRet == ERROR_SUCCESS) {

        DWORD   dwType, dwLen;

        dwLen = sizeof(iRet);
        if (ERROR_SUCCESS != RegQueryValueEx(hKey, pKey, 0L, &dwType,
                                             (LPBYTE)&iRet, &dwLen)) {
            iRet = iDefault;
        }
        RegCloseKey(hKey);
    }
    return iRet;
}



/* -------------------------------------------------------------------------
** CMpegVideoCodec
** -------------------------------------------------------------------------
*/
CMpegVideoCodec::CMpegVideoCodec(
    TCHAR *pName,
    LPUNKNOWN pUnk,
    HRESULT *phr
    )
    : CTransformFilter(pName, pUnk, CLSID_CMpegVideoCodec),
      m_pVideoDecoder(NULL),
      m_PtsQueue(NAME("Pts queue"), 30, FALSE, FALSE),
      m_pFrameBuff(NULL),
      m_Buffer(NULL),
      m_pOutSample(NULL)
{
    //
    // Pick up any user preferences - this should get moved to the register
    //
    m_IgnoreQualityMessage = GetDecoderInt(TEXT("IgnoreQMessages"), FALSE);

    m_dwCtrlDefault = GetDecoderInt(TEXT("VideoFramesDecoded"), DECODE_IPB);
    m_dwCtrlDefault &= 0x3F;

    m_dwQualDefault = GetDecoderInt(TEXT("VideoQuality"), 0);
    m_dwQualDefault &= 0x30000000;
    m_dwCtrl = (m_dwCtrlDefault | m_dwQualDefault);


    if (GetDecoderInt(TEXT("GreyScale"), 0)) {
        m_PaletteType = GREY_PALETTE;
        m_dwOutputFormatDib = MM_Y_DIB;
        m_dwOutputFormatDdb = MM_Y_DDB;
    }
    else {
        m_PaletteType = COLOUR_PALETTE;
        m_dwOutputFormatDib = MM_RGB8_DIB;
        m_dwOutputFormatDdb = MM_RGB8_DDB;
    }

    //
    // Reset frame stats
    //
    ZeroMemory(m_dwFramesSkipped, sizeof(m_dwFramesSkipped));
    ZeroMemory(m_dwFramesDecoded, sizeof(m_dwFramesDecoded));

    m_PerfDecode = MSR_REGISTER(TEXT("Decode Time  - Start/Stop"));
    m_QualMsg = MSR_REGISTER(TEXT("Quality Message"));
    m_FrameDrawn = MSR_REGISTER(TEXT("Frame Drawn"));
    m_FrameType = MSR_REGISTER(TEXT("Frame Type"));
}


CMpegVideoCodec::~CMpegVideoCodec(
    )
{
    CTimePosition   *pTimePos;

    DbgLog((LOG_TRACE, 2, TEXT("CMpegVideoCodec::~CMpegVideoCodec")));

    delete m_pVideoDecoder;
    m_pVideoDecoder = NULL;

    delete [] m_pFrameBuff;
    m_pFrameBuff = NULL;

    delete [] m_Buffer;
    m_Buffer = NULL;


    //
    // Purge the PTS queue.
    //
    while( (pTimePos = m_PtsQueue.RemoveHead()) != NULL) {
        delete pTimePos;
    }

    //
    // This should have been deleted in BreakConnect
    //
    ASSERT(m_Buffer == NULL);
}



/******************************Public*Routine******************************\
* CreateInstance
*
* This goes in the factory template table to create new instances
*
\**************************************************************************/
CUnknown *
CMpegVideoCodec::CreateInstance(
    LPUNKNOWN pUnk,
    HRESULT * phr
    )
{
    DbgLog((LOG_TRACE, 2, TEXT("CMpegVideoCodec::CreateInstance")));
    return new CMpegVideoCodec(TEXT("MPEG Video codec filter"), pUnk, phr);
}


/******************************Public*Routine******************************\
* NonDelegatingQueryInterface
*
* Here we would reveal ISpecifyPropertyPages and IMpegVideoDecoder if
* the framework had a property page.
*
\**************************************************************************/
STDMETHODIMP
CMpegVideoCodec::NonDelegatingQueryInterface(
    REFIID riid,
    void ** ppv
    )
{
    return CTransformFilter::NonDelegatingQueryInterface(riid, ppv);
}


/******************************Public*Routine******************************\
* EndOfStream
*
*
*
\**************************************************************************/
HRESULT
CMpegVideoCodec::EndOfStream()
{
    DbgLog((LOG_TRACE, 2, TEXT("End of stream called")));
    CAutoLock lck(&m_csReceive);

    if (m_pVideoDecoder == NULL) {
        return VFW_E_WRONG_STATE;
    }

    DecodeUntilBufferEmpty();
    ResetVideoDecoder();
    return CTransformFilter::EndOfStream();
}


/******************************Public*Routine******************************\
* EndFlush
*
*
*
\**************************************************************************/
HRESULT
CMpegVideoCodec::EndFlush()
{
    DbgLog((LOG_TRACE, 2, TEXT("End flush called")));
    CAutoLock lck(&m_csReceive);
    ResetVideoDecoder();
    return CTransformFilter::EndFlush();
}


/******************************Public*Routine******************************\
* Receive
*
* Copy the input sample into our buffer.  Loop while the buffer size is
* greater than or equal to the size given in the Vbv for the next picture
* decode the picture and pass it along to the output pin for rendering.
*
\**************************************************************************/
HRESULT
CMpegVideoCodec::Receive(
    IMediaSample *pSample
    )
{
    //
    //  Make sure the pin doesn't go inactive on us
    //

    HRESULT   hr;
    CAutoLock lck(&m_csReceive);

    if (m_pVideoDecoder == NULL || m_pFrameBuff == NULL) {
        return E_UNEXPECTED;
    }

    //
    // Check for a discontinuity, if one is found decode and display any
    // frames left in the circular buffer, reset the decoder and then continue
    // processing the current sample.
    //
    if (pSample->IsDiscontinuity() == S_OK) {

        DecodeUntilBufferEmpty();
        ResetVideoDecoder();

        //
        //  Find out what the current stop time is...
        //  ... get logical duration from upstream
        //
        HRESULT hrPos;
        REFTIME dStart, dStop;
        IMediaPosition *pPosition = NULL;

        hrPos = m_pInput->GetConnected()->QueryInterface(IID_IMediaPosition,
                                                         (void **)&pPosition);

        if ( SUCCEEDED(hrPos)
          && pPosition != NULL
          && SUCCEEDED(pPosition->get_CurrentPosition(&dStart))
          && SUCCEEDED(pPosition->get_StopTime(&dStop)))
        {
            m_tStop =
                (CRefTime)(COARefTime)dStop - (CRefTime)(COARefTime)dStart;
        }
        else {
            m_tStop = 0x7FFFFFFFFFFFFFFF;
        }

        if (pPosition != NULL) {
            pPosition->Release();
        }

        DbgLog((LOG_TRACE, 2,
                TEXT("Receive() : Discontinuity - setting stop time to %s"),
                (LPCTSTR)CDisp(m_tStop)));
    }

    m_pSample = NULL;
    m_LenLeftInPacket = 0L;

    //
    // If this sample is a sync point we need to
    // update our clock and reset the count of
    // samples received since the last sync point.
    //

    if (pSample->IsSyncPoint() == S_OK) {

        CRefTime        tStart, tStop;
        CTimePosition   *pTimePos;

        pSample->GetTime((REFERENCE_TIME*)&tStart,
                         (REFERENCE_TIME*)&tStop);

        pTimePos = new CTimePosition(&tStart,
                                     m_BufferCurr + m_BufferFullness >= m_BufferStart + m_VBlockSize ?
                                     m_BufferCurr + m_BufferFullness - m_VBlockSize : m_BufferCurr + m_BufferFullness);

        DbgLog(( LOG_TRACE, 4, TEXT("PTS: %s  Buffer pos: %ld"),
                 (LPCTSTR)CDisp(tStart), m_BufferCurr + m_BufferFullness ));
        m_PtsQueue.AddTail(pTimePos);

    }

    //
    // Decode and if necessary display as many frames as
    // possible until we exhaust our circular buffer.
    //

    do {

        hr = CopySampleToBuffer(pSample);

        while (SUCCEEDED(hr) && (m_BufferFullness >= m_seqInfo.lvbv)) {

            hr = DecodeNextPicture();
        }

    } while (S_OK == hr && (m_LenLeftInPacket != 0L));

    return hr;
}


/*****************************Private*Routine******************************\
* ResetVideoDecoder
*
*
*
\**************************************************************************/
void
CMpegVideoCodec::ResetVideoDecoder()
{
    //
    // Make him eat our sequence header
    //
    // m_BufferStart = m_BufferCurr = m_Buffer->GetPointer();
    m_BufferStart = m_BufferCurr = m_Buffer;
    m_BufferFullness = m_seqInfo.lActualHeaderLen;

    CopyMemory(m_BufferStart, m_seqInfo.RawHeader, m_seqInfo.lActualHeaderLen);

    m_TimeSinceLastSyncPoint = 0L;
    m_pOutSample = NULL;

    //
    // Don't play anything until we get a sync point
    // but don't make it too negative or we'll overflow!
    //
    m_TimeAtLastSyncPoint = (LONGLONG)-0x7FFFFFFFFFFFFF;
    m_tStopPrev = m_TimeAtLastSyncPoint;

    m_pVideoDecoder->ResetVideo();


    //
    // Purge the PTS queue.
    //
    CTimePosition *pTimePos;
    while( (pTimePos = m_PtsQueue.RemoveHead()) != NULL) {
        delete pTimePos;
    }


    //
    // Reset IP tracking
    //
    m_NextIP.Reset();

}


/*****************************Private*Routine******************************\
* DecodeUntilBufferEmpty
*
*
*
\**************************************************************************/
void
CMpegVideoCodec::DecodeUntilBufferEmpty()
{
    HRESULT hr = S_OK;

    while (SUCCEEDED(hr) && (m_BufferFullness > 0)) {

        LPBYTE pCurrentCurr = m_BufferCurr;

        DbgLog((LOG_TRACE, 3, TEXT("Forcing out a frame")));
        hr = DecodeNextPicture();

        if (S_OK != hr || m_BufferCurr == pCurrentCurr) {

            //
            // We're not making any progress!
            //
            DbgLog((LOG_ERROR, 2,
                    TEXT("CMpegVideoCodec::DecodeUntilBufferEmpty() ")
                    TEXT("- Stuck with %d bytes in buffer"),
                    m_BufferFullness));
            break;
        }
    }

    //
    // Now see if there's a frame decoded but not presented
    //
    if (m_NextIP.TimeToDraw()) {

        DbgLog((LOG_TRACE, 2, TEXT("Trying to get last frame")));

        DWORD dwCtrl = m_dwCtrl;
        m_dwCtrl &= 0xFFFF0000;
        m_dwCtrl |= DECODE_DIS;

        if (S_OK != DecodeNextPicture()) {
            DbgLog((LOG_ERROR, 2, TEXT("Failed to display last frame")));
        }

        m_dwCtrl = dwCtrl; // Restore
    }
}


/*****************************Private*Routine******************************\
* GetDecodeBufferAndFormat
*
* This function is called by the mpeg decoder when it is time to determine
* the colour conversion format and the destination buffer pointer for the
* frame.
*
\**************************************************************************/
LPBYTE
CMpegVideoCodec::GetDecodeBufferAndFormat()
{
    unsigned char   *pDst;
    AM_MEDIA_TYPE      *pmt;
    CRefTime        tStartTime(m_tStopPrev);
    CRefTime        tStopTime;

    // Don't ask for a buffer with a negative start time.
    if (tStartTime < (LONGLONG)0) {
        tStartTime = (LONGLONG)0;
    }

    //
    // this may block for an indeterminate amount of time
    //
    tStopTime = tStartTime + m_seqInfo.tPictureTime;
    hrCallback = m_pOutput->GetDeliveryBuffer(&m_pOutSample,
                                              (REFERENCE_TIME*)&tStartTime,
                                              (REFERENCE_TIME*)&tStopTime,
                                              0 // Must set proper flag
                                                // if prev frame skipped
                                              );
    if (FAILED(hrCallback)) {
        return NULL;
    }
    ASSERT(m_pOutSample);

    hrCallback = m_pOutSample->GetPointer(&pDst);
    if (FAILED(hrCallback)) {
        return NULL;
    }
    ASSERT(pDst);

    //
    // If the media type has changed then pmt is NOT NULL
    //
    m_pOutSample->GetMediaType(&pmt);
    if (pmt != NULL) {
        CMediaType cmt(*pmt);
        DeleteMediaType(pmt);
        SetOutputPinMediaType(&cmt);
    }
    return pDst;
}


/*****************************Private*Routine******************************\
* DecodeNextPicture
*
* Decodes the next picture stored in the circular buffer.  If the picture
* is not "skipped" it is passed to the output pin.  Updates m_BufferStart,
* m_BufferCurr and m_BufferFullness to reflect any emptying of the buffer that
* has taken place.
*
\**************************************************************************/
HRESULT
CMpegVideoCodec::DecodeNextPicture()
{
    DWORD   rc;
    HRESULT hr = S_OK;

    //
    // ParthaSr.
    // trying to recover from the absence of quality messages
    // when I do a skip - so I am predictively skipping
    // frames.  - Snap back to default if I have already
    // skipped enough frames
    //
    if (m_dwLateBy < 750000L) {
        m_dwCtrl = m_dwCtrlDefault | m_dwQualDefault;
    }

    m_VideoControl.dwCtrl = m_dwCtrl;

    //
    // If we're within 1 second of the end do all IP
    //
    if (m_tStop - m_tStopPrev < CRefTime(1000L)) {

        //
        // But are we almost at the end? in which case we need to decode
        // everything
        //
        if (m_tStop - m_tStopPrev < CRefTime(m_seqInfo.tPictureTime) +
                                    CRefTime(m_seqInfo.tPictureTime)) {

            if ((m_dwCtrl & 0xFFFF) != DECODE_DIS) {
                m_VideoControl.dwCtrl = (m_dwCtrl & 0xFFFF0000) | DECODE_IPB;
            }
        }
        else {

            if ((m_dwCtrl & 0xFFFF) < DECODE_IP) {
                m_VideoControl.dwCtrl = (m_dwCtrl & 0xFFFF0000) | DECODE_IP;
            }
        }
    }
    else {

        //
        // Only do IP if we're before the start
        //
        if ((m_dwCtrl & 0x3F) > DECODE_IP) {

            if (m_tStopPrev
              + CRefTime(m_seqInfo.tPictureTime)
              + CRefTime(m_seqInfo.tPictureTime) < CRefTime(0L)) {

                m_VideoControl.dwCtrl = (m_dwCtrl & 0xFFFF0000) | DECODE_IP;
            }

        }
    }
    DbgLog((LOG_TRACE, 4, TEXT("Decode flags = %X"), m_VideoControl.dwCtrl));


    //
    // Initialize the video codec with the new video data buffer.  I am
    // assuming that there is at least one video frame in the data
    // buffer.  When this function gets called to force out the frames
    // left in the buffer it needs to guarded with a try except block.
    //
    m_VideoControl.pCmprWrite  = m_BufferCurr + m_BufferFullness;
    m_VideoControl.pCmprRead   = m_BufferCurr;

    //
    // This is where the action starts, call the codec and let it do it stuff.
    //

    //  Don't die if Windows NT takes away the DCI surface
    try {

        MSR_START(m_PerfDecode);
        rc = m_pVideoDecoder->DecodeFrame(&m_VideoControl);
        MSR_STOP(m_PerfDecode);
    }
    except(EXCEPTION_EXECUTE_HANDLER) {

        DbgLog((LOG_ERROR, 1, TEXT("Exception in decoder!")));
        rc = DECODE_ERR_DATA;
    }

    //
    // Did the frame decode OK
    //

    if (rc == DECODE_SUCCESS) {

#ifdef DEBUG
        static char *ft[4] = { "Dummy", "I", "P", "B" };
#endif

        //
        // Record frames stats
        //
        MSR_INTEGER(m_FrameType, m_VideoControl.dwFrameType);
        if (m_VideoControl.dwSkipFlag) {

            DbgLog((LOG_TRACE, 2, TEXT("%hs Frame skipped"),
                    ft[m_VideoControl.dwFrameType]));

            m_dwFramesSkipped[m_VideoControl.dwFrameType]++;

            LARGE_INTEGER li;
            li.QuadPart = m_seqInfo.tPictureTime;
            if (m_dwLateBy > li.LowPart) {
                m_dwLateBy -= li.LowPart;
            }
        }
        else {
            m_dwFramesDecoded[m_VideoControl.dwFrameType]++;
        }

        if (m_VideoControl.dwFrameType == FTYPE_I ||
            m_VideoControl.dwFrameType == FTYPE_P) {

            //
            //  Do I/P - we're actually going to draw the I/P we
            //  decoded last time we got one, not the present one
            //

            if (m_NextIP.GetTime((REFERENCE_TIME *)&m_TimeAtLastSyncPoint)) {
                m_TimeSinceLastSyncPoint = 0;
            }

            REFERENCE_TIME t;
            BOOL bIFrame = m_VideoControl.dwFrameType == FTYPE_I;

            if (UpdateTimeSyncPoint(m_VideoControl.pFrameStartPos, &t)) {
                m_NextIP.Set(m_VideoControl.dwSkipFlag, bIFrame,
                             TRUE, t, m_VideoControl.dwTemporalReference);
            }
            else {
                m_NextIP.Set(m_VideoControl.dwSkipFlag, bIFrame,
                             FALSE, t, m_VideoControl.dwTemporalReference);
            }
        }
        else {

            //
            //  Do B
            //
            m_NextIP.NextRef(m_VideoControl.dwTemporalReference);
            if ( UpdateTimeSyncPoint(m_VideoControl.pFrameStartPos,
                                     (REFERENCE_TIME*)&m_TimeAtLastSyncPoint) ) {
                m_TimeSinceLastSyncPoint = 0;
            }
        }

        CRefTime tStop(m_TimeAtLastSyncPoint + m_TimeSinceLastSyncPoint +
                       m_seqInfo.tPictureTime);

        if (!m_VideoControl.dwSkipFlag) {

            LPBITMAPINFOHEADER  lpbiDst = HEADER(m_pOutput->CurrentMediaType().Format());

            //
            // if the time is < 0, then this is preroll to get from keyframe to
            // the current frame. We decompress it into the output buffer but
            // don't deliver it.
            //
            // In order that there are no gaps we actually start the next
            // frame where we predicted it would start, rather than the
            // actual synch point
            //

            CRefTime tStart(m_tStopPrev);

            DbgLog((LOG_TRACE, 2,
                    TEXT("%hs Frame decoded - tStart = %s, tStop = %s"),
                    ft[m_VideoControl.dwFrameType],
                    (LPCTSTR)CDisp(tStart), (LPCTSTR)CDisp(tStop)));


            #pragma message (REMIND("Do Preroll right"))
            if (tStop > 0L && tStart <= m_tStop) {

                // decompressed frames are always key
                m_pOutSample->SetSyncPoint(TRUE);
                m_pOutSample->SetActualDataLength(lpbiDst->biSizeImage);
                m_pOutSample->SetTime((REFERENCE_TIME*)&tStart,
                                      (REFERENCE_TIME*)&tStop);

                DbgLog((LOG_TRACE, 2, TEXT("%hs Frame sent to next filter"),
                        ft[m_VideoControl.dwFrameType]));


                hr = m_pOutput->Deliver(m_pOutSample);
                MSR_NOTE(m_FrameDrawn);
            }
        }

        //
        // We have successfully decoded a frame.
        // Set the new position
        //
        m_tStopPrev = tStop;
        UpdateBufferPosition(m_VideoControl.pCmprRead);
        m_TimeSinceLastSyncPoint += m_seqInfo.tPictureTime;
    }
    else if (rc == DECODE_ERR_QUARTZ) {

        DbgLog((LOG_ERROR, 2,
                TEXT("Could not get buffer from down stream filter")));
        hr = hrCallback;
    }
    else if (rc == DECODE_ERR_DATA) {

        //
        // We did not have enough data available to decode the
        // current frame, so save the data for next time.
        //
        // Since this can only happen at a discontinuity, end of
        // stream or undecipherable data we'll just throw the data
        // away if it can't be eaten
        DbgLog((LOG_ERROR, 2, TEXT("Buffer underflow") ));

        UpdateBufferPosition(m_VideoControl.pCmprRead);

        //
        //  Notify the filter graph of stream errors
        //
        NotifyEvent(EC_STREAM_ERROR_STILLPLAYING, hr, 0);
    }
    else {

        //
        // Some sort of error occurred, throw the remainder of the
        // buffer away and skip this packet.
        //
        DbgLog((LOG_ERROR, 2,
                TEXT("Bad return code %d from MediaMatics video codec!"), rc ));
        UpdateBufferPosition(m_VideoControl.pCmprRead);

        //
        //  Notify the filter graph of stream errors
        //
        NotifyEvent(EC_STREAM_ERROR_STILLPLAYING, hr, 0);
    }


    //
    // release the output buffer. If the connected pin still needs it,
    // it will have addrefed it itself.
    //
    if (m_pOutSample != NULL) {
        m_pOutSample->Release();
        m_pOutSample = NULL;
    }

    return hr;
}


/*****************************Private*Routine******************************\
* CopySampleToBuffer
*
* Copies the sample to the input buffer and returns the number of bytes
* present in the buffer.  Updates m_BufferStart, m_BufferCurr and
* m_BufferFullness to reflect any wrapping in the buffer
* that may have occurred.
*
\**************************************************************************/
HRESULT
CMpegVideoCodec::CopySampleToBuffer(
    IMediaSample *pSample
    )
{
    BYTE    *pSrc;
    long    LenLeftInBuffer;
    long    AmountToCopy;
    HRESULT hr;

    if (m_pSample == NULL) {

        hr = pSample->GetPointer(&pSrc);
        if (FAILED(hr)) {
            return hr;
        }
        ASSERT(pSrc);

        // Skip pass the stream header and extract the packet length.
        //
        if (m_bPayloadOnly) {
            m_pSample = pSrc;
            m_LenLeftInPacket = pSample->GetActualDataLength();
        }
        else {
            m_pSample = SkipToPacketData(pSrc, m_LenLeftInPacket);
            if (m_pSample == NULL) {
                return E_INVALIDARG;
            }
        }
    }


    //
    // Move what remains in the video data buffer to the top of the buffer
    // and append the new video data to it.
    // Don't ever let it fill up completely or we'll have to worry about
    // full vs empty
    //
    LenLeftInBuffer = m_VBlockSize - m_BufferFullness;

    AmountToCopy = min(LenLeftInBuffer, m_LenLeftInPacket);
    CopyMemory(m_BufferCurr + m_BufferFullness, m_pSample, AmountToCopy);


    m_LenLeftInPacket -= AmountToCopy;
    m_pSample         += AmountToCopy;
    m_BufferFullness  += AmountToCopy;

    return S_OK;
}


/*****************************Private*Routine******************************\
* UpdateBufferPosition
*
* Updates m_BufferCurr from the new position passed in.
* Checks if m_BufferCurr is in the 'virtual' space at the end of the buffer
* and if it is adjusts m_BufferCurrent and m_BufferFullness down by the real
* buffer size
*
\**************************************************************************/
void
CMpegVideoCodec::UpdateBufferPosition(
    LPBYTE lpNewCurrent
    )
{
    ASSERT(lpNewCurrent >= m_BufferCurr);

    //
    // Sometimes the position gets reported as 1 too many (!)
    //
    if (lpNewCurrent > m_BufferCurr + m_BufferFullness) {
        lpNewCurrent = m_BufferCurr + m_BufferFullness;
    }
    m_BufferFullness -= lpNewCurrent - m_BufferCurr;
    if (lpNewCurrent >= (m_BufferStart + m_VBlockSize)) {
        lpNewCurrent -= m_VBlockSize;
    }

    //
    // Fix up the time code list
    //
    while (TRUE) {

        POSITION pos = m_PtsQueue.GetHeadPosition();
        if (pos == NULL) {
            break;
        }

        CTimePosition *pTimePos = m_PtsQueue.Get(pos);

        if (BuffOffset(pTimePos->m_BufferPosition) < BuffOffset(lpNewCurrent)) {
            delete pTimePos;
            m_PtsQueue.RemoveHead();
        }
        else {
            break;
        }
    }

    //
    //  Advance to the new position
    //
    // m_BufferCurr = lpNewCurrent;
    MoveMemory(m_BufferCurr, lpNewCurrent, m_BufferFullness);
}




/*****************************Private*Routine******************************\
* UpdateTimeSyncPoint
*
* Each time we get a media sample with a Pts
* time stamp we add a TIMEPOSITION entry to a queue of time positions.
* Each time we decode an I frame we record the starting and ending position
* of the I frame picture within the input buffer, this information is
* then used to find a suitable time stamp to associate with the frame.  If
* a suitable time stamp cannot be found or the frame is not an I frame we
* calculate a suitable time code by extrapolation.
*
\**************************************************************************/
BOOL
CMpegVideoCodec::UpdateTimeSyncPoint(
    LPBYTE lpPicStart,
    REFERENCE_TIME *Time
    )
{
    POSITION  pos = m_PtsQueue.GetHeadPosition();
    BOOL      bFound;

    for (bFound = FALSE; pos != NULL; ) {
        CTimePosition *pTimePos = m_PtsQueue.GetNext(pos);

        if (BuffOffset(pTimePos->m_BufferPosition) <= BuffOffset(lpPicStart)) {

            //  Buffer start time stamp could be for us (but keep looking
            //  in case there's a better one)
            //
            //  NOTE this ASSUMES there are no packets with time stamps but no
            //  start code so in fact this one should be ours

            bFound = TRUE;
            *Time  = pTimePos->m_PtsTimeStamp;
            delete pTimePos;
            m_PtsQueue.RemoveHead();
        }
        else {
            break;
        }
    }

    if (bFound) {
        DbgLog((LOG_TRACE, 3,
                TEXT("CMpegVideoCodec::UpdateTimeSyncPoint() : Found time %s"),
                (LPCTSTR)CDisp(*Time) ));
    }

    return bFound;
}



/*****************************Private*Routine******************************\
* BuffOffset
*
* Adjusts the supplied offset so that is based upon m_BufferCurr
*
\**************************************************************************/
inline ptrdiff_t
CMpegVideoCodec::BuffOffset(
    LPBYTE Offset
    )
{
    ptrdiff_t x = Offset - m_BufferCurr;
    if (x < 0) {
        x += m_VBlockSize;
    }
    return x;
}


/******************************Public*Routine******************************\

* CheckInputType
*
* Check if you can support mtIn
*
\**************************************************************************/
HRESULT
CMpegVideoCodec::CheckInputType(
    const CMediaType* pmtIn
    )
{
    DbgLog((LOG_TRACE, 2, TEXT("CMpegVideoCodec::CheckInputType")));

    //
    //  Check for native streams
    //

    if (*pmtIn->Type() == MEDIATYPE_Stream &&
        *pmtIn->Subtype() == MEDIASUBTYPE_MPEG1Video) {

        //
        //  If there's no format block we'll read the stream during
        //  CompleteConnect()
        //
        if (pmtIn->cbFormat == 0) {
            return S_OK;
        }
    }
    else {
        //
        // check this is an MPEG video format type
        //
        if (*pmtIn->FormatType() != FORMAT_MPEGVideo) {
            return E_INVALIDARG;
        }

        //
        // we only support MEDIATYPE_Video
        //
        if (*pmtIn->Type() != MEDIATYPE_Video) {
            return E_INVALIDARG;
        }

        if (*pmtIn->Subtype() != MEDIASUBTYPE_MPEG1Packet &&
            *pmtIn->Subtype() != MEDIASUBTYPE_MPEG1Payload) {
            return E_INVALIDARG;
        }
    }

    if (pmtIn->cbFormat < SIZE_VIDEOHEADER + sizeof(DWORD) ||
        pmtIn->cbFormat < SIZE_MPEG1VIDEOINFO((MPEG1VIDEOINFO *)pmtIn->pbFormat)) {
        return E_INVALIDARG;
    }

    //
    // Check the sequence header and save the info
    //

    MPEG1VIDEOINFO* videoInfo = (MPEG1VIDEOINFO *)pmtIn->pbFormat;
    if (!ParseSequenceHeader(videoInfo->bSequenceHeader,
                             videoInfo->cbSequenceHeader, &m_seqInfo)) {
        return E_INVALIDARG;
    }

    return S_OK;
}


/******************************Public*Routine******************************\
* CheckTransform
*
* Check if you can support the transform from this input to this output
*
\**************************************************************************/
HRESULT
CMpegVideoCodec::CheckTransform(
    const CMediaType* pmtIn,
    const CMediaType* pmtOut
    )
{
    DbgLog((LOG_TRACE, 2, TEXT("CMpegVideoCodec::CheckTransform")));

    // we only output video
    if (*pmtOut->Type() != MEDIATYPE_Video) {
        return VFW_E_TYPE_NOT_ACCEPTED;
    }

    // Check there is a format block
    if (*pmtOut->FormatType() != FORMAT_VideoInfo) {
        return VFW_E_TYPE_NOT_ACCEPTED;
    }

    //
    // See if we can use dci/direct draw.
    // First check that there is a non empty target rectangle.
    //

    VIDEOINFO *videoInfo = (VIDEOINFO *)pmtOut->pbFormat;
    if (!IsRectEmpty(&videoInfo->rcTarget)) {

        //
        // Next, check that the source rectangle is the entire movie.
        //

        if ( videoInfo->rcSource.left   == 0
          && videoInfo->rcSource.top    == 0
          && videoInfo->rcSource.right  == m_seqInfo.lWidth
          && videoInfo->rcSource.bottom == m_seqInfo.lHeight) {

            //
            // Now check that the target rectangles size is the same as
            // the movies, that is there is no stretching or shrinking.
            //

            if ( (videoInfo->rcTarget.right - videoInfo->rcTarget.left)
                    == m_seqInfo.lWidth
              && (videoInfo->rcTarget.bottom - videoInfo->rcTarget.top)
                    == m_seqInfo.lHeight) {
#ifndef _X86_
                // On Risc machines make sure we are DWORD aligned
                if ((videoInfo->rcTarget.left & 0x03) == 0x00)
#endif
                {
                    DbgLog((LOG_TRACE, 2, TEXT("Using DCI")));
                    return S_OK;
                }
            }
        }
        DbgLog((LOG_TRACE, 2, TEXT("NOT Using DCI")));
        return E_FAIL;
    }


    return S_OK;
}


/******************************Public*Routine******************************\
* SetMediaType
*
* Overriden to know when the media type is actually set
*
\**************************************************************************/
HRESULT
CMpegVideoCodec::SetMediaType(
    PIN_DIRECTION direction,
    const CMediaType *pmt
    )
{
    DbgLog((LOG_TRACE, 2, TEXT("CMpegVideoCodec::SetMediaType")));

    if (direction == PINDIR_INPUT) {

        //
        // Get the data type
        //
        if (*pmt->Subtype() == MEDIASUBTYPE_MPEG1Packet) {
            m_bPayloadOnly = FALSE;
        }
        else {
            ASSERT(*pmt->Subtype() == MEDIASUBTYPE_MPEG1Payload ||
                   *pmt->Subtype() == MEDIASUBTYPE_MPEG1Video);
            m_bPayloadOnly = TRUE;
        }
    }
    else {
        SetOutputPinMediaType(pmt);
    }
    return S_OK;
}


/*****************************Private*Routine******************************\
* SetOutputPinMediaType
*
* This function is a static member function of CMpegVideoCodec, this is so that
* it can be called from SetMediaType (above) and from the static member
* function GetDecodeBufferAndFormat.  Note that we pass in the "this" pointer
* explicitly.
*
\**************************************************************************/
void
CMpegVideoCodec::SetOutputPinMediaType(
    const CMediaType *pmt
    )
{
    VIDEOINFO   *pvi;
    LONG        lStride;
    LONG        lOffset;

    if (*pmt->Subtype() == MEDIASUBTYPE_Y41P) {
        m_dwOutputFormatDib = MM_411PK;
        m_dwOutputFormatDdb = MM_411PK;
    }
    else if (*pmt->Subtype() == MEDIASUBTYPE_YUY2) {
        m_dwOutputFormatDib = MM_422PK;
        m_dwOutputFormatDdb = MM_422PK;
    }
    else if (*pmt->Subtype() == MEDIASUBTYPE_UYVY) {
        m_dwOutputFormatDib = MM_422SPK;
        m_dwOutputFormatDdb = MM_422SPK;
    }
    else if (*pmt->Subtype() == MEDIASUBTYPE_RGB24) {
        m_dwOutputFormatDib = MM_RGB24_DIB;
        m_dwOutputFormatDdb = MM_RGB24_DDB;
    }
    else if (*pmt->Subtype() == MEDIASUBTYPE_RGB565) {
        m_dwOutputFormatDib = MM_RGB565_DIB;
        m_dwOutputFormatDdb = MM_RGB565_DDB;
    }
    else if (*pmt->Subtype() == MEDIASUBTYPE_RGB555) {
        m_dwOutputFormatDib = MM_RGB555_DIB;
        m_dwOutputFormatDdb = MM_RGB555_DDB;
    }
    else {
        ASSERT(*pmt->Subtype() == MEDIASUBTYPE_RGB8);
        if (m_PaletteType == COLOUR_PALETTE) {
            m_dwOutputFormatDib = MM_RGB8_DIB;
            m_dwOutputFormatDdb = MM_RGB8_DDB;
        }
        else {
            m_dwOutputFormatDib = MM_Y_DIB;
            m_dwOutputFormatDdb = MM_Y_DDB;
        }
    }

    //
    // lStride is the distance between in bytes between a pel on the
    // screen and the pel directly underneath it.
    //

    pvi = (VIDEOINFO *)pmt->pbFormat;
    lStride = ((pvi->bmiHeader.biWidth * pvi->bmiHeader.biBitCount) + 7) / 8;
    lStride = (lStride + 3) & ~3;


    //
    // lOffset is the distance in bytes from the top corner of the
    // target bitmap to the top corner of the video image.  When we are
    // using DIBs this value allways be zero.
    //
    // When we are using DCI/DirectDraw this value will only be zero if
    // we are drawing the video image at the top left hand corner of the
    // display.
    //

    lOffset = (((pvi->rcTarget.left * pvi->bmiHeader.biBitCount) + 7) / 8) +
                (pvi->rcTarget.top * lStride);

    m_VideoControl.dwOutStride = lStride;
    m_VideoControl.dwOutOffset = lOffset;


    //
    // See what orientation we need to use when colour converting
    // the frame.
    //

    if (pvi->bmiHeader.biHeight > 0) {
        m_VideoControl.dwOutputFormat = m_dwOutputFormatDib;
    }
    else {
        m_VideoControl.dwOutputFormat = m_dwOutputFormatDdb;
    }
}


/******************************Public*Routine******************************\
* GetMediaType
*
* Return our preferred output media types (in order)
*
\**************************************************************************/
HRESULT
CMpegVideoCodec::GetMediaType(
    int iPosition,
    CMediaType *pmt
    )
{
    VIDEOINFO   *pVideoInfo;
    CMediaType  cmt;

    DbgLog((LOG_TRACE, 2, TEXT("CMpegVideoCodec::GetMediaType")));

    if (iPosition < 0) {
        return E_INVALIDARG;
    }

    //
    // Quick hack to enable greyscale for Robin
    //
    if (m_PaletteType == GREY_PALETTE) {

        if (iPosition > 0) {
            return VFW_S_NO_MORE_ITEMS;
        }

        iPosition = MT_RGB8;
    }

    //
    // We copy the proposed output format so that we can play around with
    // it all we like and still leave the original preferred format
    // untouched.  We try each of the known BITMAPINFO types in turn
    // starting off with the best quality moving through to the worst
    // (palettised) format
    //

    cmt = m_pInput->CurrentMediaType();

    if (*cmt.Type() != MEDIATYPE_Video) {
        pVideoInfo = (VIDEOINFO *)cmt.ReallocFormatBuffer(SIZE_PREHEADER);
        if (pVideoInfo == NULL) {
            return E_OUTOFMEMORY;
        }
        pVideoInfo->rcSource.top = 0;
        pVideoInfo->rcSource.left = 0;
        pVideoInfo->rcSource.right = m_seqInfo.lWidth;
        pVideoInfo->rcSource.bottom = m_seqInfo.lHeight;
        pVideoInfo->AvgTimePerFrame = m_seqInfo.tPictureTime;
        pVideoInfo->rcTarget = pVideoInfo->rcSource;
    }
    else {
        pVideoInfo = (VIDEOINFO *) cmt.Format();
        ASSERT(pVideoInfo != NULL);
    }

    //
    // Fill in the output format according to requested position, see the
    // Media Type enum in mpgvideo.h for the list of supported types and
    // their positions.
    //
    switch (iPosition) {
    case MT_Y41P:
        pVideoInfo = (VIDEOINFO *)cmt.ReallocFormatBuffer(SIZE_VIDEOHEADER);
        if (pVideoInfo == NULL) {
            return E_OUTOFMEMORY;
        }
        InitDestinationVideoInfo(pVideoInfo, MAKEFOURCC('Y','4','1','P'), 12);

        *pmt = cmt;
        pmt->SetSubtype(&MEDIASUBTYPE_Y41P);
        break;

    case MT_YUY2:
        pVideoInfo = (VIDEOINFO *)cmt.ReallocFormatBuffer(SIZE_VIDEOHEADER);
        if (pVideoInfo == NULL) {
            return E_OUTOFMEMORY;
        }
        InitDestinationVideoInfo(pVideoInfo, MAKEFOURCC('Y','U','Y','2'), 16);

        *pmt = cmt;
        pmt->SetSubtype(&MEDIASUBTYPE_YUY2);
        break;

    case MT_UYVY:
        pVideoInfo = (VIDEOINFO *)cmt.ReallocFormatBuffer(SIZE_VIDEOHEADER);
        if (pVideoInfo == NULL) {
            return E_OUTOFMEMORY;
        }
        InitDestinationVideoInfo(pVideoInfo, MAKEFOURCC('U','Y','V','Y'), 16);

        *pmt = cmt;
        pmt->SetSubtype(&MEDIASUBTYPE_UYVY);
        break;

    case MT_RGB24:
        pVideoInfo = (VIDEOINFO *)cmt.ReallocFormatBuffer(SIZE_VIDEOHEADER);
        if (pVideoInfo == NULL) {
            return E_OUTOFMEMORY;
        }
        InitDestinationVideoInfo(pVideoInfo, BI_RGB, 24);

        *pmt = cmt;
        pmt->SetSubtype(&MEDIASUBTYPE_RGB24);
        break;

    case MT_RGB565:
        pVideoInfo = (VIDEOINFO *)cmt.ReallocFormatBuffer(SIZE_VIDEOHEADER +
                                                          SIZE_MASKS);
        if (pVideoInfo == NULL) {
            return E_OUTOFMEMORY;
        }

        InitDestinationVideoInfo(pVideoInfo, BI_BITFIELDS, 16);

        DWORD *pdw;
        pdw = (DWORD *)(HEADER(pVideoInfo) + 1);
        pdw[iRED]   = bits565[iRED];
        pdw[iGREEN] = bits565[iGREEN];
        pdw[iBLUE]  = bits565[iBLUE];

        *pmt = cmt;
        pmt->SetSubtype(&MEDIASUBTYPE_RGB565);
        break;

    case MT_RGB555:
        pVideoInfo = (VIDEOINFO *)cmt.ReallocFormatBuffer(SIZE_VIDEOHEADER);
        if (pVideoInfo == NULL) {
            return E_OUTOFMEMORY;
        }
        InitDestinationVideoInfo(pVideoInfo, BI_RGB, 16);

        *pmt = cmt;
        pmt->SetSubtype(&MEDIASUBTYPE_RGB555);
        break;

    case MT_RGB8:
        pVideoInfo = (VIDEOINFO *)cmt.ReallocFormatBuffer(
                                           SIZE_VIDEOHEADER + SIZE_PALETTE);
        if (pVideoInfo == NULL) {
            return E_OUTOFMEMORY;
        }
        InitDestinationVideoInfo(pVideoInfo, BI_RGB, 8);
        InitDestinationPalette(pVideoInfo);

        *pmt = cmt;
        pmt->SetSubtype(&MEDIASUBTYPE_RGB8);
        break;

    default:
        return VFW_S_NO_MORE_ITEMS;

    }

    //
    // This block assumes that lpbi has been set up to point to a valid
    // bitmapinfoheader and that cmt has been copied into *pmt.
    // This is taken care of in the switch statement above.  This should
    // kept in mind when new formats are added.
    //
    pmt->SetType(&MEDIATYPE_Video);
    pmt->SetFormatType(&FORMAT_VideoInfo);

    //
    // we assume the output format is uncompressed
    //
    pmt->SetTemporalCompression(FALSE);
    pmt->SetSampleSize(HEADER(pVideoInfo)->biSizeImage);
    return S_OK;
}


/*****************************Private*Routine******************************\
* InitDestinationVideoInfo
*
* Fills in common video and bitmap info header fields
*
\**************************************************************************/
void
CMpegVideoCodec::InitDestinationVideoInfo(
    VIDEOINFO *pVideoInfo,
    DWORD dwComppression,
    int nBitCount
    )
{
    LPBITMAPINFOHEADER lpbi = HEADER(pVideoInfo);
    lpbi->biSize          = sizeof(BITMAPINFOHEADER);
    lpbi->biWidth         = m_seqInfo.lWidth;
    lpbi->biHeight        = m_seqInfo.lHeight;
    lpbi->biPlanes        = 1;
    lpbi->biBitCount      = nBitCount;
    lpbi->biXPelsPerMeter = 0;
    lpbi->biYPelsPerMeter = 0;
    lpbi->biCompression   = dwComppression;
    lpbi->biSizeImage     = GetBitmapSize(lpbi);

    //
    // The "bit" rate is image size in bytes times 8 (to convert to bits)
    // divided by the AvgTimePerFrame.  This result is in bits per 100 nSec,
    // so we multiply by 10000000 to convert to bits per second, this multiply
    // is combined with "times" 8 above so the calculations becomes:
    //
    // BitRate = (biSizeImage * 80000000) / AvgTimePerFrame
    //
    LARGE_INTEGER li;
    li.QuadPart = pVideoInfo->AvgTimePerFrame;
    pVideoInfo->dwBitRate = MulDiv(lpbi->biSizeImage, 80000000, li.LowPart);
    pVideoInfo->dwBitErrorRate = 0L;

}


/*****************************Private*Routine******************************\
* InitDestinationPalette
*
* Creates a standard colour palette or gray scale palette as determined
* by the PaletteType parameter.
*
\**************************************************************************/
void
CMpegVideoCodec::InitDestinationPalette(
    VIDEOINFO *pVideoInfo
    )
{
    int             i;
    int             iPalLowEnd;
    int             iPalHiStart;
    PALETTEENTRY    pal[24];
    HDC             hDC;

    if (m_PaletteType == COLOUR_PALETTE) {
        for ( i = 0; i < 256; i++ ) {
            pVideoInfo->bmiColors[i].rgbRed      = PaletteData[i].r;
            pVideoInfo->bmiColors[i].rgbBlue     = PaletteData[i].b;
            pVideoInfo->bmiColors[i].rgbGreen    = PaletteData[i].g;
            pVideoInfo->bmiColors[i].rgbReserved = 0;
        }
        iPalLowEnd  = 16;
        iPalHiStart = 232;
    }
    else {
        for ( i = 0; i < 256; i++ ) {
            pVideoInfo->bmiColors[i].rgbRed      = i;
            pVideoInfo->bmiColors[i].rgbBlue     = i;
            pVideoInfo->bmiColors[i].rgbGreen    = i;
            pVideoInfo->bmiColors[i].rgbReserved = 0;
        }
        iPalLowEnd  = 10;
        iPalHiStart = 246;
    }

    hDC = GetDC(GetDesktopWindow());
    GetSystemPaletteEntries(hDC, 0, 16, &pal[0] );
    for ( i = 0; i < iPalLowEnd; i++ ) {

        pVideoInfo->bmiColors[i].rgbRed   = pal[i].peRed;
        pVideoInfo->bmiColors[i].rgbGreen = pal[i].peGreen;
        pVideoInfo->bmiColors[i].rgbBlue  = pal[i].peBlue;
    }

    GetSystemPaletteEntries(hDC, iPalHiStart, 256 - iPalHiStart, &pal[0] );
    for ( i = iPalHiStart; i < 256; i++ ) {

        pVideoInfo->bmiColors[i].rgbRed   = pal[i - iPalHiStart].peRed;
        pVideoInfo->bmiColors[i].rgbGreen = pal[i - iPalHiStart].peGreen;
        pVideoInfo->bmiColors[i].rgbBlue  = pal[i - iPalHiStart].peBlue;
    }
    ReleaseDC(GetDesktopWindow(), hDC);


    HEADER(pVideoInfo)->biClrUsed = 256;
    HEADER(pVideoInfo)->biClrImportant = 0;
}


/******************************Public*Routine******************************\
* DecideBufferSize
*
* Called from CBaseOutputPin to prepare the allocator's count
* of buffers and sizes
*
\**************************************************************************/
HRESULT
CMpegVideoCodec::DecideBufferSize(
    IMemAllocator * pAllocator,
    ALLOCATOR_PROPERTIES * pProperties
    )
{
    DbgLog((LOG_TRACE, 2, TEXT("CMpegVideoCodec::DecideBufferSize")));

    ASSERT(pAllocator);
    ASSERT(pProperties);
    HRESULT hr = NOERROR;

    pProperties->cBuffers = 1;
    pProperties->cbBuffer = m_pOutput->CurrentMediaType().GetSampleSize();

    ASSERT(pProperties->cbBuffer);
    DbgLog((LOG_TRACE, 2, TEXT("Sample size = %ld\n"), pProperties->cbBuffer));

    // Ask the allocator to reserve us some sample memory, NOTE the function
    // can succeed (that is return NOERROR) but still not have allocated the
    // memory that we requested, so we must check we got whatever we wanted

    ALLOCATOR_PROPERTIES Actual;
    hr = pAllocator->SetProperties(pProperties,&Actual);
    if (FAILED(hr)) {
        return hr;
    }

    ASSERT(Actual.cbAlign == 1);
    ASSERT(Actual.cbPrefix == 0);

    if (Actual.cbBuffer < pProperties->cbBuffer ||
        Actual.cBuffers < pProperties->cBuffers) {

            // can't use this allocator
            return E_INVALIDARG;
    }
    return S_OK;
}


/******************************Public*Routine******************************\
* StartStreaming
*
*
*
\**************************************************************************/
HRESULT
CMpegVideoCodec::StartStreaming(
    void
    )
{
    CAutoLock   lock(&m_csFilter);
    long        Size;
    // HRESULT     hr;

    ASSERT(MEDIASUBTYPE_RGB8   == *m_pOutput->CurrentMediaType().Subtype() ||
           MEDIASUBTYPE_RGB555 == *m_pOutput->CurrentMediaType().Subtype() ||
           MEDIASUBTYPE_RGB565 == *m_pOutput->CurrentMediaType().Subtype() ||
           MEDIASUBTYPE_Y41P   == *m_pOutput->CurrentMediaType().Subtype() ||
           MEDIASUBTYPE_YUY2   == *m_pOutput->CurrentMediaType().Subtype() ||
           MEDIASUBTYPE_UYVY   == *m_pOutput->CurrentMediaType().Subtype() ||
           MEDIASUBTYPE_RGB24  == *m_pOutput->CurrentMediaType().Subtype());

    ASSERT(m_pFrameBuff == NULL);
    ASSERT(m_pVideoDecoder == NULL);
    ASSERT(m_Buffer == NULL);

    //
    // Create the video codec - if we fail to open the codec it is
    // probably because the codec was unable to allocate memory for its
    // decoding tables.
    //
    m_pVideoDecoder = new CVideoDecoder(this);
    if (m_pVideoDecoder == NULL) {
        return E_OUTOFMEMORY;
    }


    //
    // Allocate some space for the codecs I and P frame buffer store
    //
    Size = m_seqInfo.lWidth * m_seqInfo.lHeight;
    m_pFrameBuff = new BYTE[ 3 * (Size + (2 * (Size / 4)))];
    if (m_pFrameBuff == NULL) {

        delete m_pVideoDecoder;
        m_pVideoDecoder = NULL;

        return E_OUTOFMEMORY;
    }

    m_VBlockSize = m_seqInfo.lvbv;
    m_VBlockSize +=  ((1 << 16) - 1);
    m_VBlockSize &= ~((1 << 16) - 1);

    // m_Buffer = new CCircularBuffer(m_VBlockSize, m_VBlockSize, hr);
    m_Buffer = new BYTE[m_VBlockSize];
    if (m_Buffer == NULL) {

        delete [] m_pFrameBuff;
        m_pFrameBuff = NULL;

        delete m_pVideoDecoder;
        m_pVideoDecoder = NULL;

        return E_OUTOFMEMORY;
    }

    //
    // Initialize the mpeg video codec
    //
    m_dwLateBy = 0;
    m_dwCtrl = (m_dwCtrlDefault | m_dwQualDefault);
    m_VideoControl.dwCtrl = m_dwCtrl;
    m_VideoControl.dwYStride = m_seqInfo.lWidth;
    m_VideoControl.dwYLines = m_seqInfo.lHeight;

    ZeroMemory(m_pFrameBuff, Size);
    m_VideoControl.pFrameBuff = m_pFrameBuff;


    //
    // Now reset the mpeg video decoder
    //
    ResetVideoDecoder();


    //
    // Reset frame stats
    //
    ZeroMemory(m_dwFramesSkipped, sizeof(m_dwFramesSkipped));
    ZeroMemory(m_dwFramesDecoded, sizeof(m_dwFramesDecoded));

    return S_OK;
}


/******************************Public*Routine******************************\
* StopStreaming
*
*
*
\**************************************************************************/
HRESULT
CMpegVideoCodec::StopStreaming(
    void
    )
{
    CAutoLock       lock(&m_csFilter);
    CAutoLock       lck(&m_csReceive);

    CTimePosition   *pTimePos;

    ASSERT(m_pFrameBuff != NULL);
    ASSERT(m_pVideoDecoder != NULL);
    ASSERT(m_Buffer != NULL);

    delete m_pVideoDecoder;
    m_pVideoDecoder = NULL;

    delete [] m_pFrameBuff;
    m_pFrameBuff = NULL;

    delete [] m_Buffer;
    m_Buffer = NULL;


    //
    // Purge the PTS queue.
    //
    DbgLog((LOG_TRACE, 2,
            TEXT("Freeing %d time entries"), m_PtsQueue.GetCount()));

    while( (pTimePos = m_PtsQueue.RemoveHead()) != NULL) {
        delete pTimePos;
    }

    return S_OK;
}

/******************************Public*Routine******************************\
* Notify
*
* Handle quality control notifications sent to us
*
\**************************************************************************/
HRESULT
CMpegVideoCodec::AlterQuality(
    Quality q
    )
{
    DWORD dwNewCtrl;
    LARGE_INTEGER li;
    li.QuadPart = q.Late;
    DWORD dwTimeLate = li.LowPart;

    MSR_INTEGER(m_QualMsg, dwTimeLate);
    li.QuadPart = m_seqInfo.tPictureTime;

    DbgLog((LOG_TRACE, 2,
            TEXT("Q = %s Prop = %4.4ld Late = %8.8ld Fram = %8.8ld"),
            q.Type == Famine ? TEXT("Famine") : TEXT("Flood "),
            q.Proportion, dwTimeLate,
            li.LowPart));

    //
    // See if the user has overidden quality messages
    //
    if (m_IgnoreQualityMessage) {
        return S_OK;
    }

    //
    // Turn off the old option and then bring in the new.
    //

    m_dwLateBy = dwTimeLate;

    if (dwTimeLate <= 750000L) {
        DbgLog((LOG_TRACE, 2, TEXT("Default QL")));
        dwNewCtrl = m_dwCtrlDefault;
    }
    else if (dwTimeLate <= (7L * 330000L)) {
        DbgLog((LOG_TRACE, 2, TEXT("IP")));
        dwNewCtrl = DECODE_IP;
    }
    else {
        DbgLog((LOG_TRACE, 2, TEXT("I")));
        dwNewCtrl = DECODE_I;
    }

    m_dwCtrl = (m_dwQualDefault | dwNewCtrl);

    return S_OK;
}


// -------------------------------------------------------------------------
// IP Tracking class
// -------------------------------------------------------------------------
//
inline CMpegVideoCodec::CNextIP::CNextIP()
{
    Reset();
}

inline void
CMpegVideoCodec::CNextIP::Set(
    DWORD dwSkipFlag,
    BOOL bIFrame,
    BOOL bTimeSet,
    REFERENCE_TIME t,
    DWORD dwTemporalReference
    )
{
    if (m_bGotFirst) {

        m_bTimeToDraw = FALSE;
        NextRef(dwTemporalReference); // Might already be time if no Bs
    }
    else {
        if (bIFrame) {
            m_bGotFirst   = TRUE;
            m_bTimeToDraw = TRUE;
        }
    }
    m_dwSkipFlag          = dwSkipFlag;
    m_bTimeSet            = bTimeSet;
    m_t                   = t;
    m_dwTemporalReference = dwTemporalReference;

}

inline BOOL
CMpegVideoCodec::CNextIP::GotFirst()
{
    return m_bGotFirst;
}

inline void
CMpegVideoCodec::CNextIP::NextRef(
    DWORD dwTemporalReference
    )
{
    //  See if we're next after this one
    //  Don't set m_bTimeToDraw = FALSE here because in the case
    //  where we're stepping through undrawable B-frames before the
    //  first I-Frame we've already set m_bTimeToDraw = TRUE
    if (((dwTemporalReference + 1) & 1023) == m_dwTemporalReference) {
        m_bTimeToDraw = TRUE;
    }
    DbgLog((LOG_TRACE, 3,
            TEXT("New Temporal Reference %d, NextIP = %d, TimeToDraw = %d"),
            dwTemporalReference, m_dwTemporalReference,
            m_bTimeToDraw));
}


inline BOOL
CMpegVideoCodec::CNextIP::GetTime(
    REFERENCE_TIME *pt
    )
{
    if (m_bTimeSet) {
        *pt = m_t;
    }
    return m_bTimeSet;
}

inline BOOL
CMpegVideoCodec::CNextIP::TimeToDraw()  const
{
    return !m_dwSkipFlag && m_bTimeToDraw;
}

inline void
CMpegVideoCodec::CNextIP::Reset()
{
    m_bGotFirst = FALSE;
    m_bTimeSet = FALSE;
    m_dwTemporalReference = 0;
    m_bTimeToDraw = FALSE;
}





/******************************Public*Routine******************************\
* SkipToPacketData
*
*
*
\**************************************************************************/
LPBYTE
SkipToPacketData(
    LPBYTE pSrc,
    long &LenLeftInPacket
    )
{
    LPBYTE  lpPacketStart;
    DWORD   bData;
    long    Length;


    //
    // Skip the stream ID and extract the packet length
    //
    pSrc += 4;
    bData = *pSrc++;
    Length = (long)((bData << 8) + *pSrc++);
    DbgLog((LOG_TRACE, 3, TEXT("Packet length %ld"), Length ));


    //
    // Record position of first byte after packet length
    //
    lpPacketStart = pSrc;


    //
    // Remove stuffing bytes
    //
    for (; ; ) {
        bData = *pSrc++;
        if (!(bData & 0x80)) {
            break;
        }
    }

    if ((bData & 0xC0) == 0x40) {
        pSrc++;
        bData = *pSrc++;
    }

    switch (bData & 0xF1) {

    case 0x21:
        pSrc += 4;
        break;

    case 0x31:
        pSrc += 9;
        break;

    default:
        if (bData != 0x0F) {
            DbgLog((LOG_TRACE, 2, TEXT("Invalid packet - 0x%2.2X\n"), bData));
            return NULL;
        }
    }

    //
    // The length left in the packet is the original length of the packet
    // less those bytes that we have just skipped over.
    //
    LenLeftInPacket = Length - (pSrc - lpPacketStart);
    return pSrc;
}



#ifdef DEBUG
LPCTSTR PictureTypes[8] = {
    TEXT("forbidden frame type"),
    TEXT("I-Frame"),
    TEXT("P-Frame"),
    TEXT("B-Frame"),
    TEXT("D-Frame"),
    TEXT("Reserved frame type"),
    TEXT("Reserved frame type"),
    TEXT("Reserved frame type")
};

LPCTSTR PelAspectRatios[16] = {
    TEXT("Forbidden"),
    TEXT("1.0000 - VGA etc"),
    TEXT("0.6735"),
    TEXT("0.7031 - 16:9, 625 line"),
    TEXT("0.7615"),
    TEXT("0.8055"),
    TEXT("0.8437 - 16:9, 525 line"),
    TEXT("0.8935"),
    TEXT("0.9375 - CCIR601, 625 line"),
    TEXT("0.9815"),
    TEXT("1.0255"),
    TEXT("1.0695"),
    TEXT("1.1250 - CCIR601, 525 line"),
    TEXT("1.1575"),
    TEXT("1.2015"),
    TEXT("Reserved")
};

LPCTSTR PictureRates[16] = {
    TEXT("Forbidden"),
    TEXT("23.976"),
    TEXT("24"),
    TEXT("25"),
    TEXT("29.97"),
    TEXT("30"),
    TEXT("50"),
    TEXT("59.94"),
    TEXT("60"),
    TEXT("Reserved"),
    TEXT("Reserved"),
    TEXT("Reserved"),
    TEXT("Reserved"),
    TEXT("Reserved"),
    TEXT("Reserved"),
    TEXT("Reserved")
};
#endif // DEBUG

const LONG PictureTimes[16] = {
    0,
    (LONG)((double)10000000 / 23.976),
    (LONG)((double)10000000 / 24),
    (LONG)((double)10000000 / 25),
    (LONG)((double)10000000 / 29.97),
    (LONG)((double)10000000 / 30),
    (LONG)((double)10000000 / 50),
    (LONG)((double)10000000 / 59.94),
    (LONG)((double)10000000 / 60)
};

const LONG AspectRatios[16] = {
    0,
    393700,
    (LONG)(393700.0 * 0.6735),
    (LONG)(393700.0 * 0.7031),
    (LONG)(393700.0 * 0.7615),
    (LONG)(393700.0 * 0.8055),
    (LONG)(393700.0 * 0.8437),
    (LONG)(393700.0 * 0.8935),
    (LONG)(393700.0 * 0.9375),
    (LONG)(393700.0 * 0.9815),
    (LONG)(393700.0 * 1.0255),
    (LONG)(393700.0 * 1.0695),
    (LONG)(393700.0 * 1.1250),
    (LONG)(393700.0 * 1.1575),
    (LONG)(393700.0 * 1.2015),
    0
};

/******************************Public*Routine******************************\
* ParseSequenceHeader
*
*
*
\**************************************************************************/
BOOL
CMpegVideoCodec::ParseSequenceHeader(
    const BYTE *pbData,
    LONG lData,
    SEQHDR_INFO *pInfo
    )
{
    ASSERT(*(UNALIGNED DWORD *)pbData == ByteSwap(SEQUENCE_HEADER_CODE));

    //
    // Check random marker bit
    //
    if (!(pbData[10] & 0x20)) {
        DbgLog((LOG_ERROR, 2, TEXT("Sequence header invalid marker bit")));
        return FALSE;
    }

    DWORD dwWidthAndHeight = ((DWORD)pbData[4] << 16) + ((DWORD)pbData[5] << 8) +
                             ((DWORD)pbData[6]);

    pInfo->lWidth = dwWidthAndHeight >> 12;
    pInfo->lHeight = dwWidthAndHeight & 0xFFF;

    DbgLog((LOG_TRACE, 2, TEXT("Width = %d, Height = %d"),
            pInfo->lWidth, pInfo->lHeight));

    //
    // the '8' bit is the scramble flag used by sigma designs - ignore
    //
    BYTE PelAspectRatioAndPictureRate = pbData[7];

    if ((PelAspectRatioAndPictureRate & 0x0F) > 8) {
        PelAspectRatioAndPictureRate &= 0xF7;
    }

    DbgLog((LOG_TRACE, 2, TEXT("Pel Aspect Ratio = %s"),
        PelAspectRatios[PelAspectRatioAndPictureRate >> 4]));
    DbgLog((LOG_TRACE, 2, TEXT("Picture Rate = %s"),
        PictureRates[PelAspectRatioAndPictureRate & 0x0F]));

    if ((PelAspectRatioAndPictureRate & 0xF0) == 0 ||
        (PelAspectRatioAndPictureRate & 0x0F) == 0) {

        DbgLog((LOG_ERROR, 2, TEXT("Sequence header invalid ratio/rate")));
        return FALSE;
    }

    pInfo->tPictureTime =
            (LONGLONG)PictureTimes[PelAspectRatioAndPictureRate & 0x0F];

    pInfo->lTimePerFrame =
            MulDiv((LONG)pInfo->tPictureTime, 9, 1000);

    /*  Pull out the bit rate and aspect ratio for the type */
    pInfo->dwBitRate = ((((DWORD)pbData[8] << 16) + ((DWORD)pbData[9] << 8) +
                          (DWORD)pbData[10]) >> 6);

    if (pInfo->dwBitRate == 0x3FFFF) {

        DbgLog((LOG_TRACE, 2, TEXT("Variable video bit rate")));
        pInfo->dwBitRate = 0;
    }
    else {

        pInfo->dwBitRate *= 400;
        DbgLog((LOG_TRACE, 2, TEXT("Video bit rate is %d bits per second"),
               pInfo->dwBitRate));
    }

    //
    // Get a DC
    //
    HDC hdc = GetDC(GetDesktopWindow());
    ASSERT(hdc != NULL);

    //
    //  Guess (randomly) 39.37 inches per meter
    //
    LONG lNotionalPelsPerMeter =
            MulDiv((LONG)GetDeviceCaps(hdc, LOGPIXELSX), 3937, 100);

    pInfo->lXPelsPerMeter = lNotionalPelsPerMeter;

    pInfo->lYPelsPerMeter = MulDiv(
                              lNotionalPelsPerMeter,
                              AspectRatios[PelAspectRatioAndPictureRate >> 4],
                              10000);
    //
    // Pull out the vbv
    //
    pInfo->lvbv = ((((LONG)pbData[10] & 0x1F) << 5) |
                    ((LONG)pbData[11] >> 3)) * 2048;

    DbgLog((LOG_TRACE, 2, TEXT("vbv size is %d bytes"), pInfo->lvbv));

    //
    // Check constrained parameter stuff
    //
    if (pbData[11] & 0x04) {

        DbgLog((LOG_TRACE, 2, TEXT("Constrained parameter video stream")));

        if (pInfo->lvbv > 40960) {

            DbgLog((LOG_ERROR, 1,
                    TEXT("Invalid vbv (%d) for Constrained stream"),
                    pInfo->lvbv));

            //
            //  Have to let this through too!  bisp.mpg has this
            //  But constrain it since it might be random
            //
            pInfo->lvbv = 40960;

        }

    }
    else {
        DbgLog((LOG_TRACE, 2, TEXT("Non-Constrained parameter video stream")));
    }

    pInfo->lActualHeaderLen = lData;
    CopyMemory((PVOID)pInfo->RawHeader, (PVOID)pbData, pInfo->lActualHeaderLen);
    return TRUE;
}
/******************************Public*Routine******************************\
* exported entry points for registration and
* unregistration (in this case they only call
* through to default implmentations).
*
*
*
* History:
*
\**************************************************************************/
STDAPI
DllRegisterServer()
{
  return AMovieDllRegisterServer2( TRUE );
}

STDAPI
DllUnregisterServer()
{
  return AMovieDllRegisterServer2( FALSE );
}


