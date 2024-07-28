/**[f******************************************************************
* tfmstruc.h -
*
* Copyright (C) 1990,1991 Hewlett-Packard Company.
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
  
/***************************************************************************
*                                                                         *
* This file defines the template for the TFM data structure.  This        *
* structure is used by the TFM Reader to create a TFM structure.          *
*                                                                         *
***************************************************************************/
  
/**************************************************************************/
/*                           TFM structure                                */
/**************************************************************************/
  
typedef WORD far        *LPWORD;
  
struct TFMType                           /* Main Data Structure */
{                                        /* General TFM information */
    BYTE     processorFamily;
    /*  BYTE far version[5]; */
    WORD     numberTypefaces;
  
    HANDLE hType;
  
    struct typefaceType               /* Metrics info about a particular typeface */
    {
        struct generalInfoType    /* General info about a particular typeface */
        {
            /*          WORD      TFM_type; */
            /*          BYTE far  uniqueAssociationID[13]; */
            BYTE      typeface[50];
            /*          BYTE far  typefaceSource[50]; */
            BYTE      copyright[63];
            /*          BYTE far  comment[100]; */
            BYTE      typefaceSelectionString[20];
            LONG      numberCharacters;
            WORD      numberSymbolSets;
        }general;
  
        struct typefaceMetricsType              /* Metrics info about a particular typeface */
        {
            DWORD     pointN;       /* Numerator   */
            DWORD     pointD;       /* Denominator */
            DWORD     nominalPointSizeN;    /* Numerator   */
            DWORD     nominalPointSizeD;    /* Denominator */
            DWORD     designUnitsN;     /* Numerator   */
            DWORD     designUnitsD;     /* Denominator */
            BYTE      strokeWeight;
            BYTE      appearanceWidth;
            BYTE      serifStyle;
            /*          BYTE      typeStyle; */
            BYTE      typeStruct;
            WORD      spacing;
            short     slant;
            DWORD     averageWidthN;    /* Numerator   */
            DWORD     averageWidthD;    /* Denominator */
            WORD      maximumWidth;
            /*          WORD      inter_wordSpacing; */
            WORD      recommendedLineSpacing;
            WORD      capheight;
            WORD      xHeight;
            WORD      ascent;
            /*          WORD      descent; */
            WORD      lowercaseAscent;
            WORD      lowercaseDescent;
            WORD      underscoreDescent;
            WORD      underscoreThickness;
            /*          WORD      uppercaseAccentHeight; */
            /*          WORD      lowercaseAccentHeight; */
        }typefaceMetrics;
  
        struct symbolType     /* Chars & symsets located in typeface */
        {
            /*          HANDLE hSymMap; */
  
            /*          LPWORD symbolMap; */
  
            HANDLE hSymDir;
  
            struct symbolSetDirectoryType
            {
                BYTE      symbolName[30];
                BYTE      selectionName[5];
                HANDLE    hSI;
                LPWORD    symbolIndex;
                WORD      symbolLength;
            }far *symbolSetDirectory;
  
        }symbol;
  
        struct characterMetricsType     /* Metrics info for each char in typeface */
        {
            HANDLE hChar;
  
            struct characterType
            {
                WORD      horizontalEscapement;
                /*              WORD      verticalEscapement;
                WORD      leftExtent;
                WORD      rightExtent;
                WORD      characterAscent;
                WORD      characterDescent;
            */          }far *character;
  
        }characterMetrics;
  
#ifdef KERN
        struct kerningType              /* Different kerning info about typefaces */
        {
            HANDLE hKern;
  
            WORD      numberPairs;
            WORD      realPairs;
            struct kernPairsType           /* Kern pair info */
            {
                WORD      firstCharIndex;
                WORD      secondCharIndex;
                short     kernValue;
            }far *kernPairs;
  
  
  
            /*
            WORD      numberKernChars;
            WORD      numberSectors;
            struct sectorKernCharType      // Sector kerning information
            {
            WORD      charIndex;
            LPWORD    leftSector;
            LPWORD    rightSector;
            }far *sectorKernChar;
            */
  
  
  
            /*
            WORD      numberTracks;
            struct trackKernType           // Track kerning information
            {
            WORD      trackValue;
            WORD      maxPointSize;
            WORD      minPointSize;
            WORD      maxKern;
            WORD      minKern;
            }far *trackKern;
            */
  
        }kerning;
#endif
  
    }far *typeface;
  
};
  
  
/***************************************************************************
***************************************************************************/
  
