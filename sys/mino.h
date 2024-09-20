#ifndef _MINO_H_
    #define _MINO_H_

    #include "bitlib.h"

    #define NOM 7
    #define FIELD_WIDTH 10
    #define FIELD_HEIGHT 20

    typedef struct {
        int x;//offset x
        int y;//offset y
        char t;//type
        int o;//mData Object
        int d[4];//data
        char r;//right margin
        char l;// left margin
        char b;//bottom margin
        char rot;//rotate
        char rt;//rotate type
        char h; //use hold
    } Mino;

    #ifndef _MINO_C_
        extern Mino mino;
        extern int * const field;

        extern int isExist(Mino*);
        extern int getNextMino(Mino*);

        extern int getCornersState(Mino*, int*);
        extern void installateField(Mino*, int*);


        extern void setMino();
        extern void _setMino(Mino*);

        extern char* getColor(int);

        extern int getHold();
        extern int changeHold();
    #else
        #include <string.h>

        #include "bag.h"
        #include "movement.h"
        
        Mino mino = {0, 0, 0, 0, {0}, 0, 0, 0, 0, 0, 0};

        static int _field[FIELD_HEIGHT + 4] = {0};
        static char fColor[FIELD_HEIGHT + 4][FIELD_WIDTH] = {0};

        int hold = 0;
        int * const field = _field + 4;

        static int const minos[NOM] = {
            0b0000001101100000,
            0b0000011000110000,
            0b0000000101110000,
            0b0000010001110000,
            0b0000001001110000,
            0b0000111100000000,
            0b0000011001100000,
        };
        
        static void initMinoStat(Mino*);
        static void updateField(void);
        static int _changeHold(Mino*);

        int changeHold();
        int isExist(Mino*);
        void _setMino(Mino*);
        int getNextMino(Mino*);
        void installateField(Mino*, int*);
        
        void setMino()
            {_setMino(&mino);}
        char* getColor(int col)
            { return fColor[col + 4]; }
        int getHold()
            { return hold; }
    #endif
#endif