if (%1)==() goto end

REM goto ResultToFile

if errorlevel 4 goto Cant_Run
if errorlevel 3 goto Passed
if errorlevel 2 goto Failed
if errorlevel 1 goto Running
if errorlevel 0 goto Not_Run


:Cant_Run
	echo 4 > %1.smr 
	goto end
:Passed
	echo 3 > %1.smr 
	goto end
:Failed
	echo 2 > %1.smr 
	goto end
:Running
	echo 1 > %1.smr 
	goto end
:Not_Run
	echo 0 > %1.smr 
	goto end


:ResultToFile
if errorlevel 0 echo 0 > %1.smr
if errorlevel 1 echo 1 > %1.smr
if errorlevel 2 echo 2 > %1.smr
if errorlevel 3 echo 3 > %1.smr
if errorlevel 4 echo 4 > %1.smr 

:end

exit

