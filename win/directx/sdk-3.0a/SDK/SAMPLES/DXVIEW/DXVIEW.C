/****************************************************************************

    Copyright (C) 1995-1996 Microsoft Corporation. All Rights Reserved.

    PROGRAM: dxview.c

    PURPOSE: DirectX Device Viewer

    FUNCTIONS:

    COMMENTS:

****************************************************************************/
//#define DX_3D
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <commctrl.h>

#define INITGUID
#include <objbase.h>
#include <initguid.h>

#include <ddraw.h>
//#include <mmreg.h>
#include <dsound.h>
#include <dplay.h>
#ifdef DX_3D
#include <d3d.h>
#endif
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

#ifdef DX_3D
IDirect3D    *lp3D;          // Direct3D object
GUID *       d3did;

IDirect3DDevice *lp3DDevice;
GUID *pid_for3D = 0;
#endif

/****************************************************************************
 ***************************************************************************/

#define DDCAPDEF(name,val,flag) {name, FIELD_OFFSET(DDCAPS,val), flag}
#define DDVALDEF(name,val)      {name, FIELD_OFFSET(DDCAPS,val), 0}
#define ROPDEF(name,rop)        DDCAPDEF(name,dwRops[((rop>>16)&0xFF)/32],(1<<((rop>>16)&0xFF)%32))

#define DSCAPDEF(name,val,flag) {name, FIELD_OFFSET(DSCAPS,val), flag}
#define DSVALDEF(name,val)      {name, FIELD_OFFSET(DSCAPS,val), 0}

#define DPCAPDEF(name,val,flag) {name, FIELD_OFFSET(DPCAPS,val), flag}
#define DPVALDEF(name,val)      {name, FIELD_OFFSET(DPCAPS,val), 0}

#ifdef DX_3D
#define D3CAPDEF(name,val,flag) {name, FIELD_OFFSET(D3DDEVICEDESC,val), flag}
#define D3VALDEF(name,val)      {name, FIELD_OFFSET(D3DDEVICEDESC,val), 0}
#endif

#define SURFCAPDEF(name,val,flag) {name, FIELD_OFFSET(DDSURFACEDESC,val), flag}
#define SURFVALDEF(name,val)      {name, FIELD_OFFSET(DDSURFACEDESC,val), 0}

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
    DDCAPDEF("BLTDEPTHFILL",              dwCaps, DDCAPS_BLTDEPTHFILL),
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
    DDCAPDEF("BANKSWITCHED",              dwCaps, DDCAPS_BANKSWITCHED),
    DDCAPDEF("CERTIFIED",                 dwCaps2,DDCAPS2_CERTIFIED),
    DDCAPDEF("NO2DDURING3DSCENE",                 dwCaps2,DDCAPS2_NO2DDURING3DSCENE),
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
    DDCAPDEF( "3DDEVICE",             ddsCaps.dwCaps, DDSCAPS_3DDEVICE),
    DDCAPDEF( "ALPHA",                ddsCaps.dwCaps, DDSCAPS_ALPHA),
    DDCAPDEF( "BACKBUFFER",           ddsCaps.dwCaps, DDSCAPS_BACKBUFFER),
    DDCAPDEF( "COMPLEX",              ddsCaps.dwCaps, DDSCAPS_COMPLEX),
    DDCAPDEF( "FLIP",                 ddsCaps.dwCaps, DDSCAPS_FLIP),
    DDCAPDEF( "FRONTBUFFER",          ddsCaps.dwCaps, DDSCAPS_FRONTBUFFER),
    DDCAPDEF( "MIPMAP",               ddsCaps.dwCaps, DDSCAPS_MIPMAP),
    DDCAPDEF( "OFFSCREENPLAIN",       ddsCaps.dwCaps, DDSCAPS_OFFSCREENPLAIN),
    DDCAPDEF( "OVERLAY",              ddsCaps.dwCaps, DDSCAPS_OVERLAY),
    DDCAPDEF( "PALETTE",              ddsCaps.dwCaps, DDSCAPS_PALETTE),
    DDCAPDEF( "PRIMARYSURFACE",       ddsCaps.dwCaps, DDSCAPS_PRIMARYSURFACE),
    DDCAPDEF( "PRIMARYSURFACELEFT",   ddsCaps.dwCaps, DDSCAPS_PRIMARYSURFACELEFT),
    DDCAPDEF( "SYSTEMMEMORY",         ddsCaps.dwCaps, DDSCAPS_SYSTEMMEMORY),
    DDCAPDEF( "TEXTURE",              ddsCaps.dwCaps, DDSCAPS_TEXTURE),
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
    {"FourCCFormat",        DDFourCCFormat,      (LPARAM)0},
    { NULL, 0, 0}
};

/****************************************************************************
 ***************************************************************************/
#ifdef DX_3D
CAPDEF ValidFlags[] =
{
    D3CAPDEF("COLORMODEL",            dwFlags, D3DDD_COLORMODEL),           
    D3CAPDEF("DEVCAPS",               dwFlags, D3DDD_DEVCAPS),              
    D3CAPDEF("TRANSFORMCAPS",         dwFlags, D3DDD_TRANSFORMCAPS),        
    D3CAPDEF("LIGHTINGCAPS",          dwFlags, D3DDD_LIGHTINGCAPS),         
    D3CAPDEF("BCLIPPING",             dwFlags, D3DDD_BCLIPPING),            
    D3CAPDEF("LINECAPS",              dwFlags, D3DDD_LINECAPS),             
    D3CAPDEF("TRICAPS",               dwFlags, D3DDD_TRICAPS),              
    D3CAPDEF("DEVICERENDERBITDEPTH",  dwFlags, D3DDD_DEVICERENDERBITDEPTH), 
    D3CAPDEF("DEVICEZBUFFERBITDEPTH", dwFlags, D3DDD_DEVICEZBUFFERBITDEPTH),
    D3CAPDEF("MAXBUFFERSIZE",         dwFlags, D3DDD_MAXBUFFERSIZE),        
    D3CAPDEF("MAXVERTEXCOUNT",        dwFlags, D3DDD_MAXVERTEXCOUNT),       
    {"",0,0}
};

/****************************************************************************
 ***************************************************************************/
CAPDEF ColorModel[] =
{
    D3CAPDEF("D3DCOLOR_MONO",            dcmColorModel, D3DCOLOR_MONO),           
    D3CAPDEF("D3DCOLOR_RGB",               dcmColorModel, D3DCOLOR_RGB),              
    {"",0,0}
 };

/****************************************************************************
 ***************************************************************************/
CAPDEF DevCaps[] =
{
    D3CAPDEF("SORTINCREASINGZ",     dwDevCaps, D3DDEVCAPS_SORTINCREASINGZ),     
    D3CAPDEF("SORTDECREASINGZ",     dwDevCaps, D3DDEVCAPS_SORTDECREASINGZ),     
    D3CAPDEF("SORTEXACT",           dwDevCaps, D3DDEVCAPS_SORTEXACT),           
    D3CAPDEF("EXECUTESYSTEMMEMORY", dwDevCaps, D3DDEVCAPS_EXECUTESYSTEMMEMORY), 
    D3CAPDEF("EXECUTEVIDEOMEMORY",  dwDevCaps, D3DDEVCAPS_EXECUTEVIDEOMEMORY),  
    D3CAPDEF("TLVERTEXSYSTEMEMORY", dwDevCaps, D3DDEVCAPS_TLVERTEXSYSTEMMEMORY),
    D3CAPDEF("TLVERTEXVIDEOMEMORY", dwDevCaps, D3DDEVCAPS_TLVERTEXVIDEOMEMORY), 
    D3CAPDEF("TEXTURESYSTEMMEMORY", dwDevCaps, D3DDEVCAPS_TEXTURESYSTEMMEMORY), 
    D3CAPDEF("TEXTUREVIDEOMEMORY",  dwDevCaps, D3DDEVCAPS_TEXTUREVIDEOMEMORY),  
    {"",0,0}
};

/****************************************************************************
 ***************************************************************************/
CAPDEF TransformCaps[] =
{
    D3CAPDEF("CLIP",  dtcTransformCaps.dwCaps, D3DTRANSFORMCAPS_CLIP),  
    {"",0,0}
};

/****************************************************************************
 ***************************************************************************/
CAPDEF LightingCaps[] =
{
    D3CAPDEF("D3DLIGHTINGMODEL_RGB",       dlcLightingCaps.dwLightingModel, D3DLIGHTINGMODEL_RGB),      
    D3CAPDEF("D3DLIGHTINGMODEL_MONO",      dlcLightingCaps.dwLightingModel, D3DLIGHTINGMODEL_MONO),     

    D3CAPDEF("D3DLIGHTCAPS_POINT",         dlcLightingCaps.dwCaps,          D3DLIGHTCAPS_POINT),        
    D3CAPDEF("D3DLIGHTCAPS_SPOT",          dlcLightingCaps.dwCaps,          D3DLIGHTCAPS_SPOT),         
    D3CAPDEF("D3DLIGHTCAPS_DIRECTIONAL",   dlcLightingCaps.dwCaps,          D3DLIGHTCAPS_DIRECTIONAL),  
    D3CAPDEF("D3DLIGHTCAPS_PARALLELPOINT", dlcLightingCaps.dwCaps,          D3DLIGHTCAPS_PARALLELPOINT),
    D3CAPDEF("D3DLIGHTCAPS_GLSPOT",        dlcLightingCaps.dwCaps,          D3DLIGHTCAPS_GLSPOT),       
    D3VALDEF("dwNumLights",                dlcLightingCaps.dwNumLights),   
    {"",0,0}
};

/****************************************************************************
 ***************************************************************************/
CAPDEF BClipping[] =
{
    D3CAPDEF("bClipping", bClipping, TRUE),
    {"",0,0}
};

/****************************************************************************
 ***************************************************************************/
CAPDEF LineCaps[] =
{
    D3CAPDEF("D3DPMISCCAPS_MASKPLANES",            dpcLineCaps.dwMiscCaps,           D3DPMISCCAPS_MASKPLANES),         
    D3CAPDEF("D3DPMISCCAPS_MASKZ",                 dpcLineCaps.dwMiscCaps,           D3DPMISCCAPS_MASKZ),              
    D3CAPDEF("D3DPMISCCAPS_LINEPATTERNREP",        dpcLineCaps.dwMiscCaps,           D3DPMISCCAPS_LINEPATTERNREP),     
    D3CAPDEF("D3DPMISCCAPS_CONFORMANT",            dpcLineCaps.dwMiscCaps,           D3DPMISCCAPS_CONFORMANT),         
    D3CAPDEF("D3DPMISCCAPS_CULLNONE",              dpcLineCaps.dwMiscCaps,           D3DPMISCCAPS_CULLNONE),           
    D3CAPDEF("D3DPMISCCAPS_CULLCW",                dpcLineCaps.dwMiscCaps,           D3DPMISCCAPS_CULLCW),             
    D3CAPDEF("D3DPMISCCAPS_CULLCCW",               dpcLineCaps.dwMiscCaps,           D3DPMISCCAPS_CULLCCW),            
    
    D3CAPDEF("D3DPRASTERCAPS_DITHER",              dpcLineCaps.dwRasterCaps,         D3DPRASTERCAPS_DITHER),           
    D3CAPDEF("D3DPRASTERCAPS_ROP2",                dpcLineCaps.dwRasterCaps,         D3DPRASTERCAPS_ROP2),             
    D3CAPDEF("D3DPRASTERCAPS_XOR",                 dpcLineCaps.dwRasterCaps,         D3DPRASTERCAPS_XOR),              
    D3CAPDEF("D3DPRASTERCAPS_PAT",                 dpcLineCaps.dwRasterCaps,         D3DPRASTERCAPS_PAT),              
    D3CAPDEF("D3DPRASTERCAPS_ZTEST",               dpcLineCaps.dwRasterCaps,         D3DPRASTERCAPS_ZTEST),            
    D3CAPDEF("D3DPRASTERCAPS_SUBPIXEL",            dpcLineCaps.dwRasterCaps,         D3DPRASTERCAPS_SUBPIXEL),         
    D3CAPDEF("D3DPRASTERCAPS_SUBPIXELX",           dpcLineCaps.dwRasterCaps,         D3DPRASTERCAPS_SUBPIXELX),        
    D3CAPDEF("D3DPRASTERCAPS_FOGVERTEX",           dpcLineCaps.dwRasterCaps,         D3DPRASTERCAPS_FOGVERTEX),        
    D3CAPDEF("D3DPRASTERCAPS_FOGTABLE",            dpcLineCaps.dwRasterCaps,         D3DPRASTERCAPS_FOGTABLE),         
    
    D3CAPDEF("D3DPCMPCAPS_NEVER",                  dpcLineCaps.dwZCmpCaps,           D3DPCMPCAPS_NEVER                 ),
    D3CAPDEF("D3DPCMPCAPS_LESS",                   dpcLineCaps.dwZCmpCaps,           D3DPCMPCAPS_LESS                  ),
    D3CAPDEF("D3DPCMPCAPS_EQUAL",                  dpcLineCaps.dwZCmpCaps,           D3DPCMPCAPS_EQUAL                 ),
    D3CAPDEF("D3DPCMPCAPS_LESSEQUAL",              dpcLineCaps.dwZCmpCaps,           D3DPCMPCAPS_LESSEQUAL             ),
    D3CAPDEF("D3DPCMPCAPS_GREATER",                dpcLineCaps.dwZCmpCaps,           D3DPCMPCAPS_GREATER               ),
    D3CAPDEF("D3DPCMPCAPS_NOTEQUAL",               dpcLineCaps.dwZCmpCaps,           D3DPCMPCAPS_NOTEQUAL              ),
    D3CAPDEF("D3DPCMPCAPS_GREATEREQUAL",           dpcLineCaps.dwZCmpCaps,           D3DPCMPCAPS_GREATEREQUAL          ),
    D3CAPDEF("D3DPCMPCAPS_ALWAYS",                 dpcLineCaps.dwZCmpCaps,           D3DPCMPCAPS_ALWAYS                ),
    
    D3CAPDEF("D3DPBLENDCAPS_ZERO",                 dpcLineCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_ZERO                ),
    D3CAPDEF("D3DPBLENDCAPS_ONE",                  dpcLineCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_ONE                 ),
    D3CAPDEF("D3DPBLENDCAPS_SRCCOLOR",             dpcLineCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_SRCCOLOR            ),
    D3CAPDEF("D3DPBLENDCAPS_INVSRCCOLOR",          dpcLineCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_INVSRCCOLOR         ),
    D3CAPDEF("D3DPBLENDCAPS_SRCALPHA",             dpcLineCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_SRCALPHA            ),
    D3CAPDEF("D3DPBLENDCAPS_INVSRCALPHA",          dpcLineCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_INVSRCALPHA         ),
    D3CAPDEF("D3DPBLENDCAPS_DESTALPHA",            dpcLineCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_DESTALPHA           ),
    D3CAPDEF("D3DPBLENDCAPS_INVDESTALPHA",         dpcLineCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_INVDESTALPHA        ),
    D3CAPDEF("D3DPBLENDCAPS_DESTCOLOR",            dpcLineCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_DESTCOLOR           ),
    D3CAPDEF("D3DPBLENDCAPS_INVDESTCOLOR",         dpcLineCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_INVDESTCOLOR        ),
    D3CAPDEF("D3DPBLENDCAPS_SRCALPHASAT",          dpcLineCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_SRCALPHASAT         ),
    D3CAPDEF("D3DPBLENDCAPS_BOTHSRCALPHA",         dpcLineCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_BOTHSRCALPHA        ),
    D3CAPDEF("D3DPBLENDCAPS_BOTHINVSRCALPHA",      dpcLineCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_BOTHINVSRCALPHA     ),

    D3CAPDEF("D3DPBLENDCAPS_ZERO",                 dpcLineCaps.dwDestBlendCaps,      D3DPBLENDCAPS_ZERO                ),
    D3CAPDEF("D3DPBLENDCAPS_ONE",                  dpcLineCaps.dwDestBlendCaps,      D3DPBLENDCAPS_ONE                 ),
    D3CAPDEF("D3DPBLENDCAPS_SRCCOLOR",             dpcLineCaps.dwDestBlendCaps,      D3DPBLENDCAPS_SRCCOLOR            ),
    D3CAPDEF("D3DPBLENDCAPS_INVSRCCOLOR",          dpcLineCaps.dwDestBlendCaps,      D3DPBLENDCAPS_INVSRCCOLOR         ),
    D3CAPDEF("D3DPBLENDCAPS_SRCALPHA",             dpcLineCaps.dwDestBlendCaps,      D3DPBLENDCAPS_SRCALPHA            ),
    D3CAPDEF("D3DPBLENDCAPS_INVSRCALPHA",          dpcLineCaps.dwDestBlendCaps,      D3DPBLENDCAPS_INVSRCALPHA         ),
    D3CAPDEF("D3DPBLENDCAPS_DESTALPHA",            dpcLineCaps.dwDestBlendCaps,      D3DPBLENDCAPS_DESTALPHA           ),
    D3CAPDEF("D3DPBLENDCAPS_INVDESTALPHA",         dpcLineCaps.dwDestBlendCaps,      D3DPBLENDCAPS_INVDESTALPHA        ),
    D3CAPDEF("D3DPBLENDCAPS_DESTCOLOR",            dpcLineCaps.dwDestBlendCaps,      D3DPBLENDCAPS_DESTCOLOR           ),
    D3CAPDEF("D3DPBLENDCAPS_INVDESTCOLOR",         dpcLineCaps.dwDestBlendCaps,      D3DPBLENDCAPS_INVDESTCOLOR        ),
    D3CAPDEF("D3DPBLENDCAPS_SRCALPHASAT",          dpcLineCaps.dwDestBlendCaps,      D3DPBLENDCAPS_SRCALPHASAT         ),
    D3CAPDEF("D3DPBLENDCAPS_BOTHSRCALPHA",         dpcLineCaps.dwDestBlendCaps,      D3DPBLENDCAPS_BOTHSRCALPHA        ),
    D3CAPDEF("D3DPBLENDCAPS_BOTHINVSRCALPHA",      dpcLineCaps.dwDestBlendCaps,      D3DPBLENDCAPS_BOTHINVSRCALPHA     ),
    
    D3CAPDEF("D3DPCMPCAPS_NEVER",                  dpcLineCaps.dwAlphaCmpCaps,       D3DPCMPCAPS_NEVER                 ),
    D3CAPDEF("D3DPCMPCAPS_LESS",                   dpcLineCaps.dwAlphaCmpCaps,       D3DPCMPCAPS_LESS                  ),
    D3CAPDEF("D3DPCMPCAPS_EQUAL",                  dpcLineCaps.dwAlphaCmpCaps,       D3DPCMPCAPS_EQUAL                 ),
    D3CAPDEF("D3DPCMPCAPS_LESSEQUAL",              dpcLineCaps.dwAlphaCmpCaps,       D3DPCMPCAPS_LESSEQUAL             ),
    D3CAPDEF("D3DPCMPCAPS_GREATER",                dpcLineCaps.dwAlphaCmpCaps,       D3DPCMPCAPS_GREATER               ),
    D3CAPDEF("D3DPCMPCAPS_NOTEQUAL",               dpcLineCaps.dwAlphaCmpCaps,       D3DPCMPCAPS_NOTEQUAL              ),
    D3CAPDEF("D3DPCMPCAPS_GREATEREQUAL",           dpcLineCaps.dwAlphaCmpCaps,       D3DPCMPCAPS_GREATEREQUAL          ),
    D3CAPDEF("D3DPCMPCAPS_ALWAYS",                 dpcLineCaps.dwAlphaCmpCaps,       D3DPCMPCAPS_ALWAYS                ),
    
    D3CAPDEF("D3DPSHADECAPS_COLORFLATMONO",        dpcLineCaps.dwShadeCaps,          D3DPSHADECAPS_COLORFLATMONO       ),
    D3CAPDEF("D3DPSHADECAPS_COLORFLATRGB",         dpcLineCaps.dwShadeCaps,          D3DPSHADECAPS_COLORFLATRGB        ),
    D3CAPDEF("D3DPSHADECAPS_COLORGOURAUDMONO",     dpcLineCaps.dwShadeCaps,          D3DPSHADECAPS_COLORGOURAUDMONO    ),
    D3CAPDEF("D3DPSHADECAPS_COLORGOURAUDRGB",      dpcLineCaps.dwShadeCaps,          D3DPSHADECAPS_COLORGOURAUDRGB     ),
    D3CAPDEF("D3DPSHADECAPS_COLORPHONGMONO",       dpcLineCaps.dwShadeCaps,          D3DPSHADECAPS_COLORPHONGMONO      ),
    D3CAPDEF("D3DPSHADECAPS_COLORPHONGRGB",        dpcLineCaps.dwShadeCaps,          D3DPSHADECAPS_COLORPHONGRGB       ),
    
    D3CAPDEF("D3DPSHADECAPS_SPECULARFLATMONO",     dpcLineCaps.dwShadeCaps,          D3DPSHADECAPS_SPECULARFLATMONO    ),
    D3CAPDEF("D3DPSHADECAPS_SPECULARFLATRGB",      dpcLineCaps.dwShadeCaps,          D3DPSHADECAPS_SPECULARFLATRGB     ),
    D3CAPDEF("D3DPSHADECAPS_SPECULARGOURAUDMONO",  dpcLineCaps.dwShadeCaps,          D3DPSHADECAPS_SPECULARGOURAUDMONO ),
    D3CAPDEF("D3DPSHADECAPS_SPECULARGOURAUDRGB",   dpcLineCaps.dwShadeCaps,          D3DPSHADECAPS_SPECULARGOURAUDRGB  ),
    D3CAPDEF("D3DPSHADECAPS_SPECULARPHONGMONO",    dpcLineCaps.dwShadeCaps,          D3DPSHADECAPS_SPECULARPHONGMONO   ),
    D3CAPDEF("D3DPSHADECAPS_SPECULARPHONGRGB",     dpcLineCaps.dwShadeCaps,          D3DPSHADECAPS_SPECULARPHONGRGB    ),
    
    D3CAPDEF("D3DPSHADECAPS_ALPHAFLATBLEND",       dpcLineCaps.dwShadeCaps,          D3DPSHADECAPS_ALPHAFLATBLEND      ),
    D3CAPDEF("D3DPSHADECAPS_ALPHAFLATSTIPPLED",    dpcLineCaps.dwShadeCaps,          D3DPSHADECAPS_ALPHAFLATSTIPPLED   ),
    D3CAPDEF("D3DPSHADECAPS_ALPHAGOURAUDBLEND",    dpcLineCaps.dwShadeCaps,          D3DPSHADECAPS_ALPHAGOURAUDBLEND   ),
    D3CAPDEF("D3DPSHADECAPS_ALPHAGOURAUDSTIPPLED", dpcLineCaps.dwShadeCaps,          D3DPSHADECAPS_ALPHAGOURAUDSTIPPLED),
    D3CAPDEF("D3DPSHADECAPS_ALPHAPHONGBLEND",      dpcLineCaps.dwShadeCaps,          D3DPSHADECAPS_ALPHAPHONGBLEND     ),
    D3CAPDEF("D3DPSHADECAPS_ALPHAPHONGSTIPPLED",   dpcLineCaps.dwShadeCaps,          D3DPSHADECAPS_ALPHAPHONGSTIPPLED  ),
    
    D3CAPDEF("D3DPSHADECAPS_FOGFLAT",              dpcLineCaps.dwShadeCaps,          D3DPSHADECAPS_FOGFLAT             ),
    D3CAPDEF("D3DPSHADECAPS_FOGGOURAUD",           dpcLineCaps.dwShadeCaps,          D3DPSHADECAPS_FOGGOURAUD          ),
    D3CAPDEF("D3DPSHADECAPS_FOGPHONG",             dpcLineCaps.dwShadeCaps,          D3DPSHADECAPS_FOGPHONG            ),
    
    D3CAPDEF("D3DPTEXTURECAPS_PERSPECTIVE",        dpcLineCaps.dwTextureCaps,        D3DPTEXTURECAPS_PERSPECTIVE       ),
    D3CAPDEF("D3DPTEXTURECAPS_POW2",               dpcLineCaps.dwTextureCaps,        D3DPTEXTURECAPS_POW2              ),
    D3CAPDEF("D3DPTEXTURECAPS_ALPHA",              dpcLineCaps.dwTextureCaps,        D3DPTEXTURECAPS_ALPHA             ),
    D3CAPDEF("D3DPTEXTURECAPS_TRANSPARENCY",       dpcLineCaps.dwTextureCaps,        D3DPTEXTURECAPS_TRANSPARENCY      ),
    D3CAPDEF("D3DPTEXTURECAPS_BORDER",             dpcLineCaps.dwTextureCaps,        D3DPTEXTURECAPS_BORDER            ),
    D3CAPDEF("D3DPTEXTURECAPS_SQUAREONLY",         dpcLineCaps.dwTextureCaps,        D3DPTEXTURECAPS_SQUAREONLY        ),
    
    
    D3CAPDEF("D3DPTFILTERCAPS_NEAREST",            dpcLineCaps.dwTextureFilterCaps,  D3DPTFILTERCAPS_NEAREST           ),
    D3CAPDEF("D3DPTFILTERCAPS_LINEAR",             dpcLineCaps.dwTextureFilterCaps,  D3DPTFILTERCAPS_LINEAR            ),
    D3CAPDEF("D3DPTFILTERCAPS_MIPNEAREST",         dpcLineCaps.dwTextureFilterCaps,  D3DPTFILTERCAPS_MIPNEAREST        ),
    D3CAPDEF("D3DPTFILTERCAPS_MIPLINEAR",          dpcLineCaps.dwTextureFilterCaps,  D3DPTFILTERCAPS_MIPLINEAR         ),
    D3CAPDEF("D3DPTFILTERCAPS_LINEARMIPNEAREST",   dpcLineCaps.dwTextureFilterCaps,  D3DPTFILTERCAPS_LINEARMIPNEAREST  ),
    D3CAPDEF("D3DPTFILTERCAPS_LINEARMIPLINEAR",    dpcLineCaps.dwTextureFilterCaps,  D3DPTFILTERCAPS_LINEARMIPLINEAR   ),
    
    
    D3CAPDEF("D3DPTBLENDCAPS_DECAL",               dpcLineCaps.dwTextureBlendCaps,   D3DPTBLENDCAPS_DECAL              ),
    D3CAPDEF("D3DPTBLENDCAPS_MODULATE",            dpcLineCaps.dwTextureBlendCaps,   D3DPTBLENDCAPS_MODULATE           ),
    D3CAPDEF("D3DPTBLENDCAPS_DECALALPHA",          dpcLineCaps.dwTextureBlendCaps,   D3DPTBLENDCAPS_DECALALPHA         ),
    D3CAPDEF("D3DPTBLENDCAPS_MODULATEALPHA",       dpcLineCaps.dwTextureBlendCaps,   D3DPTBLENDCAPS_MODULATEALPHA      ),
    D3CAPDEF("D3DPTBLENDCAPS_DECALMASK",           dpcLineCaps.dwTextureBlendCaps,   D3DPTBLENDCAPS_DECALMASK          ),
    D3CAPDEF("D3DPTBLENDCAPS_MODULATEMASK",        dpcLineCaps.dwTextureBlendCaps,   D3DPTBLENDCAPS_MODULATEMASK       ),
    D3CAPDEF("D3DPTBLENDCAPS_COPY",                dpcLineCaps.dwTextureBlendCaps,   D3DPTBLENDCAPS_COPY               ),
    
    D3CAPDEF("D3DPTADDRESSCAPS_WRAP",              dpcLineCaps.dwTextureAddressCaps, D3DPTADDRESSCAPS_WRAP             ),
    D3CAPDEF("D3DPTADDRESSCAPS_MIRROR",            dpcLineCaps.dwTextureAddressCaps, D3DPTADDRESSCAPS_MIRROR           ),
    D3CAPDEF("D3DPTADDRESSCAPS_CLAMP",             dpcLineCaps.dwTextureAddressCaps, D3DPTADDRESSCAPS_CLAMP            ),
    {"",0,0}
};

/****************************************************************************
 ***************************************************************************/
CAPDEF TriCaps[] =
{
    D3CAPDEF("D3DPMISCCAPS_MASKPLANES",            dpcTriCaps.dwMiscCaps,           D3DPMISCCAPS_MASKPLANES),         
    D3CAPDEF("D3DPMISCCAPS_MASKZ",                 dpcTriCaps.dwMiscCaps,           D3DPMISCCAPS_MASKZ),              
    D3CAPDEF("D3DPMISCCAPS_LINEPATTERNREP",        dpcTriCaps.dwMiscCaps,           D3DPMISCCAPS_LINEPATTERNREP),     
    D3CAPDEF("D3DPMISCCAPS_CONFORMANT",            dpcTriCaps.dwMiscCaps,           D3DPMISCCAPS_CONFORMANT),         
    D3CAPDEF("D3DPMISCCAPS_CULLNONE",              dpcTriCaps.dwMiscCaps,           D3DPMISCCAPS_CULLNONE),           
    D3CAPDEF("D3DPMISCCAPS_CULLCW",                dpcTriCaps.dwMiscCaps,           D3DPMISCCAPS_CULLCW),             
    D3CAPDEF("D3DPMISCCAPS_CULLCCW",               dpcTriCaps.dwMiscCaps,           D3DPMISCCAPS_CULLCCW),            
    
    D3CAPDEF("D3DPRASTERCAPS_DITHER",              dpcTriCaps.dwRasterCaps,         D3DPRASTERCAPS_DITHER),           
    D3CAPDEF("D3DPRASTERCAPS_ROP2",                dpcTriCaps.dwRasterCaps,         D3DPRASTERCAPS_ROP2),             
    D3CAPDEF("D3DPRASTERCAPS_XOR",                 dpcTriCaps.dwRasterCaps,         D3DPRASTERCAPS_XOR),              
    D3CAPDEF("D3DPRASTERCAPS_PAT",                 dpcTriCaps.dwRasterCaps,         D3DPRASTERCAPS_PAT),              
    D3CAPDEF("D3DPRASTERCAPS_ZTEST",               dpcTriCaps.dwRasterCaps,         D3DPRASTERCAPS_ZTEST),            
    D3CAPDEF("D3DPRASTERCAPS_SUBPIXEL",            dpcTriCaps.dwRasterCaps,         D3DPRASTERCAPS_SUBPIXEL),         
    D3CAPDEF("D3DPRASTERCAPS_SUBPIXELX",           dpcTriCaps.dwRasterCaps,         D3DPRASTERCAPS_SUBPIXELX),        
    D3CAPDEF("D3DPRASTERCAPS_FOGVERTEX",           dpcTriCaps.dwRasterCaps,         D3DPRASTERCAPS_FOGVERTEX),        
    D3CAPDEF("D3DPRASTERCAPS_FOGTABLE",            dpcTriCaps.dwRasterCaps,         D3DPRASTERCAPS_FOGTABLE),         
    
    D3CAPDEF("D3DPCMPCAPS_NEVER",                  dpcTriCaps.dwZCmpCaps,           D3DPCMPCAPS_NEVER                 ),
    D3CAPDEF("D3DPCMPCAPS_LESS",                   dpcTriCaps.dwZCmpCaps,           D3DPCMPCAPS_LESS                  ),
    D3CAPDEF("D3DPCMPCAPS_EQUAL",                  dpcTriCaps.dwZCmpCaps,           D3DPCMPCAPS_EQUAL                 ),
    D3CAPDEF("D3DPCMPCAPS_LESSEQUAL",              dpcTriCaps.dwZCmpCaps,           D3DPCMPCAPS_LESSEQUAL             ),
    D3CAPDEF("D3DPCMPCAPS_GREATER",                dpcTriCaps.dwZCmpCaps,           D3DPCMPCAPS_GREATER               ),
    D3CAPDEF("D3DPCMPCAPS_NOTEQUAL",               dpcTriCaps.dwZCmpCaps,           D3DPCMPCAPS_NOTEQUAL              ),
    D3CAPDEF("D3DPCMPCAPS_GREATEREQUAL",           dpcTriCaps.dwZCmpCaps,           D3DPCMPCAPS_GREATEREQUAL          ),
    D3CAPDEF("D3DPCMPCAPS_ALWAYS",                 dpcTriCaps.dwZCmpCaps,           D3DPCMPCAPS_ALWAYS                ),
    
    D3CAPDEF("D3DPBLENDCAPS_ZERO",                 dpcTriCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_ZERO                ),
    D3CAPDEF("D3DPBLENDCAPS_ONE",                  dpcTriCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_ONE                 ),
    D3CAPDEF("D3DPBLENDCAPS_SRCCOLOR",             dpcTriCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_SRCCOLOR            ),
    D3CAPDEF("D3DPBLENDCAPS_INVSRCCOLOR",          dpcTriCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_INVSRCCOLOR         ),
    D3CAPDEF("D3DPBLENDCAPS_SRCALPHA",             dpcTriCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_SRCALPHA            ),
    D3CAPDEF("D3DPBLENDCAPS_INVSRCALPHA",          dpcTriCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_INVSRCALPHA         ),
    D3CAPDEF("D3DPBLENDCAPS_DESTALPHA",            dpcTriCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_DESTALPHA           ),
    D3CAPDEF("D3DPBLENDCAPS_INVDESTALPHA",         dpcTriCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_INVDESTALPHA        ),
    D3CAPDEF("D3DPBLENDCAPS_DESTCOLOR",            dpcTriCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_DESTCOLOR           ),
    D3CAPDEF("D3DPBLENDCAPS_INVDESTCOLOR",         dpcTriCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_INVDESTCOLOR        ),
    D3CAPDEF("D3DPBLENDCAPS_SRCALPHASAT",          dpcTriCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_SRCALPHASAT         ),
    D3CAPDEF("D3DPBLENDCAPS_BOTHSRCALPHA",         dpcTriCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_BOTHSRCALPHA        ),
    D3CAPDEF("D3DPBLENDCAPS_BOTHINVSRCALPHA",      dpcTriCaps.dwSrcBlendCaps,       D3DPBLENDCAPS_BOTHINVSRCALPHA     ),
    
    D3CAPDEF("D3DPBLENDCAPS_ZERO",                 dpcTriCaps.dwDestBlendCaps,      D3DPBLENDCAPS_ZERO                ),
    D3CAPDEF("D3DPBLENDCAPS_ONE",                  dpcTriCaps.dwDestBlendCaps,      D3DPBLENDCAPS_ONE                 ),
    D3CAPDEF("D3DPBLENDCAPS_SRCCOLOR",             dpcTriCaps.dwDestBlendCaps,      D3DPBLENDCAPS_SRCCOLOR            ),
    D3CAPDEF("D3DPBLENDCAPS_INVSRCCOLOR",          dpcTriCaps.dwDestBlendCaps,      D3DPBLENDCAPS_INVSRCCOLOR         ),
    D3CAPDEF("D3DPBLENDCAPS_SRCALPHA",             dpcTriCaps.dwDestBlendCaps,      D3DPBLENDCAPS_SRCALPHA            ),
    D3CAPDEF("D3DPBLENDCAPS_INVSRCALPHA",          dpcTriCaps.dwDestBlendCaps,      D3DPBLENDCAPS_INVSRCALPHA         ),
    D3CAPDEF("D3DPBLENDCAPS_DESTALPHA",            dpcTriCaps.dwDestBlendCaps,      D3DPBLENDCAPS_DESTALPHA           ),
    D3CAPDEF("D3DPBLENDCAPS_INVDESTALPHA",         dpcTriCaps.dwDestBlendCaps,      D3DPBLENDCAPS_INVDESTALPHA        ),
    D3CAPDEF("D3DPBLENDCAPS_DESTCOLOR",            dpcTriCaps.dwDestBlendCaps,      D3DPBLENDCAPS_DESTCOLOR           ),
    D3CAPDEF("D3DPBLENDCAPS_INVDESTCOLOR",         dpcTriCaps.dwDestBlendCaps,      D3DPBLENDCAPS_INVDESTCOLOR        ),
    D3CAPDEF("D3DPBLENDCAPS_SRCALPHASAT",          dpcTriCaps.dwDestBlendCaps,      D3DPBLENDCAPS_SRCALPHASAT         ),
    D3CAPDEF("D3DPBLENDCAPS_BOTHSRCALPHA",         dpcTriCaps.dwDestBlendCaps,      D3DPBLENDCAPS_BOTHSRCALPHA        ),
    D3CAPDEF("D3DPBLENDCAPS_BOTHINVSRCALPHA",      dpcTriCaps.dwDestBlendCaps,      D3DPBLENDCAPS_BOTHINVSRCALPHA     ),
    
    D3CAPDEF("D3DPCMPCAPS_NEVER",                  dpcTriCaps.dwAlphaCmpCaps,       D3DPCMPCAPS_NEVER                 ),
    D3CAPDEF("D3DPCMPCAPS_LESS",                   dpcTriCaps.dwAlphaCmpCaps,       D3DPCMPCAPS_LESS                  ),
    D3CAPDEF("D3DPCMPCAPS_EQUAL",                  dpcTriCaps.dwAlphaCmpCaps,       D3DPCMPCAPS_EQUAL                 ),
    D3CAPDEF("D3DPCMPCAPS_LESSEQUAL",              dpcTriCaps.dwAlphaCmpCaps,       D3DPCMPCAPS_LESSEQUAL             ),
    D3CAPDEF("D3DPCMPCAPS_GREATER",                dpcTriCaps.dwAlphaCmpCaps,       D3DPCMPCAPS_GREATER               ),
    D3CAPDEF("D3DPCMPCAPS_NOTEQUAL",               dpcTriCaps.dwAlphaCmpCaps,       D3DPCMPCAPS_NOTEQUAL              ),
    D3CAPDEF("D3DPCMPCAPS_GREATEREQUAL",           dpcTriCaps.dwAlphaCmpCaps,       D3DPCMPCAPS_GREATEREQUAL          ),
    D3CAPDEF("D3DPCMPCAPS_ALWAYS",                 dpcTriCaps.dwAlphaCmpCaps,       D3DPCMPCAPS_ALWAYS                ),
    
    D3CAPDEF("D3DPSHADECAPS_COLORFLATMONO",        dpcTriCaps.dwShadeCaps,          D3DPSHADECAPS_COLORFLATMONO       ),
    D3CAPDEF("D3DPSHADECAPS_COLORFLATRGB",         dpcTriCaps.dwShadeCaps,          D3DPSHADECAPS_COLORFLATRGB        ),
    D3CAPDEF("D3DPSHADECAPS_COLORGOURAUDMONO",     dpcTriCaps.dwShadeCaps,          D3DPSHADECAPS_COLORGOURAUDMONO    ),
    D3CAPDEF("D3DPSHADECAPS_COLORGOURAUDRGB",      dpcTriCaps.dwShadeCaps,          D3DPSHADECAPS_COLORGOURAUDRGB     ),
    D3CAPDEF("D3DPSHADECAPS_COLORPHONGMONO",       dpcTriCaps.dwShadeCaps,          D3DPSHADECAPS_COLORPHONGMONO      ),
    D3CAPDEF("D3DPSHADECAPS_COLORPHONGRGB",        dpcTriCaps.dwShadeCaps,          D3DPSHADECAPS_COLORPHONGRGB       ),
    
    D3CAPDEF("D3DPSHADECAPS_SPECULARFLATMONO",     dpcTriCaps.dwShadeCaps,          D3DPSHADECAPS_SPECULARFLATMONO    ),
    D3CAPDEF("D3DPSHADECAPS_SPECULARFLATRGB",      dpcTriCaps.dwShadeCaps,          D3DPSHADECAPS_SPECULARFLATRGB     ),
    D3CAPDEF("D3DPSHADECAPS_SPECULARGOURAUDMONO",  dpcTriCaps.dwShadeCaps,          D3DPSHADECAPS_SPECULARGOURAUDMONO ),
    D3CAPDEF("D3DPSHADECAPS_SPECULARGOURAUDRGB",   dpcTriCaps.dwShadeCaps,          D3DPSHADECAPS_SPECULARGOURAUDRGB  ),
    D3CAPDEF("D3DPSHADECAPS_SPECULARPHONGMONO",    dpcTriCaps.dwShadeCaps,          D3DPSHADECAPS_SPECULARPHONGMONO   ),
    D3CAPDEF("D3DPSHADECAPS_SPECULARPHONGRGB",     dpcTriCaps.dwShadeCaps,          D3DPSHADECAPS_SPECULARPHONGRGB    ),
    
    D3CAPDEF("D3DPSHADECAPS_ALPHAFLATBLEND",       dpcTriCaps.dwShadeCaps,          D3DPSHADECAPS_ALPHAFLATBLEND      ),
    D3CAPDEF("D3DPSHADECAPS_ALPHAFLATSTIPPLED",    dpcTriCaps.dwShadeCaps,          D3DPSHADECAPS_ALPHAFLATSTIPPLED   ),
    D3CAPDEF("D3DPSHADECAPS_ALPHAGOURAUDBLEND",    dpcTriCaps.dwShadeCaps,          D3DPSHADECAPS_ALPHAGOURAUDBLEND   ),
    D3CAPDEF("D3DPSHADECAPS_ALPHAGOURAUDSTIPPLED", dpcTriCaps.dwShadeCaps,          D3DPSHADECAPS_ALPHAGOURAUDSTIPPLED),
    D3CAPDEF("D3DPSHADECAPS_ALPHAPHONGBLEND",      dpcTriCaps.dwShadeCaps,          D3DPSHADECAPS_ALPHAPHONGBLEND     ),
    D3CAPDEF("D3DPSHADECAPS_ALPHAPHONGSTIPPLED",   dpcTriCaps.dwShadeCaps,          D3DPSHADECAPS_ALPHAPHONGSTIPPLED  ),
    
    D3CAPDEF("D3DPSHADECAPS_FOGFLAT",              dpcTriCaps.dwShadeCaps,          D3DPSHADECAPS_FOGFLAT             ),
    D3CAPDEF("D3DPSHADECAPS_FOGGOURAUD",           dpcTriCaps.dwShadeCaps,          D3DPSHADECAPS_FOGGOURAUD          ),
    D3CAPDEF("D3DPSHADECAPS_FOGPHONG",             dpcTriCaps.dwShadeCaps,          D3DPSHADECAPS_FOGPHONG            ),
    
    D3CAPDEF("D3DPTEXTURECAPS_PERSPECTIVE",        dpcTriCaps.dwTextureCaps,        D3DPTEXTURECAPS_PERSPECTIVE       ),
    D3CAPDEF("D3DPTEXTURECAPS_POW2",               dpcTriCaps.dwTextureCaps,        D3DPTEXTURECAPS_POW2              ),
    D3CAPDEF("D3DPTEXTURECAPS_ALPHA",              dpcTriCaps.dwTextureCaps,        D3DPTEXTURECAPS_ALPHA             ),
    D3CAPDEF("D3DPTEXTURECAPS_TRANSPARENCY",       dpcTriCaps.dwTextureCaps,        D3DPTEXTURECAPS_TRANSPARENCY      ),
    D3CAPDEF("D3DPTEXTURECAPS_BORDER",             dpcTriCaps.dwTextureCaps,        D3DPTEXTURECAPS_BORDER            ),
    D3CAPDEF("D3DPTEXTURECAPS_SQUAREONLY",         dpcTriCaps.dwTextureCaps,        D3DPTEXTURECAPS_SQUAREONLY        ),
    
    
    D3CAPDEF("D3DPTFILTERCAPS_NEAREST",            dpcTriCaps.dwTextureFilterCaps,  D3DPTFILTERCAPS_NEAREST           ),
    D3CAPDEF("D3DPTFILTERCAPS_LINEAR",             dpcTriCaps.dwTextureFilterCaps,  D3DPTFILTERCAPS_LINEAR            ),
    D3CAPDEF("D3DPTFILTERCAPS_MIPNEAREST",         dpcTriCaps.dwTextureFilterCaps,  D3DPTFILTERCAPS_MIPNEAREST        ),
    D3CAPDEF("D3DPTFILTERCAPS_MIPLINEAR",          dpcTriCaps.dwTextureFilterCaps,  D3DPTFILTERCAPS_MIPLINEAR         ),
    D3CAPDEF("D3DPTFILTERCAPS_LINEARMIPNEAREST",   dpcTriCaps.dwTextureFilterCaps,  D3DPTFILTERCAPS_LINEARMIPNEAREST  ),
    D3CAPDEF("D3DPTFILTERCAPS_LINEARMIPLINEAR",    dpcTriCaps.dwTextureFilterCaps,  D3DPTFILTERCAPS_LINEARMIPLINEAR   ),
    
    
    D3CAPDEF("D3DPTBLENDCAPS_DECAL",               dpcTriCaps.dwTextureBlendCaps,   D3DPTBLENDCAPS_DECAL              ),
    D3CAPDEF("D3DPTBLENDCAPS_MODULATE",            dpcTriCaps.dwTextureBlendCaps,   D3DPTBLENDCAPS_MODULATE           ),
    D3CAPDEF("D3DPTBLENDCAPS_DECALALPHA",          dpcTriCaps.dwTextureBlendCaps,   D3DPTBLENDCAPS_DECALALPHA         ),
    D3CAPDEF("D3DPTBLENDCAPS_MODULATEALPHA",       dpcTriCaps.dwTextureBlendCaps,   D3DPTBLENDCAPS_MODULATEALPHA      ),
    D3CAPDEF("D3DPTBLENDCAPS_DECALMASK",           dpcTriCaps.dwTextureBlendCaps,   D3DPTBLENDCAPS_DECALMASK          ),
    D3CAPDEF("D3DPTBLENDCAPS_MODULATEMASK",        dpcTriCaps.dwTextureBlendCaps,   D3DPTBLENDCAPS_MODULATEMASK       ),
    D3CAPDEF("D3DPTBLENDCAPS_COPY",                dpcTriCaps.dwTextureBlendCaps,   D3DPTBLENDCAPS_COPY               ),
    
    D3CAPDEF("D3DPTADDRESSCAPS_WRAP",              dpcTriCaps.dwTextureAddressCaps, D3DPTADDRESSCAPS_WRAP             ),
    D3CAPDEF("D3DPTADDRESSCAPS_MIRROR",            dpcTriCaps.dwTextureAddressCaps, D3DPTADDRESSCAPS_MIRROR           ),
    D3CAPDEF("D3DPTADDRESSCAPS_CLAMP",             dpcTriCaps.dwTextureAddressCaps, D3DPTADDRESSCAPS_CLAMP            ),
    {"",0,0}
};

/****************************************************************************
 ***************************************************************************/
CAPDEF D3dMisc[] =
{
    D3VALDEF("MaxBufferSize",         dwMaxBufferSize),        
    D3VALDEF("MaxVertexCount",        dwMaxVertexCount),       
    D3VALDEF("DeviceRenderBitDepth",  dwDeviceRenderBitDepth), 
    D3VALDEF("DeviceZBufferBitDepth", dwDeviceZBufferBitDepth),
    {"",0,0}
};

/****************************************************************************
 ***************************************************************************/
CAPDEFS D3CapDefs[] =
{
    {"",              D3AddCaps, (LPARAM)ValidFlags},  
    {"ColorModel",    D3AddCaps, (LPARAM)ColorModel},
    {"DevCaps",       D3AddCaps, (LPARAM)DevCaps},
    {"TransformCaps", D3AddCaps, (LPARAM)TransformCaps},
    {"LightingCaps",  D3AddCaps, (LPARAM)LightingCaps},
    {"BClipping",     D3AddCaps, (LPARAM)BClipping},
    {"LineCaps",      D3AddCaps, (LPARAM)LineCaps},
    {"TriCaps",       D3AddCaps, (LPARAM)TriCaps},
    {"Misc",          D3AddCaps, (LPARAM)D3dMisc},      
    {NULL, 0, 0}
};
#endif

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
    {
        IDirectDraw_Release(lpDD);
        lpDD = NULL;
    }

    // There is no need to create DirectDraw emulation-only just to get
    // the HEL caps.  In fact, this will fail if there is another DirectDraw
    // app running and using the hardware.
    if( pid == (GUID *)DDCREATE_EMULATIONONLY )
    {
        pid = NULL;
    }
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

//================================================================
//================================================================
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
            LVAddText(g_hwndLV, 0, pcd->szName, "test");
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
//================================================================
#ifdef DX_3D
void D3AddCaps(LPARAM lParam1, LPARAM lParam2)
{
    // lParam1 is the CAP3DDEVICEDESC Struct
    // lParam2 is the CAPDEF table we should use

    // Unlike other AddCaps function this info has been prethought for us
    // so just print it out.
    AddCapsToLV((CAPDEF *)lParam2, (LPVOID)lParam1);
}
#endif

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
//================================================================
void DDFourCCFormat(LPARAM lParam1, LPARAM lParam2)
{
    HRESULT ddrval;
    int iNumOfCodes,iCount;
    DWORD *FourCC;
    char szText[5]={0,0,0,0,0};

    if(lpDD != NULL)
    {
        ddrval = IDirectDraw_GetFourCCCodes(lpDD,&iNumOfCodes, NULL);
        LVAddColumn(g_hwndLV, 0, "Codes", 24);
        LVAddColumn(g_hwndLV, 1, "", 24);
        if( ddrval == DD_OK)
        {
            FourCC = GlobalAlloc(GPTR,(sizeof(DWORD)*iNumOfCodes));
            if(FourCC)
            {
                ddrval = IDirectDraw_GetFourCCCodes(lpDD,&iNumOfCodes, FourCC);
                // Assume all FourCC values are ascii strings
                for(iCount = 0;iCount < iNumOfCodes; iCount++)
                {
                    memcpy(szText,&FourCC[iCount],4);
                    LVAddText(g_hwndLV, 0, "%s", szText);
                }
            }
        }
    }
}

//================================================================
//================================================================
typedef struct LLMode
{
    DWORD x,y,bpp;      
        BOOL IsModeX;
        struct LLMode *Next;
}LinkMode;      

static LinkMode *pModesHead = NULL; 

//================================================================
// EnumDisplayModesCallback1
//================================================================
HRESULT CALLBACK EnumDisplayModesCallback1(LPDDSURFACEDESC pddsd, LPVOID Context)
{
    static LinkMode *pModesTail = NULL; 
    LinkMode *tmp=NULL;

    tmp = GlobalAlloc(GPTR,sizeof(LinkMode));
    if(tmp != NULL)
    {
        tmp->x = pddsd->dwWidth;
        tmp->y = pddsd->dwHeight;
        tmp->bpp = pddsd->ddpfPixelFormat.dwRGBBitCount;
                tmp->IsModeX = TRUE;
                tmp->Next = NULL;
                if(pModesHead == NULL)
                {
                        pModesHead = tmp;
                        pModesTail = tmp;
                }else
                {
                        pModesTail->Next = tmp;
                        pModesTail = tmp;
                }
    }//Hey if we out of memory silent failure
    return DDENUMRET_OK;

} /* EnumModesCallback */

//================================================================
// EnumDisplayModesCallback2
//================================================================
HRESULT CALLBACK EnumDisplayModesCallback2(LPDDSURFACEDESC pddsd, LPVOID Context)
{
    LinkMode *tmp= NULL;

    tmp = pModesHead;
    while(tmp != NULL) //cycle though all modes since unique modes don't always happen
    {
        if( tmp->x == pddsd->dwWidth &&
            tmp->y == pddsd->dwHeight &&
            tmp->bpp == pddsd->ddpfPixelFormat.dwRGBBitCount)
            {
                tmp->IsModeX = FALSE;
        
            }
                tmp = tmp->Next;
        }
    
    return DDENUMRET_OK;

} /* EnumModesCallback */

//================================================================
// EnumDisplayModesCallback2
//================================================================
void DisplayEnumModes()
{
    LinkMode *tmp= NULL;

    tmp = pModesHead;
    while(tmp != NULL) //cycle though all modes since unique modes don't always happen
    {
        if(tmp->IsModeX)
        {
            LVAddText(g_hwndLV, 0, "%dx%dx%d (ModeX)", tmp->x, tmp->y, tmp->bpp);
        }else
        {
            LVAddText(g_hwndLV, 0, "%dx%dx%d ", tmp->x, tmp->y, tmp->bpp);
        }
        tmp = tmp->Next;
    }
} /* EnumModesCallback */

//================================================================
// Should we hourglass the cursor? this takes a while
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
        // Get the current mode mode for this driver
        ddsd.dwSize = sizeof(DDSURFACEDESC);
        IDirectDraw_GetDisplayMode(lpDD, &ddsd);

        mode = MAKEMODE(ddsd.dwWidth, ddsd.dwHeight, ddsd.ddpfPixelFormat.dwRGBBitCount);

        // Find all mode for this driver and add them to the listview
        //Erase previous list
        {
            LinkMode *tmp, *tmp1;       

            tmp = pModesHead;
            while(tmp != NULL)
            {
                tmp1 = tmp;
                tmp = tmp->Next;
                GlobalFree(tmp1);  //Note We will exit with a list in place
            }
            pModesHead = NULL;
        }
        //Get Mode with ModeX
        IDirectDraw_SetCooperativeLevel(lpDD, g_hwndMain, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWMODEX | DDSCL_NOWINDOWCHANGES);
        IDirectDraw_EnumDisplayModes(lpDD, 0, NULL, (LPVOID)mode, EnumDisplayModesCallback1);
        //Get Modes with out ModeX
        IDirectDraw_SetCooperativeLevel(lpDD, g_hwndMain, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_NOWINDOWCHANGES);
        IDirectDraw_EnumDisplayModes(lpDD, 0, NULL, (LPVOID)mode, EnumDisplayModesCallback2);
        //Call Function to out stuff in listview
        DisplayEnumModes();
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
#ifdef DX_3D
    HRESULT ddrval;
#endif

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
#ifdef DX_3D
        {
            DDCAPS DDCaps;
            lpDD = DDCreate(pid);
            DDCaps.dwSize = sizeof(DDCAPS);
            ddrval = lpDD->lpVtbl->GetCaps(lpDD, &DDCaps,NULL);
            if(ddrval == DD_OK)
                if(DDCaps.dwSize != 0)
                    if(DDCaps.ddsCaps.dwCaps & DDSCAPS_3D != 0)
                        pid_for3D = pid;
        }
#endif
    DDCapDefs[0].szName = szText;
    AddCapsToTV(hParent, DDCapDefs, (LPARAM)pid);

    return(DDENUMRET_OK);
}

//================================================================
//================================================================
HRESULT CALLBACK DSEnumCallBack(const GUID *lpGUID, LPSTR lpDriverDesc, LPSTR lpDriverName, LPVOID lpContext)
{
    HTREEITEM hParent = (HTREEITEM)lpContext;
    TCHAR     szText[256];
    LPGUID    lpTemp = NULL;

    if( lpGUID != NULL )
        {
        if(( lpTemp = LocalAlloc( LPTR, sizeof(GUID))) == NULL )
            return( TRUE );

        memcpy( lpTemp, lpGUID, sizeof(GUID));
        }

    // Add subnode to treeview
    if (lpDriverName && *lpDriverName)
        wsprintf(szText, "%s (%s)", lpDriverDesc, lpDriverName);
    else
        lstrcpy(szText, lpDriverDesc);

    DSCapDefs[0].szName = szText;
    AddCapsToTV(hParent, DSCapDefs, (LPARAM)lpTemp);

    return(DDENUMRET_OK);
}

//================================================================
//================================================================
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
#ifdef DX_3D
HRESULT CALLBACK D3EnumCallback(
            LPGUID pid,
            LPSTR lpDriverDesc,
            LPSTR lpDriverName, 
            LPD3DDEVICEDESC lpD3DDeviceDesc1, 
            LPD3DDEVICEDESC lpD3DDeviceDesc2, 
            LPVOID lpContext)
{
    HTREEITEM hParent = (HTREEITEM)lpContext;
    TCHAR szText[256];

    //Store this info now is much easier than recreating it later.
    CAP3DDEVICEDESC *temp;
    if(lpD3DDeviceDesc1->dwFlags != 0)
    {
        temp = LocalAlloc(LPTR, sizeof(CAP3DDEVICEDESC));
        memcpy(temp,lpD3DDeviceDesc1,sizeof(CAP3DDEVICEDESC));
    }else
    {
        temp = LocalAlloc(LPTR, sizeof(CAP3DDEVICEDESC));
        memcpy(temp,lpD3DDeviceDesc2,sizeof(CAP3DDEVICEDESC));
    }
    memcpy(&temp->guid,pid,sizeof(GUID));

    // Add subnode to treeview
    if (lpDriverName && *lpDriverName)
        wsprintf(szText, "%s (%s)", lpDriverDesc, lpDriverName);
    else
        lstrcpy(szText, lpDriverDesc);

    D3CapDefs[0].szName = szText;
    AddCapsToTV(hParent, D3CapDefs, (LPARAM)temp);

    return(DDENUMRET_OK);
}

//================================================================
//================================================================
HRESULT Direct3DEnumerate(LPD3DENUMDEVICESCALLBACK lp3DEnumCallback, LPVOID lpVoid)
{
    TCHAR szText[256];
    HRESULT ddrval;

    if (lpDD)
    {
        IDirectDraw_Release(lpDD);
        lpDD = NULL;
    }
    if((ddrval = DirectDrawCreate(pid_for3D, &lpDD, NULL)) != DD_OK)
    {
        wsprintf(szText, "Error %x",ddrval);
        OutputDebugString(szText);
    }
    ddrval = IDirectDraw_QueryInterface(lpDD,&IID_IDirect3D,&lp3D);
    if ((ddrval == DD_OK)&&(lp3D != NULL))
    {
        IDirect3D_EnumDevices(lp3D, lp3DEnumCallback, lpVoid);
    }
    return(DDENUMRET_OK);
    
}
#endif

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

    // Add direct 3D devices
#ifdef DX_3D
    hTree = TVAddNode(TVI_ROOT, "Direct3D Devices", TRUE,
        IDI_DIRECTX, NULL, 0, 0);

    Direct3DEnumerate(D3EnumCallback, hTree); //This is not a real 3D function
    TreeView_Expand(g_hwndTV, hTree, TVE_EXPAND);
#endif

        
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
#ifdef _X86_
#pragma optimize("", off)
#endif
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
    va_list vl;

    va_start(vl, sz );
    wvsprintf(ach,sz, vl);

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
    va_end(vl);
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
#ifdef _X86_
#pragma optimize("", on)
#endif

