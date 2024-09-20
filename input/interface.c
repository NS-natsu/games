#define _INTERFACE_C_
#include "interface.h"

static int getInput(char a, char b){
    int ret = 1 ^ !GetKey(a);
    return (ret * 3) | (!!GetKey(b) << 1);
}

int input(Observe* ob) {
    if(GetAsyncKeyState(VK_ESCAPE) & 0x8000) return -1;
    if(GetAsyncKeyState('E') & 0x8000){
        ob->isSleep = 0;
    }
    if(ob->isSleep) return 0;
    if(GetAsyncKeyState('Q') & 0x8000){
        ob->isSleep = 1;
        return 0;
    }
    
    ob->reload = 0;
    if(GetAsyncKeyState('U') & 0x8000){
        if(!(statI.repeatReload)){
            ob->reload = 1;
        }
        statI.repeatReload = 1;
    } else {statI.repeatReload = 0;}

    Operate *op = &(ob->ope);
    
    int input = 0;
    int buff = getInput('A', 'D');
    if(buff & 1){//left
        statI.waitRight = statI.repeatRight = 0;
        if(!(statI.repeatLeft)){
            input = 1;
            op->hMove = 1;
            statI.repeatLeft = 1;
            statI.waitLeft = hInterval[0];
        } else if(--(statI.waitLeft) <= 0){
            input = 1;
            op->hMove = 1;
            statI.waitLeft = hInterval[1];
        }
    } else if(buff){//right
        statI.waitLeft = statI.repeatLeft = 0;
        if(!(statI.repeatRight)){
            input = 1;
            op->hMove = 2;
            statI.repeatRight = 1;
            statI.waitRight = hInterval[0];
        } else if(--(statI.waitRight) <= 0){
            input = 1;
            op->hMove = 2;
            statI.waitRight = hInterval[1];
        }
    } else {
        statI.waitLeft = statI.repeatLeft = 
        statI.waitRight = statI.repeatRight = 0;
    }

    buff = getInput('S', 'W');
    if(buff & 1){//soft drop
        statI.repeatBottom = 0;
        if(!(statI.repeatDown)){
            input = 1;
            op->vMove = 1;
            statI.repeatDown = 1;
            statI.waitDown = fInterval[ob->log.level] / 20;
        } else if(--(statI.waitDown) <= 0){
            input = 1;
            op->vMove = 1;
            if(ob->log.ground) statI.waitDown = fInterval[0];
            else statI.waitDown = fInterval[ob->log.level] / 20;
        }
    } else if(buff){//hard drop
        statI.repeatDown = 0;
        if(!(statI.repeatBottom)){
            input = 1;
            op->vMove = 2;
            statI.repeatBottom = 1;
            statI.waitDown = fInterval[0];
        }else if(--(statI.waitDown) <= 0){
            input = 1;
            op->vMove = 1;
            if(ob->log.ground) statI.waitDown = fInterval[0];
            else statI.waitDown = fInterval[ob->log.level];
        }
    } else {
        statI.repeatDown = statI.repeatBottom = 0;
        if(--(statI.waitDown) <= 0){
            input = 1;
            op->vMove = 1;
            if(ob->log.ground) statI.waitDown = fInterval[0];
            else statI.waitDown = fInterval[ob->log.level];
        }
    }

    //右優先
    buff = getInput('C', 'Z');
    if(!buff){
        statI.repeatRot = 0;
        buff = !!GetKey('H');
        if(statI.repeatHold) {
            statI.repeatHold = buff;
        }else if(buff){
            input = 1;
            op->hold = 1;
            statI.repeatHold = 1;
        }
    } else if(statI.repeatRot == 0){
        input = 1;
        op->rot = buff - 1;
        statI.repeatRot = 1;
    }

    if(ob->log.installate) {
        op->rot = 0;
        op->hold = 0;
        op->vMove = 0;
        op->hMove = 0;
        return 0;
    }
    return input;
}

void setupInterface(Observe* ob){
    statI.waitDown = fInterval[1];

    Observe o = {0};
    memcpy(ob, &o, sizeof(Observe));
    ob->log.level = 1;
}

int operation(Observe* ob, int *chFlag){
    int state = 0;
    if(ob->ope.hold){
        state = changeHold();
        *chFlag |= U_HOLD | U_NEXT;
        ob->log.before = BO_OTHER;
        if(state){
            ob->log.ground = 0;
            ob->log.arrivalDepth = 0;
            statI.waitDown = fInterval[ob->log.level];
            if(statI.repeatDown)
                statI.waitDown /= 20;
        }
    } else if(ob->ope.rot){
        state = rotate(ob->ope.rot - 1);
        if(ob->log.arrivalDepth < mino.y){
            ob->log.arrivalDepth = mino.y;
    
            if(ob->log.ground){
                ob->log.ground = 16;
                statI.waitDown = ROCKDOWN;
            }
        } else if(state && 1 < ob->log.ground){
            ob->log.ground -= 1;
            statI.waitDown = ROCKDOWN;
        }
        ob->log.before = BO_ROTATE;
    }else if(ob->ope.hMove){
        state = moveH(ob, ob->ope.hMove - 1);
        ob->log.before = BO_WIDEMOVE;
    }

    if(ob->log.ground == 1) {
        ob->log.ground = 0;
        ob->ope.vMove = 1;
    }

    if(ob->ope.vMove){
        state = moveD(ob, ob->ope.vMove - 1);
    }

    if(state == 2) {
        int val = evaluate(ob);
        if(val){
            ob->log.installate = 30;
            setString(val - 1);
        } else ob->log.installate = 5;
    }

    if(ob->log.installate){
        switch(ob->log.installate){
            case 1:
                *chFlag |= U_NEXT | U_MINO;
                getNextMino(&mino);
                ob->log.ground = 0;
                ob->log.arrivalDepth = 0;
                if(isExist(&mino)) getAvailableMoves();
                else state = -1;
            if(0)case 20:;
            if(0)case 10:;
                nextLineSkin();
                break;
            default:
                break;
        }
        ob->log.installate -= 1;
        *chFlag |= U_FIELD;
    } else if(state) {
        *chFlag |= U_MINO | U_FIELD;
    }

    return state;
}