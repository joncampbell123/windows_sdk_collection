;
; Copyright (c) Microsoft Corporation,  1988
;
ifdef debugo			; If debug override is defined
     abdebug    =       1	;     include debugging code
else				; else
     abdebug 	=	0	;     0 = no debugging code, 1 = debugging code
endif
ablistlevel	=	2	; 0 = minimum, 0 to 3 allowed
abtrace 	=	0	; 0 = no tracing,   1 = trace code.
				; (always leave it to 0)
