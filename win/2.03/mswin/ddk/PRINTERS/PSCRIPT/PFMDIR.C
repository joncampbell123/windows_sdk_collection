#include <fcntl.h>
#include <types.h>
#include <io.h>
#include <stat.h>


#define FALSE	    0
#define TRUE	    1
#define NULL	    0

typedef int BOOL;
typedef char BYTE;
typedef short int WORD;
typedef long int DWORD;


char rgbLine[81];	    /* Buffer containing the current line */
char rgbBuffer[16384];	    /* The output buffer */
char rgbInput[4096];	    /* The input buffer */
int cbInput;		    /* The number of bytes in the input buffer */
char *pbInput;		    /* Ptr to the current input buffer location */
int cbBuffer;		    /* The number of bytes in the output buffer */
char *pbBuffer; 	    /* Ptr to the current output buffer location */
char *szLine;		    /* Ptr to the current position in the line */
int cFonts;		    /* The number of fonts */
int fhIn;		    /* The input file handle */
BOOL fEOF = FALSE;
BOOL fUnGetLine = FALSE;

int cNames;
char *rgszNames[100];
char rgbNames[14 * 100];

/* An image of the device font structure */
WORD dfFontName;
WORD dfVersion = 0x0100;	/* Version 1.00 */
DWORD dfSize;
char dfCopyright[60];
WORD dfType;
WORD dfPoints;
WORD dfVertRes;
WORD dfHorizRes;
WORD dfAscent;
WORD dfInternalLeading;
WORD dfExternalLeading;
BYTE dfItalic;
BYTE dfUnderline;
BYTE dfStrikeOut;
WORD dfWeight;
BYTE dfCharSet;
WORD dfPixWidth;
WORD dfPixHeight;
WORD dfPitchAndFamily;
WORD dfAvgWidth;
WORD dfMaxWidth;
BYTE dfFirstChar;
BYTE dfLastChar;
BYTE dfDefaultChar;
BYTE dfBreakChar;
WORD dfWidthBytes;
DWORD dfDevice;
DWORD dfFace;
DWORD dfBitsPointer;
DWORD dfBitsOffset;
WORD dfSizeFields;
DWORD dfExtMetricsOffset;
DWORD dfExtentTable;
DWORD dfOriginTable;
DWORD dfPairKernTable;
DWORD dfTrackKernTable;
DWORD dfDriverInfo;
DWORD dfReserved;
char szFace[80];
char szFont[80];




/******************************************************
* Name: szIsEqual()
*
* Action: Compare two null terminated strings.
*
* Returns: TRUE if the two strings are equal.
*	   FALSE if they are different.
*
*******************************************************
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




/*********************************************************
* Name: szMove()
*
* Action: Copy a string with error checking to make sure
*	  that the destination buffer does not overflow.
*
**********************************************************
*/
szMove(szDst, szSrc, cbDst)
char *szDst;
char *szSrc;
int cbDst;
    {
    while (*szDst++ = *szSrc++)
	if (--cbDst <= 0)
	    {
	    *(szDst-1) = 0;
	    break;
	    }
    }




/**********************************************************
* Name: GetBuffer()
*
* Action: Refill the input buffer.
*
* Returns: TRUE if the end of file is encountered.
*
***********************************************************
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
	    }
	}
    pbBuffer = rgbBuffer;
    return(fEOF);
    }




/*************************************************************
* Name: WriteBuffer()
*
* Action: Write the contents of the output buffer to the file.
*
**************************************************************
*/
WriteBuffer(fh)
int fh;
    {
    if ((cbBuffer > 0) && (fh >= 0))
	write(fh, rgbBuffer, cbBuffer);
    }



/************************************************************
* Name: ResetBuffer()
*
* Action: Reset the output buffer pointers so that the output
*	  buffer is empty.
*
*************************************************************
*/
ResetBuffer()
    {
    pbBuffer = rgbBuffer;
    cbBuffer = 0;
    }



/******************************************************
* Name: GetByte()
*
* Action: Get a byte from the input buffer.
*
*******************************************************
*/
int GetByte()
    {
    int iByte;

    if (cbInput>0)
	{
	iByte = *pbInput++ & 0x0ff;
	--cbInput;
	}
    else
	{
	printf("pfmdir: Premature end of file encountered\n");
	exit(1);
	}
    return(iByte);
    }


/********************************************************
* Name: GetWord()
*
* Action: Get a two byte word from the input buffer
*	  and return its integer value.
*
**********************************************************
*/
int GetWord()
    {
    int iWord;

    iWord = GetByte();
    iWord |= GetByte() << 8;
    return(iWord);
    }




/***************************************************************
* Name: GetLong()
*
* Action: Get a four byte integer from the input buffer and
*	  return its value.
*
****************************************************************
*/
long GetLong()
    {
    long lWord;

    lWord = GetWord();
    lWord |= ((long)GetWord()) << 16;
    return(lWord);
    }



/***************************************************
* Name: PutByte()
*
* Action: Append a byte to the output buffer.
*
****************************************************
*/
PutByte(iByte)
short int iByte;
    {
    if ((cbBuffer+1) > sizeof(rgbBuffer))
	{
	printf("PutByte: output buffer overflow\n");
	exit(1);
	}
    *pbBuffer++ = iByte & 0x0ff;
    ++cbBuffer;
    }



/**************************************************************
* Name: PutRgb()
*
* Action: Append an array of bytes to the output buffer.
*
***************************************************************
*/
PutRgb(pb, cb)
char *pb;
int cb;
    {
    while (--cb>=0)
	PutByte(*pb++);
    }



/***************************************************************
* Name: PutWord()
*
* Action: Append a two byte word to the output buffer.
*
***************************************************************
*/
PutWord(iWord)
short int iWord;
    {
    if ((cbBuffer+2) > sizeof(rgbBuffer))
	{
	printf("PutWord: output buffer overflow\n");
	exit(1);
	}

    *pbBuffer++ = iWord & 0x0ff;
    *pbBuffer++ = (iWord >> 8) & 0x0ff;
    cbBuffer += 2;
    }



/******************************************************************
* Name: PutLong()
*
* Action: Append a four byte integer to the output buffer.
*
******************************************************************
*/
PutLong(lWord)
long lWord;
    {
    PutWord((int) (lWord & 0x0ffffL));
    lWord >>= 16;
    PutWord((int) (lWord & 0x0ffffL));
    }




/******************************************************
* Name: PutString()
*
* Action: Append a null terminated string to the output
*	  buffer.
*
*******************************************************
*/
PutString(sz)
char *sz;
    {
    int bCh;

    do
	{
	bCh = *pbBuffer++ = *sz++;
	++cbBuffer;
	} while (bCh);
    }




/*************************************************************
* Name: UnGetLine()
*
* Action: Push the current line back into the input buffer.
*
**************************************************************
*/
UnGetLine()
    {
    fUnGetLine = TRUE;
    szLine = rgbLine;
    }



/****************************************************************
* Name: GetLine()
*
* Action: Get a line from the input buffer with redundant white
*	  space stripped out.
*
*****************************************************************
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
		if (GetBuffer())
		    break;
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



/***********************************************************
* Name: EatWhite()
*
* Action: Advance the input buffer ptr to the first non-white
*	  character.
*
************************************************************
*/
EatWhite()
    {
    while (*szLine && (*szLine==' ' || *szLine=='\t'))
	++szLine;
    }



/***********************************************************
* Name: GetName()
*
* Action: Get a white space delimited name from the input file.
*
*************************************************************
*/
GetName(szWord, cbWord)
char *szWord;
int cbWord;
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




/*********************************************************
* Name: GetFileName()
*
* Action: Get the next file name from the input file.
*
**********************************************************
*/
GetFileName(sz, cb)
char *sz;
int cb;
    {
    static char *szFile = rgbNames;
    char *szMarker;


    szMarker = szFile;
    if (--cFonts>=0)
	{
	while (*sz++ = *szFile++)
	    if (--cb==0)
		{
		printf("pfmdir: file name too long = %s\n", szMarker);
		exit(1);
		}
	}
    else
	*sz = 0;

    return(*szMarker!=0);
    }



/***************************************************************
* Name: LoadNames()
*
* Action: Load the font names from the input file.
*
****************************************************************
*/
LoadNames(szFile)
char *szFile;
    {
    char *pbSrc;
    char *pbDst;
    int cbNames;

    char szName[80];


    fhIn = open(szFile, O_RDONLY, S_IREAD);
    if (fhIn < 0)
	{
	printf("afm: Can't open %s\n", szFile);
	exit(1);
	}
    pbDst = rgbNames;
    cbNames = 0;
    while (!GetLine())
	{
	if ((cbNames + sizeof(szName)) > sizeof(rgbNames))
	    {
	    printf("pfmdir: ERROR name buffer overflow\n");
	    close(fhIn);
	    exit(1);
	    }
	GetName(szName, sizeof(szName));

	++cFonts;
	pbSrc = szName;
	while (*pbDst++ = *pbSrc++)
	    ++cbNames;
	++cbNames;
	}
    close(fhIn);
    }




/**************************************************************
* Name: WriteDir()
*
* Action: Write the font directory from the output buffer.
*
****************************************************************
*/
int WriteDir(szFile)
char *szFile;
    {
    int fh;
    char *pbDst;
    char szDir[80];

    pbDst = szDir;
    while (*szFile!=0 && *szFile!='.')
	*pbDst++ = *szFile++;
    *pbDst++ = '.';
    *pbDst++ = 'd';
    *pbDst++ = 'i';
    *pbDst++ = 'r';
    *pbDst++ = 0;


    fh = open(szDir, O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE);
    if (fh<=0)
	{
	printf("Can't create: %s\n", szDir);
	exit(1);
	}
    close(fh);
    fh = open(szDir, O_RDWR | O_BINARY | O_TRUNC);


    if ((cbBuffer > 0) && (fh >= 0))
	write(fh, rgbBuffer, cbBuffer);
    close(fh);

    }




/**************************************************************
* Name: GetDf()
*
* Action: Get the device font information by reading the pfm
*	  file.
*
***************************************************************
*/
GetDf(szFile)
char *szFile;
    {
    int fh;
    char *pb;

    pb = szFile;
    while (*pb!=0 && *pb!='.')
	++pb;
    if (*pb++!= '.')
	goto ERROR;
    if (*pb++!='p')
	goto ERROR;
    if (*pb++!='f')
	goto ERROR;
    if (*pb++!='m')
	goto ERROR;
    if (*pb++!=0)
	goto ERROR;

    fh = open(szFile, O_RDWR | O_BINARY);
    if (fh < 0)
	{
	printf("pfmdir: Can't open %s\n", szFile);
	exit(1);
	}
    cbInput = read(fh, rgbInput, sizeof(rgbInput));
    close(fh);

    pbInput = rgbInput;


    dfVersion = GetWord();
    pbInput += 64;
    cbInput -= 64;

    dfType = GetWord();
    dfPoints = GetWord();
    dfVertRes = GetWord();
    dfHorizRes = GetWord();
    dfAscent = GetWord();

    dfInternalLeading = GetWord();
    dfExternalLeading = GetWord();
    dfItalic = GetByte();
    dfUnderline = GetByte();
    dfStrikeOut = GetByte();
    dfWeight = GetWord();
    dfCharSet = GetByte();
    dfPixWidth = GetWord();
    dfPixHeight = GetWord();

    dfPitchAndFamily = GetByte();
    dfAvgWidth = GetWord();
    dfMaxWidth = GetWord();
    dfFirstChar = GetByte();
    dfLastChar = GetByte();
    dfDefaultChar = GetByte();

    dfBreakChar = GetByte();
    dfWidthBytes = GetWord();
    dfDevice = GetLong();
    dfFace = GetLong();
    dfBitsPointer = GetLong();
    dfBitsOffset = GetLong();
    dfSizeFields = GetWord();
    dfExtMetricsOffset = GetLong();
    dfExtentTable = GetLong();
    dfOriginTable = GetLong();
    dfPairKernTable = GetLong();
    dfTrackKernTable = GetLong();
    dfDriverInfo = GetLong();
    dfReserved = GetLong();

    szMove(szFace, rgbInput + dfFace, sizeof(szFace));


#if FALSE
    printf("dfVersion = %04x\n", dfVersion);
    printf("dfType = %d\n", dfType);
    printf("dfPoints = %d\n", dfPoints);
    printf("dfVertRes = %d\n", dfVertRes);
    printf("dfHorizRes = %d\n", dfHorizRes);
    printf("dfAscent = %d\n", dfAscent);
    printf("dfInternalLeading = %d\n", dfInternalLeading);
    printf("dfExternalLeading = %d\n", dfExternalLeading);
    printf("dfItalic = %02x\n", dfItalic);
    printf("dfUnderline = %02x\n", dfUnderline);
    printf("dfStrikeOut = %02x\n", dfStrikeOut);
    printf("dfWeight = %d\n", dfWeight);
    printf("dfCharSet = %02x\n", dfCharSet);
    printf("dfPixWidth = %d\n", dfPixWidth);
    printf("dfPixHeight = %d\n", dfPixHeight);
    printf("dfPitchAndFamily = %02x\n", dfPitchAndFamily);
    printf("dfAvgWidth = %d\n", dfAvgWidth);
    printf("dfMaxWidth = %d\n", dfMaxWidth);
    printf("dfFirstChar = %02x\n", dfFirstChar);
    printf("dfLastChar = %02x\n", dfLastChar);
    printf("dfDefaultChar = %02x\n", dfDefaultChar);
    printf("dfBreakChar = %02x\n", dfBreakChar);
    printf("dfWidthBytes = %d\n", dfWidthBytes);
    printf("dfDevice = %08lx\n", dfDevice);
    printf("dfFace = %08lx\n", dfFace);
    printf("dfBitsPointer = %08lx\n", dfBitsPointer);
    printf("dfBitsOffset = %08lx\n", dfBitsOffset);
    printf("dfSizeFields = %d\n", dfSizeFields);
    printf("dfExtMetricsOffset = %08lx\n", dfExtMetricsOffset);
    printf("dfExtentTable = %08lx\n", dfExtentTable);
    printf("dfOriginTable = %08lx\n", dfOriginTable);
    printf("dfPairKernTable = %08lx\n", dfPairKernTable);
    printf("dfTrackKernTable = %08lx\n", dfTrackKernTable);
    printf("dfDriverInfo = %08lx\n", dfDriverInfo);
    printf("dfReserved = %08lx\n", dfReserved);
#endif

    return;

ERROR:
    printf("pfmdir: Invalid file type = %s\n", szFile);
    exit(1);

    }



/*************************************************************
* Name: MakeDirEntry()
*
* Action: Set the device font data for the next directory
*	  entry in the output buffer.
*
* Note that the first pass is used to compute offset information
* only.
*
***************************************************************
*/
int MakeDirEntry(fPass2, szFile)
BOOL fPass2;		/* TRUE if this is the second pass */
char *szFile;
    {
    static char szFont[80];
    static char *pdf;
    static char *pbSave;
    static int	cbSave;
    static int iMarker;
    char *pbSrc;
    char *pbDst;
    int i;
    int j;


    if (!fPass2)
	{
	pbSrc = szFile;
	pbDst = szFont;
	while (*pbSrc!=0 && *pbSrc!='.')
	    *pbDst++ = *pbSrc++;
	*pbDst = 0;

	pbSave = pbBuffer;
	cbSave = cbBuffer;
	GetDf(szFile);
	}

    dfDriverInfo = 0L;
    dfPairKernTable = 0L;
    dfTrackKernTable = 0L;

    PutWord(dfSize);
    PutWord(dfFontName);

if (!fPass2)
    pdf = pbBuffer;

    PutWord(dfType);
    PutWord(dfPoints);
    PutWord(dfVertRes);
    PutWord(dfHorizRes);
    PutWord(dfAscent);
    PutWord(dfInternalLeading);
    PutWord(dfExternalLeading);
    PutByte(dfItalic);
    PutByte(dfUnderline);
    PutByte(dfStrikeOut);
    PutWord(dfWeight);
    PutByte(dfCharSet);
    PutWord(dfPixWidth);
    PutWord(dfPixHeight);
    PutByte(dfPitchAndFamily);
    PutWord(dfAvgWidth);
    PutWord(dfMaxWidth);
    PutByte(dfFirstChar);
    PutByte(dfLastChar);
    PutByte(dfDefaultChar);
    PutByte(dfBreakChar);
    PutWord(dfWidthBytes);

    PutLong(dfDevice);
    PutLong(dfFace );
    PutLong(dfBitsPointer);
    PutLong(dfBitsOffset);
    PutWord(dfSizeFields);
    PutLong(dfExtMetricsOffset);
    PutLong(dfExtentTable);
    PutLong(dfOriginTable);
    PutLong(dfPairKernTable);
    PutLong(dfTrackKernTable);
    PutLong(dfDriverInfo);
    PutLong(dfReserved);


    dfDevice = pbBuffer - pdf;
    PutString("PostScript");

    dfFace = pbBuffer - pdf;
    PutString(szFace);

    dfFontName = pbBuffer - pdf;
    PutString(szFont);


    dfOriginTable = NULL;

    if (!fPass2)
	{
	dfSize = pbBuffer - pbSave;
	pbBuffer = pbSave;
	cbBuffer = cbSave;
	MakeDirEntry(TRUE, szFile);
	}

    }






ShowNames()
    {
    char szWord[80];

    while (TRUE)
	{
	GetFileName(szWord, sizeof(szWord));
	if (szWord[0] == 0)
	    break;
	printf("%s\n", szWord);
	}
    }



/*****************************************************************
* Name: MakeDir()
*
* Action: This function controls the font directory creation
*	  by appending each font entry to the font directory
*
********************************************************************
*/
MakeDir()
    {
    char szFile[80];
    char *pb;
int j, bCh;


int i;
for (i=0; i<sizeof(rgbBuffer); ++i)
    rgbBuffer[i] = 0;

    cbBuffer = 0;
    pbBuffer = rgbBuffer;
    PutWord(cFonts);

    while (GetFileName(szFile, sizeof(szFile)))
	{
	MakeDirEntry(FALSE, szFile);
	}
#ifdef UNDEFINED

    i = 0;
    while (i<cbBuffer)
	{
	for (j=0; j<16; ++j)
	    {
	    bCh = rgbBuffer[i] & 0x0ff;
	    if (bCh>=' ' && bCh<=0x07f)
		printf("%c", bCh);
	    else
		printf(".");
	    ++i;
	    }
	printf("\n");
	}
#endif

    }


main(argc, argv)
int argc;
char **argv;
    {
    char szFile[80];

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
    LoadNames(*++argv);
    MakeDir();
    WriteDir(*argv);
    }
