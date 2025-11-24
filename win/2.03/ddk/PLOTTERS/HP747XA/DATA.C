/* The actual data allocation for the plotters driver. */

#define     ALLOCATE

#include    "plotters.h"
#include    "hp7470A.h"



GDIINFO hp747xAGDIInfo
                = {
                0x0100,         /* Version = 0100h for now. */
                DT_PLOTTER,     /* Device classification. */
                0,              /* Horizontal size in millimeters. */
                0,              /* Vertical   size in millimeters. */
                0,              /* Horizontal width in pixels. */
                0,              /* Vertical   width in pixels. */
                1,              /* Number of bits per pixel. */
                1,              /* Number of planes. */
                0,              /* Number of brushes the device has. */
                0,              /* Number of pens the device has. */
                0,              /* Number of markers the device has. (Reserved) */
                1,              /* Number of fonts the device has. */
                0,              /* Number of colors in color table. */
                0,              /* Size required for the device descriptor. */
                CC_NONE,        /* Curves    capabilities. */
                LC_POLYLINE
#ifdef  STYLED_LINES
                 | LC_STYLED
#endif
                         ,      /* Line      capabilities. */
                PC_SCANLINE,    /* Polygonal capabilities. */
                TC_CP_STROKE |
                TC_SF_X_YINDEP |
                TC_SA_DOUBLE |
                TC_SA_INTEGER |
                TC_SA_CONTIN |
                TC_IA_ABLE |
                TC_UA_ABLE |
                TC_SO_ABLE,     /* Text      capabilities. */
                CP_RECTANGLE,   /* Clipping  capabilities. */
#ifdef  BANDING
                RC_BANDING |
#endif
                RC_NONE,        /* Bitblt    capabilities. */
                10,             /* Length of X leg. */
                10,             /* Length of Y leg. */
                14,             /* Length of hypotenuse. */
                0,              /* Length of segment for line styles. */

                                /* The HP7470A repeats its linestyles every
                                   500 plotter units which is 500/RESOLUTION
                                   GDI units. */

                                /* These scalings get filled in at enable time. */
                1, 1,           /* Metric  Lo res WinX,WinY  (PTTYPE). */
                1, 1,           /* Metric  Lo res VptX,VptY  (PTTYPE). */
                1, 1,           /* Metric  Hi res WinX,WinY  (PTTYPE). */
                1, 1,           /* Metric  Hi res VptX,VptY  (PTTYPE). */
                1, 1,           /* English Lo res WinX,WinY  (PTTYPE). */
                1, 1,           /* English Lo res VptX,VptY  (PTTYPE). */
                1, 1,           /* English Hi res WinX,WinY  (PTTYPE). */
                1, 1,           /* English Hi res VptX,VptY  (PTTYPE). */
                1, 1,           /* Twips          WinX,WinY  (PTTYPE). */
                1, 1,           /* Twips          VptX,VptY  (PTTYPE). */
                0,              /* Logical pixels/inch in X. */
                0,              /* Logical pixels/inch in Y. */
                DC_SPDevice,    /* dpDCManage. Controls creation of PDevices. */
                0,              /* Reserved for future use. */
                0,              /* Reserved for future use. */
                0,              /* Reserved for future use. */
                0,              /* Reserved for future use. */
                0               /* Reserved for future use. */
                };


DWORD           hp747xAPenColors[9]
                = {
                0x00000000,     /* BLACKPEN */
                0x000000FF,     /* REDPEN */
                0x0000FF00,     /* GREENPEN */
                0x00FF0000,     /* BLUEPEN */
                0x0000FFFF,     /* YELLOWPEN */
                0x00FF00FF,     /* VIOLETPEN */
                0x00FFFF00,     /* TURQUOISEPEN */
                0x000080FF,     /* ORANGEPEN */
                0x0000C0FF      /* BROWNPEN */
                };
