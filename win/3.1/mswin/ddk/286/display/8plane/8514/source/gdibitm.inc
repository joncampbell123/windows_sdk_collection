;/*	GDIBITM.INC



Bitmap		struc							    ;*/ typedef struct {		    /*
  bmType	dw	0	; 0 means memory bitmap.		    ;*/ unsigned short int bmType;	    /*
  bmWidth	dw	0	; Width of bitmap in pixels		    ;*/ unsigned short int bmWidth;	    /*
  bmHeight	dw	0	; Height of bitmap in pixels		    ;*/ unsigned short int bmHeight;	    /*
  bmWidthBytes	dw	0	; #bytes per scan line			    ;*/ unsigned short int bmWidthBytes;    /*
  bmPlanes	db	0	; # of planes in bitmap 		    ;*/ Byte		   bmPlanes;	    /*
  bmBitsPixel	db	0	; # of bits per pixel			    ;*/ Byte		   bmBitsPixel;     /*
  bmBits	dd	0	; Far pointer to bits of main memory bitmap ;*/ Byte FAR	   *bmBits;	    /*
Bitmap		ends							    ;*/ } BITMAP;			    /*




;	The bitmap descriptor must be kept in main memory so that
;	the lpDevice field in the DC can be passed off to the OEM
;	code regardless of whether the DC is to a main memory bitmap
;	or the device (want to avoid checking and having to pick
;	up a different pointer based on whether the DC is to a real
;	device or a memory bitmap).

ipBitmap	    struc						    ;*/ typedef struct	{		  /*
  ilBitmapOverhead  db	SIZE BITMAP dup (?)				    ;*/ BITMAP		ilBitmapOverhead; /*
  bmWidthPlanes     dd	0	; Product of bmWidthBytes and bmHeight	    ;*/ DWORD		   bmWidthPlanes; /*
  bmlpPDevice	    dd	0	; Pointer to associated PDevice 	    ;*/ Byte FAR	   *bmlpPDevice;  /*
  bmSegmentIndex    dw	0						    ;*/ unsigned short int bmSegmentIndex; /*
  bmScanSegment     dw	0						    ;*/ unsigned short int bmScanSegment; /*
  bmFillBytes	    dw	0						    ;*/ unsigned short int bmFillBytes;   /*
		    dw	0						    ;*/ unsigned short int futureUse4;	  /*
		    dw	0						    ;*/ unsigned short int futureUse5;	  /*
  ipBits	    db	?	; The bits will start here		    ;*/ Byte		ipBits[1];	  /*
ipBitmap	    ends						    ;*/ } IPBITMAP;			  /*

bmSegPlane1	    equ ipBits	;Segment of first plane goes here


;	The internal logical bitmap consists of the logical object
;	overhead and a global handle to the bitmap


ilBitmap	    struc						    ;*/ typedef struct	{		    /*
		    db	SIZE ilObjHead dup (?)				    ;*/ ILOBJHEAD	   ilBitmapHead;    /*
  bmhBits	    dw	0	; Global handle to real bitmap		    ;*/ GLOBALHANDLE	   bmhBits;	    /*
  bmCount	    dw	0	; various flags 			    ;*/ short		   bmCount;	    /*
  bmhDC 	    dw	0	; DC the bitmap is selected into	    ;*/ LOCALHANDLE	   bmhDC;	    /*
  bmMagicX	    dw	0	; screen graber X			    ;*/ short		   bmMagicX;	    /*
  bmMagicY	    dw	0	; screen graber Y			    ;*/ short		   bmMagicY;	    /*
  bmhPlane	    dw	0	; Handle to the separate plane		    ;*/ GLOBALHANDLE	   bmhPlane;	    /*
  bmFlags	    dw	0	; >64K flag goes here			    ;*/ short		   bmFlags;	    /*

ilBitmap	    ends						    ;*/ } ILBITMAP;			    /*

BITMAP_SELECTED     equ     00000001b
BITMAP_64K	    equ     00000001b


; structures used for Device Independent Bitmap (DIB) processing.
; all taken out of Presentation Manager's documentation 
;  Tuesday 25-October-1988 15:04   -by-   Ron Gery [rong]

; C definitions are provided below (separately).

; triple used in PM1.1 (BitmapCoreInfo) format color table
RGBTriple	    struc	
  rgbtBlue 	    db	0
  rgbtGreen 	    db	0
  rgbtRed 	    db	0
RGBTriple	    ends

; RGB DWORD used in PM2.0 format color table
RGBQuad		    struc
  rgbBlue 	    db	0
  rgbGreen 	    db	0
  rgbRed 	    db	0
  rgbReserved 	    db	0
RGBQuad		    ends

BitmapCoreHeader    struc
  bcSize 	    dd	0
  bcWidth 	    dw	0
  bcHeight 	    dw	0
  bcPlanes 	    dw	0
  bcBitCount 	    dw	0
BitmapCoreHeader    ends

; new format bitmap structure based on PM2.0 format DCR.
;  Tuesday 23-May-1989 16:05   -by-   Ron Gery [rong]

BitmapInfoHeader    struc
  biSize 	    dd	0
  biWidth 	    dd	0
  biHeight 	    dd	0
  biPlanes 	    dw	0
  biBitCount 	    dw	0

  biStyle 	    dd	0
  biSizeImage 	    dd	0
  biXPelsPerMeter   dd	0
  biYPelsPerMeter   dd	0
  biClrUsed	    dd	0
  biClrImportant    dd	0
BitmapInfoHeader    ends

BitmapInfo	    struc
  bmiHeader	db (SIZE BITMAPINFOHEADER) DUP (?)
  bmiColors	db ?			; array of RGBQUADS
BitmapInfo	    ends

	if	0
*/

/* C definitions for DIBs, as defined in windows.h */

typedef struct {
	DWORD	bcSize;
	WORD	bcWidth;
	WORD	bcHeight;
	WORD	bcPlanes;
	WORD	bcBitCount;
} BITMAPCOREHEADER;
typedef BITMAPCOREHEADER FAR *LPBITMAPCOREHEADER;
typedef BITMAPCOREHEADER *PBITMAPCOREHEADER;

typedef struct {
  	DWORD	   biSize;
  	DWORD	   biWidth;
  	DWORD	   biHeight;
  	WORD	   biPlanes;
  	WORD	   biBitCount;

	DWORD	   biStyle;
	DWORD	   biSizeImage;
	DWORD	   biXPelsPerMeter;
	DWORD	   biYPelsPerMeter;
	DWORD	   biClrUsed;
	DWORD	   biClrImportant;
} BITMAPINFOHEADER;

typedef BITMAPINFOHEADER FAR *LPBITMAPINFOHEADER;
typedef BITMAPINFOHEADER *PBITMAPINFOHEADER;

typedef struct {
	BYTE	rgbtBlue;
	BYTE	rgbtGreen;
	BYTE	rgbtRed;
} RGBTRIPLE;

typedef struct {
	BYTE	rgbBlue;
	BYTE	rgbGreen;
	BYTE	rgbRed;
	BYTE	rgbReserved;
} RGBQUAD;

typedef struct { 
    BITMAPCOREHEADER	bmicHeader;
    RGBQUAD		bmiColors[1];
} BITMAPINFO;

typedef BITMAPINFO FAR *LPBITMAPINFO;
typedef BITMAPINFO *PBITMAPINFO;




#define BITMAP_SELECTED     0x01
#define BITMAP_64K	    0x01

/*
	endif
;*/
