#
# app16.mak
#

proj1 = dll16
proj2 = app16

all : $(proj1).dll $(proj2).exe

#update the object files if necessary

$(proj1).obj : $(proj1).c
   cl /c /ASw -G2sw -Ow -W3 -Zp /Od /Oi /Zi $(proj1).c

$(proj2).obj : $(proj2).c
   cl -c -G2 -W3 -GA -GEf -Zp /Od /Oi /Zi $(proj2).c

# update the import library if necessary

$(proj1).lib : $(proj1).def
   implib $(proj1).lib $(proj1).def

# update the executable files if necessary

$(proj1).dll : $(proj1).obj
   link $(proj1),$(proj1).dll,nul,/NOE /COD /NOD:slibce sdllcew libw.lib w32sut16.lib,$(proj1).def
   rc -t $(proj1).dll

$(proj2).exe : $(proj2).obj $(proj2).def $(proj2).rc $(proj1).lib $(proj2).dlg
   rc -r $(proj2).rc
   link $(proj2).obj,$(proj2).exe,nul,libw.lib /COD /NOD:slibce slibcew $(proj1).lib,$(proj2).def
   rc $(proj2).res $(proj2).exe
