%1
CD\CT\BCT\CP1
%2\Tilt.exe %3 %4 %5 %6 %7
if errorlevel 1 goto erred
del *.fil
copy tilt.log %8
%2\Return0.com
goto done
:erred
del *.fil
copy tilt.log %8
%2\Return1.com
:done
%2\ERRVALUE
