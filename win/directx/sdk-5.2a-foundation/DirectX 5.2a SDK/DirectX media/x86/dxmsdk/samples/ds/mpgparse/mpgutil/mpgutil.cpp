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
/*  MPEG 1 & 2 parsing routines */

#include <streams.h>
#include <mpegdef.h>
#include <mmreg.h>
#include <mpeg2typ.h>
#include <mpgutil.h>

#ifdef DEBUG
LPCTSTR PictureTypes[8]   = { TEXT("forbidden frame type"),
                              TEXT("I-Frame"),
                              TEXT("P-Frame"),
                              TEXT("B-Frame"),
                              TEXT("D-Frame"),
                              TEXT("Reserved frame type"),
                              TEXT("Reserved frame type"),
                              TEXT("Reserved frame type")
                            };
LPCTSTR PelAspectRatios[16] = { TEXT("Forbidden"),
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
                                TEXT("Reserved") };
LPCTSTR PictureRates[16] = { TEXT("Forbidden"),
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
                             TEXT("Reserved") };
#endif // DBG

const LONG PictureTimes[16] = { 0,
                                (LONG)((double)10000000 / 23.976),
                                (LONG)((double)10000000 / 24),
                                (LONG)((double)10000000 / 25),
                                (LONG)((double)10000000 / 29.97),
                                (LONG)((double)10000000 / 30),
                                (LONG)((double)10000000 / 50),
                                (LONG)((double)10000000 / 59.94),
                                (LONG)((double)10000000 / 60)
                              };

const float fPictureRates[] =
   {
     (float)0,
     (float)23.976,
     (float)24,
     (float)25,
     (float)29.97,
     (float)30,
     (float)50,
     (float)59.94,
     (float)60.0
   };

const LONG AspectRatios[16] = { 0,
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

/*  Bit rate tables */
const WORD BitRates[3][16] =
{{  0, 32,  64,  96,  128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 0 },
 {  0, 32,  48,  56,   64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384, 0 },
 {  0, 32,  40,  48,   56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 0 }
};

BOOL inline TESTBIT(const BYTE * pbData, int n)
{
    return 0 != (pbData[n >> 3] & (0x80 >> (n & 7)));
}

/*  Read an extended clock */
BOOL GetExtendedClock(const BYTE * pbData, LONGLONG *pllClock)
{
    /*  Check marker bits
        These occur at bit positions 5, 21, 37, 47 */
    if (!TESTBIT(pbData, 5) ||
        !TESTBIT(pbData, 21) ||
        !TESTBIT(pbData, 37) ||
        !TESTBIT(pbData, 47)) {
        DbgLog((LOG_ERROR, 1, TEXT("Invalid extended clock marker bits")));
        return FALSE;
    }

    /*  Compute the clock value

        * = ignore
        M = Marker
        x = value

            0        1       2        3        4
        **xxxMxx xxxxxxxx xxxxxMxx xxxxxxxx xxxxxM**
    */
    LARGE_INTEGER liClock;
    liClock.HighPart = (pbData[0] & 0x20) != 0;
    liClock.LowPart = ((pbData[0] & 0x18) << 27) +
                      ((pbData[0] & 0x03) << 28) +
                      ( pbData[1]         << 20) +
                      ((pbData[2] & 0xF8) << 12) +
                      ((pbData[2] & 0x03) << 13) +
                      ( pbData[3]         << 5) +
                      ( pbData[4]         >> 3);

    /*  Now compute the residual */
    LONG lCRE = ((pbData[4] & 3) << 7) + (pbData[5] >> 1);
    if (lCRE >= 300) {
        DbgLog((LOG_ERROR, 1, TEXT("Clock reference extension >= 300")));
        return FALSE;
    }

    *pllClock = liClock.QuadPart * 300 + lCRE;
    return TRUE;
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

DWORD ParseMPEG2PackHeader(const BYTE * pbData, DWORD cbData)
{
    ASSERT(cbData >= 4);
    ASSERT(*(UNALIGNED DWORD *)pbData == DWORD_SWAP(PACK_START_CODE));

    /*  Is there enough for the basic length ? */
    if (cbData < 18) {
        return 0;
    }
    DWORD dwLen = 14 + (pbData[13] & 0x07);
    if (cbData < dwLen + 4) {
        return 0;
    }

    /*  Check check bits '01' */
    if (pbData[4] & 0xC0 != 0x40) {
        DbgLog((LOG_ERROR, 1, TEXT("Invalid bits at start of pack")));
        return 4;
    }
    /*  Get SCR */
    LONGLONG llSCR;
    if (!GetExtendedClock(pbData + 4, &llSCR)) {
        DbgLog((LOG_ERROR, 1, TEXT("Invalid pack header SCR field")));
        return 4;
    }

    /*  Get the mux rate */
    DWORD dwMuxRate = (pbData[10] << (22 - 8)) +
                      (pbData[11] << (22 - 16)) +
                      (pbData[12] >> (24 - 22));
    /*  Check the marker bits */
    if ((pbData[12] & 0x03) != 0x03) {
        DbgLog((LOG_ERROR, 1, TEXT("Invalid marker bits in pack header")));
        return 4;
    }

    /*  Check the stuffing bytes */
    for (DWORD i = 14; i < dwLen; i++ ) {
        if (pbData[i] != 0xFF) {
            DbgLog((LOG_ERROR, 1, TEXT("Invalid pack stuffing byte 0x%2.2X"),
                    pbData[i]));
            return 4;
        }
    }
    DbgLog((LOG_TRACE, 4, TEXT("Decoded pack - ESCR %s, Mux Rate %d bytes/sec"),
            (LPCTSTR)CDisp(llSCR, CDISP_DEC),
            dwMuxRate * 50));

    return dwLen;
}

/*  Parse system header

    Parameters :
        pbData - pointer to data containing the system header
        cbData - length of data

    Returns :
        Number of bytes processed (0 if the header doesn't fit
        in the data or 4 if the header is invalid)
*/

LONG ParseSystemHeader(const BYTE * pbData, DWORD cbData)
{
    BOOL bHasAudio = FALSE;
    BOOL bHasVideo = FALSE;

    ASSERT(cbData >= 4);
    ASSERT(*(UNALIGNED DWORD *)pbData == DWORD_SWAP(SYSTEM_HEADER_START_CODE));

    /*  Checkt the length */
    if (cbData < 6) {
        return 0;
    }

    DWORD dwLen = 6 + pbData[5] + (pbData[4] << 8);
    if (dwLen < SYSTEM_HEADER_BASIC_LENGTH) {
        DbgLog((LOG_ERROR, 1, TEXT("Invalid System Header length")));
        return 4;
    }

    if (cbData < dwLen) {
        return 0;
    }
    /*  Check the marker bits */
    if (0 == (pbData[6] & 0x80) ||
        0 == (pbData[8] & 0x01) ||
        0 == (pbData[10] & 0x20)) {
        DbgLog((LOG_ERROR, 1, TEXT("Invalid System Header Marker bits")));
        return 4;
    }

    /*  Parse the 'buffer bounds and scale' list */
    const BYTE * pbNext = pbData + SYSTEM_HEADER_BASIC_LENGTH;
    DWORD cbLeft = dwLen - SYSTEM_HEADER_BASIC_LENGTH;
    for ( ; cbLeft >= 3; cbLeft -= 3, pbNext += 3) {
        if (pbNext[0] == AUDIO_GLOBAL) {
            bHasAudio == TRUE;
        } else
        if (pbNext[0] == VIDEO_GLOBAL) {
            bHasVideo = TRUE;
        } else {
            if (pbNext[0] < PROGRAM_STREAM_MAP) {
                DbgLog((LOG_ERROR, 1, TEXT("Invalid stream id in system header")));
                return 4;
            }
            if (IsVideoStreamId(pbNext[0])) {
                bHasVideo = TRUE;
            } else
            if (IsAudioStreamId(pbNext[0])) {
                bHasAudio = TRUE;
            }
        }
    }
    if (cbLeft != 0) {
        DbgLog((LOG_ERROR, 1, TEXT("Invalid system header length")));
        return 4;
    }
    DbgLog((LOG_TRACE, 4, TEXT("System Header %s, %s"),
            bHasAudio ? TEXT("audio") : TEXT("No audio"),
            bHasVideo ? TEXT("video") : TEXT("No video")));
    return dwLen;
}

/*  Parse an MPEG-1 packet */
DWORD ParseMPEG1Packet(
    const BYTE * pbData,
    DWORD cbData,
    MPEG_PACKET_DATA *pPacketData
)
{
    ZeroMemory((PVOID)pPacketData, sizeof(*pPacketData));
    DWORD dwStartCode = DWORD_SWAP(*(UNALIGNED DWORD *)pbData);
    DbgLog((LOG_TRACE, 4, TEXT("Parse packet %d bytes"), cbData));
    /*  Send it to the right stream */
    if (cbData < 6) {
        return 0;
    }

    /*  Find the length */
    DWORD dwLen = ((LONG)pbData[4] << 8) + (LONG)pbData[5] + 6;
    DbgLog((LOG_TRACE, 4, TEXT("Packet length %d bytes"), dwLen));
    if (dwLen > cbData) {
        return 0;
    }
    pPacketData->dwPacketLen = dwLen;

    /*  Pull out PTS if any */
    DWORD dwHeaderSize = 6;

    if (dwStartCode != PRIVATE_STREAM_2) {
        DWORD dwPts = 6;
        for (;;) {
            if (dwPts >= dwLen) {
                return 4;
            }

            if (pbData[dwPts] & 0x80) {
                /*  Stuffing byte */
                if (pbData[dwPts] != 0xFF) {
                    return 4;
                }
                dwPts++;
                continue;
            }

            /*  Check for STD (nextbits == '01') -
                we know the next bit is 0 so check the next one after that
            */
            if (pbData[dwPts] & 0x40) { // STD stuff
                dwPts += 2;
                continue;
            }

            /*  No PTS - normal case */
            if (pbData[dwPts] == 0x0F) {
                dwHeaderSize = dwPts + 1;
                break;
            }

            if ((pbData[dwPts] & 0xF0) == 0x20 ||
                (pbData[dwPts] & 0xF0) == 0x30) {


                /*  PTS or PTS and DTS */
                dwHeaderSize = (pbData[dwPts] & 0xF0) == 0x20 ? dwPts + 5 :
                                                             dwPts + 10;
                if (dwHeaderSize > dwLen) {
                    return 4;
                }
                if (!GetClock(pbData + dwPts, &pPacketData->llPts)) {
                    return 4;
                }
                pPacketData->bHasPts = TRUE;
                break;
            } else {
                return 4;
                break;
            }
        }
    }
    pPacketData->dwHeaderLen = dwHeaderSize;
    return dwLen;
}

/*  Parse an MPEG-2 packet */
DWORD ParseMPEG2Packet(
    const BYTE * pbData,
    DWORD cbData,
    MPEG_PACKET_DATA *pPacketData)
{
    ASSERT(cbData >= 4);
    ASSERT(pbData[3] >= PROGRAM_STREAM_MAP);

    ZeroMemory((PVOID)pPacketData, sizeof(*pPacketData));

    if (cbData < 6) {
        return 0;
    }
    DWORD dwLen = 6 + pbData[5] + (pbData[4] << 8);
    if (cbData < dwLen + 4) {
        return 0;
    }

    /*  Check for normal */
    switch (pbData[3]) {
    case PROGRAM_STREAM_MAP:
    case PRIVATE_STREAM_2:
    case ECM_STREAM:
    case EMM_STREAM:
    case PROGRAM_STREAM_DIRECTORY:
    case 0xF2: // DSMCC_Stream
    case 0xF8: // H.222 Type E
    case PADDING_STREAM:
        /*  Just the bytes */
        pPacketData->dwHeaderLen = 6;
        pPacketData->dwPacketLen = dwLen;
        pPacketData->bHasPts = FALSE;
        pPacketData->llPts = 0;
        break;

    /*  PES header */
    default:
        {
            if (!ParseMPEG2PacketHeader(pbData, dwLen, pPacketData)) {
                return 4;
            }
        }
        break;

    }
    return dwLen;
}

/*  Parse a packet header when present */
BOOL ParseMPEG2PacketHeader(
    const BYTE * pbData,
    DWORD dwLen,
    MPEG_PACKET_DATA *pPacketData
)
{
    /*  Compute the length */
    if (dwLen < 6 + 3) {
        return FALSE;
    }

    pPacketData->dwHeaderLen = 6 + 3 + pbData[8];
    pPacketData->dwPacketLen = dwLen;

    /*  Extract the PTS */
    if (MPEG2PacketHasPTS(pbData)) {
        pPacketData->bHasPts = TRUE;
        pPacketData->llPts = MPEG2PacketPTS(pbData);
    } else {
        pPacketData->bHasPts = FALSE;
        pPacketData->llPts = 0;
    }

    return TRUE;
}

BOOL ParseSequenceHeader(const BYTE *pbData, LONG lData, SEQHDR_INFO *pInfo)
{
    ASSERT(*(UNALIGNED DWORD *)pbData == DWORD_SWAP(SEQUENCE_HEADER_CODE));

    /*  Check random marker bit */
    if (!(pbData[10] & 0x20)) {
        DbgLog((LOG_ERROR, 2, TEXT("Sequence header invalid marker bit")));
        return FALSE;
    }

    DWORD dwWidthAndHeight = ((DWORD)pbData[4] << 16) +
                             ((DWORD)pbData[5] << 8) +
                             ((DWORD)pbData[6]);

    pInfo->lWidth = dwWidthAndHeight >> 12;
    pInfo->lHeight = dwWidthAndHeight & 0xFFF;
    DbgLog((LOG_TRACE, 2, TEXT("Width = %d, Height = %d"),
        pInfo->lWidth,
        pInfo->lHeight));

    /* the '8' bit is the scramble flag used by sigma designs - ignore */
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

    pInfo->tPictureTime = (LONGLONG)PictureTimes[PelAspectRatioAndPictureRate & 0x0F];
    pInfo->fPictureRate = fPictureRates[PelAspectRatioAndPictureRate & 0x0F];
    pInfo->lTimePerFrame = MulDiv((LONG)pInfo->tPictureTime, 9, 1000);

    /*  Pull out the bit rate and aspect ratio for the type */
    pInfo->dwBitRate = ((((DWORD)pbData[8] << 16) +
                   ((DWORD)pbData[9] << 8) +
                   (DWORD)pbData[10]) >> 6);
    if (pInfo->dwBitRate == 0x3FFFF) {
        DbgLog((LOG_TRACE, 2, TEXT("Variable video bit rate")));
        pInfo->dwBitRate = 0;
    } else {
        pInfo->dwBitRate *= 400;
        DbgLog((LOG_TRACE, 2, TEXT("Video bit rate is %d bits per second"),
               pInfo->dwBitRate));
    }

#if 0
#pragma message (REMIND("Get pel aspect ratio right don't call GDI - it will create a thread!"))
    /*  Get a DC */
    HDC hdc = GetDC(GetDesktopWindow());

    ASSERT(hdc != NULL);
    /*  Guess (randomly) 39.37 inches per meter */
    LONG lNotionalPelsPerMeter = MulDiv((LONG)GetDeviceCaps(hdc, LOGICALPELSX),
                                        3937, 100);
#else
    LONG lNotionalPelsPerMeter = 2000;
#endif

    pInfo->lXPelsPerMeter = lNotionalPelsPerMeter;

    pInfo->lYPelsPerMeter = MulDiv(
                              lNotionalPelsPerMeter,
                              AspectRatios[PelAspectRatioAndPictureRate >> 4],
                              10000);
    /*  Pull out the vbv */
    pInfo->lvbv = ((((LONG)pbData[10] & 0x1F) << 5) |
             ((LONG)pbData[11] >> 3)) * 2048;

    DbgLog((LOG_TRACE, 2, TEXT("vbv size is %d bytes"), pInfo->lvbv));

    /*  Check constrained parameter stuff */
    if (pbData[11] & 0x04) {
        DbgLog((LOG_TRACE, 2, TEXT("Constrained parameter video stream")));

        if (pInfo->lvbv > 40960) {
            DbgLog((LOG_ERROR, 1, TEXT("Invalid vbv (%d) for Constrained stream"),
                    pInfo->lvbv));

            /*  Have to let this through too!  bisp.mpg has this */
            /*  But constrain it since it might be random        */
            pInfo->lvbv = 40960;
        }
    } else {
        DbgLog((LOG_TRACE, 2, TEXT("Non-Constrained parameter video stream")));
    }

    /*  tp_orig has a vbv of 2048 (!) */
    if (pInfo->lvbv < 20000) {
        DbgLog((LOG_TRACE, 2, TEXT("Small vbv (%d) - setting to 40960"),
               pInfo->lvbv));
        pInfo->lvbv = 40960;
    }

    pInfo->lActualHeaderLen = SequenceHeaderSize(pbData);
    CopyMemory((PVOID)pInfo->RawHeader, (PVOID)pbData, pInfo->lActualHeaderLen);
    return TRUE;
}

HRESULT GetVideoMediaType(CMediaType *cmt, BOOL bPayload, const SEQHDR_INFO *pInfo)
{
    cmt->majortype = MEDIATYPE_Video;
    cmt->subtype = bPayload ? MEDIASUBTYPE_MPEG1Payload :
                              MEDIASUBTYPE_MPEG1Packet;
    VIDEOINFO *videoInfo =
        (VIDEOINFO *)cmt->AllocFormatBuffer(FIELD_OFFSET(MPEG1VIDEOINFO, bSequenceHeader[pInfo->lActualHeaderLen]));
    if (videoInfo == NULL) {
        return E_OUTOFMEMORY;
    }
    RESET_HEADER(videoInfo);

    videoInfo->dwBitRate          = pInfo->dwBitRate;
    videoInfo->rcSource.right     = pInfo->lWidth;
    videoInfo->bmiHeader.biWidth  = pInfo->lWidth;
    videoInfo->rcSource.bottom    = pInfo->lHeight;
    videoInfo->bmiHeader.biHeight = pInfo->lHeight;
    videoInfo->bmiHeader.biXPelsPerMeter = pInfo->lXPelsPerMeter;
    videoInfo->bmiHeader.biYPelsPerMeter = pInfo->lYPelsPerMeter;
    videoInfo->bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);

    videoInfo->AvgTimePerFrame = pInfo->tPictureTime;
    MPEG1VIDEOINFO *mpgvideoInfo = (MPEG1VIDEOINFO *)videoInfo;
    mpgvideoInfo->cbSequenceHeader = pInfo->lActualHeaderLen;
    CopyMemory((PVOID)mpgvideoInfo->bSequenceHeader,
               (PVOID)pInfo->RawHeader,
               pInfo->lActualHeaderLen);
    mpgvideoInfo->dwStartTimeCode = pInfo->dwStartTimeCode;


    cmt->SetFormatType(&FORMAT_MPEGVideo);
    return S_OK;
}

BOOL CheckAudioHeader(const BYTE * pbData)
{
    /*  Just check it's valid */
#pragma message (REMIND("Check audio header"))
    if ((pbData[2] & 0x0C) == 0x0C) {
        DbgLog((LOG_ERROR, 2, TEXT("Invalid audio sampling frequency")));
        return FALSE;
    }
    if ((pbData[1] & 0x08) != 0x08) {
        DbgLog((LOG_ERROR, 2, TEXT("Invalid audio ID bit = 0")));
        return FALSE;
    }
    if (((pbData[1] >> 1) & 3) == 0x00) {
        DbgLog((LOG_ERROR, 2, TEXT("Invalid audio Layer")));
        return FALSE;
    }

    if (((pbData[2] >> 2) & 3) == 3) {
        DbgLog((LOG_ERROR, 2, TEXT("Invalid sample rate")));
        return FALSE;
    }
    if ((pbData[2] >> 4) == 0x0F) {
        DbgLog((LOG_ERROR, 2, TEXT("Invalid bit rate")));
        return FALSE;
    }

    return TRUE;
}

LONG SampleRate(const BYTE * pbData)
{
    switch ((pbData[2] >> 2) & 3) {
        case 0:
            return 44100;

        case 1:
            return 48000;

        case 2:
            return 32000;

        default:
            DbgBreak("Unexpected Sample Rate");
            return 44100;
    }
}

BOOL ParseAudioHeader(const BYTE * pbData, MPEG1WAVEFORMAT *pFormat)
{
    if (!CheckAudioHeader(pbData)) {
        return FALSE;
    }
    pFormat->wfx.wFormatTag = WAVE_FORMAT_MPEG;

    /*  Get number of channels from Mode */
    switch (pbData[3] >> 6) {
    case 0x00:
        pFormat->fwHeadMode = ACM_MPEG_STEREO;
        break;
    case 0x01:
        pFormat->fwHeadMode = ACM_MPEG_JOINTSTEREO;
        break;
    case 0x02:
        pFormat->fwHeadMode = ACM_MPEG_DUALCHANNEL;
        break;
    case 0x03:
        pFormat->fwHeadMode = ACM_MPEG_SINGLECHANNEL;
        break;
    }
    pFormat->wfx.nChannels =
        (WORD)(pFormat->fwHeadMode == ACM_MPEG_SINGLECHANNEL ? 1 : 2);
    pFormat->fwHeadModeExt = (WORD)(1 << (pbData[3] >> 4));
    pFormat->wHeadEmphasis = (WORD)((pbData[3] & 0x03) + 1);
    pFormat->fwHeadFlags   = (WORD)(((pbData[2] & 1) ? ACM_MPEG_PRIVATEBIT : 0) +
                           ((pbData[3] & 8) ? ACM_MPEG_COPYRIGHT : 0) +
                           ((pbData[3] & 4) ? ACM_MPEG_ORIGINALHOME : 0) +
                           ((pbData[1] & 1) ? ACM_MPEG_PROTECTIONBIT : 0) +
                           ((pbData[1] & 0x08) ? ACM_MPEG_ID_MPEG1 : 0));

    int Layer;

    /*  Get the layer so we can work out the bit rate */
    switch ((pbData[1] >> 1) & 3) {
        case 3:
            pFormat->fwHeadLayer = ACM_MPEG_LAYER1;
            Layer = 1;
            break;
        case 2:
            pFormat->fwHeadLayer = ACM_MPEG_LAYER2;
            Layer = 2;
            break;
        case 1:
            pFormat->fwHeadLayer = ACM_MPEG_LAYER3;
            Layer = 3;
            break;
        case 0:
            return (FALSE);
    }

    /*  Get samples per second from sampling frequency */
    pFormat->wfx.nSamplesPerSec = SampleRate(pbData);
    pFormat->dwHeadBitrate =
        (DWORD)BitRates[Layer - 1][pbData[2] >> 4] * 1000;
    pFormat->wfx.nAvgBytesPerSec = pFormat->dwHeadBitrate / 8;

    /*  Deal with free format (!) */
#pragma message (REMIND("Handle variable bit rate (index 0)"))

    if (pFormat->wfx.nSamplesPerSec != 44100 &&
        /*  Layer 3 can sometimes switch bitrates */
        !(Layer == 3 && /* !m_pStreamList->AudioLock() && */
            (pbData[2] >> 4) == 0)) {

        if (Layer == 1) {
            pFormat->wfx.nBlockAlign = (WORD)
                (4 * ((pFormat->dwHeadBitrate * 12) / pFormat->wfx.nSamplesPerSec));
        } else {
            pFormat->wfx.nBlockAlign = (WORD)
                ((144 * pFormat->dwHeadBitrate) / pFormat->wfx.nSamplesPerSec);
        }
    } else {
        pFormat->wfx.nBlockAlign = 1;
    }


    pFormat->wfx.wBitsPerSample = 0;
    pFormat->wfx.cbSize = sizeof(MPEG1WAVEFORMAT) - sizeof(WAVEFORMATEX);

    pFormat->dwPTSLow  = 0;
    pFormat->dwPTSHigh = 0;

    return TRUE;
}

BOOL GetClock(const BYTE * pData, LONGLONG *Clock)
{
    BYTE  Byte1 = pData[0];
    DWORD Word2 = ((DWORD)pData[1] << 8) + (DWORD)pData[2];
    DWORD Word3 = ((DWORD)pData[3] << 8) + (DWORD)pData[4];

    /*  Do checks */
    if ((Byte1 & 0xE0) != 0x20 ||
        (Word2 & 1) != 1 ||
        (Word3 & 1) != 1) {
        DbgLog((LOG_TRACE, 2, TEXT("Invalid clock field - 0x%2.2X 0x%4.4X 0x%4.4X"),
            Byte1, Word2, Word3));
        return FALSE;
    }

    LARGE_INTEGER liClock;
    liClock.HighPart = (Byte1 & 8) != 0;
    liClock.LowPart  = (DWORD)((((DWORD)Byte1 & 0x6) << 29) +
                       (((DWORD)Word2 & 0xFFFE) << 14) +
                       ((DWORD)Word3 >> 1));

    *Clock = liClock.QuadPart;

    return TRUE;
}

/*  Find the next start code */
BOOL NextStartCode(const BYTE * *ppbData, DWORD *pdwLeft)
{
    const BYTE * pbData = *ppbData;
    DWORD dwLeft = *pdwLeft;

    while (dwLeft > 4 &&
           (*(UNALIGNED DWORD *)pbData & 0x00FFFFFF) != 0x00010000) {
        dwLeft--;
        pbData++;
    }
    *ppbData = pbData;
    *pdwLeft = dwLeft;
    return dwLeft >= 4;
}

/*  Parse AC3 header */
BOOL ParseAC3Header(const BYTE * pbData, DOLBYAC3WAVEFORMAT *pwf)
{
    ZeroMemory((PVOID)pwf, sizeof(*pwf));
    pwf->wfx.cbSize = sizeof(DOLBYAC3WAVEFORMAT) - sizeof(WAVEFORMATEX);

    /*  First check it IS AC3 */
    if (pbData[0] == 0x77 && pbData[1] == 0x0B) {
    } else
    if (pbData[0] == 0x0B && pbData[1] == 0x77) {
        pwf->bBigEndian = TRUE;
    } else {
        return FALSE;
    }

    /*  Get the sampling rate */
    BYTE bData = pwf->bBigEndian ? pbData[4] : pbData[5];
    int SampleRateCode = bData >> 6;
    int BitRateCode = bData & 0x3F;
    switch (SampleRateCode) {
    case 1:
        pwf->wfx.nSamplesPerSec = 44100;
        break;
    case 2:
        pwf->wfx.nSamplesPerSec = 32000;
        break;

    default:  // Isn't one illegal?
        pwf->wfx.nSamplesPerSec = 48000;
        break;
    }

    if (BitRateCode >= 38) {
        return FALSE;
    }

    /*  Get the bit rate */
    int BitRates[] =
    {
      32,  32,  40,  40,  48,  48,  56,  56,  64,  64,
      80,  80,  96,  96, 112, 112, 128, 128, 160, 160,
      192, 192, 224, 224, 256, 256, 320, 320, 384, 384,
      448, 448, 512, 512, 576, 576, 640, 640
    };
    pwf->wfx.nAvgBytesPerSec = BitRates[BitRateCode] * (1000 / 8);

    /*  Get the frame size
        Since every frame contains 1536 samples we can compute it as

        Bypes per frame  = Samples per frame (1536)  * Bytes per second
                           --------------------------------------------
                                      Samples per second
    */
    pwf->wfx.nBlockAlign = MulDiv(1536,
                              pwf->wfx.nAvgBytesPerSec,
                              pwf->wfx.nSamplesPerSec);

    pwf->wfx.nChannels = 6; // Hack!
    pwf->wfx.wFormatTag = WAVE_FORMAT_DOLBY_AC3;

    DbgLog((LOG_TRACE, 3, TEXT("Wave Format Dolby AC3")));
    DbgLog((LOG_TRACE, 3, TEXT("%d bytes per sec, %d samples per sec, align %d"),
           pwf->wfx.nAvgBytesPerSec,
           pwf->wfx.nSamplesPerSec,
           pwf->wfx.nBlockAlign));

    return TRUE;
}

/*  Return the PTS clock value as a 33 bit number
    Returns LONGLONG(-1) if the clock is invalid
*/
LONGLONG MPEG2PacketPTS(const BYTE * pbPacket)
{
    ASSERT(MPEG2PacketHasPTS(pbPacket));
    LONGLONG llClock;
    if (!GetClock(pbPacket + 9, &llClock)) {
        return (LONGLONG)-1;
    }
    return llClock;
}
