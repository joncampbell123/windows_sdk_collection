//*************************************************************
//  File name: data.c
//
//  Description: DATA.C contains static info that will eventually
//               become resource data.
//
//*************************************************************

#define PRINTDRIVER
#define NOPQ
#define NOINSTALLFILES
#include <print.h>
#include <gdidefs.inc>

#include "device.h"
#include "driver.h"
#include "color.h"
#include "control.h"
#include "dialog.h"
#include "glib.h"
#include "initlib.h"
#include "object.h"
#include "output.h"
#include "print.h"
#include "profile.h"
#include "text.h"
#include "utils.h"

#include "devmode.h"

short ColorSupport [MAX_PEN_TYPES] [MAX_COLORS] =
  /* The following boolean table indicates the color supported by each of the
     pen types. */
{
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },   /* P3 */
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },   /* P7 */
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },   /* T3 */
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },   /* T6 */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },   /* V25 */
    { 1, 1, 1, 1, 0, 0, 1, 1, 1, 1 },   /* V35 */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },   /* V50 */
    { 1, 1, 1, 1, 0, 0, 1, 1, 1, 1 },   /* V70 */
    { 0, 0, 1, 1, 0, 0, 0, 0, 1, 1 },   /* R3 */
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },   /* 0.18 */
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },   /* 0.25 */
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },   /* 0.35 */
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },   /* 0.50 */
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },   /* 0.70 */
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }    /* 1.00 */
};

short ForceValue [NUM_PLOTTERS] [8] =
  /* Each plotter that is capable of specifing the force does so by expressing
     it in a value from 1 to 8.  These actual force applied by the plotter
     varies and is listed in the table below. */
{
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 15, 24, 30, 36, 45, 51, 57, 66 },
    { 10, 18, 26, 34, 42, 50, 58, 66 },
    { 10, 18, 26, 34, 42, 50, 58, 66 },
    { 10, 18, 26, 34, 42, 50, 58, 66 },
    { 10, 18, 26, 34, 42, 50, 58, 66 },
    { 10, 18, 26, 34, 42, 50, 58, 66 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 15, 24, 30, 36, 45, 51, 57, 66 },
    { 15, 24, 30, 36, 45, 51, 57, 66 }
};

// supported paper sizes in 10th's of a mm
POINT PaperSize [MAX_MEDIA_SIZES] =
{
    { 2159,2794 },
    { 2794,4318 },
    { 4318,5588 },
    { 5588,8636 },
    { 8636,11176 },
    { 2100,2970 },
    { 2970,4200 },
    { 4200,5940 },
    { 5940,8410 },
    { 8410,11890 },
    { 6096,9144 },
    { 9144,12192 }
};


/*** EOF: data.c ***/
