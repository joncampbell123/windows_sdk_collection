/**[f******************************************************************
* pclsf_yn.c -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1990,1991 Hewlett-Packard Company.
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

/* 11/19/91 RK(HP) Moved localizable text to SF_YN.H */
  
#include <conio.h>
#include <string.h>
#include "nocrap.h"
#include "windows.h"
#include "dosutils.h"
#include "sf_yn.h"
  
static short validResponse(char);
static void blast(char *);
  
int main(argc, argv)
int argc;
char *argv[];
{
    short errlevel = 1;
    short k;
    char ch[4];
    char *s;
  
    if (argc > 1)
    {
        s = argv[1];
        k = strlen(s);
  
        if (k > 0 && s[k-1] == ':')
            s[k-1] = '\0';
  
        do {
//            blast("Download PCL fonts to ");
            blast(YN_DOWNLD);
            blast(s);
//            blast("? [y/n] ");
            blast(YN_YN);
            ch[0] = (char)getch();
            ch[1] = '\r';
            ch[2] = '\n';
            ch[3] = '\0';
            blast(ch);
        } while ((errlevel=validResponse(ch[0])) < 0);
    }
  
    return(errlevel);
}
  
  
static short validResponse(c)
char c;
{
//    if (c == 'y' || c == 'Y')
    if (c == YN_Y || c == YN_CAPY)
        return (0);
//    else if (c == 'n' || c == 'N')
    else if (c == YN_N || c == YN_CAPN)
        return (1);
    else
    {
//        blast("Please press 'y' for yes or 'n' for no.\r\n\r\n");
        blast(YN_TRYAGAIN);
        return (-1);
    }
}
  
  
static void blast(s)
char *s;
{
    WORD writ;
    dos_write(STDERR, s, strlen(s), &writ);
}
  
