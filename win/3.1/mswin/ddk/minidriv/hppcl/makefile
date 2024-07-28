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
DRVNAME = HPPCL

#-------------------------------------------------------------
# Enter the names of all of the resident font file names
# (with extensions) after FONTS =
#-------------------------------------------------------------
FONTS = newpfm\*.7J newpfm\*.8M newpfm\*.8U newpfm\*.0A newpfm\*.15U \
        newpfm\*.0N newpfm\*.1U newpfm\*.8Q newpfm\*.0U newpfm\*.11Q \
        newpfm\*.0B newpfm\*.PFM

#-------------------------------------------------------------
# Enter the names of all of the resident font file names
# (with extensions) after FONTS =
#-------------------------------------------------------------
CTTS =  ECMA94.CTT  GENERIC7.CTT GENERIC8.CTT MATH8.CTT MIXCOMP.CTT \
        NOTRANS.CTT ROMAN8.CTT   USASCII.CTT  Z1A.CTT

#-------------------------------------------------------------
# Enter the names of all of the version resource files
# (with extensions) after RCV =
#-------------------------------------------------------------
RCV =   HPPCL.RCV

#**********************************************************************
# Do not edit below this line
#**********************************************************************

target:  $(DRVNAME).drv

$(DRVNAME).res:    $(DRVNAME).rc $(DRVNAME).GPC $(FONTS) $(CTTS) $(RCV)
    rc -r -I. $(DRVNAME).RC

minidriv.obj:	    ..\minidriv.c
    cl -u -c -Asnw -PLM -G2sw -W3 -Oas -Zpe -DNODEVINSTALL -DNODEVMODE -I. ..\minidriv.c

$(DRVNAME).obj:     $(DRVNAME).c
    cl -u -c -Asnw -PLM -G2sw -W3 -Oas -Zpe -I. $(DRVNAME).c

$(DRVNAME).exe:    minidriv.obj $(DRVNAME).obj $(DRVNAME).res $(DRVNAME).def
    link @$(DRVNAME).lnk
    mapsym $(DRVNAME)

$(DRVNAME).drv:     $(DRVNAME).res $(DRVNAME).exe
    rc -30 -t -v -I. $(DRVNAME)
    copy $(DRVNAME).exe $(DRVNAME).drv


