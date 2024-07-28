/****************************************************************************
 *
 *   ctdev.c
 * 
 *   Hardware specific routines.  
 *
 *   Microsoft Video for Windows Sample Capture Driver
 *   Chips & Technologies 9001 based frame grabbers.
 *
 ***************************************************************************/
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/

#include <windows.h>
#include <mmsystem.h>
#include <conio.h>
#include <msvideo.h>
#include <msviddrv.h>
#include "ct.h"         // General include
#include "ctdev.h"      // Device specific include
#include "debug.h"      

#ifdef _VBLASTER
#include "pcvideo.h"
char gszDriverName[]              = "VBlaster.drv";
#endif

#ifdef _BRAVADO
#include "vwproto.h"
VWCFG VWConfigBuf;
char gszDriverName[]              = "Bravado.drv";
#endif

#define KEY_COLOR_INDEX  5

WORD FAR PASCAL GetProfileHex(LPSTR szApp, LPSTR szEntry, WORD wDef);

static HBRUSH   hOverlayBrush;      // Overlay brush used to paint window
static RECT     rcLastScreen;       // Last screen coords of overlay window
static BOOL     fOverlayEnabled;    // Is overlay showing?

/*======================================================== 
             Low level read/write routines               
=========================================================*/

// Write a byte to a PCVideo Chip register
void FAR PASCAL CT_WritePCVideo (int nIndex, int nValue)
{
    outp (wPCVideoAddress, nIndex);
    outp (wPCVideoAddress + 1, nValue);
}

// Read a byte from a PC Video chip register
int FAR PASCAL CT_ReadPCVideo (int nIndex)
{
    outp (wPCVideoAddress, nIndex);
    return (inp (wPCVideoAddress + 1));
}


/*======================================================== 
             Initialization and Fini
=========================================================*/

// Initialization routine called only once at driver load time.
// Returns TRUE on success
BOOL FAR PASCAL CT_Init ()
{
    WORD wRet;
    WORD wLuma;

#pragma message("Try reducing luma bandwidth!!!")
    wLuma = GetProfileHex(gszDriverName, "9051Reg6", 0x6c );

#ifdef _BRAVADO
    VW_SetConfigBuf (&VWConfigBuf);
    VW_SetDefaultConfiguration (0);
    CT_SetPortAddress (wPCVideoAddress); 

    wRet = (VW_Init (0) == 1);  // returns 1 on success
    VW_Set9051Reg (6, wLuma);    // Lower luma bandwidth to reduce noise!!!
#endif

#ifdef _VBLASTER
    wRet = vbcInitialize (); 
    vbcSetRegister (0x8a06, (BYTE) wLuma);
#endif

    if (wRet) {
        wPCVideoAddress = CT_GetPortAddress ();

        // Create the overlay brush
        hOverlayBrush = CT_SetKeyColor ();
        CT_OverlayEnable (FALSE);   // Initially disable the overlay
        CT_Acquire (FALSE);         // Turn off acquisition so we can test memory
    }
    return wRet;
}

// Called during shutdown of the driver.
void FAR PASCAL CT_Fini ()
{
    if (hOverlayBrush) {
        DeleteObject (hOverlayBrush);
        hOverlayBrush = NULL;
    }

#ifdef _BRAVADO
    VW_CleanUp ();
#endif

#ifdef _VBLASTER
    vbcExit (); 
#endif
}

/*======================================================== 
                Ports and addresses
=========================================================*/

// Set the Base I/O port used by the device
void FAR PASCAL CT_SetPortAddress (int nPort)
{
#ifdef _BRAVADO
    VW_SetIOAddr (nPort);
#endif

#ifdef _VBLASTER
    PCV_SetPortAddress (nPort);   // This doesn't work!!!
#endif

    wPCVideoAddress = CT_GetPortAddress ();
}

// Get the Base I/O port used by the device
int FAR PASCAL CT_GetPortAddress ()
{
#ifdef _BRAVADO
    return VW_GetIOAddr ();
#endif

#ifdef _VBLASTER
    // VBlaster uses the address set in the Creative Labs setup app.
    return vbcGetPortAddress ();
#endif
}

// Get Linear memory address of frame buffer
// Returns value in the range 1 to 15 (meg)
int FAR PASCAL CT_GetFrameAddress ()
{
#ifdef _BRAVADO
    return VW_GetVidAddr ();
#endif

#ifdef _VBLASTER
    return vbcGetVideoAddress ();
#endif
}

// Set Linear memory address of frame buffer
// nVidAddr is linear address div. 0x100000 (ie. 1-15)
int FAR PASCAL CT_SetFrameAddress (int nVidAddr)
{
#ifdef _BRAVADO
    return VW_SetVidAddr (nVidAddr);
#endif

#ifdef _VBLASTER
    return vbcSetVideoAddress (nVidAddr);
#endif
}

/*======================================================== 
                    Configuration
=========================================================*/

// Load configuration from a file.
// Returns TRUE on success, FALSE on failure
BOOL FAR PASCAL CT_LoadConfiguration (LPSTR lpszFile)
{
#ifdef _BRAVADO
    return (VW_LoadConfiguration (lpszFile));
#endif

#ifdef _VBLASTER
    return vbcLoadConfiguration ();     // Does not support file name!!!
#endif
}

// Save configuration to a file.
// Returns TRUE on success, FALSE on failure
BOOL FAR PASCAL CT_SaveConfiguration (LPSTR lpszFile)
{
#ifdef _BRAVADO
    return (VW_SaveConfiguration (lpszFile));
#endif

#ifdef _VBLASTER
    return vbcSaveConfiguration ();     // Does not support file name!!!
#endif
}

// Set the video source
// nSource ranges from 0 to ...
void FAR PASCAL CT_SetVideoSource (int nSource)
{
#ifdef _BRAVADO
    VW_SetVidSource (nSource);
#endif

#ifdef _VBLASTER
    vbcSetVideoSource ((WORD) nSource);
#endif
}

// Get the number of video input channels available.
int FAR PASCAL CT_GetVideoChannelCount (void)
{
#ifdef _BRAVADO
    return 3;   // Some cables only have 1 input, but always allow 3
#endif

#ifdef _VBLASTER
    return 3;
#endif
}

// Set the video standard
// 0 = NTSC, 1 = PAL
void FAR PASCAL CT_SetVideoStandard (int nStandard)
{
#ifdef _BRAVADO
    VW_SetVidStandard (nStandard == 0 ? 1 : 0);
#endif

#ifdef _VBLASTER
   vbcSetInputFormat (nStandard == 0 ? CF_NTSC : CF_PAL);
#endif
}

// Return TRUE if the device can accept SVideo
BOOL FAR PASCAL CT_HasSVideo ()
{
#ifdef _BRAVADO
    return TRUE;
#endif

#ifdef _VBLASTER
   return FALSE;
#endif
}

// Set the selected input channel format to:
// 0 = composite
// 1 = SVideo
// 2 = RGB (someday?)

BOOL FAR PASCAL CT_SetVideoCableFormat (int nInputMode)
{
#ifdef _BRAVADO
    VW_SetSVid (nInputMode);
    return TRUE;
#endif

#ifdef _VBLASTER
    return FALSE;       // Can't do SVideo
#endif
}

// Set the Color control registers 
// nVal on input ranges from 0 to 0x3f
void FAR PASCAL CT_SetColor(int nReg, int nVal)
{
#ifdef _BRAVADO
    // All Bravado values EXCEPT hue range 0-0x3f, hue goes from 0-0xff
    if (nReg == 0)
        nVal *= 4;      
    VW_SetColor (nReg, nVal);
#endif

#ifdef _VBLASTER
    // All Video Blaster color values range 0-0xff
    nVal *= 4; 
    switch (nReg) {
        case CT_COLOR_HUE:          nReg = 3; break;
        case CT_COLOR_BRIGHTNESS:   nReg = 0; break;
        case CT_COLOR_SAT:          nReg = 1; break;
        case CT_COLOR_CONTRAST:     nReg = 2; break;
    }
    vbcSetColor (nReg, (BYTE) nVal);
#endif
}


/*======================================================== 
                    Rectangles
=========================================================*/

// Create and Position the overlay window
void FAR PASCAL CT_SetDisplayRect (LPRECT lpRectangle)
{
    RECT rR = *lpRectangle;

    OffsetRect (&rR, rcLastScreen.left, rcLastScreen.top);

#ifdef _BRAVADO
    VW_SetVidWindow (rR.left, rR.top, WidthRect(rR), HeightRect(rR), 1);
#endif

#ifdef _VBLASTER
    vbcCreateWindow (rR.left, rR.top, WidthRect(rR), HeightRect(rR), 1);
#endif
}

void FAR PASCAL CT_SetPanAndScroll (LPRECT lpRectangle)
{
    RECT rR = *lpRectangle;
#ifdef _BRAVADO
    VW_SetVidPan (rR.left, rR.top);
#endif

#ifdef _VBLASTER
    vbcPanWindow (rR.left, rR.top);
#endif
        
}

/*======================================================== 
            Overlay keying and Painting
=========================================================*/

// Set the overlay key color
// Returns a brush which is used to paint the key color
HBRUSH FAR PASCAL CT_SetKeyColor ()
{
#ifdef _BRAVADO
    {
        HWND hWnd;
        HDC  hDC;
        int  iColors;

        hWnd = GetDesktopWindow ();
        hDC  = GetDC (hWnd);
        iColors = GetDeviceCaps (hDC, NUMCOLORS);
        ReleaseDC (hWnd, hDC);

        // Bravado16 only uses high byte for key, so create a special brush
        if (iColors > 256) {
            COLORREF cref = RGB (0x7f, 0, 0x7f);

            VW_SetRGBKeyColor (cref);
            return  (CreateSolidBrush (cref));
        }
        else {
            VW_SetKeyColor (KEY_COLOR_INDEX);
            return  (CreateSolidBrush (PALETTEINDEX (KEY_COLOR_INDEX)));
        }
        hDC  = GetDC (hWnd);
    }
#endif

#ifdef _VBLASTER
    vbcSetColorKey ((WORD) KEY_COLOR_INDEX);
    return  (CreateSolidBrush (PALETTEINDEX (KEY_COLOR_INDEX)));
#endif
}

// Turn overlay display on or off
void FAR PASCAL CT_OverlayEnable (BOOL fDisplay)
{
    fOverlayEnabled = fDisplay;

#ifdef _BRAVADO
    VW_SetVidShow (fDisplay);
#endif

#ifdef _VBLASTER
    if (fDisplay)
        vbcEnableVideo ();
    else
        vbcDisableVideo ();
#endif
}


// Update the overlay window rectangle and paint the key color
void FAR PASCAL CT_Update (HWND hWnd, HDC hDC)
{
    RECT rcScreen, rcClient;
    HBRUSH hbrOld;

    rcScreen = grcDestExtOut;
    rcScreen.right = rcScreen.left + gwWidth -1;
    rcScreen.bottom = rcScreen.top + gwHeight -1;

#ifdef _BRAVADO
    if (!EqualRect (&rcScreen, &rcLastScreen)) {
        VW_SetVidWindow (rcScreen.left, rcScreen.top, 
                WidthRect(rcScreen), HeightRect(rcScreen), 1);
    }
#endif

#ifdef _VBLASTER
    if (!EqualRect (&rcScreen, &rcLastScreen)) {
        vbcSetWindowPosition (rcScreen.left, rcScreen.top);
        vbcSetWindowSize (WidthRect(rcScreen), HeightRect(rcScreen), 1);
    }
#endif

    CT_SetPanAndScroll (&grcSourceExtOut);

    // Paint the key color into grcDestExtOut (which is in screen coords)
    rcClient = grcDestExtOut;
    ScreenToClient (hWnd, (LPPOINT) &rcClient);
    ScreenToClient (hWnd, (LPPOINT) &rcClient+1);
    hbrOld = SelectObject (hDC, hOverlayBrush);
    PatBlt (hDC, rcClient.left, rcClient.top, 
            rcClient.right - rcClient.left, 
            rcClient.bottom - rcClient.top, 
            PATCOPY);
    SelectObject (hDC, hbrOld);

    rcLastScreen = rcScreen;
}

/*======================================================== 
            Image acquistion and grabbing
=========================================================*/

// Turn acquisition of images into frame buffer on or off
// Not interrupt callable, since it enters the client DLL.
void FAR PASCAL CT_Acquire (BOOL fAcquire)
{
#ifdef _BRAVADO
    VW_SetFreezeVid (fAcquire ? 0 : 1); // Backwards
#endif

#ifdef _VBLASTER
    if (fAcquire)
        vbcUnFreezeVideo ();
    else
        vbcFreezeVideo ();
#endif
    
}

// This routine is interrupt callable, and bypasses
// the client DLL.
void FAR PASCAL CT_PrivateAcquire (BOOL fAcquire)
{
    CT_WritePCVideo (0x20, ((CT_ReadPCVideo (0x20) & 0xfe) 
        | (fAcquire ? 1 : 0)));

    // On a FREEZE, wait for acquisition to complete
    // otherwise PCVIDEO asserts the READY line for 20mS.
    // when the first video memory read occurs!!!
    // This behavior violates PC bus specs.
    if (!fAcquire)
        while (CT_ReadPCVideo (0x20) & 1);
}


// Capture a single frame
// Return frozen
void FAR PASCAL CT_GrabFrame (void)
{
    CT_Acquire (TRUE);
    CT_WaitVSync (0);    // Wait Even
    CT_WaitVSync (1);    // Wait Odd
    CT_Acquire (FALSE);
}
/*======================================================== 
              Interrupts and Sync
=========================================================*/
// Set the IRQ used by the device.
// Returns the IRQ actually set.
int FAR PASCAL CT_SetIRQUsed (int nIRQ)
{

#ifdef _BRAVADO
    return 9;          // only 9 is available from this card
#endif

#ifdef _VBLASTER
    {
        int nTemp;
        switch (nIRQ) {
           case 5:  nTemp = 1; break;
           case 10: nTemp = 2; break;
           case 11: nTemp = 4; break;
           case 12: nTemp = 8; break;
           default: return (CT_GetIRQUsed ()); // illegal value, return default
        }
        CT_WritePCVideo (0x82, nTemp);
        return nIRQ;
    }
#endif
}

// Returns the IRQ used by the device
int FAR PASCAL CT_GetIRQUsed ()
{
#ifdef _BRAVADO
    return 9;          // only 9 is available from this card
#endif

#ifdef _VBLASTER
    return vbcGetIntrNo();
#endif
}

#define EVEN_VSYNC_INTERRUPT 0x01
#define  ODD_VSYNC_INTERRUPT 0x02

// Enable the PCVideo chip to create odd and even interrupts
void FAR PASCAL CT_IRQEnable (void)
{
#if FRAME_INTERRUPT
    CT_WritePCVideo (0x09, EVEN_VSYNC_INTERRUPT);
#else
    CT_WritePCVideo (0x09, EVEN_VSYNC_INTERRUPT | ODD_VSYNC_INTERRUPT);
#endif
}

void FAR PASCAL CT_IRQDisable (void)
{
    CT_WritePCVideo (0x09, 0);         // Disable Interrupts
}

// Clear PCVideo chip interrupts
void FAR PASCAL CT_IRQClear (void)
{
    CT_WritePCVideo (0x09, 0);         // Clear the interrupt

#if FRAME_INTERRUPT
    CT_WritePCVideo (0x09, EVEN_VSYNC_INTERRUPT);  // Re-enable it
#else
    CT_WritePCVideo (0x09, ODD_VSYNC_INTERRUPT | EVEN_VSYNC_INTERRUPT);  // Re-enable it
#endif
}


// Wait for Vertical Sync
// nSync = 0, wait even
// nSync = 1, wait odd
// nSync = 2, wait either
void FAR PASCAL CT_WaitVSync (int nSync)
{
    WORD  wRegVal;
    BOOL  bOdd;
    DWORD dwStartTime = timeGetTime();
    
    while (timeGetTime () - dwStartTime < 33) {
        wRegVal = CT_ReadPCVideo (9);
        bOdd = wRegVal & 0x08;
        if (wRegVal & 0x04) {        // if VSync
           if ((nSync == 2) || 
              ((nSync == 1) && bOdd) ||
              ((nSync == 0) && !bOdd))
              break;
        }
    }
}
