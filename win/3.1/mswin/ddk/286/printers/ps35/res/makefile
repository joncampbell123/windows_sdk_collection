#---------------------------------------------------------------------------
# Makefile for PSCRIPT.DRV resources
#
# programs used:
#
#   SMASH.EXE   - compress postscript files (.ps -> .cps)
#       AFM.EXE     - fontmetrics resource builder (outputs filename.PFM)
#   MKPRN.EXE   - build printer descriptions from .PPD files
#
#---------------------------------------------------------------------------

!IFNDEF LANG
RES_DIR=.\messages\usa
!ELSE
RES_DIR=.\messages\$(LANG)
!ENDIF

INC    = ..
UTILS  = ..\UTILS


OPTS   = -I$(INC)
CPP    = cl $(OPTS) /EP
SMASH  = $(UTILS)\smash.exe
AFM    = $(UTILS)\afm.exe
MKPRN  = $(UTILS)\mkprn -s

.SUFFIXES: .ps .cps .afm .pfm .ppd .cap

#
# default rule: compress .ps file to .cps file
#

.ps.cps:
	$(SMASH) $*.ps $*.cps

CPS= header.cps dlprefix.cps dlsuffix.cps headchk.cps setcomm.cps      \
     ehandler.cps headeps.cps olivchst.cps rle.cps         \
     cimage.cps t3hdr.cps headti.cps

#
# default rule, use afmtopfm.dat as default data base
#

.afm.pfm:
	$(AFM) $*.afm afmtopfm.dat

PFM= agb.pfm agbo.pfm agd.pfm agdo.pfm bd.pfm bdi.pfm bl.pfm bli.pfm        \
     cb.pfm cbo.pfm cm.pfm co.pfm gb.pfm gbi.pfm gl.pfm gli.pfm hb.pfm      \
     hbo.pfm hc.pfm hcb.pfm hcbo.pfm hco.pfm hm.pfm ho.pfm hvbl.pfm         \
     hvblo.pfm hvl.pfm hvlo.pfm hvn.pfm hvnb.pfm hvnbo.pfm hvno.pfm krb.pfm \
     krkb.pfm krkx.pfm krkx.pfm krrg.pfm lb.pfm lbo.pfm ld.pfm ldo.pfm      \
     ncb.pfm ncbi.pfm nci.pfm ncr.pfm ob.pfm obo.pfm oo.pfm or.pfm pb.pfm   \
     pbi.pfm pi.pfm pr.pfm sd.pfm sdi.pfm sl.pfm sli.pfm sm.pfm tb.pfm      \
     tbi.pfm ti.pfm tr.pfm vtb.pfm vtbi.pfm vti.pfm vtr.pfm zc.pfm zd.pfm   \
     ar.pfm arb.pfm arbo.pfm aro.pfm arn.pfm arnb.pfm arnbo.pfm arno.pfm    \
     ntr.pfm ntb.pfm ntbi.pfm nti.pfm  vtb.pfm vtbi.pfm vti.pfm vtr.pfm     \
     zca.pfm zcab.pfm zcabi.pfm zcai.pfm

#
# printer capabilities and printer specific postscript code
#
# .pss .dir and .cap are all outputs from mkprn.exe
#
#.ppd.pss:
#.ppd.dir:

.ppd.cap:
	$(MKPRN) $*

CAP= agfap400.cap apple230.cap apple380.cap apple422.cap ast__470.cap   \
     datap462.cap decln03r.cap declps40.cap ibm20470.cap ibm30505.cap   \
     sheetfdr.cap lwnt_470.cap   \
     lwntx470.cap oliv5000.cap  oliv303.cap qms81470.cap                \
     qms8_461.cap qms8p461.cap qmscs494.cap ti08_450.cap ti15_470.cap   \
     vt600480.cap   wang15.cap wang15fp.cap  lc890xl.cap colormat.cap   \
     9000_493.cap 

#
# postscript files incorporated as uncompressed resources
#

PS= t1eftr1.ps t1eftr2.ps t1ehdr1.ps t1ehdr2.ps t1hdr1.ps t1hdr2.ps 

#------------------------------- dependancies -------------------------

#International mods
!IFNDEF LANG
resource:  pscript.res ps.res
!ELSE
resource: pscript.res
!ENDIF

#
# compressed postscript
#

header.cps: header.ps
	$(CPP) -D FULL_HEADER $*.ps > $*.i
	$(SMASH) $*.i $*.cps
   erase $*.i

dlprefix.cps: dlprefix.ps
	$(CPP) $*.ps > $*.i
	$(SMASH) $*.i $*.cps
   erase $*.i

cimage.cps: cimage.ps

rle.cps: rle.ps

dlsuffix.cps: dlsuffix.ps

setcomm.cps: setcomm.ps

headchk.cps: headchk.ps
	$(CPP) $*.ps > $*.i
	$(SMASH) $*.i $*.cps
   erase $*.i

# build headeps from header without defining FULL_HEADER

headeps.cps: header.ps
	$(CPP) header.ps > $*.i
	$(SMASH) $*.i $*.cps
   erase $*.i

# build headti from header with TRUEIMAGE_HEADER defined

headti.cps: header.ps
	$(CPP) -D FULL_HEADER -D TRUEIMAGE_HEADER header.ps > $*.i
	$(SMASH) $*.i headti.cps
   erase $*.i

software.cps: software.ps

hardware.cps: hardware.ps

olivchst.cps: olivchst.ps

ehandler.cps: ehandler.ps

t3hdr.cps: t3hdr.ps

#
# resourcable fontmetrics
# 
# special build for TrueType metrics -- the fonts use the same
# metrics as PostScript fonts but with different names
#

ar.pfm: hm.afm
	copy truetype.afm+hm.afm ar.afm
	$(AFM) ar.afm tiafmpfm.dat
	erase ar.afm

arb.pfm: hb.afm
	copy truetype.afm+hb.afm arb.afm
	$(AFM) arb.afm tiafmpfm.dat
	erase arb.afm

arbo.pfm: hbo.afm
	copy truetype.afm+hbo.afm arbo.afm
	$(AFM) arbo.afm tiafmpfm.dat
	erase arbo.afm

aro.pfm: ho.afm
	copy truetype.afm+ho.afm aro.afm
	$(AFM) aro.afm tiafmpfm.dat
	erase aro.afm

arn.pfm: hvn.afm
	copy truetype.afm+hvn.afm arn.afm
	$(AFM) arn.afm tiafmpfm.dat
	erase arn.afm

arnb.pfm: hvnb.afm
	copy truetype.afm+hvnb.afm arnb.afm
	$(AFM) arnb.afm tiafmpfm.dat
	erase arnb.afm

arnbo.pfm: hvnbo.afm
	copy truetype.afm+hvnbo.afm arnbo.afm
	$(AFM) arnbo.afm tiafmpfm.dat
	erase arnbo.afm

arno.pfm: hvno.afm
	copy truetype.afm+hvno.afm arno.afm
	$(AFM) arno.afm tiafmpfm.dat
	erase arno.afm

ntb.pfm: tb.afm
	copy truetype.afm+tb.afm ntb.afm
	$(AFM) ntb.afm tiafmpfm.dat
	erase ntb.afm

ntbi.pfm: tbi.afm
	copy truetype.afm+tbi.afm ntbi.afm
	$(AFM) ntbi.afm tiafmpfm.dat
	erase ntbi.afm

nti.pfm: ti.afm
	copy truetype.afm+ti.afm nti.afm
	$(AFM) nti.afm tiafmpfm.dat
	erase nti.afm

ntr.pfm: tr.afm
	copy truetype.afm+tr.afm ntr.afm
	$(AFM) ntr.afm tiafmpfm.dat
	erase ntr.afm

zca.pfm: pr.afm
	copy truetype.afm+pr.afm zca.afm
	$(AFM) zca.afm tiafmpfm.dat
	erase zca.afm

zcab.pfm: pb.afm
	copy truetype.afm+pb.afm zcab.afm
	$(AFM) zcab.afm tiafmpfm.dat
	erase zcab.afm

zcabi.pfm: pbi.afm
	copy truetype.afm+pbi.afm zcabi.afm
	$(AFM) zcabi.afm tiafmpfm.dat
	erase zcabi.afm

zcai.pfm: pi.afm
	copy truetype.afm+pi.afm zcai.afm
	$(AFM) zcai.afm tiafmpfm.dat
	erase zcai.afm

#
# resourcable fontmetrics
# 

agb.pfm: agb.afm

agbo.pfm: agbo.afm

agd.pfm: agd.afm

agdo.pfm: agdo.afm

bd.pfm: bd.afm

bdi.pfm: bdi.afm

bl.pfm: bl.afm

bli.pfm: bli.afm

cb.pfm: cb.afm

cbo.pfm: cbo.afm

cm.pfm: cm.afm

co.pfm: co.afm

gb.pfm: gb.afm

gbi.pfm: gbi.afm

gl.pfm: gl.afm

gli.pfm: gli.afm

hb.pfm: hb.afm

hbo.pfm: hbo.afm

hc.pfm: hc.afm

hcb.pfm: hcb.afm

hcbo.pfm: hcbo.afm

hco.pfm: hco.afm

hm.pfm: hm.afm

ho.pfm: ho.afm

hvbl.pfm: hvbl.afm

hvblo.pfm: hvblo.afm

hvl.pfm: hvl.afm

hvlo.pfm: hvlo.afm

hvn.pfm: hvn.afm

hvnb.pfm: hvnb.afm

hvnbo.pfm: hvnbo.afm

hvno.pfm: hvno.afm

krb.pfm: krb.afm

krkb.pfm: krkb.afm

krkx.pfm: krkx.afm

krrg.pfm: krrg.afm

lb.pfm: lb.afm

lbo.pfm: lbo.afm

ld.pfm: ld.afm

ldo.pfm: ldo.afm

ncb.pfm: ncb.afm

ncbi.pfm: ncbi.afm

nci.pfm: nci.afm

ncr.pfm: ncr.afm

ob.pfm: ob.afm

obo.pfm: obo.afm

oo.pfm: oo.afm

or.pfm: or.afm

pb.pfm: pb.afm

pbi.pfm: pbi.afm

pi.pfm: pi.afm

pr.pfm: pr.afm

sd.pfm: sd.afm

sdi.pfm: sdi.afm

sl.pfm: sl.afm

sli.pfm: sli.afm

sm.pfm: sm.afm

tb.pfm: tb.afm

tbi.pfm: tbi.afm

ti.pfm: ti.afm

tr.pfm: tr.afm

vtb.pfm: vtb.afm

vtbi.pfm: vtbi.afm

vti.pfm: vti.afm

vtr.pfm: vtr.afm

zc.pfm: zc.afm

zd.pfm: zd.afm

#
# printer capabilities (.CAP), postscript code (.PSS) and font dir (.DIR)
#

agfap400.cap: agfap400.ppd

apple230.cap: apple230.ppd

apple380.cap: apple380.ppd

apple422.cap: apple422.ppd

ast__470.cap: ast__470.ppd

datap462.cap: datap462.ppd

decln03r.cap: decln03r.ppd

declps40.cap: declps40.ppd

ibm20470.cap: ibm20470.ppd

ibm30505.cap: ibm30505.ppd

sheetfdr.cap: sheetfdr.ppd

lwnt_470.cap: lwnt_470.ppd

lwntx470.cap: lwntx470.ppd

oliv5000.cap: oliv5000.ppd

oliv303.cap: oliv303.ppd

qms81470.cap: qms81470.ppd

qms8_461.cap: qms8_461.ppd

qms8p461.cap: qms8p461.ppd

qmscs494.cap: qmscs494.ppd

qume_470.cap: qume_470.ppd

ti08_450.cap: ti08_450.ppd

ti15_470.cap: ti15_470.ppd

vt600480.cap: vt600480.ppd

wang15.cap: wang15.ppd

wang15fp.cap: wang15fp.ppd

lc890xl.cap: lc890xl.ppd

colormat.cap: colormat.ppd

9000_493.cap: 9000_493.ppd



!IFNDEF LANG
# pscript.res is dependant on the following:
#    *.pfm *.cps *.dta *.cap *.pss *.dir
#
# include one of each of these files in the dependancy list to make this go
#

pscript.res: $(RES_DIR)\pscript.rc $(RES_DIR)\ps.dlg $(RES_DIR)\pscript.rcv \
             printcap.rc fonts.rc papersiz.rc printcap.rc    \
             $(INC)\win.h $(INC)\resource.h $(INC)\printers.h           \
             $(INC)\psoption.h $(INC)\dmrc.h $(INC)\pserrors.h          \
             $(INC)\psver.h                                             \
             $(CPS) $(PFM) $(CAP) $(PS)
    rc -r -I$(RES_DIR) -I.. -Fopscript.res $(RES_DIR)\pscript.rc


# we build this resource file so we can edit the dialog boxes with
# the dialog editor (it can't read pscript.res, it is too big)

ps.res: $(RES_DIR)\ps.rc $(RES_DIR)\ps.dlg $(INC)\resource.h \
 $(INC)\dmrc.h
    rc -r -I$(RES_DIR) -I.. -Fops.res $(RES_DIR)\ps.rc

!ELSE
pscript.res: $(RES_DIR)\$@
    copy $(RES_DIR)\$@
!ENDIF
