/* (c) Copyright 1987 MICROGRAFX, Inc., All Rights Reserved. This material is
   confidential and a trade secret. Permission to use this work for any purpose
   must be obtained in writing from MICROGRAFX, 1820 N. Greenville Ave.,
   Richardson, Tx.  75081.

*******************************************************************************

				       fill

*******************************************************************************
*/

typedef struct
	{
	    HANDLE  hIndex,
		    hMem;
	    LPPOINT lpPoints,
		    lpOldPoints;
	    short   nPoints,
		    nPairs;

	} POLYSET;
typedef POLYSET FAR *LPPOLYSET;


extern POLYSET Polyset [];

extern BOOL NEAR PASCAL fill (LPPDEVICE,LPPOLYSET,short,short,short,BOOL,BOOL);
