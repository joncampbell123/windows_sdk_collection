@echo *
@echo ***Batch File to build examples with WATCOM C/C++ 
@echo *
cd ddex1
wmake /f makefile.wat debug retail
cd ..\ddex2
wmake /f makefile.wat debug retail
cd ..\ddex3
wmake /f makefile.wat debug retail
cd ..\ddex4
wmake /f makefile.wat debug retail
cd ..\ddex5
wmake /f makefile.wat debug retail
cd ..\donut
wmake /f makefile.wat debug retail
cd ..\stretch
wmake /f makefile.wat debug retail
cd ..
