/************************************************************

   PROGRAM: SREC.C

   PURPOSE: Generic template for a custom recognizer DLL.

   COMMENTS:

      This code is a skeleton program containing all necessary
      functionality for a simple recognizer.  The recognizer is
      a DLL loaded at runtime by the pen-aware application.
      This particular recognizer is used by the PENAPP sample
      application.
      
      This sample recognizer inputs a single stroke of data points.
      Taking the beginning and ending points of the stroke, SREC
      calculates the nearest compass direction of the line formed
      by these endpoints.

      SREC then fills out the symbol graph (passed through the
      lParam structure of the application's window procedure on
      the WM_RCRESULT message) using the following special codes:

         Value       Direction
         -------     -----------
         syvEast     right
         syvSouth    down
         syvWest     left
         syvNorth    up
         syvDot      point

      Application Requirements:

         Any application choosing to use this recognizer (SREC) must
         set the following RC struct item:

         1) lPcm = PCM_PENUP | any other flags.


   RULES FOR ALL RECOGNIZERS:

      All recognizers must take heed to the following structure
      items in the RC structure:

         Required RC struct items:

            1) lRcOptions...     RCO_SAVEEXTRADATA

            2) wTimeOut

      The following struct items may be ignored:

         Optional RC struct items:

            1) lRcOptions...  RCO_SINGLE (recognizer specific)

            2) GUIDE

            3) wRcOrient (recognizer specific, only required for orientation
               dependent glyphs)

            4) wRcDirect

            5) wRcPreferences

************************************************************/

#define NOCOMM
#include <windows.h>
#define  DEFPENMOD      /* for PENINFO struct */
#include <penwin.h>
#include <penwoem.h>
#include "main.h"

/******** Prototypes *********/

REC DoRecognition (LPRC lprc, HPENDATA hpendata, LPRCRESULT lprcresult);
VOID CalcNearestDirection (LPPOINT lppointEnds);
LPSTR lstrncpy (LPSTR lpszA, LPSTR lpszB, int cch);

/******** Macros *********/

#define  ABS(n)   ((n) < 0 ? -(n) : n)

/******** Constants *********/

#define cpntMax   128      /* Max count of points for point buffer */

#define szID   "US Compass direction, single-stroke"
#define szMsg1 "This application requires Standard or Enhanced Mode Windows."

/******** Module Variables *********/

static SYE  syeGlobal;


/******** Code *********/

/*----------------------------------------------------------
Purpose: Main DLL function
Returns: TRUE
*/
int FAR PASCAL LibMain(
   HANDLE hinst,        // Instance handle
   WORD wDataSeg,       // Data segment
   WORD cbHeapSize,     // Size of heap
   LPSTR lpszCmdLine)   // Command line
   {
   Unused(hinst); 
   Unused(wDataSeg);
   Unused(lpszCmdLine);

   if (cbHeapSize != 0)
      {
      UnlockData(0);
      }

   return TRUE;
   }


/*----------------------------------------------------------
Purpose: DLL termination function
Returns: 1
*/
WORD FAR PASCAL WEP(int nParam)
   {
   Unused(nParam);

   return 1;
   }


/*----------------------------------------------------------
Purpose: Initializes recognizer
Returns: TRUE on success, FAIL on failure
Comment: DLL receives this call when an application calls
         InstallRecognizer.
*/
BOOL WINAPI InitRecognizer(LPRC lprc)     /* Ptr to RC struct */
   {
   Unused(lprc);

   /* We don't do any initializing for this recognizer.  But if we
   ** wanted to, we would do it here.
   */
   return TRUE;   
   }


/*----------------------------------------------------------
Purpose: Perform final unloading of recognizer
Returns: --
Cond:    DLL receives this call when an application calls
         UninstallRecognizer.
*/
VOID WINAPI CloseRecognizer(VOID)
   {
   /* We don't do anything for this recognizer.  But if we
   ** wanted to, we would do it here.
   */
   return;
   }


/*----------------------------------------------------------
Purpose: Configure recognizer for special options.
Returns: 
Cond:    DLL receives this call when the Control Panel or
         application makes a change to the recognizer
         configuration.

         This recognizer provides no custom configuration
         other than returning the identification string.
*/
UINT WINAPI ConfigRecognizer(
   UINT wConfig,     /* Configuration subfunction */
   WPARAM wParam,    /* Varies */
   LPARAM lParam)    /* Varies */
   {
   WORD  wRet = TRUE;

   Unused(wParam);

   switch (wConfig)
      {
      case WCR_CONFIGDIALOG:
         break;            

      case WCR_RCCHANGE:
         break;

      case WCR_RECOGNAME:
         lstrncpy((LPSTR)lParam, szID, wParam);
         break;
   
      case WCR_TRAIN:
         wRet = TRAIN_NONE;      /* Does not support training */
         break;

      case WCR_DEFAULT:          /* Incapable of being system default */
      case WCR_QUERY:            /* Does not support configuration dialog */
      case WCR_QUERYLANGUAGE:    /* Does not support any language */
      case WCR_TRAINMAX:
      case WCR_TRAINDIRTY:
      case WCR_TRAINCUSTOM:
      case WCR_TRAINSAVE:
         wRet = FALSE;
         break;

      case WCR_VERSION:
         wRet = 0x0103;    /* Recognizer version 3.1 */
         break;
      }
   return wRet;
   }


/*----------------------------------------------------------
Purpose: Perform context-independent training
Returns: TRUE if ink in hpendata can be trained
Cond:    --
*/
BOOL WINAPI TrainInkInternal(
   LPRC lprc,           /* Ptr to RC struct */
   HPENDATA hpendata,   /* Pendata handle */
   LPSYV lpsyv)         /* Ptr to symbol value buffer */
   {
   Unused(lprc); 
   Unused(hpendata);
   Unused(lpsyv);

   return FALSE;   /* This recognizer does not support training */ 
   }


/*----------------------------------------------------------
Purpose: Perform contextual training
Returns: TRUE if ink can be trained
Cond:    --
*/
BOOL WINAPI TrainContextInternal(
   LPRCRESULT lprcresult,     /* Ptr to RCRESULT struct */
   LPSYE lpsye,               /* Ptr to symbol elements */
   int csye,                  /* Count of symbol elements */
   LPSYC lpsyc,               /* Ptr to symbol correspondence buffer */
   int csyc)                  /* Count of SYCs */
   {
   Unused(lprcresult);
   Unused(lpsye); 
   Unused(csye);
   Unused(lpsyc); 
   Unused(csyc);
   
   return FALSE;  /* This recognizer does not support training */
   }


/*----------------------------------------------------------
Purpose: OEM recognizer function.
Returns: Various
Comment: This function receives pen stroke input through
         RecGetPenData and performs recognition.  Recognition
         results are passed in an RCRESULT struct through
         the lpFuncResults function.

         Returns one of several values available for this
         function.  See documentation for more details.

         A recognizer has the following general form:

         {
         Allocate memory to buffer results and raw data;

         while (GetPenHwData(..) == REC_OK)
            {
            Yield sometimes;
            Add points to pendata buffer
            if (overflow)
               {
               EndPenCollection(REC_OOM);
               break;
               }
            if (enough data to recognize)
               {
               Do recognition;

               Fill RCRESULT struct;
               Call lpFuncResults(..rcresult..);
               if (lpFuncResults == 0)
                  return (valid value for RecRecognize);
               }
            }

         if (still some raw data left)
            {
            Do recognition;
            Fill RCRESULT struct;
            Call lpFuncResults(..rcresult..);
            }
         Free buffer memory;

         return (valid value for RecRecognize);
         }

         For SREC, this function receives input of data
         points of one stroke and calculates the closest
         compass direction of the stroke.  

         Note that, unlike a typical recognizer, this
         recognizer collects all data points first before
         calculating nearest compass direction.

         Also this recognizer assumes the PCM_PENUP is set.
         Thus no timeout checking is done.
*/
REC WINAPI RecognizeInternal(
   LPRC lprc,                    /* Ptr to RC struct */
   LPFUNCRESULTS lpFuncResults)  /* Ptr to results function */
   {
   WORD  rgpntOem[cpntMax*MAXOEMDATAWORDS];     /* temporary buffer */
   POINT rgpnt[cpntMax];      /* actual data buffer */
   WORD  cYield   = 0;        /* yield count */
   BOOL  fSaveAll= (lprc->lRcOptions & RCO_SAVEALLDATA) != 0;
   LPVOID  lpvOem = (fSaveAll ? (LPVOID)rgpntOem : (LPVOID)NULL);
   REC      rec = REC_OK;
   HPENDATA hpendata;
   HPENDATA hpendataT = NULL;
   RCRESULT rcresult;
   STROKEINFO si;

   if ((lprc->lPcm & PCM_PENUP) == 0)
      {
      /* Did application set the PCM_PENUP as method for
      ** ending recognition?
      */
      return REC_NOPENUP;      /* Recognizer specific error */ 
      }

   /* Allocate OEM data buffer
   */
   if ( (hpendata = CreatePenData(NULL, (fSaveAll ? -1 : 0), PDTS_STANDARDSCALE, GMEM_SHARE)) == NULL)
      return REC_OOM;

   /* Data input loop
   */
   while ((rec = GetPenHwData(rgpnt, lpvOem, cpntMax, 0, &si)) == REC_OK)
      {
      if (si.cPnt != 0)
         {
         if ( (hpendataT = AddPointsPenData(hpendata, rgpnt, lpvOem, &si)) == NULL)
            {
            rec = REC_OOM;
            EndPenCollection(REC_OOM);
            break;
            }
         hpendata = hpendataT;
         }

      if ((cYield++ % 5) && (lprc->lpfnYield))   /* lpfnYield can be NULL */ 
         {
         (*lprc->lpfnYield)();       /* Yield */ 
         }
      }

   /* Copy last point.  Note that last point only really counts
   ** if rec == REC_TERMPENUP
   */
   if (hpendataT != NULL && rec == REC_TERMPENUP)
      {
      /* Normal ending of loop above.  Add the pen up stroke
      */
      if ( (hpendata = AddPointsPenData(hpendata, rgpnt, lpvOem, &si)) == NULL)
         rec = REC_OOM;
      }


   if (rec == REC_TERMPENUP)
      {
      /* Send results back to app
      */ 
      DoRecognition(lprc, hpendata, &rcresult);
      (*lpFuncResults)((LPRCRESULT) &rcresult, rec);
      }

   /* Free up memory used to save data.  If app wanted to save it, they
   ** need to make a copy or set the RCO_SAVEHPENDATA flag.
   */ 
   if (hpendata != NULL && (lprc->lRcOptions&RCO_SAVEHPENDATA)==0)
      DestroyPenData(hpendata);

   return rec;
   }



/*----------------------------------------------------------
Purpose: OEM recognizer function.  This function takes the
         given data points (lppntIn) and performs recognition.
         Also passed is any custom OEM data (lpvOEM).
         Recognition results are passed in an RCRESULT struct
         through the lpFuncResults function.
Returns: REC_NOINPUT if no data or REC_DONE if finished
Comment: For SREC, this function takes the list of data points
         of the single stroke and calculates the closest
         compass direction of the stroke. 
*/
REC WINAPI RecognizeDataInternal(
   LPRC lprc,                    /* Ptr to RC struct */
   HPENDATA hpendata,            /* Pendata handle */
   LPFUNCRESULTS lpFuncResults)  /* Ptr to results function */
   {
   RCRESULT rcresult;
   REC rec;

   /* Check for empty buffer
   */
   rec =  DoRecognition(lprc, hpendata, &rcresult);

   if (rec == REC_OK)
      (*lpFuncResults)((LPRCRESULT) &rcresult, rec = REC_DONE);

   return rec;
   }


/*----------------------------------------------------------
Purpose: Private function sharing common code between
         RecognizeInternal and RecognizeDataInternal.  This
         fills in the RCRESULT structure.
Returns: lprcresult struct is updated
Cond:    --
*/
REC DoRecognition(
   LPRC lprc,              /* Ptr to RC struct */
   HPENDATA hpendata,      /* Pendata handle */
   LPRCRESULT lprcresult)  /* Ptr to RCRESULT */
   {
   LPPENDATA   lppendata;
   LPPOINT     lppoint;
   POINT    rgpntEnds[2];
   STROKEINFO  si;

   /* Check for empty buffer
   */
   if (hpendata == 0 || (lppendata = BeginEnumStrokes(hpendata)) == NULL)
      return REC_NOINPUT;

   /* Grab endpoints from first stroke
   */
   GetPenDataStroke(lppendata, 0, &lppoint, NULL, &si);
   rgpntEnds[0] = lppoint[0];
   rgpntEnds[1] = lppoint[si.cPnt-1];
   EndEnumStrokes(hpendata);

   /* Build symbol graph
   */
   lprcresult->syg.rgpntHotSpots[0] = rgpntEnds[0];   /* Set hotspots to endpoints */
   lprcresult->syg.rgpntHotSpots[1] = rgpntEnds[1];
   lprcresult->syg.cHotSpot = 2;

   lprcresult->syg.lRecogVal = 0L;                    /* Not used */

   CalcNearestDirection(&rgpntEnds[0]);
   lprcresult->syg.lpsye = &syeGlobal;
   lprcresult->syg.cSye = 1;

   /* Fill data struct
   */
   lprcresult->wResultsType   = RCRT_PRIVATE;      /* Recognizer-specific result */
   lprcresult->nBaseLine      = 0;
   lprcresult->nMidLine       = 0;
   lprcresult->hSyv        = 0;

   lprcresult->hpendata    = hpendata;
   lprcresult->lprc        = lprc;
   return REC_OK;
   }


/*----------------------------------------------------------
Purpose: Private function to calculate the closest compass
         direction of the line defined by pntFirst and pntLast.
Returns: 
Comment: The created symbol element is composed of a
         recognizer-specific symbol value (SYV), confidence level,
         and recognizer value.

         This algorithm takes no tablet aspect ratio into account.
*/
VOID CalcNearestDirection(LPPOINT lppointEnds)  /* Ptr to buffer of 2 POINT structs */
   {
   int    dx;
   int    dy;
   BOOL fIsEastward;
   BOOL fIsSouthward;
   BOOL fIsHoriz;

   dx = (lppointEnds+1)->x - lppointEnds->x;
   dy = (lppointEnds+1)->y - lppointEnds->y;

   fIsEastward = dx > 0 ? TRUE : FALSE;
   fIsSouthward = dy > 0 ? TRUE : FALSE;
   fIsHoriz = ABS(dx) > ABS(dy) ? TRUE : FALSE;

   if (fIsHoriz)
      {
      syeGlobal.syv = fIsEastward ? syvEast : syvWest;
      }
   else
      {
      syeGlobal.syv = fIsSouthward ? syvSouth : syvNorth;
      }
   if (dx == 0 && dy == 0)
      {
      syeGlobal.syv = syvDot;
      }

   syeGlobal.cl = 100;        /* Set confidence level */
   syeGlobal.lRecogVal = 0L;  /* Not used */
   }


/*----------------------------------------------------------
Purpose: Far pointer version of strncpy
Returns: Ptr to copied string
*/
LPSTR lstrncpy(
   LPSTR lpszA,   /* Dest string */
   LPSTR lpszB,   /* Source string */
   int cch)       /* Max count of characters to copy */
   {
   LPSTR lpszSav = lpszA;

   while (cch-- > 0 && (*lpszA++ = *lpszB++))
      ;
   *(--lpszA) = NULL;
   return lpszSav;
   }
