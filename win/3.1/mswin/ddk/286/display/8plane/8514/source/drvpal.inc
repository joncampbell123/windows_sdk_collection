BrushCopies struc
    bcStyle	    db	    ?
    bcColor	    db	    ?
    bcPattern	    db	    7 dup(?)
    bcBgColor	    db	    ?
    bcFgColor	    db	    ?	; bit x set: something has changed--look out!
    bcFlags	    db	    ?	; bit 0->new brush, bit 1->new indices
BrushCopies ends

BrushOrig   struc
    boStyle	    db	    ?
    boColor	    db	    ?
    boPattern	    db	    7 dup(?)
    boBgColor	    db	    ?
    boFgcolor	    db	    ?
BrushOrig   ends

NEWINDEX	    equ     2
INITCHECK	    equ     080h
TOGGLEMASK	    equ     0c0h
BITBLTACCELERATE    equ     1		; if set use tanslate tables in blts
					; (color) to and from the screen
NOMEMSTRBLT	    equ     2		; if set translate as usual

RPALETTE	    equ     02ebh
WPALETTE	    equ     02ech
DPALETTE	    equ     02edh
