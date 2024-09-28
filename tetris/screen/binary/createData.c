#include <stdio.h>

int main(){
    FILE *ifp, *ofp;

    char *eof = "\0";
    unsigned char buff[1];
    
    ifp = fopen("line", "rb");
    ofp = fopen("../offsetLine.txt", "w");
    for(int i = 0; i < 10; i++){
        fread(buff, 1, 1, ifp);
        fprintf(ofp, "%#x,", *buff);
    }
    fclose(ofp);

    ofp = fopen("../line.txt", "w");
    for(int i = 0; i < 4; i++){
        fprintf(ofp, "{");
        for(int i = 0; i < 181; i++){
            fread(buff, 1, 1, ifp);
            fprintf(ofp, "%#x,", *buff);
        }
        fseek(ofp, -1L, SEEK_CUR);
        fprintf(ofp, "},");
    }
    fseek(ofp, -1L, SEEK_CUR);
    fclose(ofp);
    fclose(ifp);

    ifp = fopen("offset", "rb");
    ofp = fopen("../offset.txt", "w");
    for(int i = 0; i < 10; i++){
        fprintf(ofp, "{");
        for(int i = 0; i < 2; i++){
            fread(buff, 1, 1, ifp);
            fprintf(ofp, "%d,", *buff);
        }
        fseek(ofp, -1L, SEEK_CUR);
        fprintf(ofp, "},");
    }
    fseek(ofp, -1L, SEEK_CUR);
    fclose(ofp);
    fclose(ifp);
    return 0;
}