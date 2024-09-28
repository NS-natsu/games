#define _TET_C_
#include "tet.h"

int init(Observe *ob){
    srand(time(NULL));
    if(setupScreen() == -1) return -1;
    
    setupList();
    setupInterface(ob);

    if(getNextMino(&mino) == 0) return -1;
    getAvailableMoves();

    system("cls");
}

void waitusec(struct timeval *start){
    int msec;
    struct timeval end;
    gettimeofday(&end, NULL);
    msec  = (end.tv_sec - start->tv_sec) * 1000;
    msec += (end.tv_usec - start->tv_usec) / 1000;
    msec = FLAME_TIME - msec;
    if(0 < msec) Sleep(msec);
    gettimeofday(start, NULL);
}

static void exitProgram(void){
    printf("準備に失敗しました\n");
    fprintf(stderr, "準備に失敗しました\n");
    while(kbhit()) _getch();
    while(!kbhit());
}

int main(){

    int chFlag;
    struct timeval start;
    Observe ob = {0};

    if(init(&ob) == -1
        || cInfo.beginConsoleSettings(&cInfo, 80, 30) == -1){
        exitProgram();
        return 0;
    }
    while(kbhit()) _getch();

    draw(&mino, &ob, -1);
    gettimeofday(&start, NULL);
    for(int i = input(&ob); ~i; i = input(&ob)){
        chFlag = 0;
        if(ob.isSleep) {
            waitusec(&start);
            continue;
        }else if(operation(&ob, &chFlag) == -1) break;
        memset(&(ob.ope), 0, sizeof(Operate));

        if(ob.reload) chFlag  = -1;
        draw(&mino, &ob, chFlag);

        waitusec(&start);
        ob.log.flameElapse += 1;
    }
    
    draw(&mino, &ob, -1);

    while(kbhit()) _getch();
    while(!kbhit());
    
    cInfo.endConsoleSettings(&cInfo);
    while(kbhit()) _getch();
    return 0;
}