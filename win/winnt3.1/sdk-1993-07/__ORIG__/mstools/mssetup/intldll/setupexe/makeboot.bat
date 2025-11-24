
rem    edit strings in SETUP.RC and STUB.INC

masm stub;
link stub;

rc -r setup.rc
link /NOL /MAP /F /PACKC:61440 /PACKD:61440 /NOD setup virwinn,/align:16, ,slibcew libw sdecompw lzexpand, setup

rc -30 -t setup.res
mapsym setup.map
virpatch setup.exe setup.sym _WHashGood

