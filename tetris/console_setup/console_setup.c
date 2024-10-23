#define _CONSOLE_SETUP_C_
#include "console_setup.h"

static void setConsoleSize(CONSOLE_INFO*p, int row, int colmn){
	COORD coord;
	coord.X = row;
	coord.Y = colmn + 1;

	SMALL_RECT rectConsoleSize;
	rectConsoleSize.Left = 0;
	rectConsoleSize.Top = 0;
	rectConsoleSize.Right = 1;
	rectConsoleSize.Bottom = 1;

	// Step1. 一度コンソール表示サイズをぎゅっと小さくする
	SetConsoleWindowInfo(p->console, TRUE, &rectConsoleSize);

	// Step2. バッファサイズを目的の大きさに設定する
	GetConsoleScreenBufferInfo(p->console, &(p->csbi));
	if (!SetConsoleScreenBufferSize(p->console, coord)) {
		return;
	}

	// Step3. ウィンドウサイズを目的の大きさに設定にする
	rectConsoleSize.Right = row - 1;
	rectConsoleSize.Bottom = colmn;
	SetConsoleWindowInfo(p->console, TRUE, &rectConsoleSize);
}
#define _POSIX_SOURCE
static int _begin(void* pCInfo, int row, int colmn){
	CONSOLE_INFO* p = (CONSOLE_INFO*)pCInfo;
	p->console = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleMode(p->console, &(p->mode));
	
	//コマンドプロンプトの色を変えられるようにする
	SetConsoleMode(p->console, p->mode | 0x0004);

	setConsoleSize(p, row, colmn);
	
        printf("\x1B[?25l");

	// Step4: コンソールの出力文字コードをutf-8にする
	char template[] = "tmpfile-chcp-XXXXXX";
	int fd = mkstemp(template);		
	if(fd == -1) return -1;

	char command[128];
	sprintf(command, "chcp >&%d", fd);
	system(command);
	system("chcp 65001");

	FILE *fp = fopen(template, "rb");
	if(fp != NULL){
		fread(command, 1, 128, fp);
		fclose(fp);
		p->chcp = atoi(strchr(command, ':') + 2);
	}

	close(fd);
	remove(template);
	return 0;
}
#undef _POSIX_SOURCE
static void _end(void* pCInfo){
	CONSOLE_INFO* p = (CONSOLE_INFO*)pCInfo;
	char command[16];

	sprintf(command, "chcp %d", p->chcp);
	system(command);
	printf("\x1B[0m\x1B[?25h");
	system("cls");
	SetConsoleMode(p->console, p->mode);

	if (!SetConsoleScreenBufferSize(p->console, p->csbi.dwSize)) {
		return;
	}
}
