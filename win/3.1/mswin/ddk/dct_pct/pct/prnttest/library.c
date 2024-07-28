/*---------------------------------------------------------------------------*\
| LIBRARY MODULE                                                              |
|   This module contains common routines which can be used to facilitate      |
|   faster coding of other modules.  It also contains the library Entry and   |
|   Exit routines.                                                            |
|                                                                             |
| STRUCTURE (----)                                                            |
|                                                                             |
| FUNCTION EXPORTS                                                            |
|   litoa()                                                                   |
|   latoi()                                                                   |
|   GetISGVersion()                                                           |
|                                                                             |
| Copyright 1990-1992 by Microsoft Corporation                                |
\*---------------------------------------------------------------------------*/

#include <windows.h>
#include "isg_test.h"

/*---------------------------------------------------------------------------*\
| LONG INTEGER TO ASCII                                                       |
|   This routine converts an integer to its ascii equivalent.  It converts    |
|   the string (reverse order), then reverses the string to represent the     |
|   correct ordering of digits.                                               |
|                                                                             |
| PRECONDITION                                                                |
|   Assumes a valid buffer which can hold the length of an integer.           |
|   Assumes iBase is a none zero integer.                                     |
|   Assumes iNumb is a valid integer in the range (-32768 to 32767).          |
|                                                                             |
| POSTCONDITION                                                               |
|   Will return a long pointer to string buffer passed to function.           |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   LPSTR - Long pointer to the beginning of the string.                      |
\*---------------------------------------------------------------------------*/
LPSTR FAR PASCAL litoa(int            iNumb,
                       register LPSTR lpString,
                       int            iBase)
{
  register int  iSign;
  LPSTR         lpSave;

  /*-----------------------------------------*\
  | Check for sign.                           |
  \*-----------------------------------------*/
  if(iNumb < 0)
    iSign = -1;
  else
    iSign = 1;

  /*-----------------------------------------*\
  | Convert integer to string (reverse order).|
  \*-----------------------------------------*/
  lpSave = lpString;
  while(iNumb > 0)
  {
    *lpString++ = (char)((iNumb % iBase) + 48);
    iNumb /= iBase;
  }
  *lpString = 0;

  return(ReverseString(lpSave,0,lstrlen(lpSave)-1));
}


/*---------------------------------------------------------------------------*\
| LONG ASCII TO INTEGER                                                       |
|   This routine returns an integer equivalent of the string passed to this   |
|   function.  The local variables are declared as REGISTER to facilitate     |
|   faster processing of function (optimized).                                |
|                                                                             |
| PRECONDITION                                                                |
|   Assumes valid pointer to string (NULL Terminated).                        |
|   Assumes string is in the range ("-32768" to "[+]32767").                  |
|                                                                             |
| POSTCONDITION                                                               |
|   Will return valid integer (-32768 to 32767).                              |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   int - Integer equivalent of string passed to function.                    |
\*---------------------------------------------------------------------------*/
int FAR PASCAL latoi(register LPSTR lpString)
{
  register int iNumb,iSign;

  /*-----------------------------------------*\
  | Is it signed?                             |
  \*-----------------------------------------*/
  if(*lpString == '-')
  {
    iSign = -1;
    lpString++;
  }
  else
  {
    iSign = 1;
    if(*lpString == '+')
          lpString++;
  }

  /*-----------------------------------------*\
  | Convert me!  Show me the way!             |
  \*-----------------------------------------*/
  iNumb = 0;
  while(*lpString)
    iNumb = (iNumb*10) + (*lpString++ - 48);

  return(iNumb*iSign);
}

/*---------------------------------------------------------------------------*\
| REVERSE STRING                                                              |
|   This routine reverses the characters in a string.                         |
|                                                                             |
| PRECONDITION                                                                |
|   Assumes valid pointer to string (NULL Terminated).                        |
|                                                                             |
| POSTCONDITION                                                               |
|   Will return a long pointer to the start of the string.                    |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   LPSTR - Beginning of the text string.                                     |
\*---------------------------------------------------------------------------*/
LPSTR FAR PASCAL ReverseString(register LPSTR lpString,
                               int            nBegin,
                               int            nEnd)
{
  register int nIdx;
  int          nCount;

  nCount = (nEnd-nBegin) >> 1;

  for(nIdx=0; nIdx <= nCount; nIdx++)
    SWAP_BYTE((lpString+nBegin+nIdx),(lpString+nEnd-nIdx));

  return(lpString);
}

