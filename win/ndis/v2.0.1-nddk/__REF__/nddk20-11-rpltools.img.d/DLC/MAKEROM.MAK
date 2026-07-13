#
#  Make File for the RIPL DLC Boot ROM
#

#
# General Macros
#
incs = includes.inc keys.inc macros.inc drvseg.inc ripl.inc message.inc

objs = romtop.obj rominit.obj bootfail.obj romdata.obj rommess.obj romvid.obj\
       romtim.obj romutil.obj ripl.obj romsend.obj romrcv.obj

objs2 = init.obj adapter.obj util.obj message.obj data.obj int.obj xmit.obj

# 
# Options Definitions
#
mopts = /ZD 

#
# Inference Rules
#
.asm.obj:
    masm $(mopts) $*;

#
#  Masm Dependencies
#
romtop.obj:  $(incs) romtop.asm

rominit.obj:  $(incs) rominit.asm

bootfail.obj:  $(incs) bootfail.asm

romdata.obj:  $(incs) romdata.asm

rommess.obj:  $(incs) rommess.asm

romvid.obj:  $(incs) romvid.asm

romtim.obj:  $(incs) romtim.asm

romsend.obj:  $(incs) romsend.asm

romrcv.obj:  $(incs) romrcv.asm

romutil.obj:  $(incs) romutil.asm

ripl.obj:  $(incs) ripl.asm

#
# Linker Dependencies
#
romdlc.bin: $(objs) $(objs2)
    link @romdlc.lnk 
