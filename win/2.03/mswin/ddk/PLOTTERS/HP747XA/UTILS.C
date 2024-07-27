
/* Various utilities. */

#include    "plotters.h"

#define islower(ch)	((ch) >= 'a' && (ch) <= 'z')
#define toupper(ch)	((ch) + ('A' - 'a'))



BOOL FAR PASCAL
StripColon(lpString, lpBuffer)
LPSTR		    lpString;
LPSTR		    lpBuffer;
{
    REGISTER LPSTR  lpS;
    REGISTER LPSTR  lpB;

    lpS = lpString;
    lpB = lpBuffer;

    if((!lpS) || (!*lpS)){
	*lpB++ = 'N';
	*lpB++ = 'U';
	*lpB++ = 'L';
	*lpB = 0;
	return;
    }

    while(*lpB++ = *lpS++) ;

    lpB -= 2;

    if(*lpB == ':') *lpB = 0;

}



BOOL FAR PASCAL
EqualNames(lpstr1, lpstr2)
LPSTR		    lpstr1;
LPSTR		    lpstr2;
{
    REGISTER char   chTmp1;
    REGISTER char   chTmp2;

    while(TRUE){
	chTmp1 = *(lpstr1++);
	chTmp2 = *(lpstr2++);

	if(islower(chTmp1))    chTmp1 = toupper(chTmp1);
	if(islower(chTmp2))    chTmp2 = toupper(chTmp2);

	if(chTmp1 != chTmp2) return(FALSE);

	if(chTmp1 == NULL) return(TRUE);
    }
}



INTEGER FAR PASCAL
FindIndex(lpName, lpNames, Limit)
LPSTR		    lpName;
LPLPSTR 	    lpNames;
INTEGER 	    Limit;
{
    INTEGER	Count;

    for(Count = 0; Count < Limit; Count++){
	if(EqualNames(lpName, *lpNames)) return(Count);
	lpNames++;
    }

    return(-1);
}



/* IntToAscii */
/* Format a number into the buffer and return its length.
   Returns -1 on error. */

INTEGER FAR PASCAL
IntToAscii(Number, lpBuffer, BufferLength)
INTEGER 	    Number;
LPSTR		    lpBuffer;
INTEGER 	    BufferLength;
{
    char		Buffer[6];
    INTEGER		Index, Count;
    WORD		Remainder, Shifted;
    BOOL		Negative;
    LPSTR		Ptr;

    if(BufferLength <= 0) return(-1);

    if(Number < 0){
	Negative  = TRUE;
	Number = -Number;
    }
    else{
	Negative = FALSE;
    }

    Index = 5;
    Buffer[5] = '0';
    Remainder = Number;
    while(Remainder){
	Shifted = Remainder/10;
	Buffer[Index--] = Remainder-(Shifted*10)+'0';
	Remainder = Shifted;
    }
    if(Number) Index++;

    Count = 6-Index;

    if(Negative){
	if((Count+1) <= BufferLength){
	    (*(lpBuffer++)) = '-';
	    BufferLength--;
	}
	else{
	    return(-1);
	}
    }

    if(Count <= BufferLength){
	Ptr = (LPSTR)Buffer + Index;
	Index = Count;
	while(Index--){
	    (*(lpBuffer++)) = (*(Ptr++));
	}
	return(Negative ? Count+1 : Count);
    }
    else{
	return(-1);
    }
}



INTEGER FAR PASCAL
OpenChannel(lpPDeviceHeader, lpChannel, lpDocName)
LPPDEVICEHEADER     lpPDeviceHeader;
LPSTR		    lpChannel;
LPSTR		    lpDocName;
{
#ifdef SPOOLING
    return(lpPDeviceHeader->ChannelNumber = ((lpPDeviceHeader->Spooling) ?
					      OpenJob(lpChannel,
						      lpDocName,
						      lpPDeviceHeader->hDC) :
					      _lcreat(lpChannel, 0)));
#else
    return(lpPDeviceHeader->ChannelNumber = OpenJob(lpChannel, lpDocName, lpPDeviceHeader->hDC));
#endif
}



INTEGER FAR PASCAL
SendString(lpPDeviceHeader, lpBuffer, Count)
LPPDEVICEHEADER     lpPDeviceHeader;
LPSTR		    lpBuffer;
INTEGER 	    Count;
{
    LPSTR		Ptr;
#if 0
    /* error has occured, cannot write anymore (linsh 10/15) */
    if(!lpPDeviceHeader->ChannelNumber)
	    return -1;
#endif
    if(((lpPDeviceHeader->Buffer.Count) + Count) > (lpPDeviceHeader->Buffer.Length)){

	FlushBuffer(lpPDeviceHeader);

	if(Count >= (lpPDeviceHeader->Buffer.Length)){
#ifdef SPOOLING
	    if(lpPDeviceHeader->Spooling){
#endif
		if(lpPDeviceHeader->ChannelNumber != 0){

		    if(WriteSpool(lpPDeviceHeader->ChannelNumber, lpBuffer, Count) < 0){

			DeleteJob(lpPDeviceHeader->ChannelNumber , 0);
			lpPDeviceHeader->ChannelNumber = 0;
		    }
		}
#ifdef SPOOLING
	    }
	    else{
		_lwrite(lpPDeviceHeader->ChannelNumber, lpBuffer, Count);
	    }
#endif
	    return;
	}
    }

    Ptr = (lpPDeviceHeader->Buffer.Buffer) + (lpPDeviceHeader->Buffer.Count);

    (lpPDeviceHeader->Buffer.Count) += Count;

    while(Count--){
	(*(Ptr++)) = (*(lpBuffer++));
    }
}



INTEGER FAR PASCAL
FlushBuffer(lpPDeviceHeader)
LPPDEVICEHEADER     lpPDeviceHeader;
{
#ifdef SPOOLING
    if(lpPDeviceHeader->Spooling){
#endif
	if(lpPDeviceHeader->ChannelNumber == 0){
	    lpPDeviceHeader->Buffer.Count = 0;
	    return;
	}
	if(WriteSpool(lpPDeviceHeader->ChannelNumber,
		       lpPDeviceHeader->Buffer.Buffer,
		       lpPDeviceHeader->Buffer.Count)
					  < 0){
	    DeleteJob(lpPDeviceHeader->ChannelNumber , 0);
	    lpPDeviceHeader->ChannelNumber = 0;
	}
	lpPDeviceHeader->Buffer.Count = 0;
#ifdef SPOOLING
    }
    else{
	_lwrite(lpPDeviceHeader->ChannelNumber,
		lpPDeviceHeader->Buffer.Buffer,
		lpPDeviceHeader->Buffer.Count);
	lpPDeviceHeader->Buffer.Count = 0;
    }
#endif
}



INTEGER FAR PASCAL
CloseChannel(lpPDeviceHeader)
LPPDEVICEHEADER     lpPDeviceHeader;
{
#ifdef SPOOLING
    if(lpPDeviceHeader->Spooling)
#endif
	CloseJob(lpPDeviceHeader->ChannelNumber);
#ifdef SPOOLING
    else
	_lclose(lpPDeviceHeader->ChannelNumber);
#endif

    lpPDeviceHeader->ChannelNumber = -1;
}



INTEGER FAR PASCAL
ColorDistance(RGBColor1, RGBColor2)
DWORD		    RGBColor1;
DWORD		    RGBColor2;
{
    HLSBLOCK		HLSColor1;
    HLSBLOCK		HLSColor2;

    RGB_to_HLS(RGBColor1, (LPHLSBLOCK)&HLSColor1);
    ConstrainHLS((LPHLSBLOCK)&HLSColor1);

    RGB_to_HLS(RGBColor2, (LPHLSBLOCK)&HLSColor2);
    ConstrainHLS((LPHLSBLOCK)&HLSColor2);

    return(Abs((HLSColor1.Hue) - (HLSColor2.Hue)) |
		((Abs((HLSColor1.Lightness) - (HLSColor2.Lightness))/500) << 13));
}



INTEGER FAR PASCAL
ConstrainHLS(lpHLS)
LPHLSBLOCK	    lpHLS;
{
    /* Constrain an HLS color to certain regions of the color wheel.
       If the color is grey, we force it to white or black.
       If not, we force it out to the rim of the wheel. */

    if(lpHLS->Saturation < 500){
	/* It's grey. */
	if(lpHLS->Lightness < 500){
	    lpHLS->Lightness = 0;
	}
	else{
	    lpHLS->Lightness = 1000;
	}
	lpHLS->Saturation = 0;
	lpHLS->Hue = 0;
    }
    else{
	/* Force it out to the rim. */
	lpHLS->Lightness = 500;
	lpHLS->Saturation = 1000;
    }
}



INTEGER FAR PASCAL
RGB_to_HLS(Color, lpHLSResult)
DWORD		    Color;
LPHLSBLOCK	    lpHLSResult;
{
    INTEGER	Maximum;
    INTEGER	Minimum;
    INTEGER	Difference;
    INTEGER	Sum;
    INTEGER	Red;
    INTEGER	Green;
    INTEGER	Blue;
    INTEGER	red;
    INTEGER	green;
    INTEGER	blue;

    /* Map a a color from the RGB to the HLS color space.
       This algorithm is from appendix B of the SIGGRAPH CORE spec.

       NOTE: We keep all fractionals in the range 0:1000. */

    Red   =	  (Color&0x000000FF)*1000L/255L;
    Green =  ((Color>>8)&0x000000FF)*1000L/255L;
    Blue  = ((Color>>16)&0x000000FF)*1000L/255L;

    if(Red > Green){
	if(Red > Blue) Maximum = Red;
	    else Maximum = Blue;
	if(Green < Blue) Minimum = Green;
	    else Minimum = Blue;
    }
    else{
	if(Red < Blue) Minimum = Red;
	    else Minimum = Blue;
	if(Green > Blue) Maximum = Green;
	    else Maximum = Blue;
    }

    if(Difference = (Maximum - Minimum)){
	red   = 1000L*(Maximum -   Red) / Difference;
	green = 1000L*(Maximum - Green) / Difference;
	blue  = 1000L*(Maximum -  Blue) / Difference;
    }

    Sum = (Maximum + Minimum);

    (lpHLSResult->Lightness) = Sum/2;

    if(!Difference){
	    (lpHLSResult->Saturation) = 0;
    }
    else if((lpHLSResult->Lightness) <= 500){
	(lpHLSResult->Saturation) = 1000L*Difference / Sum;
    }
    else{
	(lpHLSResult->Saturation) = 1000L*Difference / (2000 - Sum);
    }

    if(!(lpHLSResult->Saturation)){
	(lpHLSResult->Hue) = 0;
    }
    else if(Red == Maximum){
	(lpHLSResult->Hue) = 2000+blue-green;
    }
    else if(Green == Maximum){
	(lpHLSResult->Hue) = 4000+red-blue;
    }
    else{
	(lpHLSResult->Hue) = 6000+green-red;
    }

    /* Convert Hue to degrees. */
    (lpHLSResult->Hue) = ((lpHLSResult->Hue)*6L)/100;

    while((lpHLSResult->Hue) < 0){
	(lpHLSResult->Hue) += 360;
    }

    while((lpHLSResult->Hue) >= 360){
	(lpHLSResult->Hue) -= 360;
    }
}



INTEGER FAR PASCAL
RotateGDIInfo(pGDIInfo)
PGDIINFO	    pGDIInfo;
{
    REGISTER short	Temp;

    Temp = pGDIInfo->dpHorzSize;
    pGDIInfo->dpHorzSize = pGDIInfo->dpVertSize;
    pGDIInfo->dpVertSize = Temp;

    Temp = pGDIInfo->dpHorzRes;
    pGDIInfo->dpHorzRes = pGDIInfo->dpVertRes;
    pGDIInfo->dpVertRes = Temp;
}



INTEGER FAR PASCAL
ScaleGDIInfo(pGDIInfo)
PGDIINFO	    pGDIInfo;
{

    /* Metric  Lo res WinX,WinY. */
    /* Metric  Lo res VptX,VptY. */
    NormalizeGDIInfoPoint((long)pGDIInfo->dpHorzSize*10L,
			  (long)pGDIInfo->dpVertSize*10L,
			  (PPOINT)&(pGDIInfo->dpMLoWin),
			  (long)pGDIInfo->dpHorzRes,
			  -(long)pGDIInfo->dpVertRes,
			  (PPOINT)&(pGDIInfo->dpMLoVpt));

    /* Metric  Hi res WinX,WinY. */
    /* Metric  Hi res VptX,VptY. */
    NormalizeGDIInfoPoint((long)pGDIInfo->dpHorzSize*100L,
			  (long)pGDIInfo->dpVertSize*100L,
			  (PPOINT)&(pGDIInfo->dpMHiWin),
			  (long)pGDIInfo->dpHorzRes,
			  -(long)pGDIInfo->dpVertRes,
			  (PPOINT)&(pGDIInfo->dpMHiVpt));

    /* English Lo res WinX,WinY. */
    /* English Lo res VptX,VptY. */
    NormalizeGDIInfoPoint((long)pGDIInfo->dpHorzSize*1000L,
			  (long)pGDIInfo->dpVertSize*1000L,
			  (PPOINT)&(pGDIInfo->dpELoWin),
			  (long)pGDIInfo->dpHorzRes*254L,
			  -(long)pGDIInfo->dpVertRes*254L,
			  (PPOINT)&(pGDIInfo->dpELoVpt));

    /* English Hi res WinX,WinY. */
    /* English Hi res VptX,VptY. */
    NormalizeGDIInfoPoint((long)pGDIInfo->dpHorzSize*10000L,
			  (long)pGDIInfo->dpVertSize*10000L,
			  (PPOINT)&(pGDIInfo->dpEHiWin),
			  (long)pGDIInfo->dpHorzRes*254L,
			  -(long)pGDIInfo->dpVertRes*254L,
			  (PPOINT)&(pGDIInfo->dpEHiVpt));

    /* Twips	      WinX,WinY. */
    /* Twips	      VptX,VptY. */
    NormalizeGDIInfoPoint((long)pGDIInfo->dpHorzSize*14400L,
			  (long)pGDIInfo->dpVertSize*14400L,
			  (PPOINT)&(pGDIInfo->dpTwpWin),
			  (long)pGDIInfo->dpHorzRes*254L,
			  -(long)pGDIInfo->dpVertRes*254L,
			  (PPOINT)&(pGDIInfo->dpTwpVpt));
}



INTEGER FAR PASCAL
NormalizeGDIInfoPoint(LX1, LY1, pPoint1, LX2, LY2, pPoint2)
long		    LX1;
long		    LY1;
PPOINT		    pPoint1;
long		    LX2;
long		    LY2;
PPOINT		    pPoint2;
{

    while((Abs(LX1) > 32767L) || (Abs(LX2) > 32767L)){
	LX1 /= 2;
	LX2 /= 2;
    }

    while((Abs(LY1) > 32767L) || (Abs(LY2) > 32767L)){
	LY1 /= 2;
	LY2 /= 2;
    }

    pPoint1->xcoord = (INTEGER)LX1;
    pPoint1->ycoord = (INTEGER)LY1;

    pPoint2->xcoord = (INTEGER)LX2;
    pPoint2->ycoord = (INTEGER)LY2;
}



INTEGER FAR PASCAL
PlotterMessageBox(lpPDeviceHeader, hWnd, lpText, lpCaption, Type)
LPPDEVICEHEADER     lpPDeviceHeader;
HWND		    hWnd;
LPSTR		    lpText;
LPSTR		    lpCaption;
WORD		    Type;

{
    char		Buffer[128];
    LPSTR		Src_Ptr, Dst_Ptr;
    short		Count;

    FlushBuffer(lpPDeviceHeader);

#ifdef SPOOLING
    if(lpPDeviceHeader->Spooling){
#endif
	Count = 0;
	Dst_Ptr = (LPSTR)Buffer;
	Src_Ptr = lpCaption;

	while(*Dst_Ptr++ = *Src_Ptr++) Count++;

	*(Dst_Ptr-1) = ':';
	*Dst_Ptr++ = ' ';
	Count += 2;

	Src_Ptr = lpText;
	while(*Dst_Ptr++ = *Src_Ptr++) Count++;

	if(WriteDialog(lpPDeviceHeader->ChannelNumber, (LPSTR)Buffer, Count)
					  < 0) lpPDeviceHeader->ChannelNumber = 0;
#ifdef SPOOLING
    }
    else{
	MessageBox(hWnd, lpText, lpCaption, Type);
    }
#endif
}



INTEGER FAR PASCAL
CheckOutputState(lpPDeviceHeader)
LPPDEVICEHEADER     lpPDeviceHeader;
{
#if 0
    if(lpPDeviceHeader->ChannelNumber == -1){
	/* Do an implicit STARTDOC if the user didn't. */
	(*(ControlFuncs[lpPDeviceHeader->DeviceNumber]))(lpPDeviceHeader,
							 STARTDOC,
							 (LPSTR)"HP 747XA",
							 0L);
    }
#endif


    if(!lpPDeviceHeader->OutputInit){

	lpPDeviceHeader->OutputInit = TRUE;

#ifdef SPOOLING
	if(lpPDeviceHeader->Spooling){
#endif
	    if(StartSpoolPage(lpPDeviceHeader->ChannelNumber) == -1){
		lpPDeviceHeader->ChannelNumber = 0;
	    }
#ifdef SPOOLING
	}
#endif
	/* Ask the user to put in new sheet of paper. */
	PlotterMessageBox(lpPDeviceHeader,
			  NULL,
			  (LPSTR)LoadPaperMess,
			  Devices[lpPDeviceHeader->DeviceNumber],
			  MB_OK|MB_NOFOCUS);
    }
}
