/* facilitate compilation accross modules without losing any optimizations
   of the engine */

#define WINDOWS

#include <windows.h>  // user interface stuff follows
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <malloc.h>
#include <memory.h>
#include <conio.h>
#include <time.h>
#include <ctype.h>
#include <search.h>
#include <stdarg.h>

#ifndef WIN32
#include <ver.h>
#else
#include <winver.h>
#endif

extern struct   rCheckConfigRec rConfig;
extern class    CMoveList CMoves;
extern struct   rGameStateRec rGameState;


extern unsigned int _charmax;  // This is because \mbudev\limits.h has 
                        // been hacked by dgreenspoon


//---[ Local header user interface dependencies ]-----------------------------------------

#include "check.h"
#include "dlg.h"
#include "wcheck.h"
#include "checkdta.h"
#include "movelist.h"
#define DEBUG_CODE
#include "dbugit.h"

/* user interface externals */
#ifdef DEBUG
ASSERTDATA
#endif

/* --------------------------------------------------------------------------
Global variables
-------------------------------------------------------------------------- */

int debug=0;              /* these variables should not be used outside the
                             scope of the checkers engine */

int computer_color = BLACK; //REVIEW: this will be put into prune.cpp instead
int depth_maximum = 5;    //REVIEW: this will be put into prune.cpp instead


#include ".\engine\lut.cpp"
#include ".\engine\debugio.cpp"
#include ".\engine\quality.cpp"
#include ".\engine\check.cpp"
#include ".\engine\prune.cpp"
#include ".\engine\valid.cpp"
