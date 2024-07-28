page            ,132
title           Block Move Support for the IBM 8514
.286c


.xlist
include         CMACROS.INC
include 	8514.INC
include 	gdidefs.inc
.list                  
;
;
subttl          Block Move Process
page +
sBegin          Code
assumes         cs,Code
assumes         ds,Data
;
;
externNP        BBExcludeCursor         ;in BOARDBLT.ASM
;
;
cProc           BlockMove,<NEAR,PUBLIC>
;
        include BOARDBLT.INC            ;frame definitions
;
cBegin          <nogen>
;
;Entry:
;       AL contains desired function to draw in.
;
mov             si,ax                   ;save our function
mov             bx,xExt                 ;get the width into BX
dec             bx                      ;decrement it as per manual
js              SBMLeave                ;if nothing to BLT, get out
mov             cx,yExt                 ;get the height into CX
dec             cx                      ;decrement it as per manual
jns             SBMSetFunction          ;if not 0, continue
;
SBMLeave:
jmp             SBMExit                 ;
;
public  SBMSetFunction
SBMSetFunction:    
CheckFIFOSpace THREE_WORDS
mov             ax,si                   ;restore saved function
or              ax,60h                  ;set the foreground
mov             dx,FUNCTION_0_PORT      ;set the background
out             dx,ax                   ;
mov             dx,FUNCTION_1_PORT      ;(we're always opaque)
out             dx,ax                   ;
;
;Set up the plane enable appropriately:
;
mov             ax,0ffh                 ;set to read all planes at once
mov             dx,READ_ENABLE_PORT     ;
out             dx,ax                   ;
;
;Now go exclude the cursor from the read area:
;
or              BoardBltFlags,ReadFlag  ;we must exclude the cursor from the
                                        ;area that we're reading from!
call            BBExcludeCursor         ;go get rid of it from read area
;
public  SBMSetMode
SBMSetMode:
;
;Now, set the mode to "block move pattern":
;                     
MakeEmptyFIFO
mov             ax,0a000h               ;
mov             dx,MODE_PORT            ;
out             dx,ax                   ;
;
public  SBMSetExtents
SBMSetExtents:
;
;At this point:
;       BX has X-extent.
;       CX has Y-extent.
;
mov             ax,bx                   ;get the X-extent
mov             dx,RECT_WIDTH_PORT      ;set the width of the character on board
out             dx,ax                   ;
mov             ax,cx                   ;get the Y-extent
mov             dx,RECT_HEIGHT_PORT     ;set it on the board
out             dx,ax                   ;
;
public  SBMDetermineDirection
SBMDetermineDirection:
;
;Now we construct the block move command taking into account the direction of
;the move:
;                           
mov             si,0c0f3h               ;start with a default command
mov             dx,SrcxOrg              ;get the source X-coordinate
mov             di,DstxOrg              ;get the destination X-coordinate
cmp             dx,di                   ;do we want to increment X as we move?
jge             SBMSetStartingX         ;yes! we're set up OK
and             si,0ffdfh               ;otherwise, subtract out the 2's bit
add             dx,bx                   ;and make our starting X the right side
add             di,bx                   ;
;                 
public  SBMSetStartingX
SBMSetStartingX:
mov             ax,dx                   ;set starting source X
and             ax,7ffh                 ;
mov             dx,Srcx_PORT            ;
out             dx,ax                   ;
mov             ax,di                   ;set starting destination X
and             ax,7ffh                 ;
mov             dx,Dstx_PORT            ;
out             dx,ax                   ;
mov             dx,SrcyOrg              ;get the source Y-coordinate
mov             di,DstyOrg              ;get the destination Y-coordinate
cmp             dx,di                   ;do we want to increment Y as we move?
jge             SBMSetStartingY         ;yes! we're set up OK
and             si,0ff7fh               ;otherwise, subtract out 2's & 4's bit
add             dx,cx                   ;and make our starting Y the bottom
add             di,cx                   ;
;                 
public  SBMSetStartingY
SBMSetStartingY:
mov             ax,dx                   ;set starting source Y
and             ax,7ffh                 ;
mov             dx,Srcy_PORT            ;
out             dx,ax                   ;
mov             ax,di                   ;set starting destination Y
and             ax,7ffh                 ;
mov             dx,Dsty_PORT            ;
out             dx,ax                   ;
;
;Now, send the command:
;                      
mov             ax,si                   ;get command into AX
mov             dx,COMMAND_FLAG_PORT    ;
out             dx,ax                   ;
;
SBMExit:
ret                                     ;and return to caller
cEnd            <nogen>
;
;
sEnd            Code
end
