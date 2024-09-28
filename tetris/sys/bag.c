#define _BAG_C_
#include "bag.h"

static void getNextList(void){
    int offset = NOM + rand() % NOM;
    for(int mino = 1; mino < 8; offset = NOM + rand() % NOM){
        if(list[offset] == 0) list[offset] = mino++;
    }
}

int getNext(void){
    int next = list[0];
    memmove(list, list + 1, sizeof(int) * (2 * NOM - 1));
    list[2 * NOM - 1] = 0;
    if(list[NOM] == 0) getNextList();
    return next;
}

void setupList(void){
    int offset = rand() % NOM;
    for(int i = 0; i < 2; i++){
        for(int mino = 1; mino <= NOM; offset = rand() % NOM){
            if(list[offset + i * NOM]) continue;
            
            list[offset + i * NOM] = mino++;
        }
    }
}

void getNextMinoList(int *l, int cnt){
    memcpy(l, list, sizeof(int) * cnt);
}