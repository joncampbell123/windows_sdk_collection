
all: largeint.lib

largeint.obj: largeint.s
    cc -O -g0 -G0 -std -checkstack 2048 -excpt -EL -c -DMIPS=1 -D_MIPS_=1 \
       -D_LANGUAGE_ASSEMBLY -o largeint.obj largeint.s

# Update the import library

largeint.lib: largeint.obj 
    mip2coff largeint.obj 
    lib32 -machine:MIPS     \
    largeint.obj	 	    \
    -out:largeint.lib

