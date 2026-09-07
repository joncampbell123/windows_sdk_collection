
Dataps.dll: dlldata.obj Data_p.obj Data_i.obj
	link /dll /out:Dataps.dll /def:Dataps.def /entry:DllMain dlldata.obj Data_p.obj Data_i.obj \
		mtxih.lib mtx.lib mtxguid.lib \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \
		ole32.lib advapi32.lib 

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		/MD \
		$<

clean:
	@del Dataps.dll
	@del Dataps.lib
	@del Dataps.exp
	@del dlldata.obj
	@del Data_p.obj
	@del Data_i.obj
