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
DRVNAME = BASE

#-------------------------------------------------------------
# Enter the names of all of the resident font file names
# (with extensions) after FONTS =
#-------------------------------------------------------------
FONTS = STANDARD.PFM

#**********************************************************************
# Do not edit below this line
#**********************************************************************

target:  $(DRVNAME).drv

$(DRVNAME).res:  $(DRVNAME).rc $(DRVNAME).GPC $(FONTS)
    rc -r -I. $(DRVNAME).RC

minidriv.obj:    ..\minidriv.c
    cl -u -c -Asnw -PLM -Gsw -W3 -Oas -Zpe -I. ..\minidriv.c

$(DRVNAME).obj:  $(DRVNAME).c
    cl -u -c -Asnw -PLM -Gsw -W3 -Oas -Zpe -I. $(DRVNAME).c

$(DRVNAME).exe:  minidriv.obj $(DRVNAME).obj $(DRVNAME).res $(DRVNAME).def
    link @$(DRVNAME).lnk
    mapsym $(DRVNAME)

$(DRVNAME).drv:  $(DRVNAME).res $(DRVNAME).exe
    rc -v -I. $(DRVNAME)
    copy $(DRVNAME).exe $(DRVNAME).drv

