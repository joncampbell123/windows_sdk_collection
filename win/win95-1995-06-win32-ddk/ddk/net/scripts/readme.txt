*****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
*****************************************************************************

This directory contains all the test scripts needed
to run TestProt, the NDIS 3.0 Test Protocol.  This
directory, in its entirety, will need to be moved
to the \tps directory when you install the Test Protocol.

This directory is at this location because of limitations
to the CDROM directory structure (must be nested less than
8 levels deep).  The final TPS directory structure when 
installed on your test machine should look like this:

\tps
    \scripts
            \1
              \1
                \*.*
              \2
                \*.*
              \3
                \*.*
            \2
              \1
                \*.*

\tps is currently located at \ddk\net\ndis3\mac\testprot\tps.
