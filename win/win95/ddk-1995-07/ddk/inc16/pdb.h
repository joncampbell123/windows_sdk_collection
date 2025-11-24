/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

/***	Process data block (otherwise known as program header)
 *
 *	These offset are documented in the MSDOS Encyclopedia, so nothing
 *	can be rearranged here, ever.  Reserved areas are probably safe
 *	for use.
 */


#define	FilPerProc	20

struct Process_data_block {
    USHORT pdb_Exit_Call;		/* INT int_abort system terminate */
    USHORT pdb_block_len;		/* size of execution block */
    UCHAR pdb_res0;
    UCHAR pdb_CPM_Call[5];		/* ancient call to system */
    ULONG pdb_Exit;			/* pointer to exit routine */
    ULONG pdb_Ctrl_C;			/* pointer to ^C routine */
    ULONG pdb_Fatal_abort;		/* pointer to fatal error */
    USHORT pdb_Parent_PID;		/* PID of parent (terminate PID) */
    UCHAR pdb_JFN_Table[FilPerProc];
    USHORT pdb_environ;			/* seg addr of environment */
    ULONG pdb_User_stack;		/* stack of self during system calls */
    USHORT pdb_JFN_Length;		/* number of handles allowed */
    ULONG pdb_JFN_Pointer;		/* pointer to JFN table */
    ULONG pdb_Next_PDB;			/* pointer to nested PDB's */
    UCHAR pdb_InterCon;			/* *** jh-3/28/90 ***  */
    UCHAR pdb_Append;			/* *** Not sure if still used *** */
    UCHAR pdb_Novell_Used[2];		/* Novell shell (redir) uses these */
    USHORT pdb_Version;			/* DOS version reported to this app */
    UCHAR pdb_pad1[14];
    UCHAR pdb_Call_system[5];		/* portable method of system call */
    UCHAR pdb_pad2[7];			/* reserved so FCB 1 can be used as
					 * an extended FCB
					 */
    UCHAR pdb_FCB1[16];			/* default FCB 1 */
    UCHAR pdb_FCB2[16];			/* default FCB 2 */
    UCHAR pdb_pad3[4];			/* not sure if used by PDB_FCB2 */
    UCHAR pdb_tail[128];		/* command tail and default DTA */
};
typedef struct Process_data_block PDB;
