

/************************** ESCAPES ****************************************/

/*
 * be sure to look at GDIESC.INC when choosing new escapes.
 * some of the Aldus escapes are messed with by GDI, in some cases wrongly.
 *
 */

#define SETCOPYCOUNT		 	17
#define SELECTPAPERSOURCE	 	18
#define PASSTHROUGH		 	19
#define GETTECHNOLOGY            	20
#define SETLINECAP               	21
#define SETLINEJOIN              	22
#define SETMITERLIMIT            	23

#define ENABLEDUPLEX			28
#define GETSETPAPERBINS			29
#define GETSETPRINTERORIENTATION	30
#define ENUMPAPERBINS			31
#define SETDIBSCALING			32
#define EPSPRINTING        		33 	/* ADOBE_11_1_88 */
#define ENUMPAPERMETRICS   		34 	/* ADOBE_11_1_88 */
#define GETSETPAPERMETRICS 		35 	/* ADOBE_11_1_88 */
#define GETVERSION         		36 	/* ADOBE_11_1_88 */

#define POSTSCRIPT_DATA			37	/* for PP */
#define POSTSCRIPT_IGNORE		38	/* for PP */

#define GETEXTENDEDTEXTMETRICS		256
#define GETEXTENTTABLE			257
#define GETPAIRKERNTABLE		258
#define GETTRACKKERNTABLE		259

#define EXTTEXTOUT			512
#define GETFACENAME			513	/* ADOBE_11_1_88 */
#define DOWNLOADFACE            	514	/* ADOBE_11_1_88 */

#define ENABLERELATIVEWIDTHS		768

#define ENABLEPAIRKERNING		769
#define SETKERNTRACK			770
#define SETALLJUSTVALUES		771
#define SETCHARSET			772

/* micrographics escapes */

#define BEGIN_PATH			4096
#define CLIP_TO_PATH			4097
#define END_PATH			4098
#define EXT_DEVICE_CAPS			4099
#define RESTORE_CTM			4100
#define SAVE_CTM	              	4101
#define SET_ARC_DIRECTION		4102
#define SET_BACKGROUND_COLOR		4103
#define SET_POLY_MODE			4104
#define SET_SCREEN_ANGLE		4105
#define SET_SPREAD			4106
#define TRANSFORM_CTM			4107
#define SET_CLIP_BOX			4108
#define SET_BOUNDS              	4109



