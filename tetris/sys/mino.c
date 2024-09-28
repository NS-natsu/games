#define _MINO_C_
#include "mino.h"

void _setMino(Mino *m){   
    for(int i = 0; i < 4; i++){
        m->d[i] = (m->o >> (12 - 4*i)) & 0xf;

        if(0 <= m->x) m->d[i] <<= m->x;
        else m->d[i] >>= -m->x;
    }
}

int getCornersState(Mino* m, int* f){
    int ret = 0;
    int square = 0;
    if(m->y + 1 < FIELD_HEIGHT){
        ret = f[m->y + 1];
    } else ret = -1;

    if(0 < m->x)square = (ret >> m->x) & 5;
    else        square = (ret << m->x) & 5;
    square <<= 6;
    if(m->y + 3 < FIELD_HEIGHT){
        ret = f[m->y + 3];
    } else ret = -1;
    if(0 < m->x)square |= (ret >> m->x) & 5;
    else        square |= (ret << m->x) & 5;
    
    static const int c[4] = {
        0b101000000,
        0b100000100,
        0b000000101,
        0b001000001,
    };
    switch(bitCnt(square)){
        case 4: ret = 2; break;
        case 3://隅が3つ埋まっている
            ret = 1;
            //凸の横は埋まっている
            ret += ((square&c[m->rot]) == c[m->rot]);
        break;
        default: ret = 0; break;
    }
    return ret;
}

static void initMinoStat(Mino *m){
    m->x = 3;
    m->y = -1;
    m->o = minos[m->t - 1];
    m->h = 0;
    m->rot = 0;
    m->rt = 0;
    _setMino(m);
}

int isExist(Mino *m){
    int p = 0;
    for(int i = 0; i < 4; i++){
        p |= (m->o >> (12 - 4*i)) & 0xf;
    }

    int l = bitCnt((p&(-p)) - 1);

    p = reverse(p);
    int r = bitCnt(reverse(p&(-p)) - 1);

    //枠内にあるか
    if(m->x + l < 0) return 0;
    if(FIELD_WIDTH <= m->x + r) return 0;

    _setMino(m);

    int b = 0;
    for(int i = 0; i< 4; i++)
        if(m->d[i]) b = i;
        
    if(FIELD_HEIGHT <= m->y + b) return 0;
    
    //重なっているか

    if(field[m->y + 0] & m->d[0]) return 0;
    if(field[m->y + 1] & m->d[1]) return 0;
    if(field[m->y + 2] & m->d[2]) return 0;
    if(field[m->y + 3] & m->d[3]) return 0;

    return 1;
}

static void updateField(void){
    for(int i = 0; i < FIELD_HEIGHT; i++){
        if(field[i] != 0x3ff) continue;

        field[i] = 0;
        memmove(_field + 1, _field, sizeof(int) * (4 + i));
        *_field = 0;

        for(int j = i + 4; 0 < j; j--){
            memcpy(fColor[j], fColor[j - 1], sizeof(char) * FIELD_WIDTH);
        }
        memset(fColor[0], 0, sizeof(int) * FIELD_WIDTH);
    }
}

void installateField(Mino *m, int *l){
    if(m->t == 0) return;
    for(int i = m->y; i < m->y + 4 ; i++){
        field[i] |= m->d[i - m->y];

        for(int j = 0; j < FIELD_WIDTH; j++){
            if( !((m->d[i - m->y] >> j) & 1) ) continue;
            fColor[i + 4][j] = m->t;
        }
    }

    memcpy(l, field, sizeof(int) * FIELD_HEIGHT);
}

int getNextMino(Mino *m){
    updateField();
    m->t = getNext();
    initMinoStat(m);
    return m->t;
}

static int _changeHold(Mino *m){
    if(m->h) return 0;

    if(hold == 0) {
        hold = m->t;
        getNextMino(m);
    }else {
        m->t ^= hold ^= m->t ^= hold;
        initMinoStat(m);
        m->h = 1;
    }
    return 1;    
}

int changeHold(){
    int ret =  _changeHold(&mino);
    if(ret) getAvailableMoves();
    return ret;
}