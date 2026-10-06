;       SCCSID = @(#)atstate.inc        5.1 86/11/10
;**  AT State Machine Definitions

START    equ     0                      ; Starting I/O
CALC     equ     1                      ; Calculate position on disk
VERIFY   equ     6                      ; Start verify portion of write
DONE     equ     7                      ; I/O is done.
IDLE     equ     8                      ; Drive is inactive
SERROR   equ     9                      ; Have an error
READ     equ     12                     ; Read from disk
WRITE    equ     13                     ; Write to disk
SETPARAM equ     14                     ; Set controller parameters

; The states below are used for floppies only - error on fixed disk


SELECT          equ  2                  ; Select drive, start motor, seek
RECAL           equ  3                  ; Drive was just recalibrated
SEEK            equ  4                  ; Seek just finished
SETTLE          equ  5                  ; Head has just settled
RDWRI           equ  6                  ; Start read or write operation
RSET            equ  10                 ; Reset the controller
TIMOUT          equ  11                 ; We had a timeout on the drive
CHKCHNGLINE     equ  15                 ; Check changeline status
SEEKRESET1      equ  16                 ; Seek after Reset to cyln 1 for reset changeline
SEEKRESET0      equ  17                 ; Seek to cylinder 0 for reset changeline
SENSESEEK0      equ  18                 ; Sense final seek for reset changeline

; The states below are used for fixed disks - note the overlap with floppies

PARAMSET        equ     2               ; Disk parameters set
FXSTARTREAD     equ     3               ; Start of read
FXSTARTWRITE    equ     4               ; Start of write
FXSTARTVERIFY   equ     5               ; Start ov verify
FXCACHEREAD     equ     10              ; Read from cache
FXCACHEWRITE    equ     11              ; Write to cache
