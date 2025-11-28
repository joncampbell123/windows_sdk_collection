/*
**
*
* LOGIREPL.C - Sample API routine.
*
* Copyright (c) 1989-1993 Microsoft Corporation as an unpublished
* licensed proprietary work.  All rights reserved.
*
* Description:
*		This routine appends a record and replaces the logical field
*		(the fifth field in the structure).
*
**
 */


#include  <pro_ext.h>
#define WORKAREA        1

MHANDLE dbhand = 0;

long FAR lreplace()
{
	Locator locate;
	Value val;
 	int mwrite, mreplace, mappend;
	int flag = 0;

	locate.l_type = 'R';
	locate.l_where = 1;
	locate.l_NTI = 1;
	locate.l_offset = 4;		// this represents the fifth field in the structure

	val.ev_type = 'L';
	val.ev_length = 1;

        mappend = _DBAppend(WORKAREA,flag);

	mreplace = _DBReplace(&locate,&val);
        mwrite = _DBWrite(WORKAREA);    // Flush the changes to disk.

	return mappend;
}

FoxInfo myFoxInfo[] ={
		{"LREPLACE", (FPFI) lreplace, 0, ""},
};


FoxTable _FoxTable = {
    (FoxTable FAR *)0, sizeof(myFoxInfo) / sizeof(FoxInfo), myFoxInfo
};
