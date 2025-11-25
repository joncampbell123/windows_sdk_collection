all: largeint.lib

largeint.obj: largeint.s
    acc -O -g0 -G0 -std -checkstack 2048 -excpt -EL -c -DALPHA=1 -D_ALPHA_=1 \
       -D_LANGUAGE_ASSEMBLY -o largeint.obj largeint.s

# Update the import library

largeint.lib: largeint.obj
    a2coff largeint.obj
    lib32 -machine:ALPHA \
    largeint.obj \
    -out:largeint.lib
