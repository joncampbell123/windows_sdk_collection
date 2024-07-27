;/*
; Include file for all routines involved in vector text.

; If INTERPOLATEDANGLES is defined, the sine tables will be stored in degrees
; rather than in 10'ths of a degree and intermediate angles will be interpolated.
INTERPOLATEDANGLES  equ     1

    if	0
;*/

/* If INTERPOLATEDANGLES is defined, the sine tables will be stored in degrees
   rather than in 10'ths of a degree and intermediate angles will be interpolated. */
#define INTERPOLATEDANGLES


/* Enable vector text. */
#define VECTORSIMULATIONS
/* #undef  VECTORSIMULATIONS /* */



/* The size of the buffer for gathering up MoveTo/LineTo's into polylines. */
#define LNBUFSZ     16



#define GetNextStroke(SPtr) \
	( WideFormat ? *((WORD FAR *)SPtr)++ : *SPtr++ )

#define CharWidth(AChar) \
	(VariablePitch ? \
	(*(((WORD FAR *)(PFont_Ptr->dfMaps))+(AChar*2)+1)) : \
	(PFont_Ptr->dfAvgWidth))


#define RotatedX(XX, YY, Theta)\
    ((Theta) ?\
     (RSin((YY), (Theta)) + RCos((XX), (Theta))) :\
     (XX))

#define RotatedY(XX, YY, Theta)\
    ((Theta) ?\
     (RCos((YY), (Theta)) - RSin((XX), (Theta))) :\
     (YY))


/* Vector font. */
/* Not device specific. */
#define SIMVECTFONT1(xPFont_Ptr)\
    ((((xPFont_Ptr)->dfType&3) == PF_VECTOR_TYPE) &&\
     (!(xPFont_Ptr)->dfDevice))

/* Device can't do vector fonts. */
/* OR the device can't do vector fonts at the precision we want. */
#define SIMVECTFONT2(xdpText, xlfOutPrecision)\
    ((!((xdpText)&TC_VA_ABLE)) ||\
     (((xlfOutPrecision) == OUT_STROKE_PRECIS) &&\
      (!((xdpText)&TC_OP_STROKE))))


#define MoveTo(HNDC, XX, YY)\
    LBMoveTo(HNDC, (XX), (YY), (LPPOINT)LnBuf, lpLnPts)

#define LineTo(HNDC, XX, YY)\
    LBLineTo(HNDC, (XX), (YY), (LPPOINT)LnBuf, lpLnPts)

#define LBFLUSHBUF(HNDC, BUFFER, COUNT)\
    ; if ((COUNT) >= 2) {\
	farLPtoSP((HNDC), (BUFFER), (COUNT));\
	PolyLineGutsAlt((HNDC), (BUFFER), (COUNT));\
	}\
    (COUNT) = 0;



BOOL	  FAR  PASCAL VectTextOutPut();
DWORD	  FAR  PASCAL VectTextExtent();

INTEGER FAR  PASCAL RCos();
INTEGER FAR  PASCAL RSin();

BOOL	NEAR PASCAL NextCharOrg();
BOOL	NEAR PASCAL BackgroundVectChar();
BOOL	NEAR PASCAL PlaceVectChar();
BOOL	NEAR PASCAL VectorRound();
INTEGER NEAR PASCAL NormalizeAngle();
BOOL	NEAR PASCAL CalcBoundingPoly();
BOOL	NEAR PASCAL RotBoundingRect();
INTEGER NEAR PASCAL ConstrainOrientation();
INTEGER NEAR PASCAL LBMoveTo();
INTEGER NEAR PASCAL LBLineTo();

;/*
    endif
;*/
