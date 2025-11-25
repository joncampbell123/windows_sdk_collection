/******************************Module*Header*******************************\
* Module Name: hardware.c
*
* Hardware dependent initialization
*
* Copyright (c) 1992 Microsoft Corporation
*
\**************************************************************************/

#include "driver.h"

VOID vBlowCache(PPDEV ppdev);

/*****************************************************************************
 * vSaveOffScreenMemory
 ****************************************************************************/
VOID vSaveOffScreenMemory(PPDEV ppdev)
{
    WORD    cmd;
    ULONG   i;
    WORD*   pwBuffer;

    DISPDBG((2, "S3.DLL!vSaveOffScreenMemory - Entry\n"));

    if (ppdev->pOffScreenSaveBuffer == NULL)
    {
        ppdev->pOffScreenSaveBuffer = (PWORD) LocalAlloc(LPTR, (OFF_SCREEN_CX * OFF_SCREEN_CY));

        if (ppdev->pOffScreenSaveBuffer == NULL)
        {
            DISPDBG((0, "S3.DLL: vSaveOffScreenMemory -  LocalAlloc failed \n"));
            return;
        }
    }

    // Transfer the image (that happens to be the off screen memory)
    // from the screen to host memory.

    cmd =   RECTANGLE_FILL     | BYTE_SWAP      | BUS_SIZE_16 |
            DRAWING_DIR_TBLRXM | DIR_TYPE_XY    | WAIT |
            DRAW               | LAST_PIXEL_ON  | READ;

    FIFOWAIT(FIFO_7_EMPTY);

    TEST_AND_SET_RD_MASK(0xff);
    OUTPW (MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));

    OUTPW (CUR_X, 0);
    OUTPW (CUR_Y, OFF_SCREEN_Y);
    OUTPW (RECT_WIDTH, (OFF_SCREEN_CX - 1));
    OUTPW (MULTIFUNC_CNTL, (RECT_HEIGHT | (OFF_SCREEN_CY - 1)));

    OUTPW (CMD, cmd);

    // Wait for the Data Available.

    while (!(INPW(GP_STAT) & READ_DATA_AVAILABLE));

    // Now transfer the data from the screen to the host memory bitmap.

    // NOTE: We call vDataPortIn once for each scan instead of doing just
    // one large call to vDataPortIn because for some reason some machines
    // choke on a REP INSW of that large a size -- the S3 gets into a mode
    // such that even when the transfer is completely done, it still
    // claims there's more data to be sent.  This happens on the machine
    // regardless of S3 chip type, but this seems to be an effective work-
    // around for all cases:

    pwBuffer = (WORD*) ppdev->pOffScreenSaveBuffer;
    for (i = OFF_SCREEN_CY; i > 0; i--)
    {
        vDataPortIn(ppdev, pwBuffer, OFF_SCREEN_CX / 2);
        pwBuffer += OFF_SCREEN_CX / 2;
    }

}


/*****************************************************************************
 *
 ****************************************************************************/
VOID vRestoreOffScreenMemory(PPDEV ppdev)
{
    WORD    cmd;


    DISPDBG((2, "S3.DLL!vRestoreOffScreenMemory - Entry\n"));

    // If there is no buffer then just return.

    if (ppdev->pOffScreenSaveBuffer == NULL) {

        // This will happen when we're in a low memory condition.

        vBlowCache(ppdev);
        return;
    }

    // Transfer the image (that happens to be the off screen memory)
    // from host memory to the off screen memory.

    cmd = RECTANGLE_FILL     | BYTE_SWAP      | BUS_SIZE_16 |
          DRAWING_DIR_TBLRXM | DIR_TYPE_XY    | WAIT |
          DRAW               | LAST_PIXEL_ON  | WRITE;

    FIFOWAIT(FIFO_8_EMPTY);

    TEST_AND_SET_WRT_MASK(0xff);
    OUTPW (MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));
    TEST_AND_SET_FRGD_MIX(SRC_CPU_DATA | OVERPAINT);

    OUTPW (CUR_X, 0);
    OUTPW (CUR_Y, OFF_SCREEN_Y);
    OUTPW (RECT_WIDTH, (OFF_SCREEN_CX - 1));
    OUTPW (MULTIFUNC_CNTL, (RECT_HEIGHT | (OFF_SCREEN_CY - 1)));

    OUTPW (CMD, cmd);

    // Now transfer the data from the screen to the host memory bitmap.

    vDataPortOut(ppdev, ppdev->pOffScreenSaveBuffer, ((OFF_SCREEN_CX * OFF_SCREEN_CY)/2));

    // We free this up for two reasons:
    // 1) Going to/from Full Screen mode is slow, no one will
    //    notice the Alloc/Free.
    // 2) It's not nice to keep a 256K chunk of memory around when
    //    one really doesn't need it.
    //

    LocalFree(ppdev->pOffScreenSaveBuffer);
    ppdev->pOffScreenSaveBuffer = NULL;
}
