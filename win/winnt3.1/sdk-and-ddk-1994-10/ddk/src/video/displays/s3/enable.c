/******************************Module*Header*******************************\
* Module Name: enable.c
*
* This module contains the functions that enable and disable the
* driver, the pdev, and the surface.
*
* Copyright (c) 1992 Microsoft Corporation
\**************************************************************************/

#include "driver.h"

#define GDI_MODE            1
#define FULL_SCREEN_MODE    2

ULONG   gLastState = GDI_MODE;

#if DBG

ULONG   gCount = 1;

DHPDEV WDrvEnablePDEV(
    DEVMODEW   *pDevmode,       // Pointer to DEVMODE
    PWSTR       pwszLogAddress, // Logical address
    ULONG       cPatterns,      // number of patterns
    HSURF      *ahsurfPatterns, // return standard patterns
    ULONG       cjGdiInfo,      // Length of memory pointed to by pGdiInfo
    ULONG      *pGdiInfo,       // Pointer to GdiInfo structure
    ULONG       cjDevInfo,      // Length of following PDEVINFO structure
    DEVINFO    *pDevInfo,       // physical device information structure
    PWSTR       pwszDataFile,   // DataFile - not used
    PWSTR       pwszDeviceName, // DeviceName - not used
    HANDLE      hDriver)        // Handle to base driver
{
    DHPDEV bRet;

    LOGDBG((1, "DrvEnablePDEV\n"));

    if (InterlockedDecrement(&gCount) != 0)
        RIP("Somebody else is in here\n");

    bRet = DrvEnablePDEV(
                pDevmode,
                pwszLogAddress,
                cPatterns,
                ahsurfPatterns,
                cjGdiInfo,
                pGdiInfo,
                cjDevInfo,
                pDevInfo,
                pwszDataFile,
                pwszDeviceName,
                hDriver);

    if (InterlockedIncrement(&gCount) <= 0)
        RIP("Can't leave\n");

    LOGDBG((5, "DrvEnablePDEV done\n"));

    return(bRet);
}

VOID WDrvCompletePDEV(
    DHPDEV dhpdev,
    HDEV  hdev)
{
    LOGDBG((1, "DrvCompletePDEV\n"));

    if (InterlockedDecrement(&gCount) != 0)
        RIP("Somebody else is in here\n");

    DrvCompletePDEV(
                dhpdev,
                hdev);

    if (InterlockedIncrement(&gCount) <= 0)
        RIP("Can't leave\n");

    LOGDBG((5, "DrvCompletePDEV done\n"));
}

VOID WDrvDisablePDEV(DHPDEV dhpdev)
{
    LOGDBG((1, "DrvDisable\n"));

    DrvDisablePDEV(dhpdev);

    LOGDBG((5, "DrvDisable done\n"));
}

HSURF WDrvEnableSurface(DHPDEV dhpdev)
{
    HSURF h;

    LOGDBG((1, "DrvEnableSurface\n"));

    if (InterlockedDecrement(&gCount) != 0)
        RIP("Somebody else is in here\n");

    h = DrvEnableSurface(dhpdev);

    if (InterlockedIncrement(&gCount) <= 0)
        RIP("Can't leave\n");

    LOGDBG((5, "DrvEnableSurface done\n"));

    return(h);
}

VOID WDrvDisableSurface(DHPDEV dhpdev)
{
    LOGDBG((1, "DrvDisableSurface\n"));

    DrvDisableSurface(dhpdev);

    LOGDBG((5, "DrvDisableSurface done\n"));
}


VOID  WDrvAssertMode(
DHPDEV dhpdev,
BOOL   bEnable)
{
    LOGDBG((1, "DrvAssertMode\n"));

    if (bEnable)
    {
        // When the driver is active we'll always expect gCount to be
        // one at the beginning of any driver call:

        if (InterlockedIncrement(&gCount) <= 0)
            RIP("Somebody else is in here\n");
    }
    else
    {
        if (InterlockedDecrement(&gCount) != 0)
            RIP("Somebody else is in here\n");
    }

    DrvAssertMode(dhpdev,bEnable);

    LOGDBG((5, "DrvAssertMode done\n"));
}


VOID WDrvMovePointer(SURFOBJ *pso,LONG x,LONG y,RECTL *prcl)
{
    LOGDBG((1, "DrvMovePointer\n"));

    if (InterlockedDecrement(&gCount) != 0)
        RIP("Somebody else is in here\n");

    DrvMovePointer(pso,x,y,prcl);

    if (InterlockedIncrement(&gCount) <= 0)
        RIP("Can't leave\n");

    LOGDBG((5, "DrvMovePointer done\n"));
}

ULONG WDrvSetPointerShape(
SURFOBJ  *pso,
SURFOBJ  *psoMask,
SURFOBJ  *psoColor,
XLATEOBJ *pxlo,
LONG      xHot,
LONG      yHot,
LONG      x,
LONG      y,
RECTL    *prcl,
FLONG     fl)
{
    ULONG u;

    LOGDBG((1, "DrvSetPointerShape\n"));

    if (InterlockedDecrement(&gCount) != 0)
        RIP("Somebody else is in here\n");

    u = DrvSetPointerShape(
                pso,
                psoMask,
                psoColor,
                pxlo,
                xHot,
                yHot,
                x,
                y,
                prcl,
                fl);

    if (InterlockedIncrement(&gCount) <= 0)
        RIP("Can't leave\n");

    LOGDBG((5, "DrvSetPointerShape done\n"));

    return(u);
}

ULONG WDrvDitherColor(
DHPDEV dhpdev,
ULONG  iMode,
ULONG  rgb,
ULONG *pul)
{
    ULONG u;

    LOGDBG((1, "DrvDitherColor\n"));

    u = DrvDitherColor(
                dhpdev,
                iMode,
                rgb,
                pul);

    LOGDBG((5, "DrvDitherColor done\n"));

    return(u);
}


BOOL WDrvSetPalette(
DHPDEV  dhpdev,
PALOBJ *ppalo,
FLONG   fl,
ULONG   iStart,
ULONG   cColors)
{
    BOOL u;

    LOGDBG((1, "DrvSetPalette\n"));

    if (InterlockedDecrement(&gCount) != 0)
        RIP("Somebody else is in here\n");

    u = DrvSetPalette(
                dhpdev,
                ppalo,
                fl,
                iStart,
                cColors);


    if (InterlockedIncrement(&gCount) <= 0)
        RIP("Can't leave\n");

    LOGDBG((5, "DrvSetPalette done\n"));

    return(u);
}

BOOL WDrvCopyBits(
SURFOBJ  *psoDest,
SURFOBJ  *psoSrc,
CLIPOBJ  *pco,
XLATEOBJ *pxlo,
RECTL    *prclDest,
POINTL   *pptlSrc)
{
    BOOL u;

    LOGDBG((1, "DrvCopyBits\n"));

    if (InterlockedDecrement(&gCount) != 0)
        RIP("Somebody else is in here\n");

    u = DrvCopyBits(
                psoDest,
                psoSrc,
                pco,
                pxlo,
                prclDest,
                pptlSrc);

    if (InterlockedIncrement(&gCount) <= 0)
        RIP("Can't leave\n");

    LOGDBG((5, "DrvCopyBits done\n"));

    return(u);
}


BOOL WDrvBitBlt(
SURFOBJ  *psoTrg,
SURFOBJ  *psoSrc,
SURFOBJ  *psoMask,
CLIPOBJ  *pco,
XLATEOBJ *pxlo,
RECTL    *prclTrg,
POINTL   *pptlSrc,
POINTL   *pptlMask,
BRUSHOBJ *pbo,
POINTL   *pptlBrush,
ROP4      rop4)
{
    BOOL u;

    LOGDBG((1, "DrvBitBlt\n"));

    if (InterlockedDecrement(&gCount) != 0)
        RIP("Somebody else is in here\n");

    u = DrvBitBlt(
                psoTrg,
                psoSrc,
                psoMask,
                pco,
                pxlo,
                prclTrg,
                pptlSrc,
                pptlMask,
                pbo,
                pptlBrush,
                rop4);

    if (InterlockedIncrement(&gCount) <= 0)
        RIP("Can't leave\n");

    LOGDBG((5, "DrvBitBlt done\n"));

    return(u);
}

BOOL WDrvTextOut(
SURFOBJ  *pso,
STROBJ   *pstro,
FONTOBJ  *pfo,
CLIPOBJ  *pco,
RECTL    *prclExtra,
RECTL    *prclOpaque,
BRUSHOBJ *pboFore,
BRUSHOBJ *pboOpaque,
POINTL   *pptlOrg,
MIX       mix)
{
    BOOL u;

    LOGDBG((1, "DrvTextOut\n"));

    if (InterlockedDecrement(&gCount) != 0)
        RIP("Somebody else is in here\n");

    u = DrvTextOut(
                pso,
                pstro,
                pfo,
                pco,
                prclExtra,
                prclOpaque,
                pboFore,
                pboOpaque,
                pptlOrg,
                mix);

    if (InterlockedIncrement(&gCount) <= 0)
        RIP("Can't leave\n");

    LOGDBG((5, "DrvTextOut done\n"));

    return(u);
}

ULONG WDrvGetModes(
HANDLE    hDriver,
ULONG     cjSize,
DEVMODEW *pdm)
{
    ULONG u;

    LOGDBG((1, "DrvGetModes\n"));

    if (InterlockedDecrement(&gCount) != 0)
        RIP("Somebody else is in here\n");

    u = DrvGetModes(
                hDriver,
                cjSize,
                pdm);

    if (InterlockedIncrement(&gCount) <= 0)
        RIP("Can't leave\n");

    LOGDBG((5, "DrvGetModes done\n"));

    return(u);
}

BOOL WDrvStrokePath(
SURFOBJ   *pso,
PATHOBJ   *ppo,
CLIPOBJ   *pco,
XFORMOBJ  *pxo,
BRUSHOBJ  *pbo,
POINTL    *pptlBrushOrg,
LINEATTRS *plineattrs,
MIX        mix)
{
    BOOL u;

    LOGDBG((1, "DrvStrokePath\n"));

    if (InterlockedDecrement(&gCount) != 0)
        RIP("Somebody else is in here\n");

    u = DrvStrokePath(
                pso,
                ppo,
                pco,
                pxo,
                pbo,
                pptlBrushOrg,
                plineattrs,
                mix);

    if (InterlockedIncrement(&gCount) <= 0)
        RIP("Can't leave\n");

    LOGDBG((5, "DrvStrokePath done\n"));

    return(u);
}


BOOL WDrvFillPath(
SURFOBJ  *pso,
PATHOBJ  *ppo,
CLIPOBJ  *pco,
BRUSHOBJ *pbo,
POINTL   *pptlBrushOrg,
MIX       mix,
FLONG     flOptions)
{
    BOOL u;

    LOGDBG((1, "DrvFillPath\n"));

    if (InterlockedDecrement(&gCount) != 0)
        RIP("Somebody else is in here\n");

    u = DrvFillPath(pso,
                ppo,
                pco,
                pbo,
                pptlBrushOrg,
                mix,
                flOptions);

    if (InterlockedIncrement(&gCount) <= 0)
        RIP("Can't leave\n");

    LOGDBG((5, "DrvFillPath done\n"));

    return(u);
}

BOOL WDrvPaint(
SURFOBJ  *pso,
CLIPOBJ  *pco,
BRUSHOBJ *pbo,
POINTL   *pptlBrushOrg,
MIX       mix)
{
    BOOL u;

    LOGDBG((1, "DrvPaint\n"));

    if (InterlockedDecrement(&gCount) != 0)
        RIP("Somebody else is in here\n");

    u = DrvPaint(
                pso,
                pco,
                pbo,
                pptlBrushOrg,
                mix);

    if (InterlockedIncrement(&gCount) <= 0)
        RIP("Can't leave\n");

    LOGDBG((5, "DrvPaint done\n"));

    return(u);
}

BOOL WDrvRealizeBrush(
BRUSHOBJ *pbo,
SURFOBJ  *psoTarget,
SURFOBJ  *psoPattern,
SURFOBJ  *psoMask,
XLATEOBJ *pxlo,
ULONG    iHatch)
{
    BOOL u;

    LOGDBG((1, "DrvRealizeBrush\n"));

    if (gCount != 0)
        RIP("DrvRealizeBrush Somebody else is in here\n");

    u = DrvRealizeBrush(
                pbo,
                psoTarget,
                psoPattern,
                psoMask,
                pxlo,
                iHatch);

    LOGDBG((5, "DrvRealizeBrush done\n"));

    return(u);
}

ULONG WDrvSaveScreenBits(SURFOBJ *pso,ULONG iMode,ULONG ident,RECTL *prcl)
{
    ULONG u;

    LOGDBG((1, "DrvSaveScreenBits\n"));

    if (InterlockedDecrement(&gCount) != 0)
        RIP("Somebody else is in here\n");

    u = DrvSaveScreenBits(pso,iMode,ident,prcl);

    if (InterlockedIncrement(&gCount) <= 0)
        RIP("Can't leave\n");

    LOGDBG((5, "DrvSaveScreenBits done\n"));

    return(u);
}

VOID WDrvDestroyFont(FONTOBJ *pfo)
{
    LOGDBG((1, "DrvDestroyFont\n"));

// !!!    if (InterlockedDecrement(&gCount) != 0)
// !!!        RIP("Somebody else is in here\n");

    DrvDestroyFont(pfo);

// !!!    if (InterlockedIncrement(&gCount) <= 0)
// !!!        RIP("Can't leave\n");

    LOGDBG((5, "DrvDestroyFont done\n"));
}

//
// On a checked build, we thunk all driver calls for debugging purposes:
//

DRVFN gadrvfn[] = {
    {   INDEX_DrvEnablePDEV,            (PFN) WDrvEnablePDEV         },
    {   INDEX_DrvCompletePDEV,          (PFN) WDrvCompletePDEV       },
    {   INDEX_DrvDisablePDEV,           (PFN) WDrvDisablePDEV        },
    {   INDEX_DrvEnableSurface,         (PFN) WDrvEnableSurface      },
    {   INDEX_DrvDisableSurface,        (PFN) WDrvDisableSurface     },
    {   INDEX_DrvAssertMode,            (PFN) WDrvAssertMode         },
    {   INDEX_DrvMovePointer,           (PFN) WDrvMovePointer        },
    {   INDEX_DrvSetPointerShape,       (PFN) WDrvSetPointerShape    },
    {   INDEX_DrvDitherColor,           (PFN) WDrvDitherColor        },
    {   INDEX_DrvSetPalette,            (PFN) WDrvSetPalette         },
    {   INDEX_DrvCopyBits,              (PFN) WDrvCopyBits           },
    {   INDEX_DrvBitBlt,                (PFN) WDrvBitBlt             },
    {   INDEX_DrvTextOut,               (PFN) WDrvTextOut            },
    {   INDEX_DrvGetModes,              (PFN) WDrvGetModes           },
    {   INDEX_DrvStrokePath,            (PFN) WDrvStrokePath         },
    {   INDEX_DrvFillPath,              (PFN) WDrvFillPath           },
    {   INDEX_DrvPaint,                 (PFN) WDrvPaint              },
    {   INDEX_DrvRealizeBrush,          (PFN) WDrvRealizeBrush       },
    {   INDEX_DrvSaveScreenBits,        (PFN) WDrvSaveScreenBits     },
    {   INDEX_DrvDestroyFont,           (PFN) WDrvDestroyFont        }
};

#else

//
// Build the driver function table gadrvfn with function index/address pairs
//

DRVFN gadrvfn[] = {
    {   INDEX_DrvEnablePDEV,            (PFN) DrvEnablePDEV         },
    {   INDEX_DrvCompletePDEV,          (PFN) DrvCompletePDEV       },
    {   INDEX_DrvDisablePDEV,           (PFN) DrvDisablePDEV        },
    {   INDEX_DrvEnableSurface,         (PFN) DrvEnableSurface      },
    {   INDEX_DrvDisableSurface,        (PFN) DrvDisableSurface     },
    {   INDEX_DrvAssertMode,            (PFN) DrvAssertMode         },
    {   INDEX_DrvMovePointer,           (PFN) DrvMovePointer        },
    {   INDEX_DrvSetPointerShape,       (PFN) DrvSetPointerShape    },
    {   INDEX_DrvDitherColor,           (PFN) DrvDitherColor        },
    {   INDEX_DrvSetPalette,            (PFN) DrvSetPalette         },
    {   INDEX_DrvCopyBits,              (PFN) DrvCopyBits           },
    {   INDEX_DrvBitBlt,                (PFN) DrvBitBlt             },
    {   INDEX_DrvTextOut,               (PFN) DrvTextOut            },
    {   INDEX_DrvGetModes,              (PFN) DrvGetModes           },
    {   INDEX_DrvStrokePath,            (PFN) DrvStrokePath         },
    {   INDEX_DrvFillPath,              (PFN) DrvFillPath           },
    {   INDEX_DrvPaint,                 (PFN) DrvPaint              },
    {   INDEX_DrvRealizeBrush,          (PFN) DrvRealizeBrush       },
    {   INDEX_DrvSaveScreenBits,        (PFN) DrvSaveScreenBits     },
    {   INDEX_DrvDestroyFont,           (PFN) DrvDestroyFont        }
};
#endif

/******************************Public*Routine******************************\
* DrvEnableDriver
*
* Enables the driver by retrieving the drivers function table and version.
*
\**************************************************************************/

BOOL DrvEnableDriver(
ULONG iEngineVersion,
ULONG cj,
PDRVENABLEDATA pded)
{
    UNREFERENCED_PARAMETER(iEngineVersion);

    DISPDBG((2, "*** S3.DLL!DrvEnableDriver - Entry ***\n"));

// Engine Version is passed down so future drivers can support previous
// engine versions.  A next generation driver can support both the old
// and new engine conventions if told what version of engine it is
// working with.  For the first version the driver does nothing with it.

    DISPDBG((2, "S3.DLL!DrvEnableDriver - iEngineVersion: %ld\n", iEngineVersion));

// Fill in as much as we can.

    if (cj >= sizeof(DRVENABLEDATA))
        pded->pdrvfn = gadrvfn;

    if (cj >= (sizeof(ULONG) * 2))
        pded->c = sizeof(gadrvfn) / sizeof(DRVFN);

// DDI version this driver was targeted for is passed back to engine.
// Future graphic's engine may break calls down to old driver format.

    if (cj >= sizeof(ULONG))
        pded->iDriverVersion = DDI_DRIVER_VERSION;

    return(TRUE);
}

/******************************Public*Routine******************************\
* DrvDisableDriver
*
* Tells the driver it is being disabled. Release any resources allocated in
* DrvEnableDriver.
*
\**************************************************************************/

VOID DrvDisableDriver(VOID)
{
    return;
}

/******************************Public*Routine******************************\
* DrvEnablePDEV
*
* DDI function, Enables the Physical Device.
*
* Return Value: device handle to pdev.
*
\**************************************************************************/

DHPDEV DrvEnablePDEV(
    DEVMODEW   *pDevmode,       // Pointer to DEVMODE
    PWSTR       pwszLogAddress, // Logical address
    ULONG       cPatterns,      // number of patterns
    HSURF      *ahsurfPatterns, // return standard patterns
    ULONG       cjGdiInfo,      // Length of memory pointed to by pGdiInfo
    ULONG      *pGdiInfo,       // Pointer to GdiInfo structure
    ULONG       cjDevInfo,      // Length of following PDEVINFO structure
    DEVINFO    *pDevInfo,       // physical device information structure
    PWSTR       pwszDataFile,   // DataFile - not used
    PWSTR       pwszDeviceName, // DeviceName - not used
    HANDLE      hDriver)        // Handle to base driver
{
    GDIINFO GdiInfo;
    DEVINFO DevInfo;
    PPDEV   ppdev = (PPDEV) NULL;

    UNREFERENCED_PARAMETER(pwszLogAddress);
    UNREFERENCED_PARAMETER(pwszDataFile);
    UNREFERENCED_PARAMETER(pwszDeviceName);

    // Allocate a physical device structure.

    ppdev = (PPDEV) LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(PDEV));

    if (ppdev == (PPDEV) NULL)
    {
        DISPDBG((0, "DISP DrvEnablePDEV failed LocalAlloc\n"));
        EngSetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto error0;
    }

    // Set up pointers in PDEV to temporary structures we build up to return.

    ppdev->pGdiInfo = &GdiInfo;
    ppdev->pDevInfo = &DevInfo;

    // Save the screen handle in the PDEV.

    ppdev->hDriver = hDriver;

    // Get the current screen mode information.  Set up device caps and devinfo.

    if (!bInitPDEV(ppdev, pDevmode))
    {
        DISPDBG((0, "S3 DrvEnablePDEV failed bGetScreenInfo\n"));
        goto error1;
    }

    // Initialize palette information.

    if (!bInitPaletteInfo(ppdev))
    {
        DISPDBG((0, "DISP DrvEnableSurface failed bInitPalette\n"));
        goto error1;
    }

    // Initialize device standard patterns.

    if (!bInitPatterns(ppdev, min(cPatterns, HS_DDI_MAX)))
    {
        DISPDBG((0, "DISP DrvEnablePDEV failed bInitPatterns\n"));
        goto error2;
    }

    // Copy the devinfo into the engine buffer.

    memcpy(pDevInfo, ppdev->pDevInfo, min(sizeof(DEVINFO), cjDevInfo));

    // Set the ahsurfPatterns array to handles each of the standard
    // patterns that were just created.

    memcpy((PVOID) ahsurfPatterns, ppdev->ahbmPat, ppdev->cPatterns*sizeof(HBITMAP));

    // Set the pdevCaps with GdiInfo we have prepared to the list of caps for this
    // pdev.

    memcpy(pGdiInfo, ppdev->pGdiInfo, min(cjGdiInfo, sizeof(GDIINFO)));

    // Set NULL into pointers for stack allocated memory.

    ppdev->pGdiInfo = (GDIINFO *) NULL;
    ppdev->pDevInfo = (DEVINFO *) NULL;

    return((DHPDEV) ppdev);

    // Error case for failure.

error2:
    vDisablePatterns(ppdev);
    vDisablePalette(ppdev);

error1:
    LocalFree(ppdev);

error0:

    return((DHPDEV) 0);
}

/******************************Public*Routine******************************\
* DrvCompletePDEV
*
* Store the HPDEV, the engines handle for this PDEV, in the DHPDEV.
*
\**************************************************************************/

VOID DrvCompletePDEV(
    DHPDEV dhpdev,
    HDEV  hdev)
{
    ((PPDEV) dhpdev)->hdevEng = hdev;
}

/******************************Public*Routine******************************\
* DrvDisablePDEV
*
* Release the resources allocated in DrvEnablePDEV.  If a surface has been
* enabled DrvDisableSurface will have already been called.
*
\**************************************************************************/

VOID DrvDisablePDEV(
    DHPDEV dhpdev)
{
    vDisablePalette((PPDEV) dhpdev);
    vDisablePatterns((PPDEV) dhpdev);
    LocalFree(dhpdev);
}

/******************************Public*Routine******************************\
* DrvEnableSurface
*
* Enable the surface for the device.  Hook the calls this driver supports.
*
* Return: Handle to the surface if successful, 0 for failure.
*
\**************************************************************************/

HSURF DrvEnableSurface(
    DHPDEV dhpdev)
{
    PPDEV ppdev;
    HSURF hsurf,
          hsurfBm;
    SIZEL sizl;
    ULONG ulBitmapType;
    FLONG flHooks;
    BOOL  bRet;

    // Create engine bitmap around frame buffer.

    ppdev = (PPDEV) dhpdev;

    if (!bInitSURF(ppdev, TRUE))
    {
        DISPDBG((0, "DISP DrvEnableSurface failed bInitSURF\n"));
        return(FALSE);
    }

    if (ppdev->ulBitCount == 8)  {
        if (!bInit256ColorPalette(ppdev)) {
            DISPDBG((0, "DISP DrvEnableSurface failed to init the 8bpp palette\n"));
            return(FALSE);
        }
    }

    sizl.cx = ppdev->cxScreen;
    sizl.cy = ppdev->cyScreen;

    // Init the bank manager for the punt system.

    bRet = bBankInit(ppdev, TRUE);
    if (bRet == FALSE)
    {
        return (NULL);
    }

    // Set all the hooks and create our bitmaps

    if (ppdev->ulBitCount == 8)
    {
        ulBitmapType = BMF_8BPP;
        flHooks = HOOKS_BMF8BPP;
    }
    else if (ppdev->ulBitCount == 16)
    {
        ulBitmapType = BMF_16BPP;
        flHooks = HOOKS_BMF16BPP;
    }
    else
    {
        DISPDBG((0,"S3.DLL: ppdev->ulBitCount is invalid\n"));
    }

    hsurfBm = (HSURF) EngCreateBitmap(sizl,
                                      (ULONG) (ppdev->lDeltaScreen),
                                      (ULONG) (ulBitmapType),
                                      (FLONG) (((ppdev->lDeltaScreen > 0) ? BMF_TOPDOWN : 0)),
                                      (PVOID) (ppdev->pjScreen));

    if ((hsurfBm == 0) || (!EngAssociateSurface(hsurfBm, ppdev->hdevEng, 0)))
    {
        DISPDBG((0, "S3.DLL!DrvEnableSurface - failed EngAssociateSurface bitmap\n"));
        EngDeleteSurface(hsurfBm);
        return(NULL);
    }

    ppdev->hsurfBm = hsurfBm;

    // Note: the engine lock cannot fail, so no error code is checked.

    ppdev->pSurfObj = EngLockSurface(hsurfBm);

    // BUGBUG Check return code.

    sizl.cy = S3BM_HEIGHT;

    hsurf = EngCreateSurface((DHSURF) ppdev, sizl);
    if (hsurf == NULL)
    {
        DISPDBG((0, "S3.DLL!DrvEnableSurface - failed EngCreateSurface bitmap\n"));
        EngDeleteSurface(hsurfBm);
        return (NULL);
    }

    if (!EngAssociateSurface(hsurf, ppdev->hdevEng, flHooks))
    {
        DISPDBG((0, "S3.DLL!DrvEnableSurface - failed EngAssociateSurface\n"));
        EngDeleteSurface(hsurfBm);
        EngDeleteSurface(hsurf);
        return(NULL);
    }

    ppdev->hsurfEng = hsurf;

    // Create a temporary bitmap for the screen to screen punt blits.
    // First, create a surface.

    sizl.cx = ppdev->cxMaxRam;

#if defined(_X86_) || defined(i386)

    ppdev->psoTemp = NULL;

#else

    sizl.cy = ppdev->cyScreen;

    hsurfBm = (HSURF) EngCreateBitmap(sizl,
                                      (ULONG) (ppdev->lDeltaScreen),
                                      BMF_8BPP,
                                      BMF_TOPDOWN,
                                      NULL);
    if (hsurfBm == NULL)
    {
        DISPDBG((0, "S3.DLL!DrvEnableSurface - failed EngCreateBitmap\n"));
        EngDeleteSurface(hsurfBm);
        EngDeleteSurface(hsurf);
        return(NULL);
    }

    // Get a pointer to the surface object.

    ppdev->psoTemp = EngLockSurface(hsurfBm);

#endif

    // These values are here for the strip drawer, they should be removed.

    ppdev->iFormat   = BMF_PHYSDEVICE;
    ppdev->lNextScan = S3BM_WIDTH;

    // Set the shadow clip values.

    ppdev->ClipRight  = 0;
    ppdev->ClipLeft   = 0;
    ppdev->ClipTop    = 0;
    ppdev->ClipBottom = 0;

    vResetS3Clipping(ppdev);

    return(hsurf);
}

/******************************Public*Routine******************************\
* DrvDisableSurface
*
* Free resources allocated by DrvEnableSurface.  Release the surface.
*
\**************************************************************************/

VOID DrvDisableSurface(
    DHPDEV dhpdev)
{
    //!!! check and return error.  unlock and delete hsurfBM see XGA
    EngDeleteSurface(((PPDEV) dhpdev)->hsurfEng);
    vDisableSURF((PPDEV) dhpdev);
    ((PPDEV) dhpdev)->hsurfEng = (HSURF) 0;
}

/******************************Public*Routine******************************\
* DrvAssertMode
*
* This asks the device to reset itself to the mode of the pdev passed in.
*
\**************************************************************************/

VOID DrvAssertMode(
    DHPDEV dhpdev,
    BOOL bEnable)
{
    PPDEV   ppdev = (PPDEV) dhpdev;
    ULONG   ulReturn;

#if CATCHIT

    if (gLastState == GDI_MODE)
    {
        if (bEnable == TRUE)
        {
            RIP("S3.DLL!DrvAssertMode - Told to go into GDI mode while in GDI mode\n");
            return;
        }
    }
    else if (gLastState == FULL_SCREEN_MODE)
    {
        if (bEnable == FALSE)
        {
            RIP("S3.DLL!DrvAssertMode - Told to go into Full Screen mode while in Full Screen mode\n");
            return;
        }
    }
    else
    {
        RIP("S3.DLL!DrvAssertMode - Undefine mode\n");
        return;
    }

#endif

    if (bEnable)
    {
        gLastState = GDI_MODE;

        // The screen must be reenabled, reinitialize the device to
        // a clean state.

        bInitSURF(ppdev, FALSE);
        bBankInit(ppdev, FALSE);

        // Restore the off screen data.  This protects the Desktop
        // from an DOS application that might trash the off screen
        // memory.

        ppdev->ClipRight = -1;
        ppdev->ClipLeft  = -1;
        ppdev->ClipTop   = -1;
        ppdev->ClipBottom = -1;
        vResetS3Clipping(ppdev);

        vRestoreOffScreenMemory(ppdev);
    }
    else
    {
        gLastState = FULL_SCREEN_MODE;

        // We must give up the display.

        ppdev->ClipRight = -1;
        ppdev->ClipLeft  = -1;
        ppdev->ClipTop   = -1;
        ppdev->ClipBottom = -1;
        vResetS3Clipping(ppdev);
        vSaveOffScreenMemory(ppdev);

        // Call the kernel driver to reset the device to a known state.

        if (!DeviceIoControl(ppdev->hDriver,
                             IOCTL_VIDEO_RESET_DEVICE,
                             NULL,
                             0,
                             NULL,
                             0,
                             &ulReturn,
                             NULL))
        {
            DISPDBG((0, "S3.DLL!DrvAssertMode - DrvAssertMode failed IOCTL"));
        }

    }

    return;
}


/******************************Public*Routine******************************\
* DrvGetModes
*
* Returns the list of available modes for the device.
*
\**************************************************************************/

ULONG DrvGetModes(
HANDLE hDriver,
ULONG cjSize,
DEVMODEW *pdm)

{

    DWORD cModes;
    DWORD cbOutputSize;
    PVIDEO_MODE_INFORMATION pVideoModeInformation, pVideoTemp;
    DWORD cOutputModes = cjSize / (sizeof(DEVMODEW) + DRIVER_EXTRA_SIZE);
    DWORD cbModeSize;

    DISPDBG((3, "S3.dll:DrvGetModes\n"));

    cModes = getAvailableModes(hDriver,
                               (PVIDEO_MODE_INFORMATION *) &pVideoModeInformation,
                                &cbModeSize);


    if (cModes == 0)
    {
        DISPDBG((0, "S3 DISP DrvGetModes failed to get mode information"));
        return 0;
    }

    if (pdm == NULL)
    {
        cbOutputSize = cModes * (sizeof(DEVMODEW) + DRIVER_EXTRA_SIZE);
    }
    else
    {
        //
        // Now copy the information for the supported modes back into the output
        // buffer
        //

        cbOutputSize = 0;

        pVideoTemp = pVideoModeInformation;

        do
        {
            if (pVideoTemp->Length != 0)
            {
                if (cOutputModes == 0)
                {
                    break;
                }

                //
                // Zero the entire structure to start off with.
                //

                memset(pdm, 0, sizeof(DEVMODEW));

                //
                // Set the name of the device to the name of the DLL.
                //

                memcpy(&(pdm->dmDeviceName), L"s3", sizeof(L"s3"));

                pdm->dmSpecVersion = DM_SPECVERSION;
                pdm->dmDriverVersion = DM_SPECVERSION;

                //
                // We currently do not support Extra information in the driver
                //

                pdm->dmDriverExtra = DRIVER_EXTRA_SIZE;

                pdm->dmSize = sizeof(DEVMODEW);
                pdm->dmBitsPerPel = pVideoTemp->NumberOfPlanes *
                                    pVideoTemp->BitsPerPlane;
                pdm->dmPelsWidth = pVideoTemp->VisScreenWidth;
                pdm->dmPelsHeight = pVideoTemp->VisScreenHeight;
                pdm->dmDisplayFrequency = pVideoTemp->Frequency;

                if (pVideoTemp->AttributeFlags & VIDEO_MODE_INTERLACED)
                {
                    pdm->dmDisplayFlags |= DM_INTERLACED;
                }

                //
                // Go to the next DEVMODE entry in the buffer.
                //

                cOutputModes--;

                pdm = (LPDEVMODEW) ( ((ULONG)pdm) + sizeof(DEVMODEW) +
                                                   DRIVER_EXTRA_SIZE);

                cbOutputSize += (sizeof(DEVMODEW) + DRIVER_EXTRA_SIZE);

            }

            pVideoTemp = (PVIDEO_MODE_INFORMATION)
                (((PUCHAR)pVideoTemp) + cbModeSize);


        } while (--cModes);
    }

    LocalFree(pVideoModeInformation);

    return cbOutputSize;
}
