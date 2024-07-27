;
;   SETENHKB.ASM
;
;   Sets the enhanced keyboard bit (10H) of location 40:96 telling Windows
;   that an Enhanced Keyboard is indeed present for all of the brain-damaged
;   no-enhanced-keyboard-support BIOSs out there (Tandy 3000, WYSE 286,
;   etc.).
;
;   Chip Anderson  10/22/87

setenhkb    segment

	    assume  cs:setenhkb

	    mov ax,0040h
	    mov di,0096h
	    mov es,ax
	    or	byte ptr es:[di],10h

	    int 20h

setenhkb    ends

	    end
