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
/*  Structures and definitions for MPEG1 filter stuff */

/*  Implement type checking and CompleteConnect */
class CMPEG1SplitterFilter : public CBaseSplitterFilter
{
public:
    /* This goes in the factory template table to create new instances */
    static CUnknown *CreateInstance(LPUNKNOWN, HRESULT *);

    //  Constructor and destructor
    CMPEG1SplitterFilter(
       LPUNKNOWN pUnk,
       HRESULT *phr);

    //  Support self-registration
    LPAMOVIESETUP_FILTER GetSetupData();

    //  Override type checking
    HRESULT CheckInputType(const CMediaType *pmt);

    //  Create specific parser
    CBaseParser *CreateParser(CParserNotify *pNotify, CMediaType *pType);
};

/*  Parser */
class CMPEG1Parser : public CBaseParser
{
public:
    CMPEG1Parser(CParserNotify *pNotify, HRESULT *phr) :
        CBaseParser(pNotify, phr),
        m_bGotFirstPts(FALSE),
        m_llFirstPts(0)
    {
    }

    /*  Initialize a parser

        pmt     - type of stream if known - can be NULL
        pRdr    - way to read the source medium - can be NULL
    */
    HRESULT Init(CParseReader *pRdr);


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
        CStream() : m_uStreamId(0),
                    m_pNotify(NULL) {}
        BOOL Initialized()
        {
            return m_pNotify != NULL && m_uStreamId != 0xFF;
        }
        BYTE           m_uStreamId;
        CStreamNotify *m_pNotify;
    };
    CStream  m_Audio;
    CStream  m_Video;
    LONGLONG m_llFirstPts;
    BOOL     m_bGotFirstPts;
};
