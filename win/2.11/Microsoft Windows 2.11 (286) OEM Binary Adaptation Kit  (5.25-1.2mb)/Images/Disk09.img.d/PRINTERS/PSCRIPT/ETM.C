/*********************************************************************
* This module of the afm compiler generates the extended text metrics
* portion of the PFM file.
*
**********************************************************************
*/
#include "pfm.h"


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
short etmUpperCaseDecent;
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


/* Convert from PostScript to extended text metrics */
AfmToEtm()
    {
    etmPointSize = 12 * 20;	/* Nominal point size = 12 */
    etmOrientation = 0;
    etmMasterHeight = 1000;
    etmMinScale = 3;
    etmMaxScale = 1000;
    etmMasterUnits = 1000;
    etmCapHeight = afm.rgcm['H'].rc.top;
    etmXHeight = afm.rgcm['x'].rc.top;
    etmLowerCaseAscent =  afm.rgcm['d'].rc.top;
    etmUpperCaseDecent = - afm.rgcm['p'].rc.bottom;
    etmSlant = afm.iItalicAngle;
    etmSuperScript = -500;
    etmSubScript = 250;
    etmSuperScriptSize = 500;
    etmSubScriptSize = 500;
    etmUnderlineOffset = afm.ulOffset;
    etmUnderlineWidth = afm.ulThick;
    etmDoubleUpperUnderlineOffset = afm.ulOffset/2;
    etmDoubleLowerUnderlineOffset = afm.ulOffset;
    etmDoubleUpperUnderlineWidth = afm.ulThick / 2;
    etmDoubleLowerUnderlineWidth = afm.ulThick / 2;
    etmStrikeOutOffset = 500;
    etmStrikeOutWidth = 100;
    etmNKernPairs = afm.kp.cPairs;
    etmNKernTracks = 0;
    }


PutEtm(iFont)
int iFont;
    {
    AfmToEtm();
    PutWord(etmSize);

    PutWord(etmPointSize);
    PutWord(etmOrientation);
    PutWord(etmMasterHeight);
    PutWord(etmMinScale);
    PutWord(etmMaxScale);
    PutWord(etmMasterUnits);
    PutWord(etmCapHeight);
    PutWord(etmXHeight);
    PutWord(etmLowerCaseAscent);
    PutWord(etmUpperCaseDecent);
    PutWord(etmSlant);
    PutWord(etmSuperScript);
    PutWord(etmSubScript);
    PutWord(etmSuperScriptSize);
    PutWord(etmSubScriptSize);
    PutWord(etmUnderlineOffset);
    PutWord(etmUnderlineWidth);
    PutWord(etmDoubleUpperUnderlineOffset);
    PutWord(etmDoubleLowerUnderlineOffset);
    PutWord(etmDoubleUpperUnderlineWidth);
    PutWord(etmDoubleLowerUnderlineWidth);
    PutWord(etmStrikeOutOffset);
    PutWord(etmStrikeOutWidth);
    PutWord(etmNKernPairs);
    PutWord(etmNKernTracks);
    }
