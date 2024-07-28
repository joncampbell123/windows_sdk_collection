/**[f******************************************************************
* getint.c -
*
* Copyright (C) 1988,1989 Aldus Corporation.  All rights reserved.
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* Company confidential.
*
**f]*****************************************************************/
  
// history
// 27 apr 89    peterbe     Tabs are 8 spaces.
  
/*  GetInt
*
*  Returns number found between current buffer position and next separator
*  char or end of string; if no number was found then returns zero.
*/
static int GetInt(LPSTR FAR *, char);
static int GetInt(bptr, sepchar)
LPSTR FAR *bptr;
char sepchar;
{
    int tempint=0;
#ifdef DEBUG_FUNCT
    DB(("Entering GetInt\n"));
#endif
    while ((**bptr) && (**bptr != sepchar))
    {
        if (**bptr>= '0' && **bptr <= '9')
        {
            tempint = (tempint * 10) + (**bptr - '0');
        }
        (*bptr)++;
    }
#ifdef DEBUG_FUNCT
    DB(("Exiting GetInt\n"));
#endif
    return tempint;
}
  
