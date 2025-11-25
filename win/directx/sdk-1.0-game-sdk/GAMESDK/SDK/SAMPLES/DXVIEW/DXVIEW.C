/****************************************************************************

    PROGRAM: dxview.c

    PURPOSE: DirectX Device Viewer

    FUNCTIONS:

    COMMENTS:

****************************************************************************/

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <ddraw.h>
#include <dsound.h>
#include <dplay.h>
#include "dxview.h"

/****************************************************************************
 ***************************************************************************/

HINSTANCE   g_hInstance;
char        g_szAppName[]   = "DXView";
char        g_szClassName[] = "DXView";
char        g_szTitle[]     = "DirectX Device Viewer";
HWND        g_hwndMain;

HWND        g_hwndLV;        // List view
HWND        g_hwndTV;        // Tree view
HIMAGELIST  g_hImageList;
HFONT       g_hFont;
int         g_xPaneSplit;
int         g_xHalfSplitWidth;
BOOL        g_bSplitMove;
DWORD       g_dwViewState;
DWORD       g_tmAveCharWidth;

/****************************************************************************
 ***************************************************************************/

IDirectDraw *lpDD;          // DirectDraw object
GUID *       ddid;

IDirectSound *lpDS;         // DirectSound object
GUID *       dsid;

IDirectPlay  *lpDP;         // DirectPlay object
GUID *       dpid;

/****************************************************************************
 ***************************************************************************/

#define DDCAPDEF(name,val,flag) {name, FIELD_OFFSET(DDCAPS,val), flag}
#define DDVALDEF(name,val)      {name, FIELD_OFFSET(DDCAPS,val), 0}
#define ROPDEF(name,rop)        DDCAPDEF(name,dwRops[((rop>>16)&0xFF)/32],(1<<((rop>>16)&0xFF)%32))

#define DSCAPDEF(name,val,flag) {name, FIELD_OFFSET(DSCAPS,val), flag}
#define DSVALDEF(name,val)      {name, FIELD_OFFSET(DSCAPS,val), 0}

#define DPCAPDEF(name,val,flag) {name, FIELD_OFFSET(DPCAPS,val), flag}
#define DPVALDEF(name,val)      {name, FIELD_OFFSET(DPCAPS,val), 0}

#define MAKEMODE(xres,yres,bpp) (((DWORD)xres << 20) | ((DWORD)yres << 8) | bpp)
#define GETXRES(mode)           (int)((mode >> 20) & 0x0FFF)
#define GETYRES(mode)           (int)((mode >> 8)  & 0x0FFF)
#define GETCRES(mode)           (int)((mode >> 0)  & 0x00FF)

/****************************************************************************
 ***************************************************************************/

CAPDEF OtherInfoDefs[] =
{
    DDVALDEF("VidMemTotal",                   dwVidMemTotal),
    DDVALDEF("VidMemFree",                    dwVidMemFree),
    DDVALDEF("dwReserved1",                   dwReserved1),
    DDVALDEF("dwReserved2",                   dwReserved2),
    DDVALDEF("dwReserved3",                   dwReserved3),
    DDVALDEF("AlphaBltConstBitDepths",        dwAlphaBltConstBitDepths),
    DDVALDEF("AlphaBltPixelBitDepths",        dwAlphaBltPixelBitDepths),
    DDVALDEF("AlphaBltSurfaceBitDepths",      dwAlphaBltSurfaceBitDepths),
    DDVALDEF("AlphaOverlayConstBitDepths",    dwAlphaOverlayConstBitDepths),
    DDVALDEF("AlphaOverlayPixelBitDepths",    dwAlphaOverlayPixelBitDepths),
    DDVALDEF("AlphaOverlaySurfaceBitDepths",  dwAlphaOverlaySurfaceBitDepths),
    DDVALDEF("ZBufferBitDepths",              dwZBufferBitDepths),
    DDVALDEF("MaxVisibleOverlays",            dwMaxVisibleOverlays),
    DDVALDEF("CurrVisibleOverlays",           dwCurrVisibleOverlays),
    DDVALDEF("NumFourCCCodes",                dwNumFourCCCodes),
    DDVALDEF("AlignBoundarySrc",              dwAlignBoundarySrc),
    DDVALDEF("AlignSizeSrc",                  dwAlignSizeSrc),
    DDVALDEF("AlignBoundaryDest",             dwAlignBoundaryDest),
    DDVALDEF("AlignSizeDest",                 dwAlignSizeDest),
    DDVALDEF("AlignStrideAlign",              dwAlignStrideAlign),
    DDVALDEF("MinOverlayStretch",             dwMinOverlayStretch),
    DDVALDEF("MaxOverlayStretch",             dwMaxOverlayStretch),
    DDVALDEF("MinLiveVideoStretch",           dwMinLiveVideoStretch),
    DDVALDEF("MaxLiveVideoStretch",           dwMaxLiveVideoStretch),
    DDVALDEF("MinHwCodecStretch",             dwMinHwCodecStretch),
    DDVALDEF("MaxHwCodecStretch",             dwMaxHwCodecStretch),
    { "", 0, 0 }
};

/****************************************************************************
 ***************************************************************************/

CAPDEF CapsDefs[] =
{
    DDCAPDEF("3D",                        dwCaps, DDCAPS_3D),
    DDCAPDEF("ALIGNBOUNDARYDEST",         dwCaps, DDCAPS_ALIGNBOUNDARYDEST),
    DDCAPDEF("ALIGNSIZEDEST",             dwCaps, DDCAPS_ALIGNSIZEDEST),
    DDCAPDEF("ALIGNBOUNDARYSRC",          dwCaps, DDCAPS_ALIGNBOUNDARYSRC),
    DDCAPDEF("ALIGNSIZESRC",              dwCaps, DDCAPS_ALIGNSIZESRC),
    DDCAPDEF("ALIGNSTRIDE",               dwCaps, DDCAPS_ALIGNSTRIDE),
    DDCAPDEF("BLT",                       dwCaps, DDCAPS_BLT),
    DDCAPDEF("BLTCOLORFILL",              dwCaps, DDCAPS_BLTCOLORFILL),
    DDCAPDEF("BLTFOURCC",                 dwCaps, DDCAPS_BLTFOURCC),
    DDCAPDEF("BLTSTRETCH",                dwCaps, DDCAPS_BLTSTRETCH),
    DDCAPDEF("BLTQUEUE",                  dwCaps, DDCAPS_BLTQUEUE),
    DDCAPDEF("GDI",                       dwCaps, DDCAPS_GDI),
    DDCAPDEF("OVERLAY",                   dwCaps, DDCAPS_OVERLAY),
    DDCAPDEF("OVERLAYCANTCLIP",           dwCaps, DDCAPS_OVERLAYCANTCLIP),
    DDCAPDEF("OVERLAYFOURCC",             dwCaps, DDCAPS_OVERLAYFOURCC),
    DDCAPDEF("OVERLAYSTRETCH",            dwCaps, DDCAPS_OVERLAYSTRETCH),
    DDCAPDEF("PALETTE",                   dwCaps, DDCAPS_PALETTE),
    DDCAPDEF("PALETTEVSYNC",              dwCaps, DDCAPS_PALETTEVSYNC),
    DDCAPDEF("READSCANLINE",              dwCaps, DDCAPS_READSCANLINE),
    DDCAPDEF("STEREOVIEW",                dwCaps, DDCAPS_STEREOVIEW),
    DDCAPDEF("VBI",                       dwCaps, DDCAPS_VBI),
    DDCAPDEF("ZBLTS",                     dwCaps, DDCAPS_ZBLTS),
    DDCAPDEF("ZOVERLAYS",                 dwCaps, DDCAPS_ZOVERLAYS),
    DDCAPDEF("COLORKEY",                  dwCaps, DDCAPS_COLORKEY),
    DDCAPDEF("ALPHA",                     dwCaps, DDCAPS_ALPHA),
    DDCAPDEF("CKEYHWASSIST",              dwCaps, DDCAPS_COLORKEYHWASSIST),
    DDCAPDEF("NOHARDWARE",                dwCaps, DDCAPS_NOHARDWARE),
    DDCAPDEF("BLTCOLORFILL",              dwCaps, DDCAPS_BLTCOLORFILL),
    DDCAPDEF("BANKSWITCHED",              dwCaps, DDCAPS_BANKSWITCHED),
    DDCAPDEF("CERTIFIED",                 dwCaps2,DDCAPS2_CERTIFIED),
    { "", 0, 0 }
};

/****************************************************************************
 ***************************************************************************/

CAPDEF CKeyCapsDefs[] =
{
    DDCAPDEF("DESTBLT",                   dwCKeyCaps, DDCKEYCAPS_DESTBLT),
    DDCAPDEF("DESTBLTCLRSPACE",           dwCKeyCaps, DDCKEYCAPS_DESTBLTCLRSPACE),
    DDCAPDEF("DESTBLTCLRSPACEYUV",        dwCKeyCaps, DDCKEYCAPS_DESTBLTCLRSPACEYUV),
    DDCAPDEF("DESTBLTYUV",                dwCKeyCaps, DDCKEYCAPS_DESTBLTYUV),
    DDCAPDEF("DESTOVERLAY",               dwCKeyCaps, DDCKEYCAPS_DESTOVERLAY),
    DDCAPDEF("DESTOVERLAYCLRSPACE",       dwCKeyCaps, DDCKEYCAPS_DESTOVERLAYCLRSPACE),
    DDCAPDEF("DESTOVERLAYCLRSPACEYUV",    dwCKeyCaps, DDCKEYCAPS_DESTOVERLAYCLRSPACEYUV),
    DDCAPDEF("DESTOVERLAYONEACTIVE",      dwCKeyCaps, DDCKEYCAPS_DESTOVERLAYONEACTIVE),
    DDCAPDEF("DESTOVERLAYYUV",            dwCKeyCaps, DDCKEYCAPS_DESTOVERLAYYUV),
    DDCAPDEF("SRCBLT",                    dwCKeyCaps, DDCKEYCAPS_SRCBLT),
    DDCAPDEF("SRCBLTCLRSPACE",            dwCKeyCaps, DDCKEYCAPS_SRCBLTCLRSPACE),
    DDCAPDEF("SRCBLTCLRSPACEYUV",         dwCKeyCaps, DDCKEYCAPS_SRCBLTCLRSPACEYUV),
    DDCAPDEF("SRCBLTYUV",                 dwCKeyCaps, DDCKEYCAPS_SRCBLTYUV),
    DDCAPDEF("SRCOVERLAY",                dwCKeyCaps, DDCKEYCAPS_SRCOVERLAY),
    DDCAPDEF("SRCOVERLAYCLRSPACE",        dwCKeyCaps, DDCKEYCAPS_SRCOVERLAYCLRSPACE),
    DDCAPDEF("SRCOVERLAYCLRSPACEYUV",     dwCKeyCaps, DDCKEYCAPS_SRCOVERLAYCLRSPACEYUV),
    DDCAPDEF("SRCOVERLAYONEACTIVE",       dwCKeyCaps, DDCKEYCAPS_SRCOVERLAYONEACTIVE),
    DDCAPDEF("SRCOVERLAYYUV",             dwCKeyCaps, DDCKEYCAPS_SRCOVERLAYYUV),
    { "", 0, 0}
};

/****************************************************************************
 ***************************************************************************/

CAPDEF FXCapsDefs[] =
{
    DDCAPDEF("BLTARITHSTRETCHY",          dwFXCaps, DDFXCAPS_BLTARITHSTRETCHY),
    DDCAPDEF("BLTARITHSTRETCHYN",         dwFXCaps, DDFXCAPS_BLTARITHSTRETCHYN),
    DDCAPDEF("BLTMIRRORLEFTRIGHT",        dwFXCaps, DDFXCAPS_BLTMIRRORLEFTRIGHT),
    DDCAPDEF("BLTMIRRORUPDOWN",           dwFXCaps, DDFXCAPS_BLTMIRRORUPDOWN),
    DDCAPDEF("BLTROTATION",               dwFXCaps, DDFXCAPS_BLTROTATION),
    DDCAPDEF("BLTROTATION90",             dwFXCaps, DDFXCAPS_BLTROTATION90),
    DDCAPDEF("BLTSHRINKX",                dwFXCaps, DDFXCAPS_BLTSHRINKX),
    DDCAPDEF("BLTSHRINKXN",               dwFXCaps, DDFXCAPS_BLTSHRINKXN),
    DDCAPDEF("BLTSHRINKY",                dwFXCaps, DDFXCAPS_BLTSHRINKY),
    DDCAPDEF("BLTSHRINKYN",               dwFXCaps, DDFXCAPS_BLTSHRINKYN),
    DDCAPDEF("BLTSTRETCHX",               dwFXCaps, DDFXCAPS_BLTSTRETCHX),
    DDCAPDEF("BLTSTRETCHXN",              dwFXCaps, DDFXCAPS_BLTSTRETCHXN),
    DDCAPDEF("BLTSTRETCHY",               dwFXCaps, DDFXCAPS_BLTSTRETCHY),
    DDCAPDEF("BLTSTRETCHYN",              dwFXCaps, DDFXCAPS_BLTSTRETCHYN),
    DDCAPDEF("OVERLAYARITHSTRETCHY",      dwFXCaps, DDFXCAPS_OVERLAYARITHSTRETCHY),
    DDCAPDEF("OVERLAYARITHSTRETCHYN",     dwFXCaps, DDFXCAPS_OVERLAYARITHSTRETCHYN),
    DDCAPDEF("OVERLAYSHRINKX",            dwFXCaps, DDFXCAPS_OVERLAYSHRINKX),
    DDCAPDEF("OVERLAYSHRINKXN",           dwFXCaps, DDFXCAPS_OVERLAYSHRINKXN),
    DDCAPDEF("OVERLAYSHRINKY",            dwFXCaps, DDFXCAPS_OVERLAYSHRINKY),
    DDCAPDEF("OVERLAYSHRINKYN",           dwFXCaps, DDFXCAPS_OVERLAYSHRINKYN),
    DDCAPDEF("OVERLAYSTRETCHX",           dwFXCaps, DDFXCAPS_OVERLAYSTRETCHX),
    DDCAPDEF("OVERLAYSTRETCHXN",          dwFXCaps, DDFXCAPS_OVERLAYSTRETCHXN),
    DDCAPDEF("OVERLAYSTRETCHY",           dwFXCaps, DDFXCAPS_OVERLAYSTRETCHY),
    DDCAPDEF("OVERLAYSTRETCHYN",          dwFXCaps, DDFXCAPS_OVERLAYSTRETCHYN),
    DDCAPDEF("OVERLAYMIRRORLEFTRIGHT",    dwFXCaps, DDFXCAPS_OVERLAYMIRRORLEFTRIGHT),
    DDCAPDEF("OVERLAYMIRRORUPDOWN",       dwFXCaps, DDFXCAPS_OVERLAYMIRRORUPDOWN),
    { "", 0, 0}
};

/****************************************************************************
 ***************************************************************************/

CAPDEF PalCapsDefs[] =
{
    DDCAPDEF("4BIT",              dwPalCaps, DDPCAPS_4BIT),
    DDCAPDEF("8BITENTRIES",       dwPalCaps, DDPCAPS_8BITENTRIES),
    DDCAPDEF("8BIT",              dwPalCaps, DDPCAPS_8BIT),
    DDCAPDEF("INITIALIZE",        dwPalCaps, DDPCAPS_INITIALIZE),
    DDCAPDEF("PRIMARYSURFACE",    dwPalCaps, DDPCAPS_PRIMARYSURFACE),
    DDCAPDEF("PRIMARYSURFACELEFT",dwPalCaps, DDPCAPS_PRIMARYSURFACELEFT),
    DDCAPDEF("VSYNC",             dwPalCaps, DDPCAPS_VSYNC),
    { "", 0, 0}
};

/****************************************************************************
 ***************************************************************************/

CAPDEF SurfCapsDefs[] =
{
    DDCAPDEF( "3D",                   ddsCaps.dwCaps, DDSCAPS_3D),
    DDCAPDEF( "ALPHA",                ddsCaps.dwCaps, DDSCAPS_ALPHA),
    DDCAPDEF( "BACKBUFFER",           ddsCaps.dwCaps, DDSCAPS_BACKBUFFER),
    DDCAPDEF( "COMPLEX",              ddsCaps.dwCaps, DDSCAPS_COMPLEX),
    DDCAPDEF( "FLIP",                 ddsCaps.dwCaps, DDSCAPS_FLIP),
    DDCAPDEF( "FRONTBUFFER",          ddsCaps.dwCaps, DDSCAPS_FRONTBUFFER),
    DDCAPDEF( "OFFSCREENPLAIN",       ddsCaps.dwCaps, DDSCAPS_OFFSCREENPLAIN),
    DDCAPDEF( "OVERLAY",              ddsCaps.dwCaps, DDSCAPS_OVERLAY),
    DDCAPDEF( "PALETTE",              ddsCaps.dwCaps, DDSCAPS_PALETTE),
    DDCAPDEF( "PRIMARYSURFACE",       ddsCaps.dwCaps, DDSCAPS_PRIMARYSURFACE),
    DDCAPDEF( "PRIMARYSURFACELEFT",   ddsCaps.dwCaps, DDSCAPS_PRIMARYSURFACELEFT),
    DDCAPDEF( "SYSTEMMEMORY",         ddsCaps.dwCaps, DDSCAPS_SYSTEMMEMORY),
    DDCAPDEF( "TEXTUREMAP",           ddsCaps.dwCaps, DDSCAPS_TEXTUREMAP),
    DDCAPDEF( "VIDEOMEMORY",          ddsCaps.dwCaps, DDSCAPS_VIDEOMEMORY),
    DDCAPDEF( "VISIBLE",              ddsCaps.dwCaps, DDSCAPS_VISIBLE),
    DDCAPDEF( "WRITEONLY",            ddsCaps.dwCaps, DDSCAPS_WRITEONLY),
    DDCAPDEF( "ZBUFFER",              ddsCaps.dwCaps, DDSCAPS_ZBUFFER),
    DDCAPDEF( "OWNDC",                ddsCaps.dwCaps, DDSCAPS_OWNDC),
    DDCAPDEF( "LIVEVIDEO",            ddsCaps.dwCaps, DDSCAPS_LIVEVIDEO),
    DDCAPDEF( "HWCODEC",              ddsCaps.dwCaps, DDSCAPS_HWCODEC),
    DDCAPDEF( "MODEX",                ddsCaps.dwCaps, DDSCAPS_MODEX),

    { "", 0, 0}
};

/****************************************************************************
 ***************************************************************************/

CAPDEF SVisionCapsDefs[] =
{
    DDCAPDEF( "ENIGMA",  dwSVCaps, DDSVCAPS_ENIGMA),
    DDCAPDEF( "FLICKER", dwSVCaps, DDSVCAPS_FLICKER),
    DDCAPDEF( "REDBLUE", dwSVCaps, DDSVCAPS_REDBLUE),
    DDCAPDEF( "SPLIT",   dwSVCaps, DDSVCAPS_SPLIT),
    { "", 0, 0}
};

/****************************************************************************
 ***************************************************************************/

CAPDEF ROPCapsDefs[] =
{
    ROPDEF("SRCCOPY",    SRCCOPY),
    ROPDEF("SRCPAINT",   SRCPAINT),
    ROPDEF("SRCAND",     SRCAND),
    ROPDEF("SRCINVERT",  SRCINVERT),
    ROPDEF("SRCERASE",   SRCERASE),
    ROPDEF("NOTSRCCOPY", NOTSRCCOPY),
    ROPDEF("NOTSRCERASE",NOTSRCERASE),
    ROPDEF("MERGECOPY",  MERGECOPY),
    ROPDEF("MERGEPAINT", MERGEPAINT),
    ROPDEF("PATCOPY",    PATCOPY),
    ROPDEF("PATPAINT",   PATPAINT),
    ROPDEF("PATINVERT",  PATINVERT),
    ROPDEF("DSTINVERT",  DSTINVERT),
    ROPDEF("BLACKNESS",  BLACKNESS),
    ROPDEF("WHITENESS",  WHITENESS),
    {"", 0, 0}
};

/****************************************************************************
 ***************************************************************************/

CAPDEFS DDCapDefs[] =
{
    {"",                    DDAddCaps,          (LPARAM)OtherInfoDefs},
    {"General",             DDAddCaps,          (LPARAM)OtherInfoDefs},
    {"General Caps",        DDAddCaps,          (LPARAM)CapsDefs},
    {"Color Key Caps",      DDAddCaps,          (LPARAM)CKeyCapsDefs},
    {"FX Caps",             DDAddCaps,          (LPARAM)FXCapsDefs},
    {"Palette Caps",        DDAddCaps,          (LPARAM)PalCapsDefs},
    {"Surface Caps",        DDAddCaps,          (LPARAM)SurfCapsDefs},
    {"Stereo Vision Caps",  DDAddCaps,          (LPARAM)SVisionCapsDefs},
    {"ROPS",                DDAddCaps,          (LPARAM)ROPCapsDefs},
    {"Video Modes",         DDAddVideoModes,    (LPARAM)0},
    { NULL, 0, 0}
};

/****************************************************************************
 ***************************************************************************/

CAPDEF DSInfo[] =
{
    DSVALDEF("MinSecondarySampleRate",          dwMinSecondarySampleRate),
    DSVALDEF("MaxSecondarySampleRate",          dwMaxSecondarySampleRate),
    DSVALDEF("PrimaryBuffers",                  dwPrimaryBuffers),
    DSVALDEF("MaxHwMixingAllBuffers",           dwMaxHwMixingAllBuffers),
    DSVALDEF("MaxHwMixingStaticBuffers",        dwMaxHwMixingStaticBuffers),
    DSVALDEF("MaxHwMixingStreamingBuffers",     dwMaxHwMixingStreamingBuffers),
    DSVALDEF("FreeHwMixingAllBuffers",          dwFreeHwMixingAllBuffers),
    DSVALDEF("FreeHwMixingStaticBuffers",       dwFreeHwMixingStaticBuffers),
    DSVALDEF("FreeHwMixingStreamingBuffers",    dwFreeHwMixingStreamingBuffers),
    DSVALDEF("MaxHw3DAllBuffers",               dwMaxHw3DAllBuffers),
    DSVALDEF("MaxHw3DStaticBuffers",            dwMaxHw3DStaticBuffers),
    DSVALDEF("MaxHw3DStreamingBuffers",         dwMaxHw3DStreamingBuffers),
    DSVALDEF("FreeHw3DAllBuffers",              dwFreeHw3DAllBuffers),
    DSVALDEF("FreeHw3DStaticBuffers",           dwFreeHw3DStaticBuffers),
    DSVALDEF("FreeHw3DStreamingBuffers",        dwFreeHw3DStreamingBuffers),
    DSVALDEF("TotalHwMemBytes",                 dwTotalHwMemBytes),
    DSVALDEF("FreeHwMemBytes",                  dwFreeHwMemBytes),
    DSVALDEF("MaxContigFreeHwMemBytes",         dwMaxContigFreeHwMemBytes),
    DSVALDEF("UnlockTransferRateHwBuffers",     dwUnlockTransferRateHwBuffers),
    DSVALDEF("PlayCpuOverheadSwBuffers",        dwPlayCpuOverheadSwBuffers),
    {"", 0, 0}
};

/****************************************************************************
 ***************************************************************************/

CAPDEF DSGeneralCaps[] =
{
    DSCAPDEF("PRIMARYMONO",       dwFlags,    DSCAPS_PRIMARYMONO),
    DSCAPDEF("PRIMARYSTEREO",     dwFlags,    DSCAPS_PRIMARYSTEREO),
    DSCAPDEF("PRIMARY8BIT",       dwFlags,    DSCAPS_PRIMARY8BIT),
    DSCAPDEF("PRIMARY16BIT",      dwFlags,    DSCAPS_PRIMARY16BIT),
    DSCAPDEF("CONTINUOUSRATE",    dwFlags,    DSCAPS_CONTINUOUSRATE),
    DSCAPDEF("EMULDRIVER",        dwFlags,    DSCAPS_EMULDRIVER),
    DSCAPDEF("SECONDARYMONO",     dwFlags,    DSCAPS_SECONDARYMONO),
    DSCAPDEF("SECONDARYSTEREO",   dwFlags,    DSCAPS_SECONDARYSTEREO),
    DSCAPDEF("SECONDARY8BIT",     dwFlags,    DSCAPS_SECONDARY8BIT),
    DSCAPDEF("SECONDARY16BIT",    dwFlags,    DSCAPS_SECONDARY16BIT),
    {"", 0, 0}
};

/****************************************************************************
 ***************************************************************************/

CAPDEFS DSCapDefs[] =
{
    {"",                    DSAddCaps,          (LPARAM)DSInfo},
    {"General",             DSAddCaps,          (LPARAM)DSInfo},
    {"General Caps",        DSAddCaps,          (LPARAM)DSGeneralCaps},
    {NULL, 0, 0}
};

/****************************************************************************
 ***************************************************************************/

CAPDEF DPInfo[] =
{
    DPVALDEF("MaxBufferSize", dwMaxBufferSize),
    DPVALDEF("MaxQueueSize",  dwMaxQueueSize),
    DPVALDEF("MaxPlayers",    dwMaxPlayers),
    DPVALDEF("HundredBaud",   dwHundredBaud),
    DPVALDEF("Latency",       dwLatency),
    {"", 0, 0}
};

/****************************************************************************
 ***************************************************************************/

CAPDEFS DPCapDefs[] =
{
    {"",                    DPAddCaps,          (LPARAM)DPInfo},
    {"General",             DPAddCaps,          (LPARAM)DPInfo},
    {"Sessions",            DPAddSessions,      (LPARAM)0},
    {NULL, 0, 0}
};

//================================================================
//  WinMain - entry point
//================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;

    g_hInstance = hInstance; // Store instance handle in our global variable

    if (InitInstance(hInstance, lpCmdLine, nCmdShow, DXView_WIDTH, DXView_HEIGHT))
    {
        while(GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return(msg.wParam);
}

//================================================================
//  InitInstance - create main window
//================================================================
BOOL InitInstance(HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow, int iWidth, int iHeight)
{
    WNDCLASS  wc;

    wc.style         = CS_HREDRAW | CS_VREDRAW; // Class style(s).
    wc.lpfnWndProc   = (WNDPROC)WndProc;        // Window Procedure
    wc.cbClsExtra    = 0;                       // No per-class extra data.
    wc.cbWndExtra    = 0;                       // No per-window extra data.
    wc.hInstance     = hInstance;               // Owner of this class
    wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DIRECTX)); // Icon name from .RC
    wc.hCursor       = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_SPLIT));// Cursor
    wc.hbrBackground = (HBRUSH)(COLOR_3DFACE+1); // Default color
    wc.lpszMenuName  = "Menu";                   // Menu name from .RC
    wc.lpszClassName = g_szClassName;            // Name to register as

    if(!RegisterClass(&wc))
    {
        return FALSE;
    }

    // Create a main window for this application instance.
    g_hwndMain = CreateWindowEx(
        0,
        g_szClassName,   // See RegisterClass() call.
        g_szTitle,       // Text for window title bar.
        WS_OVERLAPPEDWINDOW,// Window style.
        CW_USEDEFAULT, CW_USEDEFAULT, iWidth, iHeight, // Use default positioning
        NULL,            // Overlapped windows have no parent.
        NULL,            // Use the window class menu.
        hInstance,       // This instance owns this window.
        NULL);

    // If window could not be created, return "failure"
    if (!g_hwndMain)
    {
        return(FALSE);
    }

    // Make the window visible; update its client area; and return "success"
    ShowWindow(g_hwndMain, nCmdShow); // Show the window

    return(TRUE);              // We succeeded...
}

//================================================================
//  WndProc - main window proc
//================================================================
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_CREATE:
            return DXView_OnCreate(hwnd);

        case WM_SIZE:
            DXView_OnSize(hwnd);
            break;

        case WM_LBUTTONDOWN:
            g_bSplitMove = TRUE;
            SetCapture(hwnd);
            break;

        case WM_LBUTTONUP:
            g_bSplitMove = FALSE;
            ReleaseCapture();
            break;

        case WM_MOUSEMOVE:
            if(g_bSplitMove)
            {
                g_xPaneSplit = (LOWORD(lParam) - g_xHalfSplitWidth);
                DXView_OnSize(hwnd);
            }
            break;

        case WM_NOTIFY:
            if (((NMHDR*)lParam)->hwndFrom == g_hwndTV)
            {
                if (((NMHDR*)lParam)->code == TVN_SELCHANGED)
                    DXView_OnTreeSelect(g_hwndTV, (NM_TREEVIEW*)lParam);
            }

            if (((NMHDR*)lParam)->hwndFrom == g_hwndLV)
            {
                if (((NMHDR*)lParam)->code == NM_RDBLCLK)
                    DXView_OnListViewDblClick(g_hwndLV, (NM_LISTVIEW*)lParam);
            }

            break;

        case WM_COMMAND:  // message: command from application menu
            DXView_OnCommand(hwnd, wParam);
            break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:  // message: window being destroyed
            DXView_Cleanup();  // Free per item struct for all items
            PostQuitMessage(0);
            break;
    }

    return(DefWindowProc(hwnd, message, wParam, lParam));
}

//================================================================
//================================================================
BOOL DXView_OnCreate(HWND hwnd)
{
    HDC hDC;
    int PixelsPerInch;
    TEXTMETRIC tm;
    static TCHAR szBuf[MAX_PATH];

    hDC = GetDC(hwnd);
    PixelsPerInch = GetDeviceCaps(hDC, LOGPIXELSX);
    g_hFont = GetStockObject(ANSI_FIXED_FONT);
    SelectObject(hDC, g_hFont);
    GetTextMetrics(hDC, &tm);
    g_tmAveCharWidth = tm.tmAveCharWidth;
    ReleaseDC(hwnd, hDC);

    // Initialize global data
    g_dwViewState = IDM_VIEWAVAIL;
    g_xPaneSplit = PixelsPerInch * 9 / 4;     //  2.25 inches
    g_xHalfSplitWidth = GetSystemMetrics(SM_CXSIZEFRAME) / 2;

    // Make sure that the common control library read to rock
    InitCommonControls();

    CheckMenuItem(GetMenu(hwnd), g_dwViewState, MF_BYCOMMAND | MF_CHECKED);

    // Create the list view window.
    g_hwndLV = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, "",
        WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT,
        0, 0, 0, 0, hwnd, (HMENU)IDC_LV, g_hInstance, NULL);

    // create the tree view window.
    g_hwndTV = CreateWindowEx(WS_EX_CLIENTEDGE, WC_TREEVIEW, "",
        WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES |
        TVS_HASBUTTONS | TVS_LINESATROOT,
        0, 0, 0, 0, hwnd, (HMENU)IDC_TV, g_hInstance, NULL);

    // create our image list.
    DXView_InitImageList();

    // Initialize the tree view
    DXView_FillTree(g_hwndTV);

    return(TRUE);
}

//================================================================
//================================================================

IDirectDraw * DDCreate(GUID *pid)
{
    if (lpDD && pid == ddid)
        return lpDD;

    if (lpDD)
        IDirectDraw_Release(lpDD);

    if (DirectDrawCreate(pid, &lpDD, NULL) == DD_OK)
    {
        ddid = pid;
        return lpDD;
    }

    MessageBox(g_hwndMain, "DirectDrawCreate failed.", g_szAppName, MB_OK);
    return NULL;
}

//================================================================
//================================================================

IDirectSound * DSCreate(GUID *pid)
{
    if (lpDS && pid == dsid)
        return lpDS;

    if (lpDS)
        IDirectSound_Release(lpDS);

    if (DirectSoundCreate(pid, &lpDS, NULL) == DD_OK)
    {
        dsid = pid;
        return lpDS;
    }

    MessageBox(g_hwndMain, "DirectSoundCreate failed.", g_szAppName, MB_OK);
    return NULL;
}

//================================================================
//================================================================

IDirectPlay * DPCreate(GUID *pid)
{
    if (lpDP && pid == dpid)
        return lpDP;

    if (lpDP)
        lpDP->lpVtbl->Release(lpDP);

    if (DirectPlayCreate(pid, &lpDP, NULL) == DD_OK)
    {
        dpid = pid;
        return lpDP;
    }

    MessageBox(g_hwndMain, "DirectPlayCreate failed.", g_szAppName, MB_OK);
    return NULL;
}

//================================================================
//================================================================

void AddCapsToTV(HTREEITEM hParent, CAPDEFS *pcds, LPARAM lParam1)
{
    HTREEITEM hTree;
    BOOL f = TRUE;

    while (pcds->szName)
    {
        hTree = TVAddNode(hParent, pcds->szName, f, IDI_CAPS,
            pcds->Callback, lParam1, pcds->lParam2);

        if (f)
        {
            hParent = hTree;
            f = FALSE;
        }

        pcds++;  // Get next Cap bit definition
    }
}

//================================================================
//================================================================
char c_szYes[] = "Yes";
char c_szNo[] = "No";
char c_szCurrentMode[] = "Current Mode";

void AddCapsToLV(CAPDEF *pcd, LPVOID pv)
{
    DWORD dwValue;

    LVAddColumn(g_hwndLV, 0, "Name", 24);
    LVAddColumn(g_hwndLV, 1, "Value", 10);

    while(pcd->szName && *pcd->szName)
    {
        dwValue = *(DWORD *)(((BYTE *)pv) + pcd->dwOffset);

        if (pcd->dwFlag)
        {
            if (pcd->dwFlag & dwValue)
            {
                LVAddText(g_hwndLV, 0, pcd->szName);
                LVAddText(g_hwndLV, 1, c_szYes);
            }
            else if (g_dwViewState == IDM_VIEWALL)
            {
                LVAddText(g_hwndLV, 0, pcd->szName);
                LVAddText(g_hwndLV, 1, c_szNo);
            }
        }
        else
        {
            LVAddText(g_hwndLV, 0, pcd->szName);
            LVAddText(g_hwndLV, 1, "%d", dwValue);
        }

        pcd++;  // Get next Cap bit definition
    }
}

//================================================================
//================================================================

void DDAddCaps(LPARAM lParam1, LPARAM lParam2)
{
    // lParam1 is the GUID for the driver we should open
    // lParam2 is the CAPDEF table we should use

    if (DDCreate((GUID*)lParam1))
    {
        DDCAPS ddcaps;

        ddcaps.dwSize = sizeof(ddcaps);

        if (lParam1 == DDCREATE_EMULATIONONLY)
            IDirectDraw_GetCaps(lpDD, NULL, &ddcaps);
        else
            IDirectDraw_GetCaps(lpDD, &ddcaps, NULL);

        AddCapsToLV((CAPDEF *)lParam2, (LPVOID)&ddcaps);
    }
}

//================================================================
//================================================================

void DSAddCaps(LPARAM lParam1, LPARAM lParam2)
{
    // lParam1 is the GUID for the driver we should open
    // lParam2 is the CAPDEF table we should use

    if (DSCreate((GUID*)lParam1))
    {
        DSCAPS dscaps;

        dscaps.dwSize = sizeof(dscaps);
        IDirectSound_GetCaps(lpDS, &dscaps);

        AddCapsToLV((CAPDEF *)lParam2, (LPVOID)&dscaps);
    }
}

//================================================================
//================================================================

void DPAddCaps(LPARAM lParam1, LPARAM lParam2)
{
    // lParam1 is the GUID for the driver we should open
    // lParam2 is the CAPDEF table we should use

    if (DPCreate((GUID*)lParam1))
    {
        DPCAPS dpcaps;

        dpcaps.dwSize = sizeof(dpcaps);
        lpDP->lpVtbl->GetCaps(lpDP, &dpcaps);

        AddCapsToLV((CAPDEF *)lParam2, (LPVOID)&dpcaps);
    }
}

//================================================================
// EnumSessionsCallback
//================================================================
BOOL PASCAL EnumSessionsCallback
(
LPDPSESSIONDESC lpDesc,         // Pointer to Session Description Struct
LPVOID lpUser,                // User definable data passed in from EnumSessions() call.
LPDWORD lpdwTimeOut,          // Used to extend the timeout if hosts aren't responding quickly enough.
DWORD dwFlags                 // Flags (used to notify us when we've timed out).
)
{
    if (dwFlags & DPESC_TIMEDOUT)
    {
        // We could reset lpdwTimeOut and return true to continue waiting
        // NOTE: This does not tell DirectPlay to query again for hosts.
        //       This just gives potentially slow hosts more time to respond
        //       to our initial query.
        return FALSE;  // Stop waiting for hosts
    }

    // Add session information to table
    LVAddText(g_hwndLV, 0, lpDesc->szSessionName);
    LVAddText(g_hwndLV, 1, "%d", lpDesc->dwSession);
    LVAddText(g_hwndLV, 2, "%d", lpDesc->dwMaxPlayers);
    LVAddText(g_hwndLV, 3, "%d", lpDesc->dwCurrentPlayers);

    return TRUE;
}

//================================================================
//================================================================

void DPAddSessions(LPARAM lParam1, LPARAM lParam2)
{
    // lParam1 is the GUID for the driver we should open
    // lParam2 is the CAPDEF table we should use

    DPSESSIONDESC   dps;
    HCURSOR         hCur=NULL;

    dps.dwSize = sizeof(dps);
    memset(&dps.guidSession, 0, sizeof(dps.guidSession));

    hCur=SetCursor(LoadCursor(NULL, IDC_WAIT));
    if (DPCreate((GUID*)lParam1))
    {
        LVAddColumn(g_hwndLV, 0, "Name", 24);
        LVAddColumn(g_hwndLV, 1, "Session", 7);
        LVAddColumn(g_hwndLV, 2, "MaxPlayers", 11);
        LVAddColumn(g_hwndLV, 3, "CurrentPlayers", 14);

        lpDP->lpVtbl->EnumSessions(lpDP, &dps, 500, EnumSessionsCallback, NULL, DPENUMSESSIONS_ALL);
    }
    if (hCur)
        SetCursor(hCur);

}

//================================================================
// EnumDisplayModesCallback
//================================================================

HRESULT CALLBACK EnumDisplayModesCallback(LPDDSURFACEDESC pddsd, LPVOID Context)
{
    LVAddText(g_hwndLV, 0, "%dx%dx%d",
        pddsd->dwWidth, pddsd->dwHeight, pddsd->ddpfPixelFormat.dwRGBBitCount);

    return DDENUMRET_OK;

} /* EnumModesCallback */

//================================================================
//================================================================
void DDAddVideoModes(LPARAM lParam1, LPARAM lParam2)
{
    DWORD mode;
    DDSURFACEDESC ddsd;

    LVAddColumn(g_hwndLV, 0, "Mode", 24);
    LVAddColumn(g_hwndLV, 1, "", 24);

    // lParam1 is the GUID for the driver we should open
    // lParam2 is not used

    if (DDCreate((GUID*)lParam1))
    {
        // get the current mode mode for this driver
        ddsd.dwSize = sizeof(DDSURFACEDESC);
        IDirectDraw_GetDisplayMode(lpDD, &ddsd);

        mode = MAKEMODE(ddsd.dwWidth, ddsd.dwHeight, ddsd.ddpfPixelFormat.dwRGBBitCount);

        // Find all mode for this driver and add them to the listview
        IDirectDraw_SetCooperativeLevel(lpDD, g_hwndMain, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWMODEX | DDSCL_NOWINDOWCHANGES);
        IDirectDraw_EnumDisplayModes(lpDD, 0, NULL, (LPVOID)mode, EnumDisplayModesCallback);
        IDirectDraw_SetCooperativeLevel(lpDD, g_hwndMain, DDSCL_NORMAL);
    }
}

//================================================================
//================================================================
void DXView_OnTreeSelect(HWND hwndTV, NM_TREEVIEW *ptv)
{
    NODEINFO *pni;

    SendMessage(g_hwndLV, WM_SETREDRAW, FALSE, 0);
    ListView_DeleteAllItems(g_hwndLV);
    LVAddColumn(g_hwndLV, 0, "", 0);

    if (ptv == NULL)
    {
        TV_ITEM tvi;
        // get lParam of current tree node
        tvi.hItem  = TreeView_GetSelection(g_hwndTV);
        tvi.mask   = TVIF_PARAM;
        tvi.lParam = 0;
        TreeView_GetItem(g_hwndTV, &tvi);
        pni = (NODEINFO*)tvi.lParam;
    }
    else
    {
        pni = (NODEINFO*)ptv->itemNew.lParam;
    }

    if (pni && pni->Callback)
    {
        pni->Callback(pni->lParam1, pni->lParam2);
    }

    SendMessage(g_hwndLV, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(g_hwndLV, NULL, TRUE);
}

//================================================================
//================================================================
void DXView_OnListViewDblClick(HWND hwndLV, NM_LISTVIEW *plv)
{
    LV_ITEM lvi;

    lvi.mask   = LVIF_PARAM;
    lvi.lParam = 0;
    lvi.iItem  = plv->iItem;
    ListView_GetItem(hwndLV, &lvi);
}


//================================================================
//================================================================
void DXView_OnCommand(HWND hwnd, WPARAM wParam)
{
    HMENU hMenu;

    switch(LOWORD(wParam))
    {
        case IDM_VIEWAVAIL:
        case IDM_VIEWALL:
            hMenu = GetMenu(hwnd);
            CheckMenuItem(hMenu, g_dwViewState, MF_BYCOMMAND | MF_UNCHECKED);
            g_dwViewState = LOWORD(wParam);
            CheckMenuItem(hMenu, g_dwViewState, MF_BYCOMMAND | MF_CHECKED);
            DXView_OnTreeSelect(g_hwndTV, NULL);
            break;

        case IDM_ABOUT:
            DialogBox(g_hInstance, "About", hwnd, (DLGPROC)About);
            break;

        case IDM_EXIT:
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;
    }
}

//================================================================
//================================================================
void DXView_Cleanup()
{
    if (lpDD)
        IDirectDraw_Release(lpDD);

    if (lpDS)
        IDirectSound_Release(lpDS);

    if (lpDP)
        lpDP->lpVtbl->Release(lpDP);

    if(g_hImageList)
        ImageList_Destroy(g_hImageList);
}

//================================================================
//================================================================
BOOL DXView_InitImageList()
{
    int cxSmIcon;
    int cySmIcon;
    UINT Index;
    HICON hIcon;

    if (g_hImageList)
        return TRUE;

    cxSmIcon = GetSystemMetrics(SM_CXSMICON);
    cySmIcon = GetSystemMetrics(SM_CYSMICON);

    //  First, create the image list that is needed.
    if((g_hImageList = ImageList_Create(cxSmIcon, cySmIcon, TRUE, IDI_LASTIMAGE - IDI_FIRSTIMAGE, 10)) == NULL)
        return(FALSE);

    //
    //  Initialize the image list with all of the icons that we'll be using
    //  Once set, send its handle to all interested child windows.
    //
    for (Index = IDI_FIRSTIMAGE; Index <= IDI_LASTIMAGE; Index++)
    {
        hIcon = LoadImage(g_hInstance, MAKEINTRESOURCE(Index), IMAGE_ICON, cxSmIcon, cySmIcon, 0);
        ImageList_AddIcon(g_hImageList, hIcon);
        DestroyIcon(hIcon);
    }

    TreeView_SetImageList(g_hwndTV, g_hImageList, TVSIL_NORMAL);

    return(TRUE);
}

//================================================================
//================================================================

HRESULT CALLBACK DDEnumCallBack(GUID *pid, LPSTR lpDriverDesc, LPSTR lpDriverName, LPVOID lpContext)
{
    HTREEITEM hParent = (HTREEITEM)lpContext;
    TCHAR szText[256];

    if (HIWORD(pid) != 0)
    {
        GUID temp = *pid;
        pid = (GUID *)LocalAlloc(LPTR, sizeof(GUID));
        *pid = temp;
    }

    // Add subnode to treeview
    if (lpDriverName && *lpDriverName)
        wsprintf(szText, "%s (%s)", lpDriverDesc, lpDriverName);
    else
        lstrcpy(szText, lpDriverDesc);

    DDCapDefs[0].szName = szText;
    AddCapsToTV(hParent, DDCapDefs, (LPARAM)pid);

    return(DDENUMRET_OK);
}

//================================================================
//================================================================

HRESULT CALLBACK DSEnumCallBack(GUID *pid, LPSTR lpDriverDesc, LPSTR lpDriverName, LPVOID lpContext)
{
    HTREEITEM hParent = (HTREEITEM)lpContext;
    TCHAR szText[256];

    if (HIWORD(pid) != 0)
    {
        GUID temp = *pid;
        pid = (GUID *)LocalAlloc(LPTR, sizeof(GUID));
        *pid = temp;
    }

    // Add subnode to treeview
    if (lpDriverName && *lpDriverName)
        wsprintf(szText, "%s (%s)", lpDriverDesc, lpDriverName);
    else
        lstrcpy(szText, lpDriverDesc);

    DSCapDefs[0].szName = szText;
    AddCapsToTV(hParent, DSCapDefs, (LPARAM)pid);

    return(DDENUMRET_OK);
}

BOOL CALLBACK DPEnumCallback(GUID *pid, LPSTR szName, DWORD major_ver, DWORD minor_ver, LPVOID lpContext)
{
    HTREEITEM hParent = (HTREEITEM)lpContext;
    TCHAR szText[256];

    if (HIWORD(pid) != 0)
    {
        GUID temp = *pid;
        pid = (GUID *)LocalAlloc(LPTR, sizeof(GUID));
        *pid = temp;
    }

    wsprintf(szText, "%s %d.%d", szName, major_ver, minor_ver);

    DPCapDefs[0].szName = szText;
    AddCapsToTV(hParent, DPCapDefs, (LPARAM)pid);

    return TRUE;
}

//================================================================
//================================================================

void DXView_FillTree(HWND hwndTV)
{
    HTREEITEM hTree;

    // Add direct draw devices

    hTree = TVAddNode(TVI_ROOT, "DirectDraw Devices", TRUE,
        IDI_DIRECTX, NULL, 0, 0);

    // Add Display Driver node(s) and capability nodes to treeview
    DirectDrawEnumerate(DDEnumCallBack, hTree);

    // Add Hardware Emulation Layer node to treeview
    DDEnumCallBack((GUID *)DDCREATE_EMULATIONONLY, "Hardware Emulation Layer", NULL, (LPVOID)hTree);

    TreeView_Expand(g_hwndTV, hTree, TVE_EXPAND);
    TreeView_SelectItem(g_hwndTV, hTree);

    // Add direct sound devices

    hTree = TVAddNode(TVI_ROOT, "DirectSound Devices", TRUE,
        IDI_DIRECTX, NULL, 0, 0);

    DirectSoundEnumerate(DSEnumCallBack, hTree);
    TreeView_Expand(g_hwndTV, hTree, TVE_EXPAND);

    // Add direct play devices

    hTree = TVAddNode(TVI_ROOT, "DirectPlay Devices", TRUE,
        IDI_DIRECTX, NULL, 0, 0);

    DirectPlayEnumerate(DPEnumCallback, hTree);
    TreeView_Expand(g_hwndTV, hTree, TVE_EXPAND);
}

//================================================================
//  About - process about box
//================================================================
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_INITDIALOG:
            return(TRUE);

        case WM_COMMAND:                      // message: received a command
            if(LOWORD(wParam) == IDOK        // "OK" box selected?
            || LOWORD(wParam) == IDCANCEL) {  // System menu close command?
                EndDialog(hDlg, TRUE);        // Exit the dialog
                return(TRUE);
            }
            break;
    }
    return(FALSE); // Didn't process the message
}

//================================================================
//================================================================
//  DXView_OnSize
//
//  DESCRIPTION:
//     Called whenever the size of the app window has changed or the size
//     of its child controls should be adjusted.
//
//  PARAMETERS:
//     hWnd, handle of app window.
//
//================================================================
//================================================================
void DXView_OnSize(HWND hWnd)
{
    HDWP hDWP;
    RECT ClientRect;
    int Height;
    HWND hKeyTreeWnd;
    HWND hValueListWnd;
    int x;
    int dx;

    if (IsIconic(hWnd))
        return;

    if ((hDWP = BeginDeferWindowPos(2)) != NULL)
    {
        //  Data structure used when calling GetEffectiveClientRect (which takes into
        //  account space taken up by the toolbars/status bars).  First half of the
        //  pair is zero when at the end of the list, second half is the control id.
        int s_EffectiveClientRectData[] = {
            1, 0,                               //  For the menu bar, but is unused
            0, 0                                //  First zero marks end of data
        };

        GetEffectiveClientRect(hWnd, &ClientRect, s_EffectiveClientRectData);
        Height = ClientRect.bottom - ClientRect.top;

        hKeyTreeWnd = g_hwndTV;

        DeferWindowPos(hDWP, hKeyTreeWnd, NULL, 0, ClientRect.top, g_xPaneSplit,
            Height, SWP_NOZORDER | SWP_NOACTIVATE);

        x = g_xPaneSplit + GetSystemMetrics(SM_CXSIZEFRAME);
        dx = ClientRect.right - ClientRect.left - x;

        hValueListWnd = g_hwndLV;

        DeferWindowPos(hDWP, hValueListWnd, NULL, x, ClientRect.top, dx, Height,
            SWP_NOZORDER | SWP_NOACTIVATE);

        EndDeferWindowPos(hDWP);
    }
}

#pragma optimize("", off)
/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/

void LVAddColumn(HWND hwndLV, int i, char *name, int width)
{
    LV_COLUMN col;

    if (i == 0)
    {
        while(ListView_DeleteColumn(hwndLV, 0))
            ;
    }

    col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
    col.fmt  = LVCFMT_LEFT;
    col.pszText = name;
    col.cchTextMax = 0;
    col.cx = width * g_tmAveCharWidth;
    col.iSubItem = 0;
    ListView_InsertColumn(hwndLV, i, &col);
}

/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/
int LVAddText(HWND hwndLV, int col, char *sz, ...)
{
    LV_ITEM lvi;
    char    ach[80];

    wvsprintf(ach,sz,(LPSTR)(&sz+1));

    lvi.mask        = LVIF_TEXT;
    lvi.iSubItem    = 0;
    lvi.state       = 0;
    lvi.stateMask   = 0;
    lvi.pszText     = ach;
    lvi.cchTextMax  = 0;
    lvi.iImage      = 0;
    lvi.lParam      = 0;

    if (col == 0)
    {
        lvi.iItem    = 0x7FFF;
        lvi.iSubItem = 0;
        return ListView_InsertItem(hwndLV, &lvi);
    }
    else
    {
        lvi.iItem    = ListView_GetItemCount(hwndLV)-1;
        lvi.iSubItem = col;
        return ListView_SetItem(hwndLV, &lvi);
    }
}

/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/
HTREEITEM TVAddNode(HTREEITEM hParent, LPSTR szText, BOOL fKids, int iImage, SELCALLBACK Callback, LPARAM lParam1, LPARAM lParam2)
{
    TV_INSERTSTRUCT tvi;
    NODEINFO *pni;

    pni = (NODEINFO *)LocalAlloc(LPTR, sizeof(NODEINFO));

    if (pni == NULL)
        return NULL;

    pni->lParam1  = lParam1;
    pni->lParam2  = lParam2;
    pni->Callback = Callback;

    // Add Node to treeview
    tvi.hParent             = hParent;
    tvi.hInsertAfter        = TVI_LAST;
    tvi.item.mask           = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_CHILDREN;
    tvi.item.iImage         = iImage - IDI_FIRSTIMAGE;
    tvi.item.iSelectedImage = iImage - IDI_FIRSTIMAGE;
    tvi.item.lParam         = (LPARAM)pni;
    tvi.item.cChildren      = fKids;
    tvi.item.pszText        = szText;

    return TreeView_InsertItem(g_hwndTV, &tvi);
}
#pragma optimize("", on)
