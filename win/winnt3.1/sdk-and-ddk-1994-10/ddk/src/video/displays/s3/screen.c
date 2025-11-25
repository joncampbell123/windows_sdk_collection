/******************************Module*Header*******************************\
* Module Name: screen.c
*
* Initializes the GDIINFO and DEVINFO structures for DrvEnablePDEV.
*
* Copyright (c) 1992 Microsoft Corporation
\**************************************************************************/

#include "driver.h"

#define SYSTM_LOGFONT {16,7,0,0,700,0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,VARIABLE_PITCH | FF_DONTCARE,L"System"}
#define HELVE_LOGFONT {12,9,0,0,400,0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_STROKE_PRECIS,PROOF_QUALITY,VARIABLE_PITCH | FF_DONTCARE,L"MS Sans Serif"}
#define COURI_LOGFONT {12,9,0,0,400,0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_STROKE_PRECIS,PROOF_QUALITY,FIXED_PITCH | FF_DONTCARE, L"Courier"}

// This is the basic devinfo for a default driver.  This is used as a base and customized based
// on information passed back from the miniport driver.

const DEVINFO gDevInfoFrameBuffer = {
    (GCAPS_OPAQUERECT |     /* Graphics capabilities               */
     GCAPS_ALTERNATEFILL | GCAPS_WINDINGFILL |
     GCAPS_TRAPPAINT | GCAPS_MONO_DITHER),
    SYSTM_LOGFONT,          /* Default font description */
    HELVE_LOGFONT,          /* ANSI variable font description   */
    COURI_LOGFONT,          /* ANSI fixed font description          */
    0,                      /* Count of device fonts          */
    0,                      /* Preferred DIB format          */
    8,                      /* Width of color dither          */
    8,                      /* Height of color dither   */
    0                       /* Default palette to use for this device */
};

PUCHAR gpucCsrBase = NULL;

// Under a heavy loaded system it seems that IOCTL_VIDEO_SET_CURRENT_MODE failed,
// which caused the system to hang.  This is where we record the last value
// from the miniport.

BOOL   gbLastReturnSetCurrentMode;

/******************************Public*Routine******************************\
* bInitSURF
*
* Enables the surface.        Maps the frame buffer into memory.
*
\**************************************************************************/

BOOL bInitSURF(
    PPDEV ppdev,
    BOOL bFirst)
{
    VIDEO_MEMORY             VideoMemory;
    VIDEO_MEMORY_INFORMATION VideoMemoryInfo;
    DWORD                    ReturnedDataLength;
    INT                      i;
    BYTE                     byte;

#if !defined(_X86_) && !defined(i386)
    VIDEO_PUBLIC_ACCESS_RANGES   VideoAccessRange;
#endif


    DISPDBG((2, "S3.DLL! bInitSURF entry\n"));

    // Set the mode.

    gbLastReturnSetCurrentMode = DeviceIoControl(ppdev->hDriver,
                                                 IOCTL_VIDEO_SET_CURRENT_MODE,
                                                 &ppdev->ulMode,  // input buffer
                                                 sizeof(DWORD),
                                                 NULL,
                                                 0,
                                                 &ReturnedDataLength,
                                                 NULL);
    if (gbLastReturnSetCurrentMode == FALSE)
    {
        DISPDBG((0, "S3.DLL!bInitSURF - Initialization error-Set mode"));
        return (FALSE);
    }

    if (bFirst)
    {
#if !defined(_X86_) && !defined(i386)

	//
	// Map io ports into virtual memory.
	//
	
	VideoMemory.RequestedVirtualAddress = NULL;
	
	if (!DeviceIoControl(ppdev->hDriver,
			     IOCTL_VIDEO_QUERY_PUBLIC_ACCESS_RANGES,
			     NULL,                      // input buffer
			     0,
			     (PVOID) &VideoAccessRange, // output buffer
			     sizeof (VideoAccessRange),
			     &ReturnedDataLength,
			     NULL)) {
	
	    RIP("S3.DLL: Initialization error-Map IO port base");
            return (FALSE);
	
	}
	
	gpucCsrBase = (PUCHAR)VideoAccessRange.VirtualAddress;

#endif

	// Set all the register addresses.

        ppdev->cur_x            = 0x86E8;
        ppdev->cur_y            = 0x82E8;

        ppdev->dest_x           = 0x8EE8;
        ppdev->dest_y           = 0x8AE8;

        ppdev->axstp            = 0x8AE8;
        ppdev->diastp           = 0x8EE8;

        ppdev->rect_width       = 0x96E8;
        ppdev->line_max         = 0x96E8;

        ppdev->err_term         = 0x92E8;

        ppdev->gp_stat          = 0x9AE8;
        ppdev->cmd              = 0x9AE8;
        ppdev->short_stroke_reg = 0x9EE8;

        ppdev->multifunc_cntl   = 0xBEE8;

        ppdev->bkgd_color       = 0xA2E8;
        ppdev->frgd_color       = 0xA6E8;

        ppdev->bkgd_mix         = 0xB6E8;
        ppdev->frgd_mix         = 0xBAE8;

        ppdev->wrt_mask         = 0xAAE8;
        ppdev->rd_mask          = 0xAEE8;

        ppdev->pixel_transfer   = 0xE2E8;

        // Check if the I/O register need to be remapped.

        OUTPW (CRTC_INDEX, ((SYSCTL_UNLOCK << 8) | CR39));

        OUTP  (CRTC_INDEX, 0x43);
        if (INP(CRTC_DATA) & 0x10)
        {
            ppdev->cur_x            ^= 0x3A0;
            ppdev->cur_y            ^= 0x3A0;
            ppdev->dest_x           ^= 0x3A0;
            ppdev->dest_y           ^= 0x3A0;
            ppdev->axstp            ^= 0x3A0;
            ppdev->diastp           ^= 0x3A0;
            ppdev->rect_width       ^= 0x3A0;
            ppdev->line_max         ^= 0x3A0;
            ppdev->err_term         ^= 0x3A0;
            ppdev->gp_stat          ^= 0x3A0;
            ppdev->cmd              ^= 0x3A0;
            ppdev->short_stroke_reg ^= 0x3A0;
            ppdev->multifunc_cntl   ^= 0x3A0;
            ppdev->bkgd_color       ^= 0x3A0;
            ppdev->frgd_color       ^= 0x3A0;
            ppdev->bkgd_mix         ^= 0x3A0;
            ppdev->frgd_mix         ^= 0x3A0;
            ppdev->wrt_mask         ^= 0x3A0;
            ppdev->rd_mask          ^= 0x3A0;
            ppdev->pixel_transfer   ^= 0x3A0;
        }

        // Get the linear memory address range.

        VideoMemory.RequestedVirtualAddress = NULL;

        if (!DeviceIoControl(ppdev->hDriver,
                             IOCTL_VIDEO_MAP_VIDEO_MEMORY,
                             (PVOID) &VideoMemory, // input buffer
                             sizeof (VIDEO_MEMORY),
                             (PVOID) &VideoMemoryInfo, // output buffer
                             sizeof (VideoMemoryInfo),
                             &ReturnedDataLength,
                             NULL))
        {
            DISPDBG((0, "S3.DLL!bInitSURF - Initialization error-Map buffer address"));
            return (FALSE);
        }

        // Record the Frame Buffer Linear Address.

        ppdev->pjScreen = (PBYTE) VideoMemoryInfo.FrameBufferBase;

	// Create a default Clip Object.  This will be used when a NULL
        // clip object is passed to us.

        ppdev->pcoDefault = EngCreateClip();

        ppdev->pcoDefault->iDComplexity = DC_RECT;
        ppdev->pcoDefault->iMode        = TC_RECTANGLES;

        ppdev->pcoDefault->rclBounds.left   = 0;
        ppdev->pcoDefault->rclBounds.top    = 0;
        ppdev->pcoDefault->rclBounds.right  = ppdev->cxScreen;
        ppdev->pcoDefault->rclBounds.bottom = ppdev->cyScreen;

        ppdev->pcoFullRam = EngCreateClip();

        ppdev->pcoFullRam->iDComplexity = DC_TRIVIAL;
        ppdev->pcoFullRam->iMode        = TC_RECTANGLES;

        ppdev->pcoFullRam->rclBounds.left   = 0;
        ppdev->pcoFullRam->rclBounds.top    = 0;
        ppdev->pcoFullRam->rclBounds.right  = ppdev->cxMaxRam;
        ppdev->pcoFullRam->rclBounds.bottom = ppdev->cyMaxRam;

        // Create a DDA object (now only used by trap enumeration)

        ppdev->pdda = EngCreateDDA();

        // Initialize the Unique Brush Counter,
        // Allocate and Initialize the Brush cache manager arrays.
        // Init the Expansion Cache Tags.

        ppdev->gBrushUnique = 1;

        ppdev->iMaxCachedColorBrushes   = MAX_COLOR_PATTERNS;
        ppdev->iNextColorBrushCacheSlot = 0;

        ppdev->pulColorBrushCacheEntries = (PULONG) LocalAlloc(LPTR, MAX_COLOR_PATTERNS * sizeof (ULONG));

        if (ppdev->pulColorBrushCacheEntries == NULL)
        {
            DISPDBG((0, "S3.DLL!bInitSURF - LocalAlloc for pulColorBrushCacheEntries failed\n"));
            return (FALSE);
        }

        ppdev->ulColorExpansionCacheTag = 0;

        ppdev->iMaxCachedMonoBrushes   = MAX_MONO_PATTERNS;
        ppdev->iNextMonoBrushCacheSlot = 0;

        ppdev->pulMonoBrushCacheEntries = (PULONG) LocalAlloc(LPTR, MAX_MONO_PATTERNS * sizeof (ULONG));

        if (ppdev->pulMonoBrushCacheEntries == NULL)
        {
            DISPDBG((0, "S3.DLL!bInitSURF - LocalAlloc for pulMonoBrushCacheEntries failed\n"));
            return (FALSE);
        }

        for (i = 0; i < 8; i++)
        {
            ppdev->aulMonoExpansionCacheTag[i] = 0;
        }

        ppdev->iNextMonoBrushExpansionSlot = 0;

        // Init the cache tags for the Source bitmap cache.

        ppdev->hsurfCachedBitmap = NULL;
        ppdev->iUniqCachedBitmap = (ULONG) -1;

        // Init the Save Screen Bits structures.

        ppdev->iUniqeSaveScreenBits           = 1;
        ppdev->SavedScreenBitsHeader.pssbLink = NULL;
        ppdev->SavedScreenBitsHeader.iUniq    = (ULONG) -1;

        // Get the chip ID and set the New Bank Control bool.

        OUTPW(0x3d4, 0x4838);          // unlocke S3 regs
        OUTP(0x3d4, 0x30);             // crtc index for S# ID reg
        byte = INP(0x3d5);             // Get the ID
        OUTPW(0x3d4, 0x0038);          // Lock the S3 regs

        ppdev->s3ChipID = byte;

        if (ppdev->cxScreen == 1280)
            ppdev->bBt485Dac = TRUE;
        else
            ppdev->bBt485Dac = FALSE;

        // Pickup the initial value for the Pointer Control Register.
        // This is requried for the 928,801/805 and benign on the 911/924.

        OUTP(CRTC_INDEX, HGC_MODE);
        ppdev->HgcMode = ((INP(CRTC_DATA) & ~0x11) << 8) | HGC_MODE;

        switch(byte)
        {
            case 0x90:                  // 801/805 rev 0
            case 0x91:                  // 801/805 rev 1
            case 0x92:                  // 801/805 rev 2
            case 0x93:                  // 801/805 rev 3

            case 0xA0:                  // 928 rev 0
            case 0xA1:                  // 928 rev 1
            case 0xA2:                  // 928 rev 2
            case 0xA3:                  // 928 rev 3
                ppdev->bNewBankControl = TRUE;

                // Pick up the initial values for the System Control and Linear Address
                // Windows Control register.

                OUTPW (CRTC_INDEX, ((SYSCTL_UNLOCK << 8) | CR39));

                OUTP(CRTC_INDEX, SYS_CNFG);
                ppdev->SysCnfg = ((INP(CRTC_DATA) << 8) | SYS_CNFG) & ~0x0900;

                OUTP(CRTC_INDEX, LAW_CTL);
                ppdev->LawCtl = ((INP(CRTC_DATA) << 8) | LAW_CTL) & ~0x1000;

                OUTP(CRTC_INDEX, EX_SCTL_2);
                ppdev->ExtSysCtl2 = (INP(CRTC_DATA) & ~0x0C);

                if (ppdev->bBt485Dac == TRUE)
                {
                    // Make a copy of the Extened DAC control register

                    OUTP(CRTC_INDEX, EX_DAC_CT);
                    ppdev->ExtDacCtl = (((INP(CRTC_DATA) << 8) | EX_DAC_CT) & ~0x0300) ;

                    // Get a copy of the Bt486 command register 0

                    OUTPW (CRTC_INDEX, (ppdev->ExtDacCtl | 0x0100));
                    ppdev->Bt485CmdReg0 = INP (BT485_ADDR_CMD_REG0);

                    // Get a copy of the Bt486 command register 1

                    OUTPW (CRTC_INDEX, (ppdev->ExtDacCtl | 0x0200));
                    ppdev->Bt485CmdReg1 = INP (BT485_ADDR_CMD_REG1);

                    // Get a copy of the Bt486 command register 2
                    // And mask off the Cursor control bits.

                    ppdev->Bt485CmdReg2 = INP (BT485_ADDR_CMD_REG2) & BT485_CURSOR_DISABLE;

                    // Disable the cursor

                    OUTP (BT485_ADDR_CMD_REG2, ppdev->Bt485CmdReg2);

                    // To get the Bt485 command register 3 we have to go through
                    // following indirection "dance".

                    // First set the Command Reg3 access bit in Command Reg0

                    OUTPW (CRTC_INDEX, (ppdev->ExtDacCtl | 0x0100));
                    OUTP  (BT485_ADDR_CMD_REG0, ppdev->Bt485CmdReg0 | BT485_CMD_REG_3_ACCESS);

                    // Now set the index to 1

                    OUTPW (CRTC_INDEX, ppdev->ExtDacCtl);
                    OUTP  (0x3c8, 0x01);

                    // Now access command register 3
                    // and save the value away

                    OUTPW (CRTC_INDEX, (ppdev->ExtDacCtl | 0x0200));
                    ppdev->Bt485CmdReg3 = INP(BT485_ADDR_CMD_REG3);

                    // Set Command register 3 for a 64 X 64 cursor.

                    ppdev->Bt485CmdReg3 |= BT485_64X64_CURSOR;
                    OUTP (BT485_ADDR_CMD_REG3, ppdev->Bt485CmdReg3);

                    // Disable access to command reg 3

                    OUTPW (CRTC_INDEX, (ppdev->ExtDacCtl | 0x0100));
                    OUTP  (BT485_ADDR_CMD_REG0, ppdev->Bt485CmdReg0);

                    // Set the color 1 and color 2 for the cursor.
                    // Select Address register; cursor/overscan color write on the Bt485.

                    OUTPW (CRTC_INDEX, (ppdev->ExtDacCtl | 0x0100));
                    OUTP (BT485_ADDR_CUR_COLOR_WRITE, BT485_CURSOR_COLOR_1);

                    // Output the RGB for Cursor color 1

                    OUTP (BT485_CUR_COLOR_DATA, 0x00);
                    OUTP (BT485_CUR_COLOR_DATA, 0x00);
                    OUTP (BT485_CUR_COLOR_DATA, 0x00);

                    // Output the RGB for Cursor color 2

                    OUTP (BT485_CUR_COLOR_DATA, 0xff);
                    OUTP (BT485_CUR_COLOR_DATA, 0xff);
                    OUTP (BT485_CUR_COLOR_DATA, 0xff);

                    // reset the DAC addr

                    OUTPW (CRTC_INDEX, ppdev->ExtDacCtl);
                }

                break;

            case 0x81:                  // 911
            case 0x82:                  // 924
            default:
                ppdev->bNewBankControl = FALSE;
                break;
        }
    }

    // Initialize the shadow copies of the S3 registers to an
    // invalid state.  This also needs to be done when we return from a
    // screen session.

    ppdev->ForegroundMix   =
    ppdev->BackgroundMix   =
    ppdev->ForegroundColor =
    ppdev->BackgroundColor =
    ppdev->WriteMask       =
    ppdev->ReadMask        = (WORD) -1;

    // Reset the clipping registers.

    vResetS3Clipping(ppdev);

    // Unlock the registers used by the cursor

    OUTPW (CRTC_INDEX, ((SYSCTL_UNLOCK << 8) | CR39));

    return(TRUE);
}

/******************************Public*Routine******************************\
* vDisableSURF
*
* Disable the surface. Un-Maps the frame in memory.
*
\**************************************************************************/

VOID vDisableSURF(PPDEV ppdev)
{
    DWORD returnedDataLength;
    VIDEO_MEMORY videoMemory;

    videoMemory.RequestedVirtualAddress = (PVOID) ppdev->pjScreen;

    if (!DeviceIoControl(ppdev->hDriver,
                        IOCTL_VIDEO_UNMAP_VIDEO_MEMORY,
                        &videoMemory,
                        sizeof(VIDEO_MEMORY),
                        NULL,
                        0,
                        &returnedDataLength,
                        NULL))
    {
        DISPDBG((0, "DISP vDisableSURF failed IOCTL_VIDEO_UNMAP\n"));
    }

#if !defined(_X86_) && !defined(i386)


    // NOTE NOTE NOTE
    //
    //	WE NEED TO UNMAP THE CSRS HERE!

#endif

}

/******************************Public*Routine******************************\
* bInitPDEV
*
* Determine the mode we should be in based on the DEVMODE passed in.
* Query mini-port to get information needed to fill in the DevInfo and the
* GdiInfo .
*
\**************************************************************************/

BOOL bInitPDEV(
    PPDEV ppdev,
    DEVMODEW *pDevMode)
{


    ULONG cModes;
    PVIDEO_MODE_INFORMATION pVideoBuffer, pVideoModeSelected, pVideoTemp;
    BOOL bSelectDefault;
    GDIINFO *pGdiInfo;
    VIDEO_MODE_INFORMATION VideoModeInformation;
    ULONG cbModeSize;


    DISPDBG((2,"S3.DLL:!bInitPDEV - Entry\n"));

    pGdiInfo = ppdev->pGdiInfo;

    //
    // calls the miniport to get mode information.
    //

    cModes = getAvailableModes(ppdev->hDriver, &pVideoBuffer, &cbModeSize);

    if (cModes == 0)
    {
        return(FALSE);
    }

    //
    // Determine if we are looking for a default mode.
    //

    if ( ((pDevMode->dmPelsWidth) ||
          (pDevMode->dmPelsHeight) ||
          (pDevMode->dmBitsPerPel) ||
          (pDevMode->dmDisplayFlags) ||
          (pDevMode->dmDisplayFrequency)) == 0)
    {
        bSelectDefault = TRUE;
    }
    else
    {
        bSelectDefault = FALSE;
    }

    //
    // Now see if the requested mode has a match in that table.
    //

    pVideoModeSelected = NULL;
    pVideoTemp = pVideoBuffer;

    while (cModes--)
    {
        if (pVideoTemp->Length != 0)
        {
            if (bSelectDefault ||
                ((pVideoTemp->VisScreenWidth  == pDevMode->dmPelsWidth) &&
                 (pVideoTemp->VisScreenHeight == pDevMode->dmPelsHeight) &&
                 (pVideoTemp->BitsPerPlane *
                  pVideoTemp->NumberOfPlanes  == pDevMode->dmBitsPerPel)) &&
                 (pVideoTemp->Frequency ==  pDevMode->dmDisplayFrequency))
            {
                pVideoModeSelected = pVideoTemp;
                DISPDBG((3, "S3: Found a match\n"));
                break;
            }
        }

        pVideoTemp = (PVIDEO_MODE_INFORMATION)
            (((PUCHAR)pVideoTemp) + cbModeSize);

    }

    //
    // If no mode has been found, return an error
    //

    if (pVideoModeSelected == NULL)
    {
        return(FALSE);
    }


    // We have chosen the one we want.  Save it in a stack buffer and
    // get rid of allocated memory before we forget to free it.

    VideoModeInformation = *pVideoModeSelected;
    LocalFree(pVideoBuffer);

    // Set up screen information from the mini-port

    ppdev->ulMode       = VideoModeInformation.ModeIndex;
    ppdev->cxScreen     = VideoModeInformation.VisScreenWidth;
    ppdev->cyScreen     = VideoModeInformation.VisScreenHeight;

    ppdev->cxMaxRam     = VideoModeInformation.VideoMemoryBitmapWidth;
    ppdev->cyMaxRam     = VideoModeInformation.VideoMemoryBitmapHeight;

    ppdev->ulBitCount   = VideoModeInformation.BitsPerPlane *
                          VideoModeInformation.NumberOfPlanes;
    ppdev->lDeltaScreen = VideoModeInformation.ScreenStride;

    ppdev->flRed        = VideoModeInformation.RedMask;
    ppdev->flGreen      = VideoModeInformation.GreenMask;
    ppdev->flBlue       = VideoModeInformation.BlueMask;

    // Fill in the GDIINFO data structure with the information
    // returned from the kernel driver.

    pGdiInfo->ulVersion    = 0x1000;        // Our driver is version 1.000
    pGdiInfo->ulTechnology = DT_RASDISPLAY;
    pGdiInfo->ulHorzSize   = VideoModeInformation.XMillimeter;
    pGdiInfo->ulVertSize   = VideoModeInformation.YMillimeter;

    pGdiInfo->ulHorzRes    = ppdev->cxScreen;
    pGdiInfo->ulVertRes    = ppdev->cyScreen;
    pGdiInfo->cBitsPixel   = VideoModeInformation.BitsPerPlane;
    pGdiInfo->cPlanes      = VideoModeInformation.NumberOfPlanes;

    // The following block of code is left in place to make it easy
    // when we go to a 16/24/32 bit per pixel bitmap.

    if (ppdev->ulBitCount == 8)
    {
        // It is Palette Managed.

        pGdiInfo->ulNumColors = 20;
        pGdiInfo->ulNumPalReg = 1 << ppdev->ulBitCount;

        pGdiInfo->flRaster = 0;     // DDI reserved field
    }
    else
    {
        pGdiInfo->ulNumColors = 1 << ppdev->ulBitCount;
        pGdiInfo->ulNumPalReg = 0;

        pGdiInfo->flRaster = 0;     // DDI reserved field
    }

    pGdiInfo->ulLogPixelsX    = 96;
    pGdiInfo->ulLogPixelsY    = 96;

    pGdiInfo->flTextCaps      = TC_RA_ABLE;

    pGdiInfo->ulDACRed        = VideoModeInformation.NumberRedBits;
    pGdiInfo->ulDACGreen      = VideoModeInformation.NumberGreenBits;
    pGdiInfo->ulDACBlue       = VideoModeInformation.NumberBlueBits;

    pGdiInfo->xStyleStep      = 1;       // A style unit is 3 pels
    pGdiInfo->yStyleStep      = 1;
    pGdiInfo->denStyleStep    = 3;

    pGdiInfo->ulAspectX       = 0x24;    // One-to-one aspect ratio
    pGdiInfo->ulAspectY       = 0x24;
    pGdiInfo->ulAspectXY      = 0x33;

    pGdiInfo->ptlPhysOffset.x = 0;
    pGdiInfo->ptlPhysOffset.y = 0;
    pGdiInfo->szlPhysSize.cx  = 0;
    pGdiInfo->szlPhysSize.cy  = 0;

    // RGB and CMY color info.

    pGdiInfo->ciDevice.Red.x            = 6700;
    pGdiInfo->ciDevice.Red.y            = 3300;
    pGdiInfo->ciDevice.Red.Y            = 0;

    pGdiInfo->ciDevice.Green.x          = 2100;
    pGdiInfo->ciDevice.Green.y          = 7100;
    pGdiInfo->ciDevice.Green.Y          = 0;

    pGdiInfo->ciDevice.Blue.x           = 1400;
    pGdiInfo->ciDevice.Blue.y           = 800;
    pGdiInfo->ciDevice.Blue.Y           = 0;

    pGdiInfo->ciDevice.Cyan.x           = 1750;
    pGdiInfo->ciDevice.Cyan.y           = 3950;
    pGdiInfo->ciDevice.Cyan.Y           = 0;

    pGdiInfo->ciDevice.Magenta.x        = 4050;
    pGdiInfo->ciDevice.Magenta.y        = 2050;
    pGdiInfo->ciDevice.Magenta.Y        = 0;

    pGdiInfo->ciDevice.Yellow.x         = 4400;
    pGdiInfo->ciDevice.Yellow.y         = 5200;
    pGdiInfo->ciDevice.Yellow.Y         = 0;

    pGdiInfo->ciDevice.AlignmentWhite.x = 3127;
    pGdiInfo->ciDevice.AlignmentWhite.y = 3290;
    pGdiInfo->ciDevice.AlignmentWhite.Y = 0;

    // Color Gamma adjustment values.

    pGdiInfo->ciDevice.RedGamma   = 20000;
    pGdiInfo->ciDevice.GreenGamma = 20000;
    pGdiInfo->ciDevice.BlueGamma  = 20000;

    // No dye correction for raster displays.

    pGdiInfo->ciDevice.MagentaInCyanDye   = 0;
    pGdiInfo->ciDevice.YellowInCyanDye    = 0;
    pGdiInfo->ciDevice.CyanInMagentaDye   = 0;
    pGdiInfo->ciDevice.YellowInMagentaDye = 0;
    pGdiInfo->ciDevice.CyanInYellowDye    = 0;
    pGdiInfo->ciDevice.MagentaInYellowDye = 0;

    pGdiInfo->ulDevicePelsDPI  = 0;   // For printers only
    pGdiInfo->ulPrimaryOrder   = PRIMARY_ORDER_CBA;
    pGdiInfo->ulHTPatternSize  = HT_PATSIZE_4x4_M;
    pGdiInfo->ulHTOutputFormat = HT_FORMAT_8BPP;
    pGdiInfo->flHTFlags        = HT_FLAG_ADDITIVE_PRIMS;

    // Fill in the devinfo structure

    *(ppdev->pDevInfo) = gDevInfoFrameBuffer;

    if (ppdev->ulBitCount == 8)
    {
        ppdev->pDevInfo->flGraphicsCaps |= (GCAPS_PALMANAGED |
                                            GCAPS_COLOR_DITHER);
        ppdev->pDevInfo->iDitherFormat   = BMF_8BPP;
    }
    else if (ppdev->ulBitCount == 16)
    {
        ppdev->pDevInfo->iDitherFormat = BMF_16BPP;
    }
    else
    {
        ppdev->pDevInfo->iDitherFormat = BMF_32BPP;
    }

    return(TRUE);
}


/******************************Public*Routine******************************\
* getAvailableModes
*
* Calls the miniport to get the list of modes supported by the kernel driver,
* and returns the list of modes supported by the diplay driver among those
*
* returns the number of entries in the videomode buffer.
* 0 means no modes are supported by the miniport or that an error occured.
*
* NOTE: the buffer must be freed up by the caller.
*
\**************************************************************************/
DWORD getAvailableModes(
HANDLE hDriver,
PVIDEO_MODE_INFORMATION *modeInformation,
DWORD *cbModeSize)
{

    ULONG ulTemp;
    VIDEO_NUM_MODES modes;
    PVIDEO_MODE_INFORMATION pVideoTemp;

    //
    // Get the number of modes supported by the mini-port
    //

    if (!DeviceIoControl(hDriver,
            IOCTL_VIDEO_QUERY_NUM_AVAIL_MODES,
            NULL,
            0,
            &modes,
            sizeof(VIDEO_NUM_MODES),
            &ulTemp,
            NULL))
    {
        DISPDBG((0, "s3.dll getAvailableModes failed VIDEO_QUERY_NUM_AVAIL_MODES\n"));
        return(0);
    }

    *cbModeSize = modes.ModeInformationLength;

    //
    // Allocate the buffer for the mini-port to write the modes in.
    //

    *modeInformation = (PVIDEO_MODE_INFORMATION)
                        LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT,
                                   modes.NumModes *
                                   modes.ModeInformationLength);

    if (*modeInformation == (PVIDEO_MODE_INFORMATION) NULL)
    {
        DISPDBG((0, "S3.dll: getAvailableModes failed LocalAlloc\n"));
        return 0;
    }

    //
    // Ask the mini-port to fill in the available modes.
    //

    if (!DeviceIoControl(hDriver,
            IOCTL_VIDEO_QUERY_AVAIL_MODES,
            NULL,
            0,
            *modeInformation,
            modes.NumModes * modes.ModeInformationLength,
            &ulTemp,
            NULL))
    {

        DISPDBG((0, "S3.dll: getAvailableModes failed VIDEO_QUERY_AVAIL_MODES\n"));

        LocalFree(*modeInformation);
        *modeInformation = (PVIDEO_MODE_INFORMATION) NULL;

        return(0);
    }

    //
    // Now see which of these modes are supported by the display driver.
    // As an internal mechanism, set the length to 0 for the modes we
    // DO NOT support.
    //

    ulTemp = modes.NumModes;
    pVideoTemp = *modeInformation;

    //
    // Mode is rejected if it is not one plane, or not graphics, or is not
    // one of 8, 16 or 32 bits per pel.
    //

    while (ulTemp--)
    {
        if ((pVideoTemp->NumberOfPlanes != 1 ) ||
            !(pVideoTemp->AttributeFlags & VIDEO_MODE_GRAPHICS) ||
            ((pVideoTemp->BitsPerPlane != 8) &&
             (pVideoTemp->BitsPerPlane != 16) &&
             (pVideoTemp->BitsPerPlane != 32)))
        {
            pVideoTemp->Length = 0;
        }

        pVideoTemp = (PVIDEO_MODE_INFORMATION)
            (((PUCHAR)pVideoTemp) + modes.ModeInformationLength);
    }

    return modes.NumModes;
}
