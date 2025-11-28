/*
**
*
* MEMREPL.C - Sample API routine.
*
* Copyright (c) 1989-1993 Microsoft Corporation as an unpublished
* licensed proprietary work.  All rights reserved.
*
* Description:
*        This is an example of an API routine that reads in the current
*       work area's memo field and then replaces the Memo Field of the specified
*        record number.
*
*        This routine takes two parameters (the second is optional).
*
*            <expN1>   ----> The field number of the memo.
*            [<expN2>] ----> The record number the replace should be made into.
*
*
*
**
 */

#include  <pro_ext.h>

#define WORKAREA        1
#define BADHANDLE       0

MHANDLE dbhand = 0;


void FAR memrepl(ParamBlk  FAR *param)
{
	Locator locate;
	Value val;
        int memchan,skip;
        long memseek, memread, memfind;



    locate.l_type = 'R';
    locate.l_where = 1;
    locate.l_NTI = 1;

//      Store the offset of the memo field.
    locate.l_offset = param->p[0].val.ev_long - 1;
    memchan = _MemoChan(WORKAREA);              // Get the FCHAN to the memo file

    if((memfind = _FindMemo(&locate)) < 0)      // Find the offset of the memo
		_Error((int) memfind);

    memread = _MemoSize(&locate);               // Find the size of the memo field

    memseek = _FSeek(memchan, memfind, 0);      // Move the file pointer

//      Read in the memo field into our handle.
	if ((dbhand = _AllocHand((unsigned) memread)) == BADHANDLE) // Read from the memo file
        _Error(182);                            // Insufficient Memory.

	memread = _FRead(memchan, _HandToPtr(dbhand), (int) memread);


    val.ev_type = 'C';
    val.ev_handle = dbhand;
    val.ev_length = memread;

//      Move to the correct record in the database.
    if (param->pCount == 2)
       _DBRead(WORKAREA, param->p[1].val.ev_long);
    else
       _DBSkip(WORKAREA, 1);


    skip = _DBReplace(&locate,&val);        // Replace the memo field.

    _FreeHand(dbhand);                      // Free the handle previously allocated.

}

FoxInfo myFoxInfo[] ={
		{"MEMREPL", (FPFI) memrepl, 2, "I,.I"},
};


FoxTable _FoxTable = {
    (FoxTable FAR *)0, sizeof(myFoxInfo) / sizeof(FoxInfo), myFoxInfo
};
