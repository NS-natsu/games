#ifndef _BAG_H_
    #define _BAG_H
    
    #ifndef _BAG_C_
        extern int getNext(void);
        extern void setupList(void);
        extern void getNextMinoList(int*, int);
    #else
        #include <stdlib.h>
        #include <string.h>
        #include "mino.h"
        
        static int list[NOM * 2] = {0};

        int getNext(void);
        void setupList(void);

        static void getNextList(void);
    #endif
#endif