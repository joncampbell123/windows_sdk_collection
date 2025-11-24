/* (c) Copyright 1987 MICROGRAFX, Inc., All Rights Reserved.  Permission to
   use this work for any purpose must be obtained in writing from MICROGRAFX,
   1820 N. Greenville Ave., Richardson, Tx.  75081.

*******************************************************************************

				   output

*******************************************************************************

*/

typedef struct
{
    short Speed [MAX_PEN_GROUPS];
    short Force [MAX_PEN_GROUPS];
    short Acceleration [MAX_PEN_GROUPS];
} DEFDEVICEINFO;

extern DEFDEVICEINFO DefDeviceInfo [NUM_PLOTTERS];

extern void FAR PASCAL check_line_construction (LPPDEVICE);
extern void FAR PASCAL construct_float (LPPDEVICE,short,short,short);
extern void FAR PASCAL construct_point (LPPDEVICE,LPPOINT,BOOL);
extern void FAR PASCAL draw_arc (LPPDEVICE,LPPOINT,LPPPEN,LPDRAWMODE,LPRECT);
extern void FAR PASCAL draw_bitmap (LPPDEVICE,int,int,LPPDEVICE,int,int,int,int,DWORD,LPPBRUSH,LPDRAWMODE);
extern void FAR PASCAL draw_chord (LPPDEVICE,LPPOINT,LPPPEN,LPPBRUSH,LPDRAWMODE,LPRECT);
extern void FAR PASCAL draw_ellipse (LPPDEVICE,LPPOINT,LPPPEN,LPPBRUSH,LPDRAWMODE,LPRECT);
extern void FAR PASCAL draw_pie (LPPDEVICE,LPPOINT,LPPPEN,LPPBRUSH,LPDRAWMODE,LPRECT);
extern void FAR PASCAL draw_polygon (LPPDEVICE,short,short,LPPOINT,LPPPEN,LPPBRUSH,LPDRAWMODE,LPRECT);
extern void FAR PASCAL draw_polyline (LPPDEVICE,int,LPPOINT,LPPPEN,LPPBRUSH,LPDRAWMODE,LPRECT);
extern void FAR PASCAL draw_polymarker (LPPDEVICE,short,LPPOINT,LPPPEN,LPDRAWMODE,LPRECT);
extern void FAR PASCAL draw_rectangle (LPPDEVICE,LPPOINT,LPPPEN,LPPBRUSH,LPDRAWMODE,LPRECT);
extern DWORD FAR PASCAL get_pixel (LPPDEVICE,int,int);
extern int FAR PASCAL scan_pixels (LPPDEVICE,int,int,DWORD,short);
extern void FAR PASCAL set_pixel (LPPDEVICE,int,int,DWORD,LPDRAWMODE);
