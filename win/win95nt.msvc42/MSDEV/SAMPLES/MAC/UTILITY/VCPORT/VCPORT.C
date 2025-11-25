char *helpScr[] = {
"\nVCPort [/c] [/g] [/i<file>] [/k<file>] [/s<file>] [/v] [/w<number>] <srcfile>", //  0
"\n",                                                                              //  1
"\n/c         - Searches case-sensitively (the default is to ignore case).",       //  2
"\n/g         - Groups together all instances found of each given keyword.",       //  3
"\n/i <file>  - Specifies an .IKW prebuilt index file (only one at a time).",      //  4
"\n/k <file>  - Specifies a .KWD text file containing a list of keywords.",        //  5
"\n/s <file>  - Specifies a filename in which to save the index created.",         //  6
"\n/v         - (Verbose) reports the files used to build the keyword index.",     //  7
"\n/w<level>  - Establishes a 'warning level' for porting issues reported.",       //  8
"\n<srcfile>  - A source file to be scanned (wildcards are recognized).",          //  9
"\n",                                                                              // 10
"\n -> See VCPort.WRI (a Windows Write file) for more complete documentation.",    // 11
"\n -> Several keyword text files can be specified, each preceded by /k, and/or",  // 12
"\n    one index file. If index or keyword files are not found in the current",    // 13
"\n    directory, then VCPort looks for them in its own directory.",               // 14
"\n -> You can also specify multiple source files (up to 256). Sourcefiles are",   // 15
"\n    sought in the current directory, and can include wildcards ( * and ? ).",   // 16
"\n -> If you will use a given index several times, it is faster to save it",      // 17
"\n    using /s and reload it using /i rather than rebuild it each time.",         // 18
"\n -> The /w flag allows you to filter out keywords based on the severity of",    // 19
"\n    the porting issue that they present. See VCPort.WRI for more details.",     // 20
"\n               -----------------------------------------------",                // 21
"\n                 Copyright (C) 1995 by Microsoft Corporation",                  // 22
};
#define HELP_LINES 23

//   *************************************************************************
//   *              Copyright (c) 1995 by Microsoft Corporation              *
//   *************************************************************************
//   * BRIEF HISTORY: This tool evolved as follows:                          *
//   *                                                                       *
//   *       -  It was inspired by the Windows NT porting tool.              *
//   *       -  Julian Jiggins first proposed it early in 1992.              *
//   *       -  Harold Henry designed it.                                    *
//   *       -  Sean Shapira made suggestions refining its design.           *
//   *       -  N. Shiva Shivakuma wrote the first version of it.            *
//   *       -  Harold Henry wrote a new 16-bit DOS version.                 *
//   *       -  Harold Henry rewrote the DOS version for 32 bits.            *
//   *       -  Rick Powell suggested wildcards, /w, and other improvements. *
//   *       -  Craig Zhou and others suggested many helpful refinements.    *
//   *                                                                       *
//   *************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <io.h>

typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned int    uInt;

#define LOTRIBYTE(x)    (((uInt)(x)) & 0xffffff)
#define TOPBYTE(x)      ((BYTE)*(((BYTE *)(&x))+3))

// #define DEBUG

#define VCPORT_VER    0x103    // Oldest compatible version of the program

#define FILE_STEP        10    // Number of file structures to allocate at a time
#define KEY_MAX_LEN     128    // Maximum length of a keyword
#define LINE_MAX_LEN   4096    // Maximum length of a text-file line ( = 4KB)
#define GROUP_START   10000    // Number of line structures to start with
#define GROUP_STEP     5000    // Number of line structures to add at a time
#define WORD_MAX      65000    // Cut-off size for an unsigned word value
#define HD_STR_LEN       32    // Length of the header string for an index file
#define MAX_PATH        260    // Maximum length of an NT path

#define VERBOSE        0x01    // Report the files used to build the index
#define LOAD_INDEX     0x02    // Load an .IKW index file
#define SAVE_INDEX     0x04    // Save index as an .IKW file
#define GROUPED        0x08    // Group lines together by keyword
#define CASE_SENSE     0x10    // Search case-sensitively for keywords
#define ONLY_INDEX     0xfb    // Do NOT load text files or save index

#define KEYWORD_FILE      0    // Keyword file fType value
#define SOURCE_FILE       1    // Source file fType value

char *impLevel[] = 
{
  "[Unknown implementation level]",     // 0
  "Portable Implementation",            // 1
  "Modified Implementation",            // 2
  "Stub Implementation",                // 3
  "Not Implemented",                    // 4
  "New Extension (Macintosh-Specific)", // 5
  "16-bit to 32-bit Windows change"     // 6
};

char VCPortVer[] =     { "1.04" };
char defIkwName[] =    { "VCPort.ikw" };
char kwdExtension[] =  { ".kwd" };
char ikwExtension[] =  { ".ikw" };

char maxWordMsg[] =    { "Only 65,000 allowed" };
char noMemMsg[] =      { "Insufficient memory" };
char forKWMsg[] =      { "to allocate keyword structures" };
char forKSMsg[] =      { "to allocate keyword string space" };
char forFnmMsg[] =     { "to allocate file names" };
char groupedMsg[] =    { "can't group keywords (try sequential output)" };
char openMsg[] =       { "Could not open file" };
char closeMsg[] =      { "Could not close file" };
char writeMsg[] =      { "Could not write to file" };
char readMsg[] =       { "Could not read from file" };
char NoKwdMsg[] =      { "No keywords found in keyword text files" };
char ovrKeywordMsg[] = { "Too many keywords for one index" };
char ovrFindMsg[] =    { "Found too many lines to group (try sequential output)" };
char ovrKeyLenMsg[] =  { "Too long a keyword (128 characters Max)" };
char noKWfNameMsg[] =  { "No keyword text-file name given after final /k flag" };
char twoIFlagsMsg[] =  { "You can only specify one prebuilt index file with /i" };
char badIndexMsg[] =   { "This does not seem to be a VCPort index file" };
char oldIndexMsg[] =   { "Incompatible index file (built by an older version)" };
char twoSFlagsMsg[] =  { "You can only save the index to one /s file" };
char noSavFMsg[] =     { "No prebuilt index file name given after /s flag" };
char againMsg[] =      { "try again" };
char NoProcMsg[] =     { "Could not determine .EXE file location: " };
char NoFindMsg[] =     { "\nVCPort found no instances of keywords...\n" }; 
char NoSrcFileMsg[] =  { "\nNo source files supplied to scan for keywords...\n" };
char fatalErrMsg[] =   { "\nFATAL ERROR - %s: %s !" };
char warningMsg[] =    { "\nWARNING - %s: %s!" };
char buildIndMsg[] =   { "\nVCPort version 1.04: now building the keyword index:" };
char badFNmMsg[] =     { "No such file(s) as" };
char noWildCardMsg[] = { "Wildcards not allowed in a keyword or index filename" };
char loadIkwMsg[] =    { "\n...loading the prebuilt index file: %s..." };
char saveIkwMsg[] =    { "\n...now saving this index as: %s..." };
char loadKwdMsg[] =    { "\n...loading the keyword text file: %s..." };
char scanSrcMsg[] =    { "\nNow scanning source files..." };
char loadSrcMsg[] =    { "\n...loading the source code file: %s..." };

#pragma pack(1)
struct lineStruct      // Should be 6 bytes long
{
  uInt
    lnSpec;            // High byte = file; Low 3 bytes = line number
  WORD
    nextL;             // Index of next line structure (64K maximum)
} *lineArray;

struct keyStruct       // 16 bytes long
{
  WORD
    testBit,    //  0
    Fchild,     //  2
    Tchild,     //  4
    headLn,     //  6
    tailLn;     //  8
  uInt
    kStr;       // 10
  BYTE
    len,        // 14
    imp;        // 15
} *keyArray;

struct saveIndexHeaderStruct
{
  BYTE hdrStr[HD_STR_LEN];
  uInt 
    numNodes,
    strSpace;
} outIhdr = { "VCPort 1.04 binary Index file:", 0, 0 },
  inIhdr;
#pragma pack()

  struct _finddata_t
    fileData;

FILE
  *fHndl;

uInt
  mode,
  nextKey,
  nextLine,
  nextKStr,
  maxSrcFiles,
  numSrcFiles,
  maxKwdFiles,
  numKwdFiles,
  numKeywords,
  numStringBytes,
  maxLines,
  numLines,
  lnNum,
  src,
  foundNone;

char
  *vcPath,
  *ifName,
  *sfName,
  **fNames,
  **kNames;

BYTE
  *keyStrings,
  alphaTable[258],
  lBuf[LINE_MAX_LEN + 2],
  kBuf[LINE_MAX_LEN + 2],
  fBuf[MAX_PATH + 2],
  showImp[10];

// **************************************************************************
// * cpystr                  Copies a string and returns the final position *
// **************************************************************************
BYTE *cpystr( register BYTE *destP, register BYTE *srcP )
{
  while ( *srcP )
    *destP++ = *srcP++;
  *destP = 0;
  return( destP );
}

// **************************************************************************
// * buildAlphaTable         Builds the character-mapping table.            *
// **************************************************************************
void buildAlphaTable( void )
{
  register uInt i;

  memset( alphaTable, 0, 256 );

  alphaTable[0]  = '$';
  alphaTable[10] = '$';
  alphaTable[13] = '$';

  for ( i = '0'; i <= '9'; i++ )
    alphaTable[i] = (BYTE) i;

  for ( i = 'A'; i <= 'Z'; i++ )
    alphaTable[i] = (BYTE) i;

  for ( i = 'a'; i <= 'z'; i++ )
    alphaTable[i] = (BYTE) ( i - 32 );

  alphaTable['_'] = '_';
}

#ifdef DEBUG
// **************************************************************************
// * displayIndex            Recursively dumps the keyword index to stdout. *
// *                         Because this routine uses IBM-PC extended      *
// * graphics characters to draw lines, its output should be displayed in   *
// * the console rather than in a Window that uses the ANSI character set.  *
// **************************************************************************
void displayIndex( uInt key, uInt level )
{
  register uInt i;
  register BYTE d;

  d = TOPBYTE( level );
  i = LOTRIBYTE( level );

  if ( i == 0 )
  {
    fprintf( stdout, "%c %s (node %d, at [%d] & 0x%02x)\n",
             ( d & 0x7f ), keyStrings + keyArray[key].kStr, key, 
             ( keyArray[key].testBit ) >> 8, (BYTE)( keyArray[key].testBit ) );
    lBuf[0] = ' ';
    level = 1;
  }
  else
  {
    lBuf[i] = 0;
    fprintf( stdout, "%s%c %c %s%s (node %d, at [%d] & 0x%02x)\n", lBuf, 
             ( ( ( d & 0x7f ) == '-' ) ? (BYTE) 195 : (BYTE) 192 ),
             ( d & 0x7f ), ( ( d & 0x80 ) ? "*" : "" ), keyStrings + keyArray[key].kStr, 
             key, ( keyArray[key].testBit ) >> 8, (BYTE)( keyArray[key].testBit ) );
    lBuf[i++] = ( ( d & 0x7f ) == '+' ) ? ' ' : (BYTE) 179;
    lBuf[i++] = ' ';
    level = i;
  }

  if ( !( d & 0x80 ) )
  {
    i = keyArray[key].Fchild;
    TOPBYTE( level ) = ( keyArray[i].testBit > keyArray[key].testBit ) ? 
                        (BYTE) '-' : ( '-' | (BYTE) 0x80 );
    displayIndex( i, level );

    i = keyArray[key].Tchild;
    TOPBYTE( level ) = ( keyArray[i].testBit > keyArray[key].testBit ) ? 
                        (BYTE) '+' : ( '+' | (BYTE) 0x80 );
    displayIndex( i, level );
  }

  if ( LOTRIBYTE( level ) == 1 )
    fprintf( stdout, "\n" );
}
#endif

// **************************************************************************
// * fatalErr                Displays an error message and exits.           *
// **************************************************************************
void fatalErr( char *str1, char *str2 )
{
  fprintf( stdout, fatalErrMsg, str1, str2 );
  exit ( 1 );
}

// **************************************************************************
// * showHelp                Displays the help screen (on stdout)           *
// **************************************************************************
void showHelp ( )
{
  register int i;

  for ( i = 0; i < HELP_LINES; i++ )
    fprintf( stdout, helpScr[i] );
  exit ( 0 );
}

// **************************************************************************
// * checkSrcNum             Checks for a free source-file name pointer.    *
// **************************************************************************
void checkSrcNum( void )
{
  if ( numSrcFiles >= maxSrcFiles )
  {
    maxSrcFiles += FILE_STEP;
    if ( ( fNames = realloc( fNames, maxSrcFiles * sizeof( BYTE * ) ) ) == NULL )
      fatalErr( noMemMsg, forFnmMsg );
  }
}

// **************************************************************************
// * parseSourceName         Handles a source file name argument.           *
// **************************************************************************
void parseSourceName( char *fileName )
{
  register BYTE *ptr;
  long srchHndl;

  ptr = fileName;
  while ( *ptr && ( *ptr != '*' ) && ( *ptr != '?' ) )
    ptr++;

  if ( *ptr )               // The filename contains a wildcard 
  {
    srchHndl = _findfirst( fileName, &fileData );
    if ( srchHndl == -1 )
      fprintf( stdout, badFNmMsg, fileName );
    else
    {
      do
      {
        if ( !( fileData.attrib & ( _A_SUBDIR | _A_SYSTEM ) ) )
        {
          checkSrcNum( );
          if ( ( ptr = strdup( fileData.name ) ) == NULL )
            fatalErr( noMemMsg, forFnmMsg );
          fNames[numSrcFiles++] = ptr;
        }
      }
      while ( _findnext( srchHndl, &fileData ) == 0 );
      _findclose( srchHndl );
    }
  }
  else                      // No wildcards
  {
    checkSrcNum( );
    fNames[numSrcFiles++] = fileName;
  }
}

// **************************************************************************
// * checkKwFNum             Checks for a free keyword-file name pointer.   *
// **************************************************************************
void checkKwFNum( void )
{
  if ( numKwdFiles >= maxKwdFiles )
  {
    maxKwdFiles += FILE_STEP;
    if ( ( kNames = realloc( kNames, maxKwdFiles * sizeof( BYTE * ) ) ) == NULL )
      fatalErr( noMemMsg, forFnmMsg );
  }
}

// **************************************************************************
// * parseKeyFiles           Handles a keyword file name argument.          *
// **************************************************************************
char *parseKeyFiles( char *fileName, char *extStr )
{
  register BYTE *ptr;
  BYTE *fNm;

  ptr = fileName;           // Check to make sure there are no wildcards
  while ( *ptr && ( *ptr != '*' ) && ( *ptr != '?' ) )
    ptr++;
  if ( *ptr-- )
    fatalErr( noWildCardMsg, fileName );

  // If there is no extension, add the default extension
  while ( ( ptr > fileName ) && ( *ptr != '.' ) && ( *ptr != 92 ) )
    ptr--;
  if ( ( ptr > fileName ) && ( *ptr == '.' ) )
    fNm = fileName;
  else
  {
    ptr = cpystr( fBuf, fileName );
    cpystr( ptr, extStr );
    fNm = fBuf;
  }

  // Look first in the current directory, then in the VCPort directory
  if ( _access( fNm, 4 ) )   // If the file is unreadable
  {
    ptr = lBuf;
    if ( vcPath != NULL )
      ptr = cpystr( ptr, vcPath );
    cpystr( ptr, fNm );
    if ( _access( lBuf, 4 ) )     // If the file is unreadable
      fatalErr( readMsg, fNm );
    if ( ( ptr = strdup( lBuf ) ) == NULL )
      fatalErr( noMemMsg, forFnmMsg );
  }
  else
  {
    if ( fNm == fBuf )
    {
      if ( ( ptr = strdup( fBuf ) ) == NULL )
        fatalErr( noMemMsg, forFnmMsg );
    }
    else
      ptr = fileName;
  }
  return( ptr );
}

// **************************************************************************
// * difbits                 Compares a string to one of the key strings    *
// *                         and returns an unsigned int of which the       *
// * lowest byte is the bit mask of the first differing bit, and the next   *
// * higher byte indicates the byte where that bit is located.              *
// **************************************************************************
uInt difbits( BYTE *str, uInt key )
{
  register BYTE
    *ptr1,
    *ptr2,
    bitMask,
    byteA,
    byteB;

  ptr1 = str;
  ptr2 = keyStrings + keyArray[key].kStr;
  while ( *ptr1 && ( *ptr1 == alphaTable[*ptr2] ) )
  {
    ptr1++;
    ptr2++;
  }
  if ( *ptr1 == *ptr2 )      // If the strings are identical...
    return( 0 );

  byteA = *ptr1;
  byteB = alphaTable[*ptr2];

  bitMask = 1;
  while ( ( byteA & bitMask ) == ( byteB & bitMask ) )
    bitMask = bitMask << 1;  // We already know that at least one bit differs!
  return( ( (uInt)( ptr1 - str ) << 8 ) | (uInt) bitMask );
}

// **************************************************************************
// * nodeSearch              Traverses the index on a given string, and     *
// *                         returns a pointer to the node in which the     *
// * string will be located, if it exists in the index.                     *
// **************************************************************************
uInt nodeSearch( BYTE *ptr, uInt len )
{
  register uInt
    Target,
    tBit;
  register BYTE
    bitM;
  uInt
    prevTB;

  Target = keyArray[0].Fchild;
  tBit = keyArray[Target].testBit;
  do
  {
    prevTB = tBit;
    bitM = (BYTE) ( tBit & 0xff );
    tBit = tBit >> 8;
    if ( tBit > len )
      break;
    if ( ptr[tBit] & bitM )
      Target = keyArray[Target].Tchild;
    else
      Target = keyArray[Target].Fchild;
    tBit = keyArray[Target].testBit;
  } 
  while ( tBit > prevTB );

  return( Target );
}

// **************************************************************************
// * copyKeyWord             Examines one line from a keyword text file in  *
// *                         the line buffer.  If the line contains a key-  *
// * word, copies it into the key string buffer and updates the key node.   *
// **************************************************************************
BYTE *copyKeyWord( void )
{
  register BYTE
    al,
    *ptr1,
    *ptr2;
  BYTE
    *start;

  ptr1 = lBuf;
  ptr2 = keyStrings + nextKStr;

  while ( isspace( *ptr1 ) )
    ptr1++;
  if ( ( al = alphaTable[*ptr1] ) < 'A' )   // If the character is punctuation
    return ( 0 );

  start = ptr1;
  do
  {
    *ptr2++ = *ptr1;
    *ptr1++ = al;
  }
  while ( ( al = alphaTable[*ptr1] ) > '$' );
  keyArray[nextKey].len = (BYTE) ( ptr1 - start );

  *ptr2++ = 0;           // Terminate keystring and advance to next start

  if ( *ptr1 )           // Make sure that the keyword string is terminated.
  {
    *ptr1++ = 0;
    while ( *ptr1 && !isdigit( *ptr1 ) ) // Get the implementation level, if any
      ptr1++;
    if ( *ptr1 && ( *ptr1 < '7' ) )
      keyArray[nextKey].imp = *ptr1 - '0';
  }
  keyArray[nextKey].kStr = nextKStr;
  nextKStr = (uInt) ( ptr2 - keyStrings );
  return( start );
}

// **************************************************************************
// * iniLine                 Processes one line from a keyword text file in *
// *                         the line buffer.  If the line contains a key-  *
// * word, adds it to the keyword index (creates a new node).               *
// **************************************************************************
void iniLine( void )
{
  register uInt Below;
  register uInt Above;
  register uInt tBit;
  register BYTE bitM;
  uInt i, j;
  BYTE *ptr;

//      **************************************************
//      * If there is no keystring on this line, then go *
//      * on; otherwise copy it into the next position.  *
//      **************************************************
  if ( ( ptr = copyKeyWord( ) ) == NULL )
    return;

//      **************************************************
//      * If the string is a duplicate, use its implevel *
//      * but reinitialize the nextKey string pointer    *
//      * and implevel, and exit.                        *
//      **************************************************
  j = nodeSearch( ptr, (uInt) keyArray[nextKey].len );
  if ( !( i = difbits( ptr, j ) ) )
  {
    keyArray[j].imp = keyArray[nextKey].imp;
    nextKStr = keyArray[nextKey].kStr;
    keyArray[nextKey].imp = 0;
    keyArray[nextKey].len = 0;
    return;
  }

//      **************************************************
//      * Locate the correct position of the new keyword *
//      * in the index and insert its node.              *
//      **************************************************
  keyArray[nextKey].testBit = i;
  Below = 0;
  tBit = (uInt) keyArray[0].testBit;

  do
  {
    Above = Below;
    j = tBit;
    bitM = (BYTE) tBit;
    tBit = tBit >> 8;
    if ( ptr[tBit] & bitM )
      Below = (uInt) keyArray[Below].Tchild;
    else
      Below = (uInt) keyArray[Below].Fchild;
    tBit = (uInt) keyArray[Below].testBit;
  }
  while ( ( i >= tBit ) && ( tBit > j ) );

  tBit = i >> 8;
  bitM = (BYTE) i;
  if ( ptr[tBit] & bitM )
  {
    keyArray[nextKey].Tchild = (WORD) nextKey;
    keyArray[nextKey].Fchild = (WORD) Below;
  }
  else
  {
    keyArray[nextKey].Tchild = (WORD) Below;
    keyArray[nextKey].Fchild = (WORD) nextKey;
  }

  tBit = keyArray[Above].testBit;
  bitM = (BYTE) tBit;
  tBit = tBit >> 8;
  if ( ptr[tBit] & bitM )
    keyArray[Above].Tchild = (WORD) nextKey;
  else
    keyArray[Above].Fchild = (WORD) nextKey;

  nextKey++;
}

// **************************************************************************
// * foundKeyWord            When a keyword is found, takes appropriate     *
// *                         action to record the find.                     *
// **************************************************************************
void foundKeyWord( register uInt fnode )
{
  foundNone = 0;
  if ( mode & GROUPED )
  {
    if ( keyArray[fnode].headLn )
      lineArray[keyArray[fnode].tailLn].nextL = (WORD) nextLine;
    else
      keyArray[fnode].headLn = (WORD) nextLine;
    lineArray[nextLine].lnSpec = ( lnNum | ( src << 24 ));
    keyArray[fnode].tailLn = nextLine++;
    if ( ++numLines >= maxLines )
    {
      if ( maxLines >= WORD_MAX )
        fatalErr( ovrFindMsg, maxWordMsg );
      maxLines += GROUP_STEP;
      if ( ( lineArray = (struct lineStruct *) realloc( lineArray, 
             maxLines * sizeof( struct lineStruct) ) ) == NULL )
        fatalErr( noMemMsg, groupedMsg );
    }
  }
  else
    fprintf( stdout, "%s(%d) : %s: %s\n", fNames[src], lnNum, 
         keyStrings + keyArray[fnode].kStr, impLevel[keyArray[fnode].imp] );
}

// **************************************************************************
// * scanFile                Scans a source file for keywords.              *
// **************************************************************************
void scanFile( void )
{
  register BYTE
    al,
    *ptr1,
    *ptr2;
  BYTE
    *str;
  uInt
    fnode;

  if ( mode & VERBOSE )
    fprintf( stdout, loadSrcMsg, fNames[src] );

  if ( ( fHndl = fopen( fNames[src], "r")) == NULL )
    fatalErr( openMsg, fNames[src] );

  lnNum = 0;

  if ( mode & CASE_SENSE )
  {
    while ( !feof( fHndl) && ( fgets( lBuf, LINE_MAX_LEN, fHndl ) != NULL ) )
    {
      lnNum++;
      ptr1 = lBuf;
      do
      {
        while ( alphaTable[*ptr1] < '$' )  // While char IS a separator
          ptr1++;
        while ( *ptr1 == '_' )             // Ignore leading underscores
          ptr1++;
        if ( !( *ptr1 ) )
          break;
        str = ptr1;
        ptr2 = kBuf;
        while ( ( al = alphaTable[*ptr1] ) > '$' ) // While NOT a separator
        {
          ptr1++;
          *ptr2++ = al;
        }
        *ptr1++ = 0;                               // Terminate the 'word'
        *ptr2 = 0;
        fnode = nodeSearch( kBuf, (uInt) ( ptr2 - kBuf ) );
        if ( ( showImp[keyArray[fnode].imp] ) && 
             ( strcmp( str, keyStrings + keyArray[fnode].kStr ) == 0 ) )
          foundKeyWord ( fnode );
      } while ( al != '$' );
    }
  }
  else
  {
    while ( !feof( fHndl) && ( fgets( lBuf, LINE_MAX_LEN, fHndl ) != NULL ) )
    {
      lnNum++;
      ptr1 = lBuf;
      do
      {
        while ( alphaTable[*ptr1] < '$' )  // While char IS a separator
          ptr1++;
        while ( *ptr1 == '_' )             // Ignore leading underscores
          ptr1++;
        if ( !( *ptr1 ) )
          break;
        ptr2 = ptr1;
        while ( ( al = alphaTable[*ptr1] ) > '$' )  // While NOT a separator
          *ptr1++ = al;
        *ptr1 = 0;                                  // Terminate the 'word'
        fnode = nodeSearch( ptr2, (uInt) ( ptr1++ - ptr2 ) );
        if ( showImp[keyArray[fnode].imp] )
        {
          str = ptr1;
          ptr1 = keyStrings + keyArray[fnode].kStr;
          while ( *ptr2 == alphaTable[*ptr1] )
          {
            ptr1++;
            ptr2++;
          }
          if ( *ptr1 == *ptr2 )        // This is a keyword match!
            foundKeyWord ( fnode );
          ptr1 = str;
        }
      } while ( al != '$' );
    }
  }
  if ( fclose ( fHndl ) )
    fatalErr( closeMsg, fNames[src] );
}

// **************************************************************************
// * countSize               Looks for a keyword in the line buffer, and if *
// *                         it finds one, increments the keyword count and *
// * adds the keyword length (including null) to the string space counter.  *
// **************************************************************************
void countSize( void )
{
  register BYTE *ptr;
  register WORD i;
  BYTE *start;

  ptr = lBuf;
  i = 1;
  while ( isspace( *ptr ) )
    ptr++;
  if ( alphaTable[*ptr] < 'A' )     // If the character is punctuation
    return;
    start = ptr;
  do
  {
    i++;
    ptr++;
  }
  while ( alphaTable[*ptr] > '$' );

  if ( i > KEY_MAX_LEN + 1 )
  {
    *ptr = 0;
    fatalErr( ovrKeyLenMsg, start );
  }

  if ( ++numKeywords > WORD_MAX )
    fatalErr( ovrKeywordMsg, maxWordMsg );

  numStringBytes += i;
}

// **************************************************************************
// * GroupedOutput           Generates output grouped by keyword.           *
// **************************************************************************
void GroupedOutput( void )
{
  register uInt
    key,
    line;
  uInt
    lNum;
  BYTE 
    *fNm,
    *impStr;

  for ( key = 0; key < numKeywords; key++ )
  {
    if ( line = keyArray[key].headLn )
    {
      impStr = impLevel[keyArray[key].imp];
      do
      {
        lNum = lineArray[line].lnSpec;
        fNm = fNames[TOPBYTE( lNum )];
        lNum &= 0xffffff;
        fprintf( stdout, "%s(%d) : %s: %s\n", fNm, lNum,
                 keyStrings + keyArray[key].kStr, impStr );
      }
      while ( ( line != keyArray[key].tailLn ) && ( line = lineArray[line].nextL ) );
    }
  }
}

// ********
// * main *
// ********
void main( int argc, char **argv )
{
  register uInt i;
  register char *ptr;
  int Ok;

  if ( argc < 2 )      // If no command-line arguments have been provided,
    showHelp( );

//      **************************************************
//      * Isolate the path to VCPort.EXE                 *
//      **************************************************
  ptr = _fullpath( lBuf, argv[0], LINE_MAX_LEN );
  if ( ptr == NULL )
    fatalErr( NoProcMsg, argv[0] );
  while ( *ptr )
    ptr++;
  while ( ( ptr > lBuf ) && ( *ptr != 92 ) && ( *ptr != 47 ) )
    ptr--;
  if ( ptr == lBuf )   // If no slash or backslash is found
    vcPath = NULL;
  else
  {
    ptr++;
    *ptr = 0;
    if ( ( vcPath = strdup( lBuf ) ) == NULL )
      fatalErr( noMemMsg, forFnmMsg );
  }

//      **************************************************
//      * Initialize most other global variables         *
//      **************************************************
  buildAlphaTable( );
  mode = 0;
  nextKey = 0;
  nextLine = 1;               // Note that lineArray[0] is never used.
  numLines = 1;
  maxLines = GROUP_START;
  maxSrcFiles = FILE_STEP;
  numSrcFiles = 0;
  maxKwdFiles = FILE_STEP;
  numKwdFiles = 0;
  numKeywords = 0;
  numStringBytes = 0;
  nextKStr = 0;
  foundNone = 255;
  for ( i = 0; i < 10; i++ )
    showImp[i] = 127;

//      **************************************************
//      * Make initial allocation for Filename pointers  *
//      **************************************************
  if ( ( ( kNames = (char **) calloc( FILE_STEP, sizeof( char * ) ) ) == NULL ) ||
       ( ( fNames = (char **) calloc( FILE_STEP, sizeof( char * ) ) ) == NULL ) )
    fatalErr( noMemMsg, forFnmMsg );

//      **************************************************
//      * Process all arguments before going on          *
//      **************************************************
  for ( i = 1; i < (uInt) argc; i++ )
  {
    ptr = argv[i];
    if ( ( *ptr == '/' ) || ( *ptr == '-' ) )    // Flag argument
    {
      ptr++;
      if ( ( *ptr == 'k' ) || ( *ptr == 'K' ) )  // Keyword text file
      {
        ptr++;
        if ( !( *ptr ) )
        {
          if ( ++i >= (uInt) argc )
            fatalErr( noKWfNameMsg, againMsg );
          ptr = argv[i];
        }
        ptr = parseKeyFiles( ptr, kwdExtension );
        checkKwFNum( );
        kNames[numKwdFiles++] = ptr;
        if ( ( fHndl = fopen( ptr, "r")) == NULL )
          fatalErr( openMsg, ptr );
        while ( !feof( fHndl) && ( fgets( lBuf, LINE_MAX_LEN, fHndl ) != NULL ) )
          countSize( );
        if ( fclose ( fHndl ) )
          fatalErr( closeMsg, ptr );
      }
      else if ( ( *ptr == 'g' ) || ( *ptr == 'G' ) )  // Group output
        mode |= GROUPED;
      else if ( ( *ptr == 'i' ) || ( *ptr == 'I' ) )  // Load index
      {
        if ( mode & LOAD_INDEX )
          fatalErr( twoIFlagsMsg, againMsg );
        mode |= LOAD_INDEX;
        ptr++;
        if ( !( *ptr ) )
          ptr = ( ++i < (uInt) argc ) ? argv[i] : defIkwName;
        ifName = parseKeyFiles( ptr, ikwExtension );
      }
      else if ( ( *ptr == 's' ) || ( *ptr == 'S' ) )  // Save index
      {
        if ( mode & SAVE_INDEX )
          fatalErr( twoSFlagsMsg, againMsg );
        mode |= SAVE_INDEX;
        ptr++;
        if ( !( *ptr ) )
        {
          if ( ++i >= (uInt) argc )
            fatalErr( noSavFMsg, againMsg );
          ptr = argv[i];
        }
        sfName = ptr;
      }
      else if ( ( *ptr == 'v' ) || ( *ptr == 'V' ) )  // Verbose mode
        mode |= VERBOSE;
      else if ( ( *ptr == 'c' ) || ( *ptr == 'C' ) )  // Case-sensitive
        mode |= CASE_SENSE;
      else if ( ( *ptr == 'w' ) || ( *ptr == 'W' ) )  // Warning level
      {
        ptr++;
        if ( !( *ptr ) )
        {
          if ( ++i < (uInt) argc )
            ptr = argv[i];
        }
        if ( *ptr )
        {
          if ( isdigit( *ptr ) )
          {
            Ok = *ptr - '1';
            while ( Ok )
            {
              showImp[Ok] = 0;
              Ok--;
            }
          }
          else if ( *ptr == ':' )
          {
            ptr++;
            for ( Ok = 0; Ok < 10; Ok++ )
              showImp[Ok] = 0;
            while ( *ptr )
            {
              if ( isdigit( *ptr ) )
                showImp[*ptr - '0'] = 127;
              ptr++;
              while ( *ptr && !( isdigit( *ptr ) ) )
                ptr++;
            }
          }
        }
      }
      else
        showHelp( );
    }
    else
      parseSourceName( ptr );
  }

//      **************************************************
//      * Check for source, keyword and index files      *
//      **************************************************
  if ( !numSrcFiles )
    mode |= VERBOSE;

  if ( !numKwdFiles )
  {
    mode &= ONLY_INDEX;
    if ( !( mode & LOAD_INDEX ) )
    {
      mode |= LOAD_INDEX;
      ifName = defIkwName;
    }
  }

//      **************************************************
//      * Add in the index file size, if necessary       *
//      **************************************************
  if ( mode & VERBOSE )
    fprintf( stdout, buildIndMsg );
  if ( mode & LOAD_INDEX )
  {
    if ( mode & VERBOSE )
      fprintf( stdout, loadIkwMsg, ifName );
    if ( ( fHndl = fopen( ifName, "rb")) == NULL )
      fatalErr( openMsg, ifName );
    if ( fread( &inIhdr, sizeof(struct saveIndexHeaderStruct), 1, fHndl ) < 1 )
      fatalErr( readMsg, ifName );
    ptr = inIhdr.hdrStr + 7;
    i = *ptr - '0';
    *ptr = VCPortVer[0];
    ptr += 2;
    i = ( i << 4 ) + ( *ptr - '0' );
    *ptr++ = VCPortVer[2];
    i = ( i << 4 ) + ( *ptr - '0' );
    *ptr = VCPortVer[3];
    if ( strncmp( inIhdr.hdrStr, outIhdr.hdrStr, HD_STR_LEN ) )
      fatalErr( badIndexMsg, ifName );
    if ( i < VCPORT_VER )
      fatalErr( oldIndexMsg, ifName );
    numKeywords += inIhdr.numNodes;
    numStringBytes += inIhdr.strSpace;
  }
  numStringBytes += 8;      // Padding

//      **************************************************
//      * Allocate memory arrays for keys and lines      *
//      **************************************************
  if ( ( numKeywords == 0 ) || ( numStringBytes == 0 ) )
    fatalErr( NoKwdMsg, againMsg );
  if ( ( keyArray = (struct keyStruct *) calloc( numKeywords + 2, 
         sizeof( struct keyStruct) ) ) == NULL )
    fatalErr( noMemMsg, forKWMsg );

  if ( ( keyStrings = (BYTE *) malloc( numStringBytes ) ) == NULL )
    fatalErr( noMemMsg, forKSMsg );

  if ( mode & GROUPED )
  {
    if ( ( lineArray = (struct lineStruct *) calloc( GROUP_START, 
        sizeof( struct lineStruct) ) ) == NULL )
      fatalErr( noMemMsg, groupedMsg );
  }

//      **************************************************
//      * Load the keyword index file, if any, first     *
//      **************************************************
  if ( mode & LOAD_INDEX )
  {
    nextKey = inIhdr.numNodes;
    nextKStr += inIhdr.strSpace;

    if ( ( fread( keyArray, sizeof(struct keyStruct), nextKey, fHndl ) < nextKey ) ||
         ( fread( keyStrings, 1, inIhdr.strSpace, fHndl ) < inIhdr.strSpace ) )
      fatalErr( readMsg, ifName );

    if ( fclose ( fHndl ) )
      fatalErr( closeMsg, ifName );
    Ok = 255;
  }
  else
    Ok = 0;


//      **************************************************
//      * Build the keyword index as specified           *
//      **************************************************
  for ( i = 0; i < numKwdFiles; i++ )
  {
    ptr = kNames[i];
    if ( mode & VERBOSE )
      fprintf( stdout, loadKwdMsg, kNames[i] );
    if ( ( fHndl = fopen( ptr, "r")) == NULL )
      fatalErr( openMsg, ptr );

//          **********************************************
//          * Copy the first keyword into the root node. *
//          **********************************************
    if ( !Ok )
    {
      while ( !( feof( fHndl) ) && 
               ( fgets( lBuf, LINE_MAX_LEN, fHndl ) != NULL ) )
      {
        if ( copyKeyWord( ) != NULL )
        {
          Ok = 255;
          nextKey++;
          break;
        }
      }
    }

//          **********************************************
//          * Insert all other keywords into the index.  *
//          **********************************************
    while ( !feof( fHndl) && ( fgets( lBuf, LINE_MAX_LEN, fHndl ) != NULL ) )
    {
      iniLine ( );
#ifdef DEBUG
//      displayIndex( 0, ('-' << 24) );
#endif
    }

    if ( fclose ( fHndl ) )
      fatalErr( closeMsg, ptr );
  }

//      **************************************************
//      * Save the keyword index file, if necessary      *
//      **************************************************
  if ( mode & SAVE_INDEX )
  {
    if ( mode & VERBOSE )
      fprintf( stdout, saveIkwMsg, sfName );
    outIhdr.numNodes = nextKey;
    outIhdr.strSpace = nextKStr;
    if ( ( fHndl = fopen( sfName, "wb")) == NULL )
      fatalErr( openMsg, sfName );
    if ( ( fwrite( &outIhdr, sizeof(struct saveIndexHeaderStruct), 1, fHndl ) != 1 ) ||
         ( fwrite( keyArray, sizeof(struct keyStruct), nextKey, fHndl ) != nextKey ) ||
         ( fwrite( keyStrings, 1, nextKStr, fHndl ) != nextKStr ) )
      fatalErr( writeMsg, sfName );
    if ( fclose ( fHndl ) )
      fatalErr( closeMsg, sfName );
  }

//      **************************************************
//      * Check for source files and exit if none...     *
//      **************************************************
  if ( numSrcFiles )
  {
    if ( mode & VERBOSE )
      fprintf( stdout, scanSrcMsg );
  //      **************************************************
  //      * Build the line database from the source files  *
  //      **************************************************
#ifdef DEBUG
    displayIndex( 0, ('-' << 24) );
#else
    for ( src = 0; src < numSrcFiles; src++ )
      scanFile( );
#endif

  //      **************************************************
  //      * Output the grouped data, if necessary          *
  //      **************************************************
    if ( foundNone )
      fprintf( stdout, NoFindMsg );
    else if ( mode & GROUPED )
      GroupedOutput( );
  }
  else
    fprintf ( stdout, NoSrcFileMsg );
  exit( 0 );
}
