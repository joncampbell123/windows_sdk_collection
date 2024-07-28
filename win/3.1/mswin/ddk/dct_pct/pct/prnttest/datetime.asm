;-----------------------------------------------------------------------------;
; DATE/TIME ROUTINES                                                          :
;   This module contains the assembly routines for retrieving the date and/or :
;   time from MS-DOS.  These routines are declared FAR so that they may be    :
;   called from other segments.                                               :
;                                                                             :
; Copyright 1990-1992 by Microsoft Corporation                               :
; SEGMENT: _TEXT                                                              :
;                                                                             :
; HISTORY: Apr 07, 1989 - created.                                            :
;                                                                             :
;-----------------------------------------------------------------------------;

MemM EQU 1
?PLM=1
?WIN=1

INCLUDE cmacros.inc

DATETIME struc
     bDay          db 0
     bMonth        db 0
     wYear         dw 0
     bDayWeek      db 0
     bHours        db 0
     bMinutes      db 0
     bSeconds      db 0
     bHundreths    db 0
DATETIME ends

?PLM=1
?WIN=1

assumes cs,CODE

sBegin  CODE
;-----------------------------------------------------------------------------;
; GET SYSTEM TIME                                                             ;
;   This routine retrieves the time from the MS-DOS function call INT 21/2C.  ;
;                                                                             ;
;   void FAR GetTime(int FAR*, int FAR*, int FAR*);                           ;
;   GetTime(&pHours,&pMinutes,&pSeconds);                                     ;
;                                                                             :
;      AFFECTS                                                                ;
;        (1) The location pointed to by pHours                                ;
;        (2) The location pointed to by pMinutes                              ;
;        (3) The location pointed to by pSeconds                              ;
;                                                                             ;
;   Int 21/2C returns the values in the following registers:                  ;
;     CH = Hours                                                              ;
;     CL = Minutes                                                            ;
;     DH = Seconds                                                            ;
;     DL = 1/100 of Seconds                                                   ;
;                                                                             ;
; CALLED ROUTINES                                                             ;
;   -none-                                                                    ;
;                                                                             ;
; PARAMETERS                                                                  ;
;   int FAR* pHours   - Long pointer to the hours.                            ;
;   int FAR* pMinutes - Long pointer to the minutes.                          ;
;   int FAR* pSeconds - Long pointer to the seconds.                          ;
;                                                                             ;
; GLOBAL VARIABLES                                                            ;
;   -none-                                                                    ;
;                                                                             ;
; RETURNS                                                                     ;
;   void                                                                      ;
;-----------------------------------------------------------------------------;
cProc GetSystemDateTime,<FAR,PUBLIC>,<cx,dx,si,di>
     parmD pDateTime
cBegin
        lds si,pDateTime                ; load address of hour variable into

        mov ah,2ch                      ; DOS function call to get the time.
        int 21h                         ; ch=hr cl=min dh=sec dl=100th of sec.
        mov byte ptr[si].bHours,ch      ; ds:si, then copy value.
        mov byte ptr[si].bMinutes,cl    ; ds:si, then copy value.
        mov byte ptr[si].bSeconds,dh    ; ds:si, then copy value.

        mov ah,2ah                      ; DOS function call to get the time.
        int 21h                         ; ch=hr cl=min dh=sec dl=100th of sec.
        mov byte ptr[si].bDayWeek,al    ; ds:si, then copy value.
        mov word ptr[si].wYear,cx       ; ds:si, then copy value.
        mov byte ptr[si].bMonth,dh      ; ds:si, then copy value.
        mov byte ptr[si].bDay,dl        ; ds:si, then copy value.
cEnd
sEnd    CODE
end

