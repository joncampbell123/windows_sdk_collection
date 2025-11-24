/* (c) Copyright 1987 MICROGRAFX, Inc., All Rights Reserved.  Permission to
   use this work for any purpose must be obtained in writing from MICROGRAFX,
   1820 N. Greenville Ave., Richardson, Tx.  75081.

*******************************************************************************

				    text

*******************************************************************************

*/

extern void FAR PASCAL draw_text (LPPDEVICE,short,short,LPRECT,LPSTR,short,LPFONTINFO,LPDRAWMODE,LPTEXTXFORM);
extern DWORD FAR PASCAL get_text_extent (LPPDEVICE,short,short,LPRECT,LPSTR,short,LPFONTINFO,LPDRAWMODE,LPTEXTXFORM);
