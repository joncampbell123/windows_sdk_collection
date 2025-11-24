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

/****************************************************************************\
*
* MED (Memory Element Descriptor) Data Structure
*
\****************************************************************************/

typedef struct MED  { /* */

        ULONG   MED_Physical;           /* Physical Pointer to MED Struct   */
        ULONG   MED_Next;               /* Next Logical MED                 */
        ULONG   MED_Adjacent;           /* Next Physical MED                */
        USHORT  MED_Type;               /* Type of data structure           */
        USHORT  MED_Size;               /* Size of piece (including med)    */
        USHORT  MED_Flags;              /* Flags                            */
		  USHORT	 MED_type_spec;		    /* type specific parameter		    */
} MED, *PMED;


/*
** Data Structure defines (MED_Flags)
*/

#define MED_FREE_POOL   0x0001          /* piece is free and in the pool    */
#define MED_FREE_FAST   0x0002          /* piece is free for quick allocate */
#define MED_ALLOC       0x0004          /* this piece is allocated          */
#define MED_PANIC			0x0008			 /* piece is panic space				 */
#define MED_EXTERNAL		0x0010			 /* piece is externally allocated	 */
#define MED_SMART			0x0020			 /* piece is "smart" alloced			 */
																	  
/*
** Data Structure defines (MED Type)
*/

#define MED_FREE_TYPE    0              /* piece is not being used - do not */
                                        /* use this as a test for the piece */
                                        /* being free. use the flags        */

#define MED_IOP          1              /* IOP DataType                     */
#define MED_ILB          2              /* ILB DataType                     */
#define MED_AEP          3              /* AEP DataType                     */
#define MED_DCB          4              /* DCB DataType                     */
#define MED_DDB          5              /* ddb DataType                     */
#define MED_DRP          6              /* DRP DataType                     */
#define MED_DVT          7              /* DVT datatype                     */
#define MED_IRB          8              /* IRB datatype                     */
#define MED_MISC         9              /* Misc datatype                    */
#define MED_ILR         10              /* ios log record                   */
#define MED_IOC         11              /* ios configuration file           */
#define MED_NEW_IOP     12              /* type used to alloc iop's due to timer issues */
#define MED_VRP         13              /* VRP datatype                     */
#define MED_PCD         14              /* PCD datatype                     */

#define MED_MAX_TYPE    14              /* MAX med type                     */

/*
** constants for use only by the memory allocation/deallocation routines
*/

#define MED_GRAN_DELTA  16              /* delta from one granule size to   */
                                        /* the next. this number must be a  */
                                        /* power of two.                    */

#define MED_FAST_GRANS  64              /* number of granules in the fast   */
                                        /* allocation table.                */

#define MED_SHIFT_CNT   4-2             /* shift count equivalent to        */
                                        /* dividing by MED_GRAN_DELTA and   */
                                        /* multiplying by the number of     */
                                        /* bytes in a dword (ie, four).     */

