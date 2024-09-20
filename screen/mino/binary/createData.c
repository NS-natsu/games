#include <stdio.h>

int main(){
    FILE *ifp, *ofp;
    char ifn[6] ={'1','.','b','i','n',0};
    char ofn[10] = {'.','.','/','m','1','.','t','x','t',0};
    char *eof = "\0";
    unsigned char buff[2];
    for(int i = 0; i < 7; i++){
        ifp = fopen(ifn, "rb");
        ofp = fopen(ofn, "w");

        while( fread(buff, 1, 1, ifp) ){
            fprintf(ofp, "%#x,", *buff);
        }

        fseek(ofp, -1L, SEEK_CUR);
        fprintf(ofp, " ", *buff);

        fclose(ofp);
        fclose(ifp);
        ifn[0] += 1;
        ofn[4] += 1;
    }

    ifp = fopen("colorData.bin", "rb");
    ofp = fopen("../colorData.txt", "w");
    fread(buff, 1, 1, ifp);
    for(int i = *buff; i; i--){
        fprintf(ofp, "{");
        for(int j = 0; j < 3; j++){
            fread(buff, 1, 1, ifp);
            fprintf(ofp, "%#x,", *buff);
        }
        fseek(ofp, -1L, SEEK_CUR);
        fprintf(ofp, "},");
    }
    fseek(ofp, -1L, SEEK_CUR);
    fprintf(ofp, " ", *buff);

    fclose(ofp);
    fclose(ifp);
    return 0;
}