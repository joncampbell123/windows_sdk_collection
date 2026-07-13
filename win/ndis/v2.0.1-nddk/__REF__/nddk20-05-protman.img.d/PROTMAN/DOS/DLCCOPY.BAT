COPY %1\PRO.MSG %2
COPY %1\PROH.MSG %2

:progbegin
if exist %3\..\BASIC\*.* goto basic
:enhanced
COPY %1\NETBIND.EXE %3
COPY %1\PROTCHK.EXE %3
goto progend
:basic
COPY %1\NETBIND.EXE %3\..\BASIC
COPY %1\PROTCHK.EXE %3\..\BASIC
:progend
