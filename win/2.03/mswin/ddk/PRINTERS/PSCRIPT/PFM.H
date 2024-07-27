#define NULL 0
#define TRUE 1
#define FALSE 0


typedef int BOOL;
typedef char BYTE;
typedef short int WORD;
typedef long int DWORD;


#define ANSI_CHARSET	      0
#define OEM_CHARSET	      255


#define FW_LIGHT    250
#define FW_NORMAL   400
#define FW_BOLD     700

/*	GDI font families.						*/
#define FF_DONTCARE	(0<<4)	/* Don't care or don't know.		*/
#define FF_ROMAN	(1<<4)	/* Variable stroke width, serifed.	*/
				/* Times Roman, Century Schoolbook, etc.*/
#define FF_SWISS	(2<<4)	/* Variable stroke width, sans-serifed. */
				/* Helvetica, Swiss, etc.		*/
#define FF_MODERN	(3<<4)	/* Constant stroke width, serifed or sans-serifed. */
				/* Pica, Elite, Courier, etc.		*/
#define FF_SCRIPT	(4<<4)	/* Cursive, etc.			*/
#define FF_DECORATIVE	(5<<4)	/* Old English, etc.			*/




typedef struct
    {
    short left;
    short bottom;
    short right;
    short top;
    }RECT;


typedef struct EMM
    {
    short capHeight;
    short xHeight;
    short loAscent;	    /* Lower-case ascent */
    short loDescent;	    /* Lower-case descent */
    short ulOffset;	    /* The underline offset */
    short ulThick;	    /* The underline thickness */
    short iSlant;	    /* The italic angle */
    RECT  rcBounds;	    /* The font bounding box */
    }EMM;


typedef struct
    {
    char szFont[32];		/* The PostScript font name */
    char szFace[32];		/* The face name of the font */
    BOOL fEnumerate;		/* TRUE if the font should be enumerated */
    BOOL fItalic;		/* TRUE if this is an italic font */
    BOOL fSymbol;		/* TRUE if the font is decorative */
    short iFamily;		/* The fonts family */
    short iWeight;		/* TRUE if this is a bold font */
    short iFirstChar;		/* The first character in the font */
    short iLastChar;		/* The last character in the font */
    short rgWidths[256];	/* Character widths from 0x020 to 0x0ff */
    }FONT;



extern PutByte(short int);
extern PutWord(short int);
extern PutLong(long);

typedef struct
    {
    unsigned short int iKey;
    short int iKernAmount;
    }KX;

typedef struct
    {
    int cPairs; 	    /* The number of kerning pairs */
    KX	rgPairs[1024];
    }KP;

/* The info for a single kern track */
typedef struct
    {
    short iDegree;    /* The degree of kerning */
    short iPtMin;     /* The minimum point size */
    short iKernMin;   /* The minimum kern amount */
    short iPtMax;     /* The maximum point size */
    short iKernMax;   /* The maximum kern amount */
    }TRACK;

/* The track kerning table for a font */
typedef struct
    {
    short cTracks;	  /* The number of kern tracks */
    TRACK rgTracks[10];  /* The kern track information */
    }KT;




/* Character metrics */
typedef struct
    {
    RECT    rc;
    int     iWidth;
    }CM;

typedef struct
    {
    int iFirstChar;
    int iLastChar;
    int iAvgWidth;
    int iMaxWidth;
    int iItalicAngle;
    int iFamily;
    int ulOffset;
    int ulThick;
    int iAscent;
    int iDescent;
    BOOL fVariablePitch;
    char szFile[80];
    char szFont[80];
    char szFace[80];
    int  iWeight;
    KP	 kp;
    KT	 kt;
    RECT rcBBox;
    CM	rgcm[256];	    /* The character metrics */
    }AFM;

AFM afm;
