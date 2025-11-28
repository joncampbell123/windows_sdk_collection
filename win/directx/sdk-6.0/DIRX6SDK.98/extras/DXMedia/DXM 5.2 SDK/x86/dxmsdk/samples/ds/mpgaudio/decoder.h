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
* Module Name: Decoder.h
*
*
\**************************************************************************/

#ifndef _INC_DECODER_H
#define _INC_DECODER_H


// -------------------------------------------------------------------------
// Forward declaration to resolve circular reference.
// -------------------------------------------------------------------------
//
class CMpegAudioCodec;


// -------------------------------------------------------------------------
// Audio decoder class
//
// This class encapsulates a low-level mpeg video decoder.
// -------------------------------------------------------------------------
//

// Decode control option
enum {
    DECODE_MONO       = 0x00000001L,
    DECODE_STEREO     = 0x00000002L,
    DECODE_QUARTER    = 0x00000800L,
    DECODE_HALF       = 0x00001000L,
    DECODE_FULL       = 0x00002000L,
    DECODE_HALF_HIQ   = 0x00004000L,
    DECODE_HALF_FULLQ = 0x00008000L,
    DECODE_16BIT      = 0x00010000L,
    DECODE_8BIT       = 0x00020000L,
    DECODE_QSOUND     = 0x00040000L,
    DECODE_QUART_INT  = 0x00080000L
};



// Function Return Values
enum {
    DECODE_SUCCESS    = 0x0000L,
    DECODE_ERR_MEMORY = 0x0001L,
    DECODE_ERR_DATA   = 0x0002L,
    DECODE_ERR_PARAM  = 0x0004L,
    DECODE_ERR_QUARTZ = 0x0008L     // error from Quartz callback
};

struct AudioCtrl {

    //
    // Output Frame Buffer
    //
    DWORD   dwOutBuffUsed;
    DWORD   dwOutBuffSize;
    DWORD   dwMpegError;

    //
    // Frame decoder control
    //
    DWORD   dwCtrl;


    //
    // Input buffer fields
    //
    DWORD   dwNumFrames;
    LPBYTE  pCmprRead;
    LPBYTE  pCmprWrite;
    LPBYTE  pOutBuffer;
};


class CAudioDecoder
    : public CCritSec {

private:
    //
    // Back pointer to the object that created us.
    //
    CMpegAudioCodec *m_pCodec;
    AudioCtrl       *m_pCtrl;

    BOOL    SkipFrame();

public:
    CAudioDecoder(CMpegAudioCodec *pMpegCodec);

    DWORD   ResetAudio();
    DWORD   DecodeAudioFrame(AudioCtrl *pCtrl);
};
#endif
