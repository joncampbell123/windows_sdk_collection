/**[f******************************************************************
* parslin.c -
*
* Copyright (C) 1989,1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*
**f]*****************************************************************/
  
/*
 * $Header: 
 */

/*
 * $Log:
 */

/*************************************************************************
*
*  Routine title: parse_line
*
*  Author:  ?, 26 February 1990
*
*
*  Description:  Parse the file given for the key sent over.  Returns
*                the values (tokens) in the found line.
*
*  Inputs:  tmp - pointer to file to be parsed (ASCII).
*           key - key to search for.
*           buffer - buffer to use to read line with.
*           bufsize - size of the buffer.
*           max_tokens - maximum amount of tokens to take from line.
*
*  Outputs: tokens_locs - list of tokens read from line.
*
*  History:
*           26-Feb-90   ?     original
*           03-May-90   dtk   modified for GLUE file to exit on specified
*                             conditions.
*
*************************************************************************/
  
  
//#define DEBUG
  
/* global includes */
#include <ctype.h>
  
/* local includes */
#include "printer.h"
#include "windows2.h"
#include "tfmread.h"
#include "glue.h"
#include "debug.h"
  
/****************************************************************************\
*                     Debug Definitions
\****************************************************************************/
  
/*  Local debug stuff.
*/
#ifdef DEBUG
    #define LOCAL_DEBUG
#endif
  
#ifdef LOCAL_DEBUG
    #define DBGpars(msg)    /*DBMSG(msg)*/
    #define DBGentry(msg)   /*DBMSG(msg)*/
#else
    #define DBGpars(msg)    /*null*/
    #define DBGentry(msg)   /*null*/
#endif
  
  
extern HANDLE hFile;
extern LONG bufpos;
  
/*int FAR PASCAL parse_line(int, LPSTR, BYTE far * far *, int, LPSTR, int, int);*/
static LPSTR win_fgets(LPSTR, int, int);
  
/*************************************************************************/
  
int FAR PASCAL parse_line (tmp, key, token_locs, max_tokens, buffer, bufsize, pass)
  
  
int   tmp;               /* pointer to open file  */
LPSTR key;               /* key     */
//BYTE  far *token_locs[]; /* array of addresses for tokens*/
BYTE  far * far *token_locs; /* array of addresses for tokens*/
int   max_tokens;        /* maximum number of tokens */
LPSTR buffer;            /* place for line of data */
int   bufsize;           /* size of buffer  */
int   pass;              /* for GLUE file positioning */
  
  
{
    LPSTR p;        /* pointers to parsed line */
    LPSTR q;
    LPSTR r;
    int  x, i;             /* counters */
    int  keylen, len;      /* line lengths */
  
    DBGentry(("parse_line: enter\n"));
  
    DBGpars(("parse_line: key = %ls\n", key));
  
    keylen = lstrlen(key);      /* length of key  */
  
    DBGpars(("parse_line: keylen = %d\n", keylen));
  
    while (win_fgets(buffer, bufsize, tmp))
    {
        DBGpars(("parse_line:     %ls\n", buffer));
  
        x = lstrlen(buffer);
        if (x == 0)
            continue;               /* NULL line   */
        p = (LPSTR)&buffer[x-1];         /* ptr to end of buffer  */
  
        /* remove ending cntr chars and spaces */
        while (((iscntrl(*p)) || (isspace(*p))) && p >= buffer)
            *p-- = EOS;
  
        p = buffer;               /* beginning of buffer  */
  
        while (isspace(*p)) ++p; /* skip white space  */
  
        if ((*p == EOS) || (*p == ';')) /* NULL line or comment */
            continue;
  
        if ((pass && (*p == '[')) || ((pass == 2) && (*p == '{')) ||
            ((pass == 3) && ((*p == '{') || (*p == 'F'))))
        {
            DBGpars(("parse_line: special case, return(-1)\n"));
            return(-1);
        }
  
        DBGpars(("parse_line: q = %ls\n", p));
        DBGpars(("parse_line: r = %ls\n", key));
  
        /* test key for equality */
        for (q = p, r = key, i = (keylen - 1);
            //  ((tolower((int)(*q)) == tolower((int)(*r))) && (i));
            ((*q == *r) && (i));
            q++, r++, i--)
        {
            if (*q == '\0')
                break;
  
            DBGpars(("parse_line: *q, *r, i = %c, %c, %d\n", *q, *r, i));
        }
  
        /* not this one */
        if (*q - *r)
        {
            DBGpars(("parse_line: key did not match\n"));
            continue;
        }
  
        q=p+keylen;
        //    *q++ = EOS;             /* terminate string in buffer */
  
        token_locs[0] = p;        /* name    */
  
  
        for (i=1; i<max_tokens; ++i) /* parse rest of tokens  */
        {
  
            while (isspace (*q))
            {
                ++q;                  /* skip white space  */
                if (*q == ';')
                {
                    *q = EOS;           /* terminate string in buffer */
                    return (i);         /* rest of line is comment */
                }
            }
  
            if (*q == EOS)
            {
                return (i);           /* return number of tokens  */
            }
  
            token_locs[i] = q;      /* next token   */
  
  
            while (!isspace (*q))
            {
                if ((*q == ';') || (*q == '}'))
                {
                    *q = EOS;           /* terminate string in buffer */
                    return (i);         /* rest of line is comment */
                }
                if (*q++ == EOS)
                {
                    return (i);         /* skip token   */
                }
            }
            if (i != max_tokens-1)  /* last one ? (gets rest of line */
                *q++ = EOS;           /* terminate string in buffer */
        }
  
  
        do                        /* find any comments at ends of lines */
        {
            if ((*q == ';') || (*q == '}'))
            {
                *q = EOS;
                x = lstrlen(token_locs[i-1]);
                p = (LPSTR)&token_locs[i-1][x-1];   /* ptr to end of buffer  */
                while (isspace (*p) && p >= token_locs[i-1]) /* remove ending spaces */
                    *p-- = EOS;
            }
        }
        while(*q++ != EOS);
  
        DBGentry(("parse_line: return(max_tokens: %d)\n", max_tokens));
  
        return (max_tokens);      /* more on the line but .. */
    }
  
    DBGentry(("parse_line: return(max_tokens: -1)\n"));
  
    return (-1);                /* error -- not there  */
}
  
/*************************************************************************
*
*  Routine title: win_fgets
*
*  Description:  Windows version of fgets() call.
*
*  Inputs:  buffer - buffer for data
*           bufsize - size of buffer
*           fp - file pointer
*
*  Outputs: buffer - buffer filled with string
*
*  Return:  buffer, or NULL if EOF or error
*
*  History:
*           17-Oct-90         original
*
*************************************************************************/
LPSTR win_fgets(LPSTR buffer, int bufsize, int fp)
  
{
    int i, bytes_read = 1;
    LPSTR pos;
    BYTE huge *lpFile;
  
    DBGentry(("win_fgets: enter\n"));
  
    if (!(lpFile = (BYTE huge *)GlobalLock(hFile)))
        return(NULL);
  
    for (i=1, pos = buffer;
        ((i<bufsize) && (*pos = lpFile[bufpos]) && (*pos!='\n'));
        i++, pos++, bufpos++)
        ;
  
    buffer[i - 1] = '\0';
  
    DBGentry(("win_fgets: exit\n"));
  
    if (lpFile[bufpos])
    {
        GlobalUnlock(hFile);
  
        bufpos++;
        return(buffer);
    }
    else
    {
        GlobalUnlock(hFile);
  
        return(NULL);   /* "EOF" */
    }
}
