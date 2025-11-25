@echo off
@echo FNTMETS REGISTER Script
:* ----------------------------------------------------------------------------
:* RUN FNTMETS 
:*
:*
:*
:* ----------------------------------------------------------------------------


:SUITE
fntmets /o /z /s%PROCESSOR_ARCHITECTURE%2.txt /d%PROCESSOR_ARCHITECTURE%2.dif
fontlist -f"msft.ini" -a
fntmets /o /z /smsft2.txt /dmsft2.dif
call ft_run.bat
fontlist -f"msft.ini" -r
