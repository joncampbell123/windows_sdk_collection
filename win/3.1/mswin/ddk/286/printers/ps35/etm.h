/* font definitions */



/* The pair kerning structure */
/* Note: the kern amount is given in hundreths of a point per character */
typedef struct {
    int cPairs; 	    /* The number of kerning pairs */
    struct {
	int iKey;	    /* The kerning pair concatenated into a key */
	int iKernAmount;
    } rgPairs[1];
} KP;
typedef KP FAR *LPKP;


/* The info for a single kern track */
typedef struct {
    short iDegree;    /* The degree of kerning */
    short iPtMin;     /* The minimum point size */
    short iKernMin;   /* The minimum kern amount */
    short iPtMax;     /* The maximum point size */
    short iKernMax;   /* The maximum kern amount */
} TRACK;

/* The track kerning table for a font */
typedef struct {
	short cTracks;	  /* The number of kern tracks */
	TRACK rgTracks[1];	/* The kern track information */
} KT;
typedef KT FAR *LPKT;




typedef struct {
    short etmSize;
    short etmPointSize;
    short etmOrientation;
    short etmMasterHeight;
    short etmMinScale;
    short etmMaxScale;
    short etmMasterUnits;
    short etmCapHeight;
    short etmXHeight;
    short etmLowerCaseAscent;
    short etmLowerCaseDescent;	/* use to be: short etmUpperCaseDecent; */
    short etmSlant;
    short etmSuperScript;
    short etmSubScript;
    short etmSuperScriptSize;
    short etmSubScriptSize;
    short etmUnderlineOffset;
    short etmUnderlineWidth;
    short etmDoubleUpperUnderlineOffset;
    short etmDoubleLowerUnderlineOffset;
    short etmDoubleUpperUnderlineWidth;
    short etmDoubleLowerUnderlineWidth;
    short etmStrikeOutOffset;
    short etmStrikeOutWidth;
    WORD etmNKernPairs;
    WORD etmNKernTracks;
} ETM;


/* The format of the printer font metrics file */

typedef struct {
    WORD dfVersion;
    DWORD dfSize;
    char dfCopyright[60];
    FONTINFO df;
} PFM;
typedef PFM FAR *LPPFM;


/* The extended FONTINFO structure */
typedef struct {
    /* The first group of fields is set each time the font is used */
    LPFONTINFO lpdf;		/* A back ptr to the fontinfo structure */
    LPSTR lszFont;		/* Far ptr to the font name */
    char  lszFace[LF_FACESIZE];	/* face name */
    LPSTR lszDevType;		/* Far ptr to the device name */
    LPKT  lpkt; 		/* Far ptr to the track kerning table */
    LPKP  lpkp; 		/* Far ptr to the pair kerning table */
    short far *rgWidths;	/* Far ptr to the width table */

    /* The second group of fields are set when the font is realized */
    int iFont;			/* The font number */
    int sx;			/* The horizontal scale factor */
    int sy;			/* The vertical scale factor */
    int orientation;		/* The character rotation angle */
    int escapement;		/* The escapement vector angle */
    int fxUnscaledAvgWidth;     /* The unscaled average width */
    long lid;			/* The font instance id */
    DWORD dfFont;

    /*** later change this to be allocated as necessary ***/
    char sfLoadPath[40];
    int noTranslate;		/* djm 12/20/87 */
} FX;
typedef FX FAR *LPFX;


typedef FONTINFO DF;
typedef FONTINFO FAR *LPDF;


typedef TEXTXFORM FT;
typedef TEXTXFORM FAR *LPFT;


typedef EXTTEXTDATA ETD;
typedef EXTTEXTDATA FAR *LPETD;
typedef APPEXTTEXTDATA ATD;
typedef APPEXTTEXTDATA FAR *LPATD;



