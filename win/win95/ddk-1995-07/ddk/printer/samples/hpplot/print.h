/* (c) Copyright 1987 MICROGRAFX, Inc., All Rights Reserved.  Permission to
   use this work for any purpose must be obtained in writing from MICROGRAFX,
   1820 N. Greenville Ave., Richardson, Tx.  75081.

*******************************************************************************

				    print

*******************************************************************************

*/

#define NEWSHEET 500
#define HP 501
#define NEWCAROUSEL 502
#define SHORTEDGE 503

extern RECT Corner [NUM_PLOTTERS] [MAX_MEDIA_SIZES];
extern BOOL MyArc [NUM_PLOTTERS],
	    AutoView [NUM_PLOTTERS],
	    CharSetSevenSupport [NUM_PLOTTERS],
	    Circle [NUM_PLOTTERS],
	    EdgeRectangle [NUM_PLOTTERS],
	    FillRectangle [NUM_PLOTTERS],
	    MyPolygon [NUM_PLOTTERS],
	    Wedge [NUM_PLOTTERS];
extern short MaxPolygonSize [NUM_PLOTTERS];

extern int FAR PASCAL abort_doc (LPPDEVICE);
extern int FAR PASCAL device_control (LPPDEVICE,short,LPSTR,LPSTR);
extern int FAR PASCAL draft_mode (LPPDEVICE,LPINT);
extern int FAR PASCAL end_doc (LPPDEVICE);
extern int FAR PASCAL flush_output (LPPDEVICE);
extern int FAR PASCAL get_color_table (LPPDEVICE,LPINT,DWORD FAR *);
extern int FAR PASCAL get_phys_page_size (LPPDEVICE,LPPOINT);
extern int FAR PASCAL get_printing_offset (LPPDEVICE,LPPOINT);
extern int FAR PASCAL get_vector_pen_size (LPPDEVICE,LPPOINT);
extern int FAR PASCAL new_frame (LPPDEVICE);
extern int FAR PASCAL next_band (LPPDEVICE,LPRECT);
extern BOOL FAR PASCAL next_color (LPPDEVICE,DWORD);
extern int FAR PASCAL set_abort_proc (LPPDEVICE,HANDLE FAR *);
extern int FAR PASCAL set_color_table (LPPDEVICE,LPCOLORTABLEENTRY);
extern void FAR PASCAL set_p1p2 (LPPDEVICE,LPRECT);
extern int FAR PASCAL start_doc (LPPDEVICE,LPSTR,LPSTR);
extern int FAR PASCAL query_esc_support (LPPDEVICE,LPINT);
