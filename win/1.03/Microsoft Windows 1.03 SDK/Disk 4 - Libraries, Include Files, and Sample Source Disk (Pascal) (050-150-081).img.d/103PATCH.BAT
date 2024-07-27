rem ; e ds:1a5d c3; w; q
copy kernel.exe kernel.bak
debug kernel.bak <103patch.bat >nul
copy kernel.bak kernel.exe
