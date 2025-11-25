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
/*  Structures and definitions for MPEG2 filter stuff */

/*  Implement type checking and CompleteConnect */
class CMPEG2SplitterFilter : public CBaseSplitterFilter
{
public:
    /* This goes in the factory template table to create new instances */
    static CUnknown *CreateInstance(LPUNKNOWN, HRESULT *);

    //  Constructor and destructor
    CMPEG2SplitterFilter(
       LPUNKNOWN pUnk,
       HRESULT *phr);

    //  Support self-registration
    LPAMOVIESETUP_FILTER GetSetupData();

    //  Override type checking
    HRESULT CheckInputType(const CMediaType *pmt);

    //  Create the parser for this filter
    CBaseParser *CreateParser(CParserNotify *pNotify, CMediaType *pType);
};

/*  MPEG-2 parsing

    We decide which streams we have by a 2-stage process:

        We find a system header.

        If this lists a program stream map we use that for finding
        stream information.

        Otherwise we use the list in the system stream header.

*/

/*  Parser */
class CMPEG2Parser : public CBaseParser
{
public:
    CMPEG2Parser(CParserNotify *pNotify, HRESULT *phr) :
        CBaseParser(pNotify, phr),
        m_bGotFirstPts(FALSE),
        m_llFirstPts(0),
        m_uCurrentAudioStream(0),
        m_uCurrentVideoStream(0),
        m_bHasAudio(FALSE),
        m_bHasVideo(FALSE),
        m_uAudioStreamId(0),
        m_uVideoStreamId(0)
    {
    }

    /*  Initialize a parser

        pmt     - type of stream if known - can be NULL
        pRdr    - way to read the source medium - can be NULL
    */
    HRESULT Init(CParseReader *pRdr);


    /*  Initialize a stream - make this virtual so we can override
        some streams in a derived class
    */
    virtual HRESULT InitStream(
        UCHAR uStreamId,
        const BYTE * pbFirstPacket,
        DWORD dwLen
    );

    /*  Initialize a stream */
    HRESULT InitAudioStream(
        UCHAR uStreamId,
        const BYTE * pbFirstPacket,
        DWORD dwLen
    );

    /*  Initialize a stream */
    HRESULT InitVideoStream(
        UCHAR uStreamId,
        const BYTE *pbFirstPacket,
        DWORD dwLen
    );

    /*  Initialize a stream */
    HRESULT InitPrivateStream1(
        UCHAR uStreamId,
        const BYTE * pbFirstPacket,
        DWORD dwLen
    );

    /*  Initialize AC3 */
    HRESULT InitAC3(
        UCHAR uStreamId,
        const BYTE * pbFirstPacket,
        DWORD dwLen
    );

    /*  Initialize subpicture */
    HRESULT InitSubPicture(
        UCHAR uStreamId,
        const BYTE * pbFirstPacket,
        DWORD dwLen
    );

    /*  Get the size and count of buffers preferred based on the
        actual content
    */
    void GetSizeAndCount(LONG *plSize, LONG *plCount);

    /*  Call this to reinitialize for a new stream */
    void StreamReset();

    /*  Call this to pass new stream data :

        pbData        - pointer to data
        lData         - length of data
        plProcessed   - Amount of data consumed
    */
    HRESULT Process(
        const BYTE * pbData,
        LONG lData,
        LONG *plProcessed
    );

private:
    /*  Helper to set timestamps */
    REFERENCE_TIME TimeStamp(LONGLONG llPts)
    {
        ASSERT(m_bGotFirstPts);
        LARGE_INTEGER liPtsOffset;
        liPtsOffset.QuadPart = llPts - m_llFirstPts;
        liPtsOffset.HighPart &= 1;
        liPtsOffset.HighPart = -liPtsOffset.HighPart;
        return llMulDiv(liPtsOffset.QuadPart,
                        UNITS,
                        MPEG_TIME_DIVISOR,
                        0);
    }
private:
    /*  Parsing structures */
    class CStream
    {
    public:
        CStream(BYTE uStreamId, BOOL bHasSubId, BYTE uSubId) :
                    m_uStreamId(uStreamId),
                    m_uSubId(uSubId),
                    m_bHasSubId(bHasSubId),
                    m_pNotify(NULL),
                    m_dwSkip(0),
                    m_pNext(NULL) {}
        virtual ~CStream() {}          // So we can delete as a CStream
        BOOL Initialized()
        {
            return m_pNotify != NULL && m_uStreamId != 0xFF;
        }
        BYTE           m_uStreamId;
        BYTE           m_uSubId;
        BOOL           m_bHasSubId;
        BOOL           m_bDoPES;
        DWORD          m_dwSkip;
        CStreamNotify *m_pNotify;
        CStream       *m_pNext;
    };

    HRESULT CreateDefaultStream(
        UCHAR   uStreamId,
        LPCWSTR lpszName,
        CMediaType *pmt,
        BOOL  bDoPES = TRUE,
        BOOL  bHasSubId = FALSE,
        BOOL  buSubId = 0
    );


    /*  Select audio and video streams - there could be multiple of
        each
    */
    BOOL     m_bHasAudio;
    UCHAR    m_uCurrentAudioStream;
    BOOL     m_bHasVideo;
    UCHAR    m_uCurrentVideoStream;

    /*  One stream per possible stream */
    class CStreamInfo {
    public:
        CStreamInfo() : m_pStream(NULL), m_bExists(FALSE) {}
        CStream  *m_pStream;
        BOOL      m_bExists;

        ~CStreamInfo()
        {
            while (m_pStream != NULL) {
                CStream *pStream = m_pStream;
                m_pStream = pStream->m_pNext;
                delete pStream;
            }
        }
        CStream *FindStream(UCHAR uSubId)
        {
            for (CStream *pSearch = m_pStream;
                 pSearch != NULL;
                 pSearch = pSearch->m_pNext) {
                if (!pSearch->m_bHasSubId || pSearch->m_uSubId == uSubId) {
                    /*  Done this one */
                    return pSearch;
                }
            }
            return NULL;
        }
    };

    /*  Get a stream corresponding to an id */
    CStreamInfo *StreamInfo(UCHAR id)
    {
        ASSERT(id >= PROGRAM_STREAM_MAP &&
               id <= PROGRAM_STREAM_DIRECTORY);
        return &m_Streams[id - PROGRAM_STREAM_MAP];
    }

    CStreamInfo m_Streams[PROGRAM_STREAM_DIRECTORY - PROGRAM_STREAM_MAP + 1];
    LONGLONG    m_llFirstPts;
    BOOL        m_bGotFirstPts;

    BYTE        m_uAudioStreamId;
    BYTE        m_uVideoStreamId;
};
