/**[f******************************************************************
* pclsf_yn.c -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.
* Company confidential.
*
**f]*****************************************************************/
  
#include <conio.h>
#include <string.h>
#include "nocrap.h"
#include "windows.h"
#include "dosutils.h"
  
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
#ifdef DEBUG_FUNCT
    DB(("Entering main\n"));
#endif
  
    if (argc > 1)
    {
        s = argv[1];
        k = strlen(s);
  
        if (k > 0 && s[k-1] == ':')
            s[k-1] = '\0';
  
        do {
            blast("Download PCL fonts to ");
            blast(s);
            blast("? [y/n] ");
            ch[0] = (char)getch();
            ch[1] = '\r';
            ch[2] = '\n';
            ch[3] = '\0';
            blast(ch);
        } while ((errlevel=validResponse(ch[0])) < 0);
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting main\n"));
#endif
    return(errlevel);
}
  
  
static short validResponse(c)
char c;
{
#ifdef DEBUG_FUNCT
    DB(("Entering validResponse\n"));
#endif
    if (c == 'y' || c == 'Y')
        return (0);
    else if (c == 'n' || c == 'N')
        return (1);
    else
    {
        blast("Please press 'y' for yes or 'n' for no.\r\n\r\n");
        return (-1);
    }
#ifdef DEBUG_FUNCT
    DB(("Entering validResponse\n"));
#endif
}
  
  
static void blast(s)
char *s;
{
    WORD writ;
#ifdef DEBUG_FUNCT
    DB(("Entering blast\n"));
#endif
    dos_write(STDERR, s, strlen(s), &writ);
}
  
