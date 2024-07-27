
/*********************************************************************
 * ENUM.C
 *
 * 20Aug87	sjp		Creation: from TEXT.C.
 *********************************************************************
 */

#include "globals.h"
#include "pscript.h"

void PASCAL ScaleFont(DV FAR *, LPFONTINFO, int, int);
void PASCAL SetLeading(LPFONTINFO, int, int, int);
int FAR PASCAL LoadFont(LPDV, LPSTR, LPDF);
int FAR PASCAL RealizeFont(DV FAR *, LPLOGFONT, LPFONTINFO, TEXTXFORM FAR *);
void near PASCAL InfoToStruct(LPFONTINFO, short, LPSTR);
int PASCAL MapFont(DV FAR *, LPLOGFONT, LPSTR, int);
int FAR PASCAL EnumDFonts(DV FAR *, LPSTR, FARPROC, LPSTR);

/* Conversion codes for the InfoToStruct function */
#define TO_TEXTXFORM	0
#define TO_TEXTMETRIC	1
#define TO_LOGFONT	2

/* The font precidence levels.	These are powers of two */
#define FP_ITALIC	     1
#define FP_WEIGHT	     2
#define FP_FAMILY	     4
#define FP_VARIABLEPITCH     8
#define FP_FIXEDPITCH	    16
#define FP_FACE 	    32


/****************************************************************************/
/*#define DEBUG_ON*/
#ifdef DEBUG_ON
#define DBMSG(msg) printf msg
#else
#define DBMSG(msg)
#endif

/*#define DEBUG_ON1*/
#ifdef DEBUG_ON1
#define DBMSG1(msg) printf msg
#else
#define DBMSG1(msg)
#endif

/* for RealizeFont testing */
/*#define DEBUG_ON4*/
#ifdef DEBUG_ON4
#define DBMSG4(msg) printf msg
#else
#define DBMSG4(msg)
#endif
/*********************************************************************/

/****************************************************************
* Name: LoadFont()
*
* Action: This routine loads a device font from either from
*	  a resource or from an external PFM file.  If the
*	  font's name starts with a $ sign, then the font
*	  metrics are located in an external PFM file.
*
*	  This routine is called twice for each font.  The
*	  first time a NULL destination ptr is passed in
*	  and this routine just returns the size of the font.
*
* Returns: The size of the device font.
*
*************************************************************
*/
int FAR PASCAL LoadFont(lpdv, lszFont, lpdf)
	LPDV lpdv;		/* Ptr to the device descriptor */
	LPSTR lszFont;		/* Ptr to the font's name string */
	LPDF lpdf;		/* Ptr to the place to put the device font */
{
    HANDLE hFont;	/* The font's memory handle */
    HANDLE hres;	/* The fonts resouce handle */
    int cbSrc;		/* The size of the device font */
    LPSTR lpbSrc;	/* Ptr to a place to put the font */
    LPFX lpfx;		/* Ptr to the extended font info */
    LPPFM lppfm;	/* Ptr to the pfm file in memory */
    int cbLeader;
    int cb;
    int fh;

	DBMSG4(((LPSTR)">LoadFont(): lpdv=%ld,lpdv->dh.hmod=%d,name=%ls\r",
		lpdv,lpdv->dh.hmod,lszFont));
    lppfm = (LPPFM) (long) NULL;
    cbLeader = ((LPSTR) &lppfm->df) - (LPSTR) lppfm;

    if (*lszFont=='$'){
		/* The font info is located in an external PFM file */
		if ((fh = _lopen((LPSTR) (lszFont + 1), READ)) < 0){
			DBMSG4(((LPSTR)"<LoadFont(): ERROR\r"));
		    return(0);
		}
		cb = (int)( _llseek(fh, 0L, 2) - (long)cbLeader );

		if (lpdf){
		    _llseek(fh, (long) cbLeader, 0);
		    _lread(fh, (LPSTR) lpdf, cb);
	    }
		_lclose(fh);

	}else{
		/* The font info is located in a resource */
		DBMSG4(((LPSTR)" LoadFont(): lpdv->dh.hmod=%d,name=%ls,my=%ls\r",
			lpdv->dh.hmod,lszFont,(LPSTR)(long)MYFONT));
		hres = FindResource(lpdv->dh.hmod, lszFont, (LPSTR)(long)MYFONT);

		if (!hres){
			DBMSG4(((LPSTR)"<LoadFont(): ERROR\r"));
		    return(0);
		}

		cb = SizeofResource(lpdv->dh.hmod, hres) - cbLeader;

		if (lpdf){
		    if (!(hFont = LoadResource(lpdv->dh.hmod, hres))){
				DBMSG4(((LPSTR)"<LoadFont(): ERROR\r"));
				return(0);
			}

		    /* Copy the fontinfo structure into the memory provided by GDI */
		    lpbSrc = LockResource(hFont);
		    lmemcpy((LPSTR) lpdf, lpbSrc + cbLeader, cb);
		    GlobalUnlock(hFont);
		    FreeResource(hFont);
		}
	}


    if (lpdf){
		/* Append the font-extra structure to the font */
		lpfx = (LPFX) (((LPSTR) lpdf) + cb);
		lpfx->dfFont = lpdf->dfDriverInfo - cbLeader;
		lpdf->dfDriverInfo = cb;

#ifdef WIN20
                /* Save the unscaled average width */
                lpfx->fxAvgWidth = lpdf->dfAvgWidth;
#endif

		/* Adjust the offsets since the copyright notice has been removed */
		lpdf->dfFace -= cbLeader;
		lpdf->dfDevice -= cbLeader;
		if (lpdf->dfTrackKernTable)
		    lpdf->dfTrackKernTable -= cbLeader;
		if (lpdf->dfPairKernTable)
		    lpdf->dfPairKernTable -= cbLeader;
		if (lpdf->dfExtentTable)
		    lpdf->dfExtentTable -= cbLeader;
		if (lpdf->dfExtMetricsOffset)
		    lpdf->dfExtMetricsOffset -= cbLeader;
	}

	DBMSG4(((LPSTR)"<LoadFont(): size=%d\r",cb+sizeof(FX)));
    return(cb + sizeof(FX));
}


/*******************************************************
* Name: RealizeFont()
*
* Action:  Realize a font specified by the logical font
*	   descriptor (LOGFONT) and the font transformation
*	   descriptor (FONTXFORM).  The font is realized
*	   in the form of a FONTINFO record.
*
*	   The realization process is broken down into
*	   the following subtasks:
*
*	    2. Scale the character width table for the requested
*	       font size.
*
* The precidence order of attributes for selecting a font
* are as given below (1 = highest, N = lowest).
*
*	1. An exact face-name match.
*	2. Pitch and Family
*	3. Bold
*	4. Italic
*
********************************************************
*/
int FAR PASCAL RealizeFont(lpdv, lplf, lpdf, lpft)
	DV FAR *lpdv;	    /* Far ptr to the device descriptor */
	LPLOGFONT lplf;     /* Far ptr to the input logical font */
	LPFONTINFO lpdf;    /* Far ptr to the memory alloc by GDI for the font */
	TEXTXFORM FAR *lpft;	/* Far ptr to the font transformation structure*/
{
    static szFont[40];

    int cb=0;	/* The size of the physical font record...0 means illegal */
    int iFont;
    LPFX lpfx;


	DBMSG4(((LPSTR)">RealizeFont(): lpdv=%ld\r",lpdv));

	/* check to see if MapFont encountered an illegal font e.g. vector font
	 * and return 0 size to the calling routine...RealizeObject().
	 */
    if((iFont = MapFont(lpdv, lplf, (LPSTR) szFont, sizeof(szFont)))<0){
		DBMSG4(((LPSTR)" RealizeFont(): illegal font...cb=%d\r",cb));
		goto EXIT;
	}
	DBMSG4(((LPSTR)" RealizeFont(): lpdv->dh.hmod=%d,name=%ls\r",
		lpdv->dh.hmod,(LPSTR)szFont));
    cb = LoadFont(lpdv, (LPSTR) szFont, lpdf);

    if (lpdf){

		lpfx = LockFont(lpdf);
		lpfx->iFont = iFont;

		/* Set the font metrics as specified in the logical font */
		ScaleFont(lpdv, lpdf, lplf->lfWidth, lplf->lfHeight);

		lpfx->orientation = lplf->lfOrientation;
		lpfx->escapement = lplf->lfEscapement;
		lpfx->lid = ++lpdv->dh.lid;

		lpdf->dfUnderline = lplf->lfUnderline;
		lpdf->dfStrikeOut = lplf->lfStrikeOut;

		InfoToStruct(lpdf, TO_TEXTXFORM, (LPSTR)lpft);
	}
	DBMSG(((LPSTR)" RealizeFont(): font O.K....cb=%d\r",cb));

EXIT:
	DBMSG4(((LPSTR)"<RealizeFont(): lpdv=%ld\r",lpdv));
    return(cb);
}


/**************************************************************
* Name: InfoToStruct()
*
* Action: Convert the physical font info (LPDF) structure
*	  to one of the three GDI structures: a) LOGFONT,
*	  b) TEXTXFORM, c) TEXTMETRIC.
*
*
* Note: Isn't it amazing that GDI need three different structures
*	to contain basically the same information!
*
*****************************************************************
*/

void near PASCAL InfoToStruct(lpdf, iStyle, lpb)
LPFONTINFO lpdf;	/* Far ptr to the device font metrics */
short iStyle;		/* The conversion style */
LPSTR lpb;		/* Far ptr to the output structure */
    {

    switch(iStyle)
	{
	case TO_LOGFONT:
		((LPLOGFONT)lpb)->lfHeight = lpdf->dfPixHeight;
		((LPLOGFONT)lpb)->lfWidth =  lpdf->dfAvgWidth;
		((LPLOGFONT)lpb)->lfEscapement = 0;
		((LPLOGFONT)lpb)->lfOrientation = 0;
		((LPLOGFONT)lpb)->lfItalic = lpdf->dfItalic;
		((LPLOGFONT)lpb)->lfWeight = lpdf->dfWeight;
		((LPLOGFONT)lpb)->lfUnderline = lpdf->dfUnderline;
		((LPLOGFONT)lpb)->lfStrikeOut = lpdf->dfStrikeOut;
		((LPLOGFONT)lpb)->lfCharSet = lpdf->dfCharSet;
		((LPLOGFONT)lpb)->lfOutPrecision = OUT_CHARACTER_PRECIS;
		((LPLOGFONT)lpb)->lfClipPrecision = CLIP_CHARACTER_PRECIS;
		((LPLOGFONT)lpb)->lfQuality = PROOF_QUALITY;
		((LPLOGFONT)lpb)->lfPitchAndFamily = lpdf->dfPitchAndFamily + 1;
		((LPLOGFONT)lpb)->lfFaceName[0] = 0;
		break;

	case TO_TEXTXFORM:
		((LPTEXTXFORM)lpb)->ftHeight = lpdf->dfPixHeight;
		((LPTEXTXFORM)lpb)->ftWidth = lpdf->dfAvgWidth;
		((LPTEXTXFORM)lpb)->ftEscapement = 0;
		((LPTEXTXFORM)lpb)->ftOrientation = 0;
		((LPTEXTXFORM)lpb)->ftWeight = lpdf->dfWeight;
		((LPTEXTXFORM)lpb)->ftItalic = lpdf->dfItalic;
		((LPTEXTXFORM)lpb)->ftUnderline = lpdf->dfUnderline;
		((LPTEXTXFORM)lpb)->ftStrikeOut = lpdf->dfStrikeOut;

		((LPTEXTXFORM)lpb)->ftOutPrecision = OUT_CHARACTER_PRECIS;
		((LPTEXTXFORM)lpb)->ftClipPrecision = CLIP_CHARACTER_PRECIS;
		((LPTEXTXFORM)lpb)->ftAccelerator = 0;
		((LPTEXTXFORM)lpb)->ftOverhang = OVERHANG;
		break;

	case TO_TEXTMETRIC:
		((LPTEXTMETRIC)lpb)->tmHeight = lpdf->dfPixHeight;
		((LPTEXTMETRIC)lpb)->tmAscent = lpdf->dfAscent;
		((LPTEXTMETRIC)lpb)->tmDescent = lpdf->dfPixHeight - lpdf->dfAscent;
		((LPTEXTMETRIC)lpb)->tmInternalLeading = lpdf->dfInternalLeading;
		((LPTEXTMETRIC)lpb)->tmExternalLeading = lpdf->dfExternalLeading;
		((LPTEXTMETRIC)lpb)->tmAveCharWidth = lpdf->dfAvgWidth;
		((LPTEXTMETRIC)lpb)->tmMaxCharWidth = lpdf->dfMaxWidth;
		((LPTEXTMETRIC)lpb)->tmItalic = lpdf->dfItalic;
		((LPTEXTMETRIC)lpb)->tmWeight = lpdf->dfWeight;
		((LPTEXTMETRIC)lpb)->tmUnderlined = 0;
		((LPTEXTMETRIC)lpb)->tmStruckOut = 0;
		((LPTEXTMETRIC)lpb)->tmFirstChar = lpdf->dfFirstChar;
		((LPTEXTMETRIC)lpb)->tmLastChar = lpdf->dfLastChar;
		((LPTEXTMETRIC)lpb)->tmDefaultChar = lpdf->dfDefaultChar + lpdf->dfFirstChar;
		((LPTEXTMETRIC)lpb)->tmBreakChar = lpdf->dfBreakChar + lpdf->dfFirstChar;
		((LPTEXTMETRIC)lpb)->tmPitchAndFamily = lpdf->dfPitchAndFamily;
		((LPTEXTMETRIC)lpb)->tmCharSet = lpdf->dfCharSet;
		 ((LPTEXTMETRIC)lpb)->tmOverhang = OVERHANG;
		((LPTEXTMETRIC)lpb)->tmDigitizedAspectX = HRES;
		((LPTEXTMETRIC)lpb)->tmDigitizedAspectY = VRES;
	}
}


/************************************************************
* Name: MapFont()
*
* Action: Search the font directory for the font that
*	  most closely matches the logical font.
*
* Returns: The font number of the selected font.
*
*
*************************************************************
*/
int PASCAL MapFont(lpdv, lplf, lszFont, cbFontName)
DV FAR *lpdv;		/* Far ptr to the device descriptor */
LPLOGFONT lplf; 	/* Far ptr to the logical font record */
LPSTR lszFont;
int cbFontName;
    {
    static char szFontCand[40];
    LPFONTINFO lpdf;	/* Far ptr to the physical font record */
    short cFonts;	/* The number of fonts in the directory */
    int   iPrecFont;	/* The highest precedence seen so far */
    int   iPrecCand;	 /* The precedence of the candiate font */
    int   dfFont;	 /* The currently selected font index */
    int   iFont;
    int   iFontCand;
    LPSTR lpbDir;	/* Far ptr to the font directory */


	DBMSG(((LPSTR)"***MapFont():\r"));

    lpbDir = LockFontDir(lpdv->dh.iPrinter);

    /* Search the font table for a font that comes closest to matching
     * the desired attributes.	This font has highest precidence.
     */
    cFonts = *((short far *)lpbDir)++;

    lpdf = (LPFONTINFO) (lpbDir + 4);
    dfFont = *((short far *)(lpbDir + 2));

    lstrcpy(lszFont, ((LPSTR)lpdf) + dfFont);


    iFont = 0;
    for (iFontCand=0; iFontCand<cFonts; ++iFontCand)
	{

	/* Get a ptr to the next entry in the font directory */
	lpdf = (LPFONTINFO)(lpbDir + 4);


	dfFont = *((short far *)(lpbDir + 2));

	lstrcpy((LPSTR) szFontCand, ((LPSTR)lpdf)+dfFont);

	lpbDir += *((short far *)lpbDir);   /* Bump to next entry */

	/* Assume zero precidence on the candidate directory entry */
	iPrecCand = 0;

	/* Check if this is a vector font */
	if(lplf->lfCharSet==OEM_CHARSET){
		iFont=-1;
		goto EXIT;
	}
	/* Check for a typeface match */
	if (!lstrcmp((LPSTR) lplf->lfFaceName, ((LPSTR)lpdf) + lpdf->dfFace))
	    iPrecCand += FP_FACE;

	/* Check for fixed pitch (second highest precidence) */
	if (!(lpdf->dfPitchAndFamily & 1))
	    {
	    if ((lplf->lfPitchAndFamily & 0x03)==FIXED_PITCH)
		iPrecCand += FP_FIXEDPITCH;
	    }
	else if ((lplf->lfPitchAndFamily & 0x03)==VARIABLE_PITCH)
	    iPrecCand += FP_VARIABLEPITCH;

	/* Check for a family match */
	if ((lplf->lfPitchAndFamily & 0x0fc) == (lpdf->dfPitchAndFamily & 0x0fc))
	    iPrecCand += FP_FAMILY;

	/* Check for a boldface weight match */
	if ((lplf->lfWeight>=FW_BOLD) && lpdf->dfWeight>=FW_BOLD)
	    iPrecCand += FP_WEIGHT;
	else if ((lplf->lfWeight<FW_BOLD) && lpdf->dfWeight<FW_BOLD)
	    iPrecCand += FP_WEIGHT;

	/* Check for an italic font match */
	if ((lplf->lfItalic & 1) && (lpdf->dfItalic & 1))
	    iPrecCand += FP_ITALIC;
	else if (!(lplf->lfItalic & 1) && !(lpdf->dfItalic & 1))
	    iPrecCand += FP_ITALIC;

	/* Select the current font if it has higher precidence */
	if (iFontCand==iFont)
	    iPrecFont = iPrecCand;
	else if (iPrecCand>iPrecFont)
	    {

	    lstrcpy(lszFont, (LPSTR) szFontCand);
	    iPrecFont = iPrecCand;
	    iFont = iFontCand;
	    }
	}

EXIT:
    UnlockFontDir(lpdv->dh.iPrinter);
	DBMSG(((LPSTR)"MapFont()***:  font (O.K. if >=0)...iFont=%d\r",iFont));
    return(iFont);
    }


/************************************************************
* Name: EnumDFonts()
*
* Action: This routine is used to enumerate the fonts available
*	  on the device. For each font, the callback function
*	  is called with the information for that font.  The
*	  callback function is called until there are no more
*	  fonts to enumerate or until the callback function
*	  returns zero.
*
* Note: All fonts are enumerated in a reasonable height (such
*	as 12 points so that dumb apps that don't realize that
*	they we can scale text will default to something reasonable.
*
*************************************************************
*/
int FAR PASCAL EnumDFonts(lpdv, lszFace,  lpfn, lpb)
DV FAR *lpdv;	    /* Far ptr to the device descriptor */
LPSTR lszFace;	    /* Far ptr to the facename string (may be NULL) */
FARPROC lpfn;	    /* Far ptr to the callback function */
LPSTR lpb;	    /* Far ptr to the client data (passed to callback) */
    {
    /* The static variables prevent stack overflow on recursion */
    static LOGFONT lf;
    static TEXTMETRIC tm;
    static char rgbFont[sizeof(FONTINFO) + 80];

    LPFONTINFO lpdf;
    int idf;		/* The font index */
    int cb;
    LPSTR lpbSrc;
    LPSTR lpbDst;
    short cFonts;
    int i;
    LPSTR lpbDir;

	/* status=1 means failure! */
	int iStatus=1;


	DBMSG(((LPSTR)">EnumDFont(): facename=%ls\r",lszFace));

    if( !(lpbDir = LockFontDir(lpdv->dh.iPrinter)) ){
		/* Couldn't get font directory! */
		DBMSG(((LPSTR)"<EnumDFont(): FAILURE...iStatus=%d\r",iStatus));
		goto EXIT;
	}

    /* Scan through the fonts in the font directory */

    cFonts = *((short far *)lpbDir)++;
	DBMSG(((LPSTR)" EnumDFont():  cFonts=%d\r",cFonts));

    for (idf=0; idf<cFonts; ++idf){
		cb = *((short far *)lpbDir);

		lpdf = (LPDF) (lpbDir + 4);
		lpbDir += cb;

		if ((!lszFace) || (!lstrcmp(lszFace, ((LPSTR)lpdf) + lpdf->dfFace))){
		    lmemcpy((LPSTR)rgbFont, (LPSTR) lpdf, cb - 4);
		    lpdf = (LPFONTINFO) rgbFont;


		    ScaleFont(lpdv, lpdf, 0, 0);
		    InfoToStruct(lpdf, TO_LOGFONT, (LPSTR) &lf);

		    /* Copy the face name to the logical font */

		    lpbSrc = ((LPSTR)lpdf) + lpdf->dfFace;
		    lpbDst = (LPSTR)lf.lfFaceName;

			DBMSG(((LPSTR)" EnumDFont():  LF_FACESIZE=%d\r",LF_FACESIZE));
		    for (i=LF_FACESIZE-1; i>0 && *lpbSrc; --i) *lpbDst++ = *lpbSrc++;
		    *lpbDst = 0;

		    InfoToStruct(lpdf, TO_TEXTMETRIC, (LPSTR) &tm);

		    if (!(iStatus=(*lpfn)((LOGFONT FAR *)&lf,(TEXTMETRIC FAR *)&tm,
				(int) DEVICE_FONTTYPE, lpb))
			){
				break;
			}
	    }
	}

    UnlockFontDir(lpdv->dh.iPrinter);
	DBMSG(((LPSTR)"<EnumDFont(): O.K. iStatus=%d\r",iStatus));

EXIT:
    return(iStatus);
    }


/**********************************************************
* Name: ScaleFont()
*
* Action: This funtion scales all width and height values
*	  in the FONTINFO record to match the specified
*	  font height and width.  Note that the values
*	  in a default FONTINFO record are based on a
*	  scale from 0 to EM (defined in globals.h).
*
*	?? Also, when an application requests a 10 point Courier
*	font, they actually get a 12 point font so that it
*	is exactly 10 pitch (to match a typewriter).
*
***********************************************************
*/

void PASCAL ScaleFont(lpdv, lpdf, cxFont, cyFontUser)
DV FAR *lpdv;	    /* Far ptr to the device descriptor */
LPFONTINFO lpdf;	/* Far ptr to the extended device font */
int cxFont;		/* The horizontal scale factor */
int cyFontUser;             /* The vertical scale factor */
{
    int sx;	    /* The horizontal scale factor */
    int i;

    LPFX lpfx;	    /* Far ptr to extra driver info */
    int cyFont;
	int cyFontForX;


	DBMSG1(((LPSTR)"***ScaleFont(): cxFont=%d,cyFontUser=%d,EM=%d\r",
		cxFont,cyFontUser,EM));

    /* Negative font height means exclude internal leading */
    if ((cyFont = cyFontUser) < 0){
        cyFont = - cyFont;
#if FALSE
		/* only needed if internal leading is non-zero.  See pfm.c in the
		 * way internal leading is compiled from .afm -> .pfm
		 */

		/* check for divide-by-zero */
        if ((lpdf->dfAscent - lpdf->dfInternalLeading) > 0){
	        cyFont = Scale(cyFont,EM,EM - lpdf->dfInternalLeading);
			DBMSG1(((LPSTR)
				"ScaleFont():NEG. font height...EM=%d,cyFont=%d\r",
				EM,cyFont));
		}
#endif
	}

    if (cxFont<0) cxFont = -cxFont;

    /* Default to a 10 point high font */
    if (cyFont==0) cyFont = Scale(10, lpdv->dh.iRes, 72);


    lpdf->dfPixHeight = cyFont;

	cyFontForX=cyFont;
#if FALSE
	/* correct when internal leading is non-zero */
	if(cyFontUser<0) cyFontForX-=lpdf->dfInternalLeading;
#endif

    SetLeading(lpdf, cyFont, lpdv->dh.iRes, cyFontUser);

	DBMSG1(((LPSTR)"ScaleFont():ascent=%d, ",lpdf->dfAscent));
    lpdf->dfAscent = Scale(lpdf->dfAscent, cyFont, EM);
	DBMSG1(((LPSTR)"ascent=%d\r",lpdf->dfAscent));

    if (!(lpdf->dfPitchAndFamily&1))       /* Fixed pitch (mono space) font */
        {

        /* If it is within two tenths of a point of 12 points, make it 12 */
        i = Scale(cyFont, 720, lpdv->dh.iRes);
        if (i>=98 && i<=102)
            cyFont = Scale(12, lpdv->dh.iRes, 72);  /* Make it 12 point */
        else
            cyFont = Scale(cyFont, 125, 100);       /* Scale it by 125% */
        }

	if (cxFont==0){
                cxFont=Scale(cyFontForX,lpdf->dfAvgWidth,EM);
                DBMSG1(((LPSTR)"ScaleFont():cxFont=%d,cyFontForX=%d,AvgWidth=%d\r",
                        cxFont,cyFontForX,lpdf->dfAvgWidth));
	}

    lpdf->dfPixWidth = cxFont;

    /* Compute the horizontal scale factor (at 1:1 sx==cyFont) */
    i = Scale(cyFontForX, lpdf->dfAvgWidth, EM);
	DBMSG1(((LPSTR)"ScaleFont()***:i=%d,cyFontForX=%d,MaxWidth=%d\r",
                i,cyFontForX,lpdf->dfAvgWidth));
    sx = Scale(cyFontForX, cxFont, i);

    /* Convert the font heigth to points (72nds of an inch) */
    lpdf->dfPoints = Scale(cyFontForX, 72, lpdv->dh.iRes);

    lpdf->dfVertRes = lpdv->dh.iRes;
    lpdf->dfHorizRes = lpdv->dh.iRes;

    /* Scale the font metrics to the desired size from a EM unit standard */
    lpdf->dfAvgWidth = Scale(lpdf->dfAvgWidth, sx, EM);
    lpdf->dfMaxWidth = Scale(lpdf->dfMaxWidth, sx, EM);


    if (lpdf->dfDriverInfo){
		lpfx = (LPFX) (((LPSTR)lpdf) + lpdf->dfDriverInfo);
		lpfx->sx = sx;
		lpfx->sy = cyFont;
	}
}


/***************************************************************
* Name: SetLeading()
*
* Action: Set the leading (inter-line spacing) value for a
*	  text font.  These values are based on some magic
*	  numbers supplied by Chris Larson who got them from
*	  somewhere in our publications department.  Note that
*	  these values are not a linear function of the font
*	  size.
*
*	  Prior to Chris Larson's mandate, the leading values
*	  were computed as 19.5% of the font height which produced
*	  output which closely matched the Macintosh output on the
*	  LaserWriter.
*
*	  Since Chris only had values for the Times-Roman, Helvetica,
*	  and Courier fonts, the LaserWriter fonts are broken out into
*	  four classes: Roman, Swiss, Modern, and other.  Each family
*	  (hopefully) has the same type of descenders, etc. so that
*	  the external leading is the same for all Roman fonts, etc.
*
********************************************************************
*/
void PASCAL SetLeading(lpdf, cyFont, iRes, cyFontUser)
LPFONTINFO lpdf;    /* Far ptr to the device font */
int  cyFont;	    /* The font height in dots */
int  iRes;	    /* The device resolution */
int  cyFontUser;
{
    int iPoints;
    int iLeading;

	DBMSG1(((LPSTR)"***SetLeading():cyFont=%d,iRes=%d,cyFontUser=%d\r",
		cyFont,iRes,cyFontUser));
    /* Compute the point size of the text */
    iPoints = Scale(cyFont, 72, iRes);

    switch(lpdf->dfPitchAndFamily & 0x0f0){
		case FF_ROMAN:
		    iLeading = 2;
		    break;
		case FF_SWISS:
		    /* Compute the leading based on the point size */
		    if (iPoints<=12) iLeading = 2;
		    else if (iPoints<14) iLeading = 3;
		    else iLeading = 4;
		    break;

		case FF_MODERN:
		    /* Courier font does not need any external leading */
		    iLeading = 0;
		    break;

		default:
		    /* Make the external leading be 19.6% of the font height */
		    iLeading = Scale(iPoints, 196, EM);
		    break;
	}

    lpdf->dfExternalLeading = Scale(iLeading, iRes, 72);

	DBMSG1(((LPSTR)"SetLeading():IntLeading=%d, ",lpdf->dfInternalLeading));
    lpdf->dfInternalLeading=Scale(lpdf->dfInternalLeading, cyFont, EM);
	DBMSG1(((LPSTR)"IntLeading=%d\r",lpdf->dfInternalLeading));
}



