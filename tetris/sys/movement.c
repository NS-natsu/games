#define _MOVEMENT_C_
#include "movement.h"

static int revField(int n){
    return reverse(n) >> 6;
}

void getVerticalMoves(Mino * m){
    int h = 0;

    for(int i = 0; i< 4; i++)
        if(m->d[i]) h = i + 1;

    //ブロックが他のブロックか壁に触れる
    for(m->b = m->y; m->b < FIELD_HEIGHT - h; m->b++){
        if( (field[m->b + 1] & m->d[0])
        ||  (field[m->b + 2] & m->d[1])
        ||  (field[m->b + 3] & m->d[2])
        ||  (field[m->b + 4] & m->d[3])
        ) break;
    }
    m->b -= m->y;
    return;
}

void getHorizontalMoves(Mino * m){
    int l,r,tmp;
    m->r = m->l = FIELD_WIDTH;

    for(int i = 0; i < 4; i++){
        l = (m->o >> (12 - 4*i)) & 0xf;

        if(!l) continue;
        if(m->y + i >= FIELD_HEIGHT) break;

        r = (l * 17) >> 2;
        r = (0xa & (r << 1)) | (0x5 & (r >> 1));

        l &= -l; r &= -r;

        //left
        if(0 <= m->x) l <<= m->x;
        else l >>= -m->x;

        tmp = rBit((l-1) & field[i + m->y]) << 1;
        tmp |= !tmp;
        l = bitCnt(l - tmp);
        if(l < m->l) m->l = l;
        
        //right
        if(0 <= 6 - m->x) r <<= 6 - m->x;
        else r >>= m->x - 6;
        tmp = rBit((r-1) & revField(field[i + m->y])) << 1;
        tmp |= !tmp;

        r = bitCnt(r - tmp);
        if(r < m->r) m->r = r;
    }
}

void getAvailableMoves(void){
    setMino();
    getVerticalMoves(&mino);
    getHorizontalMoves(&mino);
}

static int moveLeft(Mino* m, int val){
    if(m->l < val) val = m->l;

    m->l -= val;
    m->r += val;
    m->x -= val;

    for(int i = 0; i < 4; i++)
        { m->d[i] >>= val; }

    return val;
}
static int moveRight(Mino* m, int val){
    if(m->r < val) val = m->r;

    m->l += val;
    m->r -= val;
    m->x += val;

    for(int i = 0; i < 4; i++)
        { m->d[i] <<= val; }

    return val;
}

int moveHorizontal(Mino* m, int val){
    if(val == 0) return 0;

    return (0 < val) ? moveRight(m, val) : moveLeft(m, -val);
}

int moveVertical(Mino* m, int val){
    if(val <= 0) return 0;
    if(m->b < val) val = m->b;

    m->b -= val;
    m->y += val;

    return val;
}