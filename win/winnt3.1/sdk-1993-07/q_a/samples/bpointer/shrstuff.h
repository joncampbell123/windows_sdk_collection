
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1993 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/

// These are the data structures used to change the data through the pointer.



typedef struct CHAINLIST;


struct CHAINLIST   *clBasePt;
struct CHAINLIST __based(clBasePt) *clRunning;

typedef struct CHAINLIST
{ int iElement;
  char cDummy;       // padding to show MIPS alignment consideration
#ifdef MIPS
  char cDummyPadding; // without this padding, MIPS would alignment fault.
  char cDummyPadding1;
  char cDummyPadding2;
#endif
  struct CHAINLIST __based(clBasePt) *clNext;
} CHAINLIST;
