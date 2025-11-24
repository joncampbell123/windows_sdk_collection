DIRLIST  =  stastrip    \
            classlib    \
            bcdemo      \
            gbdemo      \
            chap02      \
            chap03      \
            chap04      \
            chap05      \
            chap06      \
            chap07      \
            chap08      \
            chap09      \
            chap10      \
            chap11      \
            chap12      \
            chap13      \
            chap14      \
            chap15      \
            chap16


all: lib $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\$@ *** >>$(SDKROOT)\samples\inole2.tmp
        @nmake -a -i >>$(SDKROOT)\samples\inole2.tmp
        cd ..

lib:
        cd $@
        @echo *** ole\inole2\$@ *** >>$(SDKROOT)\samples\inole2.tmp
        @nmake -i >>$(SDKROOT)\samples\inole2.tmp
        cd ..
