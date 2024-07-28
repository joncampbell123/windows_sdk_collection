        parmD   lpDstDev                ;--> to the destination
        parmW   Style                   ;Output operation
        parmW   Count                   ;# of points
        parmD   lpPoints                ;--> to a set of points
        parmD   lpPPen                  ;--> to physical pen
        parmD   lpPBrush                ;--> to physical brush
        parmD   lpDrawMode              ;--> to a Drawing mode
        parmD   lpClipRect              ;--> to a clipping rectangle if <> 0
;
        localD  lpLocPBrush
        localW  Rop2
        localW  DstxOrg
        localW  DstyOrg
        localW  xExt
        localW  yExt
        localW  LineError
        localW  LineIncrement
        localW  yCoordinate
        localW  X1
        localW  Y1
        localW  X2
        localW  Y2
        localW  deltaX
        localW  deltaY
        localW  PenPattern
        localW  HugeBitmapFlag
        localW  ScanlinesPerSegment
        localW  StartingBitmapSeg
        localW  BytesPerLine
        localW  RoutineAddress
        localB  RunningBitMask
        localB  PenColour
        localB  BrushStyle
        localB  BackgroundColour
	localB	ForegroundColour
	localB	BackgroundMono
	localB	ForegroundMono
        localB  OpaqueFlag
        localB  BandFlags
        localB  OutlinePlane
	localB	ColourFormat
	localB	NibbleMask
        localV  RectPoints,20
        localV  LocalPBrush,10
