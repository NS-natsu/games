#ifndef _SCREEN_H_
    #define _SCREEN_H_
    
    #include "../input/interface.h"

    #define U_MINO          1
    #define U_HOLD          2
    #define U_NEXT          4
    #define U_FIELD         8
    #define U_BACKGROUND    16

    #ifndef _SCREEN_C_
        extern int setupScreen(void);
        extern void draw(Mino*, Observe*, int);

        extern void setString(int);

        extern void eraseLines(int*);
        extern void nextLineSkin(void);
    #else
        #include <stdio.h>
        #include <stdlib.h>
        #include <string.h>

        #include "../sys/bag.h"
        #include "../sys/mino.h"
        #define ESCAPE_SEQUENCE "\033[%d;%dH\033[38;5;%.3sm%s"

        static unsigned char offset[10][2] = {
            {4, 4}, //hold
            {4, 43}, {8, 44}, {12, 44}, {16, 44}, //next
            {2, 19}, //field
            {12, 5},{13, 9},{15, 9}, //score
            {18, 1} //text
        };
        
        static char colors[8][3];//0 未設置
        static unsigned char line[4][182];
        static unsigned char offsetLine[10] = {0};

        static int lineSkin[FIELD_HEIGHT] = {0};

        static const unsigned char minoData[NOM + 1][41] = {
            {
                #include "mino/m0.txt"
            },{
                #include "mino/m1.txt"
            },{
                #include "mino/m2.txt"
            },{
                #include "mino/m3.txt"
            },{
                #include "mino/m4.txt"
            },{
                #include "mino/m5.txt"
            },{
                #include "mino/m6.txt"
            },{
                #include "mino/m7.txt"
            }
        };
        
        struct {
            char string[64];
            int alive_time;
        } txt = {0};

        static const char textTemplate[12][20] = {
            "Single            ",
            "Double            ",
            "Triple            ",
            "TETORiS           ",
            "T-Spin Mini       ",
            "T-Spin Mini Single",
            "T-Spin Mini Double",
            "T-Spin            ",
            "T-Spin Single     ",
            "T-Spin Double     ",
            "T-Spin Triple     ",
            "Back-To-Back      "
        };
        
        int setupScreen(void);
        
        void setString(int);
        void eraseLines(int*);
        void nextLineSkin(void);
        void draw(Mino*,Observe*,int);

        static void drawTxt(void);
        static void drawMino(Mino*);
    #endif
#endif