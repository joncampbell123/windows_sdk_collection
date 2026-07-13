@echo off
call mkdos PRODUCT
if errorlevel 1 goto :end
call mkdos DEV
if errorlevel 1 goto :end
call mkos2 PRODUCT
if errorlevel 1 goto :end
call mkos2 DEV
:end
