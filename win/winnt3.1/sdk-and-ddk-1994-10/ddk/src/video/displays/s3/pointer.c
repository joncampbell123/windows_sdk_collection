/******************************Module*Header*******************************\
* Module Name: pointer.c
*
* This module contains the hardware cursor support for Disp.
*
* Copyright (c) 1992 Microsoft Corporation
\**************************************************************************/

#include "driver.h"


ULONG DrvSetColorPointerShape(
    SURFOBJ     *pso,
    SURFOBJ     *psoMask,
    SURFOBJ     *psoColor,
    XLATEOBJ    *pxlo,
    LONG        xHot,
    LONG        yHot,
    LONG        x,
    LONG        y,
    RECTL       *prcl,
    FLONG       fl
);

ULONG DrvSetMonoHwPointerShape(
    SURFOBJ     *pso,
    SURFOBJ     *psoMask,
    SURFOBJ     *psoColor,
    XLATEOBJ    *pxlo,
    LONG        xHot,
    LONG        yHot,
    LONG        x,
    LONG        y,
    RECTL       *prcl,
    FLONG       fl
);

ULONG DrvSetBt485PointerShape(
    SURFOBJ     *pso,
    SURFOBJ     *psoMask,
    SURFOBJ     *psoColor,
    XLATEOBJ    *pxlo,
    LONG        xHot,
    LONG        yHot,
    LONG        x,
    LONG        y,
    RECTL       *prcl,
    FLONG       fl
);



VOID DrvMoveColorPointer(SURFOBJ *pso,LONG x,LONG y,RECTL *prcl);
VOID DrvMoveHwPointer(SURFOBJ *pso,LONG x,LONG y,RECTL *prcl);
VOID DrvMoveBt485Pointer(SURFOBJ *pso, LONG x, LONG y, RECTL *prcl);



/*****************************************************************************
 * DrvMovePointer -
 ****************************************************************************/
VOID DrvMovePointer(
    SURFOBJ *pso,
    LONG x,
    LONG y,
    RECTL *prcl)
{
    PPDEV   ppdev;

    ppdev = (PPDEV) pso->dhpdev;

    if (ppdev->flPointer & COLOR_POINTER)
        DrvMoveColorPointer(pso, x, y, prcl);
    else
    {
        if (ppdev->bBt485Dac == TRUE)
        {
            DrvMoveBt485Pointer(pso, x, y, prcl);
        }
        else
        {
            DrvMoveHwPointer(pso, x, y, prcl);
        }
    }
}


/*****************************************************************************
 * DrvMoveColorPointer -
 ****************************************************************************/
VOID DrvMoveColorPointer(
    SURFOBJ *pso,
    LONG x,
    LONG y,
    RECTL *prcl)
{
    INT     xDest, yDest;
    WORD    cmd;
    RECTL   rclClip;
    PPDEV   ppdev;

    ppdev = (PPDEV) pso->dhpdev;

    // Sync with the rest of the driver.

    GPWAIT();

    // If x is -1 then take down the cursor.

    if (x == -1)
    {
        ppdev->flPointer |= TAKE_DOWN_POINTER;
    }

    // Adjust the actual position on the screen for the hot spot.

    xDest = x - ppdev->ptlHotSpot.x;
    yDest = y - ppdev->ptlHotSpot.y;

    // If the save buffer has valid data, copy whats in the save
    // buffer back to the screen.

    if (((ppdev->flPointer & VALID_SAVE_BUFFER) &&
        (!(ppdev->flPointer & ANIMATEUPDATE)))  ||
        ((ppdev->flPointer & VALID_SAVE_BUFFER) &&
         (ppdev->flPointer & ANIMATEUPDATE)     &&
         ((ppdev->ptlLastPosition.x != xDest)   ||
          (ppdev->ptlLastPosition.y != yDest))))
    {
        // The restore will always take place to the screen,
        // so limit the clip area to the screen.

        rclClip.left   = 0;
        rclClip.top    = 0;
        rclClip.right  = ppdev->cxScreen;
        rclClip.bottom = ppdev->cyScreen;

        vSetS3ClipRect(ppdev, &rclClip);

        cmd  = BITBLT             | DRAW | DIR_TYPE_XY |
               DRAWING_DIR_TBLRXM | WRITE;

        FIFOWAIT(FIFO_2_EMPTY);

        TEST_AND_SET_FRGD_MIX(SRC_DISPLAY_MEMORY | OVERPAINT);
        OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));

        FIFOWAIT(FIFO_7_EMPTY);

        OUTPW(RECT_WIDTH, ppdev->szlPointer.cx - 1);
        OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | ppdev->szlPointer.cy - 1));
        OUTPW(CUR_X, COLOR_POINTER_SAVE_X);
        OUTPW(CUR_Y, COLOR_POINTER_Y);
        OUTPW(DEST_X, ppdev->ptlLastPosition.x);
        OUTPW(DEST_Y, ppdev->ptlLastPosition.y);

        OUTPW(CMD, cmd);

        // Indicate that the save buffer is no longer valid.

        ppdev->flPointer &= ~VALID_SAVE_BUFFER;

    }

    // If we're just taking down the pointer then were done,
    // so this an early exit.

    if (ppdev->flPointer & TAKE_DOWN_POINTER )
    {
        ppdev->flPointer &= ~TAKE_DOWN_POINTER;
        vResetS3Clipping(ppdev);
        return;
    }


    // Set the clipping for the pointers masks, save, and work area.

    rclClip.left   = 0;
    rclClip.top    = COLOR_POINTER_Y;
    rclClip.right  = COLOR_POINTER_SAVE_X + COLOR_POINTER_CX;
    rclClip.bottom = (COLOR_POINTER_Y + COLOR_POINTER_CY);

    vSetS3ClipRect(ppdev, &rclClip);

    if (!(ppdev->flPointer & ANIMATEUPDATE) ||
        !(ppdev->flPointer & VALID_SAVE_BUFFER))
    {
        // Save the area under where we plan to draw the new cursor.

        cmd  = BITBLT             | DRAW | DIR_TYPE_XY |
               DRAWING_DIR_TBLRXM | WRITE;

        FIFOWAIT(FIFO_2_EMPTY);

        TEST_AND_SET_FRGD_MIX(SRC_DISPLAY_MEMORY | OVERPAINT);
        OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));

        FIFOWAIT(FIFO_7_EMPTY);

        OUTPW(RECT_WIDTH, ppdev->szlPointer.cx - 1);
        OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | ppdev->szlPointer.cy - 1));
        OUTPW(CUR_X, xDest);
        OUTPW(CUR_Y, yDest);
        OUTPW(DEST_X,COLOR_POINTER_SAVE_X);
        OUTPW(DEST_Y,COLOR_POINTER_Y);

        OUTPW(CMD, cmd);
    }

    // Validate the save buffer.

    ppdev->flPointer |= VALID_SAVE_BUFFER;

    // Now copy the saved data to the work buffer.

    cmd  = BITBLT             | DRAW | DIR_TYPE_XY |
           DRAWING_DIR_TBLRXM | WRITE;

    FIFOWAIT(FIFO_2_EMPTY);

    TEST_AND_SET_FRGD_MIX(SRC_DISPLAY_MEMORY | OVERPAINT);
    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));

    FIFOWAIT(FIFO_7_EMPTY);

    OUTPW(RECT_WIDTH, ppdev->szlPointer.cx - 1);
    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | ppdev->szlPointer.cy - 1));
    OUTPW(CUR_X, COLOR_POINTER_SAVE_X);
    OUTPW(CUR_Y, COLOR_POINTER_Y);
    OUTPW(DEST_X,COLOR_POINTER_WORK_X);
    OUTPW(DEST_Y,COLOR_POINTER_Y);

    OUTPW(CMD, cmd);

    // Record the current position as the last position;

    ppdev->ptlLastPosition.x = xDest;
    ppdev->ptlLastPosition.y = yDest;

    // AND in the AND mask to the work buffer

    cmd  = BITBLT             | DRAW                | DIR_TYPE_XY |
           DRAWING_DIR_TBLRXM | MULTIPLE_PIXELS     | WRITE;

    FIFOWAIT(FIFO_6_EMPTY);

    TEST_AND_SET_FRGD_MIX(FOREGROUND_COLOR | SCREEN_AND_NEW);
    TEST_AND_SET_FRGD_COLOR(0xff);
    TEST_AND_SET_BKGD_MIX(BACKGROUND_COLOR | SCREEN_AND_NEW);
    SET_BKGD_COLOR(0x00);
    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | DISPLAY_MEMORY));
    TEST_AND_SET_RD_MASK(COLOR_POINTER_AND_PLANE);


    FIFOWAIT(FIFO_7_EMPTY);

    OUTPW(RECT_WIDTH, ppdev->szlPointer.cx - 1);
    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | ppdev->szlPointer.cy - 1));
    OUTPW(CUR_X, 0);
    OUTPW(CUR_Y, COLOR_POINTER_Y);
    OUTPW(DEST_X,COLOR_POINTER_WORK_X);
    OUTPW(DEST_Y,COLOR_POINTER_Y);
    OUTPW(CMD, cmd);

    FIFOWAIT(FIFO_1_EMPTY);

    TEST_AND_SET_RD_MASK(0xff);

    // Or in the Color data to the work buffer.

    cmd  = BITBLT             | DRAW         | DIR_TYPE_XY |
           DRAWING_DIR_TBLRXM | SINGLE_PIXEL | WRITE;

    FIFOWAIT(FIFO_3_EMPTY);

    TEST_AND_SET_FRGD_MIX(SRC_DISPLAY_MEMORY | SCREEN_OR_NEW);
    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));

    FIFOWAIT(FIFO_7_EMPTY);

    OUTPW(RECT_WIDTH, ppdev->szlPointer.cx - 1);
    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | ppdev->szlPointer.cy - 1));
    OUTPW(CUR_X, COLOR_POINTER_COLOR_DATA_X);
    OUTPW(CUR_Y, COLOR_POINTER_Y);
    OUTPW(DEST_X,COLOR_POINTER_WORK_X);
    OUTPW(DEST_Y,COLOR_POINTER_Y);
    OUTPW(CMD, cmd);

    // Now copy the work buffer to the screen.

    rclClip.left   = 0;
    rclClip.top    = 0;
    rclClip.right  = ppdev->cxScreen;
    rclClip.bottom = ppdev->cyScreen;

    vSetS3ClipRect(ppdev, &rclClip);

    cmd  = BITBLT             | DRAW | DIR_TYPE_XY |
           DRAWING_DIR_TBLRXM | WRITE;

    FIFOWAIT(FIFO_2_EMPTY);

    TEST_AND_SET_FRGD_MIX(SRC_DISPLAY_MEMORY | OVERPAINT);
    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));

    FIFOWAIT(FIFO_7_EMPTY);

    OUTPW(RECT_WIDTH, ppdev->szlPointer.cx - 1);
    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | ppdev->szlPointer.cy - 1));
    OUTPW(CUR_X, COLOR_POINTER_WORK_X);
    OUTPW(CUR_Y, COLOR_POINTER_Y);
    OUTPW(DEST_X, xDest);
    OUTPW(DEST_Y, yDest);
    OUTPW(CMD, cmd);

    FIFOWAIT(FIFO_1_EMPTY);

    TEST_AND_SET_RD_MASK(0xff);

    // If the GDI requests the bounding box of the pointer,
    // return it.

    if (prcl != NULL)
    {
        prcl->left   = xDest;
        prcl->top    = yDest;
        prcl->right  = xDest + ppdev->szlPointer.cx;
        prcl->bottom = yDest + ppdev->szlPointer.cy;

    }

    // Reset the clipping.  This can go when no H/W clipping is done
    // on the color pointer.

    vResetS3Clipping(ppdev);
}

/*****************************************************************************
 * DrvMoveHwPointer -
 ****************************************************************************/
VOID DrvMoveHwPointer(
    SURFOBJ *pso,
    LONG x,
    LONG y,
    RECTL *prcl)
{
    WORD    msb, lsb;

    PPDEV   ppdev;

    ppdev = (PPDEV) pso->dhpdev;

    // Save the CRTC Index.

    ppdev->CrtcIndex = INP(CRTC_INDEX);

    // If x is -1 then take down the cursor.

    if (x == -1)
    {
        OUTPW (CRTC_INDEX, (ppdev->HgcMode & ~(HGC_ENABLE << 8)));
    }

    // Adjust the actual pointer position depending upon
    // the hot spot.

    x -= ppdev->ptlHotSpot.x;
    y -= ppdev->ptlHotSpot.y;

    // Record the current position as the last position;

    ppdev->ptlLastPosition.x = x;
    ppdev->ptlLastPosition.y = y;

    if (x <= 0)
    {
        OUTPW (CRTC_INDEX, ((-x << 8) | HGC_DX));
        x = 0;
    }
    else
    {
        OUTPW (CRTC_INDEX, ((0 << 8) | HGC_DX));
    }

    if (y <= 0)
    {
        OUTPW (CRTC_INDEX, ((-y << 8) | HGC_DY));
        y = 0;
    }
    else
    {
        OUTPW (CRTC_INDEX, ((0 << 8) | HGC_DY));
    }

    // Set the position of the cursor.

    msb = HIBYTE (x);
    lsb = LOBYTE (x);
    OUTPW (CRTC_INDEX, ((lsb << 8) | HGC_ORGX_LSB));
    OUTPW (CRTC_INDEX, ((msb << 8) | HGC_ORGX_MSB));

    msb = HIBYTE (y);
    lsb = LOBYTE (y);
    OUTPW (CRTC_INDEX, ((lsb << 8) | HGC_ORGY_LSB));
    OUTPW (CRTC_INDEX, ((msb << 8) | HGC_ORGY_MSB));

    // Restore the CRTC index

    OUTP(CRTC_INDEX, ppdev->CrtcIndex);

}

/*****************************************************************************
 * DrvMoveBt485Pointer -
 ****************************************************************************/
VOID DrvMoveBt485Pointer(
    SURFOBJ *pso,
    LONG x,
    LONG y,
    RECTL *prcl)
{
    WORD    msb, lsb;

    PPDEV   ppdev;

    ppdev = (PPDEV) pso->dhpdev;

    // Save the CRTC Index.

    ppdev->CrtcIndex = INP(CRTC_INDEX);

    // If x is -1 then take down the cursor.

    if (x == -1)
    {
        OUTPW (CRTC_INDEX, (ppdev->ExtDacCtl | 0x0200));
        OUTP  (BT485_ADDR_CMD_REG2, ppdev->Bt485CmdReg2 & BT485_CURSOR_DISABLE);
        OUTPW (CRTC_INDEX, (ppdev->ExtDacCtl));

        return;
    }

    // Adjust the actual pointer position depending upon
    // the hot spot.

    x -= ppdev->ptlHotSpot.x;
    y -= ppdev->ptlHotSpot.y;

    // Record the current position as the last position;

    ppdev->ptlLastPosition.x = x;
    ppdev->ptlLastPosition.y = y;

    // Adjust for the placement in the Bt485

    x += 64;
    y += 64;

    // Set the position of the cursor.

    OUTPW (CRTC_INDEX, (ppdev->ExtDacCtl | 0x0300));

    msb = HIBYTE (x);
    lsb = LOBYTE (x);

    OUTP(BT485_CURSOR_X_LOW,  lsb);
    OUTP(BT485_CURSOR_X_HIGH, msb);

    msb = HIBYTE (y);
    lsb = LOBYTE (y);

    OUTP(BT485_CURSOR_Y_LOW,  lsb);
    OUTP(BT485_CURSOR_Y_HIGH, msb);

    OUTPW (CRTC_INDEX, (ppdev->ExtDacCtl));

    // Restore the CRTC index

    OUTP(CRTC_INDEX, ppdev->CrtcIndex);

}



/*****************************************************************************
 * DrvSetPointerShape -
 ****************************************************************************/
ULONG DrvSetPointerShape(
    SURFOBJ     *pso,
    SURFOBJ     *psoMask,
    SURFOBJ     *psoColor,
    XLATEOBJ    *pxlo,
    LONG        xHot,
    LONG        yHot,
    LONG        x,
    LONG        y,
    RECTL       *prcl,
    FLONG       fl)
{
    ULONG   ulRet;
    PPDEV   ppdev;
    BOOL    bResetAnimateFlag;

    ppdev = (PPDEV) pso->dhpdev;

    // Save the hot spot in the pdev.

    ppdev->ptlHotSpot.x = xHot;
    ppdev->ptlHotSpot.y = yHot;

    ppdev->szlPointer.cx = psoMask->sizlBitmap.cx;
    ppdev->szlPointer.cy = psoMask->sizlBitmap.cy / 2;

    // The pointer may be larger than we can handle.
    // If it is we must cleanup the screen and let the engine
    // take care of it.

    if (psoMask->sizlBitmap.cx > 64 || psoMask->sizlBitmap.cy > 64)
    {
        // If it's a color pointer take it down.

        if (   (ppdev->flPointer & COLOR_POINTER)
            && (ppdev->flPointer & VALID_SAVE_BUFFER)
           )
        {
            ulRet = DrvSetColorPointerShape(pso, NULL, NULL, NULL,
                                            0, 0, 0, 0, NULL, 0);
        }

        // Disable the mono hardware pointer.

        if (ppdev->bBt485Dac == TRUE)
        {
            // Disable the H/W cursor on the Bt 485.

            OUTPW (CRTC_INDEX, (ppdev->ExtDacCtl | 0x0200));
            OUTP  (BT485_ADDR_CMD_REG2, ppdev->Bt485CmdReg2 & BT485_CURSOR_DISABLE);
            OUTPW (CRTC_INDEX, (ppdev->ExtDacCtl));

        }
        else
        {
            OUTPW (CRTC_INDEX, (ppdev->HgcMode & ~(HGC_ENABLE << 8)));
        }

        // reset our local pointer flags.

        ppdev->flPointer = 0;

        return (SPS_DECLINE);

    }

    // Set the AnimateUpdate flag.

    if ((fl & SPS_ANIMATEUPDATE) &&
        ((x - xHot) == ppdev->ptlLastPosition.x) &&
        ((y - yHot) == ppdev->ptlLastPosition.y))
    {
        ppdev->flPointer |= ANIMATEUPDATE;
    }
    else
    {
        ppdev->flPointer &= ~ANIMATEUPDATE;
    }

    if (psoColor != NULL)
    {
        // Disable the mono hardware pointer.

        if (ppdev->bBt485Dac == TRUE)
        {
            // Disable the H/W cursor on the Bt 485.

            OUTPW (CRTC_INDEX, (ppdev->ExtDacCtl | 0x0200));
            OUTP  (BT485_ADDR_CMD_REG2, ppdev->Bt485CmdReg2 & BT485_CURSOR_DISABLE);
            OUTPW (CRTC_INDEX, (ppdev->ExtDacCtl));

        }
        else
        {
            OUTPW (CRTC_INDEX, (ppdev->HgcMode & ~(HGC_ENABLE << 8)));
        }

        ppdev->flPointer |= COLOR_POINTER;
        ulRet = DrvSetColorPointerShape(pso, psoMask, psoColor, pxlo,
                                        xHot, yHot, x, y, prcl, fl);

    }
    else
    {
        // Take down the color pointer if it is visible.

        if (   (ppdev->flPointer & COLOR_POINTER)
            && (ppdev->flPointer & VALID_SAVE_BUFFER)
           )
        {
            // If we are making a transition from a color to a monochrome pointer
            // and we haven't moved the pointer, and we're still in Animate Update mode,
            // then we have to turn off the Animate Update flag for this call to turn off
            // the color pointer.

            if (ppdev->flPointer & ANIMATEUPDATE)
            {
                bResetAnimateFlag = TRUE;
                ppdev->flPointer &= ~ANIMATEUPDATE;
            }
            else
            {
                bResetAnimateFlag = FALSE;
            }

            ulRet = DrvSetColorPointerShape(pso, NULL, NULL, NULL,
                                            0, 0, 0, 0, NULL, 0);

            if (bResetAnimateFlag == TRUE)
            {
                ppdev->flPointer |= ANIMATEUPDATE;
            }
        }

        // Take care of the monochrome pointer.

        ppdev->flPointer &= ~COLOR_POINTER;

        if (ppdev->bBt485Dac == TRUE)
        {
            ulRet = DrvSetBt485PointerShape(pso, psoMask, psoColor, pxlo,
                                            xHot, yHot, x, y, prcl, fl);
        }
        else
        {
            ulRet = DrvSetMonoHwPointerShape(pso, psoMask, psoColor, pxlo,
                                             xHot, yHot, x, y, prcl, fl);
        }
    }

    // Now that we have done an animation update, revert to a normal
    // pointer.

    if (ppdev->flPointer & ANIMATEUPDATE)
    {
        ppdev->flPointer &= ~ANIMATEUPDATE;
    }

    return (ulRet);
}


/*****************************************************************************
 * DrvSetColorPointerShape -
 ****************************************************************************/
ULONG DrvSetColorPointerShape(
    SURFOBJ     *pso,
    SURFOBJ     *psoMask,
    SURFOBJ     *psoColor,
    XLATEOBJ    *pxlo,
    LONG        xHot,
    LONG        yHot,
    LONG        x,
    LONG        y,
    RECTL       *prcl,
    FLONG       fl)
{
    UINT    i, j, k;
    PBYTE   pb;
    WORD    Cmd;
    UINT    cxMask, cyMask, cyAND, cyXOR, cxANDBytes, cxXORBytes;
    UINT    cxColor, cyColor;
    PBYTE   pjAND, pjXOR, pjColor;
    LONG    lDelta, lColorDelta;
    PBYTE   pb4Bpp;
    UINT    nSrcBytes;
    PULONG  pulXlate;
    RECTL   rclClip;
    PPDEV   ppdev;
    BYTE    LineBuff8Bpp[64];

    DISPDBG((3, "S3.DLL:DrvSetColorPointerShape  - Entry\n"));

    ppdev = (PPDEV) pso->dhpdev;

    // Sync this operation with the rest of the driver.

    GPWAIT();

    // Remove the current pointer, if it is on the screen.

    if ((ppdev->flPointer & VALID_SAVE_BUFFER) && (!(ppdev->flPointer & ANIMATEUPDATE)))
    {
        DrvMovePointer(pso, -1, -1, NULL);
    }

    // If the pointer is completely transparent, then just return.

    if (psoMask == NULL)
    {
        return (SPS_ACCEPT_EXCLUDE);
    }

    // If the GDI requests the bounding box of the pointer,
    // return it.

    if (prcl != NULL)
    {
        cxMask = psoMask->sizlBitmap.cx;
        cyMask = psoMask->sizlBitmap.cy;

        prcl->left   = x - xHot;
        prcl->top    = y - yHot;
        prcl->right  = prcl->left + cxMask;
        prcl->bottom = prcl->top  + cyMask;

    }

    // Set the clipping.

    rclClip.left   = 0;
    rclClip.top    = COLOR_POINTER_Y;
    rclClip.right  = COLOR_POINTER_CX * 3;
    rclClip.bottom = COLOR_POINTER_Y + COLOR_POINTER_CY;

    vSetS3ClipRect(ppdev, &rclClip);

    // Get the bitmap dimensions.

    cxMask = psoMask->sizlBitmap.cx;
    cyMask = psoMask->sizlBitmap.cy;

    cxColor = psoColor->sizlBitmap.cx;
    cyColor = psoColor->sizlBitmap.cy;

    cyAND = cyXOR = cyMask / 2;
    cxANDBytes = cxXORBytes = cxMask / 8;

    // Set up pointers to the AND and XOR masks.

    pjAND  =  psoMask->pvScan0;
    lDelta = psoMask->lDelta;
    pjXOR  = pjAND + (cyAND * lDelta);

    pjColor = psoColor->pvScan0;
    lColorDelta = psoColor->lDelta;

    // Zero out coprocessor memory for the masks, color data,
    // and the work area.  Be carefull not to wipe out the save area.

    Cmd = RECTANGLE_FILL |
          DRAW           | DRAWING_DIR_TBLRXM | DIR_TYPE_XY |
          LAST_PIXEL_ON  | SINGLE_PIXEL       | WRITE;

    FIFOWAIT(FIFO_3_EMPTY);

    TEST_AND_SET_FRGD_MIX(LOGICAL_0);
    TEST_AND_SET_WRT_MASK(0xff);
    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));

    FIFOWAIT(FIFO_5_EMPTY);

    OUTPW(CUR_X, 0);
    OUTPW(CUR_Y, COLOR_POINTER_Y);
    OUTPW(RECT_WIDTH, (COLOR_POINTER_CX * 2) - 1);
    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | COLOR_POINTER_CY - 1));
    OUTPW(CMD, Cmd);

    // Copy the AND mask to the off screen memory.

    Cmd = RECTANGLE_FILL | BUS_SIZE_8         | WAIT |
          DRAW           | DRAWING_DIR_TBLRXM | DIR_TYPE_XY |
          LAST_PIXEL_ON  | MULTIPLE_PIXELS    | WRITE;


    FIFOWAIT(FIFO_6_EMPTY);

    TEST_AND_SET_FRGD_MIX(FOREGROUND_COLOR | OVERPAINT);
    TEST_AND_SET_FRGD_COLOR(0xff);
    TEST_AND_SET_BKGD_MIX(BACKGROUND_COLOR | OVERPAINT);
    SET_BKGD_COLOR(0x00);
    TEST_AND_SET_WRT_MASK(COLOR_POINTER_AND_PLANE);
    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | CPU_DATA));

    FIFOWAIT(FIFO_5_EMPTY);

    OUTPW(CUR_X, 0);
    OUTPW(CUR_Y, COLOR_POINTER_Y);
    OUTPW(RECT_WIDTH, cxMask - 1);
    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | cyAND - 1));

    GPWAIT();

    OUTPW(CMD, Cmd);

    CHECK_DATA_READY;

    // Now transfer the AND mask data.

    FIFOWAIT(FIFO_8_EMPTY);

    pb = pjAND;
    for (i = 0; i < cyAND; i++)
    {
        vDataPortOutB(ppdev,(PBYTE) pb, cxANDBytes);
        pb += lDelta;
    }

    CHECK_DATA_COMPLETE;


    // Copy the XOR mask to the off screen memory.

    Cmd = RECTANGLE_FILL | BUS_SIZE_8         | WAIT |
          DRAW           | DRAWING_DIR_TBLRXM | DIR_TYPE_XY |
          LAST_PIXEL_ON  | MULTIPLE_PIXELS    | WRITE;

    FIFOWAIT(FIFO_6_EMPTY);

    TEST_AND_SET_FRGD_MIX(FOREGROUND_COLOR | OVERPAINT);
    TEST_AND_SET_FRGD_COLOR(0xff);
    TEST_AND_SET_BKGD_MIX(BACKGROUND_COLOR | OVERPAINT);
    SET_BKGD_COLOR(0x00);
    TEST_AND_SET_WRT_MASK(COLOR_POINTER_XOR_PLANE);
    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | CPU_DATA));

    FIFOWAIT(FIFO_5_EMPTY);

    OUTPW(CUR_X, 0);
    OUTPW(CUR_Y, COLOR_POINTER_Y);
    OUTPW(RECT_WIDTH, cxMask - 1);
    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | cyXOR - 1));

    GPWAIT();

    OUTPW(CMD, Cmd);

    // Now transfer the XOR mask data.

    FIFOWAIT(FIFO_8_EMPTY);

    pb = pjXOR;
    for (i = 0; i < cyXOR; i++)
    {
        vDataPortOutB(ppdev,(PBYTE) pb, cxXORBytes);
        pb += lDelta;
    }

    FIFOWAIT(FIFO_1_EMPTY);

    TEST_AND_SET_WRT_MASK(0xff);

    if (psoColor->iBitmapFormat == BMF_8BPP)
    {
        // Copy the Color mask to the off screen memory.

        Cmd = RECTANGLE_FILL | BUS_SIZE_8         | WAIT |
              DRAW           | DRAWING_DIR_TBLRXM | DIR_TYPE_XY |
              LAST_PIXEL_ON  | SINGLE_PIXEL       | WRITE;

        FIFOWAIT(FIFO_7_EMPTY);

        TEST_AND_SET_FRGD_MIX(SRC_CPU_DATA | OVERPAINT);
        OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));
        OUTPW(CUR_X, COLOR_POINTER_CX);
        OUTPW(CUR_Y, COLOR_POINTER_Y);
        OUTPW(RECT_WIDTH, cxColor - 1);
        OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | cyColor - 1));

        GPWAIT();

        OUTPW(CMD, Cmd);

        CHECK_DATA_READY;

        // Now transfer the data.

        // Note: It would nice to do the entire bitmap in one
        //       fell swoop, but there is no gaurantee source will
        //       wrap on a bitmap boundary.

        FIFOWAIT(FIFO_8_EMPTY);

        pb = pjColor;
        for (i = 0; i < cyColor; i++)
        {
            vDataPortOutB(ppdev,pb, cxColor);
            pb += lColorDelta;
        }

        CHECK_DATA_COMPLETE;
    }
    else
    {
        Cmd = RECTANGLE_FILL | BUS_SIZE_8         | WAIT |
              DRAW           | DRAWING_DIR_TBLRXM | DIR_TYPE_XY |
              LAST_PIXEL_ON  | SINGLE_PIXEL       | WRITE;

        FIFOWAIT(FIFO_7_EMPTY);

        TEST_AND_SET_FRGD_MIX(SRC_CPU_DATA | OVERPAINT);
        OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));
        OUTPW(CUR_X, COLOR_POINTER_CX);
        OUTPW(CUR_Y, COLOR_POINTER_Y);
        OUTPW(RECT_WIDTH, cxColor - 1);
        OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | cyColor - 1));

        GPWAIT();

        OUTPW(CMD, Cmd);

        CHECK_DATA_READY;

        // Now transfer the data.

        // Note: It would nice to do the entire bitmap in one
        //       fell swoop, but there is no gaurantee source will
        //       wrap on a bitmap boundary.

        pb4Bpp = pjColor;
        nSrcBytes = (cxColor + 1) / 2;

        if (pxlo->flXlate & XO_TABLE)
        {
            pulXlate = pxlo->pulXlate;
        }
        else
        {
            pulXlate = XLATEOBJ_piVector(pxlo);
        }

        FIFOWAIT(FIFO_8_EMPTY);

        for (i = 0; i < cyColor; i++)
        {
            for (k = 0, j = 0; j < nSrcBytes; j++)
            {
                LineBuff8Bpp[k++] = (BYTE) pulXlate[(pb4Bpp[j] & 0xF0) >> 4];
                LineBuff8Bpp[k++] = (BYTE) pulXlate[pb4Bpp[j] & 0x0F];
            }

            vDataPortOutB(ppdev,LineBuff8Bpp, cxColor);
            pb4Bpp += lColorDelta;
        }

        CHECK_DATA_COMPLETE;
    }

    // Set the position of the cursor.

    DrvMovePointer(pso, x, y, NULL);

    return (SPS_ACCEPT_EXCLUDE);
}

/*****************************************************************************
 * DrvSetMonoHwPointerShape -
 ****************************************************************************/
ULONG DrvSetMonoHwPointerShape(
    SURFOBJ     *pso,
    SURFOBJ     *psoMask,
    SURFOBJ     *psoColor,
    XLATEOBJ    *pxlo,
    LONG        xHot,
    LONG        yHot,
    LONG        x,
    LONG        y,
    RECTL       *prcl,
    FLONG       fl)
{
    UINT    i, j, k, cxMask, cyMask, cyAND, cxAND, cyXOR, cxXOR;
    PBYTE   pjAND, pjXOR;
    PWORD   pwS3AndMask, pwS3XorMask, pInterleavedMasks;
    WORD    msb, lsb;
    INT     lDelta;
    PPDEV   ppdev;
    WORD    PointerDataY;
    BYTE    s3AndMask[64][8],
            s3XorMask[64][8];
    WORD    s3InterleavedMasks[512];

    DISPDBG((3, "S3.DLL:DrvSetMonoHwPointerShape - Entry\n"));

    ppdev = (PPDEV) pso->dhpdev;

    // If the mask is NULL this implies the pointer is not
    // visible.

    if (psoMask == NULL)
    {
        OUTPW (CRTC_INDEX, (ppdev->HgcMode & ~(HGC_ENABLE << 8)));
        return (SPS_ACCEPT_NOEXCLUDE);
    }

    // Init the AND and XOR masks.

    memset (s3AndMask, 0xFF, 512);
    memset (s3XorMask, 0x00, 512);

    // Get the bitmap dimensions.

    cxMask = psoMask->sizlBitmap.cx;
    cyMask = psoMask->sizlBitmap.cy;

    cyAND = cyXOR = cyMask / 2;
    cxAND = cxXOR = cxMask / 8;

    // Set up pointers to the AND and XOR masks.

    pjAND  =  psoMask->pvScan0;
    lDelta = psoMask->lDelta;
    pjXOR  = pjAND + (cyAND * lDelta);

    // Copy the AND mask.

    memset (s3AndMask, 0xFFFFFFFF, 512);
    memset (s3XorMask, 0, 512);

    for (i = 0; i < cyAND; i++)
    {
        // Copy over a line of the AND mask.

        for (j = 0; j < cxAND; j++)
        {
            s3AndMask[i][j] = pjAND[j];
        }

        // point to the next line of the AND mask.

        pjAND += lDelta;
    }

    // Copy the XOR mask.

    for (i = 0; i < cyXOR; i++)
    {
        // Copy over a line of the XOR mask.

        for (j = 0; j < cxXOR; j++)
        {
            s3XorMask[i][j] = pjXOR[j];
        }

        // point to the next line of the XOR mask.

        pjXOR += lDelta;
    }

    // Interleave the S3AND and S3XOR masks.
    // This will allow an rep stosw instruction to down
    // load the mask to the card..

    pwS3AndMask = (PWORD) s3AndMask;
    pwS3XorMask = (PWORD) s3XorMask;
    pInterleavedMasks = s3InterleavedMasks;

    // Interleave the S3AND and S3XOR masks.
    // This will allow an rep stosw instruction to down
    // load the mask to the card..

    pwS3AndMask = (PWORD) s3AndMask;
    pwS3XorMask = (PWORD) s3XorMask;
    pInterleavedMasks = s3InterleavedMasks;

    k = 256;

    for (i = 0; i < k; i++)
    {
        *pInterleavedMasks++ = *pwS3AndMask++;
        *pInterleavedMasks++ = *pwS3XorMask++;
    }

    // Down load the pointer shape to the S3 chip.
    // Determine Which Monochrome pointer buffer is available.

    if (ppdev->MonoPointerData == 0)
    {
        PointerDataY = LOWORD (PTR_DATA_Y);
        ppdev->MonoPointerData = 1;
    }
    else
    {
        PointerDataY = LOWORD (PTR_DATA_Y - 1);
        ppdev->MonoPointerData = 0;
    }

    // Set the clipping window to include the last line of offscreen
    // memory.

    vResetS3Clipping(ppdev);

    // Wait for Fifo 7 to empty.

    FIFOWAIT(FIFO_7_EMPTY);

    // Now set up for the rectangle.

    OUTPW (CUR_X, PTR_DATA_X);
    OUTPW (CUR_Y, PointerDataY);

    OUTPW (RECT_WIDTH, ppdev->cxMaxRam - 1);
    OUTPW (MULTIFUNC_CNTL, RECT_HEIGHT);

    OUTPW (MULTIFUNC_CNTL, DATA_EXTENSION);

    TEST_AND_SET_FRGD_MIX(SRC_CPU_DATA | OVERPAINT);

    GPWAIT();

    OUTPW (CMD, (RECTANGLE_FILL | BYTE_SWAP |
                 BUS_SIZE_16    | WAIT      |
                 DRAWING_DIR_TBLRXM         |
                 DRAW      | WRITE));

    CHECK_DATA_READY;

    // Now down load the masks.

    vDataPortOut(ppdev, (PWORD) s3InterleavedMasks, 512);

    CHECK_DATA_COMPLETE;

    // This should do it. (the cursor should be downloaded)
    // Now, wasn't that simple, NOT!

    // Note: At one point we disabled the pointer before setting the new one.
    //       this caused an excessive flicker of the pointer, so we removed the disable.
    //       At another point, we found the cursor jumped to the left for 1 frame if
    //       we did not disable the cursor for non-animated pointers.  So, now
    //       we handle each case separately.


    if (!(ppdev->flPointer & ANIMATEUPDATE))
    {
        OUTPW (CRTC_INDEX, (ppdev->HgcMode | (HGC_DISABLE << 8)));
    }

    // Set the hardware graphics cursor storage area start address for
    // the new pointer data.

    msb = HIBYTE (PointerDataY);
    lsb = LOBYTE (PointerDataY);

    OUTPW (CRTC_INDEX, ((msb << 8) | CR4C));
    OUTPW (CRTC_INDEX, ((lsb << 8) | CR4D));

    // Set the position of the cursor.
    // Need to do this twice, due to shadowing in the 928 and 801/805.

    DrvMoveHwPointer(pso, x, y, NULL);

    // On the 928, 801/805 wait for vertical interval then set the
    // position a second time.

    if (ppdev->s3ChipID >= 0x90)
    {
        while (INP(STATUS_1) & VSY_NOT);
        DrvMoveHwPointer(pso, x, y, NULL);

        while (!(INP(STATUS_1) & VSY_NOT));
        DrvMoveHwPointer(pso, x, y, NULL);

    }

    OUTPW (CRTC_INDEX, (ppdev->HgcMode | (HGC_ENABLE << 8)));

    // Reset the clipping.  This can go when no H/W clipping is done
    // on the color pointer.

    vResetS3Clipping(ppdev);

    return (SPS_ACCEPT_NOEXCLUDE);
}



/*****************************************************************************
 * DrvSetBt485PointerShape -
 ****************************************************************************/
ULONG DrvSetBt485PointerShape(
    SURFOBJ     *pso,
    SURFOBJ     *psoMask,
    SURFOBJ     *psoColor,
    XLATEOBJ    *pxlo,
    LONG        xHot,
    LONG        yHot,
    LONG        x,
    LONG        y,
    RECTL       *prcl,
    FLONG       fl)
{
    UINT    i, j, cxMask, cyMask, cyAND, cxAND, cyXOR, cxXOR;
    PBYTE   pjAND, pjXOR;
    INT     lDelta;
    PPDEV   ppdev;
    BYTE    s3AndMask[64][8],
            s3XorMask[64][8];

    DISPDBG((3, "S3.DLL:DrvSetBt485PointerShape - Entry\n"));

    ppdev = (PPDEV) pso->dhpdev;

    // If the mask is NULL this implies the pointer is not
    // visible.

    if (psoMask == NULL)
    {
        // Disable the H/W cursor on the Bt 485.

        OUTPW (CRTC_INDEX, (ppdev->ExtDacCtl | 0x0200));
        OUTP  (BT485_ADDR_CMD_REG2, ppdev->Bt485CmdReg2 & BT485_CURSOR_DISABLE);
        OUTPW (CRTC_INDEX, (ppdev->ExtDacCtl));

        return (SPS_ACCEPT_NOEXCLUDE);
    }

    // Init the AND and XOR masks.

    memset (s3AndMask, 0xFF, 512);
    memset (s3XorMask, 0x00, 512);

    // Get the bitmap dimensions.

    cxMask = psoMask->sizlBitmap.cx;
    cyMask = psoMask->sizlBitmap.cy;

    cyAND = cyXOR = cyMask / 2;
    cxAND = cxXOR = cxMask / 8;

    // Set up pointers to the AND and XOR masks.

    pjAND  =  psoMask->pvScan0;
    lDelta = psoMask->lDelta;
    pjXOR  = pjAND + (cyAND * lDelta);

    // Copy the AND mask.

    for (i = 0; i < cyAND; i++)
    {
        // Copy over a line of the AND mask.

        for (j = 0; j < cxAND; j++)
        {
            s3AndMask[i][j] = pjAND[j];
        }

        // point to the next line of the AND mask.

        pjAND += lDelta;
    }

    // Copy the XOR mask.

    for (i = 0; i < cyXOR; i++)
    {
        // Copy over a line of the XOR mask.

        for (j = 0; j < cxXOR; j++)
        {
            s3XorMask[i][j] = pjXOR[j];
        }

        // point to the next line of the XOR mask.

        pjXOR += lDelta;
    }

    pjAND = (PBYTE) s3AndMask;
    pjXOR = (PBYTE) s3XorMask;

    // Set the cursor for 64 X 64, and set the 2 MSB's for the cursor
    // RAM addr to 0.
    // First get access to Command Register 3

    OUTPW (CRTC_INDEX, (ppdev->ExtDacCtl | 0x0100));
    OUTP  (BT485_ADDR_CMD_REG0, ppdev->Bt485CmdReg0 | BT485_CMD_REG_3_ACCESS);

    OUTPW (CRTC_INDEX, ppdev->ExtDacCtl);
    OUTP  (0x3c8, 0x01);

    OUTPW (CRTC_INDEX, (ppdev->ExtDacCtl | 0x0200));
    OUTP  (BT485_ADDR_CMD_REG3, ppdev->Bt485CmdReg3);

    // Disable the H/W cursor on the Bt 485.

    OUTPW (CRTC_INDEX, (ppdev->ExtDacCtl | 0x0200));
    OUTP  (BT485_ADDR_CMD_REG2, ppdev->Bt485CmdReg2 & BT485_CURSOR_DISABLE);

    // Down load the AND mask

    OUTPW (CRTC_INDEX, ppdev->ExtDacCtl);
    OUTP (BT485_ADDR_CUR_RAM_WRITE, 0x0);

    OUTPW (CRTC_INDEX, (ppdev->ExtDacCtl | 0x0200));

    // Down load the XOR mask

    for (i = 0 ; i < 512 ; i++)
    {
        OUTP (BT485_CUR_RAM_ARRAY_DATA, pjXOR[i]);
    }

    // Down load the AND mask

    for (i = 0 ; i < 512 ; i++)
    {
        OUTP (BT485_CUR_RAM_ARRAY_DATA, pjAND[i]);
    }

    // Set the position of the cursor.
    // Need to do this twice, due to shadowing in the 928 and 801/805.

    DrvMoveBt485Pointer(pso, x, y, NULL);

    // Enable the H/W cursor

    OUTPW (CRTC_INDEX, (ppdev->ExtDacCtl | 0x0200));
    OUTP  (BT485_ADDR_CMD_REG2, ppdev->Bt485CmdReg2 | BT485_CURSOR_MODE2);

    // Reset DAC extended registers.

    OUTPW (CRTC_INDEX, (ppdev->ExtDacCtl));

    return (SPS_ACCEPT_NOEXCLUDE);
}




