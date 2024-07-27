/* IDSTRs and IDPMTs are in 5 different groups,
   the high byte is for masking, see MB_ERRxxxx definitions */

#define MB_ERRMASK	       0xf000
#define MB_ERRASTR	       0x1000
#define MB_ERREXCL	       0x2000
#define MB_ERRQUES	       0x3000
#define MB_ERRHAND	       0x4000

#ifndef NOSTRUNDO
/* NONERROR group, from 0x0001 -- 0x0fff */
/*    Menu and Undo strings */
#define IDSTRUndoBase	       0x0001
#define IDSTRUndoEdit	       0x0002
#define IDSTRUndoLook	       0x0003
#define IDSTRUndoTyping        0x0004
#define IDSTRShowRuler	       0x0005
#define IDSTRHideRuler	       0x0006
#define IDSTRAbout	       0x0007

    /* UNDO menu string lengths, including terminator */
#define cchSzUndo   (25)

#endif	/* NOSTRUNDO */




#define IDSTRHELPF	       0x000b

#define IDSTRChangeSel	       0x000c
#define IDSTRChangeAll	       0x000d

#define IDSTRChPage	       0x000e

#define IDSTROn 	       0x0013
#define IDSTRReplaceFile       0x0016
#define IDSTRChars	       0x0017
#define IDSTRSearching	       0x0018

#define IDS_MERGE1	       0x0019

/***** FOLLOWING MESSAGES ARE "*" LEVEL MESSAGES */
/* MB_ERRASTR group, from 0x1000 -- 0x1fff */

#define IDPMTSearchDone        0x1000
#define IDPMTNotFound	       0x1001
#define IDPMTNoReplace	       0x1002
#define IDPMTCancelSearch      0x1003
#define IDPMTDelPicture        0x1004

/***** FOLLOWING MESSAGES ARE "?" LEVEL MESSAGES */
/* MB_ERRQUES group, from 0x3000 -- 0x3fff */

#define IDPMTAddFont	   0x3000
#define IDPMTTruncateSz    0x3001

/***** FOLLOWING MESSAGES ARE "!" LEVEL MESSAGES */
/* MB_ERREXCL group, from 0x2000 -- 0x2fff */

#ifndef NOSTRERRORS
#define IDPMTBadFile	       0x2006
#define IDPMTCantOpen	       0x2019
#define IDPMTDirtyDoc	       0x201a
#define IDPMTCantRunM	       0x201b
#define IDPMTCantRunD	       0x201d
#define IDPMTCantRunF	       0x2021
#define IDPMTNoPath	       0x201c
#define IDPMTNoAvail	       0x201f
#define IDPMTReadOnly	       0x2020
#define IDPMTRenameFail        0x2023

/*    Dialog field errors */

#define IDPMTNoPage	       0x2007
#define IDPMTNOTNUM	       0x2008
#define IDPMTBFS	       0x2009
#define IDPMTNPI	       0x200a
#define IDPMTNOTDXA	       0x200b
#define IDPMTNPDXA	       0x200c
#define IDPMTMTL	       0x200d
#define IDPMTBadFilename       0x200e

#define IDPMT2Complex	       0x200f
#define IDPMTBadMove	       0x2010
#define IDPMTNoHelpFile        0x2011
#define IDPMTDFULL	       0x2012
#define IDPMTPRFAIL	       0x2013
#define IDPMTWPR	       0x2015
#define IDPMTDRFULL	       0x2016
#define IDPMTClipLarge	       0x2017
#define IDPMTClipQuest	       0x201e
#define IDPMTBadPrinter        0x2018
#define IDPMTCantPrint	       0x2022
#define IDPMTPrPictErr	       0x2024
#define IDPMTPrDiskErr	       0x2025
#ifdef KANJI
#define IDPMTLargePaste        0x2026
#define IDPMTLongEdit	       0x2027
#endif /* KANJI */


/***** FOLLOWING MESSAGES ARE "<hand>" LEVEL MESSAGES */
/* MB_ERRHAND group, from 0x4000 -- 0x4fff */

#define IDPMTSDE	       0x4000
#define IDPMTSDN	       0x4001
#define IDPMTNoMemory	       0x4002
#define IDPMTSFER	       0x4003
#define IDPMTMEM	       0x4004
#define IDPMTWinFailure        0x4005
#endif	    /* NOSTRERRORS */
