/*
	VWPROTO.H
	Prototypes and useful constants for VW.DLL and VWDOS.LIB
	Truevision, Inc.
	JF V1.0 9/23/91
*/

void far pascal VW_SetConfigBuf(void far *lpVCFG);
void far pascal VW_SetDefaultConfiguration(int nMode);
short far pascal VW_Init(int nMode);
void far pascal VW_CleanUp(void);
void far pascal VW_InitMode(int nMode);
int far pascal VW_LoadConfiguration(char far * lpCfgFileSpec);
int far pascal VW_SaveConfiguration(char far * lpCfgFileSpec);
void far pascal VW_SetIOAddr(int nPortAddr);
int far pascal VW_GetIOAddr(void);
void far pascal VW_Set8425Reg(int nRegNum, int nRegVal);
int far pascal VW_Get8425Reg(int nRegNum);
void far pascal VW_Set9051Reg(int nRegNum, int nRegVal);
int far pascal VW_Get9051Reg(int nRegNum);
void far pascal VW_Set4680Reg(int nRegNum, int nRegVal);
int far pascal VW_Get4680Reg(int nRegNum);
void far pascal VW_SetPCVReg(int nRegNum, int nRegVal);
int far pascal VW_GetPCVReg(int nRegNum);
void far pascal VW_SetI2CReg(int nAddr, int nRegNum, int nRegVal);

void far pascal VW_SetVidWindow(int nULX,int nULY,int nWidth,int nHeight, int fFit);
void far pascal VW_SetVidPan(int nXStart, int nYStart);
void far pascal VW_SetKeyColor(int nVGAColor);
void far pascal VW_SetFreezeVid(int fFreeze);
void far pascal VW_SetUVMask(int nUVMask);
void far pascal VW_SetYMask(int nYMask);
void far pascal VW_SetVidShow(int fVidShow);
void far pascal VW_SetVidStandard(int nVidForm);
int far pascal VW_GetVidStandard(void);
void far pascal VW_SetSVid(int fSVid);
int far pascal VW_GetSVid(void);
void far pascal VW_SetColor(int nColorReg, int nColorValue);
int far pascal VW_GetColor(int nColorReg);
void far pascal VW_ClearVidRect(int nULX, int nULY, int nWidth, int nHeight, int nPixVal);
void far pascal VW_SetVidAddr(int nVidAddr);
int far pascal VW_GetVidAddr(void);
void far pascal VW_SetVidSource(int nSrcVid);
int far pascal VW_GetVidSource(void);
void far pascal VW_SetXZoom(int nZoom);
void far pascal VW_SetYZoom(int nZoom);
void far pascal VW_SetFieldRep(int nRepVal);
int far pascal VW_GetFieldRep(void);
void far pascal VW_SetVidAcqRect(int nULX,int nULY,int nWidth,int nHeight);
void far pascal VW_SetVidAcqAddr(int nULX,int nULY);
void far pascal VW_SetRGBKeyColor(unsigned long lRGB);
void far pascal VW_SetScaleSize(int nWidth, int nHeight, int fFit);
void far pascal VW_SetVidRect(int nULX,int nULY,int nWidth,int nHeight);

void far pascal VW_SetMute(int fMute);
int far pascal VW_GetMute(void);
void far pascal VW_SetLeftVolume(int nVolValue);
int far pascal VW_GetLeftVolume(void);
void far pascal VW_SetRightVolume(int nVolValue);
int far pascal VW_GetRightVolume(void);
void far pascal VW_SetBass(int nBassValue);
int far pascal VW_GetBass(void);
void far pascal VW_SetTreble(int nTrebleValue);
int far pascal VW_GetTreble(void);
void far pascal VW_SetAudioSource(int nSrcValue);
int far pascal VW_GetAudioSource(void);
void far pascal VW_SetStereo(int nStereoValue);
int far pascal VW_GetStereo(void);
void far pascal VW_SetSpeakers(int fSpeaker);
int far pascal VW_GetSpeakers(void);

int far pascal VW_SaveMMP(char far * lpFile, int nULX, int nULY, int nWidth, int nHeight);
int far pascal VW_LoadMMP(char far * lpFile, int nULX, int nULY);
int far pascal VW_SaveTGA(char far * lpFile, int nULX, int nULY, int nWidth, int nHeight, int nBitsPerPix);
int far pascal VW_LoadTGA(char far * lpFile, int nULX, int nULY);
void far pascal VW_RGB2YUV(char far * lpRGBBuf, char far * lpYUVBuf, int nLineLen, int nBitsPerPix);
void far pascal VW_YUV2RGB(char far * lpYUVbuf, char far * lpRGBBuf, int nLineLen, int nBitsPerPix);
void far pascal VW_PackYUV(char far * lpUnpackedBuf, char far * lpPackedBuf, int nLineLen);
void far pascal VW_UnpackYUV(char far * lpPackedBuf, char far * lpUnpackedbuf, int nLineLen);
void far pascal VW_ReadFBRect(char far * lpDestBuf, int nULX, int nULY, int nWidth, int nHeight);
void far pascal VW_WriteFBRect(char far * lpSrcBuf, int nULX, int nULY, int nWidth, int nHeight);

/* Some Useful definitions */

/* Frame buffer size */
#define FULLVIDEOWIDTH 720
#define FULLVIDEOHEIGHT 512

/* Acquisition rectangle sizes */
#define VIDEOWIDTH 720
#define VIDEOHEIGHT 486
#define PALVIDEOWIDTH 720
#define PALVIDEOHEIGHT 576

/* Left framebuffer offset for load/save/clear (bytes) */
#define FBLEFTOFF 16

#define OVLINELEN 2048

#define PAL	0
#define NTSC	1
#define COMPOSIT 0
#define SVID	1

#define NPCV	0x51
#define N9051   12
#define N4680   16
#define N8425   9
#define NMISC   16

/* VW Configuration Structure */
typedef struct VWCFG {
	unsigned int	nBaseAddr;	/* IO adddress */
	unsigned int	nSelector[16]; /* Selectors used for accessing Frame Buffer */
	unsigned long	lFBAddr;	/* These parameters are contained in the */
	int	nDWX1, nDWY1, nDWWidth, nDWHeight; /* regs, but must be accessed often */
	int	nScaleX, nScaleY;
	int	nPanX, nPanY;
	int	nZoomX, nZoomY;
	int	nFreeze;
	int	nPCVVals[NPCV];	/* Reg values */
	int	n9051Vals[N9051];
	int	n4680Vals[N4680];
	int	n8425Vals[N8425];
	int	nMiscVals[NMISC];	/* space to store misc, application */
					/* specific info */
	int	nVersion;		/* These new fields were added at the */
	int	nFitWin;		/* end for backward compatibility */
	int	nFieldRep;
	int	nIncZoomY;
	int	nAcqRX1, nAcqRY1, nAcqRWidth, nAcqRHeight;
	int	nAcqAX1, nAcqAY1;
	int	nVidStandard;
	int	nReserved[5];
} VWCFG;

