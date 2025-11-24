
!include <ntwin32.mak>

all: largeint.lib

divlarge.obj: divlarge.c
    $(cc) $(cflags) $(cvars) divlarge.c

largeint.obj: largeint.asm
    masm386 -Mx -DSTD_CALL -DFPO=0 -Di386=1 -D_X86_=1 \
    largeint.asm,largeint.obj;

# Update the import library

largeint.lib: largeint.obj divlarge.obj
    $(implib) -machine:$(CPU)     \
    largeint.obj	 	    \
    divlarge.obj		\
    -out:largeint.lib

