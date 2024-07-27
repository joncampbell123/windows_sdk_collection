/* SUFONT.C -- select fonts for screen and iodevices */

/*  History
**
**   8 jul 87   mp  start history
**  31 jan 89   wch references to Is360K removed - always FALSE
*/

#include <stdio.h>
#include <string.h>
#include "setup.h"

/* special values for res[0] */
#define RES_CONT 1                      /* continuusscaling font */
#define RES_SPEC 2                      /* device specific font */

#define RESMAX 1 + 1 + 2 * 6 + 1        /* # of resolutioninfos: */
                                        /* for screen, continousscalling and */
                                        /* 2 for each printer per port(6) */
struct ResInfo {
    int r_res[3];                       /* resolution integers */
    union dfile *r_pfont;               /* pointer to appropriate font */
    unsigned r_penalty;                 /* computed penalty */
};

/* forward declarations */
void CopyRes(int *, int *);
static void CheckFontGroup(union dfile *, struct ResInfo *);
static void StoreFont(union dfile *);

extern union dfile * pselect[];


/* select the appropriate fonts for the screen and iodevices */

void SelectFonts()
{
    struct ResInfo r[RESMAX];           /* resolution infos */
    struct ResInfo *pr = r;
    union dfile *pd;                    /* pointer to printer/font info */
    union dfile *pdLast;                /* pointer to previous info */

                                        /* screen resolution */
/* BILLHU */
    pdLast = (S_DISPLAY < SSIZE) ? pselect[S_DISPLAY] : NULL;
    CopyRes(pr->r_res, pdLast->fon.res);
    pr++;
                                        /* vector fonts */
    pr->r_res[0] = 0;
    pr->r_res[1] = RES_CONT;
    pr++;
                                        /* printer resolutions */
/* BILLHU
    for (pd = RecNPtr(S_PRINTER); pd && (pr < r + (RESMAX - 1));
*/
    for (pd = (S_PRINTER < SSIZE) ? pselect[S_PRINTER] : NULL; pd && (pr < r + (RESMAX - 1));
                                                        pd = pd->iodev.p) {
        CopyRes(pr->r_res, pd->iodev.res1);
        pr++;
        if (pd->iodev.res2[1] && (pr < r + (RESMAX - 1))) {
            CopyRes(pr->r_res, pd->iodev.res2);
            pr++;
        }
    }
    pr->r_res[1] = 0;                   /* end of list */

    /* select fonts */

    pd = (pdLast = (union dfile *)&pheads[N_PRINTFONTS])->fon.p;
    while (pd) {
        if (pd->fon.res[1]) {
            CheckFontGroup(pd, r);
          			/* 360K setup: only first font set */
            pd = pdLast->fon.p;
        } else {
            pd = (pdLast = pd)->fon.p;
        }
    }
}

/* copies resolution numbers */

void CopyRes(piDst, piSrc)
int *piDst, *piSrc;
{
    int i = 3;

    while (i--) {
        *piDst++ = *piSrc++;
	 }
}


/* checks a group of fonts (e.g. all Courier fonts) which fonts are needed */
/* for resolutions in array r */

static void CheckFontGroup(pd, r)
union dfile *pd;                        /* actual font */
struct ResInfo r[];                     /* array of resolution infos */
{
    CHAR *name = pd->n.descr;           /* name of font group */
    int iLen;                           /* length of font group name */
    struct ResInfo *pr, *pr1;           /* pointer to resolution info array */
    unsigned uPenalty;
    unsigned *pu1, *pu2;

    iLen = strchr(name, ' ') - name;    /* compute length of font name */

    if (pd->fon.res[0] == 0) {           /* not raster font */
        for (pr = r; pr->r_res[1]; pr++)
            if (pr->r_res[0] == 0 && pr->r_res[1] == pd->fon.res[1]) {
                StoreFont(pd);
                break;
            }
        pd->fon.res[1] = 0;     /* avoid second test of this font! */
    } else {
        for (pr = r; pr->r_res[1]; pr++) {         /* initialize ResInfos */
            pr->r_pfont = NULL;
            pr->r_penalty = -1;
        }
        for (;pd; pd = pd->fon.p) {
            if (strncmp(name, pd->n.descr, iLen) == 0) {      /* same group */
                for (pr = r; pr->r_res[1]; pr++) {       /* all resolutions */
                    if (pr->r_res[0]) {                /* only raster fonts */
/* BILLHU
                        uPenalty = ComputeFontPenalty(pr->r_res, pd->fon.res);
*/
	                    pu1 = pr->r_res;
	                    pu2 = pd->fon.res;
                        uPenalty = abs(*pu1++ - *pu2++) * 10;
                        uPenalty += abs(*pu1++ - *pu2++);
                        uPenalty += 2 * abs(*pu1++ - *pu2++);
                        if (pr->r_penalty > uPenalty) {  /* compute minimum */
                            pr->r_penalty = uPenalty;
                            pr->r_pfont = pd;
                        }
                    }
                }
                pd->fon.res[1] = 0;     /* avoid second test of this font! */
            }
        }
        for (pr = r; pr->r_res[1]; pr++)
            if (pd = pr->r_pfont) {
                StoreFont(pd);
            }
    }
}

/* Compute how usefull a special font for a given resolution is */

/* BILLHU
static unsigned ComputeFontPenalty(pu1, pu2)
unsigned *pu1, *pu2;                    ** pointer to resolutions **
{
    unsigned u = 10;

    u *= abs(*pu1++ - *pu2++);
    u += abs(*pu1++ - *pu2++);
    u += 2 * abs(*pu1++ - *pu2++);

    return u;
}
*/

/* store font in list of files which have to be copied */

static void StoreFont(pdfont)
union dfile *pdfont;                    /* pointer to font */
{
    union dfile *pd;                    /* pointer to font i */
    union dfile *pdLast = (union dfile *)&pheads[N_PRINTFONTS];
                                        /* pointer to root of font infos */
    while (pd = pdLast->fon.p) {
        if (pd == pdfont) {
            pdLast->fon.p = pd->fon.p;              /* close font chain */

/* BILLHU
            pd->fon.p = RecNPtr(S_FONT); */          /* add at beginning of */
            pd->fon.p = (S_FONT < SSIZE) ? pselect[S_FONT] : NULL;
            StoreStandardItem(S_FONT, pd);      /* selected fonts chain */
            break;
        }
        pdLast = pd;
    }
}
