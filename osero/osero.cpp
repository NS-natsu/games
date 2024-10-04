#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

static const int boardsize = 8;
static const int hval = 4;
static const int wsize = 2 * hval;
static const int nextheight = boardsize * wsize + 1;
static const int hsize = hval * nextheight;
static const int turn_motion_count = 6;
static const int Stime = 100;

static int black=0, white=0, player = 1, pass = 0, labor = 1, _CPU = 1;
static bool isCPU = true; 
static bool Playing_game_records = false;

static const int scan[8] =	{ -1 * (boardsize + 3),	-1 * (boardsize + 2), -1 * (boardsize + 1),
												-1,											 1,
								   (boardsize + 1),		 (boardsize + 2),	   (boardsize + 3)
};

static int board[boardsize+2][boardsize+2] = {0};
static char _screen[(nextheight*hval*boardsize)+1] = { 0 };

class save {
	FILE *tmp1, *tmp2;
public:
	save() {
		tmp1 = tmpfile();
		tmp2 = tmpfile();
	}
	~save() {
		fclose(tmp1);
		fclose(tmp2);
	}
	void set(int ver, int side) {
		FILE *fp;
		int buff = 0;
		char place[3] = { 0 };
		fp = tmpfile();
		fseek(tmp1, 0, SEEK_SET);
		for (int i = 0; i < labor-1; i++) {
			buff = fgetc(tmp1);
			fputc(buff,fp);

			buff = fgetc(tmp1);
			fputc(buff, fp);
		}
		fclose(tmp1);
		tmp1 = fp;
		place[0] = 'A' + side - 1;
		place[1] = '0' + ver;
		fprintf(tmp1, "%s", place);
		fprintf(tmp2, "%s", place);
	}
	void dicision() {
		FILE *fp;
		char filename1[256] = { 0 };
		char filename2[256] = { 0 };
		time_t timer = time(NULL);
		struct tm timeptr = { 0 };
		errno_t error;
		int buff = 0;

		error = localtime_s(&timeptr, &timer);

		strftime(filename1, 256, "オセロ棋譜_%Y年%m月%d日,%H時%M分%S秒.txt", &timeptr);
		strftime(filename2, 256, "オセロ棋譜_%Y年%m月%d日,%H時%M分%S秒_デバッグ.txt", &timeptr);

		fopen_s(&fp, filename1, "w");
		rewind(tmp1);
		while ((buff=fgetc(tmp1)) != EOF) fputc(buff, fp);

		fclose(fp);


		fopen_s(&fp, filename2, "w");
		rewind(tmp2);
		while ((buff = fgetc(tmp2)) != EOF) fputc(buff, fp);

		fclose(fp);
	}
	void und() {
		fseek(tmp1, -4, SEEK_CUR);

		fprintf(tmp2, "Z0");
	}
	void red() {
		fgetc(tmp1); fgetc(tmp1);
		fgetc(tmp1); fgetc(tmp1);

		fprintf(tmp2, "Y0");
	}
} record;
class Undo_Redo {
	FILE *tmp;
	int _labor;
public:
	Undo_Redo() {
		tmp = tmpfile();
		_labor=0;
	}
	~Undo_Redo() {
		fclose(tmp);
	}
	void Do() {
		FILE *fp = tmpfile();
		char buff[(boardsize + 2)*(boardsize + 2)+2] = { 0 };
		_labor = labor;
		fseek(tmp, 0, SEEK_SET);
		for (int i = 0; i < _labor; i++) {
			fgets(buff, sizeof(buff), tmp);
			fprintf(fp, "%s", buff);
		}
		fclose(tmp);
		tmp = fp;
		for (int i = 0; i < boardsize + 2; i++)
			for (int j = 0; j < boardsize + 2; j++)
				fprintf(tmp, "%d", board[i][j]);
		fprintf(tmp, "\n");
	}
	void undo() {
		if (2 < labor) {
			labor -= 2;
			if( labor == _labor ) fseek(tmp, -3 * ((boardsize + 2)*(boardsize + 2) + 1), SEEK_END);
			else fseek(tmp, -3 * ((boardsize + 2)*(boardsize + 2) + 1), SEEK_CUR);
			for (int i = 0; i < boardsize + 2; i++)
				for (int j = 0; j < boardsize + 2; j++)
					board[i][j] = fgetc(tmp) - '0';
			fgetc(tmp);
			record.und();
		}
	}
	void redo() {
		if (labor < _labor) {
			labor += 2;
			for (int Re = 2; Re; Re--) {
				for (int i = 0; i < boardsize + 2; i++)
					for (int j = 0; j < boardsize + 2; j++)
						board[i][j] = fgetc(tmp) - '0';
				fgetc(tmp);
			}
			record.red();
		}
	}
} his;
class playinggame {
	FILE *fpg;
public:
	playinggame() {
		char filename[15] = "再生用棋譜.txt";
		fopen_s(&fpg, filename, "r");
	}
	~playinggame() {
		if(fpg!=NULL) fclose(fpg);
	}
	void get(int *ver, int *side) {
		int input = 0;
		input = fgetc(fpg);
		*side = (int)(input - 'A' + 1);
		input = fgetc(fpg);
		*ver = (int)(input - '0');
	}
}PGR;
class CConsole {
	HANDLE		m_hConsoleStdOut;	// 出力コンソールのハンドル
public:
	CConsole() {
		m_hConsoleStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
		SetCursorInfo();
	}
	~CConsole() {
		if (m_hConsoleStdOut != INVALID_HANDLE_VALUE)
			::CloseHandle(m_hConsoleStdOut);
	}
	void SetCursorInfo(void) {
		CONSOLE_CURSOR_INFO		info;
		info.dwSize = (DWORD)10;
		info.bVisible = false;
		::SetConsoleCursorInfo(m_hConsoleStdOut, &info);
	}
	void SetCursorPosition(void) {
		SetCursorInfo();
		COORD	coord;
		coord.X = (short)0; coord.Y = (short)0;
		::SetConsoleCursorPosition(m_hConsoleStdOut, coord);
	}
}G;

#define FILENAME "aa.txt"
#define cursor G.SetCursorPosition()
#define addrec(X,Y) record.set(X,Y)
#define recdic record.dicision()
#define push his.Do()
#define Undo(X,Y) {his.undo(); check(); screen(X,Y);}
#define Redo(X,Y) {his.redo(); check(); screen(X,Y);}
#define GetPlaceInfo(x,y) PGR.get(x,y),input(x,y)

bool check();

void count(void) {
	black = white = 0;
	for (int ver = 1; ver < boardsize+1; ver++) {
		for (int side = 1; side < boardsize + 1; side++) {
			if (board[ver][side] == 1 || (board[ver][side] % 10 == 2 && 10 <= board[ver][side])) black++;
			if (board[ver][side] == 2 || (board[ver][side] % 10 == 1 && 10 <= board[ver][side])) white++;
		}
	}
	return;
}
void _check(int *pos,int *data) {
	if (*pos == 3) {
		for (int search = 0; search < 8; search++) {
			for (int *point = pos;;) {
				point += scan[search];
				data[search]+=1;
				if (*point == player && (point - scan[search]) != pos) { data[search]--; break; }
				else if (*point == player && (point - scan[search]) == pos) {data[search] = 0; break;}
				else if (*point == 0 || *point == 3 || *point == 4) { data[search] = 0; break; }
			}
		}
	}
	return ;
}
int relate(int *pos) {
	if (*pos == 0) {
		for (int search = 0; search < 8; ++search) {
			for (int *point = pos; *point != 4;) {
				point += scan[search];
				if (*point == player && (point - scan[search]) != pos) return 3;
				else if (*point == player && (point - scan[search]) == pos) break;
				else if (*point == 0 || *point == 3) break;
			}
		}
	}
	return *pos;
}
void makepict(int ver, int side) {
	FILE *fp;
	int serect = 0;
	char read[13] = { 0 };
	char seek[13] = { 0 };
	char input[10] = { 0 };
	fopen_s(&fp, FILENAME, "r");
	for (int i = 0; i < boardsize; i++){
		for (int j = 0; j < boardsize; j++) {
			fseek(fp, SEEK_SET, 0);
			serect = board[i + 1][j + 1];
			if (!Playing_game_records && ver != 0 && side != 0 && ver == i+1 && side == j+1) {
				sprintf_s(seek, sizeof(seek), "@@serect%d", (int)(serect % 10)); do { if (fgets(read, sizeof(read), fp) == NULL) break; } while (strncmp(read, seek, 9));
			}else switch (serect % 10) {
			case 0:
				do { if(fgets(read, sizeof(read), fp)==NULL) break; } while (strncmp(read, "@@space",7));
				break;
			case 1:
				if (serect < 10) do { if(fgets(read, sizeof(read), fp)==NULL) break; } while (strncmp(read, "@@nomal_B",9));
				else {
					sprintf_s(seek, sizeof(seek), "@@turn%d_B", (int)(serect / 10)); do { if(fgets(read, sizeof(read), fp)==NULL) break; } while (strncmp(read, seek,9));
				}
				break;
			case 2:
				if (serect < 10) do { if(fgets(read, sizeof(read), fp)==NULL) break; } while (strncmp(read, "@@nomal_H",9));
				else {
					sprintf_s(seek, sizeof(seek), "@@turn%d_H", (int)(serect / 10)); do { if(fgets(read, sizeof(read), fp)==NULL) break; } while (strncmp(read, seek,9));
				}
				break;
			case 3:
				if ((isCPU && player == _CPU) || Playing_game_records) do { if (fgets(read, sizeof(read), fp) == NULL) break; } while (strncmp(read, "@@space", 7));
				else do { if (fgets(read, sizeof(read), fp) == NULL) break; } while (strncmp(read, "@@place", 7));
				break;
			case 4:
				do { if (fgets(read, sizeof(read), fp) == NULL) break; } while (strncmp(read, "@@wall", 6));
				break;
			default: break;
			}
			for (int k = 0; k < hval; k++) {
				if(fgets(input, sizeof(input), fp)==NULL) strcpy_s(input,"mmmmmmmm");
				for (int l = 0; l < wsize; l++) {
					_screen[hsize*i + wsize * j + nextheight * k + l] = input[l];
				}
			}
		}
		for (int k = 0; k < hval; k++) {
			_screen[hsize*i + nextheight * k + wsize*boardsize] = '\n';
		}
	}
	_screen[(nextheight*hval*boardsize)] = '\0';
	fclose(fp);
}
int screen(int ver=0,int side=0) {
	count();
	makepict(ver,side);
	cursor;
	printf_s("%sの番(白:%d,黒:%d),%d手目\n%s", (player == 1) ? "黒" : "白", white, black, labor,_screen);
	Sleep(Stime);
	return 0;
}
bool turn_motion() {
	bool retflag = false;
	for(int i=1;i<boardsize + 1;i++)
		for (int j = 1; j < boardsize + 1; j++) {
			if (10 <= board[i][j] && board[i][j] < turn_motion_count * 10) board[i][j] += 10, retflag = true;
			if (turn_motion_count * 10 <= board[i][j]) board[i][j] = 3 - (board[i][j] % 10);
		}
	return retflag;
}
int turn(int *pos) {
	int *move[8] = { 0 };
	int turnval[8] = { 0 };
	_check(pos, &(turnval[0]));	screen();
	for (int i = 0; i < 8; i++) move[i] = pos;
	*pos = player;
	while ( 0 < turnval[0] || 0 < turnval[1] || 0 < turnval[2] || 0 < turnval[3] || 0 < turnval[4] || 0 < turnval[5] || 0 < turnval[6] || 0 < turnval[7] ) {
		for (int serch = 0; serch < 8; serch++) {
			if (turnval[serch]) {
				turnval[serch]--;
				move[serch] += scan[serch];
				*(move[serch]) += 10;
			}
		}
		screen(); turn_motion();
	}
	do { screen(); } while (turn_motion());
	for (int ver = 1; ver < boardsize + 1; ver++) {
		for (int side = 1; side < boardsize + 1; side++) {
			if(board[ver][side]!=4) board[ver][side] %= 3;
		}
	}
	return 0;
}
int selectofCPU(void) {
	for(int ver=1;ver<boardsize + 1;ver++)
		for(int side=1;side<boardsize + 1;side++)
			if (board[ver][side] == 3) {
				addrec(ver, side);
				return turn(&board[ver][side]);
			}
}
int input(int*ver,int*side,bool wallflag=false) {
		int I = 0;
		screen(*ver, *side);
		while (GetAsyncKeyState(VK_F5) ||
			GetAsyncKeyState(VK_F6) ||
			GetAsyncKeyState(VK_LSHIFT) ||
			GetAsyncKeyState(VK_RSHIFT) ||
			GetAsyncKeyState(VK_RIGHT) ||
			GetAsyncKeyState(VK_LEFT) ||
			GetAsyncKeyState(VK_DOWN) ||
			GetAsyncKeyState(VK_UP) ||
			GetAsyncKeyState(VK_RETURN));
		while (1) {
			if ((I=_getch()) != EOF || wallflag ) {
				_getch();
				if (GetAsyncKeyState(VK_RIGHT)) {
					if (Playing_game_records) return 0;
					*side = *side % boardsize + 1;
					screen(*ver, *side);
					while (GetAsyncKeyState(VK_RIGHT));
				}
				else if (GetAsyncKeyState(VK_LEFT)) {
					if (Playing_game_records) return 0;
					*side = (*side + boardsize - 2) % boardsize + 1;
					screen(*ver, *side);
					while (GetAsyncKeyState(VK_LEFT));
				}
				if (GetAsyncKeyState(VK_DOWN)) {
					if (Playing_game_records) return 0;
					*ver = *ver % boardsize + 1;
					screen(*ver, *side);
					while (GetAsyncKeyState(VK_DOWN));
				}
				else if (GetAsyncKeyState(VK_UP)) {
					if (Playing_game_records) return 0;
					*ver = (*ver + boardsize - 2) % boardsize + 1;
					screen(*ver, *side);
					while (GetAsyncKeyState(VK_UP));
				}
				if (!wallflag && GetAsyncKeyState(VK_F5)) Undo(*ver, *side)
				else if(!wallflag && GetAsyncKeyState(VK_F6)) Redo(*ver, *side)
				if (GetAsyncKeyState(VK_LSHIFT)) return 1;
				if (GetAsyncKeyState(VK_RSHIFT)) return 2;
				if (GetAsyncKeyState(VK_RETURN)) return 3;
				if (GetAsyncKeyState(VK_ESCAPE)) if(wallflag){return 4;}else exit(0);
			}
		}
	};	
int select(bool wallflag=false) {
	if (isCPU && player & _CPU) return selectofCPU();
	int ver=1, side=1;
	if (wallflag) {
		{
			int i = 0;
			while ((i=(int)input(&ver,&side,true)) != 4) {
				switch (i) {
				case 1:
					if (board[ver][side] == 0 || board[ver][side] == 4) board[ver][side] = 1;
					else if (board[ver][side] == 1) board[ver][side] = 0;
					break;
				case 2:
					if (board[ver][side] == 0 || board[ver][side] == 4) board[ver][side] = 2;
					else if (board[ver][side] == 2) board[ver][side] = 0;
					break;
				case 3:
					if (board[ver][side] == 0) board[ver][side] = 4;
					else if (board[ver][side] == 4) board[ver][side] = 0;
					break;
				default: break;
				}
			}
		}
		wallflag = !wallflag;
	}else while (1) {
		if (Playing_game_records) GetPlaceInfo(&ver, &side);
		else input(&ver,&side);
		if (Playing_game_records && side == 26 && ver == 0) Undo(0, 0)
		else if (Playing_game_records && side == 25 && ver == 0) Redo(0, 0)
		if (board[ver][side] == 3) { addrec(ver, side); return turn(&board[ver][side]); }
		screen();
	}
}
bool check() {
	for (int ver = 1; ver < boardsize + 1; ver++) {
		for (int side = 1; side < boardsize + 1; side++) {
			board[ver][side] = relate(&board[ver][side]);
		}
	}
	for (int ver = 1; ver < boardsize + 1; ver++) {
		for (int side = 1; side < boardsize + 1; side++) {
			if (board[ver][side] == 3) return TRUE;
		}
	}
	if (pass == 0) {
		pass = 1;
		return TRUE;
	}
	return FALSE;
}
void progress() {
	if (pass == 1) {
		player = 3 - player;
		if (check()) {
			printf_s("PASS!!");
			Sleep(1000);
			for(int i=0;i<6;i++) printf_s("\b \b");
			pass = 0;
		}
		else Sleep(1000);
		return;
	}
	pass = 0;
	screen(); select();
	++labor;
	screen();
	push;
	player = 3 - player;
	return;
}
void init(){
	for (int i = 0; i < boardsize + 2; i++) {
		board[i][0] = board[0][i] = 4;
		board[i][boardsize + 1] = board[boardsize + 1][i] = 4;
	}
	board[(boardsize / 2)][(boardsize / 2) + 1] = board[(boardsize / 2) + 1][(boardsize / 2)] = 1;
	board[(boardsize / 2)][(boardsize / 2)] = board[(boardsize / 2) + 1][(boardsize / 2) + 1] = 2;
	if (Playing_game_records) isCPU = false;
}
void result(void) {
	count();
	printf("白:%d,黒:%d,%d手\n", white, black, labor - 1);
	if (white == black) printf("引き分け\n");
	printf("%sの勝ち\n", (white > black) ? "白" : "黒");
	if (!Playing_game_records) recdic;
}
int main() {
	init();
	push;
	//if(!Playing_game_records) select(true);
	while (check()) progress();
	result();
	return 0;
}
