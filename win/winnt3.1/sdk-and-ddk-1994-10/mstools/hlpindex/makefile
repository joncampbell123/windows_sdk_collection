#############################################################################
#
# MAKEFILE
#
#############################################################################
#
# Program Description: Generic Makefile for Help Index
#
#############################################################################
#
# The following are the optionally passed variables:
#
# STOPF		= Contains an alternate stop word file.
# FIELDF	= Contains an alternate field definition file.
# IND_ONLY	= IND_ONLY=Y will not rebuild the HLP file, force build .ind.
# OS2_FLAG      = Run bound version of forage (FORAGEP)
#                   instead of DMPI version (FORAGE).
#############################################################################

!IFNDEF NAME
!ERROR -- Viewer "NAME" macro undefined
!endif

#
!IFNDEF OS2_FLAG
FORAGE	   =FORAGE
!else
FORAGE	   =FORAGEP
!endif


!IFNDEF STOPF
STOPF		=$(NAME)
!endif

!IFNDEF FIELDF
FIELDF		=$(NAME)
!endif

# This will only build the index file 
#  
CLEAN_IND=DEL_ZOF
WIN_FS          =

# Multivolume help files- this option selected
# w/ builddoc /m parameter does BOTH /i command function as well
# as build using a .MLT file.  (used by both forage and w_scan).

!IFNDEF MULTIVOL
MLTFILE=
FOPTS= /w $(NAME).hlp
!else
MLTFILE=$(NAME).mlt
FOPTS= /m $(NAME).mlt
WIN_FS          =
!endif


goal:	$(NAME).ind $(CLEAN_IND) 

clean:
	-del $(NAME).wrs
	-del $(NAME).lst
	-del $(NAME).oca
	-del $(NAME).zon
	-del $(NAME).zos
	-del $(NAME).cfg
	-del $(NAME).ans
	-del $(NAME).ind

#following for special font option- ignore.
$(NAME).lgh: $(NAME).tbl 
        masm $(NAME).tbl,$(NAME).lg2;
        link $(NAME).lg2,$(NAME).lg3,NUL,,NUL
	@echo .
	@echo ....Ignore warning - No stack segment 
	@echo ....Ignore warning - Program has no start address
	@echo .
	exe2bin $(NAME).lg3,$(NAME).lgh
	del $(NAME).lg2
	del $(NAME).lg3

DEL_ZOF:
	@-del	$(NAME).zof
$(NAME).mlt:	$(WIN_FS) bag.ini
	getmlt bag.ini,$(NAME).mlt

$(NAME).zof:	$(NAME).mlt
	$(FORAGE) $(FOPTS)
	@echo .
	@echo .
#create a marker file so we can delete large .ans file and still
#   have good restartability.
        @echo > $(NAME).zof

#$(NAME).oca:	$(NAME).zof $(NAME).lgh 
$(NAME).oca:	$(NAME).zof 
#	w_scan $(NAME).ans,,$(STOPF).stp,$(FIELDF).fld,$(MLTFILE),$(NAME).lst,longhorn;
	w_scan $(NAME).ans,,$(STOPF).stp,$(FIELDF).fld,$(MLTFILE),$(NAME).lst;
        @-del $(NAME).ans
	@echo .
	@echo .

$(NAME).ind:	$(NAME).oca
#	index $(NAME),,$(NAME),$(STOPF).stp,$(FIELDF).fld,$(NAME).map,longhorn;
	index $(NAME),,$(NAME),$(STOPF).stp,$(FIELDF).fld,$(NAME).map;
	@-del $(NAME).wrs
	@-del $(NAME).zon
	@-del $(NAME).zos
	@-del $(NAME).cfg
	@-del $(NAME).lst
	@-del $(NAME).map
	@-del $(NAME).mlt
        
