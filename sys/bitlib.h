#ifndef _BITLIB_H_
    #define _BITLIB_H_
    
    #ifndef _BITLIB_C_
        extern int bitCnt(int);
        extern int reverse(int);
        extern int rBit(int);

        //4*4
        extern int rotateRight4x4(int);
        extern int rotateLeft4x4(int);
    #else
        int rBit(int);
        int bitCnt(int);
        int reverse(int);
        int rotateLeft4x4(int);
        int rotateRight4x4(int);

        static int flipHorizontal(int);
        static int flipDiagnoalA1D4(int);
        static int flipDiagnoalA4D1(int);
        static int delta_swap(int,int,int);
    #endif
#endif