DIRLIST =  lsasamp    \
           gina       \
           secfile    \
           sidcln     \
           takeown    \
           check_sd   \
           sd_flppy   \
           sockauth   \
           subauth    \
           audit      \
           disptrus   \
           domtrust   \
           httpauth   \
           lsaprivs   \
           machacct   \
           rcmd       \
           secperf    \
           textsid    \
           ginastub   \
           aclapi     \
           dcomperm

all: $(DIRLIST)

$(DIRLIST):
        IF EXIST $@\nul <<nmaketmp.bat
    cd $@
    @echo *** win32\winnt\security\$@ *** >>$(MSTOOLS)\samples\security.tmp
    @nmake -a -i -nologo >>$(MSTOOLS)\samples\security.tmp
    cd ..
<<
