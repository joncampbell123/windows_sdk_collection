@echo off

REM nuke old libs
if exist %1dllc%2w.lib del %1dllc%2w.lib >NUL

REM Copy Windows lib to temp lib
copy %1libc%2w.lib %1tmpc%2w.lib >NUL

REM Remove exe-specific modules in Windows lib 
lib %1tmpc%2w.lib @wincdll.bld >NUL

REM Create DLL lib
lib %1dllc%2w.lib+%1dllcw0.lib+%1tmpc%2w.lib; >NUL

REM delete .baks
del %1tmpc%2w.lib >NUL
if exist %1tmpc%2w.bak del %1tmpc%2w.bak >NUL
if exist %1dllc%2w.bak del %1dllc%2w.bak >NUL

REM invoke with mem model and math coprocessor:
REM dllcw [s m c l] [a e]




