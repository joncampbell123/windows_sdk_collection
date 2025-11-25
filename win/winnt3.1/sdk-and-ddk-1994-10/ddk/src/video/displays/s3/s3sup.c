/******************************Module*Header*******************************\
* Module Name: S3Sup.c
*
* S3 support routines.
*
* Copyright (c) 1992 Microsoft Corporation
*
\**************************************************************************/

#include "driver.h"

#define TOP_CLIP        0x1
#define LEFT_CLIP       0x2
#define RIGHT_CLIP      0x4
#define BOTTOM_CLIP     0x8

#if CATCHIT

ULONG nOutpAccesses,
      nOutpwAccesses,
      nInpAccesses,
      nInpwAccesses;

#define CHIP_STATE_TEST                                     \
            byte = inp(0x3d4);                              \
            outp(0x3d4, 0x39);                              \
            if ((inp(0x3d5) & 0xA0) != 0xA0)                \
                RIP("S3.DLL! Invalid Reg 39\n");            \
            outp(0x3d4, 0x40);                              \
            if (!(inp(0x3d5) & 0x1))                        \
                RIP("S3.DLL! Invalid Reg 40 bit 0\n");      \
            outp(0x3d4, byte);

#else

#if LOG_OUTS

#define MAX_LOG 256

typedef struct _logentry {
    WORD    port,
            val ;
} LOGENTRY;

LOGENTRY    aLogEntries[MAX_LOG];
INT         iCurrentLogEntry = 0;


#define LOG_OUTPUT                                      \
    aLogEntries[iCurrentLogEntry].port = (WORD) port;   \
    aLogEntries[iCurrentLogEntry].val  = (WORD) val;    \
    iCurrentLogEntry = ++iCurrentLogEntry % MAX_LOG;


#endif
#endif


BYTE Rop2ToS3Rop[] = {
    LOGICAL_0,              /*  0      1 */
    NOT_SCREEN_AND_NOT_NEW, /* DPon    2 */
    SCREEN_AND_NOT_NEW,     /* DPna    3 */
    NOT_NEW,                /* Pn      4 */
    NOT_SCREEN_AND_NEW,     /* PDna    5 */
    NOT_SCREEN,             /* Dn      6 */
    SCREEN_XOR_NEW,         /* DPx     7 */
    NOT_SCREEN_OR_NOT_NEW,  /* DPan    8 */
    SCREEN_AND_NEW,         /* DPa     9 */
    NOT_SCREEN_XOR_NEW,     /* DPxn    10 */
    LEAVE_ALONE,            /* D       11 */
    SCREEN_OR_NOT_NEW,      /* DPno    12 */
    OVERPAINT,              /* P       13 */
    NOT_SCREEN_OR_NEW,      /* PDno    14 */
    SCREEN_OR_NEW,          /* DPo     15 */
    LOGICAL_1               /*  1      16 */
};

#if defined(i386)

/****************************************************************************\
 * vDataPortInB
\****************************************************************************/
VOID vDataPortInB(PPDEV ppdev, PBYTE pb, UINT count)
{
    ULONG pixtrans = (ULONG) (PIXEL_TRANSFER);

    _asm {
        cld

        mov ecx, count
        mov edi, pb
        mov edx, pixtrans

        rep insb
    }
}

/*****************************************************************************
 * vDataPortIn
 ****************************************************************************/
VOID vDataPortIn(PPDEV ppdev, PWORD pw, UINT count)
{
    ULONG pixtrans = (ULONG) (PIXEL_TRANSFER);


    _asm {
        cld

        mov ecx, count
        mov edi, pw
        mov edx, pixtrans

        rep insw
    }
}


/*****************************************************************************
 * vDataPortOutB
 ****************************************************************************/
VOID vDataPortOutB(PPDEV ppdev, PBYTE pb, UINT count)
{
    ULONG pixtrans = (ULONG) (PIXEL_TRANSFER);

    _asm {
        cld

        mov ecx, count
        mov esi, pb
        mov edx, pixtrans

        rep outsb
    }
}

/*****************************************************************************
 * vDataPortOut
 ****************************************************************************/
VOID vDataPortOut(PPDEV ppdev, PWORD pw, UINT count)
{
    ULONG pixtrans = (ULONG) (PIXEL_TRANSFER);

    _asm {
        cld

        mov ecx, count
        mov esi, pw
        mov edx, pixtrans

        rep outsw
    }
}

#else

/****************************************************************************\
 * vDataPortInB
\****************************************************************************/
VOID vDataPortInB(PPDEV ppdev, PBYTE pb, UINT count)
{
	while (count-- > 0) {
	    *((PUCHAR)pb)++ = READ_PORT_UCHAR (gpucCsrBase + PIXEL_TRANSFER);
	}
}

/*****************************************************************************
 * vDataPortIn
 ****************************************************************************/
VOID vDataPortIn(PPDEV ppdev, PWORD pw, UINT count)
{
	while (count-- > 0) {
	    *((USHORT UNALIGNED *)pw)++ = READ_PORT_USHORT ((PUSHORT)(gpucCsrBase + PIXEL_TRANSFER));
	}
}


/*****************************************************************************
 * vDataPortOutB
 ****************************************************************************/
VOID vDataPortOutB(PPDEV ppdev, PBYTE pb, UINT count)
{
	while (count-- > 0) {
	    WRITE_PORT_UCHAR (gpucCsrBase + PIXEL_TRANSFER, *((PUCHAR)pb)++);
	}
}

/*****************************************************************************
 * vDataPortOut
 ****************************************************************************/
VOID vDataPortOut(PPDEV ppdev, PWORD pw, UINT count)
{
	while (count-- > 0) {
	    WRITE_PORT_USHORT ((PUSHORT)(gpucCsrBase + PIXEL_TRANSFER), *((USHORT UNALIGNED *)pw)++);
	}

}

#endif //i386

/*****************************************************************************
 * bIntersectTest -
 ****************************************************************************/
BOOL bIntersectTest(PRECTL prcl1, PRECTL prcl2)
{

    if (    (prcl1->left > prcl2->right) ||
            (prcl1->right < prcl2->left) ||
            (prcl1->top > prcl2->bottom) ||
            (prcl1->bottom < prcl2->top) )
        return(FALSE);

    return(TRUE);
}


/*****************************************************************************
 * bTrivialAccept   - Test for a trivial accept rect 1 being inside or
 *                    coincident with rect 2.
 ****************************************************************************/
BOOL bTrivialAcceptTest(PRECTL prcl1, PRECTL prcl2)
{

    if (    (prcl1->left < prcl2->left)     ||
            (prcl1->right > prcl2->right)   ||
            (prcl1->top < prcl2->top)       ||
            (prcl1->bottom > prcl2->bottom) )
        return(FALSE);

    return(TRUE);
}


/*****************************************************************************
 * vResetS3Clipping
 *****************************************************************************/
VOID vResetS3Clipping(PPDEV ppdev)
{
    RECTL   rcl;

    rcl.top    = 0;
    rcl.left   = 0;
    rcl.right  = S3_MAX_RAM_WIDTH;
    rcl.bottom = S3_MAX_RAM_HEIGHT;

    vSetS3ClipRect(ppdev, &rcl);
}

/*****************************************************************************
 * vSetS3ClipRect
 *
 * Important note: GDI is inclusive/exclusive,
 *                 the chip is inclusive/inclusive,
 *                 and this routine does the mapping.
 *****************************************************************************/
VOID vSetS3ClipRect(PPDEV ppdev, PRECTL prclClip)
{
    ULONG   fl;
    WORD    clipcount;

    fl = 0;
    clipcount = 0;

    if (ppdev->ClipTop != prclClip->top) {
        fl |= TOP_CLIP;
        ppdev->ClipTop = prclClip->top;
        clipcount++;
    }

    if (ppdev->ClipLeft != prclClip->left) {
        fl |= LEFT_CLIP;
        ppdev->ClipLeft = prclClip->left;
        clipcount++;
    }

    if (ppdev->ClipRight != prclClip->right) {
        fl |= RIGHT_CLIP;
        ppdev->ClipRight = prclClip->right;
        clipcount++;
    }

    if (ppdev->ClipBottom != prclClip->bottom) {
        fl |= BOTTOM_CLIP;
        ppdev->ClipBottom = prclClip->bottom;
        clipcount++;
    }

    if (fl == 0)
        return;
#if 1

    if (ppdev->cxScreen == ppdev->cxMaxRam)
    {
        if (ppdev->ClipRight > ( (LONG)(ppdev->cxScreen)))
        {
            RIP("S3.DLL!vSetS3ClipRect - (ppdev->ClipRight > ppdev->cxScreen)\n");
        }
    }

    DISPDBG((1, "S3.DLL!vSetS3ClipRect - New Clipping %d, %d  %d, %d\n",
                 ppdev->ClipTop, ppdev->ClipLeft,
                 ppdev->ClipRight, ppdev->ClipBottom));

#endif

    //
    // Wait for the minium amount of queue entries needed
    //

    clipcount = (FIFO_1_EMPTY << 1) >> clipcount;

    FIFOWAIT(clipcount);

    if (fl & TOP_CLIP)
        OUTPW (MULTIFUNC_CNTL, (CLIP_TOP | ppdev->ClipTop));

    if (fl & LEFT_CLIP)
        OUTPW (MULTIFUNC_CNTL, (CLIP_LEFT | ppdev->ClipLeft));

    if (fl & RIGHT_CLIP)
        OUTPW (MULTIFUNC_CNTL, (CLIP_RIGHT | (ppdev->ClipRight - 1)));

    if (fl & BOTTOM_CLIP)
        OUTPW (MULTIFUNC_CNTL, (CLIP_BOTTOM | (ppdev->ClipBottom - 1)));
}


#if CATCHIT

VOID outpw_test(unsigned port , unsigned val)
{
    BYTE    byte;

    CHIP_STATE_TEST;

    LOGDBG((9, "Ow %lx: %lx\n", port, val));

    nOutpwAccesses++;

    outpw(port, val);
    return;
}


VOID outp_test(unsigned port, int val)
{
    BYTE    byte;

    CHIP_STATE_TEST;

    LOGDBG((9, "Ob %lx: %lx\n", port, val));

    nOutpAccesses++;

    outp(port, val);
    return;
}


BYTE inp_test(WORD port)
{
    BYTE    byte;

    CHIP_STATE_TEST;

    nInpAccesses++;

    return(inp(port));
}


WORD inpw_test(WORD port)
{
    BYTE    byte;

    CHIP_STATE_TEST;

    nInpwAccesses++;

    return(inpw(port));
}

VOID vCheckDataReady(PPDEV ppdev)
{
    ASSERTS3((INPW(GP_STAT) & HARDWARE_BUSY),
             "S3.DLL - S3 not ready for data transfer\n");
}

VOID vCheckDataComplete(PPDEV ppdev)
{
    LONG i;

// We loop because it may take a while for the hardware to finish
// digesting all the data we transferred:

    for (i = 1000; i > 0; i--)
    {
        if (!(INPW(GP_STAT) & HARDWARE_BUSY))
            return;
    }

    RIP("S3.DLL - S3 data transfer not complete\n");
}

#endif

#if LOG_OUTS

VOID outpw_log(unsigned port , unsigned val)
{
    BYTE    byte;

    LOG_OUTPUT;

    LOGDBG((9, "Ow %lx: %lx\n", port, val));

    return (outpw(port, val));
}


VOID outp_log(unsigned port, int val)
{
    BYTE    byte;

    LOG_OUTPUT;

    LOGDBG((9, "Ob %lx: %lx\n", port, val));

    return (outp(port, val));
}


#endif




#if !defined(_X86_) && !defined(i386)

/******************************************************************************
 * vPuntGetBits - Get the bits from the device surface onto the "punt" bitmap.
 *****************************************************************************/
VOID vPuntGetBits(PPDEV ppdev, SURFOBJ *psoTrg, RECTL *prclTrg)
{
UINT    i, j ;

LONG    lDestDelta,
        xTrg, yTrg,
        cxTrg, cyTrg ;

PBYTE   pbScan0,
        pbDestRect ;

PWORD   pw ;

WORD    s3Cmd ;

RECTL   rclClip ;


        // Default the clipping to the entire screen to get the data.
        // we can over write portions of the host dest bitmap
        // because we never display it, we only use this dest bitmap
        // as work surface for the engine.

        rclClip.left   = 0 ;
        rclClip.top    = 0 ;
        rclClip.right  = S3BM_WIDTH;
        rclClip.bottom = S3BM_HEIGHT;

        vSetS3ClipRect(ppdev, &rclClip) ;

        // Calculate the size of the target rectangle, and pick up
        // some convienent locals.

        pbScan0 = (PBYTE) ppdev->psoTemp->pvScan0 ;

        // The source rectangle passed into DrvCopyBits/DrvBitBlt might
        // not be clipped to the visible surface (because the clip object
        // would take care of that), so we have to be careful that we don't
        // overwrite anything outside the visible surface:

        xTrg = max(prclTrg->left, 0) ;
        yTrg = max(prclTrg->top, 0) ;

        cxTrg = min(prclTrg->right, (LONG) ppdev->cxScreen) - xTrg ;
        cyTrg = min(prclTrg->bottom, (LONG) ppdev->cyScreen) - yTrg ;

        lDestDelta = ppdev->psoTemp->lDelta ;

        // Copy the target rectangle from the real screen to the
        // bitmap we are telling the engine is the screen.

        // Calculate the location of the dest rect.

        pbDestRect = pbScan0 + (yTrg * lDestDelta) + xTrg ;

        // Set the S3 chip up for the copy.

        s3Cmd = RECTANGLE_FILL     | BYTE_SWAP      | BUS_SIZE_16     |
                DRAWING_DIR_TBLRXM | DIR_TYPE_XY    | WAIT |
                DRAW               | LAST_PIXEL_ON  | READ ;

        FIFOWAIT(FIFO_6_EMPTY) ;

        outpw (MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES)) ;
        outpw (CUR_X, xTrg) ;
        outpw (CUR_Y, yTrg) ;
        outpw (RECT_WIDTH, cxTrg - 1) ;
        outpw (MULTIFUNC_CNTL, (RECT_HEIGHT | (cyTrg - 1))) ;
        outpw (CMD, s3Cmd) ;

        // Wait for the Data Available.

        while (!(inpw(GP_STAT) & READ_DATA_AVAILABLE)) ;

        // Now transfer the data from the screen to the host memory bitmap.

        pw = (PWORD) pbDestRect ;
        j = (cxTrg + 1) / 2 ;

        for (i = 0 ; i < (UINT) cyTrg ; i++)
        {
            vDataPortIn(ppdev, pw, j) ;
            ((PBYTE) pw) += lDestDelta ;
        }

}

/******************************************************************************
 * vPuntPutBits - Put the bits from  the "punt" bitmap to the device surface.
 *****************************************************************************/
VOID vPuntPutBits(PPDEV ppdev, SURFOBJ *psoTrg, RECTL *prclTrg)
{
UINT    i, j ;

LONG    lDestDelta,
        xTrg, yTrg,
        cxTrg, cyTrg ;

PBYTE   pbScan0,
        pbDestRect ;

PWORD   pw ;

WORD    s3Cmd ;

        // Set the clipping to exactly what we need on the destination.

        vSetS3ClipRect(ppdev, prclTrg) ;

        // Recalculate the target position(s) and extent(s)

        // The target rectangle passed into DrvCopyBits/DrvBitBlt might
        // not be clipped to the visible surface (because the clip object
        // would take care of that), so we have to be careful that we don't
        // overwrite anything outside the visible surface:

        xTrg = max(prclTrg->left, 0) ;
        yTrg = max(prclTrg->top, 0) ;

        cxTrg = min(prclTrg->right, (LONG) ppdev->cxScreen) - xTrg ;
        cyTrg = min(prclTrg->bottom, (LONG) ppdev->cyScreen) - yTrg ;

        pbScan0    = (PBYTE) (ppdev->psoTemp->pvScan0) ;
        lDestDelta = ppdev->psoTemp->lDelta ;
        pbDestRect = pbScan0 + (yTrg * lDestDelta) + xTrg ;

        // Put the bits back on the screen.

        s3Cmd = RECTANGLE_FILL | BUS_SIZE_16        | BYTE_SWAP   | WAIT |
                DRAW           | DRAWING_DIR_TBLRXM | DIR_TYPE_XY |
                LAST_PIXEL_ON  | SINGLE_PIXEL       | WRITE ;

        FIFOWAIT(FIFO_7_EMPTY) ;

        outpw(FRGD_MIX, (SRC_CPU_DATA | OVERPAINT)) ;
        outpw(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES)) ;
        outpw (CUR_X, xTrg) ;
        outpw (CUR_Y, yTrg) ;
        outpw (RECT_WIDTH, cxTrg - 1) ;
        outpw (MULTIFUNC_CNTL, (RECT_HEIGHT | (cyTrg - 1))) ;
        outpw(CMD, s3Cmd) ;

        // Now transfer the data, from the host memory bitmap to the screen.

        pw = (PWORD) pbDestRect ;
        j = (cxTrg + 1) / 2 ;

        for (i = 0 ; i < (UINT) cyTrg ; i++)
        {
            vDataPortOut(ppdev, pw, j) ;
            ((PBYTE) pw) += lDestDelta ;
        }

        return ;
}

#endif // !defined(_X86_) && !defined(i386)

