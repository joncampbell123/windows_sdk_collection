;--------------------------------------------------------------------
;
; Include file that defines the structure of an old app partition
;

;-------------------------------------------------------
;
;  A partition is made up of 8 parts, divided on paragraph boundaries
;
;  1.  The partition information structure (1 paragraph)
;  2.  The area used to store screen snap shots when they are taken
;  3.  The area used to save the screen during a COPY operation
;  4.  The area used to strore the HPSystem saved state (Vectra Only)
;  5.  The area used to save SFT created by Windows (Only in extended mode)
;  6.  The area used for screen state involved in switching
;  7.  The area used to store old mouse driver state info
;  8.  The mini memory arena used by the old app
;
;  The HP buffer is divided into two equal parts:
;  a.  HPSeg: Saved area of the original HPSystem before the oldap is started
;  b.  HPSeg+HPPara: Saved area of HPsystem during a context switch
;
;
;   The variables
;
;	  BufSeg
;	  BufSize
;	  BufPara
;
; 	  CopySeg
;	  CopyPara
;
;         HPSeg
;	  HPPara
;
;         SFTSeg
;         SFTPara
;
;	  SwitchSeg
;	  SwitchSize
;	  SwitchPara
;
;	  MouseSeg
;	  MousePara
;
;  in the old app DS determine the various addresses of these pieces
;  (if present).
;

;-----------------------------------------------
;
;  A partition is swappable if all of the following are true:
;
;	1.  Modifies screen
;	2.  Doesn't modify keyboard, COM1, or COM2
;	3.  Doesn't prevent swapping
;
;  A program should look for a swap partition not in memory if it:
;
;	1.  Modifies the screen
;	2.  Doesn't modify memory
;	3.  Swap drive is valid
;

;-----------------------------------------------
;
; This is the partition structure of an old app partition
;
PartitionStruc	STRUC
prtOwner	dw	?	; DS of current owner, 0 = none
prtParent	dw	?	; segment of parent partition, 0 = none
prtfSwap	db	?	; indicates partition is swappable
prtfSwitch	db	?	; indicates we have a screen switch seg
prtfBuf 	db	?	; indicates we have a buf (screen grab) seg
prtfMouse	db	?	; indicates we have a mouse seg
prtCount	db	?	; Number of DS's sharing this partition
;
;!!  HP
;
prtfHP		db	?	; indicated we have a HP saved buffer
prtOrgOwner	dw	?	; For use by EEMS support only:
				;  = Instance DS of partition creator
				;  = 0 if Original creator was terminated
;******
PartitionStruc	ENDS		; prtCount is decremented when an app exits
				; if it decrements to zero the block is freed.
