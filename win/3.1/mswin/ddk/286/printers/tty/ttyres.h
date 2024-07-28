/*
 +--------------------------------------------------------------------------+
 |                                                                          |
 |     TTYRES.h                                                             |
 |                                                                          |
 |     TTY.DRV                                                              |
 |     FRALC Consulores, S.C.                                               |
 |     (c) Mexico 1988, 1989                                                |
 |                                                                          |
 |     Copyright (c) 1989-1990, Microsoft Corporation.			    |
 |                                                                          |
 |     Define dialog control ID's                                           |
 |                                                                          |
 +--------------------------------------------------------------------------+
*/

//	Microsoft history
//	08 dec 89	peterbe		Add S_DATE, S_COPYRIGHT.
//	20 oct 89	peterbe		checked in

#define MYFONT	    257

#if 0
#define FNFID	    	102
#define DFEID	    	103
#define CAPTID	    	104
#endif

#define MAIN_DLG	100
#define ADD_DLG		101
#define MOD_DLG		102
#define CHAR_DLG	103
#define ABOUT_DLG	104

/* Controles del MainDialog
 */

#define MD_ADD		3
#define MD_MOD		4
#define MD_CHAR		5
#define MD_HELP		6
#define MD_ABOUT	7
#define MD_PBOX		8
#define MD_LETTER	9
#define MD_LEGAL	10
#define MD_A4		11
#define MD_B5		12
#define MD_CONT		13
#define MD_CUT		14
#define MD_2ANCHO	15
#define MD_NOPAGEBREAK	16

/* Controles de los dialogs Add y Modify
 */

#define AD_CANCEL	2
#define AD_NAME		3
#define AD_RESET	6
#define AD_10CPI	8
#define AD_12CPI	10
#define AD_16CPI	12
#define AD_BDW		14
#define AD_EDW		16
#define AD_BORRA	17

/* Controles del dialogo de caracteres
 */

#define CD_CARANSI	4
#define CD_CODIMPRESORA 6

/* Strings
 */
#define IDS_MSG_CAPTION     200
#define IDS_MSG_NOSETTINGS  202
#define IDS_MSG_FEED	    203
#define IDS_MSG_DEL	    204

#define STR_NEWID	209

#define INI_PAPER	210
#define INI_WIDE	211
#define INI_CUT		212
#define INI_BREAK	213

#define PAP_LETTER	220
#define PAP_LEGAL	221
#define PAP_A4		222
#define PAP_B5		223
#define PAP_FIRST	PAP_LETTER
#define PAP_LAST	PAP_B5
#define PAP_DEF		PAP_LETTER

#define STR_NO		230  /* no invertir p.f. */
#define STR_YES		231
// #define S_DATE	   232	   // just contains date of update.

// #define S_COPYRIGHT	   233	   // contains copyright string.

#define DM_WIDE 	0x8000
#define DM_PAGEBREAK	0x4000
