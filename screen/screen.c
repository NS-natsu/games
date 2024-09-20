#define _SCREEN_C_
#include "screen.h"

int setupScreen(void){
    FILE *fp = fopen("screen/line", "rb");
    if(fp != NULL){
        fread(offsetLine, 1, 10, fp);
        fread(line[0], 1, 181, fp);
        fread(line[1], 1, 181, fp);
        fread(line[2], 1, 181, fp);
        fread(line[3], 1, 181, fp);
        fclose(fp);
    } else {
        printf("error");
    }
    
    fp = fopen("screen/mino/colorData", "rb");
    int n;
    unsigned char buff[4];

    if(fp != NULL){
        fread(buff, 1, 1, fp);
        n = *buff;
        fread(colors, 1, n*3, fp);
        fclose(fp);
    } else {
        printf("error");
    }

    fp = fopen("screen/offset", "rb");

    if(fp != NULL){
        fread(buff, 1, 1, fp);
        n = *buff;

        for(int i = 0; i < n; i++){
            fread(buff, 1, 2, fp);
            memcpy(offset[i], buff, 2);
        }

        fclose(fp);
    } else {
        printf("error");
    }
}

void setString(int type){
    txt.alive_time = 48;
    sprintf(txt.string, "\x1B[%02d;%02dH%s\x1B[%02d;%02dH%s",
            offset[9][0], offset[9][1],textTemplate[type & 0xf],
            offset[9][0]+1, offset[9][1], (type & 16) ? textTemplate[11] : "                  ");
}

void eraseLines(int *l){
    for(int i = 0; i < FIELD_HEIGHT; i++){
        lineSkin[i] = l[i];
    }
}

void nextLineSkin(void){
    for(int i = 0; i < FIELD_HEIGHT; i++){
        lineSkin[i] += (0 < lineSkin[i]);
        lineSkin[i] %= 4;
    }
}

void draw(Mino *m, Observe *ob, int update){
    if(update & U_BACKGROUND){
        system("screen\\background.bat");
    }

    if(update & U_HOLD){
        int hold = getHold();

        fprintf(stdout, ESCAPE_SEQUENCE,
        offset[0][0] + 1, offset[0][1] + 1,
        colors[hold], minoData[hold]);
    }

    if(update & U_NEXT){
        int next[4] = {0,0,0,0};
        getNextMinoList(next, 4);
        for(int i = 0; i < 4; i++){
            fprintf(stdout, ESCAPE_SEQUENCE,
            offset[i + 1][0] + 1, offset[i + 1][1] + 1,
            colors[next[i]], minoData[next[i]]);
        }
    }

    if(update & U_FIELD){
        for(int i = 0; i < FIELD_HEIGHT; i++){
            char *color = getColor(i);

            for(int j = 0; j < 10; j++){
                line[lineSkin[i]][offsetLine[j] + 0] = colors[color[j]][0];
                line[lineSkin[i]][offsetLine[j] + 1] = colors[color[j]][1];
                line[lineSkin[i]][offsetLine[j] + 2] = colors[color[j]][2];
                
                //line[lineSkin[i]][offsetLine[j] + 8] = '0' + !!color[j];
            }
            
            printf("\033[%d;%dH%s", i + offset[5][0], offset[5][1], line[lineSkin[i]]);
        }
    }

    if(update & U_MINO){
        drawMino(m);
    }

    /*score*/
    printf("\033[0m\033[%d;%dH%10d", offset[6][0], offset[6][1], ob->log.score);
    printf("\033[%d;%dH%6d", offset[7][0], offset[7][1], ob->log.level);
    printf("\033[%d;%dH%6d", offset[8][0], offset[8][1], ob->log.lines);

    drawTxt();
    printf("\033[23;1H\033[0m");

    printf("%3d\t %3d\n", m->x, m->y);
    printf("%3d\t %3d\n", m->l, m->r);
    printf("%3d\t %3d\n", m->rot, m->b);
    printf("%3d\t %10llu\n", m->t, ob->log.flameElapse);
    printf("%3d\t %3d\n", ob->log.ground, ob->state);
    printf("%3d\t %3d", ob->log.arrivalDepth, ob->waitDown);
}

static void drawMino(Mino *m){
    //操作中のミノの落下予想位置
    fprintf(stdout, "\033[38;5;%.3sm",colors[m->t]);

    char str[64] = {0};
    int off = 0, offY = m->y + m->b;
    for(int y = offY; y - offY < 4; y++){
        if(y < 0) continue;

        for(int x = 0; x < FIELD_WIDTH; x++){
            if( !(1 & (m->d[y - offY] >> x)) ) continue;

            sprintf(str + off, "\033[%02d;%02dH", y + offset[5][0], (x<<1) + offset[5][1]);

            off += 8;
            str[off + 0] = str[off + 3] = 0xe2;
            str[off + 1] = str[off + 4] = 0x96;
            str[off + 2] = str[off + 5] = 0x93;
            off += 6;
        }
    }
    printf("%s", str);
    
    //操作中のミノ
    memset(str, 0, 64);
    off = 0, offY = m->y;
    for(int y = offY; y - offY < 4; y++){
        if(y < 0) continue;

        for(int x = 0; x < FIELD_WIDTH; x++){
            if( !(1 & (m->d[y - offY] >> x)) ) continue;

            sprintf(str + off, "\033[%02d;%02dH", y + offset[5][0], (x<<1) + offset[5][1]);

            off += 8;
            str[off + 0] = str[off + 3] = 0xe2;
            str[off + 1] = str[off + 4] = 0x96;
            str[off + 2] = 0x88; str[off + 5] = 0x89;
            
            if(x == FIELD_WIDTH - 1) str[off + 5]--;
            off += 6;
        }
    }
    printf("%s", str);
}

static void drawTxt(void){
    if(--(txt.alive_time) <= 0){
        txt.alive_time = 0;
        sprintf(txt.string, "\x1B[%02d;%02dH                  \x1B[%02d;%02dH                  ",
                offset[9][0], offset[9][1],offset[9][0]+1, offset[9][1]);
    }

    fprintf(stdout, "\x1B[38;5;51m\x1B[48;5;247m%s", txt.string);
}