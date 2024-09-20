#include <stdio.h>

int main(){
    FILE *ifp, *ofp;
    char ifn[2] = {'1', 0};
    char ofn[7] = {'m', '1', '.', 't', 'x', 't', 0};
    char *eof = "\0";
    unsigned char buff[2];
    for(int i = 0; i < 7; i++){
        ifp = fopen(ifn, "rb");
        ofp = fopen(ofn, "w");
        ofp = freopen(ofn, "r+", ofp);

        while( fread(buff, 1, 1, ifp) ){
            fprintf(ofp, "%#x,", *buff);
        }

        fseek(ofp, -1L, SEEK_CUR);
        fprintf(ofp, " ", *buff);

        fclose(ofp);
        fclose(ifp);
        ifn[0] += 1;
        ofn[1] += 1;
    }
}