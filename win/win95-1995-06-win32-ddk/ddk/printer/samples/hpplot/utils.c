//*************************************************************
//  File name: utils.c
//
//  Description: 
//
//
//  History:    Date       Author     Comment
//              9/ 7/93    DonMil       
//*************************************************************

#define PRINTDRIVER
#define NOPQ
#define NOINSTALLFILES
#include <print.h>
#include <gdidefs.inc>
#include "utils.h"

//--------------------------------------------------------------------------//
// Function: ltoa
//
// Action:  This function converts the given long integer into an ASCII string.
//
// return:  The length of the string.
//--------------------------------------------------------------------------//

long FAR PASCAL ltoa(buf, n)
LPSTR buf;
long n;
{
    BOOL fNeg;
    long i, j, k;

    if (fNeg = (n < 0))
        n = -n;

    for (i = 0; n; i++)
    {
        buf[i] = (char)(n % 10) + '0';
        n /= 10;
    }

    // n was zero
    if (i == 0)
        buf[i++] = '0';

    if (fNeg)
        buf[i++] = '-';

    for (j = 0, k = i-1 ; j < i / 2 ; j++,k--)
    {
        char tmp;

        tmp = buf[j];
        buf[j] = buf[k];
        buf[k] = tmp;
    }

    buf[i] = 0;

    return i;
}

/*** EOF: utils.c ***/
