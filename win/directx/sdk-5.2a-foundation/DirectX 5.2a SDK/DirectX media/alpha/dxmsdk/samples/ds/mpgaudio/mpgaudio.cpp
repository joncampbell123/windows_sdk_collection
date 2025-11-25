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
* Module Name: MpgAudio.cpp
*
* Implements a prototype Mpeg Audio Software codec.  It just consume
* the passed in packets.
*
*
\**************************************************************************/

#include "MpgAudio.h"

// define the GUIDs for streams and my CLSID in this file
#include <initguid.h>

#include "MpegUids.h"

// setup data

const AMOVIESETUP_MEDIATYPE psudIpPinTypes[] =
{ { &MEDIATYPE_Audio                // clsMajorType
  , &MEDIASUBTYPE_MPEG1Packet  }    // clsMinorType
, { &MEDIATYPE_Audio                // clsMajorType
  , &MEDIASUBTYPE_MPEG1AudioPayload }    // clsMinorType
, { &MEDIATYPE_Stream               // clsMajorType
  , &MEDIASUBTYPE_MPEG1Audio   } }; // clsMinorType

const AMOVIESETUP_MEDIATYPE sudOpPinTypes =
{ &MEDIATYPE_Audio      // clsMajorType
, &MEDIASUBTYPE_PCM };  // clsMinorType

const AMOVIESETUP_PIN psudPins[] =
{ { L"Input"            // strName
  , FALSE               // bRendered
  , FALSE               // bOutput
  , FALSE               // bZero
  , FALSE               // bMany
  , &CLSID_NULL         // clsConnectsToFilter
  , L"Output"           // strConnectsToPin
  , 3                   // nTypes
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
sudMPEGAudio = { &CLSID_CMpegFrameworkAudioCodec // clsID
               , L"MPEG Framework Audio Codec"   // strName
               , 0x00680000                      // dwMerit
               , 2                               // nPins
               , psudPins };                     // lpPin


/* -------------------------------------------------------------------------
** list of class ids and creator functions for class factory
** -------------------------------------------------------------------------
*/
CFactoryTemplate g_Templates[] = {
    { L"MPEG Framework Audio Codec"
    , &CLSID_CMpegFrameworkAudioCodec
    , CMpegAudioCodec::CreateInstance
    , NULL
    , &sudMPEGAudio }
};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);


/* -------------------------------------------------------------------------
** Decoder strings
** -------------------------------------------------------------------------
*/
const TCHAR chAudioChannels[]      = TEXT("AudioChannels");
const TCHAR chAudioFreqDivider[]   = TEXT("AudioFreqDivider");
const TCHAR chAudioQuality[]       = TEXT("AudioQuality");
const TCHAR chAudioQuarterInt[]    = TEXT("AudioQuarterInt");
const TCHAR chAudioBits[]          = TEXT("AudioBits");
const TCHAR chRegistryKey[] =
    TEXT("Software\\Microsoft\\Multimedia\\ActiveMovie Filters\\Sample MPEG Audio Decoder");



// --- CMpegAudioCodec ----------------------------------------
CMpegAudioCodec::CMpegAudioCodec(TCHAR *pName, LPUNKNOWN pUnk, HRESULT *phr)
    : CTransformFilter(pName, pUnk, CLSID_CMpegAudioCodec),
    m_pAudioDecoder(NULL)
{
    m_FreqDiv  = GetDecoderInteger(chAudioFreqDivider, 4);
    m_PrefChan = GetDecoderInteger(chAudioChannels, 1);
    m_Quality  = GetDecoderInteger(chAudioQuality, 0);
    m_QuarterInt = GetDecoderInteger(chAudioQuarterInt, 0);
    m_WordSize = GetDecoderInteger(chAudioBits, 16);
    if (m_QuarterInt) {
        m_FreqDiv = 4;
    }
}

CMpegAudioCodec::~CMpegAudioCodec()
{
    delete m_pAudioDecoder;
    m_pAudioDecoder = NULL;
}


/******************************Public*Routine******************************\
* CreateInstance
*
* this goes in the factory template table to create new instances
*
*
\**************************************************************************/
CUnknown *
CMpegAudioCodec::CreateInstance(
    LPUNKNOWN pUnk,
    HRESULT *phr
    )
{
    DbgLog((LOG_TRACE, 2, TEXT("CMpegAudioCodec::CreateInstance")));
    return new CMpegAudioCodec(TEXT("MPEG Audio Codec Filter"), pUnk, phr);
}


/******************************Public*Routine******************************\
* NonDelegatingQueryInterface
*
* Reveals ISpecifyPropertyPages
*
*
\**************************************************************************/
STDMETHODIMP
CMpegAudioCodec::NonDelegatingQueryInterface(
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
\**************************************************************************/
HRESULT
CMpegAudioCodec::EndOfStream()
{
    DbgLog((LOG_TRACE, 2, TEXT("End of stream called")));
    CAutoLock lck(&m_csReceive);

    //
    // Here we would normally decode the remainder of the buffer.  However,
    // the audio codec is designed such that its buffer never contains
    // any full frames of coded data, so all we have to do is reset the
    // audio codec.
    //

    if (m_pAudioDecoder == NULL) {
        return VFW_E_WRONG_STATE;
    }

    ResetAudioDecoder();
    return CTransformFilter::EndOfStream();
}


/******************************Public*Routine******************************\
* EndFlush
*
*
\**************************************************************************/
HRESULT
CMpegAudioCodec::EndFlush()
{
    DbgLog((LOG_TRACE, 2, TEXT("End flush called")));
    CAutoLock lck(&m_csReceive);

    ResetAudioDecoder();
    return CTransformFilter::EndFlush();
}


/*****************************Private*Routine******************************\
* ProcessDiscontiuity
*
*
\**************************************************************************/
void
CMpegAudioCodec::ProcessDiscontiuity(
    IMediaSample *pSample
    )
{
    ResetAudioDecoder();

    //
    //  Find out what the current stop time is
    //

    //
    //  Get logical duration from upstream
    //
    REFTIME dStart, dStop;
    IMediaPosition *pPosition;

    if (SUCCEEDED(m_pInput->GetConnected()->QueryInterface(IID_IMediaPosition,
                     (void **)&pPosition))
     && SUCCEEDED(pPosition->get_CurrentPosition(&dStart))
     && SUCCEEDED(pPosition->get_StopTime(&dStop)))
    {
        m_tStop = (CRefTime)(COARefTime)dStop - (CRefTime)(COARefTime)dStart;
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


/*****************************Private*Routine******************************\
* ProcessSyncPoint
*
*
\**************************************************************************/
void
CMpegAudioCodec::ProcessSyncPoint(
    IMediaSample *pSample,
    BYTE *pSrc
    )
{
    CRefTime  tStart, tStop;

    pSample->GetTime((REFERENCE_TIME*)&tStart,
                     (REFERENCE_TIME*)&tStop);

    m_TimeAtLastSyncPoint = tStart;
    m_TimeSinceLastSyncPoint = 0;

    //
    // If our buffer contains a partial audio frame adjust the
    // sync point time by one frame.  Note that LookForSyncWord advances
    // the m_lpCurr buffer pointer, if there is no sync word found
    // m_lpCurr should equal m_lpEnd, if the last byte in our audio buffer
    // is the first byte of the audio sync word m_lpCurr will equal
    // m_lpEnd - 1.
    // This can fail because sync words can be found other than at
    // frame start!
    //
    if (LookForSyncWord()) {
        m_TimeAtLastSyncPoint -= m_TimePerFrame;
    }
    else {

        ASSERT(m_lpCurr <= m_lpEnd);

        //
        // Now check that there was not a pertial sync word left in our
        // buffer.
        //
        if (m_lpCurr < m_lpEnd) {
            if ((*m_lpCurr == 0xFF) && (pSrc[0] & 0xF0) == 0xF0) {
                m_TimeAtLastSyncPoint -= m_TimePerFrame;
            }
        }
    }
}



/*****************************Private*Routine******************************\
* DeliverSample
*
*
\**************************************************************************/
HRESULT
CMpegAudioCodec::DeliverSample(
    IMediaSample *pOutSample,
    CRefTime &TimeDecoded,
    int nActBytesWritten
    )
{
    CRefTime tStart, tStop;

    //
    // decompressed frames are always key
    //
    pOutSample->SetSyncPoint(TRUE);
    pOutSample->SetActualDataLength(nActBytesWritten);

    //
    // Extrapolate the correct time stamp.
    //
    tStart = m_TimeAtLastSyncPoint + m_TimeSinceLastSyncPoint;
    if (tStart < (LONG)0) {

        /*  Only play from the start */
    }

    tStop = tStart + TimeDecoded;
    m_TimeSinceLastSyncPoint += TimeDecoded;
    pOutSample->SetTime((REFERENCE_TIME*)&tStart,
                        (REFERENCE_TIME*)&tStop);

    DbgLog((LOG_TRACE, 3, TEXT("Writing %ld bytes with time stamp %s"),
            nActBytesWritten, (LPCTSTR)CDisp(tStart) ));

    return m_pOutput->Deliver(pOutSample);
}


/******************************Public*Routine******************************\
* Receive
*
* Copy the input sample into our buffer.  Loop while the buffer size is
* greater than or equal to the size required to decode a frame of audio data.
* Decode the audio frame and pass it along to the output pin for rendering.
*
*
\**************************************************************************/
HRESULT
CMpegAudioCodec::Receive(
    IMediaSample *pSample
    )
{
    //  Make sure the pin doesn't go inactive on us
    CAutoLock lck(&m_csReceive);
    if (m_pAudioDecoder == NULL) {
        return E_UNEXPECTED;
    }

    IMediaSample    *pOutSample;
    BYTE            *pDst;
    BYTE            *pSrc;
    BYTE            *lpPacket;
    HRESULT         hr;
    long            LenLeftInBuffer = 0L;
    long            LenLeftInPacket;

    DbgLog((LOG_TRACE, 2, TEXT("CMpegAudioCodec::Receive")));

    //
    // Check for a discontinuity, if one is found just reset the decoder
    // and then continue processing the current sample.  We do not have to
    // decode any frames left in the audio buffer because the audio codec
    // always consumes all the complete audio data frames copied to
    // the buffer.
    //
    if (pSample->IsDiscontinuity() == S_OK) {
        ProcessDiscontiuity(pSample);
    }

    hr = pSample->GetPointer(&pSrc);
    if (FAILED(hr)) {
        return hr;
    }

    //
    // Get a pointer to the actual mpeg data and determine the
    // length of data supplied.
    //
    if (m_bPayloadOnly) {
        lpPacket = pSrc;
        LenLeftInPacket = pSample->GetActualDataLength();
    }
    else {
        lpPacket = SkipToPacketData(pSrc, LenLeftInPacket);
        if (lpPacket == NULL) {
            NotifyEvent(EC_STREAM_ERROR_STILLPLAYING, E_INVALIDARG, 0);
            return S_OK;
        }
    }
    DbgLog((LOG_TRACE, 2, TEXT("Left in Packet %ld"), LenLeftInPacket ));


    //
    // If this sample is a sync point we need to update our clock and
    // reset the count of samples received since the last sync point.
    // If our buffer contains a partial audio frame we need to make a suitable
    // adjustment to the sync point time as the time refers to the first audio
    // frame in the current sample which would not necessarly be the first
    // sample decoded.
    //

    if (pSample->IsSyncPoint() == S_OK) {
        ProcessSyncPoint(pSample, pSrc);
    }


    //
    // Now, decode the sample data passed to us.
    //

    do {

        LPBYTE      lpDstEnd;
        CRefTime    TimeDecoded;
        DWORD       rc;
        int         nActBytesWritten = 0;

        GetNextPacketChunk(lpPacket, LenLeftInBuffer, LenLeftInPacket);

        DbgLog((LOG_TRACE, 3, TEXT("Left in Buffer %ld"), LenLeftInBuffer ));
        DbgLog((LOG_TRACE, 3, TEXT("Left in Packet %ld"), LenLeftInPacket ));

        if (LenLeftInBuffer < m_FrameSize && LenLeftInPacket == 0L) {
             break;
        }

        //
        // this may block for an indeterminate amount of time
        //
        hr = m_pOutput->GetDeliveryBuffer(&pOutSample,NULL,NULL,0);
        if (FAILED(hr)) {
            break;
        }
        ASSERT(pOutSample);

        hr = pOutSample->GetPointer(&pDst);
        if (FAILED(hr)) {
            break;
        }
        ASSERT(pDst);


        //
        // Initialize the audio control structure
        //
        m_AudioControl.dwNumFrames   = 1;
        m_AudioControl.dwOutBuffSize = m_pOutput->CurrentMediaType().GetSampleSize();
        m_AudioControl.dwOutBuffUsed = 0;
        m_AudioControl.pCmprRead  = m_lpStart;
        m_AudioControl.pCmprWrite = m_lpEnd;


        //
        // Determine the end of the output buffer so that we don't try to
        // decode data beyond it.  m_BytesPerFrame is the number of bytes
        // one frame of Mpeg audio data will expand to after it has been
        // decoded.
        //
        lpDstEnd = pDst + m_AudioControl.dwOutBuffSize - m_FrameSizeOutput;
        TimeDecoded = 0;


        //
        // While there is an audio frame in the buffer and the frame is
        // complete and there is space in the output buffer to store the
        // decompressed frame, decompress it !!
        //

        while (LookForSyncWord()
           && (m_lpCurr + m_FrameSize + Padding() < m_lpEnd)
           && (pDst <= lpDstEnd)) {

            m_AudioControl.pOutBuffer  = pDst;
            m_AudioControl.pCmprRead   = m_lpCurr;
            m_AudioControl.dwMpegError = 0;

            rc = m_pAudioDecoder->DecodeAudioFrame(&m_AudioControl);
            switch (rc) {

            case 0:
                //
                // We have successfully decoded an audio frame.
                //
                if (m_FrameSize == 0L) {
                    m_FrameSize = (LPBYTE)m_AudioControl.pCmprRead -
                                          m_lpCurr - Padding();
                }
                m_lpCurr = (LPBYTE)m_AudioControl.pCmprRead;
                pDst += m_AudioControl.dwOutBuffUsed;
                nActBytesWritten += m_AudioControl.dwOutBuffUsed;
                m_AudioControl.dwOutBuffSize -= m_AudioControl.dwOutBuffUsed;
                TimeDecoded += m_TimePerFrame;
                break;


            case 2:
                //
                // We did not have enough data available to decode the
                // current audio frame.  This is buffer underflow.
                //
                DbgLog((LOG_ERROR, 1, TEXT("Buffer underflow !!")));

            default:
                //
                // Some sort of error occurred, throw the remainder of the
                // buffer away and skip this packet.
                //
                DbgLog((LOG_ERROR, 1, TEXT("Bad return code from audio codec!")));
                m_lpCurr = m_lpEnd;
                NotifyEvent(EC_STREAM_ERROR_STILLPLAYING, E_INVALIDARG, 0);
                hr = S_OK;
                break;
            }
        }


        //
        // Have we decoded any data ?  If so we need to deliver the data to
        // to the filter that receives our output, usually the audio
        // rendering filter.
        //

        if (TimeDecoded > (LONG)0) {

            hr = DeliverSample(pOutSample, TimeDecoded, nActBytesWritten);
            if (FAILED(hr)) {
                DbgLog((LOG_ERROR, 2,
                        TEXT("CMpegAudioCodec::Deliver failed 0x%8.8X"), hr));
                pOutSample->Release();
                break;
            }
        }

        //
        // Release the output buffer. If the connected pin still needs it,
        // it will have addrefed it itself.
        //
        pOutSample->Release();


        //
        // Stop decoding when we have consumed all the data in the input
        // media sample.
        //

    } while (LenLeftInPacket != 0L);

    //
    //  We notify the filter graph of problems but return S_FALSE
    //  back to the caller to notify end of stream
    //

    // if (hr != S_OK) {
    //     NotifyEvent(hr == S_FALSE ? EC_COMPLETE : EC_ERRORABORT, 0, 0);
    // }
    return hr;
}



/*****************************Private*Routine******************************\
* ResetAudioDecoder
*
*
\**************************************************************************/
void
CMpegAudioCodec::ResetAudioDecoder()
{
    m_pAudioDecoder->ResetAudio();
    m_lpStart = m_lpCurr = m_lpEnd = &m_Buffer[0];
    m_FrameSize = 0L;
}



/******************************Public*Routine******************************\
* GetNextPacketChunk
*
*
\**************************************************************************/
void
CMpegAudioCodec::GetNextPacketChunk(
    LPBYTE  &lpPacket,
    long    &LenLeftInBuffer,
    long    &LenLeftInPacket
    )
{
    long AmountToCopy;

    LenLeftInBuffer = m_lpEnd - m_lpCurr;

    //
    // Move what remains in the audio data buffer to the top of
    // the buffer and append the new audio data to it.
    //
    AmountToCopy = min( (AUDIO_BUFF_SIZE - LenLeftInBuffer), LenLeftInPacket);

    MoveMemory(m_lpStart, m_lpCurr, LenLeftInBuffer);
    CopyMemory(m_lpStart + LenLeftInBuffer, lpPacket,  AmountToCopy);

    LenLeftInPacket -= AmountToCopy;
    lpPacket += AmountToCopy;


    //
    // Adjust the buffer pointers so that m_lpCurr points to
    // the start of the buffer and m_lpEnd points to the last
    // valid audio byte in buffer.
    //
    m_lpCurr = m_lpStart;
    m_lpEnd = m_lpStart + LenLeftInBuffer + AmountToCopy;
    LenLeftInBuffer = m_lpEnd - m_lpCurr;
}


/******************************Public*Routine******************************\
* LookForSyncWord
*
*
\**************************************************************************/
BOOL
CMpegAudioCodec::LookForSyncWord()
{
    /* now look for sync */
    int sm = 0;

    while (m_lpCurr < m_lpEnd && sm < 2)  {

        switch (sm) {
        case 0:
            sm = (*m_lpCurr == 0xff);
            break;

        case 1:
            if ((*m_lpCurr & 0xf0) == 0xf0) sm = 2; /* sync found */
            else sm = (*m_lpCurr == 0xff);
            break;
        }
        m_lpCurr++;
    }

    //
    // When we get here we have either run out of buffer or found the first
    // "sm" bytes of a valid sync word.
    //
    // Don't forget to put back the sync word bytes that we have just
    // read otherwise they would be lost forever.
    //
    m_lpCurr -= sm;

    if (sm < 2) {
        return FALSE;   // sync not found.
    }

    return TRUE;
}


/******************************Public*Routine******************************\
* Padding
*
* Returns 1 if the padding bit is set, zero otherwise
*
*
\**************************************************************************/
int
CMpegAudioCodec::Padding()
{
    DWORD dw1 = *(UNALIGNED DWORD *)m_lpCurr;

    //
    // Determine the audio layer for the given frame
    //
    switch (dw1 & 0x600) {

    case 0x200:     // Layer 1
        return (dw1 & 0x20000) ? 4 : 0;

    case 0x400:
    case 0x600:     // Layer 2 or Layer 3
        return (dw1 & 0x20000) ? 1 : 0;

    default:        // Error !!
        return 0;
    }
}




/******************************Public*Routine******************************\
* CheckInputType
*
*
* check if you can support mtIn
*
*
\**************************************************************************/
HRESULT
CMpegAudioCodec::CheckInputType(
    const CMediaType* pmtIn
    )
{
    DbgLog((LOG_TRACE, 2, TEXT("CMpegAudioCodec::CheckInputType")));

    //  Check for native streams
    if (*pmtIn->Type() == MEDIATYPE_Stream &&
        *pmtIn->Subtype() == MEDIASUBTYPE_MPEG1Audio) {

        /*  This will be checked a bit more in Connect() */
        return S_OK;
    }

    // check this is an MPEG audio format type
    if (*pmtIn->FormatType() != FORMAT_WaveFormatEx) {
        DbgLog((LOG_ERROR, 2, TEXT("\tFormat not FORMAT_WaveFormatEx")));
        return E_INVALIDARG;
    }

    // we only support MEDIATYPE_Audio
    if (*pmtIn->Type() != MEDIATYPE_Audio) {
        DbgLog((LOG_ERROR, 2, TEXT("\tNot MEDIATYPE_Audio")));
        return E_INVALIDARG;
    }

    if (*pmtIn->Subtype() != MEDIASUBTYPE_MPEG1Packet &&
        *pmtIn->Subtype() != MEDIASUBTYPE_MPEG1Payload &&
        *pmtIn->Subtype() != MEDIASUBTYPE_MPEG1AudioPayload ) {
        DbgLog((LOG_ERROR, 2, TEXT("\tNot MEDIASUBTYPE_MPEG1Packet or Payload")));
        return E_INVALIDARG;
    }

    if (pmtIn->FormatLength() != sizeof(MPEG1WAVEFORMAT)) {
        DbgLog((LOG_ERROR, 2,
               TEXT("\tFormat block size != sizeof(MPEG1WAVEFORMAT)")));
        return E_INVALIDARG;
    }

    LPMPEG1WAVEFORMAT pwf = (LPMPEG1WAVEFORMAT)pmtIn->Format();
    if (pwf->wfx.wFormatTag != WAVE_FORMAT_MPEG) {
        DbgLog((LOG_ERROR, 2,
               TEXT("\tFormat tag not WAVE_FORMAT_MPEG")));
        return E_INVALIDARG;
    }

    //
    // Make sure that we have not been given layer III data, we don't know
    // how to decode layer III.
    //
    if (((LPMPEG1WAVEFORMAT)pmtIn->pbFormat)->fwHeadLayer == ACM_MPEG_LAYER3) {
        DbgLog((LOG_ERROR, 2, TEXT("\tCan't decode layer III data")));
        return E_INVALIDARG;
    }

    CopyMemory(&m_Format, pmtIn->pbFormat, sizeof(MPEG1WAVEFORMAT));
    return S_OK;
}



/******************************Public*Routine******************************\
* CheckTransform
*
* check if you can support the transform from this input to this output
*
*
\**************************************************************************/
HRESULT
CMpegAudioCodec::CheckTransform(
    const CMediaType* pmtIn,
    const CMediaType* pmtOut
    )
{
    /*  We only accept the input types we propose based on our
        input connection
    */
    DbgLog((LOG_TRACE, 2, TEXT("CMpegAudioCodec::CheckTransform")));
    for (int i = 0; ; i++) {
        CMediaType cmt;
        HRESULT hr = GetMediaType(i, &cmt);
        if (S_OK != hr) {
            return VFW_E_TYPE_NOT_ACCEPTED;
        }
        if (cmt == *pmtOut) {
            return S_OK;
        }
    }
}


/******************************Public*Routine******************************\
* SetMediaType
*
* overriden to know when the media type is actually set
*
*
\**************************************************************************/
HRESULT
CMpegAudioCodec::SetMediaType(
    PIN_DIRECTION direction,
    const CMediaType *pmt
    )
{
    if (direction == PINDIR_INPUT) {

        //
        // Get the data type
        //
        if (*pmt->Subtype() == MEDIASUBTYPE_MPEG1Packet) {
            m_bPayloadOnly = FALSE;
        }
        else {
            ASSERT(*pmt->Subtype() == MEDIASUBTYPE_MPEG1Payload ||
                   *pmt->Subtype() == MEDIASUBTYPE_MPEG1Audio ||
                   *pmt->Subtype() == MEDIASUBTYPE_MPEG1AudioPayload);
            m_bPayloadOnly = TRUE;
        }
    }
    else {

        LPWAVEFORMATEX lpWf = (LPWAVEFORMATEX)pmt->pbFormat;
        int SamplesPerFrame;

        if (m_QuarterInt) {
            m_dwCtrl = DECODE_QUART_INT | DECODE_QUARTER;
        }
        else {
            switch (m_FreqDiv) {
            case 1:
                m_dwCtrl = DECODE_FULL;
                break;

            case 2:
                m_dwCtrl = DECODE_HALF;
                break;

            case 4:
                m_dwCtrl = DECODE_QUARTER;
                break;
            }
        }

        switch (lpWf->wBitsPerSample) {
        case 16:
            m_dwCtrl |= DECODE_16BIT;
            break;

        case 8:
            m_dwCtrl |= DECODE_8BIT;
            break;
        }

        switch (m_Quality) {
        case DECODE_HALF_FULLQ:
            m_dwCtrl |= DECODE_HALF_FULLQ;
            break;

        case DECODE_HALF_HIQ:
            m_dwCtrl |= DECODE_HALF_HIQ;
            break;
        }

        switch (lpWf->nChannels) {
        case 2:
            m_dwCtrl |= DECODE_STEREO;
            break;

        case 1:
            m_dwCtrl |= DECODE_MONO;
            break;
        }

        if (m_Format.fwHeadLayer == ACM_MPEG_LAYER1) {
            SamplesPerFrame = 384;
        }
        else {
            SamplesPerFrame = 1152;
        }

        m_TimePerFrame = MulDiv(SamplesPerFrame, 10000000,
                                m_Format.wfx.nSamplesPerSec);

        m_FrameSizeOutput = MulDiv(MulDiv(SamplesPerFrame,
                                          lpWf->wBitsPerSample, 8),
                                   lpWf->nChannels, m_FreqDiv);
    }
    return S_OK;
}




/******************************Public*Routine******************************\
* GetMediaType
*
* Return our preferred output media types (in order)
*
*
\**************************************************************************/
HRESULT
CMpegAudioCodec::GetMediaType(
    int iPosition,
    CMediaType *pmt
    )
{
    LPWAVEFORMATEX      lpWf;
    LPWAVEFORMATEX      lpWfIn;
    LPMPEG1WAVEFORMAT   lpWfInMpeg;
    CMediaType          cmt;
    int                 SamplesPerFrame;
    WORD                nChannels;
    WORD                nBitsPerSample;

    if (iPosition < 0) {
        return E_INVALIDARG;
    }

    //
    // If the requested output word size is 8 bits do not reveal the 16 bits
    // per sample option.
    //
    if (m_WordSize == 8) {
        if (iPosition > 0) {
            return VFW_S_NO_MORE_ITEMS;
        }
        iPosition = 1;
    }

    switch (iPosition) {
    case 0:
        nBitsPerSample = 16;
        break;

    case 1:
        nBitsPerSample = 8;
        break;

    default:
        return VFW_S_NO_MORE_ITEMS;
    }

    lpWf = (LPWAVEFORMATEX)pmt->AllocFormatBuffer(sizeof(WAVEFORMATEX));
    if (lpWf == NULL) {
        return E_OUTOFMEMORY;
    }
    if (*m_pInput->CurrentMediaType().Type() == MEDIATYPE_Stream) {
        lpWfIn = &m_Format.wfx;
        lpWfInMpeg = &m_Format;
    } else {
        lpWfIn = (LPWAVEFORMATEX)m_pInput->CurrentMediaType().pbFormat;
        lpWfInMpeg = (LPMPEG1WAVEFORMAT)m_pInput->CurrentMediaType().pbFormat;
    }
    ASSERT(lpWfIn != NULL);


    //
    // Dump the input format
    //
    DbgLog((LOG_TRACE, 2, TEXT("nSamplesPerSec = %ld"), lpWfIn->nSamplesPerSec));
    DbgLog((LOG_TRACE, 2, TEXT("nChannels      = %hd"), lpWfIn->nChannels));
    DbgLog((LOG_TRACE, 2, TEXT("Layer          = %hd"), lpWfInMpeg->fwHeadLayer));

    //
    // If the number of channels available matches the number preferred
    // we decode all the channels available.  Otherwise we decode the
    // minimum of the chanels available and the channels preferred.
    //
    if (lpWfIn->nChannels == m_PrefChan) {
        nChannels = m_PrefChan;
    }
    else {
        nChannels = min(m_PrefChan, lpWfIn->nChannels);
    }

    lpWf->wFormatTag       = WAVE_FORMAT_PCM;
    lpWf->nChannels        = nChannels;
    lpWf->nSamplesPerSec   = lpWfIn->nSamplesPerSec / m_FreqDiv;
    lpWf->nBlockAlign      = (nBitsPerSample * nChannels) / 8;
    lpWf->nAvgBytesPerSec  = lpWf->nSamplesPerSec * lpWf->nBlockAlign;
    lpWf->wBitsPerSample   = nBitsPerSample;
    lpWf->cbSize           = 0;

    //
    // Dump the ouput format
    //
    DbgLog((LOG_TRACE, 2, TEXT("!!nSamplesPerSec = %ld"), lpWf->nSamplesPerSec));
    DbgLog((LOG_TRACE, 2, TEXT("!!nChannels      = %hd"), lpWf->nChannels));
    DbgLog((LOG_TRACE, 2, TEXT("!!nBlockAlign    = %hd"), lpWf->nBlockAlign));
    DbgLog((LOG_TRACE, 2, TEXT("!!wBitsPerSample = %hd"), lpWf->wBitsPerSample));
    DbgLog((LOG_TRACE, 2, TEXT("!!nAvgBytesPerSec= %ld\n"), lpWf->nAvgBytesPerSec));

    //
    // we assume the output format is uncompressed
    //
    pmt->SetType(&MEDIATYPE_Audio);
    pmt->SetSubtype(&MEDIASUBTYPE_PCM);
    pmt->SetFormatType(&FORMAT_WaveFormatEx);
    pmt->SetTemporalCompression(FALSE);

    //
    // The time per frame and output sample size depend on the layer
    // of mpeg audio data being compressed.
    //
    if (lpWfInMpeg->fwHeadLayer == ACM_MPEG_LAYER1) {
        SamplesPerFrame = 384;
    }
    else {
        SamplesPerFrame = 1152;
    }

    m_TimePerFrame = MulDiv(SamplesPerFrame, 10000000, lpWfIn->nSamplesPerSec);
    m_FrameSizeOutput = MulDiv(MulDiv(SamplesPerFrame, nBitsPerSample, 8),
                               nChannels, m_FreqDiv);

    pmt->SetSampleSize(m_FrameSizeOutput * MAX_FRAMES_PER_OUTPUT_SAMPLE);
    return S_OK;
}


/******************************Public*Routine******************************\
* DecideBufferSize
*
*
* Called from CBaseOutputPin to prepare the allocator's count
* of buffers and sizes
*
*
\**************************************************************************/
HRESULT
CMpegAudioCodec::DecideBufferSize(
    IMemAllocator *pAllocator,
    ALLOCATOR_PROPERTIES * pProperties
    )
{
    DbgLog((LOG_TRACE, 2, TEXT("CMpegAudioCodec::DecideBufferSize")));

    ASSERT(pAllocator);
    ASSERT(pProperties);
    HRESULT hr = NOERROR;

    pProperties->cBuffers = 8;
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
\**************************************************************************/
HRESULT
CMpegAudioCodec::StartStreaming()
{
    CAutoLock       lock(&m_csFilter);

    DbgLog((LOG_TRACE, 2, TEXT("CMpegAudioCodec::StartStreaming")));

    m_pAudioDecoder = new CAudioDecoder(this);
    if (m_pAudioDecoder == NULL) {
        return E_OUTOFMEMORY;
    }

    ResetAudioDecoder();
    return S_OK;
}


/******************************Public*Routine******************************\
* StopStreaming
*
*
\**************************************************************************/
HRESULT
CMpegAudioCodec::StopStreaming()
{
    CAutoLock lock(&m_csFilter);
    CAutoLock lck(&m_csReceive);
    DbgLog((LOG_TRACE, 2, TEXT("CMpegAudioCodec::StopStreaming")));

    ASSERT(m_pAudioDecoder != NULL);

    delete m_pAudioDecoder;
    m_pAudioDecoder = NULL;

    return S_OK;
}


/******************************Public*Routine******************************\
* GetDecoderInteger
*
*
\**************************************************************************/
int
GetDecoderInteger(
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

/******************************Public*Routine******************************\
* SkipToPacketData
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

