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
* Module Name: Decoder.cpp
*
* Implements a prototype Mpeg Video Software codec.  It just consume
* the passed in packets.
*
\**************************************************************************/
#include "MpgVideo.h"



/* -------------------------------------------------------------------------
** CVideoDecoder
** -------------------------------------------------------------------------
*/

/******************************Public*Routine******************************\
* CVideoDecoder
*
* Constructor and destructor
*
\**************************************************************************/
CVideoDecoder::CVideoDecoder(CMpegVideoCodec *pMpegCodec)
    : m_pCodec(pMpegCodec) {}



/******************************Public*Routine******************************\
* ResetVideo
*
*
*
\**************************************************************************/
DWORD
CVideoDecoder::ResetVideo()
{
    CAutoLock lck(this);
    return DECODE_SUCCESS;
}


/******************************Public*Routine******************************\
* DecodeFrame
*
*
*
\**************************************************************************/
DWORD
CVideoDecoder::DecodeFrame(
    VideoCtrl *pCtrl
    )
{
    CAutoLock   lck(this);
    BOOL        Done = FALSE;
    DWORD       rc = DECODE_SUCCESS;


    m_pCtrl = pCtrl;

    if ((m_pCtrl->dwCtrl & 0xffff) == DECODE_DIS) {

        LPBYTE lpOutputBuffer = m_pCodec->GetDecodeBufferAndFormat();

        if (lpOutputBuffer == NULL) {
            rc = DECODE_ERR_QUARTZ;
        }
        return rc;
    }

    m_pCtrl->dwSkipFlag = 1;
    while (!Done) {

        DWORD sc = NextStartCode();

        switch (sc) {

        case PICTURE_START_CODE:
            DbgLog((LOG_TRACE, 2, TEXT("PICTURE_START_CODE")));
            m_pCtrl->pFrameStartPos = m_pCtrl->pCmprRead;
            rc = SkipFrame();
            Done = TRUE;
            break;

        case SEQUENCE_HEADER_CODE:
            DbgLog((LOG_TRACE, 2, TEXT("SEQUENCE_HEADER_CODE")));
            Discard32Bits();
            break;

        case SEQUENCE_END_CODE:
            DbgLog((LOG_TRACE, 2, TEXT("SEQUENCE_END_CODE")));
            Discard32Bits();
            Done = TRUE;
            break;

        case GROUP_START_CODE:
            DbgLog((LOG_TRACE, 2, TEXT("GROUP_START_CODE")));
            Discard32Bits();
            break;

        case (DWORD)-1:
            //
            // We didn't find a valid start code.
            //
            rc = DECODE_ERR_DATA;
            Done = TRUE;
            break;

        default:
            DbgLog((LOG_TRACE, 2, TEXT("Unexpected start code %#X"), sc));
            Discard32Bits();
            break;
        }
    }


    if (!m_pCtrl->dwSkipFlag) {

        LPBYTE lpOutputBuffer = m_pCodec->GetDecodeBufferAndFormat();

        if (lpOutputBuffer == NULL) {
            rc = DECODE_ERR_QUARTZ;
        }
    }

    return rc;
}


/*****************************Private*Routine******************************\
* NextStartCode
*
* Returns the next start code of -1 if none found within the buffer limits.
* Advances pCmprRead to point to begining of the start code.
*
\**************************************************************************/
DWORD
CVideoDecoder::NextStartCode()
{
    LPBYTE  lpCurr = m_pCtrl->pCmprRead;
    LPBYTE  lpEnd = m_pCtrl->pCmprWrite;
    int     sm = 0;
    DWORD   StartCode = 0x00000100L;

    while ((lpCurr < lpEnd) && sm < 3) {

        switch (sm) {
        case 0:
            lpCurr = (LPBYTE)memchr(lpCurr, 0, lpEnd - lpCurr);
            if (lpCurr != NULL) lpCurr++, sm = 1;
            else lpCurr = lpEnd;
            break;

        case 1:
            if (*lpCurr++ == 0) sm = 2;
            else sm = 0;
            break;

        case 2:
            if (*lpCurr == 1) sm = 3;
            else if (*lpCurr == 0) sm = 2;
            else sm = 0;
            lpCurr++;
            break;
        }
    }

    //
    // When we get here we have either run out of buffer or found the first
    // 3 bytes of a valid start code.
    //
    // If we have the first three bytes of the start code.  The next
    // byte completes the start code.
    //
    // Don't forget to put back the start code bytes that we have just
    // read otherwise they would be lost forever.
    //
    if (lpCurr >= lpEnd) {
        StartCode = (DWORD)-1;
    }
    else {
        StartCode += *lpCurr;
    }
    lpCurr -= sm;

    m_pCtrl->pCmprRead = lpCurr;
    return StartCode;
}



/*****************************Private*Routine******************************\
* Discard32Bits
*
* Throw away the next 32 bits
*
\**************************************************************************/
void
CVideoDecoder::Discard32Bits()
{
    m_pCtrl->pCmprRead += 4;
}


/*****************************Private*Routine******************************\
* SkipFrame
*
*
*
\**************************************************************************/
DWORD
CVideoDecoder::SkipFrame()
{
    DWORD   dwX;

    //
    // If there isn't enough room for the information required then
    // then return 0L
    //
    if (m_pCtrl->pCmprRead > (m_pCtrl->pCmprWrite - 8)) {
        return DECODE_ERR_DATA;
    }

    Discard32Bits();    // Eat the picyure start code

    dwX = ByteSwap(*(UNALIGNED DWORD *)m_pCtrl->pCmprRead);
    m_pCtrl->pCmprRead += 4;

    m_pCtrl->dwTemporalReference  = (dwX & 0xFFC00000) >> 22;
    m_pCtrl->dwFrameType = (dwX & 0x00380000) >> 19;

    switch (m_pCtrl->dwCtrl & 0xffff) {
    case DECODE_NULL:
        m_pCtrl->dwSkipFlag = 1;
        break;

    case DECODE_I:
        m_pCtrl->dwSkipFlag = (m_pCtrl->dwFrameType != FTYPE_I);
        break;

    case DECODE_IP:
        m_pCtrl->dwSkipFlag = (m_pCtrl->dwFrameType == FTYPE_B);
        break;

    case DECODE_IPB:
        m_pCtrl->dwSkipFlag = 0;
        break;
    }

    for (; ; ) {

        dwX = NextStartCode();

        if (dwX == (DWORD)-1) {
            return DECODE_ERR_DATA;
        }
        else {

            BYTE Code = (BYTE)(dwX & 0xFF);

            if (Code == 0x00 || Code > 0xAF) {
                return DECODE_SUCCESS;
            }
            else {
                Discard32Bits();
            }
        }
    }
}
