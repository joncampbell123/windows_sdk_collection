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
#include <mpeg1.h>
#include <initguid.h>
//  f28300a0-f0cc-11cf-93d5-0080c795857f
DEFINE_GUID(CLSID_MPEG1Sample,
    0xf28300a0, 0xf0cc, 0x11cf, 0x93, 0xd5, 0x00, 0x80, 0xc7, 0x95, 0x85, 0x7f);


/*  Stuff to make this into a filter */
/* List of class IDs and creator functions for the class factory. This
   provides the link between the OLE entry point in the DLL and an object
   being created. The class factory will call the static CreateInstance
   function when it is asked to create a CLSID_MPEG1Splitter object */

CFactoryTemplate g_Templates[1] = {
    {L"MPEG1 Sample", &CLSID_MPEG1Sample, CMPEG1SplitterFilter::CreateInstance}
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);


/* This goes in the factory template table to create new instances */

CUnknown *CMPEG1SplitterFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *phr)
{
    CUnknown *pUnkRet = new CMPEG1SplitterFilter(pUnk, phr);
    return pUnkRet;
}

/*  Registration setup stuff */
//  Setup data

AMOVIESETUP_MEDIATYPE sudMpgInputType[] =
{
    { &MEDIATYPE_Stream, &MEDIASUBTYPE_MPEG1System }
};
AMOVIESETUP_MEDIATYPE sudMpgAudioOutputType[] =
{
    { &MEDIATYPE_Audio, &MEDIASUBTYPE_MPEG1AudioPayload }
};
AMOVIESETUP_MEDIATYPE sudMpgVideoOutputType[] =
{
    { &MEDIATYPE_Video, &MEDIASUBTYPE_MPEG1Payload }
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
    &CLSID_MPEG1Sample,
    L"MPEG1 Splitter Sample",
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
CMPEG1SplitterFilter::CMPEG1SplitterFilter(
   LPUNKNOWN pUnk,
   HRESULT *phr) :
   CBaseSplitterFilter(
       TEXT("CMPEG1SplitterFilter"),
       pUnk,
       CLSID_MPEG1Sample,
       phr)
{
    //  Create our input pin
    m_pInput = new CSplitterInputPin(this, phr);
}
//  Override type checking
HRESULT CMPEG1SplitterFilter::CheckInputType(const CMediaType *pmt)
{
    /*  We'll accept our preferred type or a wild card for the subtype */

    if (pmt->majortype != MEDIATYPE_Stream ||
        pmt->subtype != MEDIASUBTYPE_MPEG1System &&
        pmt->subtype != MEDIASUBTYPE_NULL) {
        return S_FALSE;
    } else {
        return S_OK;
    }
}

LPAMOVIESETUP_FILTER CMPEG1SplitterFilter::GetSetupData()
{
    return &sudMpgsplit;
}

/*  Complete connection and instantiate parser
    This involves:

    Instatiate the parser with for the type and have it check the format
*/

CBaseParser *CMPEG1SplitterFilter::CreateParser(
    CParserNotify *pNotify,
    CMediaType *pType
)
{
    HRESULT hr = S_OK;
    return new CMPEG1Parser(pNotify, &hr);
}

/*  Cheap'n nasty parser - DON'T do yours like this! */
/*  Initialize a parser

    pmt     - type of stream if known - can be NULL
    pRdr    - way to read the source medium - can be NULL
*/
HRESULT CMPEG1Parser::Init(CParseReader *pRdr)
{
    const DWORD dwLen = 65536;
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
    PBYTE pbCurrent = pbData;
    while (dwLeft >= 140) {
        DWORD dwCode = *(UNALIGNED DWORD *)pbCurrent;

        /*  Check if it's a valid start code */
        if ((dwCode & 0x00FFFFFF) == 0x00010000) {
            dwCode = DWORD_SWAP(dwCode);
            if (VALID_PACKET(dwCode)){
                MPEG_PACKET_DATA Info;
                DWORD dwPacketLen = ParseMPEG1Packet(
                                        pbCurrent,
                                        dwLeft,
                                        &Info);
                if (dwPacketLen > 4) {
                    if (!m_bGotFirstPts && Info.bHasPts) {
                        m_llFirstPts = Info.llPts;
                        m_bGotFirstPts = TRUE;
                    }
                    if (IsVideoStreamId((BYTE)(dwCode & 0xFF))) {
                        m_Video.m_uStreamId = (BYTE)(dwCode & 0xFF);
                        const BYTE * pbHeader = pbCurrent + Info.dwHeaderLen;
                        SEQHDR_INFO seqInfo;

                        /*  Create our format block */
                        if (DWORD_SWAP(SEQUENCE_HEADER_CODE) ==
                            *(UNALIGNED DWORD *)pbHeader &&
                            ParseSequenceHeader(pbHeader, dwLeft, &seqInfo)) {
                            /*  Create the media type from the stream
                                only support Payload streams for now
                            */
                            CMediaType cmt;
                            GetVideoMediaType(&cmt, TRUE, &seqInfo);

                            /*  Create our video stream */
                            m_pNotify->CreateStream(L"Video", &m_Video.m_pNotify);
                            m_Video.m_pNotify->AddMediaType(&cmt);
                        } else {
                        }
                    } else if (IsAudioStreamId((BYTE)(dwCode & 0xFF))) {
                        m_Audio.m_uStreamId = (BYTE)(dwCode & 0xFF);

                        /*  Now let's hope the first 2 bytes are a frame
                            start !
                        */
                        MPEG1WAVEFORMAT wf;
                        if (ParseAudioHeader(pbCurrent + Info.dwHeaderLen,
                                             &wf)) {
                            CMediaType cmt(&MEDIATYPE_Audio);
                            cmt.subtype = MEDIASUBTYPE_MPEG1AudioPayload;
                            cmt.SetFormat((PBYTE)&wf, sizeof(wf));
                            cmt.SetFormatType(&FORMAT_WaveFormatEx);
                            m_Audio.m_uStreamId = (BYTE)(dwCode & 0xFF);
                            m_pNotify->CreateStream(
                                L"Audio",
                                &m_Audio.m_pNotify);
                            m_Audio.m_pNotify->AddMediaType(&cmt);
                        }
                    }
                }
                ASSERT(dwLeft >= dwPacketLen);
                pbCurrent += dwPacketLen;
                dwLeft -= dwPacketLen;
                continue;
            }
        }

        /*  Hideously inefficient! */
        pbCurrent++;
        dwLeft--;
    }
    delete [] pbData;
    if (!m_bGotFirstPts || (!m_Audio.Initialized() && !m_Video.Initialized())) {
        return VFW_E_TYPE_NOT_ACCEPTED;
    } else {
        return S_OK;
    }
}


/*  Get the size and count of buffers preferred based on the
    actual content
*/
void CMPEG1Parser::GetSizeAndCount(LONG *plSize, LONG *plCount)
{
    *plSize = 32768;
    *plCount = 4;
}

/*  Call this to reinitialize for a new stream */
void CMPEG1Parser::StreamReset()
{
}

/*  Call this to pass new stream data :

    pbData        - pointer to data
    lData         - length of data
    plProcessed   - Amount of data consumed
*/
HRESULT CMPEG1Parser::Process(
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
            DWORD dwPacketLen = ParseMPEG1Packet(pbCurrent, dwLeft, &Info);
            if (dwPacketLen == 0) {
                break;
            }
            if (dwPacketLen == 4) {
                dwLeft -= 4;
                pbCurrent += 4;
                continue;
            }
            /*  If it's a packet we're interested in send it on */
            CStream *pStream = NULL;
            if (((BYTE)(dwCode & 0xFF)) == m_Audio.m_uStreamId) {
                pStream = &m_Audio;
            } else
            if (((BYTE)(dwCode & 0xFF)) == m_Video.m_uStreamId) {
                pStream = &m_Video;
            }
            if (pStream != NULL) {
                DbgLog((LOG_TRACE, 3, TEXT("Send sample for stream %2.2X"),
                        pStream->m_uStreamId));
                pStream->m_pNotify->SendSample(
                    pbCurrent + Info.dwHeaderLen,
                    Info.dwPacketLen - Info.dwHeaderLen,
                    Info.bHasPts ? TimeStamp(Info.llPts) : 0,
                    Info.bHasPts);
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
