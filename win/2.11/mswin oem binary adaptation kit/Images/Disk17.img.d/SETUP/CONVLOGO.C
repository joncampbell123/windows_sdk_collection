/* 
	CONVLOGO.C:  Convert .bmp to logo data

	History:
		06 Mar 87 [seans]	Wrote it.
		10 Mar 87 [seans]       Converted to mslogo.asm format.
		16 Jun 87 [martinp]     Add Copyright information to asm file

*/

#include <stdio.h>
#include <fcntl.h>

#define BMSTRUCTSZ 16

#define BUFSZ 4096
char buf[BUFSZ];
char *psz;

main(argc, argv)
int argc;
char **argv;
{
	int fh, iMax, i, j;

	/* Open and seek past bitmap struct. */
	if ((fh = open(argv[1], O_RDONLY | O_BINARY)) == (-1)) {
		fprintf(stderr, "open failed\n");
		fprintf(stderr, "usage:  convlogo logo.bmp\n"); 
		exit (-1);
	}
	if (lseek(fh, (long)BMSTRUCTSZ, 0) == -1L) {
		fprintf(stderr, "seek failed\n");
		exit (-1);
	}

	/* read data into buffer */
	if ((iMax = read(fh, buf, BUFSZ)) == -1) {
		fprintf(stderr, "read failed\n");
		exit (-1);
	}

	/* convert the buffer */
	for (i = 0; i < iMax; i++)
		buf[i] ^= 0xff;

	/* print it out neatly */
	printf("\tTITLE   MSLOGO - Binary data for Microsoft logo\n\n");
	printf("; This file is used to create the default SETUP.LGO file\n\n");
	printf("DATA\tSEGMENT BYTE\n\n");
	printf("\tdb\t'Microsoft Windows/286',0\n");
	printf("\tdb\t'Version 2.11',0\n");
	printf("\tdb\t0\n");
	printf("\tdb\t0\n");
	printf("\tdb\t0\n");
	printf("\tdb\t0\n");
	printf("\tdb\t'Copyright (c) Microsoft Corporation, 1989.'\n");
	printf("\tdb\t'  All Rights Reserved.',0\n");
	printf("\tdb\t'Microsoft is a registered trademark of Microsoft Corp.',0\n");
	printf("\tdb\t1\t\t\t;Merging logo\n");
	printf("\tdb\t36\t\t\t;Height of logo\n");
	printf("\tdb\t67\t\t\t;Width  of logo");

	/* even scan lines */
	psz = buf;
	while (psz < &buf[iMax]) {
		dolines();
		psz += 34; /* skip the odd scan lines for now*/
	}

	/* odd scan lines */
	psz = buf + 34;
	while (psz < &buf[iMax]) {
		dolines();
		psz += 34; /* skip the even scan lines this time*/
	}
	
	/* and the epilog */
	printf("\n\n\nDATA\tENDS\n\n");
	printf("END\n");

   exit(0);
}

dolines() {
	int i, j, iwide;
	char c;
	/* six full lines */
	for (i = 0; i < 6; i++) {
		iwide = widen(*psz++);
		printf("\n\tdb\t%.3XH", ((iwide >> 8) & 0xff));
		printf(",%.3XH", (iwide & 0xff));
		for (j = 0; j < 4; j++) {
			iwide = widen(*psz++);
			printf(",%.3XH", ((iwide >> 8) & 0xff));
			printf(",%.3XH", (iwide & 0xff));
		}
	}
	/* then a partial line */
	iwide = widen(*psz++);
	printf("\n\tdb\t%.3XH", ((iwide >> 8) & 0xff));
	printf(",%.3XH", (iwide & 0xff));
	for (j = 0; j < 2; j++) {
		iwide = widen(*psz++);
		printf(",%.3XH", ((iwide >> 8) & 0xff));
		printf(",%.3XH", (iwide & 0xff));
	}
	iwide = widen(*psz++);
	printf(",%.3XH", (iwide & 0xff));
}

widen(c)
char c;
{
	int i, j, result;
	char d;
	
	result = 0;
	j = 1; /* j = i ** 2 */
	/* loop through the bits of c */
	for (i = 1; i < 9; i++) {
		d = c & 0x01;
		c >>= 1;
		result += d * j * j;
		j *= 2;
	}
	result += result * 2; /* fill in the empties. */
	return (result);
}
