        cp z:\580\hct\alpha\diskload.exe .
        cp z:\580\rats\alpha\kernel\file_io.32\akrnfil2.dll .
        cp z:\580\rats\alpha\kernel\file_io.32\chldsize.exe .
        cp z:\580\rats\alpha\kernel\file_io.32\chldread.exe .
        cp z:\580\rats\alpha\kernel\file_io.32\openf.exe .
        cp z:\580\rats\alpha\kernel\file_io.32\rasync.exe .
        cp z:\580\rats\alpha\kernel\file_io.32\readch.exe .
        cp z:\580\rats\alpha\kernel\file_io.32\rother.exe .
        cp z:\580\rats\alpha\kernel\file_io.32\wasync.exe .
        cp z:\580\rats\alpha\kernel\file_io.32\wother.exe .
        cp z:\580\rats\alpha\kernel\file_io.32\writech.exe .
        cp z:\580\rats\alpha\kernel\mappedio.32\akrn1map.dll .

	@REM Async CRC Test binaries

	@REM GER - crt bug on build 462 - copy special ones	
	@REM cp z:\580\rats\alpha\kernel\ntft\stress\crcchk.exe .

 cp z:\580\rats\alpha\kernel\ntft\stress\ccchk.exe .
	cp z:\580\rats\alpha\kernel\ntft\stress\freedisk.exe .
	cp z:\580\rats\alpha\kernel\ntft\stress\sizecopy.exe .
	cp z:\580\rats\alpha\kernel\ntft\stress\tstbegin.exe .
	cp z:\580\rats\alpha\kernel\ntft\stress\tstend.exe .

	@REM Format test - no binaries

	@REM DevCtl.32 

	cp z:\580\rats\alpha\kernel\devctl.32\getgeom.exe .
	cp z:\580\rats\alpha\kernel\devctl.32\getperf.exe .
	cp z:\580\rats\alpha\kernel\devctl.32\getmedia.exe .
	cp z:\580\rats\alpha\kernel\devctl.32\reassign.exe .
	cp z:\580\rats\alpha\kernel\devctl.32\mediaej.exe .
	cp z:\580\rats\alpha\kernel\devctl.32\mediald.exe .

        in -of -c "updated for 580\"
