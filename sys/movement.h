#ifndef _MOVEMENT_H_
    #define _MOVEMENT_H_

    #ifndef _MOVEMENT_C_
        extern void getVerticalMoves(Mino*);
        extern void getHorizontalMoves(Mino*);

        extern void getAvailableMoves();

        extern int moveHorizontal(Mino*, int);
        extern int moveVertical(Mino*, int);
    #else
        #include "mino.h"
        void getAvailableMoves(void);
        void getVerticalMoves(Mino*);
        void getHorizontalMoves(Mino*);

        int moveVertical(Mino*,int);
        int moveHorizontal(Mino*,int);

        static int revField(int);
        static int moveLeft(Mino*,int);
        static int moveRight(Mino*,int);
    #endif
#endif