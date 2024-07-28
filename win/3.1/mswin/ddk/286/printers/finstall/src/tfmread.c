/**[f******************************************************************
* tfmread.c -
*
* Copyright (C) 1989,1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*
**f]*****************************************************************/
  
/*
 * $Header: 
 */

/*
 * $Log:
 */

/*************************** tfmread.c **************************************/
/*
*  Summary:  Reads data (tags) into appropriate location in the TFM structure
*            depending on the tag name and number. If a certain tags
*            information is not required, the appropriate case statement
*            should be deleted and the default case will skip over the data.
*            In order to integrate this code into another program, the exits
*            should be changed to return codes.
*
*
*    Inputs:  tfmin - input file name.
*
*   Outputs:  a pointer to the TFM structure.
*
* Modifications:
*
*      9-Aug-89  dtk  change parameter types.
*      8-Aug-89  dtk  Comments added.
*/
/****************************************************************************/
  
  
//#define DEBUG
  
/* local includes */
  
#include "printer.h"
#include "tfmread.h"
#include "tfmdefs.h"
//#include "tfmstruc.h"
#include "windows2.h"
#include "neededh.h"
  
/****************************************************************************\
*                     Debug Definitions
\****************************************************************************/
  
/*  Local debug stuff.
*/
#ifdef DEBUG
    #define LOCAL_DEBUG
#endif
  
#ifdef LOCAL_DEBUG
    #define DBGerr(msg) /* DBMSG(msg) */
    #define DBGwinsf(msg)   /* DBMSG(msg) */
    #define DBGtfm(msg) /* DBMSG(msg) */
    #define DBGentry(msg)   /* DBMSG(msg) */
#else
    #define DBGerr(msg) /*null*/
    #define DBGwinsf(msg)   /*null*/
    #define DBGtfm(msg) /*null*/
    #define DBGentry(msg)   /*null*/
#endif
  
short Intel_byte_order;
short Num_typefaces,Num_symsets;
HANDLE  hFile;
BYTE huge *lpFile;
LONG    bufpos;
//extern int outfile;
  
void FAR PASCAL tfmread(HANDLE, LPSTR);
void FAR PASCAL FreeTFM(HANDLE);
  
/* void get_version(struct TFMType far *TFM, int fp); */
void   get_number_typefaces(struct TFMType far *TFM, int fp);
void   get_string(int fp, LPSTR string);
BYTE   getbyte(int fp);
short  getshort(int fp);
long   getlong(int fp);
/* double getrational(int fp); */
/* void getfname(int length, LPSTR string); */
  
  
/***************************************************************************/
  
void FAR PASCAL tfmread(HANDLE hT, LPSTR tfmin)
  
{
  
    int fp;         /* input file pointer */
    LONG i;
    int x,j,k,z;  /* counter variables */
  
    WORD
    index_length,   /* length of character index array */
    ptype,          /* processor type (Intel vs Motorola) */
    tag_value,      /* numerical tag value */
    data_type,      /* data type -- char, short, long ... */
    number_tags,    /* number of tags per typeface */
#ifdef KERN
    number_pairs,   /* number of kern pairs per symbol set */
    number_tracks,  /* number of tracks to kern per symbol set */
    number_sectors, /* number of sectors to kern per symbol set */
#endif
    number_chars;   /* number of characters per typeface */
  
    LONG
    filesize,    /* size of TFM file */
    remaining,       /* Unread bytes in glue file      */
    readsize,    /* # of bytes to read in _lread() */
    block_size,     /* size of the data for a tag (in its own data type */
    nextl,          /* used for incrementing current position */
    length,         /* size of the data for a tag (in bytes) */
    offset,         /* offset position or data area for a tag */
    index_offset,   /* offset to character index array */
    dir_offset;     /* offset to first directory */
  
    long pos,pos1;        /* temporary position holders */
    struct TFMType far *TFM;  /* pointer to TFM structure */
  
  
    short type_size[19];  /* This array holds the size (in bytes) of each data type */
  
    DBGentry(("tfmread: enter\n"));
  
    type_size[tagBYTE]       = 1;
    type_size[tagASCII]      = 1;
    type_size[tagSHORT]      = 2;
    type_size[tagLONG]       = 4;   /* byte sizes */
    type_size[tagRATIONAL]   = 8;
    type_size[tagSIGNEDBYTE] = 1;
    type_size[tagSIGNEDSHORT]= 2;
    type_size[tagSIGNEDLONG] = 4;
  
    /* Free TFM struct memory */
    FreeTFM(hT);
  
    if ((fp = _lopen(tfmin, 0)) < 0)     /* open input file */
        return;
  
    if ((remaining = filesize = _llseek(fp, 0L, 2)) < 0L)
        return;
  
    if (hFile = GlobalAlloc(GMEM_MOVEABLE, (DWORD)(filesize + 1L)))
    {
        if (!(lpFile = (BYTE huge *)GlobalLock(hFile)))
        {
            GlobalFree(hFile);
            return;
        }
    }
    else
        return;
  
    _llseek(fp,0L,0);   /* set pointer to begining of file */
  
    while (remaining)
    {
        if (remaining > MAXBUFSIZE)
            readsize = MAXBUFSIZE;
        else
            readsize = remaining;
  
        if (_lread(fp, (LPSTR)&lpFile[filesize-remaining],
            (WORD)readsize) < (WORD)readsize)
        {
            GlobalFree(hFile);
            return;
        }
  
        remaining -= readsize;
    }
  
    lpFile[filesize] = '\0';
  
    GlobalUnlock(hFile);
    _lclose(fp);       /* close the input file */
  
    bufpos = 0;
  
    /* lock down memory for TFM pointer */
    if (!(TFM = (struct TFMType far *)GlobalLock(hT)))
    {
        GlobalFree(hFile);
        return;
    }
  
    ptype = getshort(fp);          /* get byte order flag */
    TFM->processorFamily = (BYTE)ptype;
    if (ptype == 0X4949)
        Intel_byte_order = TRUE;      /* set byte order to Intel */
    else
        if (ptype ==0X4D4D)
            Intel_byte_order = FALSE;
        else
            return;
  
    DBGtfm(("tfmread: TFM->processorFamily = %c\n", TFM->processorFamily));
  
    // get_version(TFM,fp);           /* read TFM version number */
    getshort(fp);             /* skip TFM version number */
  
    get_number_typefaces(TFM, fp);  /* get # of typefaces in the TFM file */
  
    /* allocate memory for number of typefaces */
    if (!((TFM->hType = GlobalAlloc(GHND,
        (DWORD)(sizeof(struct typefaceType) * Num_typefaces))) &&
        (TFM->typeface = (struct typefaceType far *)GlobalLock(TFM->hType))))
        return;
  
    for(x = 0; x < Num_typefaces; x++)
    {
        dir_offset = getlong(fp);       /* get directory offset */
        bufpos = dir_offset;
        //  _llseek(fp,dir_offset,0);        /* goto offset position */
  
        number_tags = getshort(fp);     /* get number of tags */
  
        for (z=0; z<number_tags; z++)   /* read the specific tag information */
  
        {
            /* read tag information */
            tag_value = getshort(fp);
            data_type = getshort(fp);
            block_size = getlong(fp);
  
            /* calc # of bytes for tag */
            length = lmul((long)type_size[data_type], block_size);
  
            if (length > 4)      /* if length > 4 bytes then the data is offset */
            {
                offset = getlong(fp);         /* read offset */
                pos = bufpos;
                //    pos = _llseek(fp, 0L, 1); /* save position */
                bufpos = offset;
                //    _llseek(fp, offset, 0);   /* seek to offset position */
            }
            else
                offset = 0L;                  /* data is in offset region */
  
            /*************************************************************************/
  
            switch(tag_value)
  
            {
  
                /*
                case tagSUBFILE:      // tag must be present to ensure valid TFM file
  
                TFM->typeface->general.TFM_type = getshort(fp);
                getshort(fp);               // skip over unused bytes
  
                break;
                */
  
                case tagCOPYRIGHT:
  
                    get_string(fp, (LPSTR)TFM->typeface->general.copyright);     /* read copyright string */
  
                    if (length <= 4)  /* if length is less than or equal to 4, the data is not offset */
                        /* therefore, after the data is read, bytes must be skipped */
                    {
                        for(i=block_size; i < 4; i++)
                            getbyte(fp);         /* skip over unused bytes */
                    }
  
  
                    break;
  
  
                    /*
                    case tagCOMMENT:
  
                    get_string(fp, (LPSTR)TFM->typeface->general.comment);    // read comment string
  
                    if (length <= 4)  // If length is <= 4, the data is not offset
                    // After data is read, bytes must be skipped
                    {
                    for(i=block_size; i < 4; i++)
                    getbyte(fp);         // skip over unused bytes
                    }
  
                    break;
                    */
  
  
                case tagSYMBOLMAP:  // this tag must exist -- it provides the # of chars
  
                    number_chars = (short)block_size; /* # of chars in typeface */
                    TFM->typeface->general.numberCharacters = number_chars;
  
                    /* Cover the unlikely case that there's < 4 chars */
  
                    if (length <= 4)  // If length is <= 4, the data is not offset
                        // After data is read, bytes must be skipped
                    {
                        for(i=block_size; i < 4; i++)
                            getbyte(fp);         // skip over unused bytes
                    }
  
                    /*          Don't need MSL #'s for chars
                    If you uncomment this, remove the conditional above
  
                    // allocate memory for the symbol set map and chars
                    // length = total # of bytes (typesize[data_type] * number_chars)
  
                    if (!((TFM->typeface->symbol.hSymMap = GlobalAlloc(GHND,
                    (DWORD)(length))) && (TFM->typeface->symbol.symbolMap =
                    (LPWORD)GlobalLock(TFM->typeface->symbol.hSymMap))))
                    return;
                    */
  
                    if (!((TFM->typeface->characterMetrics.hChar = GlobalAlloc(GHND,
                        (DWORD)(TFM->typeface->general.numberCharacters * sizeof(struct characterType)))) &&
                        (TFM->typeface->characterMetrics.character =
                        (struct characterType far *)GlobalLock(TFM->typeface->characterMetrics.hChar))))
                        return;
  
                    /*          Don't need MSL #'s for chars
                    If you uncomment this, remove the conditional at top of case
  
                    for (j=0; j<number_chars; j++)     // read MSL # for every char
                    TFM->typeface->symbol.symbolMap[j] = getshort(fp);
                    */
  
                    break;
  
  
                case tagSYMBOLSETDIR:
  
                    /* calc # of symsets in typeface */
                    Num_symsets = (((short)block_size)/14);
  
                    /* assign the number of symbol sets to the structure variable */
                    TFM->typeface->general.numberSymbolSets = Num_symsets;
  
                    /* allocate memory for symbol set directory */
                    if (!((TFM->typeface->symbol.hSymDir = GlobalAlloc(GHND,
                        (DWORD)(Num_symsets * sizeof(struct symbolSetDirectoryType)))) &&
                        (TFM->typeface->symbol.symbolSetDirectory =
                        (struct symbolSetDirectoryType far *)GlobalLock(TFM->typeface->symbol.hSymDir))))
                        return;
  
                    for(j=0; j<(Num_symsets); j++)  /* read in data for each symset */
  
                    {
                        nextl = getlong(fp);          /* read offset to symset name */
                        pos1 = bufpos;
                        //      pos1 = _llseek(fp, 0L, 1);    /* save position */
  
                        bufpos = nextl;
                        //      _llseek(fp, nextl, 0);        /* seek to offset location */
                        /* read symbol set name */
                        get_string(fp, (LPSTR)TFM->typeface->symbol.symbolSetDirectory[j].symbolName);
                        bufpos = pos1;
                        //      _llseek(fp, pos1, 0);         /* restore position */
  
                        nextl = getlong(fp);          /* read offset to selection str */
                        pos1 = bufpos;
                        //      pos1 = _llseek(fp, 0L, 1);    /* save position */
  
                        bufpos = nextl;
                        //      _llseek(fp, nextl, 0);        /* seek to offset location */
                        /* read selection string */
                        get_string(fp, (LPSTR)TFM->typeface->symbol.symbolSetDirectory[j].selectionName);
                        bufpos = pos1;
                        //      _llseek(fp, pos1, 0);         /* restore position */
  
                        index_offset = getlong(fp);   /* read offset to index array */
                        index_length = getshort(fp);  /* read length of index array */
                        pos1 = bufpos;
                        //      pos1 = _llseek(fp, 0L, 1);    /* save position */
                        /* assign symbol set index array length to structure variable */
                        TFM->typeface->symbol.symbolSetDirectory[j].symbolLength =
                        (WORD)index_length;
  
                        /* allocate memory to tempory pointer for symbol set index array */
                        if (!((TFM->typeface->symbol.symbolSetDirectory[j].hSI =
                            GlobalAlloc(GHND,
                            (DWORD)(sizeof(WORD) * index_length))) &&
                            (TFM->typeface->symbol.symbolSetDirectory[j].symbolIndex =
                            (LPWORD)GlobalLock(TFM->typeface->symbol.symbolSetDirectory[j].hSI))))
                            return;
  
                        bufpos = index_offset;
                        //             _llseek(fp, index_offset, 0);
  
                        for(k = 0; k < index_length; k++)  /* read symbol indicies */
                            TFM->typeface->symbol.symbolSetDirectory[j].symbolIndex[k] =
                            getshort(fp);
  
                        bufpos = pos1;
                        //             _llseek(fp, pos1, 0);  /* restore position */
  
  
                    }
                    break;
  
  
                    /*
                    case tagUNIQUEASSOCID:
  
                    // read in unique association identification
                    get_string(fp, (LPSTR)TFM->typeface->general.uniqueAssociationID);
  
                    if (length <= 4)  // If length is <= to 4, the data is not offset
                    {                 // After the data is read, bytes must be skipped
                    for(i=block_size; i < 4; i++)
                    getbyte(fp);    // skip over unused bytes
                    }
  
                    break;
                    */
  
  
                case tagPOINT:
  
                    //          TFM->typeface->typefaceMetrics.point = getrational(fp);  /* read pt size */
  
                    TFM->typeface->typefaceMetrics.pointN = getlong(fp);
                    TFM->typeface->typefaceMetrics.pointD = getlong(fp);
  
                    break;
  
  
                case tagNOMINALPOINT:
  
                    /* read in nominal point size */
                    //          TFM->typeface->typefaceMetrics.nominalPointSize = getrational(fp);
  
                    TFM->typeface->typefaceMetrics.nominalPointSizeN = getlong(fp);
                    TFM->typeface->typefaceMetrics.nominalPointSizeD = getlong(fp);
  
                    break;
  
  
                case tagDESIGNUNITS:
  
                    //          TFM->typeface->typefaceMetrics.designUnits = getrational(fp);   /* read in design units */
  
                    TFM->typeface->typefaceMetrics.designUnitsN = getlong(fp);
                    TFM->typeface->typefaceMetrics.designUnitsD = getlong(fp);
  
                    break;
  
  
                case tagTYPESTRUCT:
  
                    TFM->typeface->typefaceMetrics.typeStruct = getbyte(fp);
                    getbyte(fp);               // skip over unused bytes
                    getshort(fp);
  
                    break;
  
  
                case tagSTROKEWT:
  
                    TFM->typeface->typefaceMetrics.strokeWeight = getbyte(fp);   /* read in stroke weight */
                    getbyte(fp);               /* skip over unused bytes */
                    getshort(fp);
  
                    break;
  
  
                case tagSPACING:
  
                    TFM->typeface->typefaceMetrics.spacing = getshort(fp);   /* read spacing index */
                    getshort(fp);           /* skip over unused byte */
  
                    break;
  
  
                case tagSLANT:
  
                    TFM->typeface->typefaceMetrics.slant = getshort(fp);  /* read in slant value */
                    getshort(fp);           /* skip over unused byte */
  
                    break;
  
  
                case tagAPPEARWIDTH:
  
                    TFM->typeface->typefaceMetrics.appearanceWidth = getbyte(fp);
                    getbyte(fp);                 // skip over unused bytes
                    getshort(fp);
  
                    break;
  
  
                case tagSERIFSTYLE:
  
                    TFM->typeface->typefaceMetrics.serifStyle = getbyte(fp);   /* read serif style */
                    getbyte(fp);              /* skip over unused bytes */
                    getshort(fp);
  
                    break;
  
  
                    /*
                    case tagTYPESTYLE:        // This tag is obsolete
  
                    TFM->typeface->typefaceMetrics.typeStyle = getbyte(fp);
                    getbyte(fp);            // skip over unused bytes
                    getshort(fp);
  
                    break;
                    */
  
  
                case tagTYPEFACE:
  
                    // if length is less than or equal to 4, the data is not offset
                    // therefore, after the data is read, bytes must be skipped
                    get_string(fp, (LPSTR)TFM->typeface->general.typeface);
  
                    if (length <= 4)
                    {
                        for(i=block_size; i < 4; i++)
                            getbyte(fp);     // skip over unused byte
                    }
  
                    break;
  
  
                    /*
                    case tagTFSOURCE:
  
                    // if length is less than or equal to 4, the data is not offset
                    // therefore, after the data is read, bytes must be skipped
  
                    // read typeface source string
                    get_string(fp, (LPSTR)TFM->typeface->general.typefaceSource);
  
                    if (length <= 4)
                    {
                    for(i=block_size; i < 4; i++)
                    getbyte(fp);         // skip over unused byte
                    }
  
                    break;
                    */
  
  
                case tagAVERAGEWD:
  
                    //            TFM->typeface->typefaceMetrics.averageWidth = getrational(fp);   /* read average width */
  
                    TFM->typeface->typefaceMetrics.averageWidthN = getlong(fp);
                    TFM->typeface->typefaceMetrics.averageWidthD = getlong(fp);
  
                    break;
  
  
                case tagMAXWIDTH:
  
                    TFM->typeface->typefaceMetrics.maximumWidth = getshort(fp);  /* read maximum width */
                    getshort(fp);             /* skip over unused bytes */
  
                    break;
  
  
                    /*
                    case tagINTERWORDSP:
  
                    // read inter-word spacing
                    TFM->typeface->typefaceMetrics.inter_wordSpacing = getshort(fp);
                    getshort(fp);                // skip over unused bytes
  
                    break;
                    */
  
  
                case tagRECLINESP:
  
                    /* read recommended line spacing */
                    TFM->typeface->typefaceMetrics.recommendedLineSpacing = getshort(fp);
                    getshort(fp);             /* skip over unused bytes */
  
                    break;
  
  
                case tagCAPHEIGHT:
  
                    TFM->typeface->typefaceMetrics.capheight = getshort(fp);   /* read capheight value*/
                    getshort(fp);             /* skip over unused bytes */
  
                    break;
  
  
                case tagXHEIGHT:
  
                    TFM->typeface->typefaceMetrics.xHeight = getshort(fp);     /* read X height value */
                    getshort(fp);          /* skip over unused bytes */
  
                    break;
  
  
                case tagASCENT:
  
                    TFM->typeface->typefaceMetrics.ascent = getshort(fp);      /* read ascent value */
                    getshort(fp);          /* skip over unused bytes */
  
                    break;
  
  
                    /*
                    case tagDESCENT:
  
                    TFM->typeface->typefaceMetrics.descent = getshort(fp);  // read descent value
                    getshort(fp);           // skip over unused bytes
  
                    break;
                    */
  
  
                case tagLOWERASCENT:
  
                    TFM->typeface->typefaceMetrics.lowercaseAscent = getshort(fp);   /* read lowercase ascent */
                    getshort(fp);                /* skip over unused bytes */
  
                    break;
  
  
                case tagLOWERDESCENT:
  
                    TFM->typeface->typefaceMetrics.lowercaseDescent = getshort(fp);  /* read lowercase descent */
                    getshort(fp);                 /* skip over unused bytes */
  
                    break;
  
  
                case tagUNDERDEPTH:
  
                    /* read underscore descent */
                    TFM->typeface->typefaceMetrics.underscoreDescent = getshort(fp);
                    getshort(fp);              /* skip over unused bytes */
  
                    break;
  
  
                case tagUNDERTHICK:
  
                    /* read underscore thickness */
                    TFM->typeface->typefaceMetrics.underscoreThickness = getshort(fp);
                    getshort(fp);              /* skip over unused bytes */
  
                    break;
  
  
                    /*
                    case tagUPPERACCENT:
  
                    // read uppercase accent height
                    TFM->typeface->typefaceMetrics.uppercaseAccentHeight = getshort(fp);
                    getshort(fp);                // skip over unused bytes
                    break;
                    */
  
  
                    /*
                    case tagLOWERACCENT:
  
                    // read lowercase accent height
                    TFM->typeface->typefaceMetrics.lowercaseAccentHeight = getshort(fp);
                    getshort(fp);                // skip over unused bytes
  
                    break;
                    */
  
  
                case tagHORIZONTALESC:
  
                    for (j=0; j<(int)block_size; j++)  /* horiz escapement for each char */
                        TFM->typeface->characterMetrics.character[j].horizontalEscapement = getshort(fp);
  
                    break;
  
  
                    /*
                    case tagVERTICALESC:
  
                    for (j=0; j<block_size; j++)    // read vertical escapement for each character
                    TFM->typeface->characterMetrics.character[j].verticalEscapement = getshort(fp);
  
                    break;
  
  
                    case tagLEFTEXTENT:
  
                    for (j=0; j<block_size; j++)    // read left extent for each character
                    TFM->typeface->characterMetrics.character[j].leftExtent =
                    getshort(fp);
  
                    break;
  
  
                    case tagRIGHTEXTENT:
  
                    for (j=0; j<block_size; j++)    // read right extent for each character
                    TFM->typeface->characterMetrics.character[j].rightExtent =
                    getshort(fp);
  
                    break;
  
  
                    case tagCHARASCENT:
  
                    for (j=0; j<block_size; j++)    // read character ascent for each character
                    TFM->typeface->characterMetrics.character[j].characterAscent =
                    getshort(fp);
  
                    break;
  
  
                    case tagCHARDESCENT:
  
                    for (j=0; j<block_size; j++)    // read chararacter descent for each character
                    TFM->typeface->characterMetrics.character[j].characterDescent =
                    getshort(fp);
  
                    break;
                    */
  
#ifdef KERN
                case tagKERNPAIRS:
  
                    /* allocate memory for kern pairs */
                    if (!((TFM->typeface->kerning.hKern = GlobalAlloc(GHND,
                        (DWORD)(length))) && (TFM->typeface->kerning.kernPairs =
                        (struct kernPairsType far *)GlobalLock(TFM->typeface->kerning.hKern))))
                        return;
  
                    number_pairs = getshort(fp);  /* get number of kern pairs */
                    /* assign number of kern pairs to structure variable */
                    TFM->typeface->kerning.numberPairs = number_pairs;
                    TFM->typeface->kerning.realPairs = 0;
  
                    for (j=0; j<number_pairs; j++)   /* read in pair kerning information for each character */
  
                    {
                        /* read first character index */
                        TFM->typeface->kerning.kernPairs[j].firstCharIndex = getshort(fp);
  
                        /* read second character index */
                        TFM->typeface->kerning.kernPairs[j].secondCharIndex = getshort(fp);
  
                        /* read kerning value */
                        TFM->typeface->kerning.kernPairs[j].kernValue = getshort(fp);
  
                        /* Update count of real pairs */
                        if (TFM->typeface->kerning.kernPairs[j].kernValue)
                            TFM->typeface->kerning.realPairs++;
                    }
  
                    break;
#endif
  
                    /*
                    case tagSECTORKERN:
  
                    number_chars = getshort(fp);  // get # of sector kern characters
  
                    // assign number of sector kern characters to structure variable
                    TFM->typeface->kerning.numberKernChars = number_chars;
  
                    // allocate memory for sector kern characters
                    if ((TFM->typeface->kerning.sectorKernChar = (struct sectorKernCharType *)
                    malloc(sizeof(struct sectorKernCharType) * (number_chars))) == NULL)
                    {
                    printf("Insufficient memory");
                    exit(1);
                    }
  
                    number_sectors = getshort(fp); // get number of sectors per char
  
                    // assign number of sectors to structure variable
                    TFM->typeface->kerning.numberSectors = number_sectors;
  
  
                    // read in sector kerning information for each sector kern char
                    for (j=0; j<number_chars; j++)
  
                    {
                    // read character index
                    TFM->typeface->kerning.sectorKernChar[j].charIndex = getshort(fp);
  
                    if ((temp1 = (short *)   // alloc mem to a temp ptr for left sector
                    malloc((number_sectors) * (type_size[tagSHORT]))) == NULL)
                    {
                    printf("Insufficient memory");
                    exit(1);
                    }
  
                    if ((temp2 = (short *)   // alloc mem to a temp ptr for right sector
                    malloc((number_sectors) * (type_size[tagSHORT]))) == NULL)
                    {
                    printf("Insufficient memory");
                    exit(1);
                    }
  
                    for (k=0; k<number_sectors; k++) // read left sector vals for char
  
                    temp1[k] = getshort(fp);
  
                    for (k=0; k<number_sectors; k++) // read right sector vals for char
  
                    temp2[k] = getshort(fp);
  
                    // assign the pointers to the structure pointers
                    TFM->typeface->kerning.sectorKernChar[j].rightSector = (WORD *)temp1;
                    TFM->typeface->kerning.sectorKernChar[j].leftSector = (WORD *)temp2;
                    }
                    break;
                    */
  
  
                    /*
                    case tagTRACKKERN:
  
                    // allocate memory for track kerning
                    if ((TFM->typeface->kerning.trackKern = (struct trackKernType *)malloc(length)) == NULL)
                    {
                    printf("Insufficient memory");
                    exit(1);
                    }
  
                    number_tracks = getshort(fp);   // get the # of defined tracks
  
                    // assign the number of track to the structure variable
                    TFM->typeface->kerning.numberTracks = number_tracks;
  
  
                    for (j=0; j<number_tracks; j++) // read track kerning info for each defined track
  
                    {
                    // read track value
                    TFM->typeface->kerning.trackKern[j].trackValue = getshort(fp);
  
                    // read maximum point size
                    TFM->typeface->kerning.trackKern[j].maxPointSize = getshort(fp);
  
                    // read minimum point size
                    TFM->typeface->kerning.trackKern[j].minPointSize = getshort(fp);
  
                    // read maximum kerning amount
                    TFM->typeface->kerning.trackKern[j].maxKern = getshort(fp);
  
                    // read minimum kerning amount
                    TFM->typeface->kerning.trackKern[j].minKern = getshort(fp);
                    }
  
                    break;
                    */
  
  
                case tagTFSELECTSTR:
  
                    /* read typeface selection string */
                    get_string(fp, (LPSTR)TFM->typeface->general.typefaceSelectionString);
  
                    /* if length is less than or equal to 4, the data is not offset */
                    /* therefore, after the data is read, bytes must be skipped */
                    if (length <= 4)
                    {
                        for(i=block_size; i < 4; i++)
                            getbyte(fp);     /* read unused bytes */
                    }
  
                    break;
  
  
                default:      /* used when the tag is not found within the case statements */
  
                    getlong(fp);   /* advance position in TFM file */
  
                    break;
  
  
            }  /* end switch */
  
            if (offset)     /* if the data was offset, reset the position */
                bufpos = pos;
            //       _llseek(fp, pos, 0);
  
        }  /* end for tags */
  
    }  /* end for number of typefaces */
  
    GlobalFree(hFile);
    GlobalUnlock(hT);
  
    DBGentry(("tfmread: exit\n"));
  
  
} /* end main */
  
/*****************************************************************************
Routine Title:  get_version
Author:  dtk, 8/4/89
Summary:  reads the version number of the TFM file and checks
to see if the high bit is set.  If it is, it appends
a leading minus sign on the version number.
  
Inputs:
*TFM - pointer to TFM structure
*fp  - file pointer to input file
  
Outputs:  none
  
Modifications:
7-Aug-89  dtk  Comments added
4-Aug-89  dtk  Original
  
****************************************************************************/
/*
void get_version(struct TFMType *TFM, int fp)
  
  
{
BYTE majorv,minorv;
LONG next;
  
DBGentry(("get_version: enter\n"));
  
majorv = getbyte(fp); // get version numbers
minorv = getbyte(fp);
  
if ((majorv & 128) ==  128)    // if high bit is set, TFM is in development
TFM->version[0] = '-';     // so append '-' to version number
else
TFM->version[0] = ' ';
  
// convert version number to ASCII
TFM->version[1] = (majorv & 127) + '0';
TFM->version[2] = '.';
itoa(minorv, (LPSTR)&TFM->version[3]), 10);
  
DBGentry(("get_version: exit\n"));
  
}
*/
  
/****************************************************************************/
  
/*****************************************************************************
Routine Title:  get_number_typefaces
Author:  dtk, 8/4/89
Summary:  reads through the the TFM file in order to determine the
number of typefaces contained in the file.
  
Inputs:
*TFM - pointer to TFM structure
*fp  - file pointer to input file
  
Outputs:  none
  
Modifications:
8-Aug-89  dtk  Comments added
4-Aug-89  dtk  Original
  
****************************************************************************/
  
void get_number_typefaces(struct TFMType far *TFM, int fp)
  
{
    LONG
    dir_offset,                  /* offset to first directory */
    typeface_offset_location,    /* offset to additional directories */
    next_typeface;               /* value at the offset reigon */
  
    WORD num_tags;               /* number of tag entries in a typeface */
    long first_dir;              /* temporary position variables */
    int done=FALSE;              /* end of file Flag */
  
    DBGentry(("get_number_typefaces: enter\n"));
  
    first_dir = bufpos;
    //    first_dir = _llseek(fp, 0L, 1); /* save current position */
    dir_offset = getlong(fp);       /* get first directory offset */
    bufpos = dir_offset;
    //    _llseek(fp, dir_offset, 0);     /* seek to offset position */
  
    Num_typefaces = 1;
  
    DBGtfm(("get_number_typefaces: first_dir  = %d\n", first_dir));
    DBGtfm(("get_number_typefaces: dir_offset = %d\n", dir_offset));
  
    while (!done)   /* while there are additional directories */
    {
        num_tags = getshort(fp);    /* get # of tag entries in typeface */
  
        DBGtfm(("get_number_typefaces: num_tags   = %d\n", num_tags));
  
        /* calculate directory offset location from begining of file */
        typeface_offset_location = ((num_tags * 12) + dir_offset);
  
        DBGtfm(("get_number_typefaces: typeface_offset_location = %d\n",
        typeface_offset_location));
  
        bufpos = typeface_offset_location;
        //       _llseek(fp,typeface_offset_location,0);  /* seek to offset locn */
  
        next_typeface = getlong(fp);   /* get next typeface offset */
  
        DBGtfm(("get_number_typefaces: next_typeface = %d\n", next_typeface));
  
        if (next_typeface)  /* if the offset does not equal zero */
        {
            next_typeface = bufpos;
            //          _llseek(fp, next_typeface, 0);  /* seek to the offset position */
            Num_typefaces++;    /* increment the number of typefaces */
        }
        else                  /* if offset = 0, there are no more typefaces */
            done = TRUE;
    }
  
    bufpos = first_dir;
    //    _llseek(fp, first_dir, 0); /* reset the position */
    /* assign the number of typefaces to the structure variable */
    TFM->numberTypefaces = Num_typefaces;
  
    DBGtfm(("get_number_typefaces: Num_typefaces = %d\n", Num_typefaces));
  
    DBGentry(("get_number_typefaces: exit\n"));
}
  
  
/****************************************************************************/
  
/****************************************************************************
Routine Title:  getbyte
Summary:  reads a byte from the input file.
  
Inputs:  *fp - pointer to input file.
  
Outputs:  one bytes.
  
Modifications:
  
*****************************************************************************/
BYTE getbyte(int fp)
  
{
    BYTE  b1;      /* temporary variable to store a word */
  
    lpFile = (BYTE huge *)GlobalLock(hFile);
  
    b1 = lpFile[bufpos++];
    //    _lread(fp, (LPSTR)&b1, 1);   /* read the byte */
  
    GlobalUnlock(hFile);
  
    return(b1);
}
/****************************************************************************/
  
/****************************************************************************
Routine Title:  getshort
Author:  dtk, 8/4/89
Summary:  reads a short (two bytes) from the input file.
  
Inputs:  *fp - pointer to input file.
  
Outputs:  one short (two bytes).
  
Modifications:
8-Aug-89  dtk  Comments added
4-Aug-89  dtk  Original
  
*****************************************************************************/
short getshort(int fp)
  
{
    WORD  w1;      /* temporary variable to store a word */
  
    w1 = (WORD)getbyte(fp);
    //    _lread(fp, (LPSTR)&w1, 2);   /* read the word */
  
    if (Intel_byte_order)   /* check for byte order */
        return((short)(w1 | ((getbyte(fp)) << 8)));
    else
        return((short)((w1 << 8) | getbyte(fp)));
  
    //    if (Intel_byte_order)   /* check for byte order */
    //       return((short)w1);
    //    else
    //    return((short)((w1<<8) | (w1>>8)));
}
/****************************************************************************/
  
/****************************************************************************
Routine Title:  getlong
Author:  dtk, 8/4/89
Summary:  reads a long (four bytes) from the input file.
  
Inputs:   *fp - pointer to input file.
  
Outputs:  one long (four bytes).
  
Modifications:   26-Oct-90 klo  Changed bit-shift to work with MSC 6.0
8-Aug-89  dtk  Comments added
4-Aug-89  dtk  Original
  
*****************************************************************************/
long getlong(int fp)
  
{
    WORD s1, s2;
    DWORD dw=0L;
  
    s1 = (WORD)getshort(fp);
    s2 = (WORD)getshort(fp);
  
    if (Intel_byte_order)   /* check for byte order */
    {
        dw = lmul((DWORD)s2, 0x10000);  /* Left shift 16 */
        dw |= (DWORD) s1;
    }
    else
    {
        dw = lmul((DWORD)s1, 0x10000);  /* Left shift 16 */
        dw |= (DWORD) s2;
    }
  
    return((long)dw);
}
  
/****************************************************************************/
  
/****************************************************************************
Routine Title:  getrational
Author:  dtk, 8/4/89
Summary:  reads two longs (four bytes each) from the input file,
divides them and returns the result.
  
Inputs:   *fp - pointer to input file.
  
Outputs:  double (double precision floating point).
  
Modifications:
8-Aug-89  dtk  Comments added
4-Aug-89  dtk  Original
  
*****************************************************************************/
//double getrational(int fp)
//
//   {
//
//  long numerator,denominator;  /* temp vars to store the longs */
//
//  numerator = getlong(fp);    /* read the numerator   */
//  denominator = getlong(fp);  /* read the denominator */
//
//  return((double)(numerator / denominator));
//   }
  
/****************************************************************************/
  
/****************************************************************************
Routine Title:  get_string
Author:  dtk, 8/4/89
Summary:  reads an ASCII string from the input file.
  
Inputs:   *fp - pointer to input file.
  
Outputs:  *string - pointer to an ASCII string.
  
Modifications:
8-Aug-89  dtk  Comments added
4-Aug-89  dtk  Original
  
*****************************************************************************/
void get_string(int fp, LPSTR string)
  
{
    int i = 0;
  
    lpFile = (BYTE huge *)GlobalLock(hFile);
  
    /* read in characters until a zero is reached */
    //  while ((_lread(fp, (LPSTR)&string[i], 1) > 0) && (string[i++]))
    while ((string[i] = lpFile[bufpos++]) && (string[i++]))
        ;
  
    GlobalUnlock(hFile);
}
  
/***************************************************************************
  
Routine title: FreeTFM
  
Summary:  Frees TFM struct memory
  
Inputs: Handle to TFM
  
Modifications:
  
***************************************************************************/
  
void FAR PASCAL FreeTFM(HANDLE hT)
  
{
    struct TFMType far *TFM;    /* pointer to TFM structure */
  
    if (TFM = (struct TFMType far *)GlobalLock(hT))
    {
        if (TFM->hType)
        {
            DBGtfm(("tfmread: Free up TFM struct fields\n"));
  
            while (TFM->numberTypefaces)
            {
                DBGtfm(("tfmread: TFM->numberTypefaces = %d\n",
                TFM->numberTypefaces));
  
                while (TFM->typeface->general.numberSymbolSets)
                {
                    DBGtfm(("tfmread: TFM->typeface->general.numberSymbolSets = %d\n",
                    TFM->typeface->general.numberSymbolSets));
                    DBGtfm(("tfmread: TFM...hSI = %d\n",
                    TFM->typeface[TFM->numberTypefaces-1].symbol.symbolSetDirectory[TFM->typeface->general.numberSymbolSets-1].hSI));
  
                    GlobalUnlock(
                    TFM->typeface[TFM->numberTypefaces-1].symbol.symbolSetDirectory[TFM->typeface->general.numberSymbolSets-1].hSI);
                    GlobalFree(
                    TFM->typeface[TFM->numberTypefaces-1].symbol.symbolSetDirectory[TFM->typeface->general.numberSymbolSets-1].hSI);
  
                    DBGtfm(("tfmread: Freed TFM...hSI\n"));
  
                    TFM->typeface->general.numberSymbolSets--;
                }
  
                /* Currently don't need MSL #'s
                GlobalUnlock(TFM->typeface[TFM->numberTypefaces-1].symbol.hSymMap);
                GlobalFree(TFM->typeface[TFM->numberTypefaces-1].symbol.hSymMap);
                */
  
                GlobalUnlock(TFM->typeface[TFM->numberTypefaces-1].symbol.hSymDir);
                GlobalFree(TFM->typeface[TFM->numberTypefaces-1].symbol.hSymDir);
  
                GlobalUnlock(TFM->typeface[TFM->numberTypefaces-1].characterMetrics.hChar);
                GlobalFree(TFM->typeface[TFM->numberTypefaces-1].characterMetrics.hChar);
  
#ifdef KERN
                GlobalUnlock(TFM->typeface[TFM->numberTypefaces-1].kerning.hKern);
                GlobalFree(TFM->typeface[TFM->numberTypefaces-1].kerning.hKern);
#endif
  
                TFM->numberTypefaces--;
            }
  
            DBGtfm(("tfmread: Free up TFM->hType\n"));
  
            GlobalUnlock(TFM->hType);
            GlobalFree(TFM->hType);
            TFM->hType = 0;
        }
    }
}
