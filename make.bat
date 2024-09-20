@echo off
cd sys
gcc -c bag.c mino.c bitlib.c
gcc -c rotate.c movement.c
ar rvs system.o bag.o bitlib.o mino.o movement.o rotate.o

del bag.o
del mino.o
del rotate.o
del bitlib.o
del movement.o

cd ..\screen
gcc -c screen.c

cd ..\console_setup
gcc -c console_setup.c

cd ..\input
gcc -c estimate.c
gcc -c interface.c
ar rvs input.o estimate.o interface.o

del estimate.o
del interface.o

cd ..\
del tetoris.exe
gcc -c tet.c

gcc -o tetris tet.o input\input.o screen\screen.o sys\system.o console_setup\console_setup.o

del tet.o
del sys\system.o
del screen\screen.o
del input\input.o
del console_setup\console_setup.o
@echo on