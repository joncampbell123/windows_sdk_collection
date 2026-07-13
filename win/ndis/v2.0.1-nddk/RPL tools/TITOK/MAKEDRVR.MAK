#
#  Make File for the RIPL Boot ROM Multi-Adapter Token-Ring 
#  Network Driver
#

#
# General Macros
#
incs = includes.inc keys.inc macros.inc adapter.inc drvseg.inc tmsstrct.inc\
       macstrct.inc message.inc ripl.inc


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
init.obj:  $(incs) init.asm

data.obj:  $(incs) data.asm

message.obj:  $(incs) message.asm

adapter.obj:  $(incs) adapter.asm

util.obj:  $(incs) util.asm

int.obj:  $(incs) int.asm

xmit.obj:  $(incs) xmit.asm

rcv.obj:  $(incs) rcv.asm
