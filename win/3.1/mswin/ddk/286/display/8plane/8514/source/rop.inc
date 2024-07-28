page +              

;First some equates to make the typing a little easier:

	n	    equ     01h
	TrailNot    equ     81h
	sr	    equ     72h
	sx	    equ     52h
	sa	    equ     0c2h
	so	    equ     0b2h
	pr	    equ     74h
	px	    equ     54h
	pa	    equ     0c4h
	po	    equ     0b4h
	dr	    equ     78h
	dxo	    equ     58h
	da	    equ     0c8h
	do	    equ     0b8h
	pb	    equ     14h 	;pattern black
	pw	    equ     24h 	;pattern white

;And here are our "operand-present" flags (the actual equated values are in
;BOARDBLT.ASM):

	S	equ	SrcPresent
	P	equ	PatternPresent
	D	equ	DstPresent
	TN	equ	TrailingNotPresent
	SB	equ	S OR P
	SD	equ	S OR D
	SN	equ	S OR TN
	PD	equ	P OR D
	PN	equ	P OR TN
	DN	equ	D OR TN
	SPD	equ	S OR P OR D
	SPN	equ	S OR P OR TN
	SPDN	equ	S OR P OR D OR TN


ROP_TABLE	label	byte
        dw      DataOFFSET ROP0
        db      ROP0Len OR P
        dw      DataOFFSET ROP1
        db      ROP1Len OR SPDN
        dw      DataOFFSET ROP2
        db      ROP2Len OR SPD
        dw      DataOFFSET ROP3
        db      ROP3Len OR SPN
        dw      DataOFFSET ROP4
        db      ROP4Len OR SPD
        dw      DataOFFSET ROP5
        db      ROP5Len OR PN
        dw      DataOFFSET ROP6
        db      ROP6Len OR SPN
        dw      DataOFFSET ROP7
        db      ROP7Len OR SPN
        dw      DataOFFSET ROP8
        db      ROP8Len OR SPD
        dw      DataOFFSET ROP9
        db      ROP9Len OR SPN
        dw      DataOFFSET ROPA
        db      ROPALen OR PD
        dw      DataOFFSET ROPB
        db      ROPBLen OR SPN
        dw      DataOFFSET ROPC
        db      ROPCLen OR SB
        dw      DataOFFSET ROPD
        db      ROPDLen OR SPDN
        dw      DataOFFSET ROPE
        db      ROPELen OR SPN
        dw      DataOFFSET ROPF
        db      ROPFLen OR PN
        dw      DataOFFSET ROP10
        db      ROP10Len OR SB
        dw      DataOFFSET ROP11
        db      ROP11Len OR SN
        dw      DataOFFSET ROP12
        db      ROP12Len OR SPDN
        dw      DataOFFSET ROP13
        db      ROP13Len OR SPDN
        dw      DataOFFSET ROP14
        db      ROP14Len OR SPDN
        dw      DataOFFSET ROP15
        db      ROP15Len OR SPDN
        dw      DataOFFSET ROP16
        db      ROP16Len OR SPD
        dw      DataOFFSET ROP17
        db      ROP17Len
        dw      DataOFFSET ROP18
        db      ROP18Len
        dw      DataOFFSET ROP19
        db      ROP19Len OR SPDN
        dw      DataOFFSET ROP1A
        db      ROP1ALen OR SPD
        dw      DataOFFSET ROP1B
        db      ROP1BLen OR SPDN
        dw      DataOFFSET ROP1C
        db      ROP1CLen OR SPD
        dw      DataOFFSET ROP1D
        db      ROP1DLen OR SPDN
        dw      DataOFFSET ROP1E
        db      ROP1ELen OR SB
        dw      DataOFFSET ROP1F
        db      ROP1FLen OR SPN
        dw      DataOFFSET ROP20
        db      ROP20Len OR SPD
        dw      DataOFFSET ROP21
        db      ROP21Len OR SPDN
        dw      DataOFFSET ROP22
        db      ROP22Len OR SPD
        dw      DataOFFSET ROP23
        db      ROP23Len OR SPN
        dw      DataOFFSET ROP24
        db      ROP24Len
        dw      DataOFFSET ROP25
        db      ROP25Len OR SPDN
        dw      DataOFFSET ROP26
        db      ROP26Len OR SPD
        dw      DataOFFSET ROP27
        db      ROP27Len OR SPD
        dw      DataOFFSET ROP28
        db      ROP28Len OR SPD
        dw      DataOFFSET ROP29
        db      ROP29Len OR SPDN
        dw      DataOFFSET ROP2A
        db      ROP2ALen OR SPD
        dw      DataOFFSET ROP2B
        db      ROP2BLen
        dw      DataOFFSET ROP2C
        db      ROP2CLen OR SB
        dw      DataOFFSET ROP2D
        db      ROP2DLen OR SB
        dw      DataOFFSET ROP2E
        db      ROP2ELen OR SPD
        dw      DataOFFSET ROP2F
        db      ROP2FLen OR SPN
        dw      DataOFFSET ROP30
        db      ROP30Len OR SB
        dw      DataOFFSET ROP31
        db      ROP31Len OR SPDN
        dw      DataOFFSET ROP32
        db      ROP32Len OR SPD
        dw      DataOFFSET ROP33
        db      ROP33Len OR SN
        dw      DataOFFSET ROP34
        db      ROP34Len OR SB
        dw      DataOFFSET ROP35
        db      ROP35Len OR SB
        dw      DataOFFSET ROP36
        db      ROP36Len OR SPD
        dw      DataOFFSET ROP37
        db      ROP37Len OR SPN
        dw      DataOFFSET ROP38
        db      ROP38Len OR SB
        dw      DataOFFSET ROP39
        db      ROP39Len OR SB
        dw      DataOFFSET ROP3A
        db      ROP3ALen OR SB
        dw      DataOFFSET ROP3B
        db      ROP3BLen OR SPN
        dw      DataOFFSET ROP3C
        db      ROP3CLen OR SB
        dw      DataOFFSET ROP3D
        db      ROP3DLen OR SB
        dw      DataOFFSET ROP3E
        db      ROP3ELen OR SPD
        dw      DataOFFSET ROP3F
        db      ROP3FLen OR SPN
        dw      DataOFFSET ROP40
        db      ROP40Len OR SB
        dw      DataOFFSET ROP41
        db      ROP41Len OR SPDN
        dw      DataOFFSET ROP42
        db      ROP42Len
        dw      DataOFFSET ROP43
        db      ROP43Len OR SPN
        dw      DataOFFSET ROP44
        db      ROP44Len OR SB
        dw      DataOFFSET ROP45
        db      ROP45Len OR SPDN
        dw      DataOFFSET ROP46
        db      ROP46Len OR SPD
        dw      DataOFFSET ROP47
        db      ROP47Len OR SPN
        dw      DataOFFSET ROP48
        db      ROP48Len OR SB
        dw      DataOFFSET ROP49
        db      ROP49Len OR SPDN
        dw      DataOFFSET ROP4A
        db      ROP4ALen OR SPD
        dw      DataOFFSET ROP4B
        db      ROP4BLen OR SPD
        dw      DataOFFSET ROP4C
        db      ROP4CLen OR SB
        dw      DataOFFSET ROP4D
        db      ROP4DLen
        dw      DataOFFSET ROP4E
        db      ROP4ELen OR SPD
        dw      DataOFFSET ROP4F
        db      ROP4FLen OR SPDN
        dw      DataOFFSET ROP50
        db      ROP50Len OR P
        dw      DataOFFSET ROP51
        db      ROP51Len OR SPDN
        dw      DataOFFSET ROP52
        db      ROP52Len OR SPD
        dw      DataOFFSET ROP53
        db      ROP53Len OR SPN
        dw      DataOFFSET ROP54
        db      ROP54Len OR SPDN
        dw      DataOFFSET ROP55
        db      ROP55Len OR TN
        dw      DataOFFSET ROP56
        db      ROP56Len OR SPD
        dw      DataOFFSET ROP57
        db      ROP57Len OR SPDN
        dw      DataOFFSET ROP58
        db      ROP58Len OR SPD
        dw      DataOFFSET ROP59
        db      ROP59Len OR SPD
        dw      DataOFFSET ROP5A
        db      ROP5ALen OR P
        dw      DataOFFSET ROP5B
        db      ROP5BLen OR SPD
        dw      DataOFFSET ROP5C
        db      ROP5CLen OR SPD
        dw      DataOFFSET ROP5D
        db      ROP5DLen OR SPDN
        dw      DataOFFSET ROP5E
        db      ROP5ELen OR SPD
        dw      DataOFFSET ROP5F
        db      ROP5FLen OR PN
        dw      DataOFFSET ROP60
        db      ROP60Len OR SB
        dw      DataOFFSET ROP61
        db      ROP61Len OR SPDN
        dw      DataOFFSET ROP62
        db      ROP62Len OR SPD
        dw      DataOFFSET ROP63
        db      ROP63Len OR SPD
        dw      DataOFFSET ROP64
        db      ROP64Len OR SPD
        dw      DataOFFSET ROP65
        db      ROP65Len OR SPD
        dw      DataOFFSET ROP66
        db      ROP66Len OR S
        dw      DataOFFSET ROP67
        db      ROP67Len OR SPD
        dw      DataOFFSET ROP68
        db      ROP68Len OR SPDN
        dw      DataOFFSET ROP69
        db      ROP69Len OR SPN
        dw      DataOFFSET ROP6A
        db      ROP6ALen OR SPD
        dw      DataOFFSET ROP6B
        db      ROP6BLen OR SPDN
        dw      DataOFFSET ROP6C
        db      ROP6CLen OR SPD
        dw      DataOFFSET ROP6D
        db      ROP6DLen OR SPDN
        dw      DataOFFSET ROP6E
        db      ROP6ELen OR SPD
        dw      DataOFFSET ROP6F
        db      ROP6FLen OR SPN
        dw      DataOFFSET ROP70
        db      ROP70Len OR SB
        dw      DataOFFSET ROP71
        db      ROP71Len
        dw      DataOFFSET ROP72
        db      ROP72Len OR SPD
        dw      DataOFFSET ROP73
        db      ROP73Len OR SPDN
        dw      DataOFFSET ROP74
        db      ROP74Len OR SPD
        dw      DataOFFSET ROP75
        db      ROP75Len OR SPDN
        dw      DataOFFSET ROP76
        db      ROP76Len OR SPD
        dw      DataOFFSET ROP77
        db      ROP77Len OR SN
        dw      DataOFFSET ROP78
        db      ROP78Len OR SB
        dw      DataOFFSET ROP79
        db      ROP79Len OR SPDN
        dw      DataOFFSET ROP7A
        db      ROP7ALen OR SPD
        dw      DataOFFSET ROP7B
        db      ROP7BLen OR SPN
        dw      DataOFFSET ROP7C
        db      ROP7CLen OR SPD
        dw      DataOFFSET ROP7D
        db      ROP7DLen OR SPDN
        dw      DataOFFSET ROP7E
        db      ROP7ELen
        dw      DataOFFSET ROP7F
        db      ROP7FLen OR SPDN

;Following is the list of Raster Operations for BITBLT and the "call table"
;suitable for each of them

ROP0    db              pb
ROP0Len         equ     $-ROP0

ROP1    db              sr,po,do,TrailNot
ROP1Len         equ     $-ROP1

ROP2    db              sr,po,n,da
ROP2Len         equ     $-ROP2

ROP3    db              sr,po,TrailNot
ROP3Len         equ     $-ROP3

ROP4    db              pr,do,n,sa
ROP4Len         equ     $-ROP4

ROP5    db              po,TrailNot
ROP5Len         equ     $-ROP5

ROP6    db              sx,n,po,TrailNot
ROP6Len         equ     $-ROP6

ROP7    db              sa,po,TrailNot
ROP7Len         equ     $-ROP7

ROP8    db              pr,n,da,sa
ROP8Len         equ     $-ROP8

ROP9    db              sx,po,TrailNot
ROP9Len         equ     $-ROP9

ROPA    db              pr,n,da
ROPALen         equ     $-ROPA

ROPB    db              n,sa,po,TrailNot
ROPBLen         equ     $-ROPB

ROPC    db              pr,n,sa
ROPCLen         equ     $-ROPC

ROPD    db              sr,n,da,po,TrailNot
ROPDLen         equ     $-ROPD

ROPE    db              so,n,po,TrailNot
ROPELen         equ     $-ROPE

ROPF    db              pr,TrailNot
ROPFLen         equ     $-ROPF

ROP10   db              so,n,pa
ROP10Len        equ     $-ROP10

ROP11   db              so,TrailNot
ROP11Len        equ     $-ROP11

ROP12   db              pr,dxo,n,so,TrailNot
ROP12Len        equ     $-ROP12

ROP13   db              pr,da,so,TrailNot
ROP13Len        equ     $-ROP13

ROP14   db              sr,px,n,do,TrailNot
ROP14Len        equ     $-ROP14

ROP15   db              sr,pa,do,TrailNot
ROP15Len        equ     $-ROP15

ROP16   db              sr,pa,n,da,sx,px
ROP16Len        equ     $-ROP16

ROP17   dw              0066h,0046h,001bh,0768h    ;2 passes: 1:Rop 66, 2:Rop 1B
ROP17Len equ     $-ROP17

ROP18   dw		005Ah,0049h,0028h,0369h	   ;2 pases: 1:Rop 5A, 2:Rop 28
ROP18Len equ     $-ROP18

ROP19   db              sr,pa,n,da,sx,TrailNot
ROP19Len        equ     $-ROP19

ROP1A   db              pr,sa,do,px
ROP1ALen        equ     $-ROP1A

ROP1B   db              sr,px,da,sx,TrailNot
ROP1BLen        equ     $-ROP1B

ROP1C   db              pr,da,so,px
ROP1CLen        equ     $-ROP1C

ROP1D	db		dr,px,sa,dxo,TrailNot
ROP1DLen        equ     $-ROP1D

ROP1E   db              so,px
ROP1ELen        equ     $-ROP1E

ROP1F   db              so,pa,TrailNot
ROP1FLen        equ     $-ROP1F

ROP20   db              sr,n,pa,da
ROP20Len        equ     $-ROP20

ROP21   db              pr,dxo,so,TrailNot
ROP21Len        equ     $-ROP21

ROP22   db              sr,n,da
ROP22Len        equ     $-ROP22

ROP23   db              n,pa,so,TrailNot
ROP23Len        equ     $-ROP23

ROP24   dw              0066h,0046h,0028h,0369h
ROP24Len        equ     $-ROP24

ROP25   db              pr,sa,n,da,px,TrailNot
ROP25Len        equ     $-ROP25

ROP26   db              sr,pa,do,sx
ROP26Len        equ     $-ROP26

ROP27   db              sr,px,n,do,sx
ROP27Len        equ     $-ROP27

ROP28   db              sr,px,da
ROP28Len        equ     $-ROP28

ROP29   db              sr,pa,do,sx,px,TrailNot
ROP29Len        equ     $-ROP29

ROP2A   db              sr,pa,n,da
ROP2ALen        equ     $-ROP2A

ROP2B   dw              005ah,0049h,001bh,0768h
ROP2BLen        equ     $-ROP2B

ROP2C   db              so,pa,sx
ROP2CLen        equ     $-ROP2C

ROP2D   db              n,so,px
ROP2DLen        equ     $-ROP2D

ROP2E   db              px,so,px
ROP2ELen        equ     $-ROP2E

ROP2F   db              n,so,pa,TrailNot
ROP2FLen        equ     $-ROP2F

ROP30   db              sr,n,pa
ROP30Len        equ     $-ROP30

ROP31   db              pr,n,da,so,TrailNot
ROP31Len        equ     $-ROP31

ROP32   db              sr,po,do,sx
ROP32Len        equ     $-ROP32

ROP33   db              sr,TrailNot
ROP33Len        equ     $-ROP33

ROP34   db              sa,po,sx
ROP34Len        equ     $-ROP34

ROP35   db              sx,n,po,sx
ROP35Len        equ     $-ROP35

ROP36   db              pr,do,sx
ROP36Len        equ     $-ROP36

ROP37   db              po,sa,TrailNot
ROP37Len        equ     $-ROP37

ROP38   db              po,sa,px
ROP38Len        equ     $-ROP38

ROP39   db              n,po,sx
ROP39Len        equ     $-ROP39

ROP3A   db              sx,po,sx
ROP3ALen        equ     $-ROP3A

ROP3B   db              n,po,sa,TrailNot
ROP3BLen        equ     $-ROP3B

ROP3C   db              sr,px
ROP3CLen        equ     $-ROP3C

ROP3D   db              so,n,po,sx
ROP3DLen        equ     $-ROP3D

ROP3E   db              sr,n,da,po,sx
ROP3ELen        equ     $-ROP3E

ROP3F   db              sr,pa,TrailNot
ROP3FLen        equ     $-ROP3F

ROP40   db              n,sa,pa
ROP40Len        equ     $-ROP40

ROP41   db              sr,px,do,TrailNot
ROP41Len        equ     $-ROP41

ROP42   dw              004dh,1954h,005ah,0049h,0055h,0009h
ROP42Len        equ     $-ROP42

ROP43   db              sa,n,pa,sx,TrailNot
ROP43Len        equ     $-ROP43

ROP44   db              n,sa
ROP44Len        equ     $-ROP44

ROP45   db              sr,n,pa,do,TrailNot
ROP45Len        equ     $-ROP45

ROP46   db              pa,so,dxo
ROP46Len        equ     $-ROP46

ROP47   db              px,sa,px,TrailNot
ROP47Len        equ     $-ROP47

ROP48   db              px,sa
ROP48Len        equ     $-ROP48

ROP49   db              pa,so,dxo,px,TrailNot
ROP49Len        equ     $-ROP49

ROP4A   db              so,pa,dxo
ROP4ALen        equ     $-ROP4A

ROP4B   db              sr,n,do,px
ROP4BLen        equ     $-ROP4B

ROP4C   db              pa,n,sa
ROP4CLen        equ     $-ROP4C

ROP4D   dw		0066h,0046h,008dh,0668h
ROP4DLen        equ     $-ROP4D

ROP4E   db              pr,sx,do,px
ROP4ELen        equ     $-ROP4E

ROP4F   db              sr,n,do,pa,TrailNot
ROP4FLen        equ     $-ROP4F

ROP50   db              n,pa
ROP50Len        equ     $-ROP50

ROP51   db              pr,n,sa,do,TrailNot
ROP51Len        equ     $-ROP51

ROP52   db              sa,po,dxo
ROP52Len        equ     $-ROP52

ROP53   db              sx,pa,sx,TrailNot
ROP53Len        equ     $-ROP53

ROP54   db              sr,po,n,do,TrailNot
ROP54Len        equ     $-ROP54

ROP55   db              TrailNot
ROP55Len        equ     $-ROP55

ROP56   db              sr,po,dxo
ROP56Len        equ     $-ROP56

ROP57   db              sr,po,da,TrailNot
ROP57Len        equ     $-ROP57

ROP58   db              pr,so,da,px
ROP58Len        equ     $-ROP58

ROP59   db              sr,n,po,dxo
ROP59Len        equ     $-ROP59

ROP5A   db              px
ROP5ALen        equ     $-ROP5A

ROP5B   db              so,n,po,dxo
ROP5BLen        equ     $-ROP5B

ROP5C   db              sx,po,dxo
ROP5CLen        equ     $-ROP5C

ROP5D   db              sr,n,po,da,TrailNot
ROP5DLen        equ     $-ROP5D

ROP5E   db              n,sa,po,dxo
ROP5ELen        equ     $-ROP5E

ROP5F   db              pa,TrailNot
ROP5FLen        equ     $-ROP5F

ROP60   db              sx,pa
ROP60Len        equ     $-ROP60

ROP61   db              sa,po,sx,dxo,TrailNot
ROP61Len        equ     $-ROP61

ROP62   db              po,sa,dxo
ROP62Len        equ     $-ROP62

ROP63   db              pr,n,do,sx
ROP63Len        equ     $-ROP63

ROP64   db              sr,po,da,sx
ROP64Len        equ     $-ROP64

ROP65   db              pr,n,so,dxo
ROP65Len        equ     $-ROP65

ROP66	db		sx
ROP66Len        equ     $-ROP66

ROP67   db              sr,po,n,do,sx
ROP67Len        equ     $-ROP67

ROP68   db              so,n,po,sx,dxo,TrailNot
ROP68Len        equ     $-ROP68

ROP69   db              sx,px,TrailNot
ROP69Len        equ     $-ROP69

ROP6A   db              sr,pa,dxo
ROP6ALen        equ     $-ROP6A

ROP6B   db              sr,po,da,sx,px,TrailNot
ROP6BLen        equ     $-ROP6B

ROP6C   db              pr,da,sx
ROP6CLen        equ     $-ROP6C

ROP6D   db              po,sa,dxo,px,TrailNot
ROP6DLen        equ     $-ROP6D

ROP6E   db              sr,n,po,da,sx
ROP6ELen        equ     $-ROP6E

ROP6F   db              sx,n,pa,TrailNot
ROP6FLen        equ     $-ROP6F

ROP70   db              sa,n,pa
ROP70Len        equ     $-ROP70

ROP71   dw              007eh,0955h,005ah,0049h,0055h,0009h
ROP71Len        equ     $-ROP71

ROP72   db              sr,px,do,sx
ROP72Len        equ     $-ROP72

ROP73   db              pr,n,do,sa,TrailNot
ROP73Len        equ     $-ROP73

ROP74   db              px,so,dxo
ROP74Len        equ     $-ROP74

ROP75   db              pr,n,so,da,TrailNot
ROP75Len        equ     $-ROP75

ROP76   db              sr,n,pa,do,sx
ROP76Len        equ     $-ROP76

ROP77   db              sa,TrailNot
ROP77Len        equ     $-ROP77

ROP78   db              sa,px
ROP78Len        equ     $-ROP78

ROP79   db              so,pa,sx,dxo,TrailNot
ROP79Len        equ     $-ROP79

ROP7A   db              n,so,pa,dxo
ROP7ALen        equ     $-ROP7A

ROP7B   db              px,n,sa,TrailNot
ROP7BLen        equ     $-ROP7B

ROP7C   db              sr,n,do,pa,sx
ROP7CLen        equ     $-ROP7C

ROP7D   db              sr,px,n,da,TrailNot
ROP7DLen        equ     $-ROP7D

ROP7E   dw              0066h,0046h,00BEh,0269h    ;2 passes: 1:Rop 66, 2:Rop BE
ROP7ELen        equ     $-ROP7E

ROP7F   db              sr,pa,da,TrailNot
ROP7FLen        equ     $-ROP7F


;We special case ROP code 256 (destination white) since the 8514 can do
;it in one operation.

DestWhite       db      pw
DestWhiteLen    equ     $-DestWhite
