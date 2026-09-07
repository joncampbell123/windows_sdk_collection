
AtlBehaveps.dll: dlldata.obj AtlBehave_p.obj AtlBehave_i.obj
	link /dll /out:AtlBehaveps.dll /def:AtlBehaveps.def /entry:DllMain dlldata.obj AtlBehave_p.obj AtlBehave_i.obj kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib 

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL $<

clean:
	@del AtlBehaveps.dll
	@del AtlBehaveps.lib
	@del AtlBehaveps.exp
	@del dlldata.obj
	@del AtlBehave_p.obj
	@del AtlBehave_i.obj
