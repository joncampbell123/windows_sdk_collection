//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
//
//-------------------------------------------------------------------------;
//
//  imaadpcm.h
//
//  Description:
//
//
//
//==========================================================================;

#ifndef _IMAADPCM_H_
#define _IMAADPCM_H_

#define IMAADPCM_MAX_CHANNELS       2
#define IMAADPCM_BITS_PER_SAMPLE    4
#define IMAADPCM_WFX_EXTRA_BYTES    (sizeof(IMAADPCMWAVEFORMAT) - sizeof(WAVEFORMATEX))
 
//
//  function prototypes from IMAADPCM.C
//
// 
DWORD FNGLOBAL imaadpcmDecode4Bit
(
    LPIMAADPCMWAVEFORMAT    pwfADPCM,
    LPBYTE                  pbSrc,
    LPPCMWAVEFORMAT         pwfPCM,
    LPBYTE                  pbDst,
    DWORD                   cbSrcLen
);

DWORD FNGLOBAL imaadpcmEncode4Bit
(
    LPPCMWAVEFORMAT         pwfPCM,
    LPBYTE                  pbSrc,
    LPIMAADPCMWAVEFORMAT    pwfADPCM,
    LPBYTE                  pbDst,
    DWORD                   cbSrcLen
);




//
//
//
//
//
LRESULT FNGLOBAL imaadpcmDecode4Bit_M08
(
    LPACMDRVSTREAMHEADER    padsh,
    DWORD                   dwSrcSamples
);

LRESULT FNGLOBAL imaadpcmDecode4Bit_M16
(
    LPACMDRVSTREAMHEADER    padsh,
    DWORD                   dwSrcSamples
);

LRESULT FNGLOBAL imaadpcmDecode4Bit_S08
(
    LPACMDRVSTREAMHEADER    padsh,
    DWORD                   dwSrcSamples
);

LRESULT FNGLOBAL imaadpcmDecode4Bit_S16
(
    LPACMDRVSTREAMHEADER    padsh,
    DWORD                   dwSrcSamples
);

LRESULT FNGLOBAL imaadpcmEncode4Bit_M08
(
    LPACMDRVSTREAMHEADER    padsh,
    DWORD                   dwSrcSamples
);

LRESULT FNGLOBAL imaadpcmEncode4Bit_M16
(
    LPACMDRVSTREAMHEADER    padsh,
    DWORD                   dwSrcSamples
);

LRESULT FNGLOBAL imaadpcmEncode4Bit_S08
(
    LPACMDRVSTREAMHEADER    padsh,
    DWORD                   dwSrcSamples
);

LRESULT FNGLOBAL imaadpcmEncode4Bit_S16
(
    LPACMDRVSTREAMHEADER    padsh,
    DWORD                   dwSrcSamples
);

#endif
