all:
        cd src
        $(MAKE) -f fi_src
        cd..\rc
        $(MAKE) -f fi_rc
        cd..\tmu
        $(MAKE) -f fi_tmu
        cd..
        if exist finstall.dll copy finstall.dll finstall.bak
        if exist finstall.dll del finstall.dll
        $(MAKE) -f fi_root
