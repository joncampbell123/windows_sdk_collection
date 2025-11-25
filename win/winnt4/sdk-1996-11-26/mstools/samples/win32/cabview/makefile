!include <win32.mak>

LINK32_OBJS= enum.obj view.obj dlg.obj icon.obj os.obj strings.obj xicon.obj debug.obj \
	da.obj IUtil.obj folder.obj sfvmenu.obj menu.obj thisdll.obj sfview.obj unknown.obj \
	cabitms.obj sfvwnd.obj dataobj.obj path.obj cabview.res


ALL : cabview.dll

thisdll.obj : thisdll.cpp Cabobj.h Cabp.h Cabvw2.h dpda.h pch.h thisdll.h thisguid.h
    $(cc) $(cflags) $(cvars) $(cdebug) thisdll.cpp 

cabview.res : cabview.rc cabview.ico cabview.ver document.ico cabview.def
    $(rc) $(rcflags) $(rcvars) $(rcdebug) cabview.rc 

folder.obj : folder.cpp cabitms.h Cabobj.h Cabp.h Cabvw2.h dataobj.h \
	dpda.h enum.h fdi.h folder.h icon.h menu.h pch.h sfview.h thisdll.h\
      thisguid.h view.h
    $(cc) $(cflags) $(cvars) $(cdebug) folder.cpp 

view.obj : view.cpp Cabobj.h Cabp.h Cabvw2.h dpda.h folder.h os.h pch.h \
	sfview.h thisdll.h unknown.h view.h
    $(cc) $(cflags) $(cvars) $(cdebug) view.cpp 


enum.obj : enum.cpp Cabobj.h Cabp.h Cabvw2.h dpda.h enum.h folder.h \
	pch.h thisdll.h	
    $(cc) $(cflags) $(cvars) $(cdebug) enum.cpp 

cabitms.obj : cabitms.cpp cabitms.h Cabobj.h Cabp.h Cabvw2.h dpda.h fdi.h \
	path.h pch.h thisdll.h
    $(cc) $(cflags) $(cvars) $(cdebug) cabitms.cpp 

icon.obj : icon.cpp Cabobj.h Cabp.h Cabvw2.h dpda.h icon.h path.h pch.h thisdll.h
    $(cc) $(cflags) $(cvars) $(cdebug)  icon.cpp 

os.obj : os.cpp Cabobj.h Cabp.h Cabvw2.h dpda.h os.h pch.h
    $(cc) $(cflags) $(cvars) $(cdebug) os.cpp 

menu.obj : menu.cpp cabitms.h Cabobj.h Cabp.h Cabvw2.h dpda.h fdi.h folder.h menu.h pch.h thisdll.h
    $(cc) $(cflags) $(cvars) $(cdebug) menu.cpp 


dataobj.obj : dataobj.cpp cabitms.h Cabobj.h Cabp.h Cabvw2.h dataobj.h dpda.h \
	fdi.h folder.h pch.h thisdll.h 
    $(cc) $(cflags) $(cvars) $(cdebug) dataobj.cpp 


sfview.obj : sfview.cpp Cabobj.h Cabp.h Cabvw2.h dlg.h dpda.h pch.h sfview.h sfvwnd.h \
	thisdll.h thisguid.h unknown.h xicon.h
    $(cc) $(cflags) $(cvars) $(cdebug) sfview.cpp 

unknown.obj : unknown.cpp Cabobj.h Cabp.h Cabvw2.h dpda.h pch.h thisdll.h unknown.h
    $(cc) $(cflags) $(cvars) $(cdebug) unknown.cpp 

dlg.obj : dlg.cpp Cabobj.h Cabp.h Cabvw2.h dlg.h dpda.h pch.h thisdll.h
    $(cc) $(cflags) $(cvars) $(cdebug) dlg.cpp 

xicon.obj : xicon.cpp Cabobj.h Cabp.h Cabvw2.h dpda.h path.h pch.h thisdll.h unknown.h xicon.h
    $(cc) $(cflags) $(cvars) $(cdebug)  xicon.cpp 


sfvwnd.obj :sfvwnd.cpp Cabobj.h Cabp.h Cabvw2.h dlg.h dpda.h pch.h sfview.h sfvwnd.h \
      thisdll.h unknown.h xicon.h
    $(cc) $(cflags) $(cvars) $(cdebug) sfvwnd.cpp 


sfvmenu.obj: sfvmenu.cpp Cabobj.h Cabp.h Cabvw2.h dlg.h dpda.h pch.h sfview.h sfvwnd.h \
      thisdll.h unknown.h xicon.h 
    $(cc) $(cflags) $(cvars) $(cdebug) sfvmenu.cpp


debug.obj: debug.c Cabobj.h Cabp.h  Cabvw2.h debug.h dpda.h pch.h 
    $(cc) $(cflags) $(cvars) $(cdebug) debug.c


path.obj : path.c  Cabobj.h Cabp.h Cabvw2.h debug.h dpda.h path.h pch.h strings.h
    $(cc) $(cflags) $(cvars) $(cdebug) path.c  


strings.obj : strings.c Cabobj.h Cabp.h Cabvw2.h dpda.h pch.h strings.h
    $(cc) $(cflags) $(cvars) $(cdebug) strings.c


IUtil.obj : IUtil.c Cabobj.h Cabp.h Cabvw2.h dpda.h pch.h 
    $(cc) $(cflags) $(cvars) $(cdebug) IUtil.c 


da.obj : da.c Cabobj.h Cabp.h Cabvw2.h dpda.h pch.h 
    $(cc) $(cflags) $(cvars) $(cdebug) da.c


cabview.dll : $(DEF_FILE) $(LINK32_OBJS)
    $(link) $(linkdebug) $(dlllflags)  $(LINK32_OBJS) \
    -out:cabview.Dll fdi.lib $(olelibsdll) $(olelibs)  shell32.lib comctl32.lib \
    /def:cabview.def



CLEAN : 
	-@erase cabview.dll
	-@erase enum.obj
	-@erase view.obj
	-@erase dlg.obj
	-@erase icon.obj
	-@erase os.obj
	-@erase strings.obj
	-@erase xicon.obj
	-@erase debug.obj
	-@erase da.obj
	-@erase IUtil.obj
	-@erase folder.obj
	-@erase sfvmenu.obj
	-@erase menu.obj
	-@erase thisdll.obj
	-@erase sfview.obj
	-@erase unknown.obj
	-@erase cabitms.obj
	-@erase sfvwnd.obj
	-@erase dataobj.obj
	-@erase path.obj
	-@erase cabview.res
	-@erase cabview.lib
	-@erase cabview.exp
