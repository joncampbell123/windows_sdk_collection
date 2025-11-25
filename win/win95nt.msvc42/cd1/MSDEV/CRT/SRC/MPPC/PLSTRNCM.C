/***
*strncmp.c - compare first n characters of two strings
*
*       Copyright (c) 1985-1991, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines strncmp() - compare first n characters of two strings
*       for lexical order.
*
*******************************************************************************/

#include <cruntime.h>
#include <string.h>
#include <plstring.h>
#include <stdlib.h>
#include <macos\types.h>

/***
*int strncmp(first, last, count) - compare first count chars of strings
*
*Purpose:
*       Compares two strings for lexical order.  The comparison stops
*       after: (1) a difference between the strings is found, (2) the end
*       of the strings is reached, or (3) count characters have been
*       compared.
*
*Entry:
*       char *first, *last - strings to compare
*       unsigned count - maximum number of characters to compare
*
*Exit:
*       returns <0 if first < last
*       returns  0 if first == last
*       returns >0 if first > last
*
*Exceptions:
*
*******************************************************************************/
short __pascal  PLstrncmp(const unsigned char * str1, const unsigned char * str2, short num)
{
        int ret = 0;
        int len1= StrLength(str1);
        int len2= StrLength(str2);
        int count = min(len1, len2);

        if (num > count)
                {
                ret = memcmp(str1+1, str2+1, count);
                if (ret == 0)
                        {
                        if (len1 > len2)
                                ret = 1;
                        else if (len1 < len2)
                                ret = -1;
                        }
                }
        else
                {
                ret = memcmp(str1+1, str2+1, num);
                }

        return (ret);
}
