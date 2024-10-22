/***************************************************************************

					 WINDOWS MUTLIMEDIA MOVIE Player 2.0
						 			Beta Release
		   				 	Loadable DLL Driver
				       (c) Copyright 1990 MacroMind
                       All Rights Reserved.

*****************************************************************************/
/*
	This file to be included in any applicaton wishing to play back Windows
	MultiMedia Movies.
*/ 

/*
------------------------------------------------------------------------------
Constants:
------------------------------------------------------------------------------
*/

/***************************\
		  ERROR CODES
\***************************/
				 	
#define 	MMPecNoError			0	/* Fucntion succesful, no error dude */
#define	MMPecFatalErr		-101	/* fatal error -- program abort */
#define	MMPecStageErr		-102	/* error creating stage window */
#define	MMPecFileVer		-110	/* wrong file version */
#define	MMPecFileType		-111	/* wrong file type -- not an MMP movie file		*/
#define	MMPecOpenErr		-112	/* DOS error opening file */
#define	MMPecReadErr		-113	/* DOS read error on file */
#define	MMPecWriteErr		-114	/* DOS write error */
#define	MMPecEofErr			-115	/* unexpected end of file */
#define	MMPecRecHeader		-116	/* invalid record header */
#define	MMPecCastErr		-117	/* error reading cast	*/
#define	MMPecCastMapErr	-118	/* cast/bitmap mismatch	*/
#define	MMPecMemAlloc		-121	/* memory allocation error	*/

#define	MMPecBadRec			-402	/* recording or mungback failed */
#define	MMPecNoMatte		-404	/* not enough memory to make a matte */
#define	MMPecOutCast		-411	/* ran out of cast members!! */
#define	MMPundefErr			-412	/* internal error, unknown cause */
#define	MMPecNoRam			-413	/* insufficient memory for offscreen buffer */
#define	MMPecBadCall		-425	/* bad call to the driver 			*/
#define	MMPecBadParm		-426	/* bad parameters	*/

#define MMP_MOVEOK 			1		/* boolean field definition */
#define SCRIPTCHANNEL   	-1		/* definition for a script channel action code */


/*
	Constants used with the Animation
*/

#define MMP_FRAME_FIRST 1
#define MMP_FRAME_LAST  0x7FFF

#define MMP_VERSION     0x00000032L



/*
	Animation status: returned from mmpAnimate and mmpStepFrame
*/

#define	ANIM_STOPPED		0		/* animation is stopped */
#define	ANIM_RUNNING		1		/* animation is running */
#define	MOUSE_PAUSE			-1		/* animation is waiting for mouse click */
#define	DEVICE_WAIT			-2		/* animation is waiting for device to finish */

/*
	Driver Invocation option flags: mmpOpen
*/

#define MMP_OPEN_STAGE_MODE	0x01	/* means do not take over entire screen */

/*
	Global MMP option flags.
*/

#define MMP_OPT_PRELOAD				0x1
#define MMP_OPT_CHECK_BOX_STYLE		0x2
#define MMP_OPT_INVERT_IMMEDIATE	0x4
#define MMP_OPT_CHANGE_PUPPETS		0x8
#define MMP_OPT_RESTORE_PUPPETS		0x10
#define MMP_OPT_USE_REG_PT_FOR_RECT	0x20
#define MMP_OPT_TEMPO_CATCH_UP		0x40

/*
	MMP_OPEN_ options for mmpOpen
*/
#define MMP_OPEN_NOSTATIC	0x0		/* default: steal system colors */
#define MMP_OPEN_STATIC		0x1		/* leave system colors */
	
/*
	MMP_STAGE_ options for MMPSetStage and MMPGetStage.
*/

#define MMP_STAGE_CENTER	0x1		/* stage rect is centered in stage wind */
#define MMP_STAGE_OFFSET	0x2		/* stage rect is offset to a fixed location */
#define MMP_STAGE_BORDER	0x4		/* border is drawn around stage rect */
#define MMP_STAGE_FIXED		0x8		/* stage size doesn't change between movies */

/*
	MMP_PRELOAD_ options.
*/

#define MMP_PRELOAD_REFERENCED	0x1		/* preload if referenced from score */
#define MMP_PRELOAD_ALL_CAST	0x2		/* preload all cast, even if not referenced */


/*
	MMP_FRAME_ options for MMPStepFrame, MMPGoToFrame, MMPDrawFrame, etc.
	High order bits are used since they may be combined with other options.
*/

#define MMP_FRAME_ERASE		0x2000	/* erase entire stage */
#define MMP_FRAME_IMAGE		0x4000	/* produce frame offscreen image */
#define MMP_FRAME_UPDATE	0x8000	/* copy offscreen image to stage */
#define MMP_FRAME_DRAW		(MMP_FRAME_IMAGE | MMP_FRAME_UPDATE)

/*
------------------------------------------------------------------------------
Types:
------------------------------------------------------------------------------
*/

typedef DWORD	MMPID;		/* ID of an MMP animation port */

typedef struct tagMMPOptions
{
	BYTE	fbPreload;			/* MMP_PRELOAD_ constants */
	char	bCheckBoxStyle;		/* MMP_CHECK_BOX_ constant */
	BYTE	bbInvertImmediate;	/* invert on screen immediately */
	BYTE	bbChangePuppets;	/* can only change sprite if puppet flag set */
	BYTE	bbRestorePuppets;	/* restore score values when puppet flag cleared */
	BYTE	bbUseRegPtForRect;	/* changing rect uses registration point */
	BYTE	bbTempoCatchUp;		/* anim tries to catch up with real time */
} MMPOptions, FAR * pMMPOptions;


typedef struct tagMMPFileInfo					// file on disk  info structure
{
	DWORD dwFileVersion;
	DWORD dwFileSize;
	DWORD dwPixelDepth;
	char  achFullName[128];
} MMPFileInfo, FAR *pMMPFileInfo;

typedef struct tagMMPMovieInfo				// movie in memory info structure
{
	DWORD dwFileVersion;
	DWORD dwTotalFrames;
	DWORD dwPixelDepth;
	DWORD dwMovieExtentX;
	DWORD dwMovieExtentY;
} MMPMovieInfo, FAR *pMMPMovieInfo;

/* per-frame-hook callback */
typedef BOOL (FAR PASCAL *pMMPFrameHook)(MMPID id,
	int sFrame, int sSubFrame);

/*
------------------------------------------------------------------------------
Functions:
------------------------------------------------------------------------------
*/

extern DWORD FAR PASCAL
mmpGetVersion();

extern MMPID FAR PASCAL
mmpOpen( HANDLE hAppInst, HWND hWndStage, WORD fsOptions );

extern BYTE FAR PASCAL
mmpClose( MMPID id, WORD fsOptions );

extern BYTE FAR PASCAL
mmpRewind( MMPID id );

extern BYTE FAR PASCAL
mmpStartAnimating( MMPID id, WORD fsOptions );

extern int FAR PASCAL		/* function modified CRK 8-30-90 */
mmpAnimate( MMPID id );

extern BYTE FAR PASCAL
mmpStopAnimating( MMPID id, WORD fsOptions );

extern int FAR PASCAL		/* function modified CRK 8-30-90 */
mmpStepFrame( MMPID id, WORD fsOptions );

extern BYTE FAR PASCAL
mmpDrawFrame( MMPID id, WORD fsOptions );

extern BYTE FAR PASCAL
mmpGoToFrame( MMPID id, short sFrameCtr, WORD fsOptions );

extern BYTE FAR PASCAL
mmpUpdate( MMPID id, HDC hDC, RECT FAR * prectArea );

extern BYTE FAR PASCAL
mmpSetFrameHook( MMPID id, pMMPFrameHook pFrameHook );

extern pMMPFrameHook FAR PASCAL
mmpGetFrameHook( MMPID id );

extern BYTE FAR PASCAL
mmpGetFileInfo( MMPID id, char FAR * pstrFileName, pMMPFileInfo pFileInfo );

extern BYTE FAR PASCAL
mmpGetMovieInfo( MMPID id, pMMPMovieInfo pMovieInfo );

extern BYTE FAR PASCAL
mmpLoadFile( MMPID id, char FAR *pstrFileName, WORD fsOptions );

extern BYTE FAR PASCAL
mmpFreeFile( MMPID id, WORD fsOptions );

extern BYTE FAR PASCAL
mmpFileLoaded( MMPID id );




extern short FAR PASCAL
mmpError( MMPID id, LPSTR lpszErrorString);

extern BYTE FAR PASCAL
mmpAnimPending( MMPID id );

extern BYTE FAR PASCAL
mmpAnimStopped( MMPID id );

extern short FAR PASCAL
mmpFrameCtr( MMPID id );

extern BYTE FAR PASCAL
mmpIsLastFrame( MMPID id );



extern BYTE FAR PASCAL
mmpSetSquelch( MMPID id, BYTE bbSquelchOn );

extern BYTE FAR PASCAL
mmpSetStage( MMPID id, HWND hWndStage, RECT FAR * prectStage, WORD fsOptions );

extern BYTE FAR PASCAL
mmpGetStage( MMPID id, HWND FAR *phWndStage, RECT FAR * prectStage, LPWORD pfsOptions );

extern BYTE FAR PASCAL
mmpSetRepeat( MMPID id, BYTE bbRepeat );

extern BYTE FAR PASCAL
mmpGetRepeat( MMPID id );


extern BYTE FAR PASCAL
mmpSetDC( MMPID id, HDC hDC );

extern HPALETTE FAR PASCAL
mmpGetPaletteHandle( MMPID id );



