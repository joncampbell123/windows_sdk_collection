echo off
Rem Batch script to build base executables.

IF "%1" == "" goto USAGE

call %1\tools\env %1

cd %1\bsrc\src\dev\atcom
make 

cd %1\bsrc\src\dev\ega
make 

cd %1\bsrc\src\dev\point
make 

cd %1\bsrc\src\dev\basedd
call bld.cmd

cd %1\bsrc\src\vdh
make

cd %1\bsrc\src\dev\mouse\familyms
make

cd %1\bsrc\src\dev\xlatetbl\viotbl
make

cd %1\bsrc\src\dev\xlatetbl\kbdtbl
make

cd %1\bsrc\src\dev\disktest\asm
make

cd %1\bsrc\src\dev\disktest\c
make

cd %1\bsrc\src\dev\disk01
make

goto EXIT

:USAGE
 
echo Usage:   BLDBASE dir
echo                  dir is the parent directory of bsrc
echo Example: BLDBASE C:\DDK12

:EXIT
echo on
