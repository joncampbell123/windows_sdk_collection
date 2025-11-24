/* (c) Copyright 1987 MICROGRAFX, Inc., All Rights Reserved.  Permission to
   use this work for any purpose must be obtained in writing from MICROGRAFX,
   1820 N. Greenville Ave., Richardson, Tx.  75081.

*******************************************************************************

				   object

*******************************************************************************

*/
// for realize_brush return - should be elsewhere?

#define PBF_SOLIDCLRBRUSH 1
#define PBF_SOLIDMONOBRUSH 2
#define PBF_SUCCESS 0x8000

extern WORD FAR PASCAL enum_brushes (LPPDEVICE,FARPROC,LPSTR);
extern int FAR PASCAL enum_fonts (LPPDEVICE,LPSTR,FARPROC,LPSTR);
extern WORD FAR PASCAL enum_pens (LPPDEVICE,FARPROC,LPSTR);
extern int FAR PASCAL get_physical_brush_size (LPPDEVICE,LPLOGBRUSH);
extern int FAR PASCAL get_physical_font_size (LPPDEVICE,LPLOGFONT,LPTEXTXFORM);
extern int FAR PASCAL get_physical_pen_size (LPPDEVICE,LPLOGPEN);
extern int FAR PASCAL realize_brush (LPPDEVICE,LPLOGBRUSH,LPPBRUSH);
extern int FAR PASCAL realize_font (LPPDEVICE,LPLOGFONT,LPFONTINFO,LPTEXTXFORM);
extern int FAR PASCAL realize_pen (LPPDEVICE,LPLOGPEN,LPPPEN);
extern DWORD FAR PASCAL rgb_to_pen (LPPDEVICE,DWORD,int);
