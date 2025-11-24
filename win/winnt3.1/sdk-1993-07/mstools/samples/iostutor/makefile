# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

.SUFFIXES:.cpp

all: exios101.exe \
     exios102.exe \
     exios103.exe \
     exios104.exe \
     exios105.exe \
     exios106.exe \
     exios107.exe \
     exios108.exe \
     exios109.exe \
     exios110.exe \
     exios111.exe \
     exios112.exe \
     exios113.exe \
     exios114.exe \
     exios115.exe \
     exios116.exe \
     exios117.exe \
     exios118.exe \
     exios119.exe \
     exios120.exe \
     exios121.exe \
     exios122.exe \
     exios201.exe \
     exios202.exe \
     exios203.exe 

.cpp.exe: 
    $(cc) $(cflags) $(cvars) $(cdebug) $*.cpp
    $(link) $(linkdebug) $(conflags) -out:$*.exe $*.obj $(conlibs)
