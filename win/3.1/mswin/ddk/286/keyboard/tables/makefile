DLL1 = kbdbe.dll kbdca.dll kbdfc.dll kbdda.dll kbdic.dll kbdfr.dll  kbdsw.dll
DLL2 = kbdfi.dll kbdgr.dll kbdit.dll kbdla.dll kbdne.dll kbdno.dll  kbdpo.dll
DLL3 = kbdsf.dll kbdsg.dll kbdsp.dll kbdus.dll kbddv.dll kbduk.dll kbdusx.dll
BINS = xlat850.bin xlat860.bin xlat861.bin xlat863.bin xlatnord.bin xlat865.bin

target:  $(DLL1) $(DLL2) $(DLL3) $(BINS)

# make keyboard table DLL's.

######## main routine for patching tables and copying stuff to driver #####

# All but US version -- X1/X2 swap for AT keyboard.

getkbd.obj: getkbd.asm ..\date.inc
    masm -DX1X2 -I..\ getkbd;

# USA driver -- no X1/X2 swap

getkbdus.obj: getkbd.asm ..\date.inc
    masm -I..\ getkbd,getkbdus;

######## individual table files. ########

# each one contains tables for several keyboards for one nationality.

INCS=trans.inc ..\keyboard.inc ..\vkoem.inc ..\vkwin.inc
ASM=masm -I.\ -I..\

# Belgian
kbdbe.obj: kbdbe.asm $(INCS)
	$(ASM) kbdbe;

# Canadian French
kbdca.obj: kbdca.asm $(INCS)
	$(ASM) kbdca;

# Canadian Multilingual
kbdfc.obj: kbdfc.asm $(INCS)
        $(ASM) kbdfc;

# Danish
kbdda.obj: kbdda.asm $(INCS)
	$(ASM) kbdda;

# Icelandic
kbdic.obj: kbdic.asm $(INCS)
	$(ASM) kbdic;

# Espana -- Olivetti Spain II Spanish/Catalan
# (no longer in 3.0)
#kbdes.obj: kbdes.asm $(INCS)
#	$(ASM) kbdes;

# French
kbdfr.obj: kbdfr.asm $(INCS)
	$(ASM) kbdfr;

# Finnish-Swedish
kbdfs.obj: kbdfs.asm $(INCS)
	$(ASM) kbdfs;

# German
kbdgr.obj: kbdgr.asm $(INCS)
	$(ASM) kbdgr;

# Italian
kbdit.obj: kbdit.asm $(INCS)
	$(ASM) kbdit;

# Latin American
kbdla.obj: kbdla.asm $(INCS)
	$(ASM) kbdla;

# Netherlands
kbdne.obj: kbdne.asm $(INCS)
	$(ASM) kbdne;

# Norway
kbdno.obj: kbdno.asm $(INCS)
	$(ASM) kbdno;

# Portugal
kbdpo.obj: kbdpo.asm $(INCS)
	$(ASM) kbdpo;

# Swiss-French
kbdsf.obj: kbdsf.asm $(INCS)
	$(ASM) kbdsf;

### Swiss-German requires flag set for special table
kbdsg.obj: kbdsg.asm $(INCS)
	$(ASM) -DSGCAPS kbdsg;

# Spanish (standard)
kbdsp.obj: kbdsp.asm $(INCS)
	$(ASM) kbdsp;

# USA
kbdus.obj: kbdus.asm $(INCS)
	$(ASM) kbdus;

# USA -- extended for international testing
kbdusx.obj: kbdusx.asm $(INCS)
	$(ASM) kbdusx;

# USA Dvorak
kbddv.obj: kbddv.asm $(INCS)
	$(ASM) kbddv;

# United Kingdom
kbduk.obj: kbduk.asm $(INCS)
	$(ASM) kbduk;


######## Oem/Ansi translation tables for .BIN files ########

# Code Page 850 OEM set, use for NO/DA now.
xlat850.bin: xlat850.asm ..\xlat850.inc ..\date.inc
	masm -DEXTENDEDANSI -I..\ xlat850;
	link xlat850;
	exe2bin xlat850
	del xlat850.obj
	del xlat850.exe

# Portugese PC OEM set, code page 860.
xlat860.bin: xlat860.asm ..\xlatpo.inc ..\date.inc
	masm -DEXTENDEDANSI -I..\ xlat860;
	link xlat860;
	exe2bin xlat860
	del xlat860.obj
	del xlat860.exe

# Icelandic PC OEM set, code page 861.
xlat861.bin: xlat861.asm ..\xlatic.inc ..\date.inc
	masm -DEXTENDEDANSI -I..\ xlat861;
	link xlat861;
	exe2bin xlat861
	del xlat861.obj
	del xlat861.exe

# French Canadian OEM set, code page 863.
xlat863.bin: xlat863.asm ..\xlatca.inc ..\date.inc
	masm -DEXTENDEDANSI -I..\ xlat863;
	link xlat863;
	exe2bin xlat863
	del xlat863.obj
	del xlat863.exe

# Nordic PC OEM set, code page 865 fo old olivetti machines.
xlatnord.bin: xlat865.asm ..\xlatno.inc ..\date.inc
	masm -DEXTENDEDANSI -I..\ xlat865;
	link xlat865;
	exe2bin xlat865.exe xlatnord.bin
	del xlat865.obj
	del xlat865.exe

# Nordic PC OEM set, code page 865 correct.
xlat865.bin: xlat865c.asm ..\xlat865c.inc ..\date.inc
	masm -DEXTENDEDANSI -I..\ xlat865c;
	link xlat865c;
	exe2bin xlat865c.exe xlat865.bin
	del xlat865c.obj
	del xlat865c.exe

# Olivetti Spanish PC OEM set, code page 220 (private Olivetti number).
# (no longer in 3.0)
#xlates.bin: xlates.asm ..\xlates.inc ..\date.inc
#	masm -I..\ xlates;
#	link xlates;
#	exe2bin xlates
#	del xlates.obj
#	del xlates.exe

# normal PC OEM set.
#xlatus.obj: xlatus.asm ..\xlatus.inc
#	masm -I..\ xlatus;

########### Assemble the key-name tables, for various DLL's #############
#           Not all DLL's have a GetKeyString() function.

# US/UK: English
namesus.obj: names.asm namesus.inc trans.inc
	command /c copy namesus.inc names.inc
	$(ASM) names, namesus;
	command /c del names.inc

# IT: Italian
namesit.obj: names.asm namesit.inc trans.inc
	command /c copy namesit.inc names.inc
	$(ASM) names, namesit;
	command /c del names.inc

# SP: Spanish (SP, ES, LA)
namessp.obj: names.asm namessp.inc trans.inc
	command /c copy namessp.inc names.inc
	$(ASM) names, namessp;
	command /c del names.inc

# FR: French (FR, SF, CA)
namesfr.obj: names.asm namesfr.inc trans.inc
	command /c copy namesfr.inc names.inc
	$(ASM) names, namesfr;
	command /c del names.inc

# GR: German (GR, SG)
namesgr.obj: names.asm namesgr.inc trans.inc
	command /c copy namesgr.inc names.inc
	$(ASM) names, namesgr;
	command /c del names.inc

# NE: Dutch key names
namesdu.obj: names.asm namesdu.inc trans.inc
	command /c copy namesdu.inc names.inc
	$(ASM) names, namesdu;
	command /c del names.inc

# NO: Norwegian key names
namesno.obj: names.asm namesno.inc trans.inc
	command /c copy namesno.inc names.inc
	$(ASM) names, namesno;
	command /c del names.inc

# SW: Swedish key names
namessw.obj: names.asm namessw.inc trans.inc
	command /c copy namessw.inc names.inc
	$(ASM) names, namessw;
	command /c del names.inc

# FI: FINNISH key names
namesfi.obj: names.asm namesfi.inc trans.inc
	command /c copy namesfi.inc names.inc
	$(ASM) names, namesfi;
	command /c del names.inc

# DA: Danish key names
namesda.obj: names.asm namesda.inc trans.inc
	command /c copy namesda.inc names.inc
	$(ASM) names, namesda;
	command /c del names.inc

# PO: Portugese key names
namespo.obj: names.asm namespo.inc trans.inc
	command /c copy namespo.inc names.inc
	$(ASM) names, namespo;
	command /c del names.inc


########### Link all the DLL's #############

# The xlatXX.obj file is xlatUS.obj except for indicated countries.

# Link Belgian DLL

kbdbe.dll: kbdbe.def getkbd.obj kbdbe.obj namesfr.obj
    link getkbd kbdbe namesfr, kbdbe.exe/align:16,kbdbe/map, \
	sdllcew libw /NOD,kbdbe.def
    rc kbdbe.exe
    mapsym kbdbe
    command /c copy kbdbe.exe kbdbe.dll
    command /c del kbdbe.exe

# Link Canadian French DLL
# Note CA OEM character set

kbdca.dll: kbdca.def getkbd.obj kbdca.obj namesfr.obj
    link getkbd kbdca namesfr, kbdca.exe/align:16,kbdca/map, \
	sdllcew libw /NOD,kbdca.def
    rc kbdca.exe
    mapsym kbdca
    command /c copy kbdca.exe kbdca.dll
    command /c del kbdca.exe

# Link Canadian Multilingual DLL
# Note CA OEM character set

kbdfc.dll: kbdfc.def getkbd.obj kbdfc.obj namesfr.obj
    link getkbd kbdfc namesfr, kbdfc.exe/align:16,kbdfc/map, \
        sdllcew libw /NOD,kbdfc.def
    rc kbdfc.exe
    mapsym kbdfc
    command /c copy kbdfc.exe kbdfc.dll
    command /c del kbdfc.exe

# Link Danish DLL
# Note NO OEM character set

kbdda.dll: kbdda.def getkbd.obj kbdda.obj  namesda.obj
    link getkbd kbdda namesda, kbdda.exe/align:16,kbdda/map, \
	sdllcew libw /NOD,kbdda.def
    rc kbdda.exe
    mapsym kbdda
    command /c copy kbdda.exe kbdda.dll
    command /c del kbdda.exe

# Link Icelandic DLL
# Note IC OEM character set

kbdic.dll: kbdic.def getkbd.obj kbdic.obj
    link getkbd kbdic, kbdic.exe/align:16,kbdic/map, \
	sdllcew libw /NOD,kbdic.def
    rc kbdic.exe
    mapsym kbdic
    command /c copy kbdic.exe kbdic.dll
    command /c del kbdic.exe

# Link Espana DLL -- Olivetti Spain II Spanish/Catalan
# Note ES OEM character set
# (no longer in 3.0)
#kbdes.dll: kbdes.def getkbd.obj kbdes.obj namessp.obj
#    link getkbd kbdes namessp, kbdes.exe/align:16,kbdes/map, \
#	sdllcew libw /NOD,kbdes.def
#    rc kbdes.exe
#    mapsym kbdes
#    command /c copy kbdes.exe kbdes.dll
#    command /c del kbdes.exe

# Link French DLL

kbdfr.dll: kbdfr.def getkbd.obj kbdfr.obj namesfr.obj
    link getkbd kbdfr namesfr, kbdfr.exe/align:16,kbdfr/map, \
	sdllcew libw /NOD,kbdfr.def
    rc kbdfr.exe
    mapsym kbdfr
    command /c copy kbdfr.exe kbdfr.dll
    command /c del kbdfr.exe

# Link Swedish DLL

kbdsw.dll: kbdsw.def getkbd.obj kbdfs.obj namessw.obj
    link getkbd kbdfs namessw, kbdsw.exe/align:16,kbdsw/map, \
	sdllcew libw /NOD,kbdsw.def
    rc kbdsw.exe
    mapsym kbdsw
    command /c copy kbdsw.exe kbdsw.dll
    command /c del kbdsw.exe

# Link Finnish DLL

# ADD NAMESFI when it exists!

kbdfi.dll: kbdfi.def getkbd.obj kbdfs.obj namesfi.obj
    link getkbd kbdfs namesfi, kbdfi.exe/align:16,kbdfi/map, \
	sdllcew libw /NOD,kbdfi.def
    rc kbdfi.exe
    mapsym kbdfi
    command /c copy kbdfi.exe kbdfi.dll
    command /c del kbdfi.exe

# Link German DLL

kbdgr.dll: kbdgr.def getkbd.obj kbdgr.obj namesgr.obj
    link getkbd kbdgr namesgr, kbdgr.exe/align:16,kbdgr/map, \
	sdllcew libw /NOD,kbdgr.def
    rc kbdgr.exe
    mapsym kbdgr
    command /c copy kbdgr.exe kbdgr.dll
    command /c del kbdgr.exe

# Link Italian DLL

kbdit.dll: kbdit.def getkbd.obj kbdit.obj namesit.obj
    link getkbd kbdit namesit, kbdit.exe/align:16,kbdit/map, \
	sdllcew libw /NOD,kbdit.def
    rc kbdit.exe
    mapsym kbdit
    command /c copy kbdit.exe kbdit.dll
    command /c del kbdit.exe

# Link Latin American DLL

kbdla.dll: kbdla.def getkbd.obj kbdla.obj namessp.obj
    link getkbd kbdla namessp, kbdla.exe/align:16,kbdla/map, \
	sdllcew libw /NOD,kbdla.def
    rc kbdla.exe
    mapsym kbdla
    command /c copy kbdla.exe kbdla.dll
    command /c del kbdla.exe

# Link Netherlands DLL

kbdne.dll: kbdne.def getkbd.obj kbdne.obj  namesdu.obj
    link getkbd kbdne namesdu, kbdne.exe/align:16,kbdne/map, \
	sdllcew libw /NOD,kbdne.def
    rc kbdne.exe
    mapsym kbdne
    command /c copy kbdne.exe kbdne.dll
    command /c del kbdne.exe

# Link Norway DLL
# Note NO OEM character set

kbdno.dll: kbdno.def getkbd.obj kbdno.obj namesno.obj
    link getkbd kbdno namesno, kbdno.exe/align:16,kbdno/map, \
	sdllcew libw /NOD,kbdno.def
    rc kbdno.exe
    mapsym kbdno
    command /c copy kbdno.exe kbdno.dll
    command /c del kbdno.exe

# Link Portugal DLL
# Note PO OEM character set

kbdpo.dll: kbdpo.def getkbd.obj kbdpo.obj namespo.obj
    link getkbd kbdpo namespo, kbdpo.exe/align:16,kbdpo/map, \
	sdllcew libw /NOD,kbdpo.def
    rc kbdpo.exe
    mapsym kbdpo
    command /c copy kbdpo.exe kbdpo.dll
    command /c del kbdpo.exe

# Link Swiss-French DLL

kbdsf.dll: kbdsf.def getkbd.obj kbdsf.obj namesfr.obj 
    link getkbd kbdsf namesfr, kbdsf.exe/align:16,kbdsf/map, \
	sdllcew libw /NOD,kbdsf.def
    rc kbdsf.exe
    mapsym kbdsf
    command /c copy kbdsf.exe kbdsf.dll
    command /c del kbdsf.exe


# Link Swiss-German DLL.

kbdsg.dll: kbdsg.def getkbd.obj kbdsg.obj 
    link getkbd kbdsg, kbdsg.exe/align:16,kbdsg/map, \
	sdllcew libw /NOD,kbdsg.def
    rc kbdsg.exe
    mapsym kbdsg
    command /c copy kbdsg.exe kbdsg.dll
    command /c del kbdsg.exe

# Spanish (standard) DLL

kbdsp.dll: kbdsp.def getkbd.obj kbdsp.obj namessp.obj
    link getkbd kbdsp namessp, kbdsp.exe/align:16,kbdsp/map, \
	sdllcew libw /NOD,kbdsp.def
    rc kbdsp.exe
    mapsym kbdsp
    command /c copy kbdsp.exe kbdsp.dll
    command /c del kbdsp.exe


# Link USA DLL. Note unique version of GETKBD.OBJ !!

kbdus.dll: kbdus.def getkbdus.obj kbdus.obj 
    link getkbdus kbdus, kbdus.exe/align:16,kbdus/map, \
	sdllcew libw /NOD,kbdus.def
    rc kbdus.exe
    mapsym kbdus
    command /c copy kbdus.exe kbdus.dll
    command /c del kbdus.exe

# Link USA special international DLL. For enhanced keyboard only.

kbdusx.dll: kbdusx.def getkbdus.obj kbdusx.obj 
    link getkbdus kbdusx, kbdusx.exe/align:16,kbdusx/map, \
	sdllcew libw /NOD,kbdusx.def
    rc kbdusx.exe
    mapsym kbdusx
    command /c copy kbdusx.exe kbdusx.dll
    command /c del kbdusx.exe


# Link USA Dvorak DLL. Note unique version of GETKBD.OBJ !!

kbddv.dll: kbddv.def getkbdus.obj kbddv.obj 
    link getkbdus kbddv, kbddv.exe/align:16,kbddv/map, \
	sdllcew libw /NOD,kbddv.def
    rc kbddv.exe
    mapsym kbddv
    command /c copy kbddv.exe kbddv.dll
    command /c del kbddv.exe


# Link U.K. DLL.

kbduk.dll: kbduk.def getkbd.obj kbduk.obj 
    link getkbd kbduk, kbduk.exe/align:16,kbduk/map, \
	sdllcew libw /NOD,kbduk.def
    rc kbduk.exe
    mapsym kbduk
    command /c copy kbduk.exe kbduk.dll
    command /c del kbduk.exe
