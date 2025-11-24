/* (c) Copyright 1987 MICROGRAFX, Inc., All Rights Reserved.  Permission to
   use this work for any purpose must be obtained in writing from MICROGRAFX,
   1820 N. Greenville Ave., Richardson, Tx.  75081.

*******************************************************************************
*******************************************************************************

                                   init_lib

*******************************************************************************
*******************************************************************************

This unit defines the function "init_lib", which initializes the various static
data items for the application.  It is called by the "__astart" function,
defined in 'init.asm'. */

#define PRINTDRIVER
#define NOPQ
#define NOINSTALLFILES
#include <print.h>
#include <gdidefs.inc>

#include "initlib.h"

/* ******************************** History ********************************* */
/* 10/31/87 (RWM) - signoff                                                   */
/* ***************************** Exported Data ****************************** */

HANDLE hModule;

/* *************************** Exported Routines **************************** */

BOOL FAR PASCAL init_lib (hInstance,HeapSize)
  /* This function initializes the run-time library.  It returns a boolean
     indicating whether the initialization was successful. */
HANDLE hInstance;
unsigned HeapSize;
{
    hModule = hInstance;
    LocalInit (NULL,NULL,HeapSize);
    return (TRUE);
}
