#############################################################################
#
#	Makefile for sample socket services device driver
#                                                                          
#############################################################################

sampless.vxd: sampless.obj
	link /VXD /NOD /DEF:sampless.def /OUT:sampless.vxd /MAP:sampless.map sampless.obj
	mapsym	-s -o SAMPLESS.sym SAMPLESS.map

sampless.obj: sampless.asm ssdefs.inc sslocal.inc
	ml -coff -DBLD_COFF -DIS_32 -W2 -Zd -c -Cx -DMASM6 -Sg sampless.asm
