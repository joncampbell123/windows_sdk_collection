//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1996 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;

#include <streams.h>
#include <pullpin.h>
#include <mmreg.h>
#include <mpeg2typ.h>
#include <alloc.h>
#include <splitter.h>
#include <mpegdef.h>
#include <mpgutil.h>
#include <initguid.h>
#include <mpeg2ids.h>
#include <mpeg2.h>
//  59b41160-f483-11cf-a66b-00aa00bf96aa
DEFINE_GUID(CLSID_MPEG2Sample,
    0x59b41160, 0xf483, 0x11cf, 0xa6, 0x6b, 0x00, 0xaa, 0x00, 0xbf, 0x96, 0xaa);


/*  Stuff to make this into a filter */
/* List of class IDs and creator functions for the class factory. This
   provides the link between the OLE entry point in the DLL and an object
   being created. The class factory will call the static CreateInstance
   function when it is asked to create a CLSID_MPEG2Splitter object */

CFactoryTemplate g_Templates[1] = {
    {L"MPEG2 Sample", &CLSID_MPEG2Sample, CMPEG2SplitterFilter::CreateInstance}
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);


/* This goes in the factory template table to create new instances */

CUnknown *CMPEG2SplitterFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *phr)
{
    CUnknown *pUnkRet = new CMPEG2SplitterFilter(pUnk, phr);
    return pUnkRet;
}

/*  Registration setup stuff */
//  Setup data

AMOVIESETUP_MEDIATYPE sudMpgInputType[] =
{
    { &MEDIATYPE_Stream, &MEDIASUBTYPE_MPEG2_PROGRAM }
};
AMOVIESETUP_MEDIATYPE sudMpgAudioOutputType[] =
{
    { &MEDIATYPE_Audio, &MEDIASUBTYPE_MPEG1AudioPayload }
};
AMOVIESETUP_MEDIATYPE sudMpgVideoOutputType[] =
{
    { &MEDIATYPE_Video, &MEDIASUBTYPE_MPEG2_VIDEO }
};

AMOVIESETUP_PIN sudMpgPins[3] =
{
    { L"Input",
      FALSE,                               // bRendered
      FALSE,                               // bOutput
      FALSE,                               // bZero
      FALSE,                               // bMany
      &CLSID_NULL,                         // clsConnectsToFilter
      NULL,                                // ConnectsToPin
      NUMELMS(sudMpgInputType),            // Number of media types
      sudMpgInputType
    },
    { L"Audio Output",
      FALSE,                               // bRendered
      TRUE,                                // bOutput
      TRUE,                                // bZero
      FALSE,                               // bMany
      &CLSID_NULL,                         // clsConnectsToFilter
      NULL,                                // ConnectsToPin
      NUMELMS(sudMpgAudioOutputType),      // Number of media types
      sudMpgAudioOutputType
    },
    { L"Video Output",
      FALSE,                               // bRendered
      TRUE,                                // bOutput
      TRUE,                                // bZero
      FALSE,                               // bMany
      &CLSID_NULL,                         // clsConnectsToFilter
      NULL,                                // ConnectsToPin
      NUMELMS(sudMpgVideoOutputType),      // Number of media types
      sudMpgVideoOutputType
    }
};

AMOVIESETUP_FILTER sudMpgsplit =
{
    &CLSID_MPEG2Sample,
    L"MPEG2 Splitter Sample",
    0,                                     // Don't use us for real!
    NUMELMS(sudMpgPins),                   // 3 pins
    sudMpgPins
};


//
// DllRegisterSever
//
// Handle the registration of this filter
//
STDAPI DllRegisterServer()
{
    return AMovieDllRegisterServer();

} // DllRegisterServer

//  Constructor and destructor
CMPEG2SplitterFilter::CMPEG2SplitterFilter(
   LPUNKNOWN pUnk,
   HRESULT *phr) :
   CBaseSplitterFilter(
       TEXT("CMPEG2SplitterFilter"),
       pUnk,
       CLSID_MPEG2Sample,
       phr)
{
    //  Create our input pin
    m_pInput = new CSplitterInputPin(this, phr);
}

//  Override type checking and connection completion
HRESULT CMPEG2SplitterFilter::CheckInputType(const CMediaType *pmt)
{
    if (pmt->majortype != MEDIATYPE_Stream ||
        pmt->subtype != MEDIASUBTYPE_MPEG2_PROGRAM &&
        pmt->subtype != MEDIASUBTYPE_NULL) {
        return S_FALSE;
    } else {
        return S_OK;
    }
}


LPAMOVIESETUP_FILTER CMPEG2SplitterFilter::GetSetupData()
{
    return &sudMpgsplit;
}

/*  Complete connection and instantiate parser
    This involves:

    Instatiate the parser with for the type and have it check the format
*/

CBaseParser *CMPEG2SplitterFilter::CreateParser(
    CParserNotify *pNotify,
    CMediaType *pType
)
{
    HRESULT hr = S_OK;
    return new CMPEG2Parser(pNotify, &hr);
}

/*  Cheap'n nasty parser - DON'T do yours like this! */
/*  Initialize a parser

    pmt     - type of stream if known - can be NULL
    pRdr    - way to read the source medium - can be NULL
*/
HRESULT CMPEG2Parser::Init(CParseReader *pRdr)
{
    const DWORD dwLen = 65536 * 4;
    /*  Just read 32K and look for interesting stuff */
    PBYTE pbData = new BYTE[dwLen];
    if (pbData == NULL) {
        return E_OUTOFMEMORY;
    }
    HRESULT hr = pRdr->Read(pbData, dwLen);
    if (S_OK != hr) {
        delete [] pbData;
        return hr;
    }

    /*  Now just loop looking for start codes */
    DWORD dwLeft = dwLen;
    const BYTE * pbCurrent = pbData;
    while (dwLeft >= 140) {
        DWORD dwCode = *(UNALIGNED DWORD *)pbCurrent;

        /*  Check if it's a valid start code */
        if ((dwCode & 0x00FFFFFF) == 0x00010000) {
            dwCode = DWORD_SWAP(dwCode);
            /*  Look for a system header */
            if (dwCode == PACK_START_CODE) {
                DWORD dwLen = MPEG2PackHeaderLength(pbCurrent, dwLeft);
                if (dwLen == 0) {

                    /*  Need more data */
                    break;
                } else {

                    /*  Skip this item */
                    dwLeft -= dwLen;
                    pbCurrent += dwLen;
                    continue;
                }
            }
            if (dwCode == SYSTEM_HEADER_START_CODE) {
                /*  Parse the MPEG-2 system header */
                DWORD dwLen = ParseSystemHeader(pbCurrent, dwLeft);
                if (dwLen > 4) {
                    /*  Pull out the stream info */
                    BYTE bStreams = pbCurrent[SYSTEM_HEADER_BASIC_LENGTH];
                    DWORD cbStreams = dwLen - SYSTEM_HEADER_BASIC_LENGTH;
                    ASSERT(cbStreams % 3 == 0);
                    const BYTE *pbStreams = pbCurrent + SYSTEM_HEADER_BASIC_LENGTH;
                    for ( ;cbStreams > 0 ; pbStreams += 3, cbStreams -= 3) {
                        if (pbStreams[0] == AUDIO_GLOBAL) {
                            m_bHasAudio = TRUE;
                        } else
                        if (pbStreams[0] == VIDEO_GLOBAL) {
                            m_bHasVideo = TRUE;
                        } else {
                            if (IsAudioStreamId(pbStreams[0])) {
                                m_bHasAudio = TRUE;
                                if (m_uAudioStreamId == 0) {
                                    m_uAudioStreamId = pbStreams[0];
                                }
                            }
                            if (IsVideoStreamId(pbStreams[0])) {
                                m_bHasVideo = TRUE;
                                if (m_uVideoStreamId == 0) {
                                    m_uVideoStreamId = pbStreams[0];
                                }
                            }
                            StreamInfo(pbStreams[0])->m_bExists = TRUE;
                        }

                    }
                    pbCurrent += dwLen;
                    dwLeft -= dwLen;
                    continue;
                } else {

                    /*  Need more data */
                    if (dwLen == 0) {
                        break;
                    }
                }
            } else
            if (VALID_PACKET(dwCode)){
                MPEG_PACKET_DATA Info;
                DWORD dwPacketLen = ParseMPEG2Packet(
                                        pbCurrent,
                                        dwLeft,
                                        &Info);
                if (dwPacketLen > 4) {
                    if (!m_bGotFirstPts && Info.bHasPts) {
                        m_llFirstPts = Info.llPts;
                        m_bGotFirstPts = TRUE;
                    }
                    InitStream((UCHAR)(dwCode & 0xFF),
                               pbCurrent + Info.dwHeaderLen,
                               Info.dwPacketLen - Info.dwHeaderLen
                              );

                    if (IsVideoStreamId((BYTE)(dwCode & 0xFF))) {
                        if (m_uVideoStreamId == 0) {
                            m_uVideoStreamId = (BYTE)(dwCode & 0xFF);
                        }
                    } else if (IsAudioStreamId((BYTE)(dwCode & 0xFF))) {
                        if (m_uAudioStreamId == 0) {
                            m_uAudioStreamId = (BYTE)(dwCode & 0xFF);
                        }
                    }
                    ASSERT(dwLeft >= dwPacketLen);
                    pbCurrent += dwPacketLen;
                    dwLeft -= dwPacketLen;
                    continue;
                } else {
                    if (dwPacketLen == 0) {
                        break;
                    }
                }
            }
        }

        /*  Hideously inefficient but then for program stream this shouldn't
            happen very often! */
        pbCurrent++;
        dwLeft--;
    }
    delete [] pbData;
    if (!m_bGotFirstPts || (m_uAudioStreamId == 0 && m_uVideoStreamId == 0)) {
        return VFW_E_TYPE_NOT_ACCEPTED;
    } else {
        return S_OK;
    }
}


/*  Get the size and count of buffers preferred based on the
    actual content
*/
void CMPEG2Parser::GetSizeAndCount(LONG *plSize, LONG *plCount)
{
    /*  HACK HACK - fix this depending on the stream */
    *plSize = 65536;
    *plCount = 8;
}

/*  Call this to reinitialize for a new stream */
void CMPEG2Parser::StreamReset()
{
    /*  Find the type of each stream */
    for (int i = PROGRAM_STREAM_MAP; i <= PROGRAM_STREAM_DIRECTORY; i++) {
        CStream *pStream = m_Streams[i - PROGRAM_STREAM_MAP].m_pStream;
        for (CStream *pSearch = pStream;
             pSearch != NULL;
             pSearch = pSearch->m_pNext) {
            AM_MEDIA_TYPE mt;
            pSearch->m_pNotify->CurrentMediaType(&mt);
            pSearch->m_bDoPES = mt.majortype == MEDIATYPE_MPEG2_PES;
            FreeMediaType(mt);
        }
    }
}

/*  Call this to pass new stream data :

    pbData        - pointer to data
    lData         - length of data
    plProcessed   - Amount of data consumed
*/
HRESULT CMPEG2Parser::Process(
    const BYTE * pbData,
    LONG lData,
    LONG *plProcessed
)
{
    /*  Just loop processing packets until we run out of data
        We should do a lot more to sync up than just eat a start
        code !
    */

    DWORD dwLeft = lData;
    const BYTE * pbCurrent = pbData;

    while (dwLeft > 4) {
        /*  Find a start code */
        DWORD dwCode = DWORD_SWAP(*(UNALIGNED DWORD *)pbCurrent);
        MPEG_PACKET_DATA Info;
        if (VALID_PACKET(dwCode)) {
            DWORD dwPacketLen = ParseMPEG2Packet(pbCurrent, dwLeft, &Info);
            if (dwPacketLen == 0) {
                break;
            }
            if (dwPacketLen == 4) {
                dwLeft -= 4;
                pbCurrent += 4;
                continue;
            }
            /*  If it's a packet we're interested in send it on */
            BYTE uStreamId = (BYTE)(dwCode & 0xFF);
            CStream *pStream = StreamInfo(uStreamId)->FindStream(
                                   pbCurrent[Info.dwHeaderLen]);
            if (pStream != NULL) {
                DbgLog((LOG_TRACE, 3, TEXT("Send sample for stream %2.2X"),
                        pStream->m_uStreamId));
                if (pStream->m_bDoPES) {
                    pStream->m_pNotify->SendSample(
                        pbCurrent,
                        Info.dwPacketLen,
                        Info.bHasPts ? TimeStamp(Info.llPts) : 0,
                        Info.bHasPts);
                } else {
                    pStream->m_pNotify->SendSample(
                        pbCurrent + Info.dwHeaderLen + pStream->m_dwSkip,
                        Info.dwPacketLen - Info.dwHeaderLen - pStream->m_dwSkip,
                        Info.bHasPts ? TimeStamp(Info.llPts) : 0,
                        Info.bHasPts);
                }
            }
            dwLeft -= Info.dwPacketLen;
            pbCurrent += Info.dwPacketLen;
        } else {
            dwLeft--;
            pbCurrent++;
        }
    }
    *plProcessed = lData - dwLeft;
    return S_OK;
}

/*  Initialize a stream */
HRESULT CMPEG2Parser::InitStream(
    UCHAR uStreamId,
    const BYTE * pbFirstPacket,
    DWORD dwLen
)
{
    /*  Do nothing if we've already seen this one */
    if (NULL != StreamInfo(uStreamId)->FindStream(pbFirstPacket[0])) {
        return S_OK;
    }

    /*  This 'base' implementation just finds the audio and video
        streams
    */
    if (IsAudioStreamId(uStreamId)) {
        return InitAudioStream(uStreamId, pbFirstPacket, dwLen);
    }
    if (IsVideoStreamId(uStreamId)) {
        return InitVideoStream(uStreamId, pbFirstPacket, dwLen);
    }
    if (uStreamId == PRIVATE_STREAM_1) {
        return InitPrivateStream1(uStreamId, pbFirstPacket, dwLen);
    }
    return S_FALSE;
}



/*  Initialize an audio stream */
HRESULT CMPEG2Parser::InitAudioStream(
    UCHAR uStreamId,
    const BYTE * pbFirstPacket,
    DWORD dwLen
)
{
    /*  Just assume it's MPEG1 for now (!) */
    MPEG1WAVEFORMAT wf;
    if (ParseAudioHeader(pbFirstPacket, &wf)) {
        CMediaType cmt(&MEDIATYPE_Audio);
        cmt.subtype = MEDIASUBTYPE_MPEG1AudioPayload;
        cmt.SetFormat((PBYTE)&wf, sizeof(wf));
        cmt.SetFormatType(&FORMAT_WaveFormatEx);
        return CreateDefaultStream(
            uStreamId,
            L"Audio",
            &cmt,
            FALSE);
    } else {
        return VFW_E_UNKNOWN_FILE_TYPE;
    }
}

/*  Initialize a video stream */
HRESULT CMPEG2Parser::InitVideoStream(
    UCHAR        uStreamId,
    const BYTE * pbFirstPacket,
    DWORD        dwLen
)
{
    /*  Parse the sequence header and extension */

    /*  Find the next start code */

    const BYTE * pbData = pbFirstPacket;
    const BYTE * pbStartSequenceHeader;
    if (!NextStartCode(&pbData, &dwLen)) {
        return VFW_E_UNKNOWN_FILE_TYPE;
    }
    if (!IsStartCode(pbData, SEQUENCE_HEADER_CODE)) {
        return VFW_E_UNKNOWN_FILE_TYPE;
    }
    if (dwLen < 200) {
        return VFW_E_UNKNOWN_FILE_TYPE;
    }

    /*  Now handle the sequence header the extension follows right
        on from the end and must be there, otherwise we're MPEG1
    */
    pbStartSequenceHeader = pbData;
    SEQHDR_INFO Info;
    if (!ParseSequenceHeader(pbData, dwLen, &Info)) {
        return VFW_E_UNKNOWN_FILE_TYPE;
    }

    /*  Parse the extension (soft of) */

    pbData += Info.lActualHeaderLen;
    dwLen -= Info.lActualHeaderLen;
    if (!NextStartCode(&pbData, &dwLen)) {
        return VFW_E_UNKNOWN_FILE_TYPE;
    }
    if (!IsStartCode(pbData, EXTENSION_START_CODE)) {
        /*  !!  Do MPEG 1 */
        return VFW_E_UNKNOWN_FILE_TYPE;
    }

    if (dwLen < 14) {
        return VFW_E_UNKNOWN_FILE_TYPE;
    }

    /*  Get the extension info - note that the various extension start
        codes can be identified by the 4-bit ID following them
        So we can just keep eating extension start codes until we get
        bored an looking for the id - it's up to the authors to
        deliver them according to the spec order
    */
    /*  Check the sequence extension id */
    if ((pbData[4] >> 4) != 0x01) {
        DbgLog((LOG_ERROR, 1, TEXT("Bad extension start code id")));
        return VFW_E_UNKNOWN_FILE_TYPE;
    }

    /*  The escape bit seems not to be used */

    Info.dwProfile = pbData[4] & 0x07;
    Info.dwLevel   = pbData[5] >> 4;
    DbgLog((LOG_TRACE, 3, TEXT("%s : %s"),
           Info.dwProfile == 5 ? TEXT("Simple Profile") :
           Info.dwProfile == 4 ? TEXT("Main Profile")   :
           Info.dwProfile == 3 ? TEXT("SNR Scalable Profile") :
           Info.dwProfile == 2 ? TEXT("Spatially Scalable Profile") :
           Info.dwProfile == 1 ? TEXT("High Profile") :
                                 TEXT("Unrecognized Profile"),
           Info.dwLevel  == 10 ? TEXT("Low Level") :
           Info.dwLevel  == 8  ? TEXT("Main Level") :
           Info.dwLevel  == 6  ? TEXT("High 1440 Level") :
           Info.dwLevel  == 4  ? TEXT("High Level") :
                                 TEXT("Unrecognized Level")));
    Info.lWidth += ((pbData[5] & 1) << 13) + (pbData[6] & 0x80) << 5;
    Info.lHeight += (pbData[6] & 0x60) << 7;
    Info.dwBitRate += 400 * (((pbData[6] & 0x1F) << (18 + 7)) +
                             ((pbData[7] & 0xFE) << (18 - 1)));


    /*  Find out the total length and save it somewhere (!) */
    dwLen -= 4;
    pbData += 4;
    while (NextStartCode(&pbData, &dwLen)) {
        if (!IsStartCode(pbData, EXTENSION_START_CODE)) {
            /*  We're there although we may have gone past - too bad */
            DWORD dwHeaderLen = pbData - pbStartSequenceHeader;
            break;
        }
        dwLen -= 4;
        pbData += 4;
    }


    /*  Hack for now - just give them the MPEG1 stuff */
    CMediaType cmt;
    HRESULT hr = GetVideoMediaType(&cmt, TRUE, &Info);
    if (FAILED(hr)) {
        return hr;
    }
    cmt.subtype = MEDIASUBTYPE_MPEG2_VIDEO;
    cmt.formattype = FORMAT_MPEG2Video;

    /*  Create our stream */
    return CreateDefaultStream(uStreamId, L"Video", &cmt);
}

/*  Initialize a stream */
HRESULT CMPEG2Parser::InitPrivateStream1(
    UCHAR uStreamId,
    const BYTE * pbFirstPacket,
    DWORD dwLen
)
{
    DbgLog((LOG_TRACE, 0, TEXT("Substream 0x%2.2X"), pbFirstPacket[0]));
    HRESULT hr = InitAC3(uStreamId, pbFirstPacket, dwLen);
    if (FAILED(hr)) {
        hr = InitSubPicture(uStreamId, pbFirstPacket, dwLen);
    }
    return hr;
}
/*  Initialize a stream */
HRESULT CMPEG2Parser::InitSubPicture(
    UCHAR uStreamId,
    const BYTE * pbFirstPacket,
    DWORD dwLen
)
{
    /*  Don't bother with typs and stuff for now - lucky this test
        can't be mistaken for AC3!
    */

    if ((pbFirstPacket[0] & 0xE0) != 0x20) {
        return VFW_E_UNKNOWN_FILE_TYPE;
    }

    CMediaType cmt(&MEDIATYPE_Video);
    cmt.SetSubtype(&MEDIASUBTYPE_SUBPICTURE);

    return CreateDefaultStream(
               uStreamId,
               L"Subpicture",
               &cmt,
               TRUE,
               TRUE,
               pbFirstPacket[0]);
}

/*  Initialize ac3 stream */
HRESULT CMPEG2Parser::InitAC3(
    UCHAR uStreamId,
    const BYTE * pbFirstPacket,
    DWORD dwLen
)
{
    /*  Do AC3 - there are 3(4?) types :

        DVD - substream id 8 (big endian?)
        non-DVD - no substream (but should be in program map)
        --  Big endian
        --  Little endian (laser disk) - sync word 0x77 0x0B
        --  Big endian (normal) - sync word 0x0B 0x77

    */

    BOOL bDVD = FALSE;
    BOOL bBigEndian = FALSE;

    const BYTE *pbData = pbFirstPacket;
    if ((pbFirstPacket[0] & 0xF8) == 0x80) {
        bDVD = TRUE;
        pbData+= 4;
    }

    if ((pbData[0] == 0x0B && pbData[1] == 0x77)) {
        bBigEndian = TRUE;
    } else
    if ((pbData[1] == 0x0B && pbData[0] == 0x77)) {
    } else return VFW_E_UNKNOWN_FILE_TYPE;

    /*  Parse the rest of the info */
    DOLBYAC3WAVEFORMAT wf;
    if (!ParseAC3Header(pbData, &wf)) {
        return VFW_E_UNKNOWN_FILE_TYPE;
    }

    /*  Create our media type */
    CMediaType cmt(&MEDIATYPE_Audio);
    cmt.SetSubtype(&MEDIASUBTYPE_DOLBY_AC3);
    cmt.SetFormatType(&FORMAT_WaveFormatEx);
    cmt.SetFormat((PBYTE)&wf, sizeof(wf));

    /*  Create out stream */
    HRESULT hr = CreateDefaultStream(
        uStreamId,
        L"AC3",
        &cmt,
        TRUE,
        bDVD,
        bDVD ? pbFirstPacket[0] : 0);

    if (S_OK != hr) {
        return hr;
    }

    /*  Set up the stream */
    if (bDVD) {
        /*  First 4 bytes skipped for DVD */
        StreamInfo(uStreamId)->m_pStream->m_dwSkip = 4;
    }
    return hr;
}

HRESULT CMPEG2Parser::CreateDefaultStream(
    UCHAR   uStreamId,
    LPCWSTR lpszName,
    CMediaType *pmt,
    BOOL bDoPes,
    BOOL bHasSubId,
    BOOL uSubId
)
{
    CStreamInfo *pStreamInfo = StreamInfo(uStreamId);

    /*  Check this one doesn't already exist */
    if (NULL != pStreamInfo->FindStream(uSubId)) {
        return S_FALSE;
    }

    CStream *pStream = new CStream(uStreamId, bHasSubId, uSubId);
    if (pStream == NULL) {
        return E_OUTOFMEMORY;
    }
    ASSERT(bHasSubId || pStreamInfo->m_pStream == NULL);

    pStream->m_pNext = pStreamInfo->m_pStream;
    pStreamInfo->m_pStream = pStream;
    m_pNotify->CreateStream(lpszName, &pStream->m_pNotify);
    pStream->m_pNotify->AddMediaType(pmt);

    /*  Support PES format too */
    if (bDoPes) {
        pmt->SetType(&MEDIATYPE_MPEG2_PES);
        pStream->m_pNotify->AddMediaType(pmt);
    }

    return S_OK;
}



