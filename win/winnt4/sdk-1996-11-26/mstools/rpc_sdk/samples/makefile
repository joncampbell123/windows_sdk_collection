all : helloc hellos
helloc:
	cd client
	nmake all
	cd ..

hellos:
	cd server
	nmake all
	cd ..

# Clean up everything
cleanall : clean
    -del client\*.exe
    -del server\*.exe

# Clean up everything but the .EXEs
clean :
    -del client\*.obj
    -del client\*.map
    -del client\hello_c.c
    -del client\hello_s.c
    -del client\hello.h
    -del client\*.rsc
    -del client\*.pdb
    -del server\*.obj
    -del server\*.map
    -del server\hello_c.c
    -del server\hello_s.c
    -del server\hello.h
    -del server\*.rsc
    -del server\*.pdb
