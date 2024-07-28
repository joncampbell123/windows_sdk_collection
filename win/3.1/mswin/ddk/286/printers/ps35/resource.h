/*
 * resource.h
 *
 * this contains resource type identifiers and some constants
 * used to name resources.
 *
 */

/*
 * resource types defined by this driver
 *
 */

#define MYFONTDIR	257	/* font direcotyr .DIR */
#define MYFONT		258	/* PFM file */
#define MY_DATA		261	/* random data */
#define PS_DATA		259	/* PS code */
#define PR_CAPS		262	/* PRINTER structs */
#define PR_PSS		263	/* PSS file */


#define DMBIN_BASE	5000	/* resource base numbers for DM strings */
#define DMPAPER_BASE	6000


/*
 * IDs for MY_DATA type
 */

#define PAPERSIZES	1


/* PostScript resource ID's (type PS_DATA) */

#define PS_HEADER	1
#define PS_DL_PREFIX	2
#define PS_DL_SUFFIX	3
#define PS_1		4
#define PS_SETCOMM	5
#define PS_EHANDLER	7
#define PS_2            8
#define PS_OLIVCHSET	9
#define PS_UNPACK	10
#define PS_FONTS	11
#define PS_CIMAGE	12
#define TI_HEADER	13

#define PS_T3HEADER     20
#define PS_T1HEADER1    21
#define PS_T1EHEADER1   23
#define PS_T1EHEADER2   24
#define PS_T1EFOOTER1   25
#define PS_T1EFOOTER2   26

/* string IDs */

#define IDS_DEVICE		100	/* for "device=" entry in WIN.INI */
#define IDS_PAPERX		101	/* for "paperX=" entries */
#define IDS_ORIENTATION		102	/* for "orientation=" entry */
#define IDS_RESOLUTION		103	/* for "resolution=" entry */
#define IDS_PAPERSOURCE		104	/* for "papersource=" entry */
#define IDS_JOBTIMEOUT		105	/* for "jobtimeout=" entry */
#define IDS_HEADER		106	/* for "header=" entry */
#define IDS_MARGINS		107	/* for "Margins=" entry */
#define IDS_YES			109	/* "yes" */
#define IDS_NO			110	/* "no" */
#define IDS_USER		111
#define IDS_DEFAULT_USER	112
#define IDS_APPLETALK		113
#define IDS_NULL		114
#define IDS_PREPARE		115
#define IDS_STRUCTURED_COMMENTS	116
#define IDS_WINDOWS		117
#define IDS_ATMODULEFILE	118
#define IDS_DEFAULT_ATFILE	119
#define IDS_DEFAULT_ATMODNAME	120
#define IDS_EPT		  	121
#define IDS_BINARYIMAGE  	122
#define IDS_COLOR		123
#define IDS_EPSHEAD		124
#define IDS_EPSBBOX		125
#define IDS_PSHEAD		126
#define IDS_PSJOB		127
#define IDS_PSTIMEOUT		128
#define IDS_PSTITLE		129
#define IDS_EXTPRINTER		130
#define IDS_PRINTER		131
#define IDS_FILE		132
#define IDS_EXTPRINTERS		133
// #define IDS_ALREADYINSTALLED    134
// #define IDS_ADDPRINTER	   135
// #define IDS_INSTSUCCESS	   136
// #define IDS_INSTFAIL 	   137
#define IDS_SETSCREENANGLE	138
#define IDS_OLIV		139
#define IDS_LZLIB		140
#define IDS_LZCOPY		141
#define IDS_ON                  142
#define IDS_SETSCREEN	        143
#define IDS_NEGIMAGE            144
#define IDS_RES                 145
#define IDS_SCREENFREQUENCY     146
#define IDS_SCREENANGLE         147
#define IDS_ADVFLAGS            148
#define IDS_PRINTERVM           149
#define IDS_DOWNLOAD            150
#define IDS_SUBCOUNT            151
#define IDS_SUBENTRY            152
#define IDS_ELINAME             153
#define IDS_ELICMD              154
#define IDS_SETRESOLUTION       155
#define IDS_PORTS               156
// #define IDS_HANDMISMATCHMSG	   157
// #define IDS_HANDMISMATCHCAP	   158
#define IDS_DUPLEX		159
#define IDS_DLFMTTYPE1		160
#define IDS_DLFMTTYPE3		161
#define IDS_DLFMTTRUETYPE	162
#define IDS_HELPFILE       163
#define IDS_SETCOMMJOBNAME 164
#define IDS_CTRLD          165
//  #define IDS_SUBFONTMISSING 166
#define IDS_SUBVERIFYDEFAULT  167
#define IDS_WARNING        168
#define IDS_NODOWNLOAD        169
#define IDS_CUSTOMWIDTH    170
#define IDS_CUSTOMHEIGHT   171
#define IDS_CUSTOMUNIT     172

#define IDS_MSG_USERSIZE	173
#define IDS_MSG_WIDTHTOOBIG   174
#define IDS_MSG_LENGTHTOOBIG  175

#define IDS_MINOUTLINEPPEM 176
#define IDS_FAVORTT  177
#define IDS_LANDSCAPEORIENT  178

/* the IDs from IDS_DEFSUBBASE to IDS_DEFSUBBASE + IDS_NUMDEFSUBS are
 * reserved for the default substitution table.
 */
#define IDS_NUMDEFSUBS          11
#define IDS_DEFSUBBASE          300

// add 4 new paper sizes (10/09/91 ZhanW)
#define DMPAPER_A_PLUS			57  // SuperA/SuperA/A4 227 x 356 mm
#define DMPAPER_B_PLUS			58  // SuperB/SuperB/A3 305 x 487 mm
#define DMPAPER_LETTER_PLUS		59  // Letter Plus 8.5 x 12.69 in
#define DMPAPER_A4_PLUS 		60  // A4 Plus 210 x 330 mm
