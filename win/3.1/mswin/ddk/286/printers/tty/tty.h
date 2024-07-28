// TTY.H for TTY driver

// ****************************************************************************
// Copyright (c) 1989-1990, Microsoft Corporation.
// ****************************************************************************
//	Microsoft history
//	27 dec 89	peterbe		Remove Y_INIT_ENT, X_INIT_ENT
//	20 oct 89	peterbe		checked in
// ****************************************************************************


#define DEV_MEMORY  0
#define DEV_DEVICE  1

/**************** sequencias de escape dependientes del device ****************/

#define HYPOTENUSE     121	    /* raiz cuadrada de x^2 + y^2 */
#define YMAJOR	       120	    /* la direccion con "finer resolution" tiene menor */
#define XMAJOR		 6	    /* distancia a viajar */
#define MAXSTYLEERR	HYPOTENUSE*2

/*  orden por portrait, invertir 1 <--> 2, 3 <--> 4 por landscape
    HorzSize  se refiare a MM_HSIZE, taman~o horizontal en milimetros
    VertSize  se refiare a MM_VSIZE, taman~o horizontal en milimetros  */

#define MM01	    254	      /* HorzSize * 10  escala (/HorzSize / 10 * 254) */
#define MM02	    254	      /* VertSize * 10  escala (/VertSize / 10 * 254) */
#define MM03	    HDPI      /* HorizRes       escala (/HorzSize / 10 * 254) */
#define MM04	    VDPI      /* VertRes        escala (/VertSize / 10 * 254) */

#define MM001	    2540      /* HorzSize * 100 escala (/HorzSize / 10 * 254) */
#define MM002	    2540      /* VertSize * 100 escala (/VertSize / 10 * 254) */
#define MM003	    HDPI      /* HorizRes       escala (/HorzSize / 10 * 254) */
#define MM004	    VDPI      /* VertRes        escala (/VertSize / 10 * 254) */

#define EnglishLo1  100	      /* HorzSize * 1000 escala (/HorzSize / 10)      */
#define EnglishLo2  100	      /* VertSize * 1000 escala (/VertSize / 10)      */
#define EnglishLo3  HDPI      /* HorizRes * 254  escala (/HorzSize / 10)      */
#define EnglishLo4  VDPI      /* VertRes * 254   escala (/VertSize / 10)      */

#define EnglishHi1  1000      /* HorzSize * 10000 escala(/HorzSize / 10)      */
#define EnglishHi2  1000      /* VertSize * 10000 escala(/VertSize / 10)      */
#define EnglishHi3  HDPI      /* HorizRes * 254   escala(/HorzSize / 10)      */
#define EnglishHi4  VDPI      /* VertRes * 254    escala(/VertSize / 10)      */


#define Twips1	    1440      /* HorzSize * 14400 escala(/HorzSize / 10)      */
#define Twips2	    1440      /* VertSize * 14400 escala(/VertSize / 10)      */
#define Twips3	    HDPI      /* HorizRes * 254   escala(/HorzSize / 10)      */
#define Twips4	    VDPI      /* VertRes * 254    escala(/VertSize / 10)      */


// define some ASCII characters and escape sequences

/* manda un "form feed" */
#define EP_FF	   "\014", 1

/* manda un "line feed" */
#define EP_LF	   "\012", 1
#define LF_CHAR	    '\012'

/* manda un retorno de carro  (carriage return) */
#define EP_CR	    "\015", 1
#define EP_NL	    "\015\012", 2

/* codigo de escape para "microspace justification" */
#define EP_BLANK    " ", 1
#define SPACE	    ' '

// Letter must 0 based
#define LETTER		    0
#define LEGAL		    1
#define A4		    2
#define B5		    3
#define MAXPAPER	    4
#define STANDARDPF	    LETTER  /* formato de papel estandar */

/* poner tambien mas papeles en devmode.h */

#define RIBBON_OFFSET	    9  /* la cabeza de impresion es 1/8" desde el */
				/* borde del cinta */
				// the print head is 1/8" from the edge of
				// the ribbon.
#define MAXPAPERFORMAT	    1
#define NSMALLPAPERFORMATS  4

// taman~os estandar de hojas desplazamientos de papel taman~o
// carta (R120X72)
// pixel resolution and logical paper size

#define HDPI	     120	 /* 120 puntos por pulgada (points/inch) */
#define VDPI	       6	 /* 72 DPI verticales */


/* PG_ACROSS tiene que ser un multiplo de 16 - must be a multiple of 16
   PG_ACROSS es la resolucion de un "scan line" */

/*  the dimensions are of the printable width not the physical
    width of the paper  */
#define PG_ACROSS_LET	 1020	    /* 120 * 8.5 */
#define PG_ACROSS_LEG	 1020	    /* 120 * 8.5 */
#define PG_ACROSS_A4	  992	    /* 120 * 8.268 */
#define PG_ACROSS_B5	  831	    /* 120 * 6.929 (.23)*/
#define PG_ACROSS_15	 1632	    /* 120 * 13.6 = */
				    /* 1632 . FALSO ->(120 * 15 = 1800). */

#define MM_HSIZE_LET	  215	    /* PG_ACROSS / HDPI * 25.4      */
#define MM_HSIZE_LEG	  215	    /* PG_ACROSS / HDPI * 25.4      */
#define MM_HSIZE_A4	  210	    /* PG_ACROSS / HDPI * 25.4      */
#define MM_HSIZE_B5	  176	    /* PG_ACROSS / HDPI * 25.4      */
#define MM_HSIZE_15	  345	    /* PG_ACROSS / HDPI * 25.4 = */
				    /*  381 BUENO->(345) */

// PG_DOWN is the number of scan lines per page.

#define PG_DOWN_LET	 66	    /* 11" */
#define PG_DOWN_LEG	 84	    /* 14" */
#define PG_DOWN_A4	 70	    /* 11.683 */
#define PG_DOWN_B5	 59	    /* 9.842 */

#define MM_VSIZE_LET	 279	    /* PG_DOWN / VDPI * 25.4 */
#define MM_VSIZE_LEG	 355	    /* PG_DOWN / VDPI * 25.4 */
#define MM_VSIZE_A4	 297	    /* PG_DOWN / VDPI * 25.4 */
#define MM_VSIZE_B5	 250	    /* PG_DOWN / VDPI * 25.4 */

#define CONTINUOUS	 30
#define CUT		 31
#define NBINS		 2

#define EP_ENABLE   "\033@", 2
#define EP_NORMAL   "\033@", 2

#define PICA_WIDTH	    12
#define ELITE_WIDTH	    10
#define COMP_MODE_WIDTH	    7


/* si esta es una variable "pitch font", trataremos de ser listos y usar un
"pitch font" fijo con un ancho que es n pixels menor solo para estar seguro */

// if this is a variable-pitch font, we try to be clever and use a
// fixed-pitch font with a width which is n pixels less just to be sure.

#define MAXDELY	    85	/* max numeros de puntos que un "line feed" cubre */
				// max. no. of pixels equiv. to a linefeed.
#define CHARWIDTH   PICA_WIDTH	/* 12 puntos por "hardware font character" */
				// 12 points per ... 
#define VAR_PITCH_KLUDGE 3

#define XMS	'L'
#define FASTXMS 'Y'


#define TRANS_WIDTH_SIZE    30	/* taman~o de Trans_width[] */

/*************** fin de la informacion "device dependent" ********************/
// end of device-independent info.

#define CCHSPOOL    2048

#define INIT_BUF    (512)   /*  buffer space for calls to textout */
			/* espacio buffer para llamadas a textout */
#define MARG_BUF    (512)   /* additional buffer space */
			/* aumento por espacio de buffer */

#define FW_NORMAL	400
#define FW_MEDIUM	500
#define FW_SEMIBOLD	600
#define FW_BOLD		700

#define EP_len(i)	((i)&0xFF)
#define EP_car(i)	((i)&0xFF)
#define EP_type(i)	(((i)>>8)&0xFF)

#define STRING_TRANS	0xFF
