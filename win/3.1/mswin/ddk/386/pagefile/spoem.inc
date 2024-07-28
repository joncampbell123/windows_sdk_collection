;
; WARNING WARNING DANGER DANGER!!!!!!!!!!!!!!!!!!!!!!!
;
;      THIS FILE MUST AGREE WITH SPOEM.H WHICH IS IN THE
;      SPART PROJECT OF WIN386. DO NOT CHANGE ONE WITHOUT
;      CHANGING THE OTHER!!!!!!
;
;

;**
; Structure of the OEM part of spart.par file
;
OEMCURVERSION	equ	0300h
OEMINT13TYPE	equ	1

PFileOEMForm struc
PVersion	dw	?	; Version of OEM part
PType		dw	?	; Type of OEM part
				;    1 == INT 13 drive type
StartCyln	dw	?	; The first partition cylinder */
EndCyln 	dw	?	; The last partition cylinder */
HeadNumStart	dw	?	; Heads >= this on StartCyln are in partition */
HeadNumEnd	dw	?	; Heads <= this on EndCyln are in partition */
INT13DrvNum	dw	?	; INT 13 DL value of drive */
INT13NumHeads	dw	?	; total # heads */
INT13SecPerTrk	dw	?	; sec/trk */
PFileOEMForm ends

INT13DrvParms struc
MaxHead 	dw	?	;Maximum useable head # */
MaxSec		dw	?	;Maximum useable Sector # (sec/trk) */
MaxCyln 	dw	?	;Maximum useable Cylinder # */
INT13DrvParms ends

BootSector    struc
JmpInstr	db	3 dup(?)
OEMNameVer	db	8 dup(?)
Bpersec 	dw	?
Secpalloc	db	?
Reserved	dw	?
NumFAT		db	?
NumRoot 	dw	?
Tsec		dw	?
Media		db	?
SecpFAT 	dw	?
SecpTrack	dw	?
Nheads		dw	?
Hidden		dw	?
BootSector    ends

BootSectorBig	struc
JmpInstrBig	db	3 dup(?)
OEMNameVerBig	db	8 dup(?)
BpersecBig	dw	?
SecpallocBig	db	?
ReservedBig	dw	?
NumFATBig	db	?
NumRootBig	dw	?
TsecOld 	dw	?	; This is 0 indicating it is BIG
MediaBig	db	?
SecpFATBig	dw	?
SecpTrackBig	dw	?
NheadsBig	dw	?
HiddenBig	dd	?
TsecBig 	dd	?
BootSectorBig	ends

PartitionRec  struc
BootInd 	db	?
StartHd 	db	?
StartSct	db	?
StartCln	db	?
SysInd		db	?
EndHd		db	?
EndSct		db	?
EndCln		db	?
StartRelSect	dd	?
PartSize	dd	?
PartitionRec ends

HFPartition   struc
BootPad 	db	446 dup(?)
Partition1	db	(SIZE PartitionRec) dup (?)
Partition2	db	(SIZE PartitionRec) dup (?)
Partition3	db	(SIZE PartitionRec) dup (?)
Partition4	db	(SIZE PartitionRec) dup (?)
PSig		dw	?
HFPartition ends
