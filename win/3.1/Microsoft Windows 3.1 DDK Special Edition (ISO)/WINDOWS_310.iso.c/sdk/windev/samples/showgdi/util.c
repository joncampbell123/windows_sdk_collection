/****************************************************************************
 Util.c

 The Util module handles misc. low-level utilities.

****************************************************************************/

#include "windows.h"

#include "util.h"


/****************************************************************************
   Globals
****************************************************************************/

HCURSOR hcurSave;    /* saved cursor before hourglass is shown */


/****************************************************************************
   Functions
****************************************************************************/

int ToIntegerPin( long l )
{
   if (l > MAXINT )
      return MAXINT;
   else if (l < -MAXINT) 
      return -MAXINT;
   else
      return (int) l;
}


int Min( int x, int y )
{
   return (x < y) ? x : y;
}


int Max( int x, int y )
{
   return (x > y) ? x : y;
}


unsigned long HexToLong( char *s )
{
   unsigned long  l = 0L;
   register char  c;

   for ( ; (c = *s); s++) {
      if (c >= '0' && c <= '9') {
         l = l * 16 + c - '0';
      }else if (c >= 'A' && c <= 'F') {
         l = l * 16 + c - 'A' + 10;
      }else if (c >= 'a' && c <= 'f') {
         l = l * 16 + c - 'a' + 10;
      }
   }
   return l;
}
      
