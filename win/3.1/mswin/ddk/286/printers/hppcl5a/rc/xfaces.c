/**[f******************************************************************
 * xfaces.c -
 *
 * Copyright (C) 1988,1989 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1988-1990 Microsoft Corporation.  All rights reserved.
 * Copyright (C) 1990, 1991 Hewlett-Packard Company.  All rights reserved.
 * Company confidential.
 *
 **f]*****************************************************************/

#include <stdio.h>
#include <string.h>

#include "windows.h"

#define NUM_FACES 56
#define FSIZE (sizeof(FACESREC) + ((NUM_FACES - 1) * sizeof(ONEFACE)))

#define swab(x) ((((x) << 8) & 0xFF00) | (((x) >> 8) & 0xFF))

main()
    {
    FILE *fp;
    char fbuf[32];
    int fsize;
    int count;
    int ind;

    if (fp = fopen("faces1.bin", "wb"))
	{
	fprintf(stderr, "XFACES: building faces1.bin");

	/*  Write number of faces and default face.
         */
	fprintf(stderr, ".");
	fbuf[0] = (char)NUM_FACES;
	count = fwrite (fbuf, 1, 1, fp);
	count += fwrite ("Courier", 1, strlen("Courier")+1, fp);

	/*  Write alias faces.
         */
	for (ind = 0; ind < NUM_FACES; ++ind)
	    {
	    if (ind < 10)
		{
		fbuf[0] = FF_ROMAN;
		fbuf[1] = 1;

		switch (ind)
		    {
		    case 0:
			strcpy(&fbuf[2], "Tms Rmn");
			fsize = strlen("Tms Rmn") + 3;
			break;

		    case 1:
			strcpy(&fbuf[2], "Times Roman");
			fsize = strlen("Times Roman") + 3;
			break;

		    case 2:
			strcpy(&fbuf[2], "Times");
			fsize = strlen("Times") + 3;
			break;

		    case 3:
			strcpy(&fbuf[2], "TmsRmn");
			fsize = strlen("TmsRmn") + 3;
			break;

		    case 4:
			strcpy(&fbuf[2], "Varitimes");
			fsize = strlen("Varitimes") + 3;
			break;

		    case 5:
			strcpy(&fbuf[2], "Dutch");
			fsize = strlen("Dutch") + 3;
			break;

		    case 6:
			strcpy(&fbuf[2], "CG Times (WN)");
			fsize = strlen("CG Times (WN)") + 3;
			break;

		    case 7:
			strcpy(&fbuf[2], "CG Times (US)");
			fsize = strlen("CG Times (US)") + 3;
			break;

		    case 8:
			strcpy(&fbuf[2], "CG Times (R8)");
			fsize = strlen("CG Times (R8)") + 3;
			break;

		    case 9:
			strcpy(&fbuf[2], "CG Times (E1)");
			fsize = strlen("CG Times (E1)") + 3;
			break;
		    }
		}
	    else if (ind < 15)
		{
		fbuf[0] = FF_ROMAN;
		fbuf[1] = 2;

		switch (ind)
		    {
		    case 10:
			strcpy(&fbuf[2], "Century Schoolbook");
			fsize = strlen("Century Schoolbook") + 3;
			break;

		    case 11:
			strcpy(&fbuf[2], "CG Cent Schl (WN)");
			fsize = strlen("CG Cent Schl (WN)") + 3;
			break;

		    case 12:
			strcpy(&fbuf[2], "CG Cent Schl (US)");
			fsize = strlen("CG Cent Schl (US)") + 3;
			break;

		    case 13:
			strcpy(&fbuf[2], "CG Cent Schl (R8)");
			fsize = strlen("CG Cent Schl (R8)") + 3;
			break;

		    case 14:
			strcpy(&fbuf[2], "CG Cent Schl (E1)");
			fsize = strlen("CG Cent Schl (E1)") + 3;
			break;
		    }
		}
	    else if (ind < 20)
		{
		fbuf[0] = FF_ROMAN;
		fbuf[1] = 3;

		switch (ind)
		    {
		    case 15:
			strcpy(&fbuf[2], "ITC Garamond");
			fsize = strlen("ITC Garamond") + 3;
			break;

		    case 16:
			strcpy(&fbuf[2], "Garmond (WN)");
			fsize = strlen("Garmond (WN)") + 3;
			break;

		    case 17:
			strcpy(&fbuf[2], "Garmond (US)");
			fsize = strlen("Garmond (US)") + 3;
			break;

		    case 18:
			strcpy(&fbuf[2], "Garmond (R8)");
			fsize = strlen("Garmond (R8)") + 3;
			break;

		    case 19:
			strcpy(&fbuf[2], "Garmond (E1)");
			fsize = strlen("Garmond (E1)") + 3;
			break;
		    }
		}
	    else if (ind < 25)
		{
		fbuf[0] = FF_ROMAN;
		fbuf[1] = 4;

		switch (ind)
		    {
		    case 20:
			strcpy(&fbuf[2], "Palatino");
			fsize = strlen("Palatino") + 3;
			break;

		    case 21:
			strcpy(&fbuf[2], "CG Palacio (WN)");
			fsize = strlen("CG Palacio (WN)") + 3;
			break;

		    case 22:
			strcpy(&fbuf[2], "CG Palacio (US)");
			fsize = strlen("CG Palacio (US)") + 3;
			break;

		    case 23:
			strcpy(&fbuf[2], "CG Palacio (R8)");
			fsize = strlen("CG Palacio (R8)") + 3;
			break;

		    case 24:
			strcpy(&fbuf[2], "CG Palacio (E1)");
			fsize = strlen("CG Palacio (E1)") + 3;
			break;
		    }
		}
	    else if (ind < 33)
		{
		fbuf[0] = FF_ROMAN;
		fbuf[1] = 5;

		switch (ind)
		    {
		    case 26:
			strcpy(&fbuf[2], "Bodoni");
			fsize = strlen("Bodoni") + 3;
			break;

		    case 27:
			strcpy(&fbuf[2], "Bauer Bodoni");
			fsize = strlen("Bauer Bodoni") + 3;
			break;

		    case 28:
			strcpy(&fbuf[2], "CG Bodoni (WN)");
			fsize = strlen("CG Bodoni (WN)") + 3;
			break;

		    case 29:
			strcpy(&fbuf[2], "CG Bodoni (US)");
			fsize = strlen("CG Bodoni (US)") + 3;
			break;

		    case 30:
			strcpy(&fbuf[2], "CG Bodoni (R8)");
			fsize = strlen("CG Bodoni (R8)") + 3;
			break;

		    case 31:
			strcpy(&fbuf[2], "CG Bodoni (E1)");
			fsize = strlen("CG Bodoni (E1)") + 3;
			break;

		    case 32:
			strcpy(&fbuf[2], "Bodoni Condensed");
			fsize = strlen("Bodoni Condensed") + 3;
			break;
		    }
		}
	    else if (ind < 39)
		{
		fbuf[0] = FF_ROMAN;
		fbuf[1] = 6;

		switch (ind)
		    {
		    case 33:
			strcpy(&fbuf[2], "Goudy Old Style");
			fsize = strlen("Goudy Old Style") + 3;
			break;

		    case 34:
			strcpy(&fbuf[2], "Goudy");
			fsize = strlen("Goudy") + 3;
			break;

		    case 35:
			strcpy(&fbuf[2], "CG Goudy (WN)");
			fsize = strlen("CG Goudy (WN)") + 3;
			break;

		    case 36:
			strcpy(&fbuf[2], "CG Goudy (US)");
			fsize = strlen("CG Goudy (US)") + 3;
			break;

		    case 37:
			strcpy(&fbuf[2], "CG Goudy (R8)");
			fsize = strlen("CG Goudy (R8)") + 3;
			break;

		    case 38:
			strcpy(&fbuf[2], "CG Goudy (E1)");
			fsize = strlen("CG Goudy (E1)") + 3;
			break;
		    }
		}
	    else if (ind < 45)
		{
		fbuf[0] = FF_ROMAN;
		fbuf[1] = 7;

		switch (ind)
		    {
		    case 39:
			strcpy(&fbuf[2], "Melior");
			fsize = strlen("Melior") + 3;
			break;

		    case 40:
			strcpy(&fbuf[2], "CG Melliza (WN)");
			fsize = strlen("CG Melliza (WN)") + 3;
			break;

		    case 42:
			strcpy(&fbuf[2], "CG Melliza (US)");
			fsize = strlen("CG Melliza (US)") + 3;
			break;

		    case 43:
			strcpy(&fbuf[2], "CG Melliza (R8)");
			fsize = strlen("CG Melliza (R8)") + 3;
			break;

		    case 44:
			strcpy(&fbuf[2], "CG Melliza (E1)");
			fsize = strlen("CG Melliza (E1)") + 3;
			break;
		    }
		}
	    else if (ind < 52)
		{
		fbuf[0] = FF_SWISS;
		fbuf[1] = 8;

		switch (ind)
		    {
		    case 45:
			strcpy(&fbuf[2], "Helv");
			fsize = strlen("Helv") + 3;
			break;

		    case 46:
			strcpy(&fbuf[2], "Helvetica");
			fsize = strlen("Helvetica") + 3;
			break;

		    case 47:
			strcpy(&fbuf[2], "Swiss");
			fsize = strlen("Swiss") + 3;
			break;

		    case 48:
			strcpy(&fbuf[2], "CG Triumv (WN)");
			fsize = strlen("CG Triumv (WN)") + 3;
			break;

		    case 49:
			strcpy(&fbuf[2], "CG Triumv (US)");
			fsize = strlen("CG Triumv (US)") + 3;
			break;

		    case 50:
			strcpy(&fbuf[2], "CG Triumv (R8)");
			fsize = strlen("CG Triumv (R8)") + 3;
			break;

		    case 51:
			strcpy(&fbuf[2], "CG Triumv (E1)");
			fsize = strlen("CG Triumv (E1)") + 3;
			break;
		    }
		}
	    else if (ind < 57)
		{
		fbuf[0] = FF_SWISS;
		fbuf[1] = 9;

		switch (ind)
		    {
		    case 52:
			strcpy(&fbuf[2], "Optima");
			fsize = strlen("Optima") + 3;
			break;

		    case 53:
			strcpy(&fbuf[2], "CG Omega (WN)");
			fsize = strlen("CG Omega (WN)") + 3;
			break;

		    case 54:
			strcpy(&fbuf[2], "CG Omega (US)");
			fsize = strlen("CG Omega (US)") + 3;
			break;

		    case 55:
			strcpy(&fbuf[2], "CG Omega (R8)");
			fsize = strlen("CG Omega (R8)") + 3;
			break;

		    case 56:
			strcpy(&fbuf[2], "CG Omega (E1)");
			fsize = strlen("CG Omega (E1)") + 3;
			break;
		    }
		}
	    else
		fprintf(stderr, "*FATAL* FOR loop too long\n");

	    fprintf(stderr, ".");
	    count += fwrite(fbuf, 1, fsize, fp);
	    }

	fprintf(stderr, "%d bytes.\n", count);
	fclose(fp);
	}

    return (0);
    }
