rem make sure you've created %TempPath%\guijr port via printman
rem make sure you've closed printman

scripter GuiJr "/textout.ini /oPrinterd.ini /L /C /d"
call printGJR
parser GuiJr GJTest*.log