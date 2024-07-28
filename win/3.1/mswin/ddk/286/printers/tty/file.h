/*
 +-- FILE.H for TTY --------------------------------------------------------+
 |                                                                          |
 |  Copyright (c) 1989-1990, Microsoft Corporation.			    |
 |  Modificado en 1986 por Jaime Garza V. FRALC Consultores (W 1.03)        |
 |  Modificado en 1988 por Jaime Garza V. FRALC Consultores (W 2.03)        |
 |  Modificado en 1989 por Armando Rodri'guez M. FRALC Consultores (W 3.00) |
 |  Modificado en 1989 por Jaime Garza V. FRALC Consultores (W 3.00)        |
 |                                                                          |
 +--------------------------------------------------------------------------+
*/
// Microsoft history
//	22 oct 89	peterbe		changed ANSISTART to 128,
//					ANSIEND to 255.
//	20 oct 89	peterbe		checked in.

/* codigos de retorno. return codes */

#define PRINTEROK	    0
#define PRINTERNOTFOUND	    1
#define PRINTERFILEERROR   -1

#define NUMPRINTERS	    0L
#define ACTPRINTER	    (long)(sizeof(int))

/* constantes y estructuras de impresoras */
// printer constants and structures.

#define PRINTERLEN  15
#define ESCAPELEN   32
#define ANSISTART 128
#define ANSIEND	  255

typedef unsigned char Byte;
typedef unsigned int  Word;

struct prconf{
	Byte    pcPageWidth;	    	/* valido 15 o 8      */
	Byte    pcPageHeight;	    	/*                    */
	Byte    pcReset[ESCAPELEN+1];   /* reset escape       */
	Byte    pc10cpi[ESCAPELEN+1];   /* 10 cpi escape      */
	Byte    pc12cpi[ESCAPELEN+1];   /* 12 cpi escape      */
	Byte    pc16cpi[ESCAPELEN+1];   /* 16 cpi escape      */
	Byte	pcDoubleOn[ESCAPELEN+1]; /* Doble Ancho escape (doublewide) */
	Byte	pcDoubleOff[ESCAPELEN+1];/*		       */
	Byte    Unused1[ESCAPELEN+1];   /* Sin Usar. Unused   */
	Byte    Unused2[ESCAPELEN+1];   /* Sin Usar. Unused   */
    };

typedef struct prconf		 PrinterConfig;
typedef struct prconf	     *	pPrinterConfig;
typedef struct prconf	near * npPrinterConfig;
typedef struct prconf	far  * lpPrinterConfig;

struct prinf{
	    char	    piPrinterName[PRINTERLEN+1];
	    PrinterConfig   piPrinterData;
	    unsigned char   piPrinterTable[ANSIEND-ANSISTART+1][ESCAPELEN+1];
	};

typedef struct prinf		 PrinterInfo;
typedef struct prinf	     *	pPrinterInfo;
typedef struct prinf	near * npPrinterInfo;
typedef struct prinf	far  * lpPrinterInfo;

typedef struct esc{
    unsigned char compress[17];
    unsigned char pica[17];
    unsigned char elite[17];
    unsigned char compress2[17];
    unsigned char pica2[17];
    unsigned char elite2[17];
    unsigned char reset[9];
    } ESC, FAR * LPESC;

typedef struct esc		PrinterCode;
typedef struct esc	    *  pPrinterCode;
typedef struct esc     near * npPrinterCode;
typedef struct esc     far  * lpPrinterCode;
