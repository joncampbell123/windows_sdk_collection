/* SUCOMPAT.C -- Check CONFIG.SYS for drivers incompatible with Windows.
**
*/

/* History
** 1.10 Windows ===========================================================
**
** 10 jun 88	peterbe	Moved txCompat[] to SUDATA.C, fixed scanning.
** 09 jun 88	peterbe	Added code/data for [compatibility] section of
**			SETUP.INF
** 01 feb 89    wch     hardcoded row == 11, removed local var 'i'.
** 01 feb 89    wch     added logic so incompatible EMM drivers allows setup
**                      to continue but suggest USERS.
** ====================================================================== */

#define LINT_ARGS

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <io.h>

#include "setup.h"

#define LINESIZE 120
#define XSIZE 60

extern BOOL bCompatHiMem;
extern textline LeaveSetupMsg[];
extern textline CompatChoices[];

CheckDriverCompat()
{
    textline txDummy[2];	/* will contain DEVICE= lines */
    FILE * in;
    char fname[13];
    union dfile * p0;
    union dfile * p;
    int nentries;	/* number of drivers listed in [compatibility] */
    int nfound;		/* number of lines in CONFIG.SYS which match */
    int iChoice;
    int row;
    char inline[LINESIZE];	/* line read from CONFIG.SYS */
    char xline[XSIZE];		/* Upper-case tail of line from CONFIG.SYS */
    char *pl;
    char *pl0;
    char *pOld;

    /* return if there are no entries in [compatibility] */
    for (nentries = 0, p = p0 = pheads[N_COMPAT]; p != NULL; p = p-> n.p)
	nentries++;

    if (nentries == 0)
	return(0);

    /* get full path name for CONFIG.SYS */
    strcpy(fname, "c:\\config.sys");
    *fname  = LocateBootDrive();
    if ((*fname < 'A') || (*fname > 'Z'))
	return(0);
    
    if (NULL != (in = fopen(fname,"r")))
	{
	/* find last line of txCompat[] screen */
/* BILLHU hardcoded this from looking at txCompat in sudata.c
	for(i = 0; (txCompat[i].row != 0) && (txCompat[i].line != NULL);i++)
	    row = txCompat[i].row + 2;
*/
	row = 11;

	/* initialize 'txDummy' */
	txDummy[1].row = txDummy[1].col = 0;
	txDummy[1].line = NULL;

	/* read lines from CONFIG.SYS and scan them for DEVICE=<illegal> */
	nfound = 0;
	while (NULL != fgets(inline, LINESIZE, in))
	    {
	    /* scan past whitespace */
	    pl0 = pl = inline + strspn(inline, " \t");
	    /* scan for DEVICE */
	    if (0 == strnicmp(pl, "device", 6))
		{
		/* skip past whitespace and check for '=' */
		pl += 6 + strspn(pl + 6, " \t");
		if (0 == strnicmp(pl, "=", 1))
		    {
		    /* copy line tail to another buffer and make upper-case */
		    strupr(strncpy(xline, pl, XSIZE-1));
		    /* look for a 'bad' driver name in the string.
		    ** It must be preceded by whitespace or backslash
		    ** or colon. */
		    for (p = p0; p != NULL; p = p->n.p)
			{
			if ((NULL !=
			     (pl = strstr(xline, strupr(p->n.fname)) ) ) &&
			   (NULL != strchr("\\: \t", *(pl - 1)) ) )
			    {
			    /* a 'bad' line has been found in CONFIG.SYS */
			    if (nfound == 0)
				{
				/* display screen if it's first bad line */
				ScrClear();
				pOld = inserttext[012-1];
				/* get name of Setup disk */
				inserttext[012-1] =
					pheads[N_DISKETTE]->n.fname;
				ScrDisplay(txCompat);
				inserttext[012-1] = pOld;
				}
			    txDummy[0].row = row++;
			    txDummy[0].col = 10;
			    txDummy[0].line = pl0;

			    ScrDisplay(txDummy);
			    nfound++;
			    break;	/* out of for loop */
			    }
			}
		    }
		}
	    }


	/* close the file */
	fclose(in);

	if (nfound)
	    {
	    /* ContinueOrQuit(); */
/* BILLHU
	    textpos(23,1);
	    exit(1);
*/
            iChoice = ScrSelectChar( CompatChoices, 0);
            if ( iChoice == 0 )
                    /* continue but set No Extended Memory Available */
                bCompatHiMem = FALSE;
            else
                /* ExitPgm(); */
                {
                ScrClear();
                ScrDisplay( LeaveSetupMsg);
                exit(1);
                }
	    }
	}
} /* end CheckDriverCompat() */
