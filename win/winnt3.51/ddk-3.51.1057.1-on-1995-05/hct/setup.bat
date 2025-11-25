@echo off

@rem
@rem Windows NT 3.1 July Release HCT - NDIS Only Setup
@rem
@rem USE SETUP.EXE to install System, Storage, MM, Printer, Docs.
@rem USE install.bat from \hct\tests\ft to install Fault Tolerance Tests
@rem
@rem setup.bat - install HCTs from floppy under DOS
@rem
@rem 8/9/94: just net from setup.bat now

        echo.
        echo Windows NT 3.5 HCT Installation  [Release 3.5]
        echo -----------------------------------------------
        echo USE SETUP.EXE to install System, Storage, MM, Printer, Docs.
        echo USE install.bat from \hct\tests\ft to install Fault Tolerance Tests


        set _nodoc=0

        if "%1"=="" goto usage

        //peteg 3/20/94 add default root drive setup checks
        //this covers drives c: - h:

        if "%1"=="c:" goto Cdefault
        if "%1"=="c:\" goto Cdefault
        if "%1"=="C:" goto Cdefault
        if "%1"=="C:\" goto Cdefault

        if "%1"=="d:" goto Ddefault
        if "%1"=="d:\" goto Ddefault
        if "%1"=="D:" goto Ddefault
        if "%1"=="D:\" goto Ddefault

        if "%1"=="e:" goto Edefault
        if "%1"=="e:\" goto Edefault
        if "%1"=="E:" goto Edefault
        if "%1"=="E:\" goto Edefault

        if "%1"=="f:" goto Fdefault
        if "%1"=="f:\" goto Fdefault
        if "%1"=="F:" goto Fdefault
        if "%1"=="F:\" goto Fdefault

        if "%1"=="g:" goto Gdefault
        if "%1"=="g:\" goto Gdefault
        if "%1"=="G:" goto Gdefault
        if "%1"=="G:\" goto Gdefault

        if "%1"=="h:" goto Hdefault
        if "%1"=="h:\" goto Hdefault
        if "%1"=="H:" goto Hdefault
        if "%1"=="H:\" goto Hdefault

        if "%1"=="/?" goto usage
        if "%1"=="/h" goto usage
        if "%1"=="/H" goto usage

        //none of these, specified path, goto set the target
        goto settgt

:Cdefault
        set _tgt=c:\hct
        goto skiptgt
:Ddefault
        set _tgt=d:\hct
        goto skiptgt
:Edefault
        set _tgt=e:\hct
        goto skiptgt
:Fdefault
        set _tgt=f:\hct
        goto skiptgt
:Gdefault
        set _tgt=g:\hct
        goto skiptgt
:Hdefault
        set _tgt=h:\hct
        goto skiptgt

	@REM _ndis_option for NDIS media option - default to ethernet

	set _ndis_option=eth

        echo.
        echo Parsing arguments...
        echo.

:settgt

        set _tgt=%1

:skiptgt

        shift

        rem peteg 3/21/94 move this stuff here so it won't get skipped.
        set _cpu=x86
        if "%PROCESSOR_ARCHITECTURE%"=="MIPS"  set _cpu=mips
        if "%PROCESSOR_ARCHITECTURE%"=="ALPHA" set _cpu=alpha
        set _nuke=
        set _option=

:loop
        if "%1"=="mips" set _cpu=mips
        if "%1"=="MIPS" set _cpu=mips

        if "%1"=="alpha" set _cpu=alpha
        if "%1"=="ALPHA" set _cpu=alpha

        if "%1"=="-d" set _nuke=yes

rem        if "%1"=="mmedia" set _option=mmedia
rem        if "%1"=="MMEDIA" set _option=mmedia

        if "%1"=="ndis" set _option=ndis
        if "%1"=="NDIS" set _option=ndis
rem 
rem Remmed this line out to not set by default to install
rem RAS-WAN if desired
rem
rem     set _option=ndis

	if "%1"=="RAS" set _option=ras
	if "%1"=="ras" set _option=ras


rem        if "%1"=="printer" set _option=printer
rem        if "%1"=="PRINTER" set _option=printer

rem        if "%1"=="storage" set _option=storage
rem        if "%1"=="STORAGE" set _option=storage

rem        if "%1"=="video" set _option=video
rem        if "%1"=="VIDEO" set _option=video

        rem peteg 3/24/94: add nodoc option to not install docs
        if "%1"=="-nodoc" set _nodoc=1
        if "%1"=="-NODOC" set _nodoc=1

	@rem _ndis_option parsing

	if "%1"=="eth" set _ndis_option=eth
	if "%1"=="ETH" set _ndis_option=eth

        if "%1"=="fddi" set _ndis_option=fddi
	if "%1"=="FDDI" set _ndis_option=fddi

	if "%1"=="tr" set _ndis_option=tr
        if "%1"=="TR" set _ndis_option=tr

        if "%1"=="" goto start

        shift
        goto loop

:start
rem peteg: removed checks for valid drive letter, since using directory now
rem        if "%_tgt%"=="" goto usage

rem  	for %%i in (c d e f g h i j k l m n o p q r s t u v w x y z) do if "%_tgt%"=="%%i:" goto goodstart
rem       	echo Be sure to specify a valid drive letter\path (e.g c:\hcttests).
rem        default path will be driveletter:\hct
rem        goto usage

:goodstart
        echo Installing %_cpu% binaries
        echo.

        if "%_nuke%"=="yes" goto nukehct

        goto install

:nukehct
        echo Deleting %_tgt%\...
        echo.
        pause
        echo.
        echo    === Ignore the following error if there is one ===
        echo.
        .\bin\dos\delnode /q %_tgt%\
        .\bin\%_cpu%\delnode /q %_tgt%\

:install
        
        @REM selective installs for DDK
 
        if "%_option%"=="mmedia" goto mmedia
        if "%_option%"=="ndis" goto ndis
        if "%_option%"=="ras" goto ras      
        if "%_option%"=="printer" goto printer
        if "%_option%"=="storage" goto storage
        if "%_option%"=="video" goto video
        
        @rem
        @rem for each component, copy generic cmd files and cpu-specific bins
        @rem
        @rem CDROM ARC test and NDIS test don't run or install under HCTs
        @rem Printer isn't installed for systems either

        echo.
        echo Copying files to %_tgt%\
        echo.

        md %_tgt%
        md %_tgt%\logs

        xcopy bin\dos                   %_tgt%\bin\dos\ /S

        xcopy bin\%_cpu%                %_tgt%\bin /S
        copy bin\*.*                    %_tgt%\bin

        xcopy prodline                  %_tgt%\prodline\ /S

        if "%_nodoc%"=="1" goto skipdoc
        xcopy doc                       %_tgt%\doc\ /S
 :skipdoc

        xcopy tests\disk\%_cpu%         %_tgt%\tests\disk\ /S
        copy tests\disk\*.*             %_tgt%\tests\disk
	xcopy tests\disk\12		%_tgt%\tests\disk\12\ /S
	xcopy tests\disk\144		%_tgt%\tests\disk\144\ /S
	xcopy tests\disk\288		%_tgt%\tests\disk\288\ /S

        md %_tgt%\tests\cdrom
        copy tests\cdrom\*.*            %_tgt%\tests\cdrom

        xcopy tests\kbd\%_cpu%          %_tgt%\tests\kbd\ /S
        copy tests\kbd\*.*              %_tgt%\tests\kbd

        xcopy tests\memory\%_cpu%       %_tgt%\tests\memory\ /S
        copy tests\memory\*.*           %_tgt%\tests\memory

        xcopy tests\mm\%_cpu%           %_tgt%\tests\mm\ /S
        copy tests\mm\*.*               %_tgt%\tests\mm

        xcopy tests\mouse\%_cpu%        %_tgt%\tests\mouse\ /S
        copy tests\mouse\*.*            %_tgt%\tests\mouse

        xcopy tests\mvdm                %_tgt%\tests\mvdm\ /S

rem xcopy tests\net\%_cpu%          %_tgt%\tests\net\ /S
rem xcopy tests\net\source          %_tgt%\tests\net\source\ /S
rem copy tests\net\*.*              %_tgt%\tests\net

        xcopy tests\npx\%_cpu%          %_tgt%\tests\npx\ /S
        copy tests\npx\*.*              %_tgt%\tests\npx

        xcopy tests\ports\%_cpu%        %_tgt%\tests\ports\ /S
        copy tests\ports\*.*            %_tgt%\tests\ports

        xcopy tests\sound\%_cpu%        %_tgt%\tests\sound\ /S
        copy tests\sound\*.*            %_tgt%\tests\sound

        xcopy tests\stress\%_cpu%       %_tgt%\tests\stress\ /S
        copy tests\stress\*.*           %_tgt%\tests\stress

rem        xcopy tests\tape\%_cpu%         %_tgt%\tests\tape\ /S
rem        copy tests\tape\*.*             %_tgt%\tests\tape
rem	copy bin\mst\%_cpu%\*.*		%_tgt%\tests\tape
rem	md %_tgt%\tests\tape\include
rem	copy bin\mst\include\mstest.inc %_tgt%\tests\tape\include

        xcopy tests\video\%_cpu%        %_tgt%\tests\video\ /S
        copy tests\video\dos            %_tgt%\tests\video
        copy tests\video\*.*            %_tgt%\tests\video
	md %_tgt%\tests\video\manual
        copy tests\video\manual\*.*     	%_tgt%\tests\video\manual
	copy tests\video\manual\%_cpu%\*.*	%_tgt%\tests\video\manual

        @rem selective emf copying

	copy bin\emf\bitblt*.emf	%_tgt%\tests\video
        copy bin\emf\brush*.emf         %_tgt%\tests\video
        copy bin\emf\text*.emf         %_tgt%\tests\video
        copy bin\emf\region*.emf       %_tgt%\tests\video
        copy bin\emf\floodf*.emf       %_tgt%\tests\video
        copy bin\emf\mskblt*.emf       %_tgt%\tests\video
        copy bin\emf\strblt*.emf       %_tgt%\tests\video
        copy bin\emf\sdbblt*.emf       %_tgt%\tests\video
	copy bin\emf\setdib*.emf	%_tgt%\tests\video
	copy bin\emf\strdib*.emf	%_tgt%\tests\video
	copy bin\emf\plgblt*.emf	%_tgt%\tests\video
	copy bin\emf\binrop*.emf	%_tgt%\tests\video
	copy bin\emf\path*.emf		%_tgt%\tests\video
	copy bin\emf\sbmode*.emf	%_tgt%\tests\video
        copy bin\emf\pen*.emf          %_tgt%\tests\video

        copy %_tgt%\bin\testmgr.exe %_tgt%
        copy %_tgt%\bin\hctinit.dll %_tgt%
        copy %_cpu%.ini                 %_tgt%\testmgr.ini
        copy testmgr.hlp                %_tgt%
        
	copy readme.txt			%_tgt%\readme.txt

        attrib -r %_tgt%\*.* /S
        del %_tgt%\bin\testmgr.exe

        goto done

:mmedia

        @REM install MultiMedia Only HCTs for DDK

        echo.
        echo Copying MultiMedia Only HCTs to %_tgt%
        echo.

        md %_tgt%
        md %_tgt%\logs

        xcopy bin\dos                   %_tgt%\bin\dos\ /S
        xcopy bin\%_cpu%                %_tgt%\bin /S
        copy bin\*.*                    %_tgt%\bin

        xcopy tests\mm\%_cpu%           %_tgt%\tests\mm\ /S
        copy tests\mm\*.*               %_tgt%\tests\mm

        copy %_tgt%\bin\testmgr.exe %_tgt%
        copy mmedia.ini                 %_tgt%\testmgr.ini
        copy testmgr.hlp                %_tgt%

	copy readme.txt			%_tgt%\readme.txt
        
        attrib -r %_tgt%\*.* /S
        del %_tgt%\bin\testmgr.exe

        goto done

:ras
        @REM added by a-gregb 9/8
        @REM to install RAS-WAN tests
        @REM doesn't run under HCT Shell...
    
	echo.
        echo.
        echo.
        echo.
        echo.
	echo		    +--------------------------+
	echo		      Installing RAS-WAN Tests
	echo		    +--------------------------+
        echo.

	md %_tgt%
        md %_tgt%\ras-wan
        copy tests\ras-wan\. %_tgt%\ras-wan
        copy tests\ras-wan\%_cpu%\. %_tgt%\ras-wan

        goto done


:ndis

        @REM install NDIS Only HCTs for DDK
        @REM doesn't run under HCT Shell ....


	echo.
        echo.
        echo.
        echo.
        echo.
	echo		    +----------------------+
	echo		      Installing NDIS Tests
	echo		    +----------------------+
        echo.


	md %_tgt%
        md %_tgt%\tps
        md %_tgt%\tps\logs
        md %_tgt%\tps\scripts

        copy tests\ndis\tps\*.*         %_tgt%\tps
        copy tests\ndis\%_cpu%\*.*      %_tgt%\tps
        xcopy tests\ndis\tps\scripts    %_tgt%\tps\scripts\ /S


	if "%_ndis_option%"=="eth" goto eth
	if "%_ndis_option%"=="fddi" goto fddi
	if "%_ndis_option%"=="tr" goto tr

:tr
	xcopy tests\ndis\tps\logs\tr_logs	%_tgt%\tps\logs\tr_logs\ /S
	xcopy tests\ndis\tps\logs\tr_logss	%_tgt%\tps\logs\tr_logss\ /S
	copy tests\ndis\tps\logs\logs*_tr	%_tgt%\tps\logs
	copy tests\ndis\tps\logs\slogs_tr	%_tgt%\tps\logs
	copy tests\ndis\tps\logs\slogsstr	%_tgt%\tps\logs

	goto done

:fddi

	xcopy tests\ndis\tps\logs\fd_logs	%_tgt%\tps\logs\fd_logs\ /S
	xcopy tests\ndis\tps\logs\fd_logss	%_tgt%\tps\logs\fd_logss\ /S
	copy tests\ndis\tps\logs\logs*_fd	%_tgt%\tps\logs
	copy tests\ndis\tps\logs\slogs_fd	%_tgt%\tps\logs
	copy tests\ndis\tps\logs\slogssfd	%_tgt%\tps\logs	

	goto done

:eth

	xcopy tests\ndis\tps\logs\e_logs	%_tgt%\tps\logs\e_logs\ /S
	xcopy tests\ndis\tps\logs\e_logss	%_tgt%\tps\logs\e_logss\ /S
	copy tests\ndis\tps\logs\logs*_e	%_tgt%\tps\logs
	copy tests\ndis\tps\logs\slogs_e	%_tgt%\tps\logs
	copy tests\ndis\tps\logs\slogss_e	%_tgt%\tps\logs

        goto done

:printer

        @REM install Printer Only HCTs for DDK

        echo.
        echo Copying Printer Only HCTs for DDK to %_tgt%\printhct
        echo.

        md %_tgt%
        md %_tgt%\printhct

	@REM GUIMan

 copy bin\emf\*.* %_tgt%\printhct
	copy bin\%_cpu%\gdiobj.dll	%_tgt%\printhct
 copy tests\video\%_cpu%\objects.dll %_tgt%\printhct
 copy tests\video\%_cpu%\debug32.dll %_tgt%\printhct

	copy tests\video\%_cpu%\guiman.exe	%_tgt%\printhct
	copy bin\%_cpu%\ntwint.dll	%_tgt%\printhct
	copy bin\%_cpu%\ntlog.dll	%_tgt%\printhct
 copy bin\%_cpu%\wintests.dll	%_tgt%\printhct

 copy bin\%_cpu%\scripter.exe %_tgt%\printhct
 copy bin\%_cpu%\parser.exe %_tgt%\printhct

 @REM Print GUIJr && FNTMETS

 copy tests\printer\*.* %_tgt%\printhct
 copy tests\printer\%_cpu%\guijr.exe %_tgt%\printhct
 copy tests\video\%_cpu%\guiprint.dll %_tgt%\printhct

 @REM FNTMETS

 copy tests\printer\%_cpu%\fntmets.exe %_tgt%\printhct

	@REM AppsData

        xcopy tests\printer\appsdata    %_tgt%\printhct\appsdata\ /S

	copy readme.txt			%_tgt%\printhct\readme.txt

       goto done

:storage

        @REM install Storage Only HCTs for DDK

        echo.
        echo Copying Storage Only HCTs for DDK to %_tgt%\
        echo.

        md %_tgt%
        md %_tgt%\logs

        xcopy bin\dos                   %_tgt%\bin\dos\ /S
        xcopy bin\%_cpu%                %_tgt%\bin /S
        copy bin\*.*                    %_tgt%\bin

        @REM pickup the File I/O tests

        xcopy tests\disk\%_cpu%         %_tgt%\tests\disk\ /S
        copy tests\disk\*.*             %_tgt%\tests\disk

	@REM pickup for DevCtl.32

	xcopy tests\disk\12		%_tgt%\tests\disk\12\ /S
	xcopy tests\disk\144		%_tgt%\tests\disk\144\ /S
	xcopy tests\disk\288		%_tgt%\tests\disk\288\ /S
	
        @REM pickup the CD-ROM tests

        xcopy tests\cdrom\%_cpu%        %_tgt%\tests\cdrom\%_cpu%\ /S
        copy tests\cdrom\*.*            %_tgt%\tests\cdrom

        xcopy tests\mm\%_cpu%           %_tgt%\tests\mm\ /S
        copy tests\mm\*.*               %_tgt%\tests\mm

        @REM pickup the Virtual Memory test for paging,etc.

        xcopy tests\memory\%_cpu%       %_tgt%\tests\memory\ /S
        copy tests\memory\*.*           %_tgt%\tests\memory

	@REM pickup the scanner tests

	xcopy tests\scanner\%_cpu%	%_tgt%\tests\scanner\ /S
	copy tests\scanner\*.*		%_tgt%\tests\scanner

        @REM pickup the System Stress tests for good measure

        xcopy tests\stress\%_cpu%       %_tgt%\tests\stress\ /S
        copy tests\stress\*.*           %_tgt%\tests\stress

        @REM pickup the Tape I/O test

        xcopy tests\tape\%_cpu%         %_tgt%\tests\tape\ /S
        md                              %_tgt%\tests\tape\include
        copy tests\tape\*.*             %_tgt%\tests\tape
        copy tests\tape\include\*.*     %_tgt%\tests\tape\include

	copy bin\mst\%_cpu%\*.*		%_tgt%\tests\tape
	md %_tgt%\tests\tape\include
	copy bin\mst\include\mstest.inc %_tgt%\tests\tape\include

        copy %_tgt%\bin\testmgr.exe %_tgt%
        copy storage.ini                %_tgt%\testmgr.ini
        copy testmgr.hlp                %_tgt%

	copy readme.txt			%_tgt%\readme.txt

        
        attrib -r %_tgt%\*.* /S
        del %_tgt%\bin\testmgr.exe

        goto done

:video

        echo .
        echo installing Video Only HCTs for DDK
        echo .

        md %_tgt%
        md %_tgt%\logs

        xcopy bin\dos                   %_tgt%\bin\dos\ /S
        xcopy bin\%_cpu%                %_tgt%\bin /S
        copy  bin\*.*                   %_tgt%\bin

        xcopy tests\video\%_cpu%        %_tgt%\tests\video\ /S
        xcopy bin\emf                   %_tgt%\tests\video\ /S

	md %_tgt%\tests\video\manual
        copy tests\video\manual\*.*             %_tgt%\tests\video\manual
	copy tests\video\manual\%_cpu%\*.*	%_tgt%\tests\video\manual
rem peteg 1/11/94: get .exe's and .dll's for automation
 copy tests\video\manual\*.exe %_tgt%\tests\video
 copy tests\video\manual\*.dll %_tgt%\tests\video
rem peteg 1/12/94: get disptest and resview automation from new dir
 copy tests\video\v_only\*.* %_tgt%\tests\video
rem peteg 2/25/94: get new opengl tests
 copy tests\video\opengl\*.* %_tgt%\tests\video
 copy tests\video\opengl\%_cpu%\*.* %_tgt%\tests\video

	xcopy tests\video\manual\excel	        %_tgt%\tests\video\manual\excel\ /S
	xcopy tests\video\manual\other	        %_tgt%\tests\video\manual\other\ /S
	xcopy tests\video\manual\pm4	        %_tgt%\tests\video\manual\pm4\ /S
	xcopy tests\video\manual\powerpnt	%_tgt%\tests\video\manual\powerpnt\ /S
	xcopy tests\video\manual\winword	%_tgt%\tests\video\manual\winword\ /S

        copy tests\video\dos            %_tgt%\tests\video
        copy tests\video\*.*            %_tgt%\tests\video
        
        copy %_tgt%\bin\testmgr.exe %_tgt%
        copy video.ini                  %_tgt%\testmgr.ini
        copy testmgr.hlp                %_tgt%

	copy readme.txt			%_tgt%\readme.txt
        
        attrib -r %_tgt%\*.* /S
        del %_tgt%\bin\testmgr.exe

        goto done

:usage
        echo.
        echo Usage: setup target [cpu] [-d] [option]
        echo.
        echo Where target is the target drive\directory (required)
        echo Note: Default Directory is drive:\hct
        echo       [cpu] is x86, mips or alpha (optional - default is current PROCESSOR ARCHITECTURE)
        echo       [-d] deletes target drive\directory (optional - default is NO delete)
        rem echo       [-nodoc] do not install docs (saves ~6M space)
        rem echo       [option] is one of:
        echo.
        rem echo          mmedia  - MultiMedia only HCTs   [  8M disk space]
        echo          ndis    - NDIS tester [default]      [ 37M disk space]
        echo            sub options : 
        echo                eth     (Ethernet Media)   [default]
        echo                fddi    (FDDI Media)
	       echo                tr      (Token Ring Media)
        rem echo          printer - Printer only HCTs      [ 24M disk space]
        rem echo          storage - Storage only HCTs      [ 25M disk space]
        rem echo          video   - Video only HCTs        [ 65M disk space]
        rem echo.
        rem echo       By default, all tests except NDIS, Printer, ARCTest
        rem echo       and Tape tests are installed.
        echo.
        echo Be sure to specify a valid drive letter\path (e.g c:\hcttests).
        echo Drive letters alone (e.g. c:) will not work.  Tests will get
        echo dumped into the root if you use drive letter alone.
        echo.
        echo.
        rem echo Requirements:
        echo.
        rem echo       Full x86 installation occupies ~62 MB of disk space.
        rem echo       Full MIPS installation occupies ~58 MB of disk space.
        remecho       Full ALPHA installation occupies ~57 MB of disk space.
        echo.

        goto end

:done
        echo.
        echo.
        echo.
        echo.
        echo.
        echo                +--------------------+
        echo                  HCT setup complete  
        echo                +--------------------+
        echo.

:end

