#ifndef _ROTATE_H_
    #define _ROTATE_H_

    #ifndef _ROTATE_C_
        extern int rotate(int);
    #else
        #include <string.h>

        #include "mino.h"
        #include "movement.h"
        
        typedef struct {
            char x;
            char y;
        } Coord;

        static const Coord srs[4][5] = {
            {{ 0, 0},{-1, 0},{-1, 1},{ 0,-2},{-1,-2}},//0
            {{ 0, 0},{-1, 0},{-1,-1},{ 0, 2},{-1, 2}},//90
            {{ 0, 0},{ 1, 0},{ 1, 1},{ 0,-2},{ 1,-2}},//180
            {{ 0, 0},{ 1, 0},{ 1,-1},{ 0, 2},{ 1, 2}} //270
        };
        static const Coord srsI[4][5] = {
            {{ 0, 0},{-2, 0},{ 1, 0},{ 1, 2},{-2,-1}},//0
            {{ 0, 0},{-2, 0},{ 1, 0},{-2, 1},{ 1,-2}},//90
            {{ 0, 0},{-1, 0},{ 2, 0},{-1,-2},{ 2, 1}},//180
            {{ 0, 0},{ 2, 0},{-1, 0},{ 2,-1},{-1, 2}} //270
        };

        int rotate(int);
        static int _rotate(int,Mino*);
        static int _rotateI(int,Mino*);
        static int _rotateA(int,Mino*);
    #endif
#endif