/****************************************************************************
 MENU.h

 This file contains all of the menu IDs.

****************************************************************************/


/* Menu IDs are of the form MII in hex where M is the menu group number
   and II is a unique item number for that menu group. */

#define MenuGroupFromID( id )    ((id) & 0xFF00)

/* Menu groups */
#define VIEW_MENU_GROUP    0x100
#define DC_MENU_GROUP      0x200
#define DRAW_MENU_GROUP    0x300
#define HELP_MENU_GROUP    0x400

/* View menu */
#define IDM_CLEAR          0x100
#define IDM_DRAWINGSIZE    0x101
#define IDM_NORMALSIZE     0x102
#define IDM_ZOOMIN         0x103
#define IDM_ZOOMOUT        0x104
#define IDM_SETSCALE       0x105
#define IDM_PIXGRID        0x106

/* General Menu */
#define IDM_DEFAULTS       0x200
#define IDM_COORDS         0x201
#define IDM_CLIP           0x202
#define IDM_BKCOLOR        0x203
#define IDM_BKMODE         0x204
#define IDM_ROP2           0x205
#define IDM_POLYMODE       0x206
#define IDM_BLTMODE        0x207

/* Brush menu */
#define IDM_NULLBRUSH      0x230
#define IDM_SOLIDBRUSH     0x231
#define IDM_HATCHBRUSH     0x232
#define IDM_PATBRUSH       0x233
#define IDM_DIBPATBRUSH    0x234
#define IDM_BRUSHORG       0x235
                           
/* Pen menu */             
#define IDM_NULLPEN        0x240
#define IDM_SOLIDPEN       0x241
#define IDM_INSIDEFRAMEPEN 0x242
#define IDM_DASHPEN        0x243
#define IDM_DOTPEN         0x244
#define IDM_DASHDOTPEN     0x245
#define IDM_DASHDOTDOTPEN  0x246
#define IDM_PENWIDTH       0x247
#define IDM_PENCOLOR       0x248
#define IDM_PENPOS         0x249

/* Text Menu */
#define IDM_FONT           0x260
#define IDM_TEXTALIGN      0x261
#define IDM_TEXTJUST       0x262
#define IDM_TEXTEXTRA      0x263
#define IDM_TEXTCOLOR      0x264

/* Draw menu */
#define IDM_SETPIXEL       0x300
#define IDM_LINETO         0x301
#define IDM_LINE           0x302
#define IDM_RECTANGLE      0x303
#define IDM_ELLIPSE        0x304
#define IDM_ROUNDRECT      0x305
#define IDM_ARC            0x306
#define IDM_PIE            0x307
#define IDM_CHORD          0x308
#define IDM_POLYLINE       0x309
#define IDM_POLYGON        0x30A
#define IDM_POLYPOLYGON    0x30B
#define IDM_PATBLT         0x30C
#define IDM_BITBLT         0x30D
#define IDM_STRETCHBLT     0x30E
#define IDM_DIBTODEVICE    0x30F
#define IDM_STRETCHDIB     0x310
#define IDM_TEXTOUT        0x311
#define IDM_EXTTEXTOUT     0x312
#define IDM_DRAWTEXT       0x313
#define IDM_FLOODFILL      0x314
#define IDM_EXTFLOODFILL   0x315

/* Help menu */                        
#define IDM_ABOUT          0x400


