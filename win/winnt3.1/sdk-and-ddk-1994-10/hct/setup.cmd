@echo off

@rem
@rem Windows NT 3.1 July Release HCT
@rem
@rem setup.bat - install HCTs from floppy under DOS
@rem

        echo.
        echo Windows NT 3.1 HCT Installation  [Release 3.10]
        echo -----------------------------------------------

        if "%1"=="" goto usage

        set _tgt=
        set _cpu=x86
        set _nuke=
        set _option=

	@REM _ndis_option for NDIS media option - default to ethernet
	set _ndis_option=eth

        echo.
        echo Parsing arguments...
        echo.

        set _tgt=%1
        shift

:loop
        if "%1"=="mips" set _cpu=mips
        if "%1"=="MIPS" set _cpu=mips

        if "%1"=="alpha" set _cpu=alpha
        if "%1"=="ALPHA" set _cpu=alpha

        if "%1"=="-d" set _nuke=yes

        if "%1"=="mmedia" set _option=mmedia
        if "%1"=="MMEDIA" set _option=mmedia

        if "%1"=="ndis" set _option=ndis
        if "%1"=="NDIS" set _option=ndis

        if "%1"=="printer" set _option=printer
        if "%1"=="PRINTER" set _option=printer

        if "%1"=="storage" set _option=storage
        if "%1"=="STORAGE" set _option=storage

        if "%1"=="video" set _option=video
        if "%1"=="VIDEO" set _option=video

	@REM _ndis_option parsing

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
        if "%_tgt%"=="" goto usage

	for %%i in (c d e f g h i j k l m n o p q r s t u v w x y z) do if "%_tgt%"=="%%i:" goto goodstart
	echo Be sure to specify a valid drive letter (e.g c:).  
        echo pathnames (e.g. c:\) will not work.
        goto usage

:goodstart
        echo Installing %_cpu% binaries
        echo.

        if "%_nuke%"=="yes" goto nukehct

        goto install

:nukehct
        echo Deleting %_tgt%\hct...
        echo.
        pause
        echo.
        echo    === Ignore the following error if there is one ===
        echo.
        .\bin\dos\delnode /q %_tgt%\hct
        .\bin\%_cpu%\delnode /q %_tgt%\hct

:install

	@REM selective installs for DDK

        if "%_option%"=="mmedia" goto mmedia
        if "%_option%"=="ndis" goto ndis
        if "%_option%"=="printer" goto printer
        if "%_option%"=="storage" goto storage
        if "%_option%"=="video" goto video

        @rem
        @rem for each component, copy generic cmd files and cpu-specific bins
        @rem
        @rem CDROM ARC test and NDIS test don't run or install under HCTs
        @rem Printer isn't installed for systems either

        echo.
        echo Copying files to %_tgt%\hct
        echo.

        md %_tgt%\hct
        md %_tgt%\hct\logs

        xcopy bin\dos                   %_tgt%\hct\bin\dos\ /S /F

        xcopy bin\%_cpu%                %_tgt%\hct\bin /S /F
        copy bin\*.*                    %_tgt%\hct\bin

        xcopy prodline                  %_tgt%\hct\prodline\ /S /F

        xcopy doc                       %_tgt%\hct\doc\ /S /F

        xcopy tests\disk\%_cpu%         %_tgt%\hct\tests\disk\ /S /F
        copy tests\disk\*.*             %_tgt%\hct\tests\disk
	xcopy tests\disk\12		%_tgt%\hct\tests\disk\12\ /S
	xcopy tests\disk\144		%_tgt%\hct\tests\disk\144\ /S
	xcopy tests\disk\288		%_tgt%\hct\tests\disk\288\ /S

        md %_tgt%\hct\tests\cdrom
        copy tests\cdrom\*.*            %_tgt%\hct\tests\cdrom

        xcopy tests\kbd\%_cpu%          %_tgt%\hct\tests\kbd\ /S /F
        copy tests\kbd\*.*              %_tgt%\hct\tests\kbd

        xcopy tests\memory\%_cpu%       %_tgt%\hct\tests\memory\ /S /F
        copy tests\memory\*.*           %_tgt%\hct\tests\memory

        xcopy tests\mm\%_cpu%           %_tgt%\hct\tests\mm\ /S /F
        copy tests\mm\*.*               %_tgt%\hct\tests\mm

        xcopy tests\mouse\%_cpu%        %_tgt%\hct\tests\mouse\ /S /F
        copy tests\mouse\*.*            %_tgt%\hct\tests\mouse

        xcopy tests\mvdm                %_tgt%\hct\tests\mvdm\ /S /F

rem        xcopy tests\net\%_cpu%          %_tgt%\hct\tests\net\ /S /F
rem        xcopy tests\net\source          %_tgt%\hct\tests\net\source\ /S /F
rem        copy tests\net\*.*              %_tgt%\hct\tests\net

        xcopy tests\npx\%_cpu%          %_tgt%\hct\tests\npx\ /S /F
        copy tests\npx\*.*              %_tgt%\hct\tests\npx

        xcopy tests\ports\%_cpu%        %_tgt%\hct\tests\ports\ /S /F
        copy tests\ports\*.*            %_tgt%\hct\tests\ports

        xcopy tests\sound\%_cpu%        %_tgt%\hct\tests\sound\ /S /F
        copy tests\sound\*.*            %_tgt%\hct\tests\sound

        xcopy tests\stress\%_cpu%       %_tgt%\hct\tests\stress\ /S /F
        copy tests\stress\*.*           %_tgt%\hct\tests\stress

rem        xcopy tests\tape\%_cpu%         %_tgt%\hct\tests\tape\ /S /F
rem        copy tests\tape\*.*             %_tgt%\hct\tests\tape
rem	copy bin\mst\%_cpu%\*.*		%_tgt%\hct\tests\tape
rem	md %_tgt%\hct\tests\tape\include
rem	copy bin\mst\include\mstest.inc %_tgt%\hct\tests\tape\include

        xcopy tests\video\%_cpu%        %_tgt%\hct\tests\video\ /S
        copy tests\video\dos            %_tgt%\hct\tests\video
	copy tests\video\*.*		%_tgt%\hct\tests\video
	md %_tgt%\hct\tests\video\manual
        copy tests\video\manual\*.*     	%_tgt%\hct\tests\video\manual
	copy tests\video\manual\%_cpu%\*.*	%_tgt%\hct\tests\video\manual

        @rem selective emf copying

	copy bin\emf\bitblt*.emf	%_tgt%\hct\tests\video
        copy bin\emf\brush*.emf         %_tgt%\hct\tests\video
        copy bin\emf\text0*.emf         %_tgt%\hct\tests\video
        copy bin\emf\region0*.emf       %_tgt%\hct\tests\video
        copy bin\emf\floodf0*.emf       %_tgt%\hct\tests\video
        copy bin\emf\mskblt0*.emf       %_tgt%\hct\tests\video
        copy bin\emf\strblt0*.emf       %_tgt%\hct\tests\video
        copy bin\emf\sdbblt0*.emf       %_tgt%\hct\tests\video
	copy bin\emf\setdib*.emf	%_tgt%\hct\tests\video
	copy bin\emf\strdib*.emf	%_tgt%\hct\tests\video
	copy bin\emf\plgblt*.emf	%_tgt%\hct\tests\video
	copy bin\emf\binrop*.emf	%_tgt%\hct\tests\video
	copy bin\emf\path*.emf		%_tgt%\hct\tests\video
	copy bin\emf\sbmode*.emf	%_tgt%\hct\tests\video
        copy bin\emf\pen0*.emf          %_tgt%\hct\tests\video

        copy %_tgt%\hct\bin\testmgr.exe %_tgt%\hct
        copy %_cpu%.ini                 %_tgt%\hct\testmgr.ini
        copy testmgr.hlp                %_tgt%\hct
        
	copy readme.txt		       %_tgt%\hct\readme.txt

        attrib -r %_tgt%\hct\*.* /S
        del %_tgt%\hct\bin\testmgr.exe

        goto done

:mmedia

        @REM install MultiMedia Only HCTs for DDK

        echo.
        echo Copying MultiMedia Only HCTs to %_tgt%\hct
        echo.

        md %_tgt%\hct
        md %_tgt%\hct\logs

        xcopy bin\dos                   %_tgt%\hct\bin\dos\ /S /F
        xcopy bin\%_cpu%                %_tgt%\hct\bin /S /F
        copy bin\*.*                    %_tgt%\hct\bin

        xcopy tests\mm\%_cpu%           %_tgt%\hct\tests\mm\ /S /F
        copy tests\mm\*.*               %_tgt%\hct\tests\mm

        copy %_tgt%\hct\bin\testmgr.exe %_tgt%\hct
        copy mmedia.ini                 %_tgt%\hct\testmgr.ini
        copy testmgr.hlp                %_tgt%\hct

	copy readme.txt			%_tgt%\hct\readme.txt
        
        attrib -r %_tgt%\hct\*.* /S
        del %_tgt%\hct\bin\testmgr.exe

        goto done

:ndis

        @REM install NDIS Only HCTs for DDK
        
        @REM doesn't run under HCT Shell ....

        md %_tgt%\tps
        md %_tgt%\tps\logs
        md %_tgt%\tps\scripts
REM        md %_tgt%\tps\logtrnpr
REM        md %_tgt%\tps\scrtrnpr


        copy tests\ndis\tps\*.*         %_tgt%\tps
        copy tests\ndis\%_cpu%\*.*      %_tgt%\tps
        xcopy tests\ndis\tps\logs       %_tgt%\tps\logs\ /S 
        xcopy tests\ndis\tps\scripts    %_tgt%\tps\scripts\ /S
REM        xcopy tests\ndis\tps\logtrnpr   %_tgt%\tps\logtrnpr\ /S
REM        xcopy tests\ndis\tps\scrtrnpr   %_tgt%\tps\scrtrnpr\ /S


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
	@rem why no slogs_fd ? - AI
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

        md %_tgt%\printhct

	@REM GUIMan

        copy bin\emf\*.*                	%_tgt%\printhct
	copy tests\video\%_cpu%\gdiobj.dll	%_tgt%\printhct
	copy tests\video\%_cpu%\guiman.exe	%_tgt%\printhct
	copy tests\video\%_cpu%\ntwint.dll	%_tgt%\printhct
	copy tests\video\%_cpu%\ntlog.dll	%_tgt%\printhct
	copy tests\video\%_cpu%\wintests.dll	%_tgt%\printhct

	@REM PrntTest

        copy tests\printer\%_cpu%\*.*   %_tgt%\printhct
        copy tests\printer\*.*          %_tgt%\printhct

	@REM AppsData

        xcopy tests\printer\appsdata    %_tgt%\printhct\appsdata\ /S

	copy readme.txt			%_tgt%\printhct\readme.txt

        goto done

:storage

        @REM install Storage Only HCTs for DDK
        
        md %_tgt%\hct
        md %_tgt%\hct\logs

        xcopy bin\dos                   %_tgt%\hct\bin\dos\ /S /F
        xcopy bin\%_cpu%                %_tgt%\hct\bin /S /F
        copy bin\*.*                    %_tgt%\hct\bin

        @REM pickup the File I/O tests

        xcopy tests\disk\%_cpu%         %_tgt%\hct\tests\disk\ /S /F
        copy tests\disk\*.*             %_tgt%\hct\tests\disk

	@REM pickup for DevCtl.32

	xcopy tests\disk\12		%_tgt%\hct\tests\disk\12\ /S
	xcopy tests\disk\144		%_tgt%\hct\tests\disk\144\ /S
	xcopy tests\disk\288		%_tgt%\hct\tests\disk\288\ /S

        @REM pickup the CD-ROM tests

        xcopy tests\cdrom\%_cpu%        %_tgt%\hct\tests\cdrom\%_cpu%\ /S /F
        copy tests\cdrom\*.*            %_tgt%\hct\tests\cdrom

        xcopy tests\mm\%_cpu%           %_tgt%\hct\tests\mm\ /S /F
        copy tests\mm\*.*               %_tgt%\hct\tests\mm

        @REM pickup the Virtual Memory test for paging,etc.

        xcopy tests\memory\%_cpu%       %_tgt%\hct\tests\memory\ /S /F
        copy tests\memory\*.*           %_tgt%\hct\tests\memory

	@REM pickup the scanner tests

	xcopy tests\scanner\%_cpu%	%_tgt%\hct\tests\scanner\ /S
	copy tests\scanner\*.*		%_tgt%\hct\tests\scanner

        @REM pickup the System Stress tests for good measure

        xcopy tests\stress\%_cpu%       %_tgt%\hct\tests\stress\ /S /F
        copy tests\stress\*.*           %_tgt%\hct\tests\stress

        @REM pickup the Tape I/O test

        xcopy tests\tape\%_cpu%         %_tgt%\hct\tests\tape\ /S /F
        copy tests\tape\*.*             %_tgt%\hct\tests\tape

	copy bin\mst\%_cpu%\*.*		%_tgt%\hct\tests\tape
	md %_tgt%\hct\tests\tape\include
	copy bin\mst\include\mstest.inc %_tgt%\hct\tests\tape\include

        copy %_tgt%\hct\bin\testmgr.exe %_tgt%\hct
        copy storage.ini                %_tgt%\hct\testmgr.ini
        copy testmgr.hlp                %_tgt%\hct

	copy readme.txt			%_tgt%\hct\readme.txt

        attrib -r %_tgt%\hct\*.* /S
        del %_tgt%\hct\bin\testmgr.exe

        goto done

:video

        echo .
        echo installing Video Only HCTs for DDK
        echo .

        md %_tgt%\hct
        md %_tgt%\hct\logs

        xcopy bin\dos                   %_tgt%\hct\bin\dos\ /S
        xcopy bin\%_cpu%                %_tgt%\hct\bin /S
        copy bin\*.*                    %_tgt%\hct\bin

        xcopy tests\video\%_cpu%        %_tgt%\hct\tests\video\ /S
        xcopy bin\emf                   %_tgt%\hct\tests\video\ /S

	md %_tgt%\hct\tests\video\manual
        copy tests\video\manual\*.*             %_tgt%\hct\tests\video\manual
	copy tests\video\manual\%_cpu%\*.*	%_tgt%\hct\tests\video\manual
	xcopy tests\video\manual\excel	        %_tgt%\hct\tests\video\manual\excel\ /S
	xcopy tests\video\manual\other	        %_tgt%\hct\tests\video\manual\other\ /S
	xcopy tests\video\manual\pm4	        %_tgt%\hct\tests\video\manual\pm4\ /S
	xcopy tests\video\manual\powerpnt	%_tgt%\hct\tests\video\manual\powerpnt\ /S
	xcopy tests\video\manual\winword	%_tgt%\hct\tests\video\manual\winword\ /S

        copy tests\video\dos            %_tgt%\hct\tests\video
        copy tests\video\*.*            %_tgt%\hct\tests\video

        copy %_tgt%\hct\bin\testmgr.exe %_tgt%\hct
        copy video.ini                  %_tgt%\hct\testmgr.ini
        copy testmgr.hlp                %_tgt%\hct

	copy readme.txt			%_tgt%\hct\readme.txt
        
        attrib -r %_tgt%\hct\*.* /S
        del %_tgt%\hct\bin\testmgr.exe

        goto done

:usage
        echo.
        echo Usage: setup target [cpu] [-d] [option]
        echo.
        echo where target is the target drive (required)
        echo       [cpu] is x86, mips or alpha (optional - default is x86)
        echo       [-d] deletes \HCT on target drive (optional - default is NO delete)
        echo       [option] is one of:
        echo.
        echo          mmedia  - MultiMedia only HCTs   [4.5M disk space]
        echo          ndis    - NDIS tester            [ 37M disk space]
        echo            sub options : 
        echo                eth     (Ethernet Media)   [default]
        echo                fddi    (FDDI Media)
	echo                tr      (Token Ring Media)
        echo          printer - Printer only HCTs      [ 24M disk space]
        echo          storage - Storage only HCTs      [ 15M disk space]
        echo          video   - Video only HCTs        [ 33M disk space]
        echo.
        echo       By default, all tests except NDIS, Printer and ARCTest are
        echo       installed.
        echo.
        echo.
        echo Requirements:
        echo.
        echo       Full x86 installation occupies ~36 MB of disk space.
        echo       Full MIPS installation occupies ~38 MB of disk space.
        echo       Full ALPHA installation occupies ~XX MB of disk space.
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
