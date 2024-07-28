#-------------------------------------------------------------
# 
# MINI-DRIVER Make file template
#
# The follwing entries should updated
#-------------------------------------------------------------
#
#-------------------------------------------------------------
# Enter the DRV file name (without extention) after DRVNAME =
#-------------------------------------------------------------
# DRIVER NAME
DRVNAME = PROPRN24

#-------------------------------------------------------------
# Enter the names of all of the resident font file names
# (with extentions) after FONTS =
#-------------------------------------------------------------
FONTS = COUR05.PFM COUR05D.PFM COUR08.PFM COUR10.PFM COUR17.PFM COURPS.PFM\
        COURPSD.PFM PREST06.PFM PREST06D.PFM PREST12.PFM

#-------------------------------------------------------------
# Enter the names of all of the version resource files
# (with extensions) after RCV =
#-------------------------------------------------------------
RCV =   $(DRVNAME).RCV

#-------------------------------------------------------------
# Enter the names of all of the character translation tables
# (with extensions) after CTTS =
#-------------------------------------------------------------
CTTS  = STDIBM.CTT

#**********************************************************************
# Do not edit below this line
#**********************************************************************
target: $(DRVNAME).drv

minidriv.obj:       ..\minidriv.c
    cl -u -c -Asnw -PLM -G2sw -W3 -Oas -Zpe -I. ..\minidriv.c

$(DRVNAME).res:    $(DRVNAME).rc $(DRVNAME).GPC $(FONTS) $(CTTS)
    rc -r -I. $(DRVNAME).RC

$(DRVNAME).obj:     $(DRVNAME).c
    cl -u -c -Asnw -PLM -G2sw -W3 -Oas -Zpe -I. $(DRVNAME).c

$(DRVNAME).exe:    minidriv.obj $(DRVNAME).obj $(DRVNAME).def
    link @$(DRVNAME).lnk
    mapsym $(DRVNAME)

$(DRVNAME).drv:     $(DRVNAME).res $(DRVNAME).exe
    rc -30 -t -v -I. $(DRVNAME)
    copy $(DRVNAME).exe $(DRVNAME).drv

