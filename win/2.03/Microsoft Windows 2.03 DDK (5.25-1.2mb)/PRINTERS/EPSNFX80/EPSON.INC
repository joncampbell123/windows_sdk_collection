
IBMColor        =       1               ;do assemble for IBM PC Color Card
Hercules        =       0               ;Don't assemble for the Hercules card

DEV_LAND        equ 8889h
DEV_PORT        equ 8888h

; start of device dependent information, you must fill this part in
; according to the printer resolution and aspect ratio

IFDEF	R120x72
; Definitions for Resolution 120 x 72 Printers

Hypotenuse   = 140d                      ;Distance moving X and Y
yMajorDist   = 120d                      ;Distance moving Y only
xMajorDist   =  72d                      ;Distance moving X only

HDPI            equ  120d       ; 120 dots per inch across the page
VDPI            equ   72d       ; 72 DPI down the page

PG_ACROSS       equ 960		; 8 inches across the page
WIDE_PG_ACROSS  equ 1584	; 14 inches across the page
PG_DOWN         equ 720		; 10 inches down the page

MM_HSIZE        equ   203d	; PG_ACROSS / HDPI * 25.4
WIDE_MM_HSIZE   equ   335d	; WIDE_PG_ACROSS / HDPI * 25.4
MM_VSIZE        equ   254d	; PG_DOWN / VDPI * 25.4

ENDIF

IFDEF	R140x84
; Definitions for Resolution 140 x 84 Printers

Hypotenuse   = 163d                      ;Distance moving X and Y
yMajorDist   = 140d                      ;Distance moving Y only
xMajorDist   =  84d                      ;Distance moving X only

HDPI            equ  140d       ; 140 dots per inch across the page
VDPI            equ   84d       ; 84 DPI down the page

PG_ACROSS       equ 1120	; 8 inches across the page
WIDE_PG_ACROSS  equ 1840	; 13.2 inches across the page
PG_DOWN         equ 896 	; 10.67 inches down the page

MM_HSIZE        equ   203d	; PG_ACROSS / HDPI * 25.4
WIDE_MM_HSIZE   equ   335d	; WIDE_PG_ACROSS / HDPI * 25.4
MM_VSIZE        equ   271d	; PG_DOWN / VDPI * 25.4

ENDIF

IFDEF	R60x72
; Definitions for Resolution 60 x 72 Printers

Hypotenuse   =  94d                      ;Distance moving X and Y
yMajorDist   =  60d                      ;Distance moving Y only
xMajorDist   =  72d                      ;Distance moving X only

HDPI            equ   60d       ; 60 dots per inch across the page
VDPI            equ   72d       ; 72 DPI down the page

PG_ACROSS       equ 480 	; 8 inches across the page
PG_DOWN         equ 784 	; 10.89 inches down the page

MM_HSIZE        equ   203d	; PG_ACROSS / HDPI * 25.4
MM_VSIZE        equ   276d	; PG_DOWN / VDPI * 25.4

ENDIF

IFDEF	R180x60
; Definitions for Resolution 180 x 60 Printers

Hypotenuse   = 190d                      ;Distance moving X and Y
yMajorDist   = 180d                      ;Distance moving Y only
xMajorDist   =  60d                      ;Distance moving X only

HDPI            equ  180d       ; 180 dots per inch across the page
VDPI            equ   60d       ; 60 DPI down the page

PG_ACROSS       equ 1440	; 8 inches across the page
WIDE_PG_ACROSS  equ 2368	; 13.2 inches across the page
PG_DOWN         equ  624	; 10.4 inches down the page

MM_HSIZE        equ   203d	; PG_ACROSS / HDPI * 25.4
WIDE_MM_HSIZE   equ   335d	; WIDE_PG_ACROSS / HDPI * 25.4
MM_VSIZE        equ   264d	; PG_DOWN / VDPI * 25.4

ENDIF

IFDEF	R120x60
; Definitions for Resolution 120 x 60 Printers

Hypotenuse   = 134d                      ;Distance moving X and Y
yMajorDist   = 120d                      ;Distance moving Y only
xMajorDist   =  60d                      ;Distance moving X only

HDPI            equ  120d       ; 120 dots per inch across the page
VDPI            equ   60d       ; 60 DPI down the page

PG_ACROSS   	equ 960		; 8 inches across the page
WIDE_PG_ACROSS 	equ 1584	; 13.2 inches across the page
PG_DOWN     	equ 624		; 10.4 inches down the page

MM_HSIZE        equ   203d	; PG_ACROSS / HDPI * 25.4
WIDE_MM_HSIZE   equ   335d	; WIDE_PG_ACROSS / HDPI * 25.4
MM_VSIZE        equ   264d	; PG_DOWN / VDPI * 25.4

ENDIF

IFDEF	R96x72
; Definitions for Resolution 96 x 72 Printers

Hypotenuse   = 120d                     ;Distance moving X and Y 
yMajorDist   = 96d                      ;Distance moving Y only
xMajorDist   = 72d                      ;Distance moving X only

HDPI            equ  96d       ; 96 dots per inch across the page
VDPI            equ   72d       ; 72 DPI down the page

PG_ACROSS   	equ 768		; 8 inches across the page
PG_DOWN     	equ 720		; 10.0 inches down the page (our Citoh style)

MM_HSIZE        equ   203d	; PG_ACROSS / HDPI * 25.4
MM_VSIZE        equ   254d	; PG_DOWN / VDPI * 25.4

ENDIF

yMinorDist   =  Hypotenuse-xMajorDist
xMinorDist   =  Hypotenuse-yMajorDist

MaxStyleErr  =  Hypotenuse*2            ;Max error before updating
                                        ;  rotating bit mask

; order for portrait, invert 1 <--> 2, 3 <--> 4 for landscape
;HorzSize  refers to MM_HSIZE, horizontal size in millimeters
;VertSize  refers to MM_VSIZE, horizontal size in millimeters

MM01        equ 254             ;HorzSize * 10   scaled (/HorzSize / 10 * 254)
MM02        equ 254             ;VertSize * 10   scaled (/VertSize / 10 * 254)
MM03        equ HDPI            ;HorizRes        scaled (/HorzSize / 10 * 254)
MM04        equ VDPI            ;VertRes         scaled (/VertSize / 10 * 254)

MM001       equ 2540            ;HorzSize * 100  scaled (/HorzSize / 10 * 254)
MM002       equ 2540            ;VertSize * 100  scaled (/VertSize / 10 * 254)
MM003       equ HDPI            ;HorizRes        scaled (/HorzSize / 10 * 254)
MM004       equ VDPI            ;VertRes         scaled (/VertSize / 10 * 254)

EnglishLo1  equ 100             ;HorzSize * 1000 scaled (/HorzSize / 10)
EnglishLo2  equ 100             ;VertSize * 1000 scaled (/VertSize / 10)
EnglishLo3  equ HDPI            ;HorizRes * 254  scaled (/HorzSize / 10)
EnglishLo4  equ VDPI            ;VertRes * 254   scaled (/VertSize / 10)

EnglishHi1  equ 1000            ;HorzSize * 10000 scaled(/HorzSize / 10)
EnglishHi2  equ 1000            ;VertSize * 10000 scaled(/VertSize / 10)
EnglishHi3  equ HDPI            ;HorizRes * 254   scaled(/HorzSize / 10)
EnglishHi4  equ VDPI            ;VertRes * 254    scaled(/VertSize / 10)


Twips1      equ 1440            ;HorzSize * 14400 scaled(/HorzSize / 10)
Twips2      equ 1440            ;VertSize * 14400 scaled(/VertSize / 10)
Twips3      equ HDPI            ;HorizRes * 254   scaled(/HorzSize / 10)
Twips4      equ VDPI            ;VertRes * 254   scaled (/VertSize / 10)

; end of device dependent information
