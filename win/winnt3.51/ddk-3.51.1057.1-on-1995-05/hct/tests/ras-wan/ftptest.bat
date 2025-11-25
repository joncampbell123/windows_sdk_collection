date<cr >> ipstress.log
time<cr >> ipstress.log
:top
ftp -s:scriptx
comp source_file new_file <n >> ipstress.log

time<cr >> ipstress.log

del new_file
goto top

:usage
ftptest ipstress.log
