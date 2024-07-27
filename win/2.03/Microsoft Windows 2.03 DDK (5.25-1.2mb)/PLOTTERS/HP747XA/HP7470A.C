/* Driver for the HP7470A plotter. */

#define LF_FACESIZE	    9
#define DF_MAPSIZE	    18

#include    "plotters.h"
#include    "hp7470A.h"
#include    "trans.dat"

extern FAR PASCAL SetRectEmpty();
extern	GDIINFO hp747xAGDIInfo;
BYTE rgCharSet[]={'C','S',0};

BYTE   *Trans[]={Trans7470,
		 Trans7475};

static	INTEGER hp747xAPenStyles[NUMPENSTYLES]
		= {
		-1		/* Solid. */
#ifdef	STYLED_LINES
		 ,
		2,		/* Dashed. */
		1,		/* Dotted. */
		5,		/* Dash-dotted. */
		6,		/* Dash-dot-dotted. */
		-1		/* Null. */
#endif
		};


static	FONTINFO    hp747xAFixedFont
		= {
		PF_VECTOR_TYPE | PF_BITS_IS_ADDRESS | PF_DEVICE_REALIZED,
				/* Type field for the font. */
		1,		/* Point size of font. */
		1,		/* Vertical digitization. */
		1,		/* Horizontal digitization. */
		1,		/* Baseline offset from top of character cell. */
		0,		/* The internal leading for the font. */
		1,		/* The external leading for the font. */
		0,		/* Flag specifying if italic. */
		0,		/* Flag specifying if underlined. */
		0,		/* Flag specifying if struck out. */
		FONTWEIGHT,	/* Weight of font. */
		CHARSET,	/* Character set of font. */
		1,		/* Width field for the font. */
		1,		/* Height field for the font. */
		FF_MODERN,	/* Flag specifying family and variable pitch. */
		1,		/* Average character width. */
		1,		/* Maximum character width. */
		FIRSTCHAR,	/* First character in the font. */
		LASTCHAR,	/* Last character in the font. */
		DEFAULTCHAR,	/* Default character for out of range. */
		BREAKCHAR,	/* Character to define wordbreaks. */
		1,		/* Number of bytes in each row. */
		sizeof(FONTINFO)-18,
				/* Offset to device name. */
		sizeof(FONTINFO)-9,
				/* Offset to face name. */
		0L,		/* Reserved pointer. */
		0L,		/* Offset to the begining of the bitmap. */
				/* Maps, etc. */
		'?', '?', '?', '?', '?', '?', '?', '?', 0,
		'M', 'o', 'd', 'e', 'r', 'n', 0, 0, 0
		};


static	LOGFONT hp747xALogFont
		= {
		1,
		1,
		0,
		0,
		FONTWEIGHT,
		0,
		0,
		0,
		CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FF_MODERN|FIXED_PITCH,
		'M', 'o', 'd', 'e', 'r', 'n', 0
		};


static	TEXTMETRIC  hp747xATextMetrics
		= {
		1,
		1,
		0,
		0,
		1,
		1,
		1,
		FONTWEIGHT,
		0,
		0,
		0,
		FIRSTCHAR,
		LASTCHAR,
		DEFAULTCHAR,
		BREAKCHAR,
		FF_MODERN,
		CHARSET,
		0,
		1,
		1
		};


DWORD FAR PASCAL
hp7470AColorInfo(lpPDevice, ColorIn, lpPColor)
LPPDEVICE	    lpPDevice;
DWORD		    ColorIn;
DWORD		    FAR *lpPColor;
{
    INTEGER	  Color;
    INTEGER	  Closest;
    INTEGER	  New;
    REGISTER	  INTEGER   Index;

    if(lpPColor){
	Closest = ColorDistance(ColorIn, *(lpPDevice->Colors));
	Color = 0;

	for(Index = 1; Index < lpPDevice->NumColors; Index++){
	    New = ColorDistance(ColorIn, *(lpPDevice->Colors + Index));
	    if(New < Closest){
		Color = Index;
		Closest = New;
	    }
	}

	return(*(lpPDevice->Colors + (*lpPColor = (DWORD)Color)));
    }
    else{
	/* Return the RGB value of lpPColor. */
	return(*(lpPDevice->Colors + (INTEGER)ColorIn));
    }
}



INTEGER FAR PASCAL
hp7470AControl(lpPDevice, Function, lpInData, lpOutData)
LPPDEVICE	    lpPDevice;
INTEGER 	    Function;
LPSTR		    lpInData;
LPSTR		    lpOutData;
{
    INTEGER	    Result;

    Result = 0;

    switch(Function){

	case NEXTBAND:

	    if(lpPDevice->Header.ChannelNumber == 0){
		Result = -1;
		/* 11/9/85 Linsh */
		SetRectEmpty((LPRECT)lpOutData);
		break;
	    }

	    ((LPRECT)lpOutData)->left = 0;
	    ((LPRECT)lpOutData)->top = 0;

#ifdef	BANDING
	    if(++(lpPDevice->Header.BandNumber) < lpPDevice->NumColors){
#else
	    if(!(lpPDevice->Header.BandNumber++)){
#endif
		if(lpPDevice->Header.Orientation == LANDSCAPE){
		    ((LPRECT)lpOutData)->right	= lpPDevice->Header.HorzRes;
		    ((LPRECT)lpOutData)->bottom = lpPDevice->Header.VertRes;
		}
		else {
		    ((LPRECT)lpOutData)->right	= lpPDevice->Header.VertRes;
		    ((LPRECT)lpOutData)->bottom = lpPDevice->Header.HorzRes;
		}
		Result = 1;
		break;
	    }

	    ((LPRECT)lpOutData)->right = 0;
	    ((LPRECT)lpOutData)->bottom = 0;

	    /* Fall into NEWFRAME. */

	case NEWFRAME:

	    Result = (lpPDevice->Header.ChannelNumber ? 1 : -1);

	    lpPDevice->Header.BandNumber = 0;

	    if(lpPDevice->Header.OutputInit){
		LIFTPEN();
		HPChangePen((LPPDEVICEHEADER)lpPDevice,
			    0,
			    (LPINT)&(lpPDevice->CurrentPen));
		SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"PA0,0;", 6);
		SETCURPOS(-1, -1);
		FlushBuffer((LPPDEVICEHEADER)lpPDevice);

#ifdef SPOOLING
		if(lpPDevice->Header.Spooling){
#endif
		    EndSpoolPage(lpPDevice->Header.ChannelNumber);
#ifdef SPOOLING
		}
#endif
		lpPDevice->Header.OutputInit = FALSE;
	    }
	    Result = (lpPDevice->Header.ChannelNumber ? 1 : -1);
	    break;

	case FLUSHOUTPUT:
	    FlushBuffer((LPPDEVICEHEADER)lpPDevice);
	    Result = 1;
	    break;

	case GETPHYSPAGESIZE:

	    if(lpPDevice->Header.Orientation == LANDSCAPE){
		((LPPOINT)lpOutData)->xcoord = lpPDevice->Header.PageSize.xcoord;
		((LPPOINT)lpOutData)->ycoord = lpPDevice->Header.PageSize.ycoord;
	    }
	    else {
		((LPPOINT)lpOutData)->xcoord = lpPDevice->Header.PageSize.ycoord;
		((LPPOINT)lpOutData)->ycoord = lpPDevice->Header.PageSize.xcoord;
	    }

	    Result = 1;
	    break;

	case GETPRINTINGOFFSET:

	    if(lpPDevice->Header.Orientation == LANDSCAPE){
		((LPPOINT)lpOutData)->xcoord = lpPDevice->Header.LandPageOffset.xcoord;
		((LPPOINT)lpOutData)->ycoord = lpPDevice->Header.LandPageOffset.ycoord;
	    }
	    else {
		((LPPOINT)lpOutData)->xcoord = lpPDevice->Header.PortPageOffset.xcoord;
		((LPPOINT)lpOutData)->ycoord = lpPDevice->Header.PortPageOffset.ycoord;
	    }
	    Result = 1;
	    break;

	case STARTDOC:

	    lpPDevice->Header.BandNumber = 0;
#if 0
	    if((lpPDevice->Header.ChannelNumber) &&
		(lpPDevice->Header.ChannelNumber != -1)){
		    /* Do an implicit ENDDOC if the user didn't. */
		(*(ControlFuncs[lpPDevice->Header.DeviceNumber]))
							   (lpPDevice,
							   ENDDOC,
							   0L,
							   0L);
	    }
	    else {
		lpPDevice->Header.OutputInit = FALSE;
	    }
#endif
	    if(OpenChannel((LPPDEVICEHEADER)lpPDevice,
			(LPSTR)lpPDevice + lpPDevice->Header.OutFileOffset,
			lpInData) == -1){

		Result = -1;
		break;
	    }

	    /* Initialize the device. */
	    (*(InitDeviceFuncs[lpPDevice->Header.DeviceNumber]))(lpPDevice, 1);

	    Result = 1;
	    break;

	case ENDDOC:
	    /* Do a NEWFRAME. */
	    (*(ControlFuncs[lpPDevice->Header.DeviceNumber]))(lpPDevice,
							      NEWFRAME,
							      0L,
							      0L);
	    CloseChannel((LPPDEVICEHEADER)lpPDevice);
	    Result = 1;
	    break;


	case QUERYESCSUPPORT:
	    switch(*(LPINT)lpInData){

		/* These are supported. */
		case NEWFRAME:
		case FLUSHOUTPUT:
		case QUERYESCSUPPORT:
		case SETABORTPROC:
		case GETPHYSPAGESIZE:
		case GETPRINTINGOFFSET:
		case STARTDOC:
		case ENDDOC:
		case ABORTDOC:
		case NEXTBAND:
		case SETCOLORTABLE:
		case GETCOLORTABLE:
		case GETVECTORPENSIZE:
		case GETVECTORBRUSHSIZE:
			Result = 1;
			break;

		/* These are not supported. */
		case DRAFTMODE:
			break;
	    }
	    break;

	case SETABORTPROC:
	    lpPDevice->Header.hDC = *(HDC FAR *)lpInData;
	    Result = 1;
	    break;

	case ABORTDOC:
	    /* Delete the job and set everything back to the start. */

	    lpPDevice->Header.Buffer.Count = 0;
	    lpPDevice->Header.OutputInit = FALSE;

#ifdef SPOOLING
	    if(lpPDevice->Header.Spooling){
#endif
		DeleteJob(lpPDevice->Header.ChannelNumber, 0);
		lpPDevice->Header.ChannelNumber = -1;
#ifdef SPOOLING
	    }
	    else {
		CloseChannel((LPPDEVICEHEADER)lpPDevice);
	    }
#endif

	    /* Rao - why sendstring when you have deleted the job? (linsh) */
	    SendString((LPPDEVICEHEADER)lpPDevice,
		       (LPSTR)"\03;PU;SP0;PA0,0;", 15);
	    SETCURPOS(-1, -1);

	    Result = 1;
	    break;

	case SETCOLORTABLE:
	case GETCOLORTABLE:
	    *(DWORD FAR *)lpOutData = *(lpPDevice->Colors +
			    (*(LPINT)lpInData < 0 ?
			     0 :
			     (*(LPINT)lpInData >= lpPDevice->NumColors ?
			      lpPDevice->NumColors-1 :
			      *(LPINT)lpInData)));
	    Result = 1;
	    break;

	case GETVECTORPENSIZE:
	case GETVECTORBRUSHSIZE:
	    /* return pen or brush size */
	    ((LPPOINT)lpOutData)->xcoord = 1;	/* width */
	    ((LPPOINT)lpOutData)->ycoord = 1;	/* height */
	    Result = 1;
	    break;

	case DRAFTMODE:
	    break;
    }

    return(Result);
}



INTEGER FAR PASCAL
hp7470ADisable(lpPDevice)
LPPDEVICE	    lpPDevice;
{
#if 0
    if((lpPDevice->Header.ChannelNumber) &&
	(lpPDevice->Header.ChannelNumber != -1)){
	    /* Do an implicit ENDDOC if the user didn't. */
	    (*(ControlFuncs[lpPDevice->Header.DeviceNumber]))(lpPDevice,
							      ENDDOC,
							      0L,
							      0L);
    }
#endif
}



INTEGER FAR PASCAL
hp7470ASetupEnable(lpPDevice, lpData)
LPPDEVICE	    lpPDevice;
LPSTR		    lpData;
{

    /* Set up the device dependent parts of GDIINFO. */

    /* Number of brushes the device has. */
    hp747xAGDIInfo.dpNumBrushes = NUMPENCOLORS;
    /* Number of pens the device has. */
    hp747xAGDIInfo.dpNumPens = NUMPENSTYLES * NUMPENCOLORS;
    /* Number of colors in color table. */
    hp747xAGDIInfo.dpNumColors = NUMPENCOLORS;
    /* Length of segment for line styles. */
    hp747xAGDIInfo.dpStyleLen = (500/RESOLUTION) * 10/8;

    /* Logical pixels/inch in X. */
    hp747xAGDIInfo.dpLogPixelsX = (STEPSPERMM * 254)/(10 * RESOLUTION);
    /* Logical pixels/inch in Y. */
    hp747xAGDIInfo.dpLogPixelsY = (STEPSPERMM * 254)/(10 * RESOLUTION);


    /* Use lpData if it's there. */
    if(lpData){
	switch (((LPMODEBLOCK)lpData)->PaperType){

	    case APAPER:
		hp747xAGDIInfo.dpHorzSize = XAPHYSICALUNITS/STEPSPERMM;
		hp747xAGDIInfo.dpVertSize = YAPHYSICALUNITS/STEPSPERMM;
		hp747xAGDIInfo.dpHorzRes  = XAPHYSICALUNITS/RESOLUTION;
		hp747xAGDIInfo.dpVertRes  = YAPHYSICALUNITS/RESOLUTION;

		if(lpPDevice){
		    lpPDevice->Header.PageSize.xcoord = (11 * STEPSPERMM * 254)/(10 * RESOLUTION);
		    lpPDevice->Header.PageSize.ycoord = (85 * STEPSPERMM * 254)/(10 * RESOLUTION * 10);

		    lpPDevice->Header.LandPageOffset.xcoord = 56;
		    lpPDevice->Header.LandPageOffset.ycoord = 64;
		    lpPDevice->Header.PortPageOffset.xcoord = 64;
		    lpPDevice->Header.PortPageOffset.ycoord = 0; /* ???????????@@@@@????????????? */
		}
		break;

	    case A4PAPER:
		hp747xAGDIInfo.dpHorzSize = XA4PHYSICALUNITS/STEPSPERMM;
		hp747xAGDIInfo.dpVertSize = YA4PHYSICALUNITS/STEPSPERMM;
		hp747xAGDIInfo.dpHorzRes  = XA4PHYSICALUNITS/RESOLUTION;
		hp747xAGDIInfo.dpVertRes  = YA4PHYSICALUNITS/RESOLUTION;

		if(lpPDevice){
		    lpPDevice->Header.PageSize.xcoord = (297 * STEPSPERMM)/RESOLUTION;
		    lpPDevice->Header.PageSize.ycoord = (210 * STEPSPERMM)/RESOLUTION;

		    lpPDevice->Header.LandPageOffset.xcoord = 56;
		    lpPDevice->Header.LandPageOffset.ycoord = 64;
		    lpPDevice->Header.PortPageOffset.xcoord = 64;
		    lpPDevice->Header.PortPageOffset.ycoord = 0; /* ???????????@@@@@????????????? */
		}
		break;

	}

	if(lpPDevice){
	    lpPDevice->Colors[0] = 0x00FFFFFF;
	    lpPDevice->Colors[1] = (((LPMODEBLOCK)lpData)->PenColors[0]);
	    lpPDevice->Colors[2] = (((LPMODEBLOCK)lpData)->PenColors[1]);
	}
    }
    else {
	hp747xAGDIInfo.dpHorzSize = XAPHYSICALUNITS/STEPSPERMM;
	hp747xAGDIInfo.dpVertSize = YAPHYSICALUNITS/STEPSPERMM;
	hp747xAGDIInfo.dpHorzRes  = XAPHYSICALUNITS/RESOLUTION;
	hp747xAGDIInfo.dpVertRes  = YAPHYSICALUNITS/RESOLUTION;

	if(lpPDevice){
	    lpPDevice->Colors[0] = 0x00FFFFFF;
	    lpPDevice->Colors[1] = 0x000000FF;
	    lpPDevice->Colors[2] = 0x00000000;

	    lpPDevice->Header.PageSize.xcoord = (11 * STEPSPERMM * 254)/(10 * RESOLUTION);
	    lpPDevice->Header.PageSize.ycoord = (85 * STEPSPERMM * 254)/(10 * RESOLUTION * 10);

	    lpPDevice->Header.LandPageOffset.xcoord = 56;
	    lpPDevice->Header.LandPageOffset.ycoord = 64;
	    lpPDevice->Header.PortPageOffset.xcoord = 64;
	    lpPDevice->Header.PortPageOffset.ycoord = 0; /* ???????????@@@@@????????????? */
	}
    }

    if(lpPDevice){
	lpPDevice->Header.OutFileOffset = (WORD)(lpPDevice->OutputFile) - (WORD)lpPDevice;
    }

    return(sizeof(PDEVICE));
}



INTEGER FAR PASCAL
hp7470AEnable(lpPDevice, Style, lpDeviceType, lpOutputFile, lpData, DeviceNumber)
LPPDEVICE	    lpPDevice;
INTEGER 	    Style;
LPSTR		    lpDeviceType;
LPSTR		    lpOutputFile;
LPSTR		    lpData;
INTEGER 	    DeviceNumber;
{
    PSTR		lpGDIInfo;
    LPSTR		Ptr;
    LPSTR		Ptr1;
    INTEGER		Count;

    char		FileBuffer[128];


    StripColon(lpOutputFile, (LPSTR)FileBuffer);

    if(Style & InquireInfo){

	/* Set up the device. */

	/* Make room on the end of the PDEVICE for the output file name. */
	Count = 0;
	Ptr = (LPSTR)FileBuffer;
	while (*Ptr++) Count++;

	/* Size required for the device descriptor. */
	hp747xAGDIInfo.dpDEVICEsize = (*(SetupEnableFuncs[DeviceNumber]))(0L, lpData) + Count;


	/* Transform the points to portrait if we must. */
	if((lpData) && ((((LPMODEBLOCK)lpData)->Orientation) == PORTRAIT)){
	    RotateGDIInfo((PGDIINFO)&hp747xAGDIInfo);
	}

	/* Fill in the scaling fields in the GDIINFO block. */
	ScaleGDIInfo((PGDIINFO)&hp747xAGDIInfo);

	/* Fill in the GDIINFO block. */
	lpGDIInfo = (PSTR)&hp747xAGDIInfo;
	for (Count = 0; Count < sizeof(GDIINFO); Count++){
	    (*(((LPSTR)lpPDevice)++)) = (*(lpGDIInfo++));
	}
	return(sizeof(GDIINFO));
    }

    else {
	/* Enable the device. */

	/* Find out about spooling. */
				      /* Use lpData if it's there. */
				      /* Spool by default. */
#ifdef SPOOLING
	lpPDevice->Header.Spooling = ((lpData) ? (((LPMODEBLOCK)lpData)->Spooling) : TRUE);
#endif

	/* Find out about slow pen speeds. */
	lpPDevice->Header.PenSpeed = ((lpData) ?
				      /* Use lpData if it's there. */
				      (((LPMODEBLOCK)lpData)->PenSpeed) :
				      /* Fast by default. */
				      FASTSPEED);

	/* Find out about orientation. */
	lpPDevice->Header.Orientation = ((lpData) ?
					 /* Use lpData if it's there. */
					 (((LPMODEBLOCK)lpData)->Orientation) :
					 /* Landscape by default. */
					 LANDSCAPE);

	/* Set up the device. */
	(*(SetupEnableFuncs[lpPDevice->Header.DeviceNumber = DeviceNumber]))(lpPDevice, lpData);

	/* Copy and save the filename. */
	Ptr = (LPSTR)lpPDevice + lpPDevice->Header.OutFileOffset;

	Ptr1 = (LPSTR)FileBuffer;
	while (*Ptr++ = *Ptr1++) ;

	lpPDevice->Header.Type = TRUE;
	lpPDevice->Header.Buffer.Length = BUFFERLENGTH;
	lpPDevice->Header.Buffer.Count = 0;

	lpPDevice->Header.OutputInit = FALSE;
	lpPDevice->Header.ChannelNumber = -1;
	lpPDevice->Header.hDC = 0;

	lpPDevice->Header.HorzRes = hp747xAGDIInfo.dpHorzRes;
	lpPDevice->Header.VertRes = hp747xAGDIInfo.dpVertRes;
	lpPDevice->NumColors = hp747xAGDIInfo.dpNumColors;
	lpPDevice->CurrentPen = 0;
	lpPDevice->PenDown = FALSE;
	lpPDevice->Clipped = FALSE;
	lpPDevice->CurPosX = -1;
	lpPDevice->CurPosY = -1;
	lpPDevice->FontHeight = 0;
	lpPDevice->FontWidth  = 0;
	lpPDevice->FontItalic = 0;

	/* Initialize the device constants only. */
	(*(InitDeviceFuncs[lpPDevice->Header.DeviceNumber]))(lpPDevice, 0);

	return(-1);
    }
}



INTEGER FAR PASCAL
hp7470AInitDevice(lpPDevice, InitAll)
LPPDEVICE	    lpPDevice;
BOOL		    InitAll;
{
    INTEGER	Args[4];

    lpPDevice->HeightNumerator = 2;
    lpPDevice->HeightDenominator = 3;
    lpPDevice->WidthNumerator = 2;
    lpPDevice->WidthDenominator = 3;

    if(!InitAll) return;

    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"\03;IN;IN;IM0;", 12);

    if(lpPDevice->Header.PenSpeed == SLOWSPEED)
	SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"VS10;", 5);
    else
	SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"VS;", 3);

    if(lpPDevice->Header.Orientation == LANDSCAPE)
	SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"DI;", 3);
    else
	SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"DI0,-1;", 7);

    Args[0] = 0;
    Args[1] = 0;
    Args[2] = (lpPDevice->Header.HorzRes)*RESOLUTION;
    Args[3] = (lpPDevice->Header.VertRes)*RESOLUTION;

    HPSendCommand((LPPDEVICEHEADER)lpPDevice, (LPSTR)"IW", (LPINT)Args, 4, 0, 0);

    HPSendCommand((LPPDEVICEHEADER)lpPDevice, (LPSTR)"IP", (LPINT)Args, 4, 0, 0);

    Args[1] = (lpPDevice->Header.HorzRes);
    Args[2] = 0;
    Args[3] = (lpPDevice->Header.VertRes);

    HPSendCommand((LPPDEVICEHEADER)lpPDevice, (LPSTR)"SC", (LPINT)Args, 4, 0, 0);


    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"PU;SP0;PA0,0;", 13);
}



INTEGER FAR PASCAL
hp7470AEnumDFonts(lpPDevice, lpFacename, lpCallbackFunc, lpData)
LPPDEVICE	    lpPDevice;
LPSTR		    lpFacename;
FARPROC 	    lpCallbackFunc;
LPSTR		    lpData;
{
    LOGFONT		LogFont;
    TEXTMETRIC		TextMetrics;
    INTEGER		Count;
    PSTR		Ptr;
    LPSTR		LPtr;

    if((!lpFacename) || (EqualNames(lpFacename, (LPSTR)"Modern"))){

	/* Enumerate the one font we have. */

	/* Fill in the LOGFONT block. */
	Ptr = (PSTR)&hp747xALogFont;
	LPtr = (LPSTR)&LogFont;
	for (Count = 0; Count < sizeof(LOGFONT); Count++){
	    (*(LPtr++)) = (*(Ptr++));
	}

	/* Fill in the TEXTMETRIC block. */
	Ptr = (PSTR)&hp747xATextMetrics;
	LPtr = (LPSTR)&TextMetrics;
	for (Count = 0; Count < sizeof(TEXTMETRIC); Count++){
	    (*(LPtr++)) = (*(Ptr++));
	}

	return((*lpCallbackFunc)((LOGFONT FAR *)&LogFont,
				  (TEXTMETRIC FAR *)&TextMetrics,
				  (INTEGER)DEVICE_FONTTYPE,
				  lpData));
    }

    return(1);
}



INTEGER FAR PASCAL
hp7470AEnumObj(lpPDevice, Style, lpCallbackFunc, lpData)
LPPDEVICE	    lpPDevice;
INTEGER 	    Style;
FARPROC 	    lpCallbackFunc;
LPSTR		    lpData;
{
    INTEGER		LineStyle;
    INTEGER		Color;
    INTEGER		Result;
    LOGPEN		LogPen;
    LOGBRUSH		LogBrush;

    switch (Style){

	case OBJ_PEN:
	    LogPen.lopnWidth.xcoord = 0;
	    LogPen.lopnWidth.ycoord = 0;
	    for (LineStyle = 0; LineStyle < NUMPENSTYLES; LineStyle++){

		LogPen.lopnStyle = LineStyle;
		for (Color = 0; Color < lpPDevice->NumColors; Color++){


		    LogPen.lopnColor =
				     *(lpPDevice->Colors + Color);
		    Result = (*lpCallbackFunc)(
					    (LOGPEN FAR *)&LogPen,
					    lpData);
		    if(!Result) return(0);
		}
	    }
	    return(Result);
	    break;

	case OBJ_BRUSH:
	    LogBrush.lbStyle = BS_SOLID;
	    for (Color = 0; Color < lpPDevice->NumColors; Color++){
		LogBrush.lbColor = *(lpPDevice->Colors + Color);
		Result = (*lpCallbackFunc)((LOGBRUSH FAR *)&LogBrush, lpData);

		if(!Result) return(0);
	    }
	    return(Result);
	    break;
    }

    return(0);
}



INTEGER FAR PASCAL
hp7470AOutput(lpPDevice, Style, Count, lpPoints, lpPPen, lpPBrush, lpDrawMode, lpClipRect)
LPPDEVICE	    lpPDevice;
INTEGER 	    Style;
INTEGER 	    Count;
LPPOINT 	    lpPoints;
LPSTR		    lpPPen;
LPSTR		    lpPBrush;
DRAWMODE	    FAR *lpDrawMode;
LPRECT		    lpClipRect;
{
    INTEGER	Args[2];
    INTEGER	PenStyle;
    INTEGER	StyledLines;

/* NOP on the boring cases of the white pen. */
    switch (Style){

	case OS_POLYLINE:
	    if(!(*((LPINT)lpPPen))) return;
#ifdef	BANDING
	    if(WRONGBAND(lpPDevice, (*((LPINT)lpPPen)))) return;
#endif
	    break;

	case OS_SCANLINES:
	    if(!(lpPBrush ? *((LPINT)lpPBrush) : *((LPINT)lpPPen  )))
		    return;
#ifdef	BANDING
	    if(WRONGBAND(lpPDevice,
			  (lpPBrush ? *((LPINT)lpPBrush) :
				      *((LPINT)lpPPen  )))) return;
#endif
	    break;
    }

    if(lpClipRect){
	HPSetClipRect((LPPDEVICEHEADER)lpPDevice, lpClipRect, RESOLUTION, lpPDevice->Header.VertRes);
	lpPDevice->Clipped = TRUE;
    }
    else {
	if(lpPDevice->Clipped){
	    HPRestoreClipRect((LPPDEVICEHEADER)lpPDevice,
			      lpPDevice->Header.HorzRes,
			      lpPDevice->Header.VertRes,
			      RESOLUTION);
	    lpPDevice->Clipped = FALSE;
	}
    }

    switch (Style){

	case OS_POLYLINE:
	    PenStyle = *(((LPINT)lpPPen)+1);
	    /* 0 and NUMPENSTYLES-1 are the solid and null pens. */
	    if((PenStyle > 0) && (PenStyle < NUMPENSTYLES-1)){
		HPSendCommand((LPPDEVICEHEADER)lpPDevice,
			      (LPSTR)"LT",
			      (LPINT)(hp747xAPenStyles+PenStyle),
			      1, 0, 0);
		StyledLines = TRUE;
	    }
	    else {
		StyledLines = FALSE;
	    }

	    /* A user might do 'MoveTo, LineTo, LineTo, etc'.
	       In that case we don't need to send all the coordinates,
	       since many will be the same. */
	    if(((lpPoints->xcoord) != (lpPDevice->CurPosX)) || ((lpPoints->ycoord) != (lpPDevice->CurPosY))){
		LIFTPEN();
		HPSendCommand((LPPDEVICEHEADER)lpPDevice,
			      (LPSTR)"PA", (LPINT)lpPoints, 2,
			      lpPDevice->Header.VertRes, 1);
	    }

	    HPChangePen((LPPDEVICEHEADER)lpPDevice,
			*((LPINT)lpPPen),
			(LPINT)&(lpPDevice->CurrentPen));
	    DROPPEN();

	    HPSendCommand((LPPDEVICEHEADER)lpPDevice,
			  (LPSTR)"PA", (LPINT)(lpPoints+1), 2*(Count-1),
			  lpPDevice->Header.VertRes, 1);

	    SETCURPOS(*(((LPINT)lpPoints) + 2*Count - 2),
		      *(((LPINT)lpPoints) + 2*Count - 1));

	    if(StyledLines){
		SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"LT;", 3);
		StyledLines = FALSE;
	    }
	    break;

	case OS_SCANLINES:
	    LIFTPEN();
	    HPChangePen((LPPDEVICEHEADER)lpPDevice,
			(lpPBrush ? *((LPINT)lpPBrush) :
				    *((LPINT)lpPPen  )),
			(LPINT)&(lpPDevice->CurrentPen));
	    ((LPINT)lpPoints)++;
	    Args[1] = *(((LPINT)lpPoints)++);
	    for (Count--; Count > 0; Count--){
		LIFTPEN();
		Args[0] = *(((LPINT)lpPoints)++);
		HPSendCommand((LPPDEVICEHEADER)lpPDevice,
			      (LPSTR)"PA", (LPINT)Args, 2,
			      lpPDevice->Header.VertRes, 1);
		DROPPEN();
		Args[0] = (*(((LPINT)lpPoints)++))-1;
		HPSendCommand((LPPDEVICEHEADER)lpPDevice,
			      (LPSTR)"PA", (LPINT)Args, 2,
			      lpPDevice->Header.VertRes, 1);
	    }
	    SETCURPOS(Args[0], Args[1]);
	    break;

	case OS_ARC:
	case OS_RECTANGLE:
	case OS_ELLIPSE:
	case OS_PIE:
	case OS_POLYMARKER:
	case OS_CHORD:
	case OS_CIRCLE:
	    break;
    }
}



DWORD FAR PASCAL
hp7470APixel(lpPDevice, X, Y, PhysColor, lpDrawMode)
LPPDEVICE	    lpPDevice;
INTEGER 	    X;
INTEGER 	    Y;
DWORD		    PhysColor;
DRAWMODE	    FAR *lpDrawMode;
{
    INTEGER	    Args[2];

    if(lpDrawMode){

	/* NOP on the boring cases of the white pen. */
	if(!(INTEGER)PhysColor) return(0L);

#ifdef	BANDING
	if(WRONGBAND(lpPDevice, (INTEGER)PhysColor)) return(0L);
#endif

	if(lpPDevice->Clipped){
	    HPRestoreClipRect((LPPDEVICEHEADER)lpPDevice,
			      lpPDevice->Header.HorzRes,
			      lpPDevice->Header.VertRes,
			      RESOLUTION);
	    lpPDevice->Clipped = FALSE;
	}

	LIFTPEN();
	HPChangePen((LPPDEVICEHEADER)lpPDevice,
		    (INTEGER)PhysColor,
		    (LPINT)&(lpPDevice->CurrentPen));

	SETCURPOS(Args[0] = X, Args[1] = Y);

	HPSendCommand((LPPDEVICEHEADER)lpPDevice,
		      (LPSTR)"PA", (LPINT)Args, 2,
		      lpPDevice->Header.VertRes, 1);

	SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"PD;", 3);
	lpPDevice->PenDown = TRUE;
    }

    return(0L);
}



INTEGER FAR PASCAL
hp7470ARealizeObject(lpPDevice, Style, lpInObj, lpOutObj, PlpTextXForm)
LPPDEVICE	    lpPDevice;
INTEGER 	    Style;
LPSTR		    lpInObj;
LPSTR		    lpOutObj;
TEXTXFORM	    FAR *PlpTextXForm;
{
    PSTR     Ptr;
    LPSTR    LPtr;
    LPSTR    LPtr1;
    INTEGER  Count;
    INTEGER  Height;
    INTEGER  Width;
    INTEGER  Weight;
    REGISTER TEXTXFORM	FAR *lpTextXForm;

    if(Style < 0) return(0);

    if(lpOutObj){
	/* Realize it. */

	switch (Style){

	    case OBJ_PEN:
		/* Map to a physical color in the low word. */
		(*(ColorInfoFuncs[(lpPDevice->Header).DeviceNumber]))
			(lpPDevice,
			 (((LOGPEN FAR *)lpInObj)->lopnColor),
			 lpOutObj);

		/* Set the color to 0 if width < 0. */
		if((((LOGPEN FAR *)lpInObj)->lopnWidth).xcoord < 0){
		    *(LPINT)lpOutObj = 0;
		}

#ifdef	STYLED_LINES
		/* Put the style in the high word. */
		if((*(((LPINT)lpOutObj)+1) =
			  (((LOGPEN FAR *)lpInObj)->lopnStyle)) ==
						 NUMPENSTYLES-1){
		    /* The null pen is color 0. */
		    *(LPINT)lpOutObj = 0;
		}
#else
		/* The style is always solid. */
		*(((LPINT)lpOutObj)+1) = 0;
#endif
		return(4);
		break;

	    case OBJ_BRUSH:
		(*(ColorInfoFuncs[(lpPDevice->Header).DeviceNumber]))
			(lpPDevice,
			 (((LOGBRUSH FAR *)lpInObj)->lbColor),
			 lpOutObj);
		return(4);
		break;

	    case OBJ_FONT:
		/* Fill in the basic font. */
		Ptr = (PSTR)&hp747xAFixedFont;
		LPtr = lpOutObj;
		for (Count = 0; Count < sizeof(FONTINFO); Count++)
		    (*(LPtr++)) = (*(Ptr++));

		/* Fill in the device name. */
		LPtr1 = lpOutObj + (INTEGER)(hp747xAFixedFont.dfDevice);
		LPtr = Devices[(lpPDevice->Header).DeviceNumber];
		for (Count = 0; Count < (DF_MAPSIZE - LF_FACESIZE - 1); Count++){
		    (*(LPtr1++)) = (*(LPtr++));
		}

/* ------------------------------------------------------------------------ */
/* To be implemented.
(((LOGFONT FAR *)lpInObj)->lfEscapement)
(((LOGFONT FAR *)lpInObj)->lfOrientation)
(((LOGFONT FAR *)lpInObj)->lfOutPrecision)
(((LOGFONT FAR *)lpInObj)->lfClipPrecision)
/* ------------------------------------------------------------------------ */

		/* Since InternalLeading is zero,
		   we can ignore negative heights. */
		Height = Abs(((LOGFONT FAR *)lpInObj)->lfHeight);
		if(!Height){
		    Height = MulDiv(lpPDevice->Header.VertRes,
				 lpPDevice->HeightDenominator*3,
				 lpPDevice->HeightNumerator*2*100);
		}
		Width  = Abs(((LOGFONT FAR *)lpInObj)->lfWidth);
		if(!Width){
		    Width = ((long)lpPDevice->Header.HorzRes*
			     (long)lpPDevice->WidthDenominator*
			     (long)Height*
			     (long)lpPDevice->HeightNumerator)
				 /
			    ((long)lpPDevice->WidthNumerator*2*
			     (long)lpPDevice->Header.VertRes*
			     (long)lpPDevice->HeightDenominator);
#if 0
		    Width = MulDiv(lpPDevice->Header.HorzRes,
				 lpPDevice->WidthDenominator*3,
				 lpPDevice->WidthNumerator*4*100);
#endif
		}

		/* Patch the fields we need to. */
		(((FONTINFO FAR *)lpOutObj)->dfAscent) =
		   ( (((FONTINFO FAR *)lpOutObj)->dfExternalLeading) =
				MulDiv(Height,
				       lpPDevice->HeightNumerator,
				       lpPDevice->HeightDenominator));
		(((FONTINFO FAR *)lpOutObj)->dfItalic) =
			     ((((LOGFONT FAR *)lpInObj)->lfItalic) & 1);
		(((FONTINFO FAR *)lpOutObj)->dfUnderline) =
			  ((((LOGFONT FAR *)lpInObj)->lfUnderline) & 1);
		(((FONTINFO FAR *)lpOutObj)->dfStrikeOut) =
			  ((((LOGFONT FAR *)lpInObj)->lfStrikeOut) & 1);
		(((FONTINFO FAR *)lpOutObj)->dfPixWidth) = Width;
		(((FONTINFO FAR *)lpOutObj)->dfPixHeight) = Height;
		(((FONTINFO FAR *)lpOutObj)->dfAvgWidth) = Width;
		(((FONTINFO FAR *)lpOutObj)->dfMaxWidth) = Width;

		/* Fill in the font transform. */
		lpTextXForm = PlpTextXForm;

		lpTextXForm->ftHeight	     = Height;
		lpTextXForm->ftWidth	     = Width;
		lpTextXForm->ftEscapement    = 0;
		lpTextXForm->ftOrientation   = 0;
		lpTextXForm->ftWeight	     =
			       (((FONTINFO FAR *)lpOutObj)->dfWeight);
		lpTextXForm->ftItalic	     =
			       (((FONTINFO FAR *)lpOutObj)->dfItalic);
		lpTextXForm->ftUnderline     =
			    (((FONTINFO FAR *)lpOutObj)->dfUnderline);
		lpTextXForm->ftStrikeOut     =
			    (((FONTINFO FAR *)lpOutObj)->dfStrikeOut);
		lpTextXForm->ftOutPrecision  =
			   (((LOGFONT FAR *)lpInObj)->lfOutPrecision);
		lpTextXForm->ftClipPrecision =
			  (((LOGFONT FAR *)lpInObj)->lfClipPrecision);

		/* Fill in the font transform accelerator. */
		lpTextXForm->ftAccelerator   = 0;

		if(lpTextXForm->ftOutPrecision ==
					       OUT_CHARACTER_PRECIS){
			lpTextXForm->ftOutPrecision |=
						      TC_OP_CHARACTER;
		}
		else if(lpTextXForm->ftOutPrecision ==
						  OUT_STROKE_PRECIS){
			lpTextXForm->ftOutPrecision |=
				       (TC_OP_STROKE|TC_OP_CHARACTER);
		}

		if(lpTextXForm->ftClipPrecision ==
						 CLIP_STROKE_PRECIS){
			lpTextXForm->ftClipPrecision |= TC_CP_STROKE;
		}

		if((Weight = (((LOGFONT FAR *)lpInObj)->lfWeight)) &&
#ifdef	OLDWEIGHTS
		    (Abs(Weight - ((lpTextXForm->ftWeight) +
						      (1000/Width))) <
		     Abs(Weight -   (lpTextXForm->ftWeight)))){
			lpTextXForm->ftWeight += (1000/Width);
#else
		    (Weight > ((lpTextXForm->ftWeight) +
					  ((FW_BOLD-FW_NORMAL)/2)))){
			lpTextXForm->ftWeight += (FW_BOLD-FW_NORMAL);
#endif
			lpTextXForm->ftAccelerator |= TC_EA_DOUBLE;
		}

		if(lpTextXForm->ftItalic){
			lpTextXForm->ftOverhang = (((FONTINFO FAR *)lpOutObj)->dfAscent) / 2;
		}
		else {
		    lpTextXForm->ftOverhang = 0;
		}

		return(sizeof(FONTINFO));
		break;

	    default:
		return(0);
		break;
	}
    }
    else {
	/* Return the size. */
	switch (Style){

	    case OBJ_PEN:
	    case OBJ_BRUSH:
		return(4);
		break;

	    case OBJ_FONT:
		return(sizeof(FONTINFO));
		break;

	    default:
		return(0);
		break;
	}
    }
}


DWORD FAR PASCAL
hp7470AStrBlt(lpPDevice, DestxOrg, DestyOrg, lpClipRect,
			     lpString, Count, lpFont, lpDrawMode, lpTextXForm)
LPPDEVICE	    lpPDevice;
INTEGER 	    DestxOrg;
INTEGER 	    DestyOrg;
LPRECT		    lpClipRect;
LPSTR		    lpString;
INTEGER 	    Count;
FONTINFO	    FAR *lpFont;
DRAWMODE	    FAR *lpDrawMode;
TEXTXFORM	    FAR *lpTextXForm;
{
    INTEGER		FontHeight;
    INTEGER		FontAscent;
    INTEGER		FontWidth;
    INTEGER		FontItalic;
    INTEGER		Extent;
    INTEGER		ExtraSpacing;
    char		Buffer[6];
    INTEGER		Args[4];
    INTEGER		YOffset;
    INTEGER		XSize100P;
    INTEGER		YSize100P;
    BYTE		FirstChar;
    BYTE		LastChar;
    BYTE		DefaultChar;
    BYTE		BreakChar;
    INTEGER		BreakErr;
    BYTE		Char;
    BYTE		CharSet=CHARSET0;
    INTEGER		DeviceNo;

    FontHeight = (lpFont->dfPixHeight);
    FontAscent = (lpFont->dfAscent);
    FontWidth  = (lpFont->dfAvgWidth);
    FontItalic = (lpFont->dfItalic);

    FirstChar	= (lpFont->dfFirstChar);
    LastChar	= (lpFont->dfLastChar);
    DefaultChar = (lpFont->dfDefaultChar) + FirstChar;
    BreakChar	= (lpFont->dfBreakChar) + FirstChar;

/* Are we plotting a string or getting the extent? */
    if(Count > 0){

#ifdef	BANDING
	if(WRONGBAND(lpPDevice, (INTEGER)(lpDrawMode->TextColor))) return(0L);
#endif

	/* Check if the string is completely clipped out. */
	if( (DestyOrg > (lpClipRect->bottom)) ||
	     ((DestyOrg + lpFont->dfPixHeight) < (lpClipRect->top)) ){
		return(0L);
		}

	/* Plot a string. */

	/* Set Clipping. */
	HPSetClipRect((LPPDEVICEHEADER)lpPDevice, lpClipRect,
		      RESOLUTION, lpPDevice->Header.VertRes);
	lpPDevice->Clipped = TRUE;

	/* Set the color. */
	LIFTPEN();
	HPChangePen((LPPDEVICEHEADER)lpPDevice,
		    (INTEGER)(lpDrawMode->TextColor),
		    (LPINT)&(lpPDevice->CurrentPen));

	/* Set the height and width. */
	if(((lpPDevice->FontHeight) != FontHeight) || ((lpPDevice->FontWidth ) != FontWidth )){

	    (lpPDevice->FontHeight) = FontHeight;
	    (lpPDevice->FontWidth ) = FontWidth;

	    /* Figure the sizes in 100th's of a percent of
	       the total plotting area. */
	    XSize100P = ((long)FontWidth *
				    (lpPDevice->WidthNumerator)*100*100) /
			((lpPDevice->Header.HorzRes) *
					   (lpPDevice->WidthDenominator));
	    YSize100P = ((long)FontAscent*100*100) /
			(lpPDevice->Header.VertRes);

	    /* Send the size string a piece at a time. */
	    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"SR", 2);
	    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)Buffer,
		       IntToAscii(XSize100P/100,
				  (LPSTR)Buffer, sizeof(Buffer)));
	    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)".", 1);
	    XSize100P = XSize100P - (XSize100P/100)*100;

	    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)Buffer,
		       IntToAscii(XSize100P/10,
				  (LPSTR)Buffer, sizeof(Buffer)));

	    XSize100P = XSize100P - (XSize100P/10)*10;

	    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)Buffer,
		       IntToAscii(XSize100P,
				  (LPSTR)Buffer, sizeof(Buffer)));

	    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)",", 1);
	    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)Buffer,
		       IntToAscii(YSize100P/100,
				  (LPSTR)Buffer, sizeof(Buffer)));
	    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)".", 1);

	    YSize100P = YSize100P - (YSize100P/100)*100;

	    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)Buffer,
		       IntToAscii(YSize100P/10,
				  (LPSTR)Buffer, sizeof(Buffer)));

	    YSize100P = YSize100P - (YSize100P/10)*10;

	    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)Buffer,
		       IntToAscii(YSize100P,
				  (LPSTR)Buffer, sizeof(Buffer)));

	    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)";", 1);
	}

	/* Set italic. */
	if((lpPDevice->FontItalic) != FontItalic){
	    if((lpPDevice->FontItalic) = FontItalic)
		SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"SL.5;", 5);
	    else
		SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"SL;", 3);
	}

	/* Move to the start. */
	Args[0] = DestxOrg;
	Args[1] = DestyOrg;
	HPSendCommand((LPPDEVICEHEADER)lpPDevice,
		      (LPSTR)"PA", (LPINT)Args, 2,
		      lpPDevice->Header.VertRes, 1);

	/* Set the character origin to the upper left. */
	SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"CP0,-.5;", 8);

	/* Start sending a string. */
	SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"LB", 2);

	Extent = Count*FontWidth;

	BreakErr = (lpDrawMode->BreakErr);

	for (; Count; Count--){
	    Char = (*(lpString++));

	    /* Translate out of range characters. */

	    ExtraSpacing = 0;

	    /* Suppress text if there is no pen selected. */
	    if(lpPDevice->CurrentPen){
		DeviceNo = lpPDevice->Header.DeviceNumber;

		if(Char >= TRANS_MIN){
		    if((Trans[DeviceNo][(Char-TRANS_MIN) * 3] < CHARSET0)
		      ||(Trans[DeviceNo][(Char-TRANS_MIN) * 3] > CHARSET7)){
			if(CharSet != CHARSET0){
			    rgCharSet[2] = CharSet = CHARSET0;
			    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"\03;", 2);
			    SendString((LPPDEVICEHEADER)lpPDevice,rgCharSet,3);
			    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"LB", 2);
			}
			SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)&Trans[DeviceNo][(Char-TRANS_MIN) * 3], 3);
		    }
		    else{
			if(CharSet != Trans[DeviceNo][(Char-TRANS_MIN) * 3]){
			    rgCharSet[2] = CharSet = Trans[DeviceNo][(Char-TRANS_MIN) * 3];
			    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"\03;", 2);
			    SendString((LPPDEVICEHEADER)lpPDevice,rgCharSet,3);
			    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"LB", 2);
			}
			if(Trans[DeviceNo][(Char-TRANS_MIN) * 3 + 2] == NULL)
			    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)&Trans[DeviceNo][(Char-TRANS_MIN) * 3 + 1], 1);
			else
			    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)&Trans[DeviceNo][(Char-TRANS_MIN) * 3 + 1], 2);

		    }
		}
		else{
		    if((Char < FirstChar) || ((Char > ANSI_MAX) && (Char < TRANS_MIN)))
			Char = DefaultChar;

		    if(CharSet != CHARSET0){
			rgCharSet[2] = CharSet = CHARSET0;
			SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"\03;", 2);
			SendString((LPPDEVICEHEADER)lpPDevice,rgCharSet,3);
			SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"LB", 2);
		    }
		    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)&Char, 1);
		}
	    }
	    /* Put the CharExtra BETWEEN each character. */
	    if(Count > 1){
		ExtraSpacing += (lpDrawMode->CharExtra);
	    }

	    if((lpDrawMode->TBreakExtra) && (Char == BreakChar)){

		/* Justify. */
		if(Char == BreakChar){
		    /* Move over by (lpDrawMode->BreakExtra). */
		    ExtraSpacing += (lpDrawMode->BreakExtra);
		    BreakErr -= (lpDrawMode->BreakRem);
		    if(BreakErr <= 0){	/* Move over by 1. */
			ExtraSpacing++;
			BreakErr += (lpDrawMode->BreakCount);
		    }
		}
	    }

	    Extent += ExtraSpacing;

	    if(ExtraSpacing && (lpPDevice->CurrentPen)){
		HPInsertTextPixels((LPPDEVICEHEADER)lpPDevice, ExtraSpacing);
	    }
	}

	/* Terminate the string. */
	SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"\03;", 2);



	if(CharSet != CHARSET0){
	    rgCharSet[2] = CHARSET0;
	    SendString((LPPDEVICEHEADER)lpPDevice, rgCharSet, 3);
	}

	/* Put on any underline. */
	if(lpFont->dfUnderline){
	    Args[0] = DestxOrg;
	    Args[1] = DestyOrg+FontAscent+1;
	    Args[2] = Args[0]+Extent-1;
	    Args[3] = Args[1];
	    HPSendCommand((LPPDEVICEHEADER)lpPDevice,
			  (LPSTR)"PA", (LPINT)Args, 2,
			  lpPDevice->Header.VertRes, 1);
	    DROPPEN();
	    HPSendCommand((LPPDEVICEHEADER)lpPDevice,
			  (LPSTR)"PA", (LPINT)Args+2, 2,
			  lpPDevice->Header.VertRes, 1);
	    LIFTPEN();
	}

	/* Put on any strikeout. */
	if(lpFont->dfStrikeOut){
	    YOffset = (FontAscent*2)/3;
	    /* The / 2 term assumes an SL command of .5. */
	    Args[0] = DestxOrg + ((FontAscent-YOffset)*FontItalic)/2;
	    Args[1] = DestyOrg+YOffset;
	    Args[2] = Args[0]+Extent-1;
	    Args[3] = Args[1];
	    HPSendCommand((LPPDEVICEHEADER)lpPDevice,
			  (LPSTR)"PA", (LPINT)Args, 2,
			  lpPDevice->Header.VertRes, 1);
	    DROPPEN();
	    HPSendCommand((LPPDEVICEHEADER)lpPDevice,
			  (LPSTR)"PA", (LPINT)Args+2, 2,
			  lpPDevice->Header.VertRes, 1);
	}

	SETCURPOS(-1, -1);
    }
    else if(Count < 0){

	/* Get the extent. */

	Count = -Count;

	Extent = Count * (FontWidth + (lpDrawMode->CharExtra));

	if(lpDrawMode->TBreakExtra){
	    for (; Count; Count--){
		Char = (*(lpString++));

		/* Translate out of range characters. */
		if((Char < FirstChar) || (Char > LastChar)){
			Char = DefaultChar;
			}

		/* Justify. */
		if(Char == BreakChar){
		    Extent += (lpDrawMode->BreakExtra);
		    (lpDrawMode->BreakErr) -=
					   (lpDrawMode->BreakRem);
		    if((lpDrawMode->BreakErr) <= 0){
			Extent++;
			lpDrawMode->BreakErr += lpDrawMode->BreakCount;
		    }
		}
	    }
	}

	/* FontItalic is always 0 or 1. */
	/* The (FontAscent/2) term assumes an SL command of .5. */
	return((((DWORD)FontHeight)<<16) | (Extent + (FontItalic*FontAscent/2)));

    }

    return(0L);
}



INTEGER FAR PASCAL
hp7470AScanLR(lpPDevice, X, Y, PhysColor, Style)
LPPDEVICE	    lpPDevice;
INTEGER 	    X;
INTEGER 	    Y;
DWORD		    PhysColor;
INTEGER 	    Style;
{
    return(X);
}
