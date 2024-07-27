/* The main code for plotters. */

#include    "plotters.h"



INTEGER FAR PASCAL
BitBlt(lpDstDev, DstxOrg, DstyOrg, lpSrcDev, SrcxOrg, SrcyOrg, xext, yext, Rop, lpPBrush, lpDrawMode)

LPPDEVICEHEADER     lpDstDev;
INTEGER 	    DstxOrg;
INTEGER 	    DstyOrg;
LPPDEVICEHEADER     lpSrcDev;
INTEGER 	    SrcxOrg;
INTEGER 	    SrcyOrg;
INTEGER 	    xext;
INTEGER 	    yext;
DWORD		    Rop;
LPSTR		    lpPBrush;
DRAWMODE	    lpDrawMode;
{
}



DWORD FAR PASCAL
ColorInfo(lpPDeviceHeader, ColorIn, lpPColor)
LPPDEVICEHEADER     lpPDeviceHeader;
DWORD		    ColorIn;
DWORD		    FAR *lpPColor;
{
    REGISTER DWORD	Result;

    if(!(lpPDeviceHeader->Type)){     /* It's a memory bitmap. */
	return(0L);
    }

    LockData();

    Result = (*(ColorInfoFuncs[lpPDeviceHeader->DeviceNumber]))(lpPDeviceHeader, ColorIn, lpPColor);

    UnlockData();

    return(Result);
}



INTEGER FAR PASCAL
Control(lpPDeviceHeader, function, lpInData, lpOutData)
LPPDEVICEHEADER     lpPDeviceHeader;
INTEGER 	    function;
LPSTR		    lpInData;
LPSTR		    lpOutData;
{
    REGISTER INTEGER	    Result;

    if(!(lpPDeviceHeader->Type)){    /* It's a memory bitmap. */
	return(0);
    }

    LockData();

    Result = (*(ControlFuncs[lpPDeviceHeader->DeviceNumber]))(lpPDeviceHeader, function, lpInData, lpOutData);

    UnlockData();

    return(Result);
}



INTEGER FAR PASCAL
Disable(lpPDeviceHeader)
LPPDEVICEHEADER     lpPDeviceHeader;
{
    if(!(lpPDeviceHeader->Type)){   /* It's a memory bitmap. */
	return;
    }

    LockData();

    (*(DisableFuncs[lpPDeviceHeader->DeviceNumber]))(lpPDeviceHeader);

    UnlockData();
}


INTEGER FAR PASCAL
Enable(lpPDeviceHeader, Style, lpDeviceType, lpOutputFile, lpData)
LPPDEVICEHEADER     lpPDeviceHeader;
INTEGER 	    Style;
LPSTR		    lpDeviceType;
LPSTR		    lpOutputFile;
LPSTR		    lpData;
{
    INTEGER		    DeviceNumber;
    REGISTER INTEGER	    Result;

    if((DeviceNumber = FindIndex(lpDeviceType, Devices, NUMDEVICES)) < 0){
	return(0);
    }

    LockData();

    if(lpData){
	if(lstrcmp((LPSTR) lpDeviceType, (LPSTR)lpData))
	    lpData = 0;
    }


    Result = (*(EnableFuncs[DeviceNumber]))(lpPDeviceHeader,
					    Style,
					    lpDeviceType,
					    lpOutputFile,
					    lpData,
					    DeviceNumber);

    UnlockData();

    return(Result);
}



INTEGER FAR PASCAL
EnumDFonts(lpPDeviceHeader, lpFacename, lpCallbackFunc, lpData)
LPPDEVICEHEADER     lpPDeviceHeader;
LPSTR		    lpFacename;
FARPROC 	    lpCallbackFunc;
LPSTR		    lpData;
{
    REGISTER INTEGER	Result;

    if(!(lpPDeviceHeader->Type)){	/* It's a memory bitmap. */
	return(0);
    }

    LockData();

    Result = (*(EnumDFontsFuncs[lpPDeviceHeader->DeviceNumber]))(lpPDeviceHeader,
								 lpFacename,
								 lpCallbackFunc,
								 lpData);

    UnlockData();

    return(Result);
}



INTEGER FAR PASCAL
EnumObj(lpPDeviceHeader, Style, lpCallbackFunc, lpData)
LPPDEVICEHEADER     lpPDeviceHeader;
INTEGER 	    Style;
FARPROC 	    lpCallbackFunc;
LPSTR		    lpData;
{
    REGISTER INTEGER	Result;

    if(!(lpPDeviceHeader->Type)){	/* It's a memory bitmap. */
	return(0);
    }

    LockData();

    Result = (*(EnumObjFuncs[lpPDeviceHeader->DeviceNumber]))(lpPDeviceHeader,
							      Style,
							      lpCallbackFunc,
							      lpData);

    UnlockData();

    return(Result);
}



INTEGER FAR PASCAL
Output(lpPDeviceHeader, Style, Count, lpPoints, lpPPen, lpPBrush, lpDrawMode, lpClipRect)
LPPDEVICEHEADER     lpPDeviceHeader;
INTEGER 	    Style;
INTEGER 	    Count;
LPPOINT 	    lpPoints;
LPSTR		    lpPPen;
LPSTR		    lpPBrush;
DRAWMODE	    FAR *lpDrawMode;
LPRECT		    lpClipRect;
{

    if(!lpPDeviceHeader->ChannelNumber)
	return(0L);

    if(!(lpPDeviceHeader->Type)){	/* It's a memory bitmap. */

	return;
    }

    LockData();

    CheckOutputState(lpPDeviceHeader);

    if(lpPDeviceHeader->hDC){
	if(!QueryAbort(lpPDeviceHeader->hDC, 0)){
	    (*(ControlFuncs[lpPDeviceHeader->DeviceNumber]))(lpPDeviceHeader,
							     ABORTDOC,
							     0L,
							     0L);
	    UnlockData();
	    return;
	}
    }

    (*(OutputFuncs[lpPDeviceHeader->DeviceNumber]))(lpPDeviceHeader,
						    Style,
						    Count,
						    lpPoints,
						    lpPPen,
						    lpPBrush,
						    lpDrawMode,
						    lpClipRect);

    UnlockData();
}



DWORD FAR PASCAL
Pixel(lpPDeviceHeader, X, Y, PhysColor, lpDrawMode)
LPPDEVICEHEADER     lpPDeviceHeader;
INTEGER 	    X;
INTEGER 	    Y;
DWORD		    PhysColor;
DRAWMODE	    FAR *lpDrawMode;
{
    REGISTER DWORD	Result;

    if(!(lpPDeviceHeader->Type)){	/* It's a memory bitmap. */
	return(0L);
    }

    LockData();

    if(lpDrawMode){

	CheckOutputState(lpPDeviceHeader);

	if(lpPDeviceHeader->hDC){
	    if(!QueryAbort(lpPDeviceHeader->hDC, 0)){
		(*(ControlFuncs[lpPDeviceHeader->DeviceNumber]))(lpPDeviceHeader,
								 ABORTDOC,
								 0L,
								 0L);
		UnlockData();
		return(0L);
	    }
	}
    }

    Result = (*(PixelFuncs[lpPDeviceHeader->DeviceNumber]))(lpPDeviceHeader,
							    X,
							    Y,
							    PhysColor,
							    lpDrawMode);

    UnlockData();
    return(Result);
}



INTEGER FAR PASCAL
RealizeObject(lpPDeviceHeader, Style, lpInObj, lpOutObj, lpTextXForm)
LPPDEVICEHEADER     lpPDeviceHeader;
INTEGER 	    Style;
LPSTR		    lpInObj;
LPSTR		    lpOutObj;
TEXTXFORM	    FAR *lpTextXForm;
{
    REGISTER INTEGER	Result;

    if(!(lpPDeviceHeader->Type)){   /* It's a memory bitmap. */
	return(0);
    }

    LockData();

    Result = (*(RealizeObjectFuncs[lpPDeviceHeader->DeviceNumber]))(lpPDeviceHeader,
								    Style,
								    lpInObj,
								    lpOutObj,
								    lpTextXForm);

    UnlockData();
    return(Result);
}



DWORD FAR PASCAL
StrBlt(lpPDeviceHeader, DestxOrg, DestyOrg, lpClipRect, lpString, Count, lpFont, lpDrawMode, lpTextXForm)
LPPDEVICEHEADER     lpPDeviceHeader;
INTEGER 	    DestxOrg;
INTEGER 	    DestyOrg;
LPRECT		    lpClipRect;
LPSTR		    lpString;
INTEGER 	    Count;
FONTINFO	    FAR *lpFont;
DRAWMODE	    FAR *lpDrawMode;
TEXTXFORM	    FAR *lpTextXForm;
{
    REGISTER DWORD	Result;

    if(!lpPDeviceHeader->ChannelNumber)
	return(OEM_FAILED);

    if(!(lpPDeviceHeader->Type)){   /* It's a memory bitmap. */
	return(0L);
    }

    LockData();

    if(Count > 0){

	CheckOutputState(lpPDeviceHeader);

	if(lpPDeviceHeader->hDC){
	    if(!QueryAbort(lpPDeviceHeader->hDC, 0)){
		(*(ControlFuncs[lpPDeviceHeader->DeviceNumber]))(lpPDeviceHeader,
								 ABORTDOC,
								 0L,
								 0L);
		UnlockData();
		return(0L);
	    }
	}
    }

    Result = (*(StrBltFuncs[lpPDeviceHeader->DeviceNumber]))(lpPDeviceHeader,
							     DestxOrg,
							     DestyOrg,
							     lpClipRect,
							     lpString,
							     Count,
							     lpFont,
							     lpDrawMode,
							     lpTextXForm);

    UnlockData();

    if(!lpPDeviceHeader->ChannelNumber)
	return(OEM_FAILED);

    return(Result);
}



INTEGER FAR PASCAL
ScanLR(lpPDeviceHeader, X, Y, PhysColor, Style)
LPPDEVICEHEADER     lpPDeviceHeader;
INTEGER 	    X;
INTEGER 	    Y;
DWORD		    PhysColor;
INTEGER 	    Style;
{
    REGISTER INTEGER	Result;

    if(!(lpPDeviceHeader->Type)){	/* It's a memory bitmap. */
	return(0);
    }

    LockData();

    Result = (*(ScanLRFuncs[lpPDeviceHeader->DeviceNumber]))(lpPDeviceHeader,
							     X,
							     Y,
							     PhysColor,
							     Style);

    UnlockData();
    return(Result);
}



INTEGER FAR PASCAL
DeviceMode(hWnd, hInstance, lpDeviceType, lpOutputFile)
HANDLE		    hWnd;
HANDLE		    hInstance;
LPSTR		    lpDeviceType;
LPSTR		    lpOutputFile;
{
    INTEGER		DeviceNumber;
    REGISTER INTEGER	Result;
    REGISTER INTEGER	fn;
    char		FileBuffer[128];

    LockData();

    Result = 0;

    if((DeviceNumber = FindIndex(lpDeviceType, Devices, NUMDEVICES)) < 0){
	MessageBox(NULL,
		   (LPSTR)"Invalid device type.",
		   (LPSTR)"HP747XA.DRV",
		   MB_OK|MB_NOFOCUS);

	goto Exit;
    }

    StripColon(lpOutputFile, (LPSTR)FileBuffer);

    if((fn = _lopen((LPSTR)FileBuffer, READ)) < 0){
	if((fn = _lcreat((LPSTR)FileBuffer, READ)) < 0){
		MessageBox( NULL,
			    (LPSTR)"Invalid output file.",
			    Devices[FindIndex(lpDeviceType, Devices, NUMDEVICES)],
			    MB_OK|MB_NOFOCUS
			   );
		goto Exit;
	    }
	    else {
		_ldelete((LPSTR)FileBuffer);
	    }
    }
    _lclose(fn);


    Result = (*(DeviceModeFuncs[DeviceNumber]))(hWnd,
						hInstance,
						lpDeviceType,
						(LPSTR)FileBuffer);

    Exit:

    UnlockData();
    return(Result);
}
