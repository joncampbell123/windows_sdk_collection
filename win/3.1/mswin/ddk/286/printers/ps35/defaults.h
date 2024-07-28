/**[f******************************************************************
 * defaults.h - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Company confidential.
 *
 **f]*****************************************************************/

/* value used to calculate the paper metrics 100dpi */
#define DEFAULTRES 100

/* values used to initialize the printer caps structures:
 * Please note the difference between DEFAULTRES and DEFAULTRESOLUTION...
 * the former is use to calculate page dimensions and the latter is used
 * as the value of the resolution to use if none is specified in WIN.INI
 */
#define DEFAULTRESOLUTION	300
#define DEFAULTJOBTIMEOUT	0
#define DEFAULTSOURCE		0
