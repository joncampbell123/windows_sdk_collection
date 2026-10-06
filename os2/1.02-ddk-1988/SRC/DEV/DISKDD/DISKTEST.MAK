#
# Copyright (C) Microsoft, 1988
#

# link version 5.1
LINK=link
# Masm version 5.1
MASM=masm

#        CREATE BINARY

disktest.obj: disktest.asm
	$(MASM) disktest;
	$(LINK) disktest;
