/******************************Module*Header*******************************\
* Module Name: Strips.c
*
*
* Copyright (c) 1992 Microsoft Corporation
\**************************************************************************/

#include "driver.h"
#include "lines.h"

USHORT usMaskWord(LINESTATE *pLineState, LONG cPels);

VOID vDumpLineData(PPDEV ppdev, STRIP *Strip, LINESTATE *LineState);

/******************************************************************************
 *
 *****************************************************************************/
VOID vSetStrips(
    PPDEV ppdev,
    LINEATTRS *pla,
    INT color,
    INT mix)
{
    USHORT  wS3Mix;

    wS3Mix = Rop2ToS3Rop[(mix & 0xFF)-1];

    // Wait for just enough room in the FIFO

    FIFOWAIT(FIFO_3_EMPTY);

    // Send out some of the commands.

    TEST_AND_SET_FRGD_MIX(FOREGROUND_COLOR | wS3Mix);
    TEST_AND_SET_FRGD_COLOR(color);

    if ((pla->fl & LA_ALTERNATE) ||
        (pla->pstyle != (FLOAT_LONG*) NULL))
    {
        TEST_AND_SET_BKGD_MIX(LEAVE_ALONE);
        SET_BKGD_COLOR(0);
        OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | CPU_DATA));
    }
    else
    {
        OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));
    }

    return;
}

/******************************************************************************
 *
 *****************************************************************************/
VOID vssSolidHorizontal(
    PPDEV ppdev,
    STRIP *pStrip,
    LINESTATE *pLineState)
{
    LONG    i, cStrips;
    PLONG   pStrips;
    LONG    xPels, xSumPels, yDir;
    USHORT  Cmd, ssCmd, dirDraw, dirSkip;

    Cmd = NOP | DRAW | WRITE | MULTIPLE_PIXELS |
          DIR_TYPE_RADIAL | LAST_PIXEL_OFF |
          BUS_SIZE_16 | BYTE_SWAP;

    cStrips = pStrip->cStrips;

    FIFOWAIT(FIFO_3_EMPTY);

    OUTPW(CUR_X, pStrip->ptlStart.x);
    OUTPW(CUR_Y, pStrip->ptlStart.y);
    OUTPW(CMD, Cmd);

    // Setup the drawing direction and the skip direction.

    dirDraw = 0x10;

    if (!(pStrip->flFlips & FL_FLIP_V))
    {
        yDir = 1;
        dirSkip = 0xC100;
    }
    else
    {
        dirSkip = 0x4100;
        yDir = -1;
    }

    // Output the short stroke commands.

    xSumPels = 0;
    pStrips = pStrip->alStrips;
    for (i = 0; i < cStrips; i++)
    {
        xPels = *pStrips++;
        xSumPels += xPels;
        ssCmd = (USHORT) (dirSkip | dirDraw | xPels);
        FIFOWAIT(FIFO_4_EMPTY);
        OUTPW(SHORT_STROKE_REG, ssCmd);
    }

    pStrip->ptlStart.x += xSumPels;
    pStrip->ptlStart.y += cStrips * yDir;

}


/******************************************************************************
 *
 *****************************************************************************/
VOID vrlSolidHorizontal(
    PPDEV ppdev,
    STRIP *pStrip,
    LINESTATE *pLineState)
{
    LONG    cStrips;
    USHORT  Cmd;
    LONG    i, yInc, x, y;
    PLONG   pStrips;


    Cmd = DRAW_LINE      | DRAW            | DIR_TYPE_RADIAL |
          LAST_PIXEL_OFF | MULTIPLE_PIXELS | DRAWING_DIRECTION_0 |
          WRITE;

    cStrips = pStrip->cStrips;

    x = pStrip->ptlStart.x;
    y = pStrip->ptlStart.y;

    yInc = 1;
    if (pStrip->flFlips & FL_FLIP_V)
        yInc = -1;

    pStrips = pStrip->alStrips;

    for (i = 0; i < cStrips; i++)
    {
        FIFOWAIT(FIFO_4_EMPTY);

        OUTPW(CUR_X, x);
        OUTPW(CUR_Y, y);
        OUTPW(LINE_MAX, *pStrips);
        OUTPW(CMD, Cmd);

        x += *pStrips++;
        y += yInc;
    }

    pStrip->ptlStart.x = x;
    pStrip->ptlStart.y = y;

}

/******************************************************************************
 *
 *****************************************************************************/
VOID vssSolidVertical(
    PPDEV ppdev,
    STRIP *pStrip,
    LINESTATE *pLineState)
{
    LONG    i, cStrips;
    PLONG   pStrips;
    LONG    yPels, ySumPels, yDir;
    USHORT  Cmd, ssCmd, dirDraw, dirSkip;

    Cmd = NOP | DRAW | WRITE | MULTIPLE_PIXELS |
          DIR_TYPE_RADIAL | LAST_PIXEL_OFF |
          BUS_SIZE_16 | BYTE_SWAP;

    cStrips = pStrip->cStrips;

    FIFOWAIT(FIFO_3_EMPTY);

    OUTPW(CUR_X, pStrip->ptlStart.x);
    OUTPW(CUR_Y, pStrip->ptlStart.y);
    OUTPW(CMD, Cmd);

    // Setup the drawing direction and the skip direction.

    if (!(pStrip->flFlips & FL_FLIP_V))
    {
        yDir = 1;
        dirDraw = 0xD0;
    }
    else
    {
        yDir = -1;
        dirDraw = 0x50;
    }

    dirSkip = 0x0100;

    // Output the short stroke commands.

    ySumPels = 0;
    pStrips = pStrip->alStrips;
    for (i = 0; i < cStrips; i++)
    {
        yPels = *pStrips++;
        ySumPels += yPels;
        ssCmd = (USHORT) (dirSkip | dirDraw | yPels);
        FIFOWAIT(FIFO_4_EMPTY);
        OUTPW(SHORT_STROKE_REG, ssCmd);
    }

    pStrip->ptlStart.x += cStrips;
    pStrip->ptlStart.y += ySumPels * yDir;

}


/******************************************************************************
 *
 *****************************************************************************/
VOID vrlSolidVertical(
    PPDEV ppdev,
    STRIP *pStrip,
    LINESTATE *pLineState)
{
    LONG    cStrips;
    USHORT  Cmd;
    LONG    i, x, y;
    PLONG   pStrips;

    cStrips = pStrip->cStrips;
    pStrips = pStrip->alStrips;

    x = pStrip->ptlStart.x;
    y = pStrip->ptlStart.y;

    if (!(pStrip->flFlips & FL_FLIP_V))
    {
        Cmd = DRAW_LINE      | DRAW            | DIR_TYPE_RADIAL |
              LAST_PIXEL_OFF | MULTIPLE_PIXELS | DRAWING_DIRECTION_270 |
              WRITE;

        for (i = 0; i < cStrips; i++)
        {
            FIFOWAIT(FIFO_4_EMPTY);

            OUTPW(CUR_X, x);
            OUTPW(CUR_Y, y);
            OUTPW(LINE_MAX, *pStrips);
            OUTPW(CMD, Cmd);

            y += *pStrips++;
            x++;
        }

    }
    else
    {
        Cmd = DRAW_LINE      | DRAW            | DIR_TYPE_RADIAL |
              LAST_PIXEL_OFF | MULTIPLE_PIXELS | DRAWING_DIRECTION_90 |
              WRITE;

        for (i = 0; i < cStrips; i++)
        {
            FIFOWAIT(FIFO_4_EMPTY);

            OUTPW(CUR_X, x);
            OUTPW(CUR_Y, y);
            OUTPW(LINE_MAX, *pStrips);
            OUTPW(CMD, Cmd);

            y -= *pStrips++;
            x++;
        }
    }

    pStrip->ptlStart.x = x;
    pStrip->ptlStart.y = y;

}


/******************************************************************************
 *
 *****************************************************************************/
VOID vssSolidDiagonalHorizontal(
    PPDEV ppdev,
    STRIP *pStrip,
    LINESTATE *pLineState)
{
    LONG    i, cStrips;
    PLONG   pStrips;
    LONG    Pels, SumPels, yDir;
    USHORT  Cmd, ssCmd, dirDraw, dirSkip;

    Cmd = NOP | DRAW | WRITE | MULTIPLE_PIXELS |
          DIR_TYPE_RADIAL | LAST_PIXEL_OFF |
          BUS_SIZE_16 | BYTE_SWAP;

    cStrips = pStrip->cStrips;

    FIFOWAIT(FIFO_3_EMPTY);

    OUTPW(CUR_X, pStrip->ptlStart.x);
    OUTPW(CUR_Y, pStrip->ptlStart.y);
    OUTPW(CMD, Cmd);

    // Setup the drawing direction and the skip direction.

    if (!(pStrip->flFlips & FL_FLIP_V))
    {
        yDir = 1;
        dirDraw = 0xF0;
        dirSkip = 0x4100;

    }
    else
    {
        yDir = -1;
        dirDraw = 0x30;
        dirSkip = 0xC100;

    }

    // Output the short stroke commands.

    SumPels = 0;
    pStrips = pStrip->alStrips;
    for (i = 0; i < cStrips; i++)
    {
        Pels = *pStrips++;
        SumPels += Pels;
        ssCmd = (USHORT)(dirSkip | dirDraw | Pels);
        FIFOWAIT(FIFO_4_EMPTY);
        OUTPW(SHORT_STROKE_REG, ssCmd);
    }

    pStrip->ptlStart.x += SumPels;
    pStrip->ptlStart.y += (SumPels - cStrips) * yDir;

}


/******************************************************************************
 *
 *****************************************************************************/
VOID vrlSolidDiagonalHorizontal(
    PPDEV ppdev,
    STRIP *pStrip,
    LINESTATE *pLineState)
{
    LONG    cStrips;
    USHORT  Cmd;
    LONG    i, x, y;
    PLONG   pStrips;

    cStrips = pStrip->cStrips;
    pStrips = pStrip->alStrips;

    x = pStrip->ptlStart.x;
    y = pStrip->ptlStart.y;

    if (!(pStrip->flFlips & FL_FLIP_V))
    {
        Cmd = DRAW_LINE      | DRAW            | DIR_TYPE_RADIAL |
              LAST_PIXEL_OFF | MULTIPLE_PIXELS | DRAWING_DIRECTION_315 |
              WRITE;

        for (i = 0; i < cStrips; i++)
        {
            FIFOWAIT(FIFO_4_EMPTY);

            OUTPW(CUR_X, x);
            OUTPW(CUR_Y, y);
            OUTPW(LINE_MAX, *pStrips);
            OUTPW(CMD, Cmd);

            y += *pStrips - 1;
            x += *pStrips++;
        }

    }
    else
    {
        Cmd = DRAW_LINE      | DRAW            | DIR_TYPE_RADIAL |
              LAST_PIXEL_OFF | MULTIPLE_PIXELS | DRAWING_DIRECTION_45 |
              WRITE;

        for (i = 0; i < cStrips; i++)
        {
            FIFOWAIT(FIFO_4_EMPTY);

            OUTPW(CUR_X, x);
            OUTPW(CUR_Y, y);
            OUTPW(LINE_MAX, *pStrips);
            OUTPW(CMD, Cmd);

            y -= *pStrips - 1;
            x += *pStrips++;
        }
    }

    pStrip->ptlStart.x = x;
    pStrip->ptlStart.y = y;

}


/******************************************************************************
 *
 *****************************************************************************/
VOID vssSolidDiagonalVertical(
    PPDEV ppdev,
    STRIP *pStrip,
    LINESTATE *pLineState)
{
    LONG    i, cStrips;
    PLONG   pStrips;
    LONG    Pels, SumPels, yDir;
    USHORT  Cmd, ssCmd, dirDraw, dirSkip;

    Cmd = NOP | DRAW | WRITE | MULTIPLE_PIXELS |
          DIR_TYPE_RADIAL | LAST_PIXEL_OFF |
          BUS_SIZE_16 | BYTE_SWAP;

    cStrips = pStrip->cStrips;

    FIFOWAIT(FIFO_3_EMPTY);

    OUTPW(CUR_X, pStrip->ptlStart.x);
    OUTPW(CUR_Y, pStrip->ptlStart.y);
    OUTPW(CMD, Cmd);

    // Setup the drawing direction and the skip direction.

    if (!(pStrip->flFlips & FL_FLIP_V))
    {
        yDir = 1;
        dirDraw = 0xF0;
    }
    else
    {
        yDir = -1;
        dirDraw = 0x30;
    }

    dirSkip = 0x8100;

    // Output the short stroke commands.

    SumPels = 0;
    pStrips = pStrip->alStrips;
    for (i = 0; i < cStrips; i++)
    {
        Pels = *pStrips++;
        SumPels += Pels;
        ssCmd = (USHORT)(dirSkip | dirDraw | Pels);
        FIFOWAIT(FIFO_4_EMPTY);
        OUTPW(SHORT_STROKE_REG, ssCmd);
    }

    pStrip->ptlStart.x += SumPels - cStrips;
    pStrip->ptlStart.y += SumPels * yDir;

}

/******************************************************************************
 *
 *****************************************************************************/
VOID vrlSolidDiagonalVertical(
    PPDEV ppdev,
    STRIP *pStrip,
    LINESTATE *pLineState)
{
    LONG    cStrips;
    USHORT  Cmd;
    LONG    i, x, y;
    PLONG   pStrips;

    cStrips = pStrip->cStrips;
    pStrips = pStrip->alStrips;

    x = pStrip->ptlStart.x;
    y = pStrip->ptlStart.y;

    if (!(pStrip->flFlips & FL_FLIP_V))
    {
        Cmd = DRAW_LINE      | DRAW            | DIR_TYPE_RADIAL |
              LAST_PIXEL_OFF | MULTIPLE_PIXELS | DRAWING_DIRECTION_315 |
              WRITE;

        for (i = 0; i < cStrips; i++)
        {
            FIFOWAIT(FIFO_4_EMPTY);

            OUTPW(CUR_X, x);
            OUTPW(CUR_Y, y);
            OUTPW(LINE_MAX, *pStrips);
            OUTPW(CMD, Cmd);

            y += *pStrips;
            x += *pStrips++ - 1;
        }

    }
    else
    {
        Cmd = DRAW_LINE      | DRAW            | DIR_TYPE_RADIAL |
              LAST_PIXEL_OFF | MULTIPLE_PIXELS | DRAWING_DIRECTION_45 |
              WRITE;

        for (i = 0; i < cStrips; i++)
        {
            FIFOWAIT(FIFO_4_EMPTY);

            OUTPW(CUR_X, x);
            OUTPW(CUR_Y, y);
            OUTPW(LINE_MAX, *pStrips);
            OUTPW(CMD, Cmd);

            y -= *pStrips;
            x += *pStrips++ - 1;
        }
    }


    pStrip->ptlStart.x = x;
    pStrip->ptlStart.y = y;

}

/******************************************************************************
 *
 *****************************************************************************/
VOID vStripStyledHorizontal(
    PPDEV ppdev,
    STRIP *pStrip,
    LINESTATE *pLineState)
{
    LONG    cStrips;
    USHORT  Cmd;
    LONG    i, yInc, x, y, cPels;
    PLONG   pStrips;

    DISPDBG((3, "\nvStripStyledHorizontal - Entry\n"));

    Cmd = DRAW_LINE      | DRAW            | DIR_TYPE_RADIAL |
          LAST_PIXEL_OFF | MULTIPLE_PIXELS | DRAWING_DIRECTION_0 |
          WRITE          |
          BUS_SIZE_16    | WAIT;

    cStrips = pStrip->cStrips;

    x = pStrip->ptlStart.x;
    y = pStrip->ptlStart.y;

    yInc = 1;
    if (pStrip->flFlips & FL_FLIP_V)
        yInc = -1;

    pStrips = pStrip->alStrips;

    for (i = 0; i < cStrips; i++)
    {
        cPels = *pStrips;

        FIFOWAIT(FIFO_4_EMPTY);

        OUTPW(CUR_X, x);
        OUTPW(CUR_Y, y);
        OUTPW(LINE_MAX, cPels);
        OUTPW(CMD, Cmd);

        x += cPels;
        y += yInc;

        while (cPels >= 0)
        {
            FIFOWAIT(FIFO_1_EMPTY);
            OUTPW(PIXEL_TRANSFER, usMaskWord(pLineState, cPels));
            cPels -= 16;
        }

        pStrips++;
    }

    pStrip->ptlStart.x = x;
    pStrip->ptlStart.y = y;

}

/******************************************************************************
 *
 *****************************************************************************/
VOID vStripStyledVertical(
    PPDEV ppdev,
    STRIP *pStrip,
    LINESTATE *pLineState)
{
    LONG    cStrips;
    USHORT  Cmd;
    LONG    i, x, y, cPels;
    PLONG   pStrips;

    DISPDBG((3, "\nvStripStyledVertical - Entry\n"));

    cStrips = pStrip->cStrips;
    pStrips = pStrip->alStrips;

    x = pStrip->ptlStart.x;
    y = pStrip->ptlStart.y;

    if (!(pStrip->flFlips & FL_FLIP_V))
    {
        Cmd = DRAW_LINE      | DRAW            | DIR_TYPE_RADIAL |
              LAST_PIXEL_OFF | MULTIPLE_PIXELS | DRAWING_DIRECTION_270 |
              WRITE          |
              BUS_SIZE_16    | WAIT;

        for (i = 0; i < cStrips; i++)
        {
            cPels = *pStrips;

            FIFOWAIT(FIFO_4_EMPTY);

            OUTPW(CUR_X, x);
            OUTPW(CUR_Y, y);
            OUTPW(LINE_MAX, cPels);
            OUTPW(CMD, Cmd);

            y += cPels;
            x++;

            while (cPels >= 0)
            {
                FIFOWAIT(FIFO_1_EMPTY);
                OUTPW(PIXEL_TRANSFER, usMaskWord(pLineState, cPels));
                cPels -= 16;
            }

            pStrips++;
        }

    }
    else
    {
        Cmd = DRAW_LINE      | DRAW            | DIR_TYPE_RADIAL |
              LAST_PIXEL_OFF | MULTIPLE_PIXELS | DRAWING_DIRECTION_90 |
              WRITE          |
              BUS_SIZE_16    | WAIT;


        for (i = 0; i < cStrips; i++)
        {
            cPels = *pStrips;

            FIFOWAIT(FIFO_4_EMPTY);

            OUTPW(CUR_X, x);
            OUTPW(CUR_Y, y);
            OUTPW(LINE_MAX, cPels);
            OUTPW(CMD, Cmd);

            y -= cPels;
            x++;

            while (cPels >= 0)
            {
                FIFOWAIT(FIFO_1_EMPTY);
                OUTPW(PIXEL_TRANSFER, usMaskWord(pLineState, cPels));
                cPels -= 16;
            }

            pStrips++;
        }
    }

    pStrip->ptlStart.x = x;
    pStrip->ptlStart.y = y;

}

/******************************************************************************
 *
 * This is a good example of how not to compute the mask for styled lines.
 *
 * The masks should be precomputed on entry to DrvStrokePath; computing
 * the pattern mask to be output to the pixel transfer register would then
 * be a couple of shifts and an Or.  Also, the style state would be updated
 * at the end of the strip function.
 *
 *****************************************************************************/

USHORT usMaskWord(
    LINESTATE* pls,
    LONG       cPels)
{
    ULONG ulMask = 0;         // Accumulating mask
    ULONG ulBit  = 0x8000;    // Rotating bit
    LONG  i;

// The S3 takes a word mask that accounts for at most 16 pixels:

    if (cPels > 16)
        cPels = 16;

    for (i = cPels; i--; i > 0)
    {
        ulMask |= (ulBit & pls->ulStyleMask);
        ulBit >>= 1;
        if (--pls->spRemaining == 0)
        {
        // Okay, we're onto the next entry in the style array, so if
        // we were working on a gap, we're now working on a dash (or
        // vice versa):

            pls->ulStyleMask = ~pls->ulStyleMask;

        // See if we've reached the end of the style array, and have to
        // wrap back around to the beginning:

            if (++pls->psp > pls->pspEnd)
                pls->psp = pls->pspStart;

        // Get the length of our new dash or gap, in pixels:

            pls->spRemaining = *pls->psp;
        }
    }

// Return the inverted result, because pls->ulStyleMask is inverted from
// the way you would expect it to be:

    return((USHORT) ~ulMask);
}

#if DBG

/******************************************************************************
 *
 *****************************************************************************/
VOID vDumpLineData(
    PPDEV ppdev,
    STRIP *Strip,
    LINESTATE *LineState)
{
    LONG    flFlips;
    PLONG   plStrips;
    LONG    i;

    DISPDBG((2, "Strip->cStrips: %d\n", Strip->cStrips));

    flFlips = Strip->flFlips;

    DISPDBG((2, "Strip->flFlips: %s%s%s%s%s\n",
                (flFlips & FL_FLIP_D)?         "FL_FLIP_D | "        : "",
                (flFlips & FL_FLIP_V)?         "FL_FLIP_V | "        : "",
                (flFlips & FL_FLIP_SLOPE_ONE)? "FL_FLIP_SLOPE_ONE | ": "",
                (flFlips & FL_FLIP_HALF)?      "FL_FLIP_HALF | "     : "",
                (flFlips & FL_FLIP_H)?         "FL_FLIP_H "          : ""));

    DISPDBG((2, "Strip->ptlStart: (%d, %d)\n",
                Strip->ptlStart.x,
                Strip->ptlStart.y));

    plStrips = Strip->alStrips;

    for (i = 0; i < Strip->cStrips; i++)
    {
        DISPDBG((2, "\talStrips[%d]: %d\n", i, plStrips[i]));
    }
}

#endif
