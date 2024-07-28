


#define INTERNAL_LEADING

#include "pscript.h"
#include <winexp.h>
#include "etm.h"
#include "fonts.h"
#include "debug.h"
#include "enum.h"
#include "utils.h"
#include "resource.h"
#include "psdata.h"
#include "driver.h"
#include "truetype.h"
#include "profile.h"

#define UnlockResource(h)   GlobalUnlock(h)

extern BOOL bTTEnabled;

DWORD   WINAPI GetAppCompatFlags(WORD);                /* ;Internal */
              			                        /* ;Internal */
/* GetAppCompatFlags flag values */			/* ;Internal */
#define GACF_30AVGWIDTH     0x00080000              /* ;Internal */
                                          /* Two fixes for Turbo Tax  */


typedef  short  huge *HPSHORT;
typedef  BYTE   huge  *HPBYTE;

/* Conversion codes for the InfoToStruct function */
#define TO_TEXTXFORM    0
#define TO_TEXTMETRIC    1
#define TO_LOGFONT    2

/* logFont OutPrecision values */
#define OUT_TT_PRECIS		4
#define OUT_DEVICE_PRECIS	5
#define OUT_TT_ONLY_PRECIS		7

typedef  BYTE  FAR  *LPBYTE;

WORD  FAR PASCAL ReturnRefEM(LPDF  lpdf);

/*------------------------ local functions --------------------------*/

void    PASCAL    ScaleFont(LPDV, LPFONTINFO, int, int);
void    PASCAL    SetLeading(LPFONTINFO, int, int);
void    PASCAL    InfoToStruct(LPFONTINFO, short, LPSTR);
int FAR PASCAL LoadFont(LPDV  lpdv, LPSTR  lszFont, LPDF  lpdf);
int FAR PASCAL EnumDFonts(lpdv, lszFace,  lpfn, lpb);
int FAR PASCAL EnumFonts(LPDV lpdv, LPSTR lszFace, 
               int (FAR PASCAL *lpfn)(LPLOGFONT,LPTEXTMETRIC,int,LPSTR),
               LPSTR lpb, WORD wFlags);
WORD PASCAL GetTTFontID(LPDV lpdv, LPSTR lpszCacheKey);


BOOL PASCAL AliasFace(LPSTR  lpDevName);

BYTE  PASCAL  ScoreFont(
    HPBYTE  lpbPublic,
    LPLOGFONT lplf,
    BYTE  HighScore,
    LPBYTE  lpScoreArray,
    BYTE   (PASCAL  *lpfScoreFunc)(LPFONTINFO   lpdf, LPLOGFONT  lplf)  );

BYTE  PASCAL  ScoreFontNames(LPFONTINFO   lpdf, LPLOGFONT  lplf);

BYTE   PASCAL  ScorePitchAndFamily(LPFONTINFO   lpdf, LPLOGFONT  lplf);

BYTE   PASCAL  ScoreItalicsAndBold(LPFONTINFO   lpdf, LPLOGFONT  lplf);

int  FAR  PASCAL  RealizeFont(lpdv, lplf, lpdf, lpft);


int PASCAL TTRealizeFont(
    LPDV lpdv, 
    LPLOGFONT lplf, 
    LPFONTINFO lpdf, 
    LPTEXTXFORM lpft, 
    LPFONTINFO  lpdfEngine, 
    WORD  cbEngineFont);


WORD   PASCAL  DevRealizeFont(
LPDV          lpdv,        // Ptr to the device descriptor
LPSTR         sfPaths,
LPFONTINFO    lpdf,        // Ptr to the place to put the device font
LPLOGFONT     lplf,
LPTEXTXFORM   lpft,
short         iFont,       // index to FontDir, first devFont is iFont = 0
BOOL          bInfoToStruct);  // if set, then munge lpft


/* AliasFace Tables */



static char *szAlias0[] = { "Tms Rmn", "Times Roman", "TimesRoman", "Times",
                                         "TmsRmn", "Varitimes", "Dutch",
                                         "Times New Roman", "TimesNewRomanPS", 
                                         NULL  };

static char *szAlias1[] = { "Helv", "Helvetica", "Arial",
                                         "Swiss", NULL };


static char *szAlias2[] = { "Courier New", "Courier", NULL };


static char *szAlias3[] = { "Arial-Narrow", "Arial Narrow", "Helvetica-Narrow", 
                    "Helvetica Narrow", NULL };


static char *szAlias4[] = { "Zapf Calligraphic", "Bookman Antiqua", "Palatino", 
                    "Book Antiqua", "ZapfCalligraphic", NULL };


static char *szAlias5[] = { "ITC Bookman", "Bookman Old Style", "Bookman", NULL };


static char *szAlias6[] = { "Century Schoolbook", "NewCenturySchlbk", 
                        "NewCenturySchoolBook", "New Century SchoolBook",
                        "CenturySchoolBook", NULL };


static char *szAlias7[] = { "Century Gothic", "AvantGarde", 
                        "ITC Avant Garde", NULL };


static char *szAlias8[] = { "Monotype Corsiva", "ZapfChancery", 
                        "ITC Zapf Chancery", NULL };


static char *szAlias9[] = { "Monotype Sorts", "ZapfDingbats", "Zapf Dingbats",
                        "ITC Zapf Dingbats", NULL };


#define  NUMCATAGORIES  10   // number of alias groups 

static WORD  k;  // selects a particular Alias Group in AliasTable[k]

static char *(*AliasTable[NUMCATAGORIES]);


/***************************************************************************
 * AliasFace
 *
 * this routine looks through an Alias table that associates 
 * groups of font names with each other.  That is any name in the
 * group is an alias for any other name in the group.
 * If the two names passed are aliases of each other
 * (are found in the same group)  then   return(TRUE);
 * else  return(FALSE);
 *
 * as an optimization the log fontname is associated with the group k.
 * this is done only once at the start of RealizeFont().
 *
 * PeterWo: this code rewritten to eliminate dependency on
 * Family property.  This code treats logical and device names
 * symmetrically.
 *
 **************************************************************************/

BOOL PASCAL AliasFace(lpDevName)
LPSTR        lpDevName;        /* the device face name */
{
    WORD     j;  // k  is a static global determined previously

    if(k >= NUMCATAGORIES)
        return(FALSE);

    for(j = 0 ; AliasTable[k][j] ; j++)
    {
        if (lstrcmpi(AliasTable[k][j], lpDevName) == 0)
            return(TRUE);
    }

    return(FALSE);
}





// ----- BYTE  ScoreFont(lpbPublic, lplf, HighScore, lpScoreArray, 
//                          lpfScoreFunc); -------
//
//  lpbPublic: points to Public portion of FontDir
//      from this all lpdfs are accessible.
//  lplf:  points to logfont
//  HighScore: contains highScore threshold
//  lpScoreArray: points to array of scores
//  lpfScoreFunc: pointer to function which assigns a score
//      based on comparision of lpdf and lplf.
//
//  Fills/rescores all fonts 
//  ScoreFont invokes lpfScoreFunc only for those fonts
//  whose's scores match the supplied HighScore.
//  The scores of non-qualifying fonts are set to zero.
//
//  Returns: the new HighScore
//-------------------------------------------------------------------------


BYTE  PASCAL  ScoreFont(lpbPublic, lplf, HighScore, 
                            lpScoreArray, lpfScoreFunc)
HPBYTE  lpbPublic;
LPLOGFONT lplf;
BYTE  HighScore;
LPBYTE  lpScoreArray;
BYTE   (PASCAL  *lpfScoreFunc)(LPFONTINFO   lpdf, LPLOGFONT  lplf);
{
    BYTE  newHighScore, newScore;
    WORD  i, numPubFonts;
    HPBYTE  lpFontEntry;
    LPFONTINFO  lpdf;


    newHighScore = 0;

    if(lpScoreArray[0] >= HighScore)
    {
        lpdf = (LPFONTINFO)(lpbPublic - ((HPSHORT)lpbPublic)[-1]);
        lpScoreArray[0] = newScore = (lpfScoreFunc)(lpdf, lplf);        
        if(newScore > newHighScore)
            newHighScore = newScore;
    }
    else
        lpScoreArray[0] = 0;  // you're out of the competition
    
    numPubFonts = *((HPSHORT)lpbPublic);
    lpFontEntry = lpbPublic + sizeof(WORD);

    for(i = 1 ; i <= numPubFonts ; i++)
    {
        if(lpScoreArray[i] >= HighScore)
        {
            WORD  EntrySize, j;
            HANDLE  hBuf = 0;

            //  all this extra crap is to allow FontDir larger
            //  than one segment.

            EntrySize = *((HPSHORT)lpFontEntry);
            if((WORD)lpFontEntry > (WORD)lpFontEntry + EntrySize)
            {
                HPBYTE  lpSrc;
                LPBYTE  lpDest;

                lpSrc = lpFontEntry + 4;
                if(!(hBuf = GlobalAlloc(GHND, (long)EntrySize)))
                    return(newHighScore);
                // failure here invalidates scoring
                
                lpDest = GlobalLock(hBuf);
                lpdf = (LPFONTINFO)lpDest;
                for(j = 0 ; j < EntrySize ; j++)
                    *lpDest++ = *lpSrc++;
            }
            else
                lpdf = (LPFONTINFO)(lpFontEntry + 4);
            lpScoreArray[i] = newScore = (lpfScoreFunc)(lpdf, lplf);        
            if(newScore > newHighScore)
                newHighScore = newScore;
            if(hBuf)
            {
                GlobalUnlock(hBuf);
                GlobalFree(hBuf);
            }
        }
        else
            lpScoreArray[i] = 0;  // you're out of the competition

        lpFontEntry += *((HPSHORT)lpFontEntry);
    }

    return(newHighScore);
}


BYTE  PASCAL  ScoreFontNames(LPFONTINFO   lpdf, LPLOGFONT  lplf)
{
    BYTE  score;
    LPSTR  lplfFaceName, lpdfFaceName;

    lplfFaceName = lplf->lfFaceName;
    lpdfFaceName = ((LPSTR)lpdf) + lpdf->dfFace;

    if (!lstrcmpi(lplfFaceName, lpdfFaceName)) 
        score = 4;
    else if (AliasFace(lpdfFaceName))
        score = 2;
    else
        score = 1;
        
    return(score);
}


BYTE   PASCAL  ScorePitchAndFamily(LPFONTINFO   lpdf, LPLOGFONT  lplf)
{
    BYTE  score = 1;
    LPSTR  lpdfFaceName;


    if(lplf->lfCharSet == lpdf->dfCharSet)
        score += 16;    // prevent Symbol from being used for Roman fonts
    if(lpdf->dfPitchAndFamily & 1)  // variable pitch font
    {
        if((lplf->lfPitchAndFamily & 0x03) == VARIABLE_PITCH)
            score += 8; 
    }
    else  // fixed pitch font
    {
        if((lplf->lfPitchAndFamily & 0x03) == FIXED_PITCH)
            score += 8; 
    }

    if ((lplf->lfPitchAndFamily & 0x0f0) == 
            (lpdf->dfPitchAndFamily & 0x0f0)) 
    {
        score += 4; 
    }

    lpdfFaceName = ((LPSTR)lpdf) + lpdf->dfFace;

    if (!lstrcmpi("Courier", lpdfFaceName)) 
        score += 2;

    return(score);
}



BYTE   PASCAL  ScoreItalicsAndBold(LPFONTINFO   lpdf, LPLOGFONT  lplf)
{
    BYTE  score = 1, logItalics, candidateItalics;
    WORD  logWeight, candidateWeight, weightDiff;

    //  hirearchy that I wanted to impose through scoring system:
    //  penalize an overItalicized font severely.
    //  Since GDI cannot simulate a non-italics font
    //  from an italicized one.  
    //  The penalty of 45 pts is set larger than the
    //  bonus of 30 + 10 = 40 pts for having perfect weight.
    //  Penalize an overweight font severely,
    //  Though not as badly as an overItalicized font.
    //  Again GDI cannot lighten the font's weight.
    //  There is a 30 point penalty for being overweight.
    //  I would rather force GDI to simulate both
    //  italics and boldness on a very light font
    //  than to use an italicized font that was too heavy.
    //  Therefore 10 + 12 < 30 ; the penalty for being overweight.
    //  Finally, its easier to simulate boldness than italics
    //  so that is why the bonus for matching italics (12) is greater
    //  than the bonus for matching weights (10).

    //  in other words, this is delicately balanced and 
    //  interlinked scoring system.  Don't change any values
    //  without knowing exactly what the consequences are.

    //  some apps violate specification by using values
    //  other than 0 or 1 for the flags.  See DDK 12-36

    logItalics = (BYTE)lplf->lfItalic & (BYTE)1;        //  possible values are 0 or 1
    candidateItalics = (BYTE)lpdf->dfItalic & (BYTE)1;

    if(logItalics >= candidateItalics)
        score += 45;  // bonus for not being overItalicized.
                      //  GDI can't unItalicize a font

    logWeight = lplf->lfWeight;
    candidateWeight = lpdf->dfWeight;

    if(logWeight >= candidateWeight)
        score += 30;  // bonus for not being overweight.
                      //  GDI can't unBolden a font

    if(logItalics == candidateItalics)
        score += 12;  // bonus for matching Italics;

    if(logWeight >= candidateWeight)
        weightDiff = (logWeight - candidateWeight) / 100;
    else
        weightDiff = (candidateWeight - logWeight) / 100;

    score += 10 - weightDiff;  //  the closer the weights match, the better

    return(score);
}


int  FAR  PASCAL  RealizeFont(lpdv, lplf, lpdf, lpft)
LPDV        lpdv;    /* device descriptor */
LPLOGFONT    lplf;    /* input logical font */
LPFONTINFO    lpdf;    /* memory alloc by GDI for the font */
LPTEXTXFORM    lpft;    /* font transformation structure*/
{
    HPBYTE  lpbDir, lpbTTfont;
    LPBYTE  
        lpbScoreArray;   //  keeps scores for every font.  
    WORD    wNumFonts,   //  total number of entries in FontDir 
                         //  including the TT font in the private section
        i, cb, cbDev;    //  size of buffer needed to hold lpdf
    HANDLE  hScore;
    BYTE    HighScore;
    BOOL    bInfoToStruct = TRUE, bPerformSub;
    LPFONTINFO    TTlpdf;   // points to TTlpdf in a subfont
    static  LPFONTINFO    lpdfEngine;
    static  BOOL  bTTreplaced;    //  a device font was found to replace TT
    static  WORD  BestDevFont,  //  holds index to DevFont with high score
        cbEngineFont;    //  saves size of Structure returned by 
                         //  EngineRealizeFont()
    static  TEXTXFORM  TextXForm;  // holds temp textxform for 
                        // EngineRealizeFont to scribble into.
    static  HPBYTE  lpbPublic;



    lpbPublic = LockFontDir(lpdv->iPrinter);
        //  assume this will succeed

    hScore = 0;  //  this isn't used on the second pass

    if(lpdf)   //  this is the GDI's second pass
        goto  RealFontPass2;  // assume FontDir is unchanged since pass 1

    bTTreplaced = FALSE;  // initialize static.

    wNumFonts = *((HPSHORT)lpbPublic) + 1;   // don't forget the EngineFont
    if (!(hScore = GlobalAlloc(GHND | GMEM_SHARE, (long)wNumFonts)))
    {
        cb = 0;
        goto Cleanup;
    }

    // initialize AliasTable

    AliasTable[0] = szAlias0;
    AliasTable[1] = szAlias1;
    AliasTable[2] = szAlias2;
    AliasTable[3] = szAlias3;
    AliasTable[4] = szAlias4;
    AliasTable[5] = szAlias5;
    AliasTable[6] = szAlias6;
    AliasTable[7] = szAlias7;
    AliasTable[8] = szAlias8;
    AliasTable[9] = szAlias9;

    for(k = 0 ; k < NUMCATAGORIES ; k++)
    {
        for(i = 0 ; AliasTable[k][i] ; i++)
        {
            if (lstrcmpi(AliasTable[k][i], lplf->lfFaceName) == 0)
                goto  FoundGroup;
        }
    }

FoundGroup:  //  k saves the group number for use by AliasFace.

    lpbScoreArray = GlobalLock(hScore);
    lmemset(lpbScoreArray, 1, wNumFonts);

    lpbDir = lpbPublic - sizeof(WORD);  // lpbDir points to offset
    lpbTTfont = lpbPublic - *((HPSHORT)lpbDir);
    lpdfEngine = (LPFONTINFO)lpbTTfont;

    //  we assume GDI always makes RealizeFont calls in Pairs
    //  and that both calls are contained in one critical section,
    //  This is necessary since the buffer accessed by LockFontDir
    //  is shared by multiple instances of the driver.
    //  This buffer holds the lpdf from the first call to be used
    //  by the second call.  Also some static variables are
    //  used to preserve important state info from the 1st to 2nd call.
    //  we also assume that the rest of the driver is aware that
    //  RealizeFont will overwrite any prevs data stored in the 
    //  private buffer

    if (bTTEnabled)
    {
        lpft = (LPTEXTXFORM)&TextXForm;
        if (!(cbEngineFont = EngineRealizeFont(lplf, lpft, lpdfEngine))
            || !lpdfEngine->dfPoints) 
            lpbScoreArray[0] = 0;  // disqualify any TT font
    }
    else
        lpbScoreArray[0] = 0;  // disqualify any TT font


    HighScore = ScoreFont(lpbPublic, lplf, HighScore = 1, 
                    lpbScoreArray, ScoreFontNames);


    bPerformSub = lpdv->DevMode.bSubFonts ||  lpdv->DevMode.bNoDownLoad;
    //  go through substitution process - substitute if possible.

    if(!bPerformSub  &&  lpbScoreArray[0]  &&  
            lplf->lfOutPrecision == OUT_TT_ONLY_PRECIS)
    //  use TT font whenever availible even if its not optimal
    {
        HighScore += 2;
        lpbScoreArray[0] = HighScore;
    }
    else  if(HighScore > 1  &&  lpbScoreArray[0] == HighScore)
    //  break ties in favor of Engine or Device font
    {
        BOOL bFavorTT;

        if(lplf->lfOutPrecision == OUT_DEVICE_PRECIS)
            bFavorTT = FALSE;
        else if(lplf->lfOutPrecision == OUT_TT_PRECIS)
            bFavorTT = TRUE;
        else
            bFavorTT = lpdv->DevMode.bFavorTT;

        if(bFavorTT)
        {   // if explictly directed to favor engine fonts - do so
            HighScore++;
            lpbScoreArray[0] = HighScore;
        }
        else  //  favor the Device Fonts by default
        {  
            
            BYTE  newScore = HighScore;
            WORD   i;

            for(i = 1 ; i < wNumFonts ; i++)
            {
                if(lpbScoreArray[i] == HighScore)
                {
                    lpbScoreArray[i] = newScore = HighScore + (BYTE)1;
                }
            }
            HighScore = newScore;
        }
    }


    if(HighScore == 1)   // no names matched - forced into random map mode
    {
        lpbScoreArray[0] = 0;  // disqualify any TT font
    }
    else if(lpbScoreArray[0] == HighScore)
    {

        if(bPerformSub)
        {
            //  the Engine font is in the running
            //  should we take steps to substitute or remove it?

            char    ReplacementFont[LF_FACESIZE];  // this is the right size
            LPSTR   lpEngineFontName;
            BYTE    tempScore;



            ReplacementFont[0] = '\0';

            lpEngineFontName = ((LPSTR)lpdfEngine) + lpdfEngine->dfFace;

            FindSubFont(lpdv, bPerformSub, lpEngineFontName, ReplacementFont);

            if(ReplacementFont[0])  // sub font exists
            {

                HPBYTE  lpFontEntry;
                LPSTR   lpDevName;
                LPFONTINFO   lpdf;

                lpFontEntry = lpbPublic + sizeof(WORD);


                for(i = 1 ; i < wNumFonts ; i++)
                {
                    //  scan every FontName in FontDir and 
                    //  set its score = HighScore + 2;
                    //  if its name matches ReplacementFont name

                    WORD  EntrySize, j;
                    HANDLE  hBuf = 0;

                    //  all this extra crap is to allow FontDir larger
                    //  than one segment.

                    EntrySize = *((HPSHORT)lpFontEntry);
                    if((WORD)lpFontEntry > (WORD)lpFontEntry + EntrySize)
                    {
                        HPBYTE  lpSrc;
                        LPBYTE  lpDest;

                        lpSrc = lpFontEntry + 4;
                        if(!(hBuf = GlobalAlloc(GHND, (long)EntrySize)))
                            break;
                        // failure here invalidates font substitution
                        lpDest = GlobalLock(hBuf);
                        lpdf = (LPFONTINFO)lpDest;
                        for(j = 0 ; j < EntrySize ; j++)
                            *lpDest++ = *lpSrc++;
                    }
                    else
                        lpdf = (LPFONTINFO)(lpFontEntry + 4);

                    lpDevName = ((LPSTR)lpdf) + lpdf->dfFace;
                    if(!lstrcmpi(lpDevName, ReplacementFont))
                    {
                        lpbScoreArray[i] = HighScore + (BYTE)2;
                    }

                    if(hBuf)
                    {
                        GlobalUnlock(hBuf);
                        GlobalFree(hBuf);
                    }

                    lpFontEntry += *((HPSHORT)lpFontEntry);
                }
                //  its possible though unlikely that the suggested
                //  replacement font was not found in FontDir.
                //  in this case just pretend no substitution was provided.
            }

            if(lpdv->DevMode.bNoDownLoad)
            {
                lpbScoreArray[0] = 0;  // disable the Engine font regardless
            }

            tempScore = 0;  // figure out the newHighScore

            for(i = 0 ; i < wNumFonts ; i++)
            {
                if(lpbScoreArray[i] > tempScore)
                {
                    tempScore = lpbScoreArray[i];
                }
            }

            if(tempScore == HighScore + (BYTE)2)
                bTTreplaced = TRUE;  // succeeded in finding a substitute font.

            HighScore = tempScore;
        }
    }

    if(HighScore == 1)   // just killed the only qualified font
    {
        HighScore = ScoreFont(lpbPublic, lplf, HighScore, 
                    lpbScoreArray, ScorePitchAndFamily);
    }

    HighScore = ScoreFont(lpbPublic, lplf, HighScore, 
                lpbScoreArray, ScoreItalicsAndBold);


    //  determine the BestDevFont

    BestDevFont = 0;  // assume the best font is the Engine Font

    for(i = 1 ; i < wNumFonts ; i++)
    {
        if(lpbScoreArray[i] == HighScore)
        {
            BestDevFont = i;
            break;   //  since Resident fonts preceed SoftFonts we
        }            //  bias ourselves toward Resident fonts.
    }

            
RealFontPass2:  //  skip all of the scoring for the 2nd pass!

    cbDev = cb = 0;
    TTlpdf = NULL;


    if (bTTreplaced == TRUE  ||  !BestDevFont)
    {
        if(lpft != (LPTEXTXFORM)&TextXForm)
            *lpft = TextXForm;

        cb = TTRealizeFont(lpdv,  lplf,  lpdf, 
                          lpft, lpdfEngine, cbEngineFont);
        if (!cb)
            goto Cleanup;

        if(BestDevFont  &&  lpdf)
        {
            //  prepare to append device font's lpdf.

            lpdf->dfType |= TYPE_SUBSTITUTED;
            bInfoToStruct = FALSE;
            TTlpdf = lpdf;
            lpdf = (LPFONTINFO)((char far *)lpdf + cb); 
                // write device FontInfo following  Engine's FontInfo
        }
        else if(lpdf)
        {
            LPTTFONTINFO lpttfi;
            SCALABLEFONTINFO FAR *lpScaFnt;

            lpScaFnt = (SCALABLEFONTINFO FAR *)lpdf;
            lpttfi = (LPTTFONTINFO) ((LPSTR) lpdf + lpdf->dfBitsOffset);
            lpttfi->TTRefEM = lpScaFnt->erEM;
            TTLockFont(lpdv, lpdf);  // realize Reference font.
        }
    }
    if(BestDevFont)
    {
        //  write into memory pointed to by user's lpdf
        //  may inadvertantly be appending to Engine lpdf.

        HPBYTE  lpFontEntry, lpSrc;
        BYTE    sfPaths[256];

        lpFontEntry = lpbPublic + sizeof(WORD);


        for(i = 1 ; i < BestDevFont ; i++)
            lpFontEntry += *((HPSHORT)lpFontEntry);

        lpSrc = (lpFontEntry + 4) + *((HPSHORT)(lpFontEntry + 2));
               // (-----lpdf------) +  (---------dfFont----------)
        
        //  ensure that sfPaths does not cross a segment.

        for(i = 0 ; lpSrc[i] ; i++)
            sfPaths[i] = lpSrc[i];
        sfPaths[i] = '\0' ;

        cbDev = DevRealizeFont(lpdv, sfPaths, lpdf, lplf, lpft, 
                                BestDevFont - 1, bInfoToStruct  );
        if(cbDev)
            cb += cbDev;
    }

    if (TTlpdf)
    {   //  force Device CharWidths to match TT CharWidths exactly
        // at this point lpdf is the Device's

        short  i, j;
    	LPFX	lpfx;
        LPTTFONTINFO lpttfi;
    	LPSHORT lpCharWidths;

        lpttfi = (LPTTFONTINFO) ((LPSTR) TTlpdf + TTlpdf->dfBitsOffset);

        EngineGetCharWidth(TTlpdf, 0, 255, lpttfi->rgwWidths);
        TTlpdf->dfType |= TYPE_HAVEWIDTHS;    
        lpdf->dfAscent = TTlpdf->dfAscent;  // force base lines to line up.

        /* get the extended font structure */
        lpfx = LockFont(lpdf);  // device font

        if (!(lpdf->dfPitchAndFamily & 1))
        {           //  This is a fixed pitch font
            lpdf->dfAvgWidth = lpttfi->rgwWidths[32];
                // since all widths in rgwWidths are the same
        }       // pick a random element.
        else
        {
            lpCharWidths = (LPSHORT) lpfx->rgWidths;
            for(j = 0, i = lpdf->dfFirstChar ; 
                i <= (short)lpdf->dfLastChar ; j++, i++)
            {
                lpCharWidths[j] = 
                        Scale(lpttfi->rgwWidths[i], EM, lpfx->sx);
            }
        }
    }



Cleanup:

    if(hScore)
    {
        GlobalUnlock(hScore);
        GlobalFree(hScore);
    }

    UnlockFontDir(lpdv->iPrinter);
    return(cb);
}


int PASCAL TTRealizeFont(
LPDV lpdv, 
LPLOGFONT lplf, 
LPFONTINFO lpdf, 
LPTEXTXFORM lpft, 
LPFONTINFO  lpdfEngine, 
WORD  cbEngineFont)
{
    LPTTFONTINFO lpttfi;
    int  cxFont, cyFont;
    char szBuf[60];
    WORD wFontID;

    if (!lpdf) 
        goto ttRealFont_exit;

    lmemcpy(lpdf, lpdfEngine, cbEngineFont);
    lpttfi = (LPTTFONTINFO) ((LPSTR) lpdf + cbEngineFont);

    lpdf->dfBitsOffset = cbEngineFont;
    
    // force download on first use
    lpttfi->iPageNumber = -1;

    cxFont = lplf->lfWidth;
    cyFont = lplf->lfHeight;

#ifdef INTERNAL_LEADING

    cyFont =  lpdf->dfPixHeight - lpdf->dfInternalLeading;

    // the two cases for pos and neg lfHeight
    // should already be taken care of by GDI
    // in the FontInfo struct returned by EngineRealizeFont.
#else
    /* Negative font height means exclude internal leading */
    if (cyFont < 0)
        cyFont = -cyFont;

#endif
    /* Default to a 10 point font */
    if (cyFont == 0)
        cyFont = Scale(10, lpdv->iRes, 72);

    lpttfi->sx = lpttfi->sy = cyFont;
    lpttfi->lid = ++lpdv->lid;
    lpttfi->lfPoints = Scale(cyFont, 72, lpdv->iRes);

    /* set the outline bit if appropriate */
    if (lpdv->DevMode.iDLFontFmt == DLFMT_TRUETYPE    ||
        (lpdv->DevMode.iDLFontFmt == DLFMT_TYPE1       && 
        cyFont >= lpdv->DevMode.iMinOutlinePPEM))
    {
        lpdf->dfType |= TYPE_OUTLINE;
    }

    /* make a font cache key to uniquely identify the font */
    wsprintf(szBuf, "%04xD%sF%04x%04x%04x%04x%04x",
             HIWORD((DWORD)lpdv),
             lplf->lfFaceName, 
             cxFont,
             (lpdf->dfType & TYPE_OUTLINE) ? 0 : cyFont,
             (lpdf->dfType & TYPE_OUTLINE) ? 0 : lplf->lfEscapement,
             lplf->lfWeight,
             lplf->lfItalic
            );

    /* get the font ID */
    wFontID = GetTTFontID(lpdv, szBuf);

    /* make the facename out of the atom */
    wsprintf(lpttfi->TTFaceName, "MSTT31%04x", wFontID);

    /* keep a copy of the logical font & transform */
    lpttfi->lfCopy = *lplf;
    lpttfi->ftCopy = *lpft;

    lpdf->dfType |= TYPE_TRUETYPE;

    if(lpft)
    {
        if(lpdf->dfUnderline != lplf->lfUnderline)
            lpft->ftUnderline = lplf->lfUnderline;
        if(lpdf->dfStrikeOut != lplf->lfStrikeOut)
            lpft->ftStrikeOut = lplf->lfStrikeOut;
    }

ttRealFont_exit:

    return cbEngineFont + sizeof(TTFONTINFO);
}


WORD   PASCAL  DevRealizeFont(lpdv, sfPaths, lpdf, lplf, lpft, iFont,
            bInfoToStruct)
LPDV          lpdv;        // Ptr to the device descriptor
LPSTR         sfPaths;
LPFONTINFO    lpdf;        // Ptr to the place to put the device font
LPLOGFONT     lplf;
LPTEXTXFORM   lpft;
short         iFont;       // index to FontDir, first devFont is iFont = 0
BOOL          bInfoToStruct;  // if set, then munge lpft
{
    LPSTR    ptr,
             sfLoadPath;    /* ABF (font file) path */
    WORD  cb;


    /* sfPaths  either contains a resource name or 
     * the softfont paths (pfm file and font download file).  
     * we hash through the string and seperate the two (if necessary).
     * if there is a download file this is a downloadable soft font */

    ptr = sfPaths;

    /* note: there can be no space after the comma! */

    while (*ptr && *ptr != ',')
        ptr++;

    if (*ptr)    /* if we hit the ',' */
    {            
        *ptr++ = 0;        /* seperate with a null */
        sfLoadPath = ptr;    /* point to the download file */
    } 
    else 
    {
        sfLoadPath = NULL;    /* this means not downloadable font */
    }

    DBMSG1((" RealizeFont(): sfPaths=%ls sfLoadPath=%ls\n",
        (LPSTR)sfPaths,
        (LPSTR)sfLoadPath));

    if (!(cb = LoadFont(lpdv, sfPaths, lpdf))) 
    {
        DBMSG((" RealizeFont(): load failed\n"));

        if(sfLoadPath)  // we deleted the comma
            *(--sfLoadPath) = ',' ;  // replace it!
        return 0;
    }

    if (lpdf)   //  only deals with device font!
    {
        LPFX    lpfx;

        lpfx = LockFont(lpdf);

        lstrcpy(lpfx->lszFace, (LPSTR)lpdf + lpdf->dfFace);

        lpdf->dfFace = LOWORD((DWORD)lpfx->lszFace);
            // now both pointers refer to the same place 
        lpfx->iFont = iFont;
        DBMSG1((" RealizeFont(): FX: i=%d,font=%ls,facd=%ls,dv=%ls\n",
            lpfx->iFont, (LPSTR)lpfx->lszFont, (LPSTR)lpfx->lszFace,
            (LPSTR )lpfx->lszDevType));
        DBMSG1((" RealizeFont(): DF: t=%d,i=%d,w=%d,cs=%d,p=%d\n",
            lpdf->dfType, lpdf->dfItalic, lpdf->dfWeight, lpdf->dfCharSet,
            lpdf->dfPitchAndFamily));

        /*** rb BitStream ****/

        /* save sfLoadPath for later when we may have to down load
         * the font data file */

        if (sfLoadPath) 
        {  /* put load path in extended info */
            DBMSG(("downloadable softfont %ls\n", (LPSTR)sfLoadPath));

            lstrcpy(lpfx->sfLoadPath, sfLoadPath);
        }
        else
            lpfx->sfLoadPath[0] = 0;

        /* Set the font metrics as specified in the logical font */
        ScaleFont(lpdv, lpdf, lplf->lfWidth, lplf->lfHeight);

        lpfx->orientation = lplf->lfOrientation;
        lpfx->escapement = lplf->lfEscapement;
        lpfx->lid = ++lpdv->lid;

        lpdf->dfUnderline = lplf->lfUnderline;
        lpdf->dfStrikeOut = lplf->lfStrikeOut;

        if (bInfoToStruct) {
           InfoToStruct(lpdf, TO_TEXTXFORM, (LPSTR)lpft);
           lpft->ftEscapement = lplf->lfEscapement;
           lpft->ftOrientation = lplf->lfOrientation;
        }
    }

    if(sfLoadPath)  // we deleted the comma
        *(--sfLoadPath) = ',' ;  // replace it!

    return (cb);
}

/****************************************************************
* Name: LoadFont()
*
* Action: This routine loads a device font from either from
*      a resource or from an external PFM file.  If the
*      font's name starts with a $ sign, then the font
*      metrics are located in an external PFM file.
*
*      This routine is called twice for each font.  The
*      first time a NULL destination ptr is passed in
*      and this routine just returns the size of the font.
*
* Returns: The size of the device font.
*
**************************************************************/

int FAR PASCAL LoadFont(lpdv, lszFont, lpdf)
LPDV    lpdv;        /* Ptr to the device descriptor */
LPSTR    lszFont;    /* Ptr to the font's name string */
LPDF    lpdf;        /* Ptr to the place to put the device font */
{
    HANDLE    hFont;        /* The font's memory handle */
    HANDLE    hres;        /* The fonts resouce handle */
    LPSTR    lpbSrc;        /* Ptr to a place to put the font */
    LPFX    lpfx;        /* Ptr to the extended font info */
    LPPFM    lppfm;        /* Ptr to the pfm file in memory */
    int    cbLeader;
    int    cb;
    int    fh;
    int    soft = 0;    /* flag to recognize soft load djm 12/20/87 */

    DBMSG1((">LoadFont(): name:%ls lpdf:%lx\n", (LPSTR)lszFont, lpdf));

    lppfm = NULL;
    cbLeader = ((LPSTR) & lppfm->df) - (LPSTR) lppfm;

    if (*lszFont == '$') {

        DBMSG1((" LoadFont(): softfont\n"));

        /* The font info is located in an external PFM file */
        if ((fh = _lopen(lszFont + 1, READ)) < 0) {
            DBMSG1((" LoadFont(): can't open %ls\n",
                (LPSTR)(lszFont + 1)));
            return 0;
        }
        cb = (int)( _llseek(fh, 0L, 2) - (long)cbLeader );

        if (lpdf) {
            _llseek(fh, (long) cbLeader, 0);
            _lread(fh, (LPSTR) lpdf, cb);
        }
        _lclose(fh);
        soft = 1;

    } else {

        DBMSG1((" LoadFont(): resident font\n"));

        /* The font info is located in a resource */

        hres = FindResource(ghInst, lszFont, MAKEINTRESOURCE(MYFONT));

        if (!hres) {
            DBMSG1((" LoadFont(): bad resource handle\n"));
            return 0;
        }

        cb = SizeofResource(ghInst, hres) - cbLeader;

        if (lpdf) {
            if (!(hFont = LoadResource(ghInst, hres))) {
                DBMSG1((" LoadFont(): can't load resource\n"));
                return 0;
            }

            /* Copy the fontinfo structure into the memory provided by GDI */
            if (lpbSrc = LockResource(hFont)) {
                lmemcpy((LPSTR) lpdf, lpbSrc + cbLeader, cb);
                GlobalUnlock(hFont);
                FreeResource(hFont);
            } else {
                DBMSG1(("LockResource() failed in LoadFont()!\n"));
                return 0;
            }
        }
    }


    if (lpdf) {
        /* Append the font-extra structure to the font */
        lpfx = (LPFX) (((LPSTR) lpdf) + cb);
        lpfx->dfFont = lpdf->dfDriverInfo - cbLeader;
        lpdf->dfDriverInfo = cb;

        /* #ifdef NOV18 */

        /* Save the unscaled average width */
        lpfx->fxUnscaledAvgWidth = lpdf->dfAvgWidth;

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

        /* djm 12/20/87 begin */
        lpfx->noTranslate = FALSE;
        if (soft) {
            if (lpdf->dfCharSet == NO_TRANSLATE_CHARSET) {
                lpdf->dfCharSet = ANSI_CHARSET;
                lpfx->noTranslate = TRUE;
            } else {
                if ((lpdf->dfPitchAndFamily & 0x0f0) == FF_DECORATIVE)
                    lpfx->noTranslate = TRUE;
            }
        } else { /* check resident fonts for Symbol & Zapf Dingbats */
            if ((lpdf->dfPitchAndFamily & 0x0f0) == FF_DECORATIVE ||
                    !lstrcmpi("Symbol", (LPBYTE)lpdf + lpdf->dfFace)  )
                // we just reclassified Symbol as a Roman font.
                // but its a roman font that doesn't need translation.

                lpfx->noTranslate = TRUE;
        }
        /* djm 12/20/87 end */
    }

    DBMSG1(("<LoadFont(): size=%d\n", cb + sizeof(FX)));

    return cb + sizeof(FX);
}

/**********************************************************
 * Name: ScaleFont()
 *
 * Action: This funtion scales all width and height values
 *      in the FONTINFO record to match the specified
 *      font height and width.  Note that the values
 *      in a default FONTINFO record are based on a
 *      scale from 0 to EM (defined in globals.h).
 *
 ************************************************************/

void PASCAL ScaleFont(lpdv, lpdf, cxFont, cyFont)
LPDV    lpdv;        /* Far ptr to the device descriptor */
LPFONTINFO lpdf;     /* Far ptr to the extended device font */
int    cxFont;        /* The horizontal scale factor */
int    cyFont;        /* The vertical scale factor */
{
    int    sx;
    LPFX    lpfx;


    DBMSG(("ScaleFont(): x:%d y:%d\n", cxFont, cyFont));

#ifdef INTERNAL_LEADING

    if (cyFont < 0) {

        // negative height request:
        //     use the typographic height (Em square) of the font

        cyFont = -cyFont;    // just flip the sign

    } else {

        // positive height request:
        //     correct for the internal leading
        // this will give a smaller font

        cyFont = Scale(cyFont, EM, lpdf->dfInternalLeading + EM);
    }

#else
    /* Negative font height means exclude internal leading */
    if (cyFont < 0)
        cyFont = -cyFont;

#endif
    /* Default to a 10 point font */
    if (cyFont == 0)
        cyFont = Scale(10, lpdv->iRes, 72);

    if (cxFont == 0) {
        /*    Default case; scale x the same amount we scale y */
        cxFont = Scale(lpdf->dfAvgWidth, cyFont, EM);
        sx = cyFont;
    } else {
        /* The app requested a specific average character width, so we
         * determine the correct scale factor to use in order to acheive
         * the desired average width.
         */
        if (cxFont < 0) 
            cxFont = -cxFont;
        sx = Scale(cxFont, EM, lpdf->dfAvgWidth);

        if(GetAppCompatFlags(0) & GACF_30AVGWIDTH)
        {   // Turbo Tax   hack
            int  threshold;

            threshold = Scale(11, lpdv->iRes, 300);

            if(cxFont < threshold)
                sx = Scale(threshold, EM, lpdf->dfAvgWidth);
            else 
            {
                sx = sx * 7 / 8 ;  //  compensate for smaller dfAvgWidth
            }
        }
    }

    /* Fill in the remaining structures using the x and y scale
     * factors we just computed. */

    // set the point size for computations in SetLeading()

    lpdf->dfPoints = Scale(cyFont, 72, lpdv->iRes);

    /*    Set up internal and external leading */
    SetLeading(lpdf, cyFont, lpdv->iRes);

    lpdf->dfPixHeight = cyFont + lpdf->dfInternalLeading;
    lpdf->dfPixWidth  = cxFont;
    lpdf->dfVertRes   = lpdv->iRes;
    lpdf->dfHorizRes  = lpdv->iRes;
    lpdf->dfAscent    = Scale(cyFont, lpdf->dfAscent, EM);
    lpdf->dfAvgWidth  = Scale(sx, lpdf->dfAvgWidth, EM);
    lpdf->dfMaxWidth  = Scale(sx, lpdf->dfMaxWidth, EM);

    /* Save the scale factors for output */
    if (lpdf->dfDriverInfo) {
        lpfx = (LPFX) (((LPSTR)lpdf) + lpdf->dfDriverInfo);
        lpfx->sx = sx;
        lpfx->sy = cyFont;
    }

    DBMSG(("  dfPoints=%d\n", lpdf->dfPoints));
    DBMSG(("  dfPixHeight=%d\n", lpdf->dfPixHeight));
    DBMSG(("  dfAscent=%d\n", lpdf->dfAscent));
    DBMSG(("  dfExternalLeading=%d\n", lpdf->dfExternalLeading));
    DBMSG(("  dfInternalLeading=%d\n", lpdf->dfInternalLeading));
    DBMSG(("  dfAvgWidth=%d\n", lpdf->dfAvgWidth));
}

/**************************************************************
* Name: InfoToStruct()
*
* Action: Convert the physical font info (LPDF) structure
*      to one of the three GDI structures: a) LOGFONT,
*      b) TEXTXFORM, c) TEXTMETRIC.
*
*
* Note: Isn't it amazing that GDI need three different structures
*    to contain basically the same information!
*
******************************************************************/

void PASCAL InfoToStruct(lpdf, iStyle, lpb)
LPFONTINFO lpdf;    /* Far ptr to the device font metrics */
short    iStyle;        /* The conversion style */
LPSTR    lpb;        /* Far ptr to the output structure */
{
    FONTINFO df;

    df = *lpdf;    /* allows use of ES for lpb */

    switch (iStyle) {
    case TO_TEXTXFORM:

        ((LPTEXTXFORM)lpb)->ftHeight = df.dfPixHeight;
        ((LPTEXTXFORM)lpb)->ftWidth = df.dfAvgWidth;
        ((LPTEXTXFORM)lpb)->ftWeight = df.dfWeight;

        ((LPTEXTXFORM)lpb)->ftItalic = df.dfItalic;
        ((LPTEXTXFORM)lpb)->ftUnderline = df.dfUnderline;
        ((LPTEXTXFORM)lpb)->ftStrikeOut = df.dfStrikeOut;

        ((LPTEXTXFORM)lpb)->ftEscapement = 0;
        ((LPTEXTXFORM)lpb)->ftOrientation = 0;
        ((LPTEXTXFORM)lpb)->ftAccelerator = 0;

        ((LPTEXTXFORM)lpb)->ftOverhang = OVERHANG;    /* 0 */
        ((LPTEXTXFORM)lpb)->ftClipPrecision = CLIP_CHARACTER_PRECIS; /* 1 */
        ((LPTEXTXFORM)lpb)->ftOutPrecision = OUT_CHARACTER_PRECIS; /* 2 */
        break;

    case TO_LOGFONT:
        ((LPLOGFONT)lpb)->lfHeight = df.dfPixHeight;
        ((LPLOGFONT)lpb)->lfWidth =  df.dfAvgWidth;
        ((LPLOGFONT)lpb)->lfEscapement = 0;
        ((LPLOGFONT)lpb)->lfOrientation = 0;
        ((LPLOGFONT)lpb)->lfItalic = df.dfItalic;
        ((LPLOGFONT)lpb)->lfWeight = df.dfWeight;
        ((LPLOGFONT)lpb)->lfUnderline = df.dfUnderline;
        ((LPLOGFONT)lpb)->lfStrikeOut = df.dfStrikeOut;
        ((LPLOGFONT)lpb)->lfOutPrecision = OUT_CHARACTER_PRECIS;
        ((LPLOGFONT)lpb)->lfClipPrecision = CLIP_CHARACTER_PRECIS;
        ((LPLOGFONT)lpb)->lfQuality = PROOF_QUALITY;
        ((LPLOGFONT)lpb)->lfPitchAndFamily = (BYTE)(df.dfPitchAndFamily + 1);
        ((LPLOGFONT)lpb)->lfFaceName[0] = 0;

        ((LPLOGFONT)lpb)->lfCharSet = (df.dfCharSet == NO_TRANSLATE_CHARSET) ?
            (BYTE)ANSI_CHARSET : (BYTE)df.dfCharSet;

        break;

    case TO_TEXTMETRIC:
        ((LPTEXTMETRIC)lpb)->tmHeight = df.dfPixHeight;
        ((LPTEXTMETRIC)lpb)->tmAscent = df.dfAscent;
        ((LPTEXTMETRIC)lpb)->tmDescent = df.dfPixHeight - df.dfAscent;
        ((LPTEXTMETRIC)lpb)->tmInternalLeading = df.dfInternalLeading;
        ((LPTEXTMETRIC)lpb)->tmExternalLeading = df.dfExternalLeading;
        ((LPTEXTMETRIC)lpb)->tmAveCharWidth = df.dfAvgWidth;
        ((LPTEXTMETRIC)lpb)->tmMaxCharWidth = df.dfMaxWidth;
        ((LPTEXTMETRIC)lpb)->tmItalic = df.dfItalic;
        ((LPTEXTMETRIC)lpb)->tmWeight = df.dfWeight;
        ((LPTEXTMETRIC)lpb)->tmUnderlined = 0;
        ((LPTEXTMETRIC)lpb)->tmStruckOut = 0;
        ((LPTEXTMETRIC)lpb)->tmFirstChar = df.dfFirstChar;
        ((LPTEXTMETRIC)lpb)->tmLastChar = df.dfLastChar;
        ((LPTEXTMETRIC)lpb)->tmDefaultChar = df.dfDefaultChar + df.dfFirstChar;
        ((LPTEXTMETRIC)lpb)->tmBreakChar = df.dfBreakChar + df.dfFirstChar;
        ((LPTEXTMETRIC)lpb)->tmPitchAndFamily = df.dfPitchAndFamily;
        ((LPTEXTMETRIC)lpb)->tmOverhang = OVERHANG;
        ((LPTEXTMETRIC)lpb)->tmDigitizedAspectX = df.dfHorizRes;
        ((LPTEXTMETRIC)lpb)->tmDigitizedAspectY = df.dfVertRes;

        ((LPTEXTMETRIC)lpb)->tmCharSet = (df.dfCharSet == NO_TRANSLATE_CHARSET) ?
            (BYTE)ANSI_CHARSET : (BYTE)df.dfCharSet;

        DBMSG4(("dfPoints %d\n", df.dfPoints));
        DBMSG4(("  tmHeight          %d\n",((LPTEXTMETRIC)lpb)->tmHeight));
        DBMSG4(("  tmAscent          %d\n",((LPTEXTMETRIC)lpb)->tmAscent));
        DBMSG4(("  tmDescent         %d\n",((LPTEXTMETRIC)lpb)->tmDescent));
        DBMSG4(("  tmInternalLeading %d\n",((LPTEXTMETRIC)lpb)->tmInternalLeading));
        DBMSG4(("  tmExternalLeading %d\n",((LPTEXTMETRIC)lpb)->tmExternalLeading));

        break;
    }
}


/***************************************************************
* Name: SetLeading()
*
* Action: Set the leading (inter-line spacing) value for a
*      text font.  These values are based on some magic
*      numbers supplied by Chris Larson who got them from
*      somewhere in our publications department.  Note that
*      these values are not a linear function of the font
*      size.
*
*      Prior to Chris Larson's mandate, the leading values
*      were computed as 19.5% of the font height which produced
*      output which closely matched the Macintosh output on the
*      LaserWriter.
*
*      Since Chris only had values for the Times-Roman, Helvetica,
*      and Courier fonts, the LaserWriter fonts are broken out into
*      four classes: Roman, Swiss, Modern, and other.  Each family
*      (hopefully) has the same type of descenders, etc. so that
*      the external leading is the same for all Roman fonts, etc.
*
********************************************************************
*/
void PASCAL SetLeading(lpdf, cyFont, iRes)
LPFONTINFO lpdf;    /* Far ptr to the device font */
int    cyFont;        /* The font height in dots */
int    iRes;        /* The device resolution */
{
    register int    ptLeadSuggest;    // suggest leading in points

    // the max amount of leading is stored in dfInternalLeading
    // compute the suggested leading based on the type of font
    // first look at the family

    switch (lpdf->dfPitchAndFamily & 0x0f0) {

    case FF_ROMAN:        // Tms Rmn  type fonts
        ptLeadSuggest = 2;
        break;

    case FF_SWISS:        // Helv    type fonts

        if (lpdf->dfPoints <= 12) 
            ptLeadSuggest = 2;
        else if (lpdf->dfPoints < 14) 
            ptLeadSuggest = 3;
        else 
            ptLeadSuggest = 4;
        break;

    default:
        // default to 19.6%
        ptLeadSuggest = Scale(lpdf->dfPoints, 196, EM);
        break;
    }

    // for all fixed pitched fonts (Courier) use no leading

    if (lpdf->dfPitchAndFamily & 0x01) {

        // variable pitch

        // scale to device units

        lpdf->dfInternalLeading = Scale(lpdf->dfInternalLeading, cyFont, EM);
    } else {

        // fixed pitch

        lpdf->dfInternalLeading = 0;
        ptLeadSuggest = 0;
    }

#ifdef INTERNAL_LEADING

    // here we make sure the internal and external leading sum
    // to the recomended leading but don't allow external leading
    // to become negative.

    lpdf->dfExternalLeading = max(0, Scale(ptLeadSuggest, iRes, 72) - lpdf->dfInternalLeading);

#else
    lpdf->dfExternalLeading = Scale(ptLeadSuggest, iRes, 72);
#endif

}

/****************************************************************************
 * Name: EnumDFonts()
 *
 * Action: This routine is used to enumerate the fonts available
 *      on the device. For each font, the callback function
 *      is called with the information for that font.  The
 *      callback function is called until there are no more
 *      fonts to enumerate or until the callback function
 *      returns zero.
 *
 * Note: All fonts are enumerated in a reasonable height (such
 *    as 12 points so that dumb apps that don't realize that
 *    they we can scale text will default to something reasonable.
 *
 ***************************************************************************/

int FAR PASCAL EnumDFonts(lpdv, lszFace,  lpfn, lpb)
LPDV    lpdv;        /* ptr to the device descriptor */
LPSTR    lszFace;    /* ptr to the facename string (may be NULL) */
FARPROC lpfn;        /* ptr to the callback function */
LPSTR    lpb;        /* ptr to the client data (passed to callback) */
{
    WORD wFlags;

    wFlags = ENUM_INTERNAL | ENUM_SOFTFONTS;
    if (bTTEnabled)
        wFlags |= ENUM_TRUETYPE;

    return EnumFonts(lpdv, lszFace, lpfn, lpb, wFlags);
}

int FAR PASCAL EnumFonts(LPDV lpdv, LPSTR lszFace, 
                         int (FAR PASCAL *lpfn)(LPLOGFONT,LPTEXTMETRIC,int,LPSTR),
                         LPSTR lpb, WORD wFlags)
{
    /* add some arbitrary amount to make sure softfonts fit */

    char rgbFont[sizeof(FONTINFO) + 200];

    LOGFONT lf;
    TEXTMETRIC tm;
    LPFONTINFO lpdf;
    int    idf;        /* The font index */
    int    cb;
    LPSTR    lpbSrc;
    LPSTR    lpbDst;
    HPBYTE   lpbDir;
    short    cFonts,cSoftFonts;        /* count fonts */
    int    i;
    char    szLastFace[LF_FACESIZE];/* name of last font enumerated */
    int    iStatus = 1;        /* status=1 means failure! */
    int    nFontType;
    WORD   offset;

    DBMSG((">EnumDFont(): facename=%ls\n", lszFace));

    /* Enumerate TrueType fonts */
    if (wFlags & ENUM_TRUETYPE)
    iStatus = EngineEnumerateFont(lszFace, lpfn, (DWORD)lpb);
    wFlags &= ~ENUM_TRUETYPE;

    // I just need a pointer to a large memory region to
    // obtain the offset to dfFace - just overlook this 
    // bit of desparate insanity.
    lpdf = (LPFONTINFO)lpdv;
    offset = (WORD)&lpdf->dfFace - (WORD)lpdf;
    
    /* if there's nothing left to enumerate return */
    if (!iStatus  ||  !wFlags)
        return iStatus;

    if ( !(lpbDir = LockFontDir(lpdv->iPrinter)) ) {

        /* Couldn't get font directory! */

        DBMSG(("<EnumDFont(): lock font failed\n"));
    return iStatus;
    }

    /* Scan through the fonts in the font directory */

    cFonts = *(LPSHORT)lpbDir;

    if ((wFlags & (ENUM_INTERNAL | ENUM_SOFTFONTS)) 
            != (ENUM_INTERNAL | ENUM_SOFTFONTS)) {

        cSoftFonts = ((HPSHORT)lpbDir)[FDIR_CSOFTFONTS];
        cFonts -= cSoftFonts;

        if (!(wFlags & ENUM_INTERNAL)) {

            /* skip over resident fonts */
            lpbDir += sizeof(short);
            while (cFonts--) {
            cb = *(HPSHORT)lpbDir;    /* get the length of this dir entry */
            lpbDir += cb;        /* advance to the next dir entry */
            }
            lpbDir -= sizeof(short);    /* offsets + sizeof(short) below */

            cFonts = cSoftFonts;
        }

    }

    lpbDir += sizeof(short);    /* move past the font count */

    DBMSG((" EnumDFont(): cFonts=%d\n", cFonts));

    /* we will keep track of the last face name enumerated so that if
    * lszFace == NULL we only enumerate one font of each face name */

    *szLastFace = 0;    /* make last face name invalid */

    for (idf = 0; idf < cFonts; ++idf) {

        HPBYTE  lpSrcPtr, lpFaceNamePtr;
        BYTE  FaceName[40];

        cb = *(HPSHORT)lpbDir;    /* get the length of this dir entry */


        DBMSG1(("entry length: cb = %d\n", cb));


        lpSrcPtr = lpbDir + 4;        /* point to the FONTINFO struct */
        lpdf = (LPDF)(lpbDir + 4);    /* point to the FONTINFO struct */

        DBMSG1(("font name: %ls\n", (LPSTR)lpdf + *(LPSHORT)(lpbDir + 2)));
        DBMSG1(("dfFace:%ls\n", (LPSTR)(lpdf) + lpdf->dfFace));
        DBMSG1(("dfDevice:%ls\n", (LPSTR)(lpdf) + lpdf->dfDevice));

        lpbDir += cb;        /* advance to the next dir entry */

        /* enumerate a font if:
        * lszFace == NULL and we haven't enumed this face yet
        * OR
        * *lszFace == this font's face */


        lpFaceNamePtr =  lpSrcPtr + *(HPSHORT)(lpSrcPtr + offset);

        for(i = 0 ; lpFaceNamePtr[i] ; i++)
            FaceName[i] = lpFaceNamePtr[i];
        FaceName[i] = '\0';

        if ((!lszFace && lstrcmpi(szLastFace, (LPSTR)FaceName)) ||
        (lszFace && !lstrcmpi(lszFace, (LPSTR)FaceName))) {

            ASSERT((cb-4) < sizeof(rgbFont));

            for(i = 0 ; i < cb - 4 ; i++)
                rgbFont[i] = lpSrcPtr[i];
            lpdf = (LPFONTINFO) rgbFont;

            ScaleFont(lpdv, lpdf, 0, 0);
            InfoToStruct(lpdf, TO_LOGFONT, (LPSTR)&lf);

            /* Copy the face name to the logical font limited by
            * LF_FACESIZE */

            lpbSrc = ((LPSTR)lpdf) + lpdf->dfFace;
            lpbDst = (LPSTR)lf.lfFaceName;

            for (i = LF_FACESIZE - 1; i > 0 && *lpbSrc; --i) 
                *lpbDst++ = *lpbSrc++;
            *lpbDst = 0;

            /* save this face name (so we don't do it again) */
            lstrcpy(szLastFace, (LPSTR)lf.lfFaceName);

            InfoToStruct(lpdf, TO_TEXTMETRIC, (LPSTR)&tm);

            /* determine font type to return */
            nFontType = DEVICE_FONTTYPE;
            if ((wFlags & ENUM_SETDEVTTBIT) && (lpdf->dfType & TYPE_TTRESIDENT))
               nFontType |= TYPE_TRUETYPE;

            if (!(iStatus = (*lpfn)((LPLOGFONT)&lf, (LPTEXTMETRIC)&tm,
                                    nFontType, lpb))) {
                break;
            }
        }
    }

    UnlockFontDir(lpdv->iPrinter);
    DBMSG(("<EnumDFont(): iStatus=%d\n", iStatus));

    return iStatus;
}

WORD PASCAL GetTTFontID(LPDV lpdv, LPSTR lpszCacheKey)
{
   HANDLE hList;
   ATOM aFace;

   /* see if the atom already exists and if so we're done! */
   aFace = GlobalFindAtom(lpszCacheKey);
   if (aFace)
      return aFace;

   /* nope, need to add to list... */
   if (!lpdv->sTTFontList) {
      hList = GlobalAlloc(GDLLHND, 128 * sizeof(WORD));
      if (!hList)
         return 0;
      lpdv->lpTTFontList = (WORD FAR *)GlobalLock(hList);
      lpdv->sTTFontList = 128;
   } else if (lpdv->cTTFontList == lpdv->sTTFontList) {
      HANDLE hNewList;

      /* grow buffer */
      hList = (HANDLE)GlobalHandle(HIWORD((DWORD)lpdv->lpTTFontList));
      GlobalUnlock(hList);
      hNewList = GlobalReAlloc(hList, (lpdv->sTTFontList + 128) * sizeof(WORD),
                               GMEM_MOVEABLE);
      if (!hNewList)
         return 0;
      lpdv->lpTTFontList = (WORD FAR *)GlobalLock(hNewList);
      lpdv->sTTFontList += 128;
   }

   /* get an atom for the string */
   aFace = GlobalAddAtom(lpszCacheKey);
   if (!aFace)
      return 0;

   /* add atom to end of list */
   lpdv->lpTTFontList[lpdv->cTTFontList++] = aFace;

   /* return atom # as id */
   return aFace;
}

void FAR PASCAL FreeTTFontTable(LPDV lpdv)
{
        int i;
        HANDLE h;

        for (i = 0; i < lpdv->cTTFontList; ++i) 
                GlobalDeleteAtom(lpdv->lpTTFontList[i]);
        h = HIWORD((DWORD)lpdv->lpTTFontList);
        if (h) {
            h = (HANDLE)GlobalHandle(h);
            GlobalUnlock(h);
            GlobalFree(h);
        }
        lpdv->cTTFontList = lpdv->sTTFontList = 0;
        lpdv->lpTTFontList = 0;
}

