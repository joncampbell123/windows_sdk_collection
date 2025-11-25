@rem
@rem propgates the ntft sub-tree to a given location
@rem

if "%1" == "" goto usage

md %1
md %1\x86
md %1\mips
md %1\alpha
md %1\ppc

call propgate w32nt\i386  %1\x86
call propgate w32nt\mips  %1\mips
call propgate w32nt\alpha %1\alpha
call propgate w32nt\ppc   %1\ppc

goto done

:usage
@echo proptree destdir

:done
