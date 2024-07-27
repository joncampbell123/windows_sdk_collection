/* SUINTL.C -- handle [country] in SETUP.INF, [intl] in WIN.INI
**
** Errors in this module exit with return code 16
*/

/*  History
**
** 17 jun 87	plb	Started.
** 19 jun 87    plb     size of KeyBuffer increased.
**  2 aug 87    mp      Default Country is DOS information
** 27 aug 87    mp      Don't write US country settings, because it is default
*/

/* Microsoft C include files */

#include <dos.h>		/* for int86() */
#include <string.h>

/* SETUP include file(s)
*/
#include "setup.h"

/* externals from SUDATA.C
*/
extern textline CountryMenu[];
extern CHAR smDosCountry[];

/* externals from SUSEL.C
*/
extern BuildMenu(int, union dfile *, BOOL, int);

/* forward declaration
*/
static union dfile * CreateCountry();
static unsigned int GetDosCountry();

struct doscountry {			/* values read from DOS */
	unsigned int	iDate;
	unsigned char	sCurrency[5];
	unsigned char	sThousand[2];
	unsigned char	sDecimal[2];
	unsigned char   sDate[2];
	unsigned char   sTime[2];
	unsigned char   iCurrency;
	unsigned char   iDigits;
	unsigned char	iTime;		/* 0:12-hr, 1:24-hr. */
	unsigned char	casemap[4];
	unsigned char	sList[2];
	unsigned char	reserved[10];	/* 5 words reserved */
					/* additional values */
	unsigned int	iCountry;	/* fill in from return value */
	unsigned int	iLzero;
	unsigned char	s1159[5];	/* AM string */
	unsigned char	s2359[5];	/* PM string */
	};

struct doscountry coIn = {
	/* read these values from DOS */
	0,	"",	"",	"",
	"",	"",	0,	0,
	0,	"",	"",	"",

	0,	/* country -- filled in from value returned in BX */
	/* these values are not supplied by DOS */
	0, "AM", "PM"
	};

/* list of [intl] section keys in order of strings in SETUP.INF & keyboard
** drivers.
*/
static char * keysIntl[] = {
	"iCountry","iDate","iCurrency", "iDigits","iTime", "iLzero",
	"s1159", "s2359","sCurrency","sThousand", "sDecimal", "sDate",
	"sTime", "sList" };

#define NKEYS (sizeof (keysIntl))/(sizeof (char *))



/* *********** Code begins .. **************** */

/* SelectCountry -- Get contry setting, write WIN.INI [intl] settings.
**
** If the WIN.INI 
*/

SelectCountry()
{
    char KeyBuffer[200];
    int c;
    int code;
    union dfile *pd;

    /* determine if [intl] section exists in WIN.INI */
    GetProfileString("intl", KeyBuffer);
    if (0 < strlen(KeyBuffer)) {
	pd = pheads[N_COUNTRY];
	if (code = GetDosCountry()) {
	    for (; pd; pd = pd->coun.p)
		if (code == atoi(pd->coun.cinfo))
		    break;
	    if (!pd)                    /* country is not in SETUP.INF */
		pd = CreateCountry();
	}
	if (pheads[N_COUNTRY] == NULL)
	    return;                     /* no country information available */

	ScrClear();
	BuildMenu(N_COUNTRY, pd, FALSE, 0);
	ScrDisplay(CountryMenu);
	c = ScrSelectList(menu);

	if (c >= 0)
	    WriteIntlString(menuptr[c]->coun.cinfo);
   }
} /* SelectCountry() */



/*  GetDosCountry() -- get country information from DOS
**
**  Output:	returns country code.
**		The first part of the coIn array is filled in
**		from the DOS information returned by function 0x38.
**		The AM/PM strings are set on the basis of the iTime
**		parameter.
*/

static unsigned int GetDosCountry()
{
    union REGS inregs;
    union REGS outregs;

    /* Call MSDOS to get country information */
    inregs.h.ah = 0x38;		/* Get/Set country function */
    inregs.h.al = 0;
    inregs.x.dx = (int)(unsigned) &coIn;
    /* Note: DS is unchanged by intdos() */
    intdos(&inregs, &outregs);
    if (outregs.x.cflag)
	return(0);
    else
	{
	coIn.iCountry = outregs.x.bx;
	if (coIn.iTime != 0)
	    {
	    coIn.s1159[0] = 0;
	    coIn.s2359[0] = 0;
	    }
	else
	    {
	    strcpy(coIn.s1159, "AM");
	    strcpy(coIn.s2359, "PM");
	    }
	return(outregs.x.bx);
	}
}



/* CreateCountry() -- Creates country entry from information from DOS.
** The coIn data structure, filled in by GetDosCountry is converted into
** a string, a dfile entry is created and added in pheads[N_COUNTRY] list.
*/
static union dfile *CreateCountry()
{
    unsigned char sBuff[100];
    union dfile *pd;

    sBuff[0] = 0;

    AppendINumber(sBuff, coIn.iCountry);
    AppendINumber(sBuff, coIn.iDate);
    AppendINumber(sBuff, coIn.iCurrency);
    AppendINumber(sBuff, coIn.iDigits);
    AppendINumber(sBuff, coIn.iTime);
    AppendINumber(sBuff, coIn.iLzero);

    AppendIString(sBuff, coIn.s1159);
    AppendIString(sBuff, coIn.s2359);
    AppendIString(sBuff, coIn.sCurrency);
    AppendIString(sBuff, coIn.sThousand);
    AppendIString(sBuff, coIn.sDecimal);
    AppendIString(sBuff, coIn.sDate);
    AppendIString(sBuff, coIn.sTime);
    AppendIString(sBuff, coIn.sList);

    if ((pd = (union dfile *) malloc(DFCOUNSIZE + strlen(sBuff))) == NULL)
	FatalError(smNoMemory, 16);
    strcpy(pd->coun.cinfo, sBuff);
    pd->coun.cname = smDosCountry;

    pd->coun.p = pheads[N_COUNTRY];      /* insert pointer at top of list */
    pheads[N_COUNTRY] = pd;

    return(pd);
} /* WriteIntlDos() */

static AppendIString(p, s)
unsigned char * p;
unsigned char * s;
{
    strcat(p, s);
    strcat(p, "!");
}

static AppendINumber(p, n)
unsigned char * p;
int n;
{
    unsigned char digits[6];

    itoa(n, digits, 10);
    AppendIString(p, digits);
}

/* WriteIntlString() -- write [intl] information from a string.
** The string must be in the format used in SETUP.INF, also in
** keyboard driver description strings (fields separated by
** '!' characters).  The order of the fields is defined in the
** keysIntl[] array above. If countrycode = 1 then string is not written.
*/
static WriteIntlString(pIntl)
unsigned char * pIntl;
{
    unsigned char * p;			/* pointer to country info */
    unsigned char * pNext;		/* pointer to next country info */
    unsigned char sIntl[100];		/* temp. copy of string at pIntl */
    int i;

    if (atoi(pIntl) == 1)
	return;                         /* don't write US settings */

    /* copy to sIntl since we're writing 0's to string */
    strcpy(sIntl, pIntl);

    p = & sIntl[0];

    for (i = 0; i < NKEYS; i++)
	{
	/* Replace '!' termination of each item with 0 */
	pNext = strchr(p, '!');
	if ( pNext )
	    *pNext = '\0';

	WriteProfileString("intl", keysIntl[i], p);

	if ( pNext )
	    p = pNext + 1;
	else
	    break;
	}

    /* write the following so that CONTROL.EXE will update [intl] info. */
    WriteProfileString("intl", "dialog", "yes");

} /* WriteIntlString */

