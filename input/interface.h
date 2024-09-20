#ifndef _INPUT_H_
    #define _INPUT_H_

    #define BO_OTHER 0
    #define BO_ROTATE 1
    #define BO_WIDEMOVE 2

    typedef struct{
            char hMove;
            char vMove;
            char rot;
            char hold;
    } Operate;

    typedef struct {
        int isSleep;
        int reload;
        Operate ope;
        struct LogData{
            char btb;
            char before;
            char ground;
            char arrivalDepth;

            int installate;
            unsigned int ren;
            unsigned int level;
            unsigned int lines;
            unsigned int score;

            unsigned long long int flameElapse;
        } log;
    } Observe;
    
    #if defined(_INTERFACE_C_) | defined(_ESTIMATE_C_) 
        #include "../sys/mino.h"
        #include "../sys/movement.h"
        #include "../screen/screen.h"

        #define ROCKDOWN 30

        struct _statI{
            char waitLeft;  char repeatLeft;
            char waitRight; char repeatRight;
            char waitDown;
            char repeatDown;    
            char repeatBottom;
            char repeatRot;
            char repeatHold;

            char repeatReload;
        };
    #endif

    #ifndef _INTERFACE_C_
        extern int input(Observe*);
        extern void setupInterface(Observe*);
        extern int operation(Observe*,int*);
    #else
        #include <windows.h>
        #include "../sys/rotate.h"
        
        #define GetKey(k) (GetAsyncKeyState(k) & 0x8000)
        
        extern int evaluate(Observe*);
        extern int moveH(Observe*, int);
        extern int moveD(Observe*, int);

        static char hInterval[2] = {10, 3}; 
        static char fInterval[16] = {
            ROCKDOWN, 48, 44, 40, 36, 32,
            28, 24, 20, 16, 12, 8, 6, 4, 2, 1
        };
        struct _statI statI = {0};

        int input(Observe*);
        int operation(Observe*,int*);
        void setupInterface(Observe*);
    #endif

    #ifndef _ESTIMATE_C_
    #else
        extern struct _statI statI;
        
        //基本スコア
        //ren,
        //single, double, triple, tetorys,
        //T-S,T-SSingle, T-SDouble, T-STriple
        int scorecard[9] = {
            50, 100, 300, 500, 800, 400, 800, 1200, 1600
        };

        //追加スコア
        //T-SMini
        //A-Csingle, A-Cdouble, A-Ctriple, A-Ctetorys,
        static const int additional[5] = {
            100, 800, 1000, 1800, 2000
        };

        int evaluate(Observe*);
        int moveH(Observe*, int);
        int moveD(Observe*, int);

        static int getScore(Observe*,int,int,int);
    #endif

#endif