/* Various HP utilities. */

#include    "plotters.h"



/* Send a command to an HP plotter in 9872 format. */
/* Command is assumed to be a string of 2 characters.
   Parameters is an integer array containing the arguments.
   NumParameters is the number of entries in Parameters.
   We implicitly assume that BUFFERLENGTH > 10.
   */

#define NUMLOCPARAMS	16



INTEGER FAR PASCAL
HPSendCommand(lpPDeviceHeader, Command, Parameters, NumParameters, Inversion, Multiplier)
LPPDEVICEHEADER     lpPDeviceHeader;
LPSTR		    Command;
LPINT		    Parameters;
INTEGER 	    NumParameters;
INTEGER 	    Inversion;
INTEGER 	    Multiplier;
{
    INTEGER		LocParameters[NUMLOCPARAMS];
    LPINT		LParameters;
    INTEGER		NumLParameters;
    LPSTR		Ptr;
    INTEGER		ParamNum;
    char		Buffer[BUFFERLENGTH];

    Ptr = (LPSTR)Buffer;

    /* Copy the command. */
    (*(Ptr++)) = (*(Command++));
    (*(Ptr++)) = *Command;

    ParamNum = 0;

    while(NumParameters > 0){

	NumLParameters = (NumParameters >= NUMLOCPARAMS ?
			  NUMLOCPARAMS :
			  NumParameters);

	/* Transform the points to landscape if we must. */
	HPRotatePoints(lpPDeviceHeader,
		       Parameters,
		       (LPINT)(LParameters = (LPINT)LocParameters),
		       NumLParameters,
		       Inversion,
		       Multiplier);

	/* Buffer the parameters. */
	while(NumLParameters-- > 0){
	    if(ParamNum){
		/* Make sure there is room for the ",", number, and ";". */
		if((BUFFERLENGTH - ((WORD)Ptr - (WORD)Buffer)) <
							(1+6+1)){
		    SendString(lpPDeviceHeader,
			       Buffer,
			       (WORD)Ptr - (WORD)Buffer);
		    Ptr = (LPSTR)Buffer;
		}
		(*(Ptr++)) = ',';
	    }

	    Ptr += IntToAscii(((Inversion && (ParamNum&1)) ?
			       Inversion - (*(LParameters++)) :
			       *(LParameters++)),
			      Ptr,
			      BUFFERLENGTH - ((WORD)Ptr - (WORD)Buffer));

	    ParamNum++;
	}

	Parameters += NUMLOCPARAMS;
	NumParameters -= NUMLOCPARAMS;
    }

    /* Terminate it. */
    (*(Ptr++)) = ';';

    /* Send it. */
    SendString(lpPDeviceHeader, Buffer, (WORD)Ptr - (WORD)Buffer);
}



INTEGER FAR PASCAL
HPChangePen(lpPDeviceHeader, NewPen, lpOldPen)
LPPDEVICEHEADER     lpPDeviceHeader;
INTEGER 	    NewPen;
LPINT		    lpOldPen;
{

    if(NewPen != *lpOldPen){

	*lpOldPen = NewPen;

	HPSendCommand(lpPDeviceHeader, (LPSTR)"SP", lpOldPen, 1, 0, 0);
    }
}



INTEGER FAR PASCAL
HPSetClipRect(lpPDeviceHeader, lpClipRect, Multiplier, Inversion)
LPPDEVICEHEADER     lpPDeviceHeader;
LPRECT		    lpClipRect;
INTEGER 	    Multiplier;
INTEGER 	    Inversion;
{
    INTEGER	Args[4];
    INTEGER	Temp;

    Args[0] = (lpClipRect->left);
    Args[1] = (lpClipRect->top);
    Args[2] = (lpClipRect->right);
    Args[3] = (lpClipRect->bottom);

    if(((lpPDeviceHeader->Orientation == LANDSCAPE) && (Args[0] > Args[2])) ||
	((lpPDeviceHeader->Orientation == PORTRAIT)  && (Args[0] < Args[2]))){
	Temp = Args[0];
	Args[0] = Args[2];
	Args[2] = Temp;
    }

    if(Args[1] < Args[3]){
	Temp = Args[1];
	Args[1] = Args[3];
	Args[3] = Temp;
    }

    Args[0] *= Multiplier;
    Args[1] *= Multiplier;
    Args[2] *= Multiplier;
    Args[3] *= Multiplier;

    HPSendCommand(lpPDeviceHeader, (LPSTR)"IW", (LPINT)Args, 4,
		  Inversion*Multiplier, Multiplier);
}



INTEGER FAR PASCAL
HPRestoreClipRect(lpPDeviceHeader, HorzRes, VertRes, Multiplier)
LPPDEVICEHEADER     lpPDeviceHeader;
INTEGER 	    HorzRes;
INTEGER 	    VertRes;
INTEGER 	    Multiplier;
{
    INTEGER	Args[4];

    Args[0] = 0;
    Args[1] = 0;
    Args[2] = HorzRes*Multiplier;
    Args[3] = VertRes*Multiplier;

    HPSendCommand(lpPDeviceHeader, (LPSTR)"IW", (LPINT)Args, 4, 0, 0);
}



INTEGER  FAR PASCAL
HPInsertTextPixels(lpPDeviceHeader, Pixels)
LPPDEVICEHEADER     lpPDeviceHeader;
INTEGER 	    Pixels;
{
    char		Buffer[6];

    /* Insert Pixels pixels while in text mode. */

    /* Take us out of text mode. */
    SendString(lpPDeviceHeader, (LPSTR)"\03;", 2);

    /* Move over. */
    SendString(lpPDeviceHeader, (LPSTR)"PR", 2);

    if(lpPDeviceHeader->Orientation == PORTRAIT){
	SendString(lpPDeviceHeader, (LPSTR)"0,-", 3);
    }

    SendString(lpPDeviceHeader,
	       (LPSTR)Buffer,
	       IntToAscii(Pixels, (LPSTR)Buffer, sizeof(Buffer)));

    if(lpPDeviceHeader->Orientation == LANDSCAPE){
	SendString(lpPDeviceHeader, (LPSTR)",0", 2);
    }

    /* Back into text mode. */
    SendString(lpPDeviceHeader, (LPSTR)";PA;LB", 6);
}



INTEGER  FAR PASCAL
HPRotatePoints(lpPDeviceHeader, lpInCoords, lpOutCoords, Count, Inversion, Multiplier)
LPPDEVICEHEADER     lpPDeviceHeader;
LPINT		    lpInCoords;
LPINT		    lpOutCoords;
INTEGER 	    Count;
INTEGER 	    Inversion;
INTEGER 	    Multiplier;
{

    if((lpPDeviceHeader->Orientation == PORTRAIT) && Inversion && !(Count&1)){
	while(Count > 0){

	    /* Rotate each point from portrait to landscape mode. */
	    *lpOutCoords++ = Multiplier * lpPDeviceHeader->HorzRes -
							  *(lpInCoords+1);
	    *lpOutCoords++ = *lpInCoords;

	    lpInCoords += 2;

	    Count -= 2;
	}
    }
    else {
	while(Count-- > 0){
	    *lpOutCoords++ = *lpInCoords++;
	}
    }
}
