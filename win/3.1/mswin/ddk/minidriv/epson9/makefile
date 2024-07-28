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
DRVNAME = EPSON9

#-------------------------------------------------------------
# Enter the names of all of the resident font file names
# (with extensions) after FONTS =
#-------------------------------------------------------------
FONTS =      ROMIPS.PFM   ROMIPS7.PFM  ROMIPSDH.PFM ROMIPX.PFM   \
ROMIPX7.PFM  ROMN05.PFM   ROMN057.PFM  ROMN05DH.PFM ROMN06.PFM   \
ROMN067.PFM  ROMN06DH.PFM ROMN10.PFM   ROMN107.PFM  ROMN10DH.PFM \
ROMN12.PFM   ROMN127.PFM  ROMN12DH.PFM ROMN17.PFM   ROMN177.PFM  \
ROMN20.PFM   ROMN207.PFM  ROMNPS.PFM   ROMNPS7.PFM  ROMNPSDH.PFM \
ROMNPX.PFM   ROMNPX7.PFM  SANIPS.PFM   SANIPSDH.PFM SANIPX.PFM   \
SANS05.PFM   SANS05DH.PFM SANS06.PFM   SANS06DH.PFM SANS10.PFM   \
SANS10DH.PFM SANS12.PFM   SANS12DH.PFM SANS17.PFM   SANS20.PFM   \
SANSPS.PFM   SANSPSDH.PFM SANSPX.PFM

#-------------------------------------------------------------
# Enter the names of all of the version resource files
# (with extensions) after RCV =
#-------------------------------------------------------------
RCV =   EPSON9.RCV

#**********************************************************************
# Do not edit below this line
#**********************************************************************

target:  $(DRVNAME).drv

$(DRVNAME).res:  $(DRVNAME).rc $(DRVNAME).GPC $(FONTS) epsonxta.ctt $(RCV)
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