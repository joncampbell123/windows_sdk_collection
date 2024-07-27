@echo off

REM nuke old libs
if exist %1libc%2w.lib del %1libc%2w.lib >NUL

if %2 == e lib %1libcew.lib+%1libcw.lib+libh.lib+%1libfpw.lib,,; >NUL

if %2 == a lib %1libcaw.lib+%1libcw.lib+libh.lib+%1libfaw.lib,,; >NUL

REM delete .baks
if exist %1libc%2w.bak del %1libc%2w.bak >NUL


REM invoke with mem model and math coprocessor:
REM libcw [s m c l] [a e]
