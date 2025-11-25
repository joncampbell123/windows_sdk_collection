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
/*  MPEG-2 utility functions */


/*  Packet header info */
typedef struct tag_MPEG_PACKET_DATA {
    DWORD    dwHeaderLen;
    DWORD    dwPacketLen;
    BOOL     bHasPts;
    LONGLONG llPts;
} MPEG_PACKET_DATA;

/*  Read an normal MPEG-1 style clock */
BOOL GetClock(const BYTE * pData, LONGLONG *Clock);

/*  Read an extended clock */
BOOL GetExtendedClock(const BYTE * pbData, LONGLONG *pllClock);

inline GetStartCode(const BYTE * pbData)
{
    return DWORD_SWAP(*(UNALIGNED DWORD *)pbData);
}


/*  Since dwCode is normally a constant just swap that instead */
inline BOOL IsStartCode(const BYTE * pbData, DWORD dwCode)
{
    return DWORD_SWAP(dwCode) == *(UNALIGNED DWORD *)pbData;
}

/*  Parse pack header

    Parameters :
        pbData - pointer to data containing the system header
        cbData - length of data

    Returns :
        Number of bytes processed (0 if the header doesn't fit
        in the data or 4 if the header is invalid)
        We will return 0 if we can't see the next start code to
        check whether it's a system header start code
*/

DWORD ParseMPEG2PackHeader(const BYTE * pbData, DWORD cbData);

/*  Length of pack header - returns 0 if < cbData */
inline DWORD MPEG2PackHeaderLength(const BYTE * pbData, DWORD cbData)
{
    /*  Need more data */
    if (cbData < 14) {
        return 0;
    } else {
        DWORD dwLen = 14 + (pbData[13] & 0x07);
        if (dwLen > cbData) {
            return 0;
        } else {
            return dwLen;
        }
    }
}

/*  Parse system header

    Parameters :
        pbData - pointer to data containing the system header
        cbData - length of data

    Returns :
        Number of bytes processed (0 if the header doesn't fit
        in the data or 4 if the header is invalid)
*/

LONG ParseSystemHeader(const BYTE * pbData, DWORD cbData);


/*  Parse an MPEG2 packet and extract information */
DWORD ParseMPEG2Packet(
    const BYTE * pbData,
    DWORD cbData,
    MPEG_PACKET_DATA *pPacketData
);

/*  Parse an MPEG1 packet and extract information */
DWORD ParseMPEG1Packet(
    const BYTE * pbData,
    DWORD cbData,
    MPEG_PACKET_DATA *pPacketData
);

BOOL ParseMPEG2PacketHeader(
    const BYTE * pbData,
    DWORD dwLen,
    MPEG_PACKET_DATA *pPacketData
);

/*  Inlines to get PTSs from MPEG2 packets */
inline BOOL MPEG2PacketHasPTS(const BYTE * pbPacket)
{
    /*  Just check if the PTS_DTS_flags are 10 or 11 (ie the first
        bit is one
    */
    return 0 != (pbPacket[7] & 0x80);
}

/*  Get the PTS from an MPEG2 packet */
LONGLONG MPEG2PacketPTS(const BYTE * pbPacket);


typedef struct {
    DWORD          dwProfile;          //  Profile - MPEG2 only
    DWORD          dwLevel;            //  Level - MPEG2 only
    LONG           lWidth;             //  Native Width in pixels
    LONG           lHeight;            //  Native Height in pixels
    LONG           lvbv;               //  vbv
    REFERENCE_TIME  tPictureTime;      //  Time per picture in 100ns units
    float          fPictureRate;       //  In frames per second
    LONG           lTimePerFrame;      //  Time per picture in MPEG units
    LONG           dwBitRate;          //  Bits per second
    LONG           lXPelsPerMeter;     //  Pel aspect ratio
    LONG           lYPelsPerMeter;     //  Pel aspect ratio
    DWORD          dwStartTimeCode;    //  First GOP time code (or -1)
    LONG           lActualHeaderLen;   //  Length of valid bytes in raw seq hdr
    BYTE           RawHeader[140];     //  The real sequence header
} SEQHDR_INFO;

/*  MPEG2 stuff */
typedef struct {
    BOOL           bExtensionPresent;
    BOOL           bDisplayExtensionPresent;
    BOOL           bScalableExtensionPresent;
    SEQHDR_INFO    seqhdrInfo;

} MPEG2_SEQHDR_INFO;

/*  Helper */
int inline SequenceHeaderSize(const BYTE *pb)
{
    /*  No quantization matrices ? */
    if ((pb[11] & 0x03) == 0x00) {
        return 12;
    }
    /*  Just non-intra quantization matrix ? */
    if ((pb[11] & 0x03) == 0x01) {
        return 12 + 64;
    }
    /*  Intra found - is there a non-intra ? */
    if (pb[11 + 64] & 0x01) {
        return 12 + 64 + 64;
    } else {
        return 12 + 64;
    }
}

/*  Extract info from video sequence header

    Returns FALSE if the sequence header is invalid
*/

BOOL ParseSequenceHeader(const BYTE *pbData, LONG lData, SEQHDR_INFO *hdrInfo);

BOOL ParseAudioHeader(const BYTE * pbData, MPEG1WAVEFORMAT *pFormat);

/*  Construct a media type from the video info */
HRESULT GetVideoMediaType(CMediaType *cmt, BOOL bPayload, const SEQHDR_INFO *pInfo);

/*  Find the next start code */
BOOL NextStartCode(const BYTE * *ppbData, DWORD *pdwLeft);

/*  Parse AC3 header */
BOOL ParseAC3Header(const BYTE * pbData, DOLBYAC3WAVEFORMAT *pwf);
