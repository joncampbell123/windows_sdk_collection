#include "pscript.h"

extern void PASCAL SelectFont(DV FAR *, LPFX);






/******************************************************************
* Name: ShowChar()
*
* Action: Print a single character on the page.
*
********************************************************************
*/
void FAR PASCAL ShowChar(lpdv, ix, iy, iCh, cx, lpfx)
LPDV lpdv;		/* Ptr to the device descriptor */
int ix; 		/* The horizontal coordinate */
int iy; 		/* The vertical coordinate */
int iCh;		/* The character to print */
int cx; 		/* The computed width of the character */
LPFX lpfx;	    /* Ptr to the extended font descriptor */
    {
    char bCh;
    LPDF lpdf = lpfx->lpdf;

    bCh = iCh;
    PrintChannel(lpdv, (LPSTR)"%d %d %d %q StrBlt\n",
	 ix, iy+lpdf->dfAscent, (int) cx, (LPSTR)&bCh, 1);
    }





/********************************************************************
* Name: SymbolFont()
*
* Action: Switch to PostScript's "symbol" font so that we can
*	  access some of the special characters.
*
*********************************************************************
*/
void PASCAL SymbolFont(lpdv, lpfx)
DV FAR *lpdv;       /* Far ptr to the device descriptor */
LPFX lpfx;
    {
    LPDF lpdf;

    extern int FAR PASCAL Lightness(RGB);


    lpdf = lpfx->lpdf;
    lpdv->dh.lidFont = -1L;

    /* Output the font change command to the printer */
    PrintChannel(lpdv, (LPSTR) "%d %d %d %d %d %d %d %d /Symbol font\n",
	    (lpdf->dfBreakChar & 0X0ff) + (lpdf->dfFirstChar & 0x0ff),
            lpfx->escapement,
            lpfx->orientation,
            lpfx->sx,
            lpfx->sy,
            lpdf->dfUnderline,
            lpdf->dfStrikeOut,
            Lightness(lpdv->dh.TextColor) < 500
            );
    }









/************************************************************
* Name: ShowFraction()
*
* Action: Print a fraction character with a numerator over
*         the denominator.
*
* Note: The PostScript fraction bar has been mapped to the
*       ANSI 1/4  (0xbc) character code.
*
*
**************************************************************
*/
void PASCAL ShowFraction(lpdv, ix, iy, iCh1, iCh2, lpfx, cx)
LPDV lpdv;      /* Far ptr to the device descriptor */
int ix;         /* The fraction's horizontal coordinate */
int iy;         /* The factionn's vertical coordinate */
char iCh1;      /* The numerator */
char iCh2;      /* The denominator */
LPFX lpfx;
int cx;         /* The character width */
    {
    long lid;           /* The font id */
    int sx1;            /* The normal font's horizontal scale factor */
    int sy1;            /* The normal font's vertical scale factor */
    int sx2;            /* The reduced font's horizontal scale factor */
    int sy2;            /* The reduced font's vertical scale factor */
    int dy;             /* The fraction bar's vertical offset */
    LPDF lpdf;		/* Ptr to the device font structure */


    if (lpdv)
        {
        lpdf = lpfx->lpdf;

        /* Save the font id and scale factor */
        lid = lpfx->lid;
        lpfx->lid = -1L;
        sx1 = lpfx->sx;
        sy1 = lpfx->sy;

        if (lpdf->dfPitchAndFamily & 1)
            {
            /* This code prints a fraction in a variable width font */

            /* Scale the font width by 65% and the height by 60% */
            sx2 = Scale(sx1, 65, 100);
            sy2 = Scale(sy1, 60, 100);
            lpfx->sx = sx2;
            lpfx->sy = sy2;
            SelectFont(lpdv, lpfx);

            /* Print the numerator */
            cx = Scale(lpfx->rgWidths[iCh1 - lpdf->dfFirstChar], 65, 100);
            ShowChar(lpdv, ix, iy, iCh1, cx, lpfx);

            /* Switch back to a normal sized font */
            lpfx->sx = sx1;
            lpfx->sy = sy1;
            SelectFont(lpdv, lpfx);

            /* Move down by 40% of the character height in points */
            dy = Scale(Scale(sy1, 72, lpdv->dh.iRes), 40, 100);


            /* Print the fraction bar */
            PrintChannel(lpdv, (LPSTR) "0 %d rmoveto\n", -dy);
            PrintChannel(lpdv, (LPSTR) "(\\274) show\n");

            /* Select the small font for the denominator */
            lpfx->sx = sx2;
            lpfx->sy = sy2;
            SelectFont(lpdv, lpfx);

            /* Print the denomonator */
            PrintChannel(lpdv, (LPSTR) "(%c) show\n", iCh2);
            }
        else
            {

            /* Use a half-size font for the numerator and denominator */
            sx2 = sx1/2;
            sy2 = sy1/2;
            lpfx->sx = sx2;
            lpfx->sy = sy2;
            SelectFont(lpdv, lpfx);
            ShowChar(lpdv, ix, iy, iCh1, cx/2, lpfx);

            ShowChar(lpdv, ix+cx/2, iy+sx2, iCh2, cx/2, lpfx);

            /* Show the faction bar in the full size font */
            lpfx->lid = lid;
            lpfx->sx = sx1;
            lpfx->sy = sy1;
            SelectFont(lpdv, lpfx);
            ShowChar(lpdv, ix, iy - sy1/8, 0x0bc, cx, lpfx);
            }

        /* Restore the font id and scale factor */
        lpfx->sx = sx1;
        lpfx->sy = sy1;
        lpfx->lid = lid;
        SelectFont(lpdv, lpfx);
        }
    }






/***********************************************************
* Name: ShowPower()
*
* Action: Print a superscripted character.
*
************************************************************
*/
BOOL FAR PASCAL ShowPower(lpdv, ix, iy, lpfx, cx, iCh)
LPDV lpdv;	    /* Ptr to the device descriptor */
int ix; 	    /* The horizontal coordinate */
int iy; 	    /* The vertical coordinate */
LPFX lpfx;	    /* Ptr to the extended device font structure */
int cx; 	    /* The character width */
int iCh;	    /* The character to print */
    {
    long lid;	    /* The font id */
    int sx;	    /* The horizontal scale factor */
    int sy;	    /* The vertical scale factor */

    lid = lpfx->lid;
    lpfx->lid = -1L;
    sx = lpfx->sx;
    sy = lpfx->sy;
    lpfx->sx = sx / 2;
    lpfx->sy = sy / 2;
    SelectFont(lpdv, lpfx);
    ShowChar(lpdv, ix, iy, iCh, cx, lpfx);
    lpfx->lid = lid;
    lpfx->sx = sx;
    lpfx->sy = sy;
    SelectFont(lpdv, lpfx);
    }




/********************************************************************
* Name: ANSI0xc6()
*
* Action: Simulate the AE ligature for the Couier font.
*
*********************************************************************
*/
BOOL FAR PASCAL ANSI0xc6(lpdv, ix, iy, lpfx, cx)
LPDV lpdv;		    /* Ptr to the device descriptor */
int ix; 		    /* The horizontal coordinate */
int iy; 		    /* The vertical coordinate */
LPFX lpfx;		    /* Ptr to the extended font structure */
int cx; 		    /* The computed character width */
    {
    int sx;
    int sy;
    long lid;
    BOOL fSimulate;         /* TRUE if the character must be simulated */

    fSimulate = FALSE;
    if ((lpfx->lpdf->dfPitchAndFamily & 0x0f0) == FF_MODERN)
        {
        if (lpdv)
            {
            sx = lpfx->sx;
            sy = lpfx->sy;
            lid = lpfx->lid;

            /* Reduce the character size to 75% of normal */
            lpfx->sx = Scale(sx, 75, 100);
            lpfx->sy = Scale(sx, 75, 100);
            cx = Scale(cx, 75, 100);

            ShowChar(lpdv, ix, iy, 'A', cx, lpfx);
            ShowChar(lpdv, ix + cx/2, iy, 'E', cx, lpfx);

            lpfx->lid = lid;
            lpfx->sx = sx;
            lpfx->sy = sy;
            SelectFont(lpdv, lpfx);

            }
        fSimulate = TRUE;
        }
    return(fSimulate);
    }





/********************************************************************
* Name: ANSI0xe6()
*
* Action: Simulate the ae ligature for the Couier font.
*
*********************************************************************
*/
BOOL FAR PASCAL ANSI0xe6(lpdv, ix, iy, lpfx, cx)
LPDV lpdv;
int ix;
int iy;
LPFX lpfx;
int cx;
    {
    int sx;
    int sy;
    long lid;
    BOOL fSimulate;

    fSimulate = FALSE;
    if ((lpfx->lpdf->dfPitchAndFamily & 0x0f0) == FF_MODERN)
        {
        if (lpdv)
            {
            sx = lpfx->sx;
            sy = lpfx->sy;
            lid = lpfx->lid;

            /* Reduce the character size to 75% of normal */
            lpfx->sx = Scale(sx, 75, 100);
            lpfx->sy = Scale(sx, 75, 100);
            cx = Scale(cx, 75, 100);

            ShowChar(lpdv, ix, iy, 'a', cx, lpfx);
            ShowChar(lpdv, ix + cx/2, iy, 'e', cx, lpfx);

            lpfx->lid = lid;
            lpfx->sx = sx;
            lpfx->sy = sy;
            SelectFont(lpdv, lpfx);
            }
        fSimulate = TRUE;
        }
    return(fSimulate);
    }






/********************************************************************
* Name: ANSI0xb5()
*
* Action: Simulate the Greek mu character.
*
*********************************************************************
*/
BOOL FAR PASCAL ANSI0xb5(lpdv, ix, iy, lpfx, cx)
LPDV lpdv;
int ix;
int iy;
LPFX lpfx;
int cx;
    {
    int iCh;

    if (lpdv)
        {
        SymbolFont(lpdv, lpfx);
        ShowChar(lpdv, ix, iy, 0x06d, cx, lpfx);
        SelectFont(lpdv, lpfx);
        }
    return(TRUE);
    }




/********************************************************************
* Name: ANSI0xa9()
*
* Action: Simulate the copyright symbol by extracting it from
*	  PostScript's symbol font.
*
*********************************************************************
*/
BOOL FAR PASCAL ANSI0xa9(lpdv, ix, iy, lpfx, cx)
LPDV lpdv;
int ix;
int iy;
LPFX lpfx;
int cx;
    {
    int iCh;

    if (lpdv)
        {
        if ((lpfx->lpdf->dfPitchAndFamily & 0x0f0) == FF_ROMAN)
            iCh = 0x0d3;    /* copyrightserif */
        else
            iCh = 0x0e3;    /* copyrightsanserif */
        SymbolFont(lpdv, lpfx);
        ShowChar(lpdv, ix, iy, iCh, cx, lpfx);
        SelectFont(lpdv, lpfx);
        }
    return(TRUE);
    }







/********************************************************************
* Name: ANSI0xae()
*
* Action: Simulate the registered symbol by extracting it from
*	  PostScript's symbol font.
*
*********************************************************************
*/
BOOL FAR PASCAL ANSI0xae(lpdv, ix, iy, lpfx, cx)
LPDV lpdv;
int ix;
int iy;
LPFX lpfx;
int cx;
    {
    int iCh;

    if (lpdv)
        {
        if ((lpfx->lpdf->dfPitchAndFamily & 0x0f0) == FF_ROMAN)
            iCh = 0x0d2;    /* registerserif */
        else
            iCh = 0x0e2;    /* registersanserif */

        SymbolFont(lpdv, lpfx);
        ShowChar(lpdv, ix, iy, iCh, cx, lpfx);
        SelectFont(lpdv, lpfx);
        }
    return(TRUE);
    }







/********************************************************************
* Name: ANSI0xbc()
*
* Action: Simulate the one-fourth symbol.
*
*********************************************************************
*/
BOOL FAR PASCAL ANSI0xbc(lpdv, ix, iy, lpfx, cx)
LPDV lpdv;
int ix;
int iy;
LPFX lpfx;
int cx;
    {
    if (lpdv)
        ShowFraction(lpdv, ix, iy, '1', '4', lpfx, cx);
    return(TRUE);
    }










/********************************************************************
* Name: ANSI0xbc()
*
* Action: Simulate the one-half symbol.
*
*********************************************************************
*/
BOOL FAR PASCAL ANSI0xbd(lpdv, ix, iy, lpfx, cx)
LPDV lpdv;
int ix;
int iy;
LPFX lpfx;
int cx;
    {
    if (lpdv)
        ShowFraction(lpdv, ix, iy, '1', '2', lpfx, cx);
    return(TRUE);
    }









/********************************************************************
* Name: ANSI0xbe()
*
* Action: Simulate the three-fourths symbol.
*
*********************************************************************
*/
BOOL FAR PASCAL ANSI0xbe(lpdv, ix, iy, lpfx, cx)
LPDV lpdv;
int ix;
int iy;
LPFX lpfx;
int cx;
    {

    if (lpdv)
        ShowFraction(lpdv, ix, iy, '3', '4', lpfx, cx);
    return(TRUE);
    }











/********************************************************************
* Name: ANSI0xf0e()
*
* Action: Simulate the lower-case funny D symbol.
*
*********************************************************************
*/
BOOL FAR PASCAL ANSI0xf0(lpdv, ix, iy, lpfx, cx)
LPDV lpdv;
int ix;
int iy;
LPFX lpfx;
int cx;
    {
    /* The partial diff character in the symbol font looks similar */
    if (lpdv)
        {
        SymbolFont(lpdv, lpfx);
        ShowChar(lpdv, ix, iy, 0x0b6, cx, lpfx);  /* Partial diff */
        SelectFont(lpdv, lpfx);
        }

    return(TRUE);
    }









/********************************************************************
* Name: ANSI0xb9()
*
* Action: Simulate the superscript-1 symbol.
*
*********************************************************************
*/
BOOL FAR PASCAL ANSI0xb9(lpdv, ix, iy, lpfx, cx)
LPDV lpdv;
int ix;
int iy;
LPFX lpfx;
int cx;
    {
    if (lpdv)
        ShowPower(lpdv, ix, iy, lpfx, cx, '1');
    return(TRUE);
    }







/********************************************************************
* Name: ANSI0xb2()
*
* Action: Simulate the superscript-2 symbol.
*
*********************************************************************
*/
BOOL FAR PASCAL ANSI0xb2(lpdv, ix, iy, lpfx, cx)
LPDV lpdv;
int ix;
int iy;
LPFX lpfx;
int cx;
    {
    if (lpdv)
        ShowPower(lpdv, ix, iy, lpfx, cx, '2');
    return(TRUE);
    }









/********************************************************************
* Name: ANSI0xb3()
*
* Action: Simulate the superscript-3 symbol.
*
*********************************************************************
*/
BOOL FAR PASCAL ANSI0xb3(lpdv, ix, iy, lpfx, cx)
LPDV lpdv;
int ix;
int iy;
LPFX lpfx;
int cx;
    {
    if (lpdv)
        ShowPower(lpdv, ix, iy, lpfx, cx, '3');
    return(TRUE);
    }










/********************************************************************
* Name: ANSI0xb1()
*
* Action: Simulate the plus-or-minus symbol.
*
*********************************************************************
*/
BOOL FAR PASCAL ANSI0xb1(lpdv, ix, iy, lpfx, cx)
LPDV lpdv;
int ix;
int iy;
LPFX lpfx;
int cx;
    {

    if (lpdv)
        {
        SymbolFont(lpdv, lpfx);
        ShowChar(lpdv, ix, iy, 0xb1, cx, lpfx);
        SelectFont(lpdv, lpfx);
        }
    return(TRUE);
    }









/********************************************************************
* Name: ANSI0xb1()
*
* Action: Simulate the logical-not symbol.
*
*********************************************************************
*/
BOOL FAR PASCAL ANSI0xac(lpdv, ix, iy, lpfx, cx)
LPDV lpdv;
int ix;
int iy;
LPFX lpfx;
int cx;
    {

    if (lpdv)
        {
        SymbolFont(lpdv, lpfx);
        ShowChar(lpdv, ix, iy, 0xd8, cx, lpfx);
        SelectFont(lpdv, lpfx);
        }
    return(TRUE);
    }






/****************************************************************
* Name: GetSimulate()
*
* Action: Returns a far pointer to a procedure which can simulate
*	  a character.	If no function is available, then NULL
*	  is returned.
*
*****************************************************************
*/
FARPROC FAR PASCAL GetSimulate(iCh)
int iCh;
    {
    FARPROC lpfn;

    lpfn = (FARPROC)(long)NULL;

#if 0	/* 87-1-16 sec */

    switch(iCh & 0x0ff)
        {
        case 0x0c6:                 /* AE Ligature */
            lpfn = ANSI0xc6;
            break;
        case 0x0e6:                 /* ae Ligature */
            lpfn = ANSI0xe6;
            break;
        case 0x0a9:                 /* Copyright */
            lpfn = ANSI0xa9;
            break;
        case 0x0ae:                 /* Registered */
            lpfn = ANSI0xae;
            break;
        case 0x0bc:                 /* One fourth */
            lpfn = ANSI0xbc;
            break;
        case 0x0bd:                 /* One half */
            lpfn = ANSI0xbd;
            break;
        case 0x0be:                 /* Three fourths */
            lpfn = ANSI0xbe;
            break;
        case 0x0f0:                 /* Lowercase version of 0xd0 */
            lpfn = ANSI0xf0;
            break;
        case 0x0b1:                 /* Plus or Minus sign */
            lpfn = ANSI0xb1;
            break;
        case 0x0b2:
            lpfn = ANSI0xb2;        /* Squared sign */
            break;
        case 0x0b3:
            lpfn = ANSI0xb3;        /* Cubed sign */
            break;
        case 0x0b9:                 /* Superscript 1 */
            lpfn = ANSI0xb9;
            break;
        case 0x0ac:                 /* Logical not character */
            lpfn = ANSI0xac;
            break;
        case 0x0b5:                 /* Greek micro sign */
            lpfn = ANSI0xb5;
            break;
        }
#endif

    return(lpfn);
    }
