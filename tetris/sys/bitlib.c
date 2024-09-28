#define _BITLIB_C
#include "bitlib.h"

int rBit(int n){
    n = reverse(n);
    n &= -n;
    return reverse(n);
}

int bitCnt(int n){
    n = (n&0x5555) + ((n>>1)&0x5555);
    n = (n&0x3333) + ((n>>2)&0x3333);
    n = (n&0x0f0f) + ((n>>4)&0x0f0f);
    n = (n&0x00ff) + ((n>>8)&0x00ff);
    return n;
}

int reverse(int n){
    n = ((n >> 8) & 0x00ff) | ((n << 8) & 0xff00);
    n = ((n >> 4) & 0x0f0f) | ((n << 4) & 0xf0f0);
    n = ((n >> 2) & 0x3333) | ((n << 2) & 0xcccc);
    n = ((n >> 1) & 0x5555) | ((n << 1) & 0xaaaa);
    return n;
}

//4*4
static int delta_swap(int x, int mask, int delta){
    int t = (x ^ (x >> delta)) & mask;
    return x ^ t ^ (t << delta);
}

static int flipDiagnoalA1D4(int x){
    x = delta_swap(x, 0x0a0a, 3);
    return delta_swap(x, 0x00cc, 6);
}

static int flipDiagnoalA4D1(int x){
    x = delta_swap(x, 0x0505, 5);
    return delta_swap(x, 0x0033, 10);
}

static int flipHorizontal(int x){
    x = delta_swap(x, 0x5555, 1);
    return delta_swap(x, 0x3333, 2);
}

int rotateRight4x4(int x){
    x = flipDiagnoalA1D4(x);
    return flipHorizontal(x);
}

int rotateLeft4x4(int x){
    x = flipDiagnoalA4D1(x);
    return flipHorizontal(x);
}

/*
unsigned int delta_swap(unsigned int x, unsigned int mask, unsigned delta){
    unsigned int t = (x ^ (x >> delta)) & mask;
    return x ^ t ^ (t << delta);
}

unsigned int rotateLeft(unsigned int x, int n, int max){
    return (x << n) | (x >> (max - n))
}

unsigned int rotateRight(unsigned int x, int n, int max){
    return (x >> n) | (x << (max - n))
}

unsigned int flipDiagnoalA1H8(int x){

}*/