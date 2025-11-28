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
* Implements a prototype Mpeg Audio Software codec.  It just consumes
* the passed in audio frames.
*
*
\**************************************************************************/
#include "MpgAudio.h"



/* -------------------------------------------------------------------------
** CAudioDecoder
** -------------------------------------------------------------------------
*/

/******************************Public*Routine******************************\
* CAudioDecoder
*
* Constructor and destructor
*
*
\**************************************************************************/
CAudioDecoder::CAudioDecoder(CMpegAudioCodec *pMpegCodec)
    : m_pCodec(pMpegCodec) {}



/******************************Public*Routine******************************\
* ResetAudio
*
*
\**************************************************************************/
DWORD
CAudioDecoder::ResetAudio()
{
    CAutoLock lck(this);
    return DECODE_SUCCESS;
}


/******************************Public*Routine******************************\
* DecodeFrame
*
*
\**************************************************************************/
DWORD
CAudioDecoder::DecodeAudioFrame(
    AudioCtrl *pCtrl
    )
{
    CAutoLock   lck(this);
    DWORD       rc = DECODE_SUCCESS;

    m_pCtrl = pCtrl;


    for (DWORD i = 0; i < m_pCtrl->dwNumFrames; i++) {

        // Skip the sync word
        m_pCtrl->pCmprRead += 2;

        // skip the rest of the frame
        if (!SkipFrame()) {
            m_pCtrl->dwOutBuffUsed = 0L;
            return DECODE_ERR_DATA;
        }
    }

    ZeroMemory(m_pCtrl->pOutBuffer, m_pCtrl->dwOutBuffSize);
    m_pCtrl->dwOutBuffUsed = m_pCtrl->dwOutBuffSize;

    return rc;
}


/*****************************Private*Routine******************************\
* SkipFrame
*
*
\**************************************************************************/
BOOL
CAudioDecoder::SkipFrame()
{
    LPBYTE  lpCurr = m_pCtrl->pCmprRead;
    LPBYTE  lpEnd = m_pCtrl->pCmprWrite;
    int sm = 0;

    while (lpCurr < lpEnd && sm < 2)  {

        switch (sm) {
        case 0:
            sm = (*lpCurr == 0xff);
            break;

        case 1:
            if ((*lpCurr & 0xf0) == 0xf0) sm = 2; /* sync found */
            else sm = (*lpCurr == 0xff);
            break;
        }
        lpCurr++;
    }

    //
    // When we get here we have either run out of buffer or found the first
    // "sm" bytes of a valid sync word.
    //
    // Don't forget to put back the sync word bytes that we have just
    // read otherwise they would be lost forever.
    //
    lpCurr -= sm;
    m_pCtrl->pCmprRead = lpCurr;

    if (sm < 2) {
        return FALSE;   // sync not found.
    }

    return TRUE;
}
