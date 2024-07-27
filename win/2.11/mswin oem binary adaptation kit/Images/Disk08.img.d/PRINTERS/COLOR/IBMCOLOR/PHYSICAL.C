#define NO_XMOVETO	1
#define NO_YMOVETO	1
#define NO_CH_LINE_OUT	1
#define NO_DUMP		1

#include "phys1.c"


/* dumps the current band of graphics output */

void NEAR PASCAL dump(lpDevice)
LPDEVICE lpDevice;
{
    register LPSTR bmp, temp;
    short x, y, offset, BitOffset;
    LPSTR buffer;
    unsigned char BitRem, tmp1, tmp2;

    buffer = lpDevice->epBuf;
    if (lpDevice->epType == DEV_LAND)
        {
        offset = lpDevice->epXOffset - BAND_HEIGHT;

        for (x = 0; x < BAND_LINE;x++)
            {
            temp = buffer + lpDevice->epPageWidth;
	    BitOffset = NPINS * x;
	    BitRem = BitOffset % 8;
            bmp = (LPSTR) lpDevice->epBmp + BitOffset / 8;
            for (y = lpDevice->epPageWidth; y; --y, bmp += BAND_HEIGHT / 8) {
		tmp1 = (unsigned char) *bmp << (unsigned char) BitRem;
		tmp2 = (unsigned char) *(bmp + 1) >> (unsigned char) (8- BitRem);
                *--temp = (tmp1 + tmp2) | 1;
	    }
            line_out(lpDevice, buffer, lpDevice->epPageWidth, x * NPINS + offset);
            }
        }
    else    /* portrait */
    {
        bmp = lpDevice->epBmp;
        offset = lpDevice->epYOffset - BAND_HEIGHT;
        for (x = 0; x < BAND_LINE; x++)
        {
            dmTranspose(bmp, buffer, lpDevice->epPageWidth >> 3);
            temp = buffer;
	    /* set all low bits so that line_out can output 8-bit bytes 
	       instead of 7-bit bytes */
            for (y = 0; y < lpDevice->epPageWidth; y++) {
            	*temp |= 1;
            	temp++;
            }
            line_out(lpDevice, buffer, lpDevice->epPageWidth, x * NPINS + offset);
            bmp += lpDevice->epPageWidth / 8 * NPINS;
        }
    }
    /* when we finish dumping a band there should not be any character
       strings left in the queue */
    if (MinPQ(lpDevice->epYPQ) != ERROR)
            FatalExit(MinPQ(lpDevice->epYPQ));
}

/* output all the character strings before y, return zero if there is
   nothing left in the queue of character strings */

BOOL NEAR PASCAL ch_line_out(lpDevice, y)
LPDEVICE lpDevice;
short y;
{
        short  tag;
        ELEMENT far * ele;
        LPSTR base;
        HANDLE yPQ;

        yPQ = lpDevice->epYPQ;
        base = GlobalLock(lpDevice->epHeap);

        for (;;)
        {
                if ((tag = MinPQ(yPQ)) == ERROR)
                        break;

                ele = (ELEMENT far *) (base + tag);

                if (ele->y >= y)
                        break;
                if (tag != ExtractPQ(yPQ))
                        {
                        /* FatalExit(0x05) */
                        break;
                        }
                YMoveTo(lpDevice, ele->y);
                XMoveTo(lpDevice, ele->x, TRUE);
                myWrite(lpDevice, ele->pstr, ele->count);
                lpDevice->epCurx += ele->size;
		if (ele->size % 7) 	/* not multiples of 7 */
                	lpDevice->epCurx--;
        }
        GlobalUnlock(lpDevice->epHeap);

        return (tag == ERROR? FALSE: TRUE);
}

/* moves the cursor the position y, and update the lpDevice->epCury
   to reflect the changes.
   Since the color printer can only moves in 1/144" and the resolution is 
   in 1/84", the cursor is moved to the nearest y.
   This roundoff error in y direction does not have any noticeable effect
   since the printer moves in multiples of 7/84" ( = 12/144" ).
   The roundoff error is within 1/84" at all times and there is no
   cumulative error.
*/

FAR PASCAL YMoveTo(lpDevice, y)
LPDEVICE lpDevice;
short y;
{
        short diff144, cury144, prevy144;  /* in 1/144"s */

	if (y <= lpDevice->epCury)
               return;

	prevy144 = lpDevice->epCury * 12 / 7;
	cury144 = y * 12 / 7;
        diff144 = cury144 - prevy144;

        /* move the printer head down by diff144/144" */

        for (; diff144 > 0; diff144 -= MAXDELY)
                {
                dely.cnt = diff144 >= MAXDELY? MAXDELY: diff144;
                myWrite(lpDevice, (LPSTR) &dely, dely.length);
                }

        lpDevice->epCury = y ;	/* in 1/84"s */
        /* for some printers feed causes an immediate carriage return
           this will take care of those printers */
        myWrite(lpDevice, ESCEXP(escapecode.cr));
        lpDevice->epCurx = 0;
}

/* moves the cursor postion to x, with or without microspace justification
   as specified by ms.
   returs the difference between actual cursor position and the x
   position requested (in 1/140"s)
   Since the color printer can only moves in 1/120" and the resolution is 
   in 1/140", the cursor is moved in multiples of 7/140" ( = 6/120" ) when
   ms is true.  Otherwise, it moves to the nearest 1/120"s.
*/
short FAR PASCAL XMoveTo(lpDevice, x, ms)
LPDEVICE lpDevice;
short x;
short ms;           /* whether to use ms justification or not */
{
        short diff120, prevx120, curx120;

        if (x < 0 || x > lpDevice->epPageWidth)
                {   /* should never happen */
                return -1;
                }
        if (x < lpDevice->epCurx)
                {
		/*
                 * if (lpDevice->epMode & DRAFTFLAG)
                 *      return 0;
		 */
                myWrite(lpDevice, ESCEXP(escapecode.cr));
                lpDevice->epCurx = 0;
		prevx120 = 0;
		curx120 = x * 6 / 7;
        	diff120 = curx120 - prevx120;
                }
        else 
		{
		prevx120 = lpDevice->epCurx * 6 / 7;
		curx120 = x * 6 / 7;
        	diff120 = curx120 - prevx120;
                }

        if (delx.cnt = diff120 / 6 * 6)
                myWrite(lpDevice, (LPSTR) &delx, delx.length);

        if (ms)
                {
        	if (delx.cnt = diff120 - delx.cnt)
                	myWrite(lpDevice, (LPSTR) &delx, delx.length);
        	lpDevice->epCurx = x;
                }
        else 
		lpDevice->epCurx = curx120 / 6 * 7;
        return x - lpDevice->epCurx;
}
