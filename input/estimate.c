#define _ESTIMATE_C_
#include "interface.h"

int moveD(Observe* ob, int n){
    int score, ret = 1;
    if( score = moveVertical(&mino, 1 | (FIELD_HEIGHT * n)) ) {
        getHorizontalMoves(&mino);
        
        //softDrop +1 per 1line
        //HardDrop +2 per 1line
        if(statI.repeatBottom){
            score <<= 1;
        } else if(!(statI.repeatDown)){
            score = 0;
        }
        ob->log.score += score;

        if(ob->log.arrivalDepth < mino.y){
            ob->log.arrivalDepth = mino.y;
            ob->log.ground = 0;
    
            if(mino.b == 0){
                ob->log.ground = 16;
                statI.waitDown = ROCKDOWN;
            }
        }
    }else ret = 2;

    return ret;
}

int moveH(Observe* ob, int n){
    int ret = 1;

    if(moveHorizontal(&mino, (n << 1) - 1)){
        getVerticalMoves(&mino);
        mino.rt = 0;
        
        if(1 < ob->log.ground){
            ob->log.ground -= 1;
            statI.waitDown = ROCKDOWN;
        }
    }else ret = 0;

    return ret;
}

static int getScore(Observe *ob, int lines, int tspin, int allClear){
    //tspinの得点
    int s = lines;
    if(tspin == 2) s += 5;
    if(s == 0) return 0;

    int score = scorecard[s];

    if(ob->log.btb && (4 <= s)) {
        score = (score * 3) / 2;
    }

    if(score) {
        score += scorecard[0] * (ob->log.ren);
        if(allClear && lines){
            score += additional[lines];
        }
        if(tspin == 1) {
            score += additional[0];
        }
    }
    return score;
}

int evaluate(Observe *ob){
    int lines = 0;
    int l[FIELD_HEIGHT] = {0};

    installateField(&mino, l);

    //tミノについて、tスピンの条件を事前に判定する
    //1.ミノ固定時にＴミノの４隅が３つ以上埋まっていること。
    //2.最後の動作が回転であること。

    //tスピンミニの条件
    //1.tスピンの条件を満たしていること。
    //2.凸側が一つ空いていること
    //3.SRSの回転補正について、中心から2以上離れた場所でないこと。

    //tミノは下4x4のデータのうち3マスのみ使用している
    /* 0 3マス未満
    *  1 t-spin mini
    *  2 t-spin
    */
    int tSpin = 0;
    if(mino.t == 5) {
        tSpin = getCornersState(&mino, l);
        if(ob->log.before != BO_ROTATE) tSpin = 0;
        if(tSpin == 1 && 3 <= mino.rt) tSpin = 0;
    }

    int isAllClear = 0;
    for(int i = 0; i < FIELD_HEIGHT; i++){
        isAllClear += !(l[i]);
        l[i] = (l[i] == 0x3ff);
        isAllClear += l[i];
        lines += l[i];
    }
    isAllClear = (isAllClear == FIELD_HEIGHT);
    eraseLines(l);
    ob->log.lines += lines;
    ob->log.level = 1 + (ob->log.lines / 10);
    if(15 <= ob->log.level) ob->log.level = 15;

    //スコア計算
    int score = getScore(ob, lines, tSpin, isAllClear);

    if(ob->log.btb && (lines == 4 || tSpin == 2)){
        lines |= 16;
    }
    if(!score){
        ob->log.ren = 0;
        ob->log.btb = 0;
    } else {
        //ラインを消さなくても維持?
        //ob->log.btb = (!lines || 4 <= lines || tSpin);
        ob->log.btb = (4 == (15&lines) || tSpin);
        ob->log.ren += (lines != 0);
    }

    ob->log.score += score;
    if(999999 < ob->log.score){
        ob->log.score = 999999;
    }

    lines += 5 * (1 <= tSpin);
    lines += 3 * (tSpin == 2);
    return lines;
}