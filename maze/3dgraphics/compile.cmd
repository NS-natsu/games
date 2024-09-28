g++ -c 3d.cpp -fopenmp -lgdi32
g++ -o 3dmaze 3d.o -lgdi32 -fopenmp -static-libgcc -static-libstdc++ -Wl,-Bstatic,--whole-archive -lwinpthread -lgomp -Wl,--no-whole-archive