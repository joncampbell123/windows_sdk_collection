#include "nodefs.h"
#include "windows.h"
#include "winexp.h"
#include "spool.h"
#include "spooler.h"

FAR SpoolerKey(hWnd, wParam)
HWND hWnd;
WORD wParam;
{
        register short jobptr;
        HDC hDC;
        RECT rect;

        jobptr = curjob;

        switch(wParam)
        {
        case VK_UP:
                if (--jobptr < 0)
                        return;
                break;

        case VK_DOWN:
                if (++jobptr == maxline)
                        return;
                break;

        case VK_TAB:
                if (GetKeyState(VK_SHIFT) < 0)      /* down, back tab */
                        {
                        if (--jobptr < 0)
                                return;
                        while (screen[jobptr] >= MAXPORT)
                               --jobptr;
                        }
                else
                        {
                        if (++jobptr < maxline)
                                while (screen[jobptr] >  MAXPORT)
                                        jobptr++;
                        if (jobptr >= maxline)
                                return;
                        }
                break;

        default:
                return;
        }

        if (jobptr != curjob)
                {
                register short scrnline;

                GetClientRect(hWnd, &rect);
                scrnline = rect.bottom / charheight;
                if (jobptr >= tos && jobptr < tos + scrnline)
                        {
                        /* new job still fits in the screen -- no need to scroll or */
                        /* repaint                                                  */
                        hDC = GetDC(hWnd);
                        InvertLine(hDC, curjob, rect.right);
                        InvertLine(hDC, curjob = jobptr, rect.right);
                        ReleaseDC(hWnd, hDC);
                        }
                else
                        {
                        /* need to repaint, set up the new tos and jobptr */
                        /* screen is shorter than one line top of screen is
                           the new jobptr */
                        if (jobptr < tos || !scrnline)
                                tos = jobptr;
                        else
                                tos = jobptr - scrnline + 1;
                        curjob = jobptr;
                        SpoolerPaint(hWnd);
                        }
                EnableMenuItem(GetMenu(hWnd), TERMINATE, screen[jobptr] > maxport? MF_ENABLED: MF_GRAYED);
                SetScrollPos(hWnd, SB_VERT, tos * 100 / (maxline - 1), TRUE);
                }
}


/* return TRUE if need to repaint */

FAR SpoolerVertScroll(hWnd, code, posNew)
HWND hWnd;
short code;
short posNew;
{
    RECT rect;
    register short scrnline, newtos,
                   scrollline;  /* number of lines to allow to scroll away */
    GetClientRect(hWnd, (LPRECT) &rect);
    scrnline = rect.bottom / charheight;
    scrollline = maxline - 1;
    switch (code)
        {
        case SB_LINEDOWN:
            if (tos  >= scrollline)
                return FALSE;
            tos++;
            break;

        case SB_LINEUP:
            if (!tos)
                return FALSE;
            tos--;
            break;

        case SB_PAGEUP:
            if (!tos)
                return FALSE;
            if ((tos -= scrnline) < 0)
                tos = 0;
            break;

        case SB_PAGEDOWN:
            if (tos == scrollline)
                    return FALSE;
            if (tos + scrnline >= scrollline)
                    tos = scrollline;
            else
                    tos += scrnline;
            break;

        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            if ((newtos = (posNew * scrollline / 100)) == tos)
                    return FALSE;
            tos = newtos;
            break;

        default:
            return FALSE;
        }
        SetScrollPos(hWnd, SB_VERT, tos * 100 / scrollline, TRUE);
        return TRUE;
}
