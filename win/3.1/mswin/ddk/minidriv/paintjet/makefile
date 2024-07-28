#-------------------------------------------------------------
# 
# MINI-DRIVER Make file template
#
# The follwing entries should updated
#-------------------------------------------------------------
#
#-------------------------------------------------------------
# Enter the DRV file name (without extension) after DRVNAME =
#-------------------------------------------------------------
# DRIVER NAME
DRVNAME = PAINTJET

#-------------------------------------------------------------
# Enter the names of all of the resident font file names
# (with extensions) after FONTS =
#-------------------------------------------------------------
FONTS = cour10.pfm lgth12.pfm lgth18.pfm

#-------------------------------------------------------------
# Enter the names of all of the version resource files
# (with extensions) after RCV =
#-------------------------------------------------------------
RCV =   $(DRVNAME).RCV

#**********************************************************************
# Do not edit below this line
#**********************************************************************
target:  $(DRVNAME).drv

$(DRVNAME).res:    $(DRVNAME).rc $(DRVNAME).GPC $(FONTS) $(RCV)
    rc -r -I. $(DRVNAME).RC

minidriv.obj:       ..\minidriv.c
    cl -u -c -Asnw -PLM -G2sw -W3 -Oas -Zpe -I. ..\minidriv.c

$(DRVNAME).obj:     $(DRVNAME).c
    cl -u -c -Asnw -PLM -G2sw -W3 -Oas -Zpe -I. $(DRVNAME).c

$(DRVNAME).exe:    minidriv.obj $(DRVNAME).obj $(DRVNAME).res $(DRVNAME).def
    link @$(DRVNAME).lnk
    mapsym $(DRVNAME)

$(DRVNAME).drv:     $(DRVNAME).res $(DRVNAME).exe
    rc -30 -t -v -I. $(DRVNAME)
    copy $(DRVNAME).exe $(DRVNAME).drv


