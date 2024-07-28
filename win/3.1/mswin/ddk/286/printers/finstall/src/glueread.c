/**[f******************************************************************
* glueread.c -
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
  
/********************************* glueread.c *****************************/
/*
* Summary:    This file contains the routines necessary to read the
*             GLUE.TXT file which contains info about the fonts available
*             to an application.
*/
/***************************************************************************/
  
  
//#define DEBUG
  
/* Use ctype.h for the isspace() macro */
  
#include <ctype.h>
  
/* local includes */
  
#include "printer.h"
#include "windows2.h"
#include "tfmread.h"
#include "glue.h"
#include "neededh.h"
  
/****************************************************************************\
*                     Debug Definitions
\****************************************************************************/
  
/*  Local debug stuff.
*/
#ifdef DEBUG
    #define LOCAL_DEBUG
#endif
  
#ifdef LOCAL_DEBUG
    #define DBGerr(msg) /* DBMSG(msg) */
    #define DBGwinsf(msg)   /* DBMSG(msg) */
    #define DBGglue(msg)    /* DBMSG(msg) */
    #define DBGentry(msg)   /* DBMSG(msg) */
#else
    #define DBGerr(msg) /*null*/
    #define DBGwinsf(msg)   /*null*/
    #define DBGglue(msg)    /*null*/
    #define DBGentry(msg)   /*null*/
#endif
  
typedef int far         *LPINT;
  
extern LONG FAR PASCAL atol(LPSTR);
  
/* external functions in same segment */
int FAR PASCAL parse_line(int, LPSTR, BYTE far * far *, int, LPSTR, int, int);
  
/* function definitions */
  
HANDLE FAR PASCAL get_glue_info(int, LPINT, LPINT);
// LPGFINFO get_font_info(int, int, int);
HANDLE FAR PASCAL get_cart_info(int, LPSTR, LPINT);
void get_weight_name(int, LPSTR);
  
HANDLE hFile;
LONG bufpos;
  
  
/***************************************************************************
  
Routine Title:  get_glue_info
  
Author:  dtk, 8/4/89
  
Summary:  This routine reads the GLUE.TXT file info into the GLUEINFO
structure. This structure is then used to build the font or
cartridge display names.
  
Inputs:  *gluein  - handle to input file.
  
Outputs:  G        - a pointer to the GLUEINFO structure.
*cartnum - number of cartridges in the glue file.
*fontnum - number of PCLEOs in the glue file.
  
Modifications:
  
27-Apr-90  dtk  original.
****************************************************************************/
  
HANDLE FAR PASCAL get_glue_info(int gluein, LPINT cartnum, LPINT fontnum)
  
  
{
  
    int       i,j;
    int       gluwt;
    int       ntokens=0;
    int       total;
    long      setpos;
    long      cartpos;
    long      fontpos;
    BYTE far  *tokens[5];
    BYTE      buf[100];
    HANDLE    hG;
    LPGLUEINFO    G;
    long      filesize;   /* Size of glue file              */
    long      remaining;  /* Unread bytes in glue file      */
    long      readsize;   /* # of bytes to read in _lread() */
    BYTE huge *lpFile;        /* Glue file in memory            */
  
    DBGentry(("get_glue_info: begin\n"));
  
    DBGglue(("get_glue_info: About to _llseek\n"));
  
    if ((remaining = filesize = _llseek(gluein, 0L, 2)) < 0L)
    {
        DBGglue(("get_glue_info: return(0)\n"));
  
        return(0);
    }
  
    if (!((hFile = GlobalAlloc(GMEM_MOVEABLE, (DWORD)(filesize + 1L))) &&
        (lpFile = (BYTE huge *)GlobalLock(hFile))))
    {
        DBGglue(("get_glue_info: return(0)\n"));
  
        return(0);
    }
  
    if (_llseek(gluein, 0L, 0) < 0L)
    {
        DBGglue(("get_glue_info: return(0)\n"));
  
        return(0);
    }
  
    while (remaining)
    {
        if (remaining > MAXBUFSIZE)
            readsize = MAXBUFSIZE;
        else
            readsize = remaining;
  
        if (_lread(gluein, (LPSTR)&lpFile[filesize-remaining],
            (WORD)readsize) < (WORD)readsize)
        {
            DBGglue(("get_glue_info: return(0)\n"));
  
            return(0);
        }
  
        remaining -= readsize;
    }
  
    lpFile[filesize] = '\0';
  
    GlobalUnlock(hFile);
    bufpos = 0;
  
    DBGglue(("get_glue_info: About to parse for cartnum\n"));
  
    ntokens = parse_line (gluein,(LPSTR)"[Cartridge]",tokens,1,buf,sizeof(buf),0);
    if (ntokens != -1)
    {
        DBGglue(("get_glue_info: Found '[Cartridge]'\n"));
  
        cartpos = bufpos;
  
        while (ntokens != -1)
        {
            ntokens = parse_line (gluein,(LPSTR)"{",tokens,1,buf,sizeof(buf),1);
  
            DBGglue(("get_glue_info: parse_line('{') == %d\n", ntokens));
  
            if (ntokens != -1)
                (*cartnum)++;
        }
    }
  
    DBGglue(("get_glue_info: cartnum = %d\n", *cartnum));
  
    bufpos = 0;
  
    ntokens = parse_line (gluein,(LPSTR)"[PCL fonts]",tokens,1,buf,sizeof(buf),0);
    if (ntokens != -1)
    {
        ntokens = parse_line (gluein,(LPSTR)"{O}",tokens,1,buf,sizeof(buf),1);
  
        if (ntokens == -1)
            (*fontnum) = 0;
        else
        {
            fontpos = bufpos;
  
            while (ntokens != -1)
            {
                ntokens = parse_line (gluein,(LPSTR)"FONT",tokens,1,buf,sizeof(buf),2);
                if (ntokens != -1)
                    (*fontnum)++;
            }
        }
    }
  
    DBGglue(("get_glue_info: fontnum = %d\n", *fontnum));
  
    total = (*fontnum) + (*cartnum);
    if (total == 0)
        return(0);
    G = 0L;
  
    if ((hG = GlobalAlloc(GHND, (DWORD)(total * sizeof(GLUEINFO)))) &&
        (G = (LPGLUEINFO)GlobalLock(hG)))
    {
        /* get cartridge name info */
  
        bufpos = cartpos;
  
        for (i=0; i<(*cartnum); i++)
        {
            ntokens = parse_line (gluein,(LPSTR)"{",tokens,3,buf,sizeof(buf),1);
            if (ntokens != -1)
            {
                lstrcpy(G[i].giFFile,tokens[1]);   /* Put cart # in PCLEO field */
                lstrcpy(G[i].giName,tokens[2]);
                if (lstrlen(G[i].giName))
                {
                    G[i].giType = 2;
  
                    /* Nab the 1st "class" field & check for scalable cart */
                    ntokens = parse_line(gluein, (LPSTR)"/class", tokens, 3, buf,
                    sizeof(buf), 2);
                    if ((ntokens != -1) && (*tokens[2] == 'O'))
                        G[i].giType++;
                }
                else
                    G[i].giType = 0;    /* Error flag for finstall */
            }
  
            /* WTF is this doing here?  if ((setpos = _llseek(gluein, 0L, 1)) < 0L))
            return(0); */
        }
  
        DBGglue(("get_glue_info: About to get font name info\n"));
  
        /* get font name info */
  
        bufpos = fontpos;
  
        for (j=(*cartnum); j<((*fontnum)+(*cartnum)); j++)
        {
            ntokens = parse_line (gluein,(LPSTR)"FONT",tokens,3,buf,sizeof(buf),2);
  
            if (ntokens != -1)
            {
                lstrcpy(G[j].giName,tokens[2]);
  
                if (lstrlen(G[j].giName))
                    G[j].giType = 1;
                else
                    G[j].giType = 0;    /* Error flag for finstall */
            }
  
            setpos = bufpos;
  
            ntokens = parse_line (gluein,(LPSTR)"/symset",tokens,4,buf,sizeof(buf),3);
            if (ntokens != -1)
                lstrcpy(G[j].giSymSet,tokens[2]);
  
            bufpos = setpos;
  
            ntokens = parse_line (gluein,(LPSTR)"/weight",tokens,3,buf,sizeof(buf),3);
            gluwt = (ntokens == -1) ? 130 : (atoi(tokens[2]));
            get_weight_name(gluwt, G[j].giWeight);
  
            bufpos = setpos;
  
            ntokens = parse_line(gluein,(LPSTR)"/tfm",tokens,3,buf,sizeof(buf),3);
            if (ntokens != -1)
                lstrcpy(G[j].giTfm, tokens[2]);
  
            bufpos = setpos;
  
            ntokens = parse_line (gluein,(LPSTR)"/slant",tokens,3,buf,sizeof(buf),3);
            if (ntokens != -1)
            {
                if (atoi(tokens[2]))
                    lstrcpy(G[j].giSlant, (LPSTR)" italic");
                else
                    G[j].giSlant[0] = '\0';
            }
            else
                G[j].giSlant[0] = '\0';
  
            bufpos = setpos;
  
            ntokens = parse_line(gluein,(LPSTR)"/font file",tokens,3,buf,sizeof(buf),3);
            if (ntokens != -1)
                lstrcpy(G[j].giFFile, tokens[2]);
  
            bufpos = setpos;
  
            ntokens = parse_line (gluein,(LPSTR)"/resource",tokens,4,buf,sizeof(buf),3);
            if (ntokens == 3)
                G[j].giMemusage = atol(tokens[3]);
            else
                G[j].giMemusage = 0;
  
            bufpos = setpos;
        }
  
        GlobalUnlock(hG);
  
    }    /* if (alloc mem for G) */
  
//  GlobalUnlock(hFile);
    GlobalFree(hFile);
  
    DBGentry(("get_glue_info: return(hG: %d)\n", hG));
  
    return(hG);
  
}
  
  
/***************************************************************************
  
Routine Title:  get_cart_info
  
Author:  dtk, 8/4/89
  
Summary:  This routine reads the GLUE.TXT file info into the GCINFO
structure. This structure is then used to find the number
of fonts and their corresponding TFM files.
  
Inputs:  *gluein  - handle to input file.
*reqname - pointer to the requested cartridge name.
  
Outputs:  GLU      - a pointer to the GCINFO structure.
*fontnum - number of fonts in the cartridge.
  
Modifications:
  
27-Apr-90  dtk  original.
  
****************************************************************************/
  
HANDLE FAR PASCAL get_cart_info(int gluein, LPSTR reqname, LPINT fontnum)
  
  
{
  
    int       i,j;
    int       found=0;
    int       ntokens=0;
    int       symtokens=0;
    int       gluwt;
    int       symnum;
    BYTE far  *tokens[5];
    BYTE      buf[100];
    BYTE      wt_name[5];
    long      cartpos;
    long      setpos;
    //long      setpos2;
    LPGCINFO  GLU;
    HANDLE    hGLU;
    LPSYMINFO SYM;
    HANDLE    hSYM;
    long      filesize;       /* Size of glue file              */
    long       remaining;     /* Unread bytes in glue file      */
    long       readsize;      /* # of bytes to read in _lread() */
    BYTE huge *lpFile;        /* Glue file in memory            */
  
  
    if ((remaining = filesize = _llseek(gluein, 0L, 2)) < 0L)
    {
        DBGglue(("get_glue_info: return(0)\n"));
  
        return(0);
    }
  
    if (!((hFile = GlobalAlloc(GMEM_MOVEABLE, (DWORD)(filesize + 1L))) &&
        (lpFile = (BYTE huge *)GlobalLock(hFile))))
    {
        if (hFile)
            GlobalFree(hFile);
  
        DBGglue(("get_glue_info: return(0)\n"));
  
        return(0);
    }
  
    if (_llseek(gluein, 0L, 0) < 0L)
    {
        GlobalUnlock(hFile);
        GlobalFree(hFile);
  
        return(0);
    }
  
    while (remaining)
    {
        if (remaining > MAXBUFSIZE)
            readsize = MAXBUFSIZE;
        else
            readsize = remaining;
  
        if (_lread(gluein, (LPSTR)&lpFile[filesize-remaining],
            (WORD)readsize) < (WORD)readsize)
        {
            GlobalUnlock(hFile);
            GlobalFree(hFile);
  
            DBGglue(("get_glue_info: return(0)\n"));
  
            return(0);
        }
  
        remaining -= readsize;
    }
  
    lpFile[filesize] = '\0';
  
    GlobalUnlock(hFile);
    bufpos = 0;
  
    ntokens = parse_line (gluein,(LPSTR)"[Cartridge]",tokens,1,buf,sizeof(buf),0);
  
    while (!(found))
    {
        ntokens = parse_line (gluein,(LPSTR)"{",tokens,3,buf,sizeof(buf),1);
        found = ((lstrcmp(tokens[2], reqname)) == 0);
    }
  
    setpos = cartpos = bufpos;
  
    /* get number of fonts and symbol sets in the cartridge */
  
    *fontnum=0;
    while (ntokens != -1)
    {
        bufpos = setpos;
  
        ntokens = parse_line (gluein,(LPSTR)"FONT",tokens,1,buf,sizeof(buf),2);
        if (ntokens != -1)
        {
            (*fontnum)++;
            setpos = bufpos;
        }
    }
    GLU = 0L; SYM = 0L;
  
    /* allocate memory for GCINFO structure */
    if ((hGLU = GlobalAlloc(GHND,(DWORD)((*fontnum)*sizeof(GCINFO)))) &&
        (GLU = (LPGCINFO)GlobalLock(hGLU)))
    {
  
        /* set position in glue file to requested cartridge again */
  
        bufpos = cartpos;
  
        /* get the font info for each font to fill in the glue structure */
  
        for (i=0; i<(*fontnum); i++)
  
        {
            ntokens = parse_line(gluein,(LPSTR)"FONT",tokens,3,buf,sizeof(buf),2);
//    if (ntokens != -1)
//     lstrcpy(GLU[i].gcFName, tokens[2]);
  
            /* save this position at the font */
            setpos = bufpos;
  
            /* Tack stroke weight to end of font name */
//      ntokens = parse_line (gluein,(LPSTR)"/weight",tokens,3,buf,sizeof(buf),3);
//      gluwt = (ntokens == -1) ? 130 : (atoi(tokens[2]));
//      get_weight_name(gluwt, (LPSTR)wt_name);
//      if ((gluwt < 171) || (gluwt > 187))
//     lstrcat((LPSTR)GLU[i].gcFName, (LPSTR)wt_name);
//
//      bufpos = setpos;
  
            ntokens = parse_line(gluein,(LPSTR)"/class",tokens,3,buf,sizeof(buf),3);
            if (ntokens != -1)
                GLU[i].gcClass = *tokens[2];
            bufpos = setpos;
  
            ntokens = parse_line(gluein,(LPSTR)"/orient",tokens,3,buf,sizeof(buf),3);
            if (ntokens != -1)
            {
                GLU[i].gcOrient = *tokens[2];
                if (GLU[i].gcOrient == 'R')           /* Reverse land or port */
                    GLU[i].gcOrient = tokens[2][1];
                GLU[i].gcOrient = (GLU[i].gcOrient == 'L') ? (BYTE)2 : (BYTE)1;
            }
            else
                GLU[i].gcOrient = 0;
  
            bufpos = setpos;
  
            symnum = 0;
            symtokens = 0;
            while (symtokens != -1)
            {
                symtokens = parse_line(gluein,(LPSTR)"/symset",tokens,1,buf,sizeof(buf),3);
                if (symtokens != -1)
                    symnum++;
            }
  
            j = 0;
  
            bufpos = setpos;
  
            if ((hSYM = GlobalAlloc(GMEM_MOVEABLE,(DWORD)(symnum*sizeof(SYMINFO)))) &&
                (SYM = (LPSYMINFO)GlobalLock(hSYM)))
            {
                while ((ntokens = parse_line(gluein, (LPSTR)"/symset", tokens, 4,
                    buf, sizeof(buf), 3)) != -1)
                {
                    lstrcpy(SYM[j++].ssPCLcode, tokens[2]);
                }
  
                GLU[i].gchSym = hSYM;
                GLU[i].gcSSIndex = j;
  
                GlobalUnlock(hSYM);
            }
  
#ifdef FRICKAFRACK
            if ((hSYM = GlobalAlloc(GMEM_MOVEABLE,(DWORD)(symnum*sizeof(SYMINFO)))) &&
                (SYM = (LPSYMINFO)GlobalLock(hSYM)))
            {
  
                ntokens = parse_line(gluein,(LPSTR)"/symset",tokens,4,buf,sizeof(buf),3);
                setpos2 = bufpos;
  
                /* get symbol set info: could be multiple */
  
                while (lstrcmp(tokens[0],(LPSTR)"/symset"))
                {
                    if (ntokens != -1)
                    {
                        lstrcpy(SYM[j].ssPCLcode, tokens[2]);
  
                        /* No symset class for now
                        SYM[j].ssClass = atoi(tokens[3]);
                        */
                    }
  
                    j++;
                    ntokens = parse_line(gluein,(LPSTR)"/symset",tokens,4,buf,sizeof(buf),3);
  
                    if (ntokens == -1)
                    {
                        bufpos = setpos2;
                        break;
                    }
                }
  
                GLU[i].gchSym = hSYM;
                GLU[i].gcSSIndex = j;
  
                GlobalUnlock(hSYM);
            }
#endif
  
            bufpos = setpos;
  
            /* get the pt size info: could be multiple */
  
            ntokens = parse_line(gluein,(LPSTR)"/ptsize",tokens,3,buf,sizeof(buf),3);
            /* Count the # of pt sizes according to the separating spaces */
            if (ntokens != -1)
            {
                GLU[i].gcNumSizes = 1;
  
                lstrcpy(GLU[i].gcPtSizes, tokens[2]);
  
                /* 1st element of array must be a #, so start w/ 2nd */
  
                for (j=1; GLU[i].gcPtSizes[j]; j++)
                    if (isspace(GLU[i].gcPtSizes[j]) &&
                        (GLU[i].gcPtSizes[j+1]) &&
                        (!(isspace(GLU[i].gcPtSizes[j+1]))))
                        GLU[i].gcNumSizes++;
            }
            else
                GLU[i].gcNumSizes = 0;
  
            bufpos = setpos;
  
            ntokens = parse_line(gluein,(LPSTR)"/tfm",tokens,3,buf,sizeof(buf),3);
            if (ntokens != -1)
                lstrcpy(GLU[i].gcTfm, tokens[2]);
  
        } /* end for */
  
        GlobalUnlock(hGLU);
  
    } /* if (ALLOC...) */
  
// GlobalUnlock(hFile);
    GlobalFree(hFile);
  
    return(hGLU);
  
}
  
  
  
/***************************************************************************
  
Routine Title:  get_font_info
  
Author:  dtk, 8/4/89
  
Summary:  This routine reads the GLUE.TXT file info into the GFINFO
structure. This structure is then used to find the font
information and its corresponding TFM file.
  
Inputs:  *gluein  - handle to input file.
index    - the position index in the glue file.
  
Outputs:  F        - a pointer to the GFINFO structure.
  
  
Modifications:
  
27-Apr-90  dtk  original.
  
****************************************************************************/
  
//LPGFINFO get_font_info(HANDLE hF, int gluein, int index, int selected)
//
//
//{
//
//
//  int       i;
//  int       ntokens=0;
//  char far  *tokens[5];
//  char far  buf[100];
//  long      setpos;
//  LPGFINFO    F;
//
//
//
//   i = selected;
//   if (_llseek(gluein, 0L, 0) < 0L)
//      return(NULL);
//
//   F = 0L;
//   if ((hF = GLobalAlloc(GMEM_MOVEABLE, (DWORD)sizeof(GFINFO))) &&
//  (F = (LPGFINFO)GlobalLock(hF)))
//   {
//
//      /* set position in glue file to requested font */
//
//   ntokens = parse_line (gluein,(LPSTR)"[PCL fonts]",tokens,1,buf,sizeof(buf),0);
//   ntokens = parse_line (gluein,(LPSTR)"{O}",tokens,1,buf,sizeof(buf),0);
//
//   for(i=0;i<index;i++)
//      ntokens = parse_line (gluein,(LPSTR)"FONT",tokens,3,buf,sizeof(buf),2);
//
//   if (ntokens != -1)
//     lstrcpy(F[i].fiFName, tokens[2]);
//
//   if ((setpos = _llseek(gluein, 0L, 1)) < 0L)
//      return(NULL);
//
//   ntokens = parse_line(gluein,(LPSTR)"/font file",tokens,3,buf,sizeof(buf),3);
//   if (ntokens != -1)
//     lstrcpy(F[i].fiFFile, tokens[2]);
//   if (_llseek(gluein, setpos, 0) < 0L)
//      return(NULL);
//
//   ntokens = parse_line(gluein,(LPSTR)"/symset",tokens,4,buf,sizeof(buf),3);
//   if (ntokens != -1)
//   {
//     lstrcpy(F[i].fiSymSet, tokens[2]);
//      F[i].fiSSClass = atoi(tokens[3]);
//   }
//   if (_llseek(gluein, setpos, 0) < 0L)
//      return(NULL);
//
//   ntokens = parse_line(gluein,(LPSTR)"/tfm",tokens,3,buf,sizeof(buf),3);
//   if (ntokens != -1)
//     lstrcpy(F[i].fiTfm, tokens[2]);
//   }  /* if (ALLOC...) */
//
//   return(F);
//
//
//}
  
  
/***************************************************************************
  
Routine Title:  get_weight_name
  
Author:  dtk
  
Summary:  This routine reads the GLUE.TXT file info into the GLUEINFO
structure. This structure is then used to build the font or
cartridge display names.
  
Inputs:  *glueweight - value for weight in the glue file.
  
Outputs:  pointer to a char string for the name of the weight value.
  
  
Modifications:  15-June-90  dtk  original.
  
  
****************************************************************************/
  
void get_weight_name(int glueweight, LPSTR lpString)
  
{
    if (glueweight < 18)
        lstrcpy(lpString, (LPSTR)" Utn");
    //  lstrcpy(lpString, (LPSTR)" UThin");
    else if (glueweight < 35)
        lstrcpy(lpString, (LPSTR)" Xtn");
    //  lstrcpy(lpString, (LPSTR)" ExThin");
    else if (glueweight < 52)
        lstrcpy(lpString, (LPSTR)" Tn");
    //  lstrcpy(lpString, (LPSTR)" Thin");
    else if (glueweight < 69)
        lstrcpy(lpString, (LPSTR)" Xl");
    //  lstrcpy(lpString, (LPSTR)" ExLt");
    else if (glueweight < 86)
        lstrcpy(lpString, (LPSTR)" Lt");
    //  lstrcpy(lpString, (LPSTR)" Light");
    else if (glueweight < 103)
        lstrcpy(lpString, (LPSTR)" Dl");
    //  lstrcpy(lpString, (LPSTR)" DemiLt");
    else if (glueweight < 120)
        lstrcpy(lpString, (LPSTR)" Sl");
    //  lstrcpy(lpString, (LPSTR)" SemiLt");
    else if (glueweight < 137)
        lpString[0] = '\0';
    else if (glueweight < 154)
        lstrcpy(lpString, (LPSTR)" Sb");
    //  lstrcpy(lpString, (LPSTR)" SemiBd");
    else if (glueweight < 171)
        lstrcpy(lpString, (LPSTR)" Db");
    //  lstrcpy(lpString, (LPSTR)" DemiBd");
    else if (glueweight < 188)
        lstrcpy(lpString, (LPSTR)" Bd");
    //  lstrcpy(lpString, (LPSTR)" Bold");
    else if (glueweight < 205)
        lstrcpy(lpString, (LPSTR)" Xb");
    //  lstrcpy(lpString, (LPSTR)" ExBd");
    else if (glueweight < 222)
        lstrcpy(lpString, (LPSTR)" Blk");
    //  lstrcpy(lpString, (LPSTR)" Black");
    else if (glueweight < 239)
        lstrcpy(lpString, (LPSTR)" Xbl");
    //  lstrcpy(lpString, (LPSTR)" ExBlk");
    else if (glueweight < 256)
        lstrcpy(lpString, (LPSTR)" UBl");
    //  lstrcpy(lpString, (LPSTR)" UltraBlk");
    else
        lpString[0] = '\0';
}
