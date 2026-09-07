/***
*setmaxf.c - Set the maximum number of streams
*
*       Copyright (c) 1995, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Defines _setmaxstdio(), a function which changes the maximum number
*       of streams (stdio-level files) which can be open simultaneously.
*
*******************************************************************************/

#include <cruntime.h>
#include <stdio.h>
#include <malloc.h>
#include <internal.h>
#include <mtdll.h>
#include <dbgint.h>

/***
*int _setmaxstdio(maxnum) - sets the maximum number of streams to maxnum
*
*Purpose:
*       Sets the maximum number of streams which may be simultaneously open
*       to maxnum. This is done by resizing the __piob[] array and updating
*       _nstream. Note that maxnum may be either larger or smaller than the
*       current _nstream value.
*
*Entry:
*       maxnum = new maximum number of streams
*
*Exit:
*
*Exceptions:
*
*******************************************************************************/

int __cdecl _setmaxstdio (
        int maxnum
        )
{
        void *newpiob;
        int retval;

        _mlock(_IOB_SCAN_LOCK);

        /*
         * Try to reallocate the __piob array. Note the vacuous case where
         * maxnum == current __piob[] size can be, and is, folded in here.
         */
        if ( (maxnum == _nstream) ||
             ((newpiob = _realloc_crt( __piob, maxnum * sizeof(void *) )) !=
             NULL) )
        {
            retval = _nstream;
            _nstream = maxnum;
        }
        else
            retval = -1;

        _munlock(_IOB_SCAN_LOCK);

        return retval;
}
