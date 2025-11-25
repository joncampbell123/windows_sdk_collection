ALLDIRS = Win32 FrmWork OLE OpenGL RPC SDKTools MM MAPI Directx

all: $(ALLDIRS)

$(ALLDIRS):
        IF EXIST $@\nul <<nmaketmp.bat
		cd $@
		@nmake -a -i -nologo
		cd ..
		@if exist samples.log copy samples.log+*.tmp samples.log >nul
		@if not exist samples.log copy *.tmp samples.log >nul
		@del *.tmp >nul
<<
