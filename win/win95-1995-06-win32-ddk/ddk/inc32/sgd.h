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
* SGD - scatter/gather descriptor
*
\****************************************************************************/

/* equivalent names:                                                        */
/*                                                                          */
/* old rlh.h/strat2.h   old rlx.h               new sgd.h                   */
/* ==================   ==================      ==================          */
/* sg_bufferptr                                 sg_buffer_ptr               */
/* sg_buffersize                                sg_buffer_size              */

/* H2INCSWITCHES -t */

typedef struct _SGD { /* */

        ULONG  SG_buff_ptr;     /* 32 bit physcial pointer to the buffer    */
        ULONG  SG_buff_size;    /* size of the buffer in bytes              */

} SGD, *PSGD;

