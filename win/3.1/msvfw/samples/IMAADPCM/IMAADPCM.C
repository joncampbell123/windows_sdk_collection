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
//  imaadpcm.c
//
//  Description:
//      This file contains encode and decode routines for the IMA's ADPCM
//      format. This format is the same format used in Intel's DVI standard.
//      Intel has made this algorithm public domain and the IMA has endorsed
//      this format as a standard for audio compression.
//
//  WARNING:
//      The audio quality of this implementation of IMA ADPCM is
//	seriously degraded due to improperly encoding the
//      step index in a block header. This algorithm sounds
//      much better if done correctly.
//
//	Due to this problem and some others, please do not use this
//	codec as sample code for implementing IMA ADPCM encode or decode.
//	This code is provided to assist the development of ACM codecs and
//	not as an example of a proper IMA ADPCM implementation.
//
//	If you wish sample code for a proper implementation of this
//	algorithm, please contact the IMA or look for forthcoming
//	releases on Compuserve.
//
//
//==========================================================================;

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <msacm.h>
#include <msacmdrv.h>
#include "codec.h"
#include "imaadpcm.h"

#include "debug.h"


#ifndef HPBYTE
    #ifdef WIN32
        #define HPBYTE
    #else
        typedef BYTE _huge *HPBYTE;
    #endif
#endif


//
//
//
//
//
static short next_step[16] =
{
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8
};

//
//
//
//
//
static short step[89] =
{
        7,     8,     9,    10,    11,    12,    13,
       14,    16,    17,    19,    21,    23,    25,
       28,    31,    34,    37,    41,    45,    50,
       55,    60,    66,    73,    80,    88,    97,
      107,   118,   130,   143,   157,   173,   190,
      209,   230,   253,   279,   307,   337,   371,
      408,   449,   494,   544,   598,   658,   724,
      796,   876,   963,  1060,  1166,  1282,  1411,
     1552,  1707,  1878,  2066,  2272,  2499,  2749,
     3024,  3327,  3660,  4026,  4428,  4871,  5358,
     5894,  6484,  7132,  7845,  8630,  9493, 10442,
    11487, 12635, 13899, 15289, 16818, 18500, 20350,
    22385, 24623, 27086, 29794, 32767
};


//--------------------------------------------------------------------------;
//
//  DWORD FNGLOBAL imaadpcmDecode4Bit
//
//  Description:
//      
//
//  Arguments:
//      LPIMAADPCMWAVEFORMAT pwfADPCM:
//
//      LPBYTE pbSrc:
//
//      LPPCMWAVEFORMAT pwfPCM:
//
//      LPBYTE pbDst:
//
//      DWORD cbSrcLen:
//
//  Return (DWORD):
//
//
//  History:
//      11/16/92    Created. 
//
//--------------------------------------------------------------------------;

DWORD FNGLOBAL imaadpcmDecode4Bit
(
    LPIMAADPCMWAVEFORMAT    pwfADPCM,
    LPBYTE                  pbSrc,
    LPPCMWAVEFORMAT         pwfPCM,
    LPBYTE                  pbDst,
    DWORD                   cbSrcLen
)
{
    short   iInput;
    short   iNextInput;
    short   iFirstNibble;
    long    lSamp;
    BYTE    bChannels;
    BYTE    bBitsPerSample;
    BYTE    m;
    UINT    n;
    UINT    uSamplesPerBlock;
    UINT    uBlockHeaderBytes;
    DWORD   dwTotalPos;
    DWORD   dwDecoded;

    short   aiSamp[IMAADPCM_MAX_CHANNELS];
    short   aiStepIndex[IMAADPCM_MAX_CHANNELS];

    long    diff;

    HPBYTE  hpSrc = pbSrc;      // !!! fix for win 32 !!!
    HPBYTE  hpDst = pbDst;


    //
    //  put some commonly used info in more accessible variables and init
    //  the uBlockHeaderBytes, dwDecoded and dwTotalPos vars...
    //
    bChannels           = (BYTE)pwfADPCM->wfx.nChannels;
    bBitsPerSample      = (BYTE)pwfPCM->wBitsPerSample;
    uSamplesPerBlock    = pwfADPCM->wSamplesPerBlock;
    uBlockHeaderBytes   = bChannels * 4;
    dwDecoded           = 0L;
    dwTotalPos          = 0L;

    //
    //  step through each byte of ADPCM data and decode it to the requested
    //  PCM format (8 or 16 bit).
    //
    while (dwTotalPos < cbSrcLen)
    {
        //
        //  the first thing we need to do with each block of ADPCM data is
        //  to read the header which consists of 4 bytes of data per channel.
        //  so our first check is to make sure that we have _at least_
        //  enough input data for a complete ADPCM block header--if there
        //  is not enough data for the header, then exit.
        //
        //  the header structure per channel looks like this:
        //      2 bytes (16 bit) sample
        //      1 byte for step table index
        //      1 byte padding (dword align)
        //
        //  this gives us (4 * bChannels) bytes of header information. note
        //  that as long as there is _at least_ (4 * bChannels) of header
        //  info, we will grab the two samples from the header (and if no
        //  data exists following the header we will exit in the decode
        //  loop below).
        //
        dwTotalPos += uBlockHeaderBytes;
        if (dwTotalPos > cbSrcLen)
            goto adpcmDecode4BitExit;
            
        //
        //  get the sample and step table index for each channel
        //
        for (m = 0; m < bChannels; m++)
        {
            aiSamp[m] = *(short huge *)hpSrc;
            hpSrc++;
            hpSrc++;

            aiStepIndex[m] = (short)*hpSrc++;

            //
            //  inc one more to get rid of padding byte
            //
            hpSrc++;
        }


        //
        //  write out first sample for each channel.
        //
        if (bBitsPerSample == (BYTE)8)
        {
            for (m = 0; m < bChannels; m++)
            {
                *hpDst++ = (char)((aiSamp[m] >> 8) + 128);
            }
        }
        else
        {
            for (m = 0; m < bChannels; m++)
            {
                *(short huge *)hpDst = aiSamp[m];
                hpDst++;
                hpDst++;
            }
        }

        //
        //  we have decoded the first sample for this block, so add one
        //  to our decoded count
        //
        dwDecoded++;


        //
        //  we now need to decode the 'data' section of the ADPCM block.
        //  this consists of packed 4 bit nibbles.
        //
        iFirstNibble = 1;
        for (n = 1; n < uSamplesPerBlock; n++)
        {
            for (m = 0; m < bChannels; m++)
            {
                if (iFirstNibble)
                {
                    //
                    //  we need to grab the next byte to decode--make sure
                    //  that there is a byte for us to grab before continue
                    //
                    dwTotalPos++;
                    if (dwTotalPos > cbSrcLen)
                        goto adpcmDecode4BitExit;

                    //
                    //  grab the next two nibbles and create sign extended
                    //  integers out of them:
                    //
                    //      iInput is the first nibble to decode
                    //      iNextInput will be the next nibble decoded
                    //
                    iNextInput  = (short)*hpSrc++;
                    iInput      = iNextInput >> 4;
                    iNextInput  = (iNextInput << 12) >> 12;

                    iFirstNibble = 0;
                }
                else
                {
                    //
                    //  put the next sign extended nibble into iInput and
                    //  decode it--also set iFirstNibble back to 1 so we
                    //  will read another byte from the source stream on
                    //  the next iteration...
                    //
                    iInput = iNextInput;
                    iFirstNibble = 1;
                }


                //
                //  IMA ADPCM Decoder
                //
                //  compute new sample estimate lSamp
                //
                //      iInput = 16 bit sign extended encoded nibble
                //
                //
                lSamp = aiSamp[m];

                diff = 0;
                if (iInput & 4) 
                    diff += step[aiStepIndex[m]];

                if (iInput & 2) 
                    diff += step[aiStepIndex[m]] >> 1;

                if (iInput & 1) 
                    diff += step[aiStepIndex[m]] >> 2;

                diff += step[aiStepIndex[m]] >> 3;

                if (iInput & 8)
                    diff = -diff;

                lSamp += diff;

                //
                //  check for overflow
                //
                if (lSamp > 32767)
                    lSamp = 32767;
                else if (lSamp < -32768)
                    lSamp = -32768;

//              DPF(3, " DECODE(%u): prev:%d, cur:%ld, inp:%d, step:%d", m, aiSamp[m], lSamp, iInput & 0x0F, aiStepIndex[m]);


                //
                //  lSamp contains the decoded iInput sample--now write it
                //  out to the destination buffer
                //
                if (bBitsPerSample == (BYTE)8)
                {
                    *hpDst++ = (char)(((short)lSamp >> 8) + 128);
                }
                else
                {
                    *(short huge *)hpDst = (short)lSamp;
                    hpDst++;
                    hpDst++;
                }
                
                //
                //  compute new stepsize step
                //
                aiStepIndex[m] += next_step[iInput & 0x0F];
                if (aiStepIndex[m] < 0)
                    aiStepIndex[m] = 0;
                else if (aiStepIndex[m] > 88)
                    aiStepIndex[m] = 88;

                aiSamp[m] = (short)lSamp;
            }

            //
            //  we have decoded one more complete sample
            //
            dwDecoded++;
        }
    }

    //
    //  we're done decoding the input data. dwDecoded contains the number
    //  of complete _SAMPLES_ that were decoded. we need to return the
    //  number of _BYTES_ decoded. so calculate the number of bytes per
    //  sample and multiply that with dwDecoded...
    //
adpcmDecode4BitExit:

    return (dwDecoded * ((bBitsPerSample >> (BYTE)3) << (bChannels >> 1)));
} // imaadpcmDecode4Bit()


//--------------------------------------------------------------------------;
//
//  DWORD FNGLOBAL imaadpcmEncode4Bit
//
//  Description:
//      
//
//  Arguments:
//      LPPCMWAVEFORMAT pwfPCM:
//
//      LPBYTE pbSrc:
//
//      LPIMAADPCMWAVEFORMAT pwfADPCM:
//
//      LPBYTE pbDst:
//
//      DWORD cbSrcLen:
//
//  Return (DWORD):
//
//
//  History:
//      11/16/92    Created. 
//
//--------------------------------------------------------------------------;

DWORD FNGLOBAL imaadpcmEncode4Bit
(
    LPPCMWAVEFORMAT         pwfPCM,
    LPBYTE                  pbSrc,
    LPIMAADPCMWAVEFORMAT    pwfADPCM,
    LPBYTE                  pbDst,
    DWORD                   cbSrcLen
)
{
    short   aiSamp[IMAADPCM_MAX_CHANNELS];
    short   aiStepIndex[IMAADPCM_MAX_CHANNELS];

    short   iSamp;
    short   iSample;
    long    lSamp;

    DWORD   dw;
    DWORD   dwTotalConverted;
    DWORD   dwInputPos;

    UINT    uSamplesPerBlock;
    UINT    cbSample;
    UINT    uBlockHeaderBytes;
    UINT    uBlockSize;
    UINT    uNextWrite;
    UINT    uFirstNibble;
    UINT    n;
    BYTE    m;
    BYTE    i;
    BYTE    bChannels;
    BYTE    bBitsPerSample;

    HPBYTE  pbSamples;

    short   mask;
    short   iOutput;
    short   tempstep;
    long    diff;

    HPBYTE  hpSrc = pbSrc;      // !!! fix for win 32 !!!
    HPBYTE  hpDst = pbDst;

    //
    //  first copy some information into more accessible (cheaper and shorter)
    //  variables--and precompute some stuff...
    //
    uSamplesPerBlock    = pwfADPCM->wSamplesPerBlock;
    bChannels           = (BYTE)pwfPCM->wf.nChannels;
    bBitsPerSample      = (BYTE)pwfPCM->wBitsPerSample;
    uBlockHeaderBytes   = bChannels * 4;
    dwInputPos          = 0L;
    dwTotalConverted    = 0L;

    //
    //  calculate the number of bytes per sample in the PCM data
    //
    //  note: the following code works because we _know_ that we only deal
    //  with 8 or 16 bits per sample PCM and 1 or 2 channels..
    //
    cbSample = (bBitsPerSample >> 3) << (bChannels >> 1);

    //
    //  step through each block of PCM data and encode it to 4 bit ADPCM
    //
    for ( ; dwInputPos < cbSrcLen; dwInputPos += (uBlockSize * cbSample))
    {
        //
        //  determine how much data we should encode for this block--this
        //  will be uSamplesPerBlock until we hit the last chunk of PCM
        //  data that will not fill a complete block. so on the last block
        //  we only encode that amount of data remaining...
        //
        dw = (cbSrcLen - dwInputPos) / cbSample;
        uBlockSize = (WORD)min((DWORD)uSamplesPerBlock, dw);

        if (uBlockSize == 0)
            break;

        //
        //
        //
        pbSamples = &hpSrc[dwInputPos];

        //
        //  write the block header for the encoded data
        //
        //  the block header is composed of the following data duplicated
        //  for each channel:
        //      2 bytes (16 bit) sample
        //      1 byte for step table index
        //      1 byte padding (dword align)
        //
        //  this gives us (4 * bChannels) bytes of header information
        //
        //  write the header for each channel
        //
        for (m = 0; m < bChannels; m++)
        {
            //
            //  grab the sample (for the current channel) to encode
            //  from the source and convert it to a 16 bit data if
            //  necessary
            //
            if (bBitsPerSample != 8)
            {
                iSample = *(short FAR *)pbSamples;
                pbSamples++;
                pbSamples++;
            }
            else
                iSample = ((short)*pbSamples++ - 128) << 8;

            aiSamp[m] = iSample;

            *(short huge *)hpDst = iSample;
            hpDst++;
            hpDst++;

            //
            //  !!! this needs to be index! this is an audio quality bug
            //      and needs to be fixed!
            //
            aiStepIndex[m] = 0;
            *hpDst++ = (BYTE)aiStepIndex[m];

            //
            //  padding
            //
            *hpDst++ = 0;
        }

        //
        //  the number of bytes that we have written to the destination
        //  buffer is (4 * bChannels)--so add this to our total number of
        //  bytes written to the destination.. for our return value.
        //
        dwTotalConverted += uBlockHeaderBytes;

        //
        //  we have written the header for this block--now write the data
        //  chunk (which consists of a bunch of encoded nibbles). note that
        //  we start our count at 1 because we already wrote the first
        //  sample into the destination buffer as part of the header
        //
        uFirstNibble = 1;
        for (n = 1; n < uBlockSize; n++)
        {
            //
            //  each channel gets encoded independently... obviously.
            //
            for (m = 0; m < bChannels; m++)
            {
                //
                //  copy into cheaper variables because we access them a lot
                //
                iSamp = aiSamp[m];

                //
                //  grab the sample to encode--convert it to 16 bit data if
                //  necessary...
                //
                if (bBitsPerSample != 8)
                {
                    iSample = *(short FAR *)pbSamples;
                    pbSamples++;
                    pbSamples++;
                }
                else
                    iSample = ((short)*pbSamples++ - 128) << 8;

                //
                //  IMA ADPCM Encoder:
                //      iSample is the 16 bit sample to encode
                //      iSamp is the previous sample
                //

                /* !!! difference may require 17 bits! */
                diff = (long)iSample - iSamp;

                if (diff >= 0)
                    iOutput = 0;             /* set sign bit */
                else
                {
                    iOutput = 8;
                    diff = -diff;
                }

                mask = 4;
                tempstep = step[aiStepIndex[m]];
        
                for (i=0; i<3; i++)           /* quantize diff sample */
                {
                    if (diff >= tempstep)
                    {
                        iOutput |= mask;
                        diff -= tempstep;
                    }
                    tempstep >>= 1;
                    mask >>= 1;
                }

//              DPF(3, " ENCODE(%u): prev:%d, cur:%d, out:%d, step:%d", m, iSamp, iSample, iOutput, aiStepIndex[m]);


                //
                //  IMA ADPCM Decoder
                //
                //  compute new sample estimate lSamp
                //
                //      iOutput = encoded nibble
                //
                //
                lSamp = iSamp;

                diff = 0;
                if (iOutput & 4) 
                    diff += step[aiStepIndex[m]];

                if (iOutput & 2) 
                    diff += step[aiStepIndex[m]] >> 1;

                if (iOutput & 1) 
                    diff += step[aiStepIndex[m]] >> 2;

                diff += step[aiStepIndex[m]] >> 3;

                if (iOutput & 8)
                    diff = -diff;

                lSamp += diff;

                //
                //  check for overflow
                //
                if (lSamp > 32767)
                    lSamp = 32767;
                else if (lSamp < -32768)
                    lSamp = -32768;

                //
                //  compute new stepsize step
                //
                aiStepIndex[m] += next_step[iOutput];
                if (aiStepIndex[m] < 0)
                    aiStepIndex[m] = 0;
                else if (aiStepIndex[m] > 88)
                    aiStepIndex[m] = 88;

                //
                //  save updated values for this channel back into the
                //  original arrays...
                //
                aiSamp[m] = (short)lSamp;


                //
                //  we have another nibble of encoded data--either combine
                //  this with the previous nibble and write out a full 
                //  byte, or save this nibble for the next nibble to be
                //  combined into a full byte and written to the destination
                //  buffer... uhg!
                //
                if (uFirstNibble)
                {
                    uNextWrite = (iOutput & 15) << 4;
                    uFirstNibble = 0;
                }
                else
                {
                    *hpDst++ = (BYTE)(uNextWrite | (iOutput & 15));
                    dwTotalConverted += 1;
                    uFirstNibble++;
                }
            }
        }
    }

    return (dwTotalConverted);
} // imaadpcmEncode4Bit()





//==========================================================================;
//
//
//
//
//==========================================================================;


LRESULT FNGLOBAL imaadpcmDecode4Bit_M08
(
    LPACMDRVSTREAMHEADER    padsh,
    DWORD                   dwSrcSamples
)
{

    return (MMSYSERR_ERROR);
}

LRESULT FNGLOBAL imaadpcmDecode4Bit_M16
(
    LPACMDRVSTREAMHEADER    padsh,
    DWORD                   dwSrcSamples
)
{

    return (MMSYSERR_ERROR);
}

LRESULT FNGLOBAL imaadpcmDecode4Bit_S08
(
    LPACMDRVSTREAMHEADER    padsh,
    DWORD                   dwSrcSamples
)
{

    return (MMSYSERR_ERROR);
}

LRESULT FNGLOBAL imaadpcmDecode4Bit_S16
(
    LPACMDRVSTREAMHEADER    padsh,
    DWORD                   dwSrcSamples
)
{

    return (MMSYSERR_ERROR);
}





//==========================================================================;
//
//
//
//
//==========================================================================;

LRESULT FNGLOBAL imaadpcmEncode4Bit_M08
(
    LPACMDRVSTREAMHEADER    padsh,
    DWORD                   dwSrcSamples
)
{

    return (MMSYSERR_ERROR);
}

LRESULT FNGLOBAL imaadpcmEncode4Bit_M16
(
    LPACMDRVSTREAMHEADER    padsh,
    DWORD                   dwSrcSamples
)
{

    return (MMSYSERR_ERROR);
}

LRESULT FNGLOBAL imaadpcmEncode4Bit_S08
(
    LPACMDRVSTREAMHEADER    padsh,
    DWORD                   dwSrcSamples
)
{

    return (MMSYSERR_ERROR);
}

LRESULT FNGLOBAL imaadpcmEncode4Bit_S16
(
    LPACMDRVSTREAMHEADER    padsh,
    DWORD                   dwSrcSamples
)
{

    return (MMSYSERR_ERROR);
}

