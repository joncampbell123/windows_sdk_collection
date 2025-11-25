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
*
*
\**************************************************************************/

#ifndef _INC_DECODER_H
#define _INC_DECODER_H


// -------------------------------------------------------------------------
// Forward declaration to resolve circular reference.
// -------------------------------------------------------------------------
//
class CMpegVideoCodec;

enum {
    SEQUENCE_HEADER_CODE = 0x000001B3,
    SEQUENCE_END_CODE    = 0x000001B7,
    GROUP_START_CODE     = 0x000001B8,
    PICTURE_START_CODE   = 0x00000100
};

// -------------------------------------------------------------------------
// Video decoder class
//
// This class encapsulates a low-level mpeg video decoder.
// -------------------------------------------------------------------------
//

// Decode control option
enum {
    DECODE_NULL = 0x0000L,  // Do not Decode any frame. Scan the stream
    DECODE_I    = 0x0001L,  // I frames only
    DECODE_IP   = 0x0003L,  // I and P frames
    DECODE_IPB  = 0x0007L,  // Decode all the frames
    DECODE_DIS  = 0x0040L   // No Decode, Convert only - force out last frame.
};

// Frame Types
enum {
    FTYPE_I     = 0x0001L,
    FTYPE_P     = 0x0002L,
    FTYPE_B     = 0x0003L
};

// Function Return Values
enum {
    DECODE_SUCCESS    = 0x0000L,
    DECODE_ERR_MEMORY = 0x0001L,
    DECODE_ERR_DATA   = 0x0002L,
    DECODE_ERR_PARAM  = 0x0004L,
    DECODE_ERR_QUARTZ = 0x0008L     // error from Quartz callback
};

struct VideoCtrl {
    //
    // Decoder Output Fields
    //
    DWORD   dwGopTimeStamp; // only valid after a GOP, -1 otherwise.
    DWORD   dwFrameType;
    LPBYTE  pFrameStartPos;
    DWORD   dwSkipFlag;     // 1 if the frame was skipped
    DWORD   dwTemporalReference;


    //
    // Output Frame Buffer
    //
    DWORD   dwOutputFormat;
    DWORD   dwOutputBuffNumber;
    LPBYTE  pFrameBuff;
    DWORD   dwOutStride;
    DWORD   dwOutOffset;
    DWORD   dwYStride;
    DWORD   dwYLines;

    //
    // Frame decoder control
    //
    DWORD   dwCtrl;

    //
    // Input buffer fields
    //
    LPBYTE  pCmprRead;
    LPBYTE  pCmprWrite;
};


class CVideoDecoder
    : public CCritSec {

private:
    //
    // Back pointer to the object that created us.
    //
    CMpegVideoCodec *m_pCodec;
    VideoCtrl       *m_pCtrl;

    DWORD   NextStartCode();
    DWORD   SkipFrame();
    void    Discard32Bits();

public:
    CVideoDecoder(CMpegVideoCodec *pMpegCodec);

    DWORD   ResetVideo();
    DWORD   DecodeFrame(VideoCtrl *pCtrl);
};
#endif
