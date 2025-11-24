/* (c) Copyright 1987 MICROGRAFX, Inc., All Rights Reserved.  Permission to
   use this work for any purpose must be obtained in writing from MICROGRAFX,
   1820 N. Greenville Ave., Richardson, Tx.  75081.

*******************************************************************************

				   control

*******************************************************************************

*/

extern POINTL PaperResolution [NUM_PLOTTERS] [MAX_MEDIA_SIZES];
extern COLORSAVAILABLE ColorsAvailable [MAX_COLORS];
extern GDIINFO PlotterGDIInfo;

extern void FAR PASCAL disable_device (LPPDEVICE);
extern int FAR PASCAL initialize_device (LPPDEVICE,LPSTR,LPSTR,LPENVIRONMENT);
extern int FAR PASCAL initialize_gdi (LPGDIINFO,LPSTR,LPSTR,LPENVIRONMENT);
