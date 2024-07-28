/************************************************************

   Header file for SREC.C and PENAPP.C


************************************************************/

#ifndef _INC_MAIN
#define _INC_MAIN

/******** Macros *********/

/* Unused(): Prevent CS 5.1 compiler warning messages.
*/
#define Unused(x)    x     

/******** Constants *********/

/* These are recognizer-specific symbols (as indicated by the
** 0x7F00 hiword) for the sample recognizer. 
*/
#define  syvDot      ((SYV)MAKELONG(0x1, 0x7F00))
#define  syvEast     ((SYV)MAKELONG(0x2, 0x7F00))
#define  syvSouth    ((SYV)MAKELONG(0x3, 0x7F00))
#define  syvWest     ((SYV)MAKELONG(0x4, 0x7F00))
#define  syvNorth    ((SYV)MAKELONG(0x5, 0x7F00))

#define  REC_NOPENUP  (REC_OEM-1)   /* Recognizer specific error */

#endif /* !_INC_MAIN */
