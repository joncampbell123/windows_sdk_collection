all: debug

debug : debug.mak
retail : retail.mak
clean : clean.mak

debug.mak retail.mak clean.mak:
        @echo ******************************
        @echo *** Building fastfile.lib ***
        @echo ******************************
        @nmake /nologo /f fastfile.vc $(@B)
        @echo ************************************
        @echo *** Building ffcreate.exe ***
        @echo ************************************
        @nmake /nologo /f ffcreate.vc $(@B)
