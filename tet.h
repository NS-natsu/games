#ifndef _TET_H_
    #define _TET_H_

    #ifdef _TET_C_
        #include <stdio.h>
        #include <conio.h>
        #include <windows.h>
        #include <sys/time.h>
        #include "sys/bag.h"
        #include "sys/mino.h"
        #include "sys/movement.h"
        #include "screen/screen.h"
        #include "console_setup/console_setup.h"

        #define FLAME_TIME ((int)(1000 / 60))

        int init(Observe*);
        void waitusec(struct timeval*);

        static void endProgram(void);
    #endif
#endif