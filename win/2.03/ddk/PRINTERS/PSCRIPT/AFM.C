/*****************************************************************
* This module of the afm compiler parses the afm file and collects
* information in the afm structure.  It then passes control to the
* pfm module which outputs the pfm file from the data in the afm
* structure.
*
*****************************************************************
*/

#include <fcntl.h>
#include <types.h>
#include <io.h>
#include <stat.h>

#include "pfm.h"

/****************************************************************************/
#ifdef DEBUG_ON
#define DBMSG(msg) printf msg
#else
#define DBMSG(msg)
#endif
/****************************************************************************/



#define TRUE	1
#define FALSE	0
#define NULL 0

typedef int BOOL;
BOOL fEOF = FALSE;
BOOL fUnGetLine = FALSE;
int fhIn;

#define FW_LIGHT    250
#define FW_NORMAL   400
#define FW_BOLD     700

#define IBULLET     0x095		/* 87-1-15 sec (was 1) */



char rgbBuffer[2048];	    /* The file buffer */
int cbBuffer;		    /* The number of bytes in the buffer */
char *pbBuffer; 	    /* Ptr to current location in buffer */


char rgbLine[160];	    /* The current line of text being processed */
char *szLine;		    /* Ptr to the current location in the line */


/* A lookup table key structure for converting strings to tokens */
typedef struct
    {
    char *szKey;	/* Ptr to the string */
    int iValue; 	/* The corresponding token value */
    }KEY;




/* The AFM tokens */
#define TK_UNDEFINED	    0
#define TK_EOF		    1
#define TK_STARTKERNDATA    2
#define TK_STARTKERNPAIRS   3
#define TK_KPX		    4
#define TK_ENDKERNPAIRS     5
#define TK_ENDKERNDATA	    6
#define TK_FONTNAME	    7
#define TK_WEIGHT	    8
#define TK_ITALICANGLE	    9

#define TK_ISFIXEDPITCH 	10
#define TK_UNDERLINEPOSITION	11
#define TK_UNDERLINETHICKNESS	12
#define TK_FONTBBOX		13
#define TK_CAPHEIGHT		14
#define TK_XHEIGHT		15
#define TK_DESCENDER		16
#define TK_ASCENDER		17
#define TK_STARTCHARMETRICS	18
#define TK_ENDCHARMETRICS	19
#define TK_ENDFONTMETRICS	20
#define TK_MSFACENAME		21
#define TK_MSBOLDNESS		22
#define TK_MSFAMILY		23
#define TK_STARTFONTMETRICS	24





/*************************************************************
* Name: szIsEqual()
*
* Action: Compare two NULL terminated strings.
*
* Returns: TRUE if they are equal
*	   FALSE if they are different
*
*************************************************************
*/
BOOL szIsEqual(sz1, sz2)
char *sz1;
char *sz2;
    {
    while (*sz1 && *sz2)
	{
	if (*sz1++ != *sz2++)
	    return(FALSE);
	}
    return(*sz1 == *sz2);
    }




/*************************************************************
* Name: szMove()
*
* Action: Copy a string.  This function will copy at most the
*	  number of bytes in the destination area - 1.
*
**************************************************************
*/
szMove(szDst, szSrc, cbDst)
char *szDst;		/* Ptr to the destination area */
char *szSrc;		/* Ptr to the source area */
int cbDst;		/* The size of the destination area */
    {
    while (*szDst++ = *szSrc++)
	if (--cbDst <= 0)
	    {
	    *(szDst-1) = 0;
	    break;
	    }
    }


/****************************************************************
* Name: GetBuffer()
*
* Action: Read a new buffer full of text from the input file.
*
* Note: If the end of file is encountered in this function then
*	the program is aborted with an error message.  Normally
*	the program will stop processing the input when it sees
*	the end of information keyword.
*
*****************************************************************
*/
BOOL GetBuffer()
    {
    cbBuffer = 0;
    if (!fEOF)
	{
	cbBuffer = read(fhIn, rgbBuffer, sizeof(rgbBuffer));
	if (cbBuffer<=0)
	    {
	    cbBuffer = 0;
	    fEOF = TRUE;
	    printf("Premature end of file encountered\n");
	    exit(1);
	    }
	}
    pbBuffer = rgbBuffer;
    return(fEOF);
    }




/*****************************************************************
* Name: UnGetLine()
*
* Action: This routine pushes the most recent line back into the
*	  input buffer.
*
******************************************************************
*/
UnGetLine()
    {
    fUnGetLine = TRUE;
    szLine = rgbLine;
    }





/******************************************************************
* Name: GetLine()
*
* Action: This routine gets the next line of text out of the
*	  input buffer.
*
*******************************************************************
*/
BOOL GetLine()
    {
    int cbLine;
    char bCh;

    if (fUnGetLine)
	{
	szLine = rgbLine;
	fUnGetLine = FALSE;
	return(FALSE);
	}

    cbLine = 0;
    szLine = rgbLine;
    *szLine = 0;
    if (!fEOF)
	{
	while(TRUE)
	    {
	    if (cbBuffer<=0)
		GetBuffer();
	    while(--cbBuffer>=0)
		{
		bCh = *pbBuffer++;
		if (bCh=='\n' || ++cbLine>sizeof(rgbLine))
		    {
		    *szLine = 0;
		    szLine = rgbLine;
		    EatWhite();
		    if (*szLine!=0)
			goto DONE;
		    szLine = rgbLine;
		    cbLine = 0;
		    continue;
		    }
		*szLine++ = bCh;
		}
	    }
	}
    *szLine = 0;

DONE:
    szLine = rgbLine;
    return(fEOF);
    }




/****************************************************************
* Name: EatWhite()
*
* Action: This routine moves the input buffer pointer forward to
*	  the next non-white character.
*
*****************************************************************
*/
EatWhite()
    {
    while (*szLine && (*szLine==' ' || *szLine=='\t'))
	++szLine;
    }






/*******************************************************************
* Name: GetWord()
*
* Action: This routine gets the next word delimited by white space
*	  from the input buffer.
*
********************************************************************
*/
GetWord(szWord, cbWord)
char *szWord;		/* Ptr to the destination area */
int cbWord;		/* The size of the destination area */
    {
    char bCh;

    EatWhite();
    while (--cbWord>0)
	{
	switch(bCh = *szLine++)
	    {
	    case 0:
		--szLine;
		goto DONE;
	    case ' ':
	    case '\t':
		--szLine;
		goto DONE;
	    case ';':
		*szWord++ = bCh;
		goto DONE;
	    default:
		*szWord++ = bCh;
		break;
	    }
	}
DONE:
    *szWord = 0;
    }





/***************************************************************
* Name: MapToken()
*
* Action: This routine maps an ascii key word into an integer token.
*
* Returns: The token value.
*
*****************************************************************
*/
int MapToken(szWord)
char *szWord;	    /* Ptr to the ascii keyword string */
    {
    static KEY rgKeys[] =
	{
	"FontBBox", TK_FONTBBOX,
	"StartFontMetrics", TK_STARTFONTMETRICS,
	"MSBoldness",	TK_MSBOLDNESS,
	"MSFaceName",	TK_MSFACENAME,
	"MSFamily",	TK_MSFAMILY,
	"FontName", TK_FONTNAME,
	"Weight",   TK_WEIGHT,
	"ItalicAngle", TK_ITALICANGLE,
	"IsFixedPitch", TK_ISFIXEDPITCH,
	"UnderlinePosition", TK_UNDERLINEPOSITION,
	"UnderlineThickness", TK_UNDERLINETHICKNESS,
	"CapHeight", TK_CAPHEIGHT,
	"XHeight", TK_XHEIGHT,
	"Descender", TK_DESCENDER,
	"Ascender", TK_ASCENDER,
	"StartCharMetrics", TK_STARTCHARMETRICS,
	"EndCharMetrics", TK_ENDCHARMETRICS,
	"StartKernData", TK_STARTKERNDATA,
	"StartKernPairs", TK_STARTKERNPAIRS,
	"KPX", TK_KPX,
	"EndKernPairs", TK_ENDKERNPAIRS,
	"EndKernData", TK_ENDKERNDATA,
	"EndFontMetrics", TK_ENDFONTMETRICS,
	NULL, 0
	};

    KEY *pkey;

    pkey = rgKeys;
    while (pkey->szKey)
	{
	if (szIsEqual(szWord, pkey->szKey))
	    {
	    return(pkey->iValue);
	    }
	++pkey;
	}
    return(TK_UNDEFINED);
    }





/************************************************************
* Name: GetNumber()
*
* Action: This routine parses an ASCII decimal number from the
*	  input file stream and returns its value.
*
**************************************************************
*/
int GetNumber()
    {
    int iVal;
    BOOL fNegative;

    fNegative = FALSE;

    iVal = 0;
    EatWhite();

    if (*szLine=='-')
	{
	fNegative = TRUE;
	++szLine;
	}
    if (*szLine<'0' || *szLine>'9')
	goto ERROR;
    while (*szLine>='0' && *szLine<='9')
	iVal = iVal * 10 + (*szLine++ - '0');
    if (fNegative)
	iVal = - iVal;

    if (*szLine==0 || *szLine==' ' || *szLine=='\t' || *szLine==';')
	return(iVal);

ERROR:
    printf("GetNumber: invalid number %s\n", szLine);
    printf("%s\n", rgbLine);
    exit(1);
    }





/******************************************************************
* Name: GetFloat()
*
* Action: This routine parses an ASCII floating point decimal number
*	  from the input file stream and returns its value scaled
*	  by a specified amount.
*
********************************************************************
*/
int GetFloat(iScale)
int iScale;	    /* The amount to scale the value by */
    {
    long lVal;
    long lDivisor;
    BOOL fNegative;
    int iFraction;

    EatWhite();

    fNegative = FALSE;
    lVal = 0L;

    if (*szLine=='-')
	{
	fNegative = TRUE;
	++szLine;
	}
    if (*szLine<'0' || *szLine>'9')
	goto ERROR;
    while (*szLine>='0' && *szLine<='9')
	lVal = lVal * 10 + (*szLine++ - '0');

    lDivisor = 1L;
    if (*szLine=='.')
	{
	++szLine;
	while (*szLine>='0' && *szLine<='9')
	    {
	    lVal = lVal * 10 + (*szLine++ - '0');
	    lDivisor = lDivisor * 10;
	    }
	}
    lVal = (lVal * iScale) / lDivisor;

    if (fNegative)
	lVal = - lVal;

    if (*szLine==0 || *szLine==' ' || *szLine=='\t' || *szLine==';')
	return((short)lVal);

ERROR:
    printf("GetFloat: invalid number %s\n", szLine);
    printf("%s\n", rgbLine);
    exit(1);
    }






/*********************************************************************
* Name: GetToken()
*
* Action: Get the next token from the input stream.
*
**********************************************************************
*/
int GetToken()
    {
    char szWord[80];

    if (*szLine==0)
	if (GetLine())
	    return(TK_EOF);
    GetWord(szWord, sizeof(szWord));
    return(MapToken(szWord));
    }


/****************************************************************
* Name: PrintLine()
*
* Action: Print a line of ASCII text with the white space stripped
*	  out.
*
******************************************************************
*/
PrintLine(szLine)
char *szLine;
    {
    char szWord[80];

    while (*szLine)
	{
	GetWord(szWord, sizeof(szWord));
	printf("%s ", szWord);
	}
    printf("\n");
    }




/***************************************************************
* Name: KxSort()
*
* Action: Sort the pair kerning data using the quicksort algorithm.
*
*****************************************************************
*/
KxSort(pkx1, pkx2)
KX *pkx1;
KX *pkx2;
    {
    static int iPivot;
    int iKernAmount;
    KX *pkx1T;
    KX *pkx2T;

    if (pkx1>=pkx2)
	return;

    iPivot = pkx1->iKey;;
    iKernAmount = pkx1->iKernAmount;
    pkx1T = pkx1;
    pkx2T = pkx2;

    while (pkx1T < pkx2T)
	{
	while (pkx1T < pkx2T)
	    {
	    if (pkx2T->iKey < iPivot)
		{
		pkx1T->iKey = pkx2T->iKey;
		pkx1T->iKernAmount = pkx2T->iKernAmount;
		++pkx1T;
		break;
		}
	    else
		--pkx2T;
	    }
	while (pkx1T < pkx2T)
	    {
	    if (pkx1T->iKey > iPivot)
		{
		pkx2T->iKey = pkx1T->iKey;
		pkx2T->iKernAmount = pkx1T->iKernAmount;
		--pkx2T;
		break;
		}
	    else
		++pkx1T;
	    }
	}
    pkx2T->iKey = iPivot;
    pkx2T->iKernAmount = iKernAmount;
    ++pkx2T;
    if ((pkx1T - pkx1) < (pkx2 - pkx2T))
	{
	KxSort(pkx1, pkx1T);
	KxSort(pkx2T, pkx2);
	}
    else
	{
	KxSort(pkx2T, pkx2);
	KxSort(pkx1, pkx1T);
	}
    }





/******************************************************************
* Name: ParseKernPairs()
*
* Action: Parse the pairwise kerning data.
*
*******************************************************************
*/
ParseKernPairs()
    {
    int i;
    int iCh1;
    int iCh2;
    int iKernAmount;
    KP *pkp;
    int iToken;
    int cPairs;
    char szWord[80];

    cPairs = GetNumber();

    pkp = &afm.kp;
    pkp->cPairs = cPairs;
    for (i=0; i<cPairs; ++i)
	{
	if (GetLine())
	    break;
	if (GetToken()!=TK_KPX)
	    {
	    UnGetLine();
	    break;
	    }
	GetWord(szWord, sizeof(szWord));
	iCh1 = GetCharCode(szWord);
	GetWord(szWord, sizeof(szWord));
	iCh2 = GetCharCode(szWord);
	iKernAmount = GetNumber();
	pkp->rgPairs[i].iKey = iCh2<<8 | iCh1;
	pkp->rgPairs[i].iKernAmount = iKernAmount;
	}

    GetLine();
    iToken = GetToken();

    if (fEOF)
	printf("GetPairs: Premature end of file encountered\n");
    else if (iToken!=TK_ENDKERNPAIRS)
	{
	printf("GetPairs: expected EndKernPairs\n");
	printf("%s\n", rgbLine);
	exit(1);
	}
    KxSort(&afm.kp.rgPairs[0], &afm.kp.rgPairs[afm.kp.cPairs - 1]);
    }




/********************************************************
* Name: ParseKernData()
*
* Action: Start processing the pairwise kerning data.
*
************************************************************
*/
ParseKernData()
    {
    int iToken;

    if (!GetLine())
	{
	if (GetToken()==TK_STARTKERNPAIRS)
	    ParseKernPairs();
	else
	    printf("ParseKernData: expected StartKernPairs\n");
	}
    else
	printf("ParseKernData: unexpected end of file\n");
    }




/***********************************************************
* Name: ParseFontName()
*
* Action: Move the font name from the input buffer into the afm
*	  structure.
*
*************************************************************
*/
ParseFontName()
    {
    EatWhite();
    szMove(afm.szFont, szLine, sizeof(afm.szFont));
    }




/*********************************************************
* Name: ParseMSFaceName()
*
* Action: Parse the windows facename entry in the input file
*	  and move it to the afm structure.
*
***********************************************************
*/
ParseMSFaceName()
    {
    EatWhite();
    szMove(afm.szFace, szLine, sizeof(afm.szFace));
    };







/*********************************************************
* Name: ParseMSBoldness()
*
* Action: Parse the windows boldness entry in the input file
*	  and move it to the afm structure.
*
***********************************************************
*/
ParseMSBoldness()
    {
    char szWord[80];
    GetWord(szWord, sizeof(szWord));

    if (szIsEqual(szWord, "Bold"))
	afm.iWeight = FW_BOLD;
    else
	afm.iWeight = FW_NORMAL;
    }









/*********************************************************
* Name: ParseMSFamily()
*
* Action: Parse the windows font family entry in the input file
*	  and set the corresponding field in the afm structure.
*
***********************************************************
*/
ParseMSFamily()
    {
    char szWord[80];
    GetWord(szWord, sizeof(szWord));
    if (szIsEqual(szWord, "Roman"))
	afm.iFamily = FF_ROMAN;
    else if (szIsEqual(szWord, "Swiss"))
	afm.iFamily = FF_SWISS;
    else if (szIsEqual(szWord, "Decorative"))
	afm.iFamily = FF_DECORATIVE;
    else if (szIsEqual(szWord, "Modern"))
	afm.iFamily = FF_MODERN;
    else
	{
	printf("MSFamily: invalid font family = %s\n", szWord);
	exit(1);
	}
    }



/******************************************************************
* Name: ParseWeight()
*
* Action: Parse the fonts weight and set the corresponding
*	  entry in the afm structure.
*
*******************************************************************
*/
ParseWeight()
    {
    char szWord[80];

    GetWord(szWord, sizeof(szWord));
    if (szIsEqual(szWord, "Book"))
	afm.iWeight = FW_NORMAL;
    else if (szIsEqual(szWord, "Bold"))
	afm.iWeight = FW_BOLD;
    else if (szIsEqual(szWord, "Light"))
	afm.iWeight = FW_LIGHT;
    else if (szIsEqual(szWord, "Medium"))
	afm.iWeight = FW_NORMAL;
    else if (szIsEqual(szWord, "Demi"))
	afm.iWeight = FW_BOLD;
    else if (szIsEqual(szWord, "Roman"))
	afm.iWeight = FW_NORMAL;
    else
	printf("ParseWeight: unknown font weight = \"%s\"\n", szWord);
    }




/**************************************************************
* Name: ParseCharMetrics()
*
* Action: Parse the character metrics entry in the input file
*	  and set the width and bounding box in the afm structure.
*
****************************************************************
*/
BOOL ParseCharMetrics()
    {
    int cChars;
    int i;
    int iWidth;
    int iChar;
    RECT rcChar;

    cChars = GetNumber();
    for (i=0; i<cChars; ++i)
	{
	if (GetLine())
	    {
	    printf("ParseCharMetrics: unexpected end of file encountered\n");
	    exit(1);
	    }
	iChar = ParseCharCode();
	 iWidth = ParseCharWidth();
	if (afm.iFamily==FF_DECORATIVE)
	    {
	    while (*szLine!=0)
		if (*szLine++ == ';')
		    break;
	    }
	else
	    iChar = ParseCharName();
	ParseCharBox(&rcChar);

	afm.rgcm[iChar].iWidth = iWidth;
	afm.rgcm[iChar].rc.top = rcChar.top;
	afm.rgcm[iChar].rc.left = rcChar.left;
	afm.rgcm[iChar].rc.right = rcChar.right;
	afm.rgcm[iChar].rc.bottom = rcChar.bottom;
	}
    GetLine();
    if (GetToken()!=TK_ENDCHARMETRICS)
	{
	printf("ParseCharMetrics: expected EndCharMetrics\n");
	printf("%s\n", rgbLine);
	exit(1);
	}
    }



/***************************************************************
* Name: ParseCharBox()
*
* Action: Parse the character's bounding box and return its
*	  dimensions in the destination rectangle.
*
****************************************************************
*/
ParseCharBox(prc)
RECT *prc;		/* Pointer to the destination rectangle */
    {
    char szWord[16];

    GetWord(szWord, sizeof(szWord));
    if (szIsEqual("B", szWord))
	{
	prc->left = GetNumber();
	prc->bottom = GetNumber();
	prc->right = GetNumber();
	prc->top = GetNumber();
	}
    else
	{
	printf("ParseCharBox: missing character box\n");
	printf("%s\n", rgbLine);
	exit(1);
	}
    EatWhite();
    if (*szLine++ != ';')
	{
	printf("ParseCharBox: missing semicolon\n");
	printf("%s\n", rgbLine);
	exit(1);
	}
    }




/*********************************************************
* Name: ParseCharName()
*
* Action: Parse a character's name and return its numeric value.
*
***********************************************************
*/
int ParseCharName()
    {
    int iChar;
    char szWord[16];

    EatWhite();
    GetWord(szWord, sizeof(szWord));
    if (szIsEqual("N", szWord))
	{
	GetWord(szWord, sizeof(szWord));
	iChar = GetCharCode(szWord);
	}
    else
	{
	printf("ParseCharName: expected name field\n");
	printf("%s\n", rgbLine);
	exit(1);
	}
    EatWhite();
    if (*szLine++ != ';')
	{
	printf("ParseCharName: expected semicolon\n");
	printf("%s\n", rgbLine);
	exit(1);
	}
    return(iChar);
    }




/***********************************************************
* Name: ParseCharWidth()
*
* Action: Parse a character's width and return its numeric
*	  value.
*
************************************************************
*/
int ParseCharWidth()
    {
    int iWidth;
    char szWord[16];


    GetWord(szWord, sizeof(szWord));
    if (szIsEqual("WX", szWord))
	{
	iWidth = GetNumber();
	if (iWidth==0)
	    {
	    printf("ParseCharWidth: zero character width\n");
	    printf("%s\n", rgbLine);
	    exit(1);
	    }
	EatWhite();
	if (*szLine++ != ';')
	    {
	    printf("ParseCharWidth: missing semicolon\n");
	    printf("%s\n", rgbLine);
	    exit(1);
	    }
	}
    else
	{
	printf("ParseCharWidth: expected \"WX\"\n");
	printf("%s\n", rgbLine);
	exit(1);
	}
    return(iWidth);
    }




/*****************************************************************
* Name: ParseCharCode()
*
* Action: Parse the ascii form of a character's code point and
*	  return its numeric value.
*
*****************************************************************
*/
int ParseCharCode()
    {
    int iChar;
    char szWord[16];

    iChar = 0;
    GetWord(szWord, sizeof(szWord));
    if (szIsEqual("C", szWord))
	{
	iChar = GetNumber();
	if (iChar==0)
	    {
	    printf("ParseCharCode: invalid character code\n");
	    printf("%s", rgbLine);
	    exit(1);
	    }
	EatWhite();
	if (*szLine++ != ';')
	    {
	    printf("ParseCharCode: missing semicolon\n");
	    printf("%s\n", rgbLine);
	    exit(1);
	    }
	}
    return(iChar);
    }



/****************************************************************
* Name: ParseBounding Box()
*
* Action: Parse a character's bounding box and return its size in
*	  the afm structure.
*
******************************************************************
*/
ParseBoundingBox()
    {
    afm.rcBBox.left = GetNumber();
    afm.rcBBox.bottom = GetNumber();
    afm.rcBBox.right = GetNumber();
    afm.rcBBox.top = GetNumber();
    }




/************************************************************
* Name: ParsePitchType()
*
* Action: Parse the pitch type and set the variable pitch
*	  flag in the afm structure.
*
*********************************************************
*/
int ParsePitchType()
    {
    int iChar;
    char szWord[16];

    EatWhite();
    GetWord(szWord, sizeof(szWord));
    if (szIsEqual("true", szWord))
	afm.fVariablePitch = FALSE;
    else
	afm.fVariablePitch = TRUE;
    }

/***********************************************************
* Name: InitAfm()
*
* Action: Initialize the afm structure.
*
***********************************************************
*/
InitAfm()
    {
    afm.iFirstChar = 0x20;
    afm.iLastChar = 0x0ff;
    afm.iAvgWidth = 0;
    afm.iMaxWidth = 0;
    afm.iItalicAngle = 0;
    afm.iFamily = 0;
    afm.ulOffset = 0;
    afm.ulThick = 0;
    afm.fVariablePitch = FALSE;
    afm.szFile[0] = 0;
    afm.szFont[0] = 0;
    afm.szFace[0] = 0;
    afm.iWeight = 400;
    afm.kp.cPairs = 0;
    afm.kt.cTracks = 0;
    }




main(argc, argv)
int argc;
char **argv;
    {
    int iToken;
    BOOL fPrint;
    BOOL fEndOfInput;
    BOOL fMSFamily;
    BOOL fMSFaceName;
    BOOL fMSBoldness;

    fMSFamily = TRUE;
    fMSFaceName = TRUE;
    fMSBoldness = TRUE;
    InitAfm();

    if (argc<2)
	{
	printf("afm: expected an AFM filename as an argument\n");
	exit(1);
	}
    else if (argc>2)
	{
	printf("afm: too many arguments\n");
	exit(1);
	}
    ++argv;

    szMove(afm.szFile, *argv, sizeof(afm.szFile));


    fhIn = open(afm.szFile, O_RDONLY);
    if (fhIn < 0)
	{
	printf("afm: Can't open %s\n", afm.szFile);
	exit(1);
	}

    fPrint = FALSE;
    fEndOfInput = FALSE;
    while (!fEndOfInput)
	{
	GetLine();
	iToken = GetToken();
	switch(iToken)
	    {
	    case TK_STARTFONTMETRICS:
		break;
	    case TK_STARTKERNDATA:
		ParseKernData();
		break;
	    case TK_FONTNAME:
		ParseFontName();
		break;
	    case TK_WEIGHT:
		ParseWeight();
		break;
	    case TK_ITALICANGLE:
		afm.iItalicAngle = GetFloat(10);
		break;
	    case TK_ISFIXEDPITCH:
		ParsePitchType();
		break;
	    case TK_UNDERLINEPOSITION:
		afm.ulOffset = abs(GetNumber());
		break;
	    case TK_UNDERLINETHICKNESS:
		afm.ulThick = GetNumber();
		break;
	    case TK_FONTBBOX:
		ParseBoundingBox();
		break;
	    case TK_CAPHEIGHT:
		break;
	    case TK_XHEIGHT:
		break;
	    case TK_DESCENDER:
                afm.iDescent = GetNumber();
		break;
	    case TK_ASCENDER:
                afm.iAscent = GetNumber();
		break;
	    case TK_STARTCHARMETRICS:
		if (fMSFamily)
		    {
		    printf("Missing \"MSFamily\" statement\n");
		    exit(1);
		    }
		ParseCharMetrics();
		break;
	    case TK_ENDFONTMETRICS:
		fEndOfInput = TRUE;
		break;
	    case TK_MSFACENAME:
		ParseMSFaceName();
		fMSFaceName = FALSE;
		break;
	    case TK_MSBOLDNESS:
		ParseMSBoldness();
		fMSBoldness = FALSE;
		break;
	    case TK_MSFAMILY:
		ParseMSFamily();
		fMSFamily = FALSE;
		break;
	    }
	szLine = rgbLine;
	if (fPrint)
	    PrintLine(szLine);
	}
    close(fhIn);
    if (fMSFaceName)
	{
	printf("Missing \"MSFaceName\" statement\n");
	exit(1);
	}
    if (fMSBoldness)
	{
	printf("Missing \"MSBoldness\" statement\n");
	exit(1);
	}
    if (fMSFamily)
	{
	printf("Missing \"MSFamily\" statement\n");
	exit(1);
	}

    FixCharWidths();
    SetAfm();
    MakeDf(FALSE);
    }

DumpWidthTable()
    {
    int i;

    for (i=0; i<256; ++i)
	printf("%d\n", afm.rgcm[i].iWidth);
    }



/******************************************************
* Name: GetCharMetrics()
*
* Action: Get the character metrics for a specified character.
*
*********************************************************
*/
GetCharMetrics(iChar, pcm)
int iChar;
CM *pcm;
    {
    CM *pcmSrc;

    pcmSrc = &afm.rgcm[iChar];
    pcm->iWidth = pcmSrc->iWidth;
    pcm->rc.top = pcmSrc->rc.top;
    pcm->rc.left = pcmSrc->rc.left;
    pcm->rc.bottom = pcmSrc->rc.bottom;
    pcm->rc.right = pcmSrc->rc.right;
    }



/*************************************************************
* Name: SetCharMetrics()
*
* Action: Set the character metrics for a specified character.
*
**************************************************************
*/
SetCharMetrics(iChar, pcm)
int iChar;
CM *pcm;
    {
    CM *pcmDst;

    pcmDst = &afm.rgcm[iChar];
    pcmDst->iWidth = pcm->iWidth;
    pcmDst->rc.top = pcm->rc.top;
    pcmDst->rc.left = pcm->rc.left;
    pcmDst->rc.bottom = pcm->rc.bottom;
    pcmDst->rc.right = pcm->rc.right;
    }




/************************************************************
* Name: GetSmallCM()
*
* Action: Compute the character metrics for small sized characters
*	  such as superscripts.
*
*************************************************************
*/
GetSmallCM(iCh, pcm)
int iCh;
CM *pcm;
    {

    GetCharMetrics(iCh, pcm);
    pcm->iWidth = pcm->iWidth / 2;
    pcm->rc.bottom = pcm->rc.top + (pcm->rc.top - pcm->rc.bottom)/2;
    pcm->rc.right = pcm->rc.left + (pcm->rc.right - pcm->rc.left)/2;
    }




/*************************************************************
* Name: SetFractionMetrics()
*
* Action: Set the character metrics for a fractional character
*	  which must be simulated.
*
**************************************************************
*/
SetFractionMetrics(iChar, iTop, iBottom)
int iChar;	/* The character code point */
int iTop;	/* The ascii numerator character */
int iBottom;	/* The denominator character */
    {
    int cxTop;	    /* The width of the numerator */
    int cxBottom;   /* The width of the denominator */
    CM cm;
#define IFRACTIONBAR  167


    /* Set denominator width to 60 percent of bottom character */
    GetCharMetrics(iBottom, &cm);
    cxBottom = (((long)cm.iWidth) * 60L)/100L;

    /* Set numerator width to 40 percent of top character */
    GetCharMetrics(iTop, &cm);
    cxTop = (((long)cm.iWidth) * 40L)/100L;

    cm.iWidth = iTop + iBottom + IFRACTIONBAR;
    cm.rc.right = cm.rc.left + cm.iWidth;
    SetCharMetrics(iChar, &cm);

    }




/***********************************************************************
* Name: FixCharWidths()
*
* Action: Fix up the character widths for those characters which
*	  must be simulated in the driver.
*
************************************************************************
*/
FixCharWidths()
    {
    CM cm1;
    CM cm2;
    CM cmBullet;
    int i;
    #define IMUWIDTH 576


    /* Don't modify the character widths if this is a decorative font */
    if (afm.iFamily == FF_DECORATIVE)
	return;

    GetCharMetrics(IBULLET, &cmBullet);

    for (i=0x07f; i<0x091; ++i)
		SetCharMetrics(i, &cmBullet);
    for (i=0x098; i<0x0a1; ++i)
		SetCharMetrics(i, &cmBullet);

    SetCharMetrics(0x0ad, &cmBullet);
    SetCharMetrics(0x0ae, &cmBullet);
    SetCharMetrics(0x0af, &cmBullet);
    SetCharMetrics(0x0ac, &cmBullet);
    SetCharMetrics(0x0d7, &cmBullet);

    GetCharMetrics('Y', &cm1);
    SetCharMetrics(0x0dd, &cm1);
    GetCharMetrics('P', &cm1);
    SetCharMetrics(0x0de, &cm1);
    GetCharMetrics('y', &cm1);
    SetCharMetrics(0x0fd, &cm1);
    GetCharMetrics('p', &cm1);
    SetCharMetrics(0x0fe, &cm1);

    GetCharMetrics('|', &cm1);
    SetCharMetrics(0x0a6, &cm1);

    GetCharMetrics('+', &cm1);
    SetCharMetrics(0x0b1, &cm1);

    GetCharMetrics(0x060, &cm1);
    SetCharMetrics(0x0b4, &cm1);

    GetCharMetrics('D', &cm1);
    SetcharMetrics(0x0d0, &cm1);


    GetSmallCM('1', &cm1);
    SetCharMetrics(0x0b9, &cm1);
    GetSmallCM('2', &cm1);
    SetCharMetrics(0x0b2, &cm1);
    GetSmallCM('3', &cm1);
    SetCharMetrics(0x0b3, &cm1);

    GetCharMetrics('X', &cm1);
    SetCharMetrics(0x0f0, &cm1);

    SetCharMetrics(0x0f7, &cmBullet);
    GetCharMetrics('1', &cm1);
    GetCharMetrics('2', &cm2);

    SetFractionMetrics(0x0bc, '1', '4');
    SetFractionMetrics(0x0bd, '1', '2');
    SetFractionMetrics(0x0be, '3', '4');

    GetCharMetrics(' ', &cm1);
    cm1.iWidth = IMUWIDTH;
    cm1.rc.right = cm1.rc.left + cm1.iWidth;
    SetCharMetrics(0x0b5, &cm1);

    }


/***************************************************************
* Name: SetAfm()
*
* Action: Set the character metrics in the afm to their default values.
*
*******************************************************************
*/
SetAfm()
    {
    int i, cx;

    afm.iFirstChar = 0x020;
    afm.iLastChar = 0x0ff;

    if (!afm.fVariablePitch)
	{
	cx = afm.rgcm[afm.iFirstChar].iWidth;
	for (i=afm.iFirstChar; i<=afm.iLastChar; ++i)
	    afm.rgcm[i].iWidth = cx;
	}

    SetAvgWidth();
    SetMaxWidth();
    }




/******************************************************************
* Name: SetAvgWidth()
*
* Action: This routine computes the average character width
*	  from the character metrics in the afm structure.
*
*******************************************************************
*/
SetAvgWidth()
    {
    CM *rgcm;
    short i;
    short iCh;	/* The mapped character */
    long cx;	/* The average character width */
    long cb;	/* The number of characters */

    rgcm = afm.rgcm;

    cx = 0L;
    cb = (long) (afm.iLastChar - afm.iFirstChar + 1);
    for (i=afm.iFirstChar; i<=afm.iLastChar; ++i){
		cx += (long) rgcm[i].iWidth;
	}
    afm.iAvgWidth = cx/cb;
}



/*****************************************************************
* Name: SetMaxWidth()
*
* Action: This routine computes the maximum character width from
*	  the character metrics in the afm structure.
*
******************************************************************
*/
SetMaxWidth()
    {
    CM *rgcm;
    short cx;
    int i;

    rgcm = afm.rgcm;

    cx = 0;
    for (i=afm.iFirstChar; i<=afm.iLastChar; ++i)
	if (rgcm[i].iWidth > cx)
	    cx = rgcm[i].iWidth;

    afm.iMaxWidth = cx;
    }
