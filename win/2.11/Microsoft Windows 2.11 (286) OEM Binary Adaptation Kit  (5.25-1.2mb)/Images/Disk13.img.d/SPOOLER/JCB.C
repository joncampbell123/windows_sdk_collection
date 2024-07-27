#include "printer.h"
#include "spool.h"
#include "spooler.h"


#define GMEM_SHARE 0x2002

short NEAR PASCAL SpoolRead(page, server, size)
PAGE far *page;
SERVER *server;
short size;
{
        register short fn;

        /* not a hard disk */
        if (!(fn = page->filenum))
                {
                if ((fn = OpenFile(page->fileBuf.szPathName, &page->fileBuf, SP_REOPEN)) == EOF)
                        return EOF;
                _llseek(fn, (long) server->filestart, 0);
                }

        if (_lread(fn, (LPSTR) server->buffer, size) != size)
                return EOF;

        if (!page->filenum)
                _lclose(fn);
        return size;
}

/* returns true if the job queue has changed */

UpdateScreen()
{
        register short i, j;
        HANDLE hJCB;
        HANDLE *screenptr;
        short changed = FALSE;

        for (screenptr = screen, i = 0; i < MAXPORT; i++)
                {
                if (!servers[i])
                        continue;
                if (!servers[i]->jobcnt && !(servers[i]->valid & SP_VALID))
                        {
                        FreePort(i);
                        continue;
                        }
                *screenptr++ = i;
                Copy((LPSTR)screenptr, (LPSTR)servers[i]->queue, servers[i]->jobcnt * sizeof(HANDLE));
                screenptr += servers[i]->jobcnt;
                }
        maxline = screenptr - screen;
        if (maxline && tos > maxline - 1)
                tos = maxline - 1;
        *screenptr = (HANDLE) 0;
}


refill(server, jcb)
SERVER * server;
JCB far *jcb;
{
        register PAGE far *page;
        HANDLE hPage;
        DIALOGMARK far *p;
        short result = 0;
        unsigned size;
        unsigned long lsize;

        if (server->pg == jcb->pagecnt)
                return (jcb->type & JB_ENDDOC)? EOF: result;

        hPage = jcb->page[server->pg];

        /* should never happen */
        if (server->pg > jcb->pagecnt || !hPage
                || !(page = (PAGE far *) GlobalLock(hPage)))
                {
                SpoolerError(IDS_NAME, IDS_TEMPFILE, MB_ICONEXCLAMATION, 0);
                ResetPort(server->portfn);
                return EOF;
                }

        if (!page->spoolsize)
                goto EndPage;

        for (;;)
		{
                if ((lsize = server->fileend - server->filestart) > SPOOL_LEN)
                        size = SPOOL_LEN;
                else
			size = lsize;

                if (size || !page->spoolsize)
                        break;

                if (!server->dlgptr)    /* new page */
                        {
                        server->filestart = server->fileend = 0;
                        server->type = DIALOG;
                        if (page->fileBuf.fFixedDisk)
                                page->filenum = OpenFile(page->fileBuf.szPathName, &page->fileBuf, SP_REOPEN);
                        else
                                page->filenum = 0;
                        }
                else
                        server->filestart = server->fileend;

                p  = &page->dialog[server->dlgptr];
                if (server->type == TEXT)
                        {
                        server->fileend += p->size;
                        server->type = DIALOG;
                        }
                else
                        {
                        if (++server->dlgptr >= page->dlgptr)
                                {
                                /* last piece of text */
                                server->fileend = page->spoolsize;
                                server->dlgptr = 0;
                                }
                        else
                                server->fileend = p[1].adr;
                        server->type = TEXT;
                        }
                }

        if (SpoolRead(page, server, size) != size)
                {
                /* fix this later */
                server->type = EOF;
                GlobalUnlock(hPage);
                SpoolerError(IDS_NAME, IDS_TEMPFILE, MB_ICONEXCLAMATION, 0);
                ResetPort(server->portfn);
                return EOF;
                }

        server->bufstart = 0;
        server->bufend = size;
        if ((server->filestart += size) == page->spoolsize || !page->spoolsize)
                {
                if (page->filenum)
                        _lclose(page->filenum);
                jcb->size -= page->spoolsize;
                GetSpoolJob(SP_DISKFREED, CalcDiskAvail());
EndPage:
                DeletePathname(page->fileBuf.szPathName);
                jcb->page[server->pg++] = 0;
                GlobalUnlock(hPage);
                GlobalFree(hPage);
                return result;
                }

jobexit:
        GlobalUnlock(hPage);
        return result;
}

FreeAll(hJCB)
HANDLE hJCB;
{
        register short i;
        short hPage;
        PAGE far *page;
        JCB far *jcb;

        jcb = (FJCB) GlobalLock(hJCB);
        for (i = 0; i <= jcb->pagecnt; i++)
                {
                if (!(hPage = jcb->page[i]))
                        continue;
                page = (PAGE FAR *) GlobalLock(hPage);
                DeletePathname(page->fileBuf.szPathName);
                GlobalUnlock(hPage);
                GlobalFree(hPage);
                }

        jcb->size = 0;
        GetSpoolJob(SP_DISKFREED, CalcDiskAvail());
        jcb->type |= JB_INVALIDDOC;
        GlobalUnlock(hJCB);

        /* don't free it if the app doesn't know it is deleted */
        if (jcb->type & JB_ENDDOC)
                GlobalFree(hJCB);
        else
                /* realloc to smaller object, need not check */
                GlobalReAlloc(hJCB, (long) 2, GMEM_SHARE);

}

pause(curjob, pauseflag)
short curjob, pauseflag;
{
        short portnum;

        if (curjob >= maxline)
                 return FALSE;

        while (screen[curjob] >= MAXPORT)
                 --curjob;

        portnum = screen[curjob];
        if (pauseflag > 0)
                servers[portnum]->pause |= pauseflag;
        else
                servers[portnum]->pause &= pauseflag;
}


/* add/remove hJCB from the server queue */

AddSpoolJob(server, hJCB, addflag)
SERVER *server;
HANDLE hJCB;
short addflag;
{
        short cnt;
        HANDLE *jcb1, *jcb2, temp = 0;
        JCB far *jcb;
	BOOL FAR PASCAL IsIconic(HWND);
        void FAR SendMessage();

        if (addflag == SP_NEWJOB)
                {
                if (server->jobcnt >= MAXSPOOL)
                        {
                        SpoolerError(IDS_NAME, IDS_MAXJOB, MB_OK | MB_SYSTEMMODAL | MB_ICONHAND, 0);
                        return EOF;
                        }

                server->queue[server->jobcnt++] = hJCB;
                }
        else
                {
                if (server->portfn != SP_RETRY && server->portfn != SP_CANCEL)
                        {
                        ClosePort(server->portfn);
                        server->portfn = SP_RETRY;
                        }
                for (jcb1 = server->queue, cnt = 0; cnt < server->jobcnt; cnt++, jcb1++)
                        if (hJCB == *jcb1)
                                    break;
                --server->jobcnt;
                if (!cnt)
                        server->hJCB = 0;

                for (jcb2 = jcb1++;cnt < server->jobcnt;cnt++)
                        *jcb2++ = *jcb1++;
                }

        if (!server->hJCB && server->jobcnt)
                {
                register PAGE far *page;


                server->pg = 0;
                server->dlgptr = 0;
                hJCB = server->hJCB = *server->queue;
                server->bufstart = server->bufend =
                        server->filestart = server->fileend = 0;
                server->type = DIALOG;
                jcb = (FJCB) GlobalLock(hJCB);
                /* check here to see if port was valid */
                refill(server, jcb);

	    if (addflag == SP_DELETEJOB)
	      {
                /* file might be open, close it in case open (raor) 10/26/87 */
                if(page = (PAGE far *) GlobalLock(jcb->page[server->pg])) {
                    if (page->fileBuf.fFixedDisk  &&  page->filenum )
                        _lclose(page->filenum);
                    page->filenum = 0;
                    GlobalUnlock(jcb->page[server->pg]);
		}
	      }

                GlobalUnlock(hJCB);
		}

        if (addflag == SP_NEWJOB)
                ActiveJobs++;
        else
		{
                ActiveJobs--;
                SendMessage((HWND)-1, WM_SPOOLERSTATUS, PR_JOBSTATUS, (DWORD)ActiveJobs);

		/* close automatically when last job is done when we are
		   iconic - 7/27/87 Bob Matthews */
		if (ActiveJobs == 0)
		    TryAutoShutdown();
		}
        return TRUE;
}


long near PASCAL CalcDiskAvail()
{
        register short i, j;
        unsigned long diskavail = 0;

        for (i = 0; i < MAXPORT; i++)
                {
                if (!servers[i])
                        continue;
                for (j = 0; j < servers[i]->jobcnt; j++)
                        {
                        register HANDLE hJCB;
                        FJCB jcb;

                        hJCB = servers[i]->queue[j];
                        jcb = (JCB far *) GlobalLock(hJCB);
                        diskavail += jcb->size;
                        GlobalUnlock(hJCB);
                        if (!(jcb->type & JB_ENDDOC))
                                break;
                        }
                }
        return diskavail;
}
