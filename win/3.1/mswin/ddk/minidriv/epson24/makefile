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
DRVNAME = EPSON24

#-------------------------------------------------------------
# Enter the names of all of the resident font file names
# (with extensions) after FONTS =
#-------------------------------------------------------------
FONTS = ROMAN05.PFM ROMAN06.PFM ROMAN10.PFM ROMAN12.PFM ROMAN15.PFM \
        ROMAN17.PFM ROMAN20.PFM ROMANPS.PFM SANS05.PFM SANS06.PFM \
        SANS10.PFM SANS12.PFM SANS15.PFM SANS17.PFM SANS20.PFM SANSPS.PFM \
        COUR05.PFM COUR06.PFM COUR10.PFM COUR12.PFM COUR15.PFM COUR17.PFM \
        COUR20.PFM PREST05.PFM PREST06.PFM PREST10.PFM PREST12.PFM \
        PREST15.PFM PREST17.PFM PREST20.PFM SCRIPT05.PFM SCRIPT06.PFM \
        SCRIPT10.PFM SCRIPT12.PFM SCRIPT15.PFM SCRIPT17.PFM SCRIPT20.PFM \
        OCRB.PFM

#-------------------------------------------------------------
# Enter the names of all of the version resource files
# (with extensions) after RCV =
#-------------------------------------------------------------
RCV =   EPSON24.RCV

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