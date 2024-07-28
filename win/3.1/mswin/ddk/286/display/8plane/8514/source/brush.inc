NewBrush    struc
    bnStyle	    db	    ?		;Brush style-solid, patterned, dithered
    bnColor	    db	    ?		;solid brush color index
    bnPattern	    db	    8 dup(?)	;hatch or mono dither pattern
    bnBgColor	    db	    ?		;pattern's fg/bg colors
    bnFgColor	    db	    ?
    bnColorBits     db	    64 dup(?)	;color/dither pattern
    bnBgMono	    db	    ?		;bg color mono bit (LSB)
    bnFgMono	    db	    ?		;fg color mono bit (LSB)
NewBrush    ends

PDIMENSION	    equ     8		; pattern block dimension
PSIZE		    equ     64		; number of bytes in pattern block
