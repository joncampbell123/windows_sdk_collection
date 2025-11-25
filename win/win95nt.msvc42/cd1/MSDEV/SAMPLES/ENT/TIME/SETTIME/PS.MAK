
settmps.dll: dlldata.obj settime_p.obj settime_i.obj
        link /dll /out:settmps.dll /def:settmps.def /entry:DllMain dlldata.obj settime_p.obj settime_i.obj kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib uuid.lib 

.c.obj:
        cl /c /Ox /DWIN32 /DREGISTER_PROXY_DLL $<

clean:
        @del settimeps.dll
        @del settimeps.lib
        @del settimeps.exp
        @del dlldata.obj
        @del settime_p.obj
        @del settime_i.obj
