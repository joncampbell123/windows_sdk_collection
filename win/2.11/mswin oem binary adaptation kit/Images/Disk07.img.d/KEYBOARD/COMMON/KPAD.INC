; This little include file defines some padding which is placed at the
; end of the keyboard tables in the preloaded DATA.OBJ, so there is always
; room to load the loadable tables.

; it is included inside of an IFDEF INDRIVER in the various DATA*.asm sources
; (US1.asm, US4.asm, ES1,asm etc.)

; Define maximum space allocated in driver for loadable keyboard tables
;
;	 ========= Windows 2.0 =================
;	29 oct 87	plb	Changed so that sizes are specified
;				in increments of 10.
;	27 oct 87	plb	Provided country-specific MAXSIZE in
;				Ericsson case.
;	09 oct 87	AP	Increased MAXSIZE for Ericsson tables
;	28 sep 87	plb	Reduced MAXSIZE to 920
;	27 aug 87	plb	Reduced MAXSIZE to 960 (largest xxx.wk2
;				file is 946 bytes.
;	29 jun 87	plb	Removed KeyEnd, changed MAXSIZE, etc.
;	 1 apr 86	plb	Created

	public MAXSIZE	; make it appear in the .MAP file!

ifndef ERICSSON
 MAXSIZE equ 820	; change this if we need more!
else
 ifdef S_820
  MAXSIZE equ 820
 else
  ifdef S_810
   MAXSIZE equ 810
  else
   ifdef S_800
    MAXSIZE equ 800
   else
    ifdef S_790
     MAXSIZE equ 790
    else
     ifdef S_780
      MAXSIZE equ 780
     else
      ifdef S_770
       MAXSIZE equ 770
      else
       ifdef S_760
        MAXSIZE equ 760
       else
        ifdef S_750
         MAXSIZE equ 750
        else
         ifdef S_740
          MAXSIZE equ 740
         else
          ifdef S_730
           MAXSIZE equ 730
          else
           ifdef S_720
            MAXSIZE equ 720
           else
            ifdef S_710
             MAXSIZE equ 710
            else
             ifdef S_700
              MAXSIZE equ 700
             else
              ifdef S_690
               MAXSIZE equ 690
              else
               ifdef S_680
                MAXSIZE equ 680
               else
                ifdef S_670
                 MAXSIZE equ 670
                else
                 MAXSIZE equ 820
		 if1
	         %out . Default size, forgot to set flag
		 endif
                endif
               endif
              endif
             endif
            endif
           endif
          endif
         endif
        endif
       endif
      endif
     endif
    endif
   endif
  endif
 endif

endif


MESSAGE	MACRO parm
%out parm
	ENDM

; define size of tables

KEYSIZE = (dataOFFSET KeyEnd) - (dataOFFSET KeyStart)

; finally, the actual padding

if2
	MESSAGE <..                 KEYSIZE = %KEYSIZE>
	MESSAGE <..                 MAXSIZE = %MAXSIZE>
endif

if KEYSIZE gt MAXSIZE
 if2
	MESSAGE <.. Table too big ..>
	errnz KEYSIZE - MAXSIZE
 endif
else
 if (MAXSIZE - KEYSIZE)
	; Put this in if the expression is nonzero
	DB	(MAXSIZE - KEYSIZE) dup (0)
 else
  if1
	%out MAXSIZE == KEYSIZE in KPAD .. largest possible table
  endif
 endif
endif
