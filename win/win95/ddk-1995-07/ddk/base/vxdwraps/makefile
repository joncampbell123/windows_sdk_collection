#****************************************************************************
#                                                                           *
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
# PURPOSE.                                                                  *
#                                                                           *
# Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
#                                                                           *
#****************************************************************************

DIRLIST = _ios _vxdldr _vpowerd _shell _vmmreg _vcomm _vmm _configmg

target: $(DIRLIST) 

$(DIRLIST):
        cd $(@:_=)
        nmake
        cd ..

clean: $(DIRLIST:_=)

$(DIRLIST:_=):
        del $@\*.obj
        
