#define _ROTATE_C_
#include "rotate.h"

static int _rotateI(int dec, Mino *m){    
    int turn = 0;

    int ky = (( m->rot) & 1) * (dec - 1);    ky |= 1;
    int kx = ((~m->rot) & 1) * (dec - 1);    kx |= 1;
    
    for(; turn < 5; turn++){
        //0 1 2 3 4 dec!=0
        //0 2 1 4 3 dec==0
        int t = (turn - (!dec)) ^ (!dec);
        t = t + (!dec) + (t < 0);

        //kx==-1の時変化あり
        //0 1 2 4 3
        //t += (kx==-1) * ((turn<<1) - 3) * (1&(turn^(turn>>1)));
        if(kx == -1) t += ((turn<<1) - 3) * (1&(turn^(turn>>1)));

        m->x += srsI[m->rot][t].x * kx;
        m->y += srsI[m->rot][t].y * ky;

        if(isExist(m)) break;

        m->x -= srsI[m->rot][t].x * kx;
        m->y -= srsI[m->rot][t].y * ky;
    }

    if(turn < 5) return turn;
    else return -1;
}

static int _rotateA(int dec, Mino *m){
    m->o = (m->o >> (!dec)) >> (2 * dec);

    int turn = 0;

    //0 1 2 3   -> 1 0 1 0 or -1 0 -1 0
    //          -> 1 1 1 1 or -1 1 -1 1
    int k = ((~m->rot) & 1) * (dec - 1); k |= 1;
    for(; turn < 5; turn++){
        m->x += srs[m->rot][turn].x * k;
        m->y += srs[m->rot][turn].y;

        if(isExist(m)) break;

        m->x -= srs[m->rot][turn].x * k;
        m->y -= srs[m->rot][turn].y;
    }

    if(turn < 5) return turn;
    else return -1;
}

static int _rotate(int dec, Mino *m){
    m->o = dec ? rotateLeft4x4(m->o) : rotateRight4x4(m->o);

    dec <<= 1;
    m->rot = (m->rot + 3 + dec) % 4;

    return (m->t == 6) ? _rotateI(dec,m) : _rotateA(dec,m);
}

int rotate(int dec){
    //四角
    if(mino.t == 7) return 0;

    Mino m;
    memcpy(&m, &mino, sizeof(Mino));

    int ret = _rotate(dec, &m);
    if(0 <= ret){
        m.rt = ret;
        memcpy(&mino, &m, sizeof(Mino));
        getAvailableMoves();
    }

    return 0 <= ret;
}