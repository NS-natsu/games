#include <stdio.h>
#include <conio.h>
#include <windows.h>
#define FILENAME "charactor.txt"
#define CHARMAX	  4
#define SPACE	 10
#define NEXT	 50
#define SIDE	SPACE+NEXT*7
#define TOP		SPACE+NEXT*8
#define SELECT	SPACE+NEXT*9
FILE *graf;
struct Stat {
	unsigned int doKing : 1;
	unsigned int dorook : 1;
	unsigned int nocheck : 1;
	unsigned int canLcastling : 1;
	unsigned int canRcastling : 1;
	unsigned int ischeck : 1;
	unsigned int isstale : 1;
	unsigned int dopawn : 1;
} stat[2];
enum Color {
	L_BLACK,	// 低輝度・黒色
	L_BLUE,		// 低輝度・青色
	L_GREEN,	// 低輝度・緑色
	L_CYAN,		// 低輝度・水色
	L_RED,		// 低輝度・赤色
	L_PURPLE,	// 低輝度・紫色
	L_YELLOW,	// 低輝度・黄色
	L_WHITE,	// 低輝度・白色

	H_BLACK,	// 高輝度・黒色
	H_BLUE,		// 高輝度・青色
	H_GREEN,	// 高輝度・緑色
	H_CYAN,		// 高輝度・水色
	H_RED,		// 高輝度・赤色
	H_PURPLE,	// 高輝度・紫色
	H_YELLOW,	// 高輝度・黄色
	H_WHITE,	// 高輝度・白色
};
class CConsole {
HANDLE		m_hConsoleStdOut;	// 出力コンソールのハンドル
public:
	CConsole() {
		m_hConsoleStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
		//SetWindowSize(45, 40);
		Sleep(5000);
		SetCursorInfo(25, false);
	}
	~CConsole() {
		fclose(graf);
		if (m_hConsoleStdOut != INVALID_HANDLE_VALUE)
			::CloseHandle(m_hConsoleStdOut);
	}
	void SetCursorInfo(DWORD dwSize = 25, BOOL bVisible = TRUE) {
		CONSOLE_CURSOR_INFO		info;
		info.dwSize = dwSize;
		info.bVisible = bVisible;
		::SetConsoleCursorInfo(m_hConsoleStdOut, &info);
	}
	void SetCursorPosition(short shXPos, short shYPos) {
		COORD	coord;
		coord.X = shXPos; coord.Y = shYPos;
		::SetConsoleCursorPosition(m_hConsoleStdOut, coord);
	}
	void SetColor(WORD wFore = L_WHITE, WORD wBack = L_BLACK) {
		::SetConsoleTextAttribute(m_hConsoleStdOut, wFore + (wBack << 4));
	}
	void SetWindowSize(int width = 150, int height = 70) {
		BOOL bRtn;
		HANDLE hWrite;
		SMALL_RECT rctWindowRect = { 0,0,width-1,height-1 };
		COORD dwCoord = { width,height };
		hWrite = GetStdHandle(STD_OUTPUT_HANDLE);
		bRtn = SetConsoleScreenBufferSize(hWrite, dwCoord);
		printf("%02d", bRtn);
		bRtn = SetConsoleWindowInfo(hWrite, TRUE, &rctWindowRect);
		printf("%02d", bRtn);
		//Sleep(2000);
	}
	void Clear(WORD wFore = L_WHITE, WORD wBack = L_BLACK){
		CONSOLE_SCREEN_BUFFER_INFO	csbi;
		DWORD	dwNumberOfCharsWritten;
		COORD	coord = { 0, 0 };
		if (::GetConsoleScreenBufferInfo(m_hConsoleStdOut, &csbi)){
			::FillConsoleOutputAttribute(m_hConsoleStdOut, wFore + (wBack << 4),
				csbi.dwSize.X * csbi.dwSize.Y, coord, &dwNumberOfCharsWritten);
			::FillConsoleOutputCharacter(m_hConsoleStdOut, ' ',
				csbi.dwSize.X * csbi.dwSize.Y, coord, &dwNumberOfCharsWritten);
		}
	}
}G;
static int CHARSIZE = 3;
int map[100] = { 0 }, black = 0, white = 0, x = 0, y = 0, put = 0, turn = 0, time=10, move[8] = { -11,-10,-9,-1,1,9,10,11 };
void king(), queen(), bishop(), knight(), rook(), pawn(), set(), check_castling(), ischecks(int);
static void Print(void),lvup_pawn(Color),motion(int);
int input(void), ischeck(int,int), state();
bool isstatemate(),ischeckmate();
int main() {
	int end = 0;
	void(*pf[])() = { king, queen, bishop, knight, rook, pawn };
	set();
	while (1) {
		check_castling();
		if ((end = state())) {
			if (end == 1);
			else;
			return 0;
		}
		while (end = input()) {
			if (end == 2) return 0;
			for (int i = 0; i < 100; i++)  if (map[i] < 1000) map[i] %= 100;
											else if (1100 <= map[i]) map[i] = map[i] % 100 + 1000;
			put = (10 * (y + 1)) + (x + 1);
			if (map[put] % 10 != 0 && ((!turn&&map[put]%100 < 50) || (turn && 50 < map[put]%100))) pf[(map[put] - 1) % 50]();
			ischecks(put);
			Print();
			while (	GetAsyncKeyState(VK_RIGHT) ||
					GetAsyncKeyState(VK_LEFT) ||
					GetAsyncKeyState(VK_DOWN) ||
					GetAsyncKeyState(VK_UP) ||
					GetAsyncKeyState(VK_RETURN));
			if (end == 3 && map[put] % 10 != 0 && ((!turn&&map[put] % 100 < 50) || (turn && 50 < map[put] % 100))) {
				for (int i = 0; i < 100; i++) if (100 <= map[i]) end = 4;
				if (end == 4) break;
			}
		}
		while (end = input()) {
			if (end == 2) return 0;
			Print();
			while (GetAsyncKeyState(VK_ESCAPE) ||
				GetAsyncKeyState(VK_RIGHT) ||
				GetAsyncKeyState(VK_LEFT) ||
				GetAsyncKeyState(VK_DOWN) ||
				GetAsyncKeyState(VK_UP) ||
				GetAsyncKeyState(VK_RETURN));
			if (end == 3 && 100 <= map[((10 * (y + 1)) + (x + 1))]%1000) {
				for (int j = 0; j < 100; j++) if( map[j] != 200 && map[j] != 300 ) map[j] %= 100;
				motion(((10 * (y + 1)) + (x + 1)) - put);
				if (map[((10 * (y + 1)) + (x + 1))] == 200){
					map[((10 * (y + 1)) + (x + 1))] = map[put], map[put] = 0;
					if (0 < ((10 * (y + 1)) + (x + 1)) - put) {
						put += 3; motion(-2);
						map[put - 2] = map[put], map[put] = 0;
					}else {
						put -= 4; motion(3);
						map[put + 3] = map[put], map[put] = 0;
					}
					stat[turn].doKing = stat[turn].dorook = true; stat[turn].canLcastling = stat[turn].canRcastling = false;
				}
				else if ( 300 <= map[((10 * (y + 1)) + (x + 1))] && map[((10 * (y + 1)) + (x + 1))] < 400) {
					map[((10 * (y + 1)) + (x + 1))] = map[put], map[put] = 0;
					map[((10 * (y + 1)) + (x + 1)) + 20 * turn - 10] = 0;
				}else map[((10 * (y + 1)) + (x + 1))] = map[put], map[put] = 0;
				for (int j = 0; j < 100; j++) map[j] %= 100;
				if (abs(put - ((10 * (y + 1)) + (x + 1))) == 20 && map[(10 * (y + 1)) + (x + 1)] % 10 == 6) map[(10 * (y + 1)) + (x + 1)] += 1000;
				put = (10 * (y + 1)) + (x + 1);
				if (map[put] % 10 == 1 && !stat[turn].doKing) stat[turn].doKing = true;
				if (map[put] % 10 == 5 && !stat[turn].dorook) stat[turn].dorook = true;
				turn = 1 - turn;
				Print();
				if (map[put] % 50 == 6 && ((10 < put&&put < 19) || (80 < put&&put < 89))) lvup_pawn((map[put] % 100 < 50 ? L_WHITE : L_BLACK));
				break;
			}
			else if (end == 3) break;
		}
	}
}
void king() {
	if (map[put] >= 50) black = 50, white = 0;
	else white = 50, black = 100;
	for (int i = 0; i < 8; i++) if (
		(0 < (put + move[i]) % 10 && (put + move[i]) % 10 < 9) &&
		(10 < (put + move[i]) && (put + move[i]) < 90) &&
		(white < map[put + move[i]] % 1000 && map[put + move[i]] % 1000<black ||
			map[put + move[i]] == 0)
		) map[put + move[i]] += 100;
	if (stat[turn].canLcastling) map[put - 2] = 200;
	if (stat[turn].canRcastling) map[put + 2] = 200;
}
void queen() {
	bishop();
	rook();
}
void bishop() {
	if (map[put] >= 50) black = 50, white = 0;
	else white = 50, black = 100;
	for (int i = 0; i < 8; i += 2) {
		if (i == 4) ++i;
		for (int j = 1;; j++) if (
			(0 < (put + j * move[i]) % 10 && (put + j * move[i]) % 10 < 9) &&
			(10 < (put + j * move[i]) && (put + j * move[i]) < 90) &&
			(white < map[put + j * move[i]] % 1000 && map[put + j * move[i]] % 1000 < black ||
				map[put + j * move[i]] == 0)
			) {
			if ((map[put + j * move[i]] += 100) != 100) break;
		}
		else break;
	}
}
void rook() {
	if (map[put] % 100 >= 50) black = 50, white = 0;
	else white = 50, black = 100;
	for (int i = 1; i < 8; i += 2) {
		if (i == 5) i--;
		for (int j = 1;; j++) if (
			(0 < (put + j * move[i]) % 10 && (put + j * move[i]) % 10 < 9) &&
			(10 < (put + j * move[i]) && (put + j * move[i]) < 90) &&
			(white < map[put + j * move[i]] % 1000 && map[put + j * move[i]] % 1000 < black ||
				map[put + j * move[i]] == 0)
			) {
			if ((map[put + j * move[i]] += 100) != 100) break;
		}
		else break;
	}
}
void knight() {
	int move[8] = { -21,-19,-12,-8,8,12,19,21 };
	if (map[put] >= 50) black = 50, white = 0;
	else white = 50, black = 100;
	for (int i = 0; i < 8; i++) if (
		(0 < (put + move[i]) % 10 && (put + move[i]) % 10 < 9) &&
		(10 < (put + move[i]) && (put + move[i]) < 90) &&
		(white < map[put + move[i]] % 1000 && map[put + move[i]] % 1000<black ||
			map[put + move[i]] == 0)
		) map[put + move[i]] += 100;
}
void pawn() {
	int col = 0;
	if (map[put] >= 50) black = 50, white = 0;
	else { white = 50, black = 100, col = 5; }
	if (10 + col * 14 < put && put < 19 + col * 14) { return; }
	if (map[put + move[1 + col]] == 0) {
		map[put + move[1 + col]] += 100;
		if (70 - col * 10 < put && put < 79 - col * 10 &&
			map[put + 2 * move[1 + col]] == 0) map[put + 2 * move[1 + col]] += 100;
	}
	for (int i = 0; i < 3; i += 2)
		if (white < map[put + move[i + col]] % 1000 && map[put + move[i + col]] % 1000 < black) map[put + move[i + col]] += 100;
		else if (white < map[put + i - 1]%100 && map[put + i - 1]%100 < black && 1000 <= map[put + i - 1]) map[put + move[i + col]] += 300;
}
int input(void) {
	while(1) {
		if (GetAsyncKeyState(VK_LSHIFT)) {
			G.SetColor();
			printf("遅延%dミリ秒",time);
			while (GetAsyncKeyState(VK_LSHIFT));
			G.Clear(); Print();
		}
		if (GetAsyncKeyState(87)) {
			if( CHARSIZE < CHARMAX ) ++CHARSIZE; G.Clear(); Print();
			while (GetAsyncKeyState(87));
		}
		if (GetAsyncKeyState(83)) {
			if( CHARSIZE ) --CHARSIZE; G.Clear(); Print();
			while (GetAsyncKeyState(83));
		}
		if (GetAsyncKeyState(68))
			{time+=10; while (GetAsyncKeyState(68));}
		if (GetAsyncKeyState(65))
			{if (time) time-=10; while (GetAsyncKeyState(65));}
		if (GetAsyncKeyState(VK_ESCAPE)) return 2;
		else if (GetAsyncKeyState(VK_RIGHT)) x = (x + 1) % 8;
		else if (GetAsyncKeyState(VK_LEFT)) x = (x + 7) % 8;
		else if (GetAsyncKeyState(VK_DOWN)) y = (y + 1) % 8;
		else if (GetAsyncKeyState(VK_UP)) y = (y + 7) % 8;
		else if (GetAsyncKeyState(VK_RETURN)) return 3;
		else continue;
		break;
	}
	return 1;
}
void check_castling(){
	stat[0].canLcastling = stat[1].canLcastling = stat[0].canRcastling = stat[1].canRcastling = false;
	if (map[11] % 100 ==  5 && map[12] % 100 == 0 && map[13] % 100 == 0 && map[14] % 100 == 0 && !stat[0].doKing && !stat[0].dorook && !ischeck(13, !turn) && !ischeck(14, !turn)) stat[0].canLcastling = true;
	if (map[18] % 100 ==  5 && map[16] % 100 == 0 && map[17] % 100 == 0						&& !stat[0].doKing && !stat[0].dorook && !ischeck(16, !turn) && !ischeck(17, !turn)) stat[0].canRcastling = true;
	if (map[11] % 100 == 55 && map[82] % 100 == 0 && map[83] % 100 == 0 && map[84] % 100 == 0 && !stat[1].doKing && !stat[1].dorook && !ischeck(83, !turn) && !ischeck(84, !turn)) stat[1].canLcastling = true;
	if (map[88] % 100 == 55 && map[86] % 100 == 0 && map[87] % 100 == 0						&& !stat[1].doKing && !stat[1].dorook && !ischeck(86, !turn) && !ischeck(87, !turn)) stat[1].canRcastling = true;
}
int ischeck(int place= (10 * (y + 1)) + (x + 1),int type=!turn){
	int verify=0, shift[8] = { -21,-19,-12,-8,8,12,19,21 };
	for (verify = place - 10; 10 < verify; verify -= 10) {//上
		if (map[verify] % 100 != 0) {
			if (place - verify == 10 && map[verify] % 100 == 50 * type + 1) return verify;
			if (map[verify] % 100 == type * 50 + 2 || map[verify] % 100 == type * 50 + 5) return verify;
			break;
		}
	}
	for (verify = place - 9; 10 < verify && (place % 10) < (verify % 10); verify -= 9) {//右上
		if (map[verify] % 100 != 0) {
			if (place - verify == 9 && map[verify] % 100 == 50 * type + 1) return verify;
			if (!type && place - verify == 9 && map[verify] % 100 == 6) return verify;
			if (map[verify] % 100 == type * 50 + 2 || map[verify] % 100 == type * 50 + 3) return verify;
			break;
		}
	}
	for (verify = place + 1; verify % 10 < 9; verify++) if (map[verify] % 100 != 0) {//右
			if (verify - place == 1 && map[verify] % 100 == 50 * type + 1) return verify;
			if (map[verify] % 100 == type * 50 + 2 || map[verify] % 100 == type * 50 + 5) return verify;
			break;
		}
	for (verify = place + 11; verify < 89 && (place % 10) < (verify % 10); verify += 11) {//右下
		if (map[verify] % 100 != 0) {
			if (type && verify - place == 11 && map[verify] % 100 == 56) return verify;
			if (verify - place == 11 && map[verify] % 100 == 50 * type + 1) return verify;
			if (map[verify] % 100 == type * 50 + 2 || map[verify] % 100 == type * 50 + 3) return verify;
			break;
		}
	}
	for (verify = place + 10; verify < 89; verify += 10) {//下
		if (map[verify] % 100 != 0) {
			if (verify - place == 10 && map[verify] % 100 == 50 * type + 1) return verify;
			if (map[verify] % 100 == type * 50 + 2 || map[verify] % 100 == type * 50 + 5) return verify;
			break;
		}
	}
	for (verify = place + 9; verify < 89 && (verify % 10) < (place % 10); verify += 9) {//左下
		if (map[verify] % 100 != 0) {
			if (type && verify - place == 9 && map[verify] % 100 == 56) return verify;
			if (verify - place == 9 && map[verify] % 100 == 50 * type + 1) return verify;
			if (map[verify] % 100 == type * 50 + 2 || map[verify] % 100 == type * 50 + 3) return verify;
			break;
		}
	}
	for (verify = place - 1; 0 < verify % 10; verify--)if (map[verify] % 100 != 0) {//左
		if (place - verify == 1 && map[verify] % 100 == 50 * type + 1) return verify;
		if (map[verify] % 100 == type * 50 + 2 || map[verify] % 100 == type * 50 + 5) return verify;
		break;
	}
	for (verify = place - 11; 10 < verify && (verify % 10) < (place % 10); verify -= 11) {//左上
		if (map[verify] % 100 != 0) {
			if (!type && place - verify == 11 && map[verify] % 100 == 6) return verify;
			if (place - verify == 11 && map[verify] % 100 == 50 * type + 1) return verify;
			if (map[verify] % 100 == type * 50 + 2 || map[verify] % 100 == type * 50 + 3) return verify;
			break;
		}
	}
	for (int i = 0; i < 8; i++)  if(11 <place + shift[i] && place + shift[i] < 89) if (map[place + shift[i]] % 100 == type * 50 + 4) return place + shift[i];
	return 0;
}
void ischecks(int put=::put) {
	int(*pos)=NULL, (*sub)=NULL, count = 0, king = 0, ID[2][2] = { {put,map[put]},{0,0} };
	for (int i = 11; i < 89; i++) if (map[i] == 50 * turn + 1) {king = i; break;}
	for (int i = 11; i < 89; i++) if (100 <= map[i]) {
		++count;
		pos = (int *)calloc(count, sizeof(int));
		for (int j = 0; j < count - 1; j++) pos[j] = sub[j];
		pos[count-1] = i;
		sub = (int *)calloc(count, sizeof(int));
		for (int j = 0; j < count; j++) sub[j] = pos[j];
	};
	for (int i = 0; i < count; i++) {
		ID[1][0] = pos[i];
		ID[1][1] = map[pos[i]];
		map[ID[0][0]] = 0;
		map[ID[1][0]] = ID[0][1];
		if (ID[0][1] == 50 * turn + 1) king = ID[1][0];
		if (!!ischeck(king,!turn)) {
			map[ID[0][0]] = ID[0][1]; map[ID[1][0]] = ID[1][1];
			map[ID[1][0]] %= 100;
		}else { map[ID[0][0]] = ID[0][1]; map[ID[1][0]] = ID[1][1]; }
	}
	free(pos); free(sub);
}
bool isstatemate() {
	int count = 0, check = 0, king = 0;
	for (int i = 11; i < 89; i++)
		if (50 * turn < map[i] % 100 && map[i] % 100 < 50 * turn + 50)
			{count++; if (map[i] % 100 == 50 * turn + 1) king = i;}
	for(int i = 0; i < 8; i++)
		if (10 < (king + move[i]) && (king + move[i]) < 89 && 0 < (king + move[i]) % 10 && (king + move[i]) % 10 < 9) check += !!ischeck(king + move[i]);
		else check++;
	return ((count == 1 && check == 8 && !ischeck(king))?true:false);
}
bool ischeckmate() {
	int(*pos) = NULL, (*sub) = NULL, count = 0, check = 0, _king = 0, data = 0;
	void(*pf[])() = { king, queen, bishop, knight, rook, pawn };
	for (int i = 11; i < 89; i++)
		if (50 * turn < map[i] % 100 && map[i] % 100 < 50 * turn + 50) {
			count++;
			if (map[i] % 100 == 50 * turn + 1) {_king = i; --count;}
			else {
				pos = (int *)calloc(count, sizeof(int));
				for (int j = 0; j < count - 1; j++) pos[j] = sub[j];
				pos[count - 1] = i;
				sub = (int *)calloc(count, sizeof(int));
				for (int j = 0; j < count; j++) sub[j] = pos[j];
			}
		};
	data = map[_king]; map[_king] = 0;
	for (int i = 0; i < 8; i++)
		if (10 < (_king + move[i]) && (_king + move[i]) < 89 && 0 < (_king + move[i]) % 10 && (_king + move[i]) % 10 < 9 && ( map[_king+move[i]]%100 == 0 || !(50 * turn < map[_king + move[i]]%100 && map[_king + move[i]] % 100 < 50 * turn + 50))) check += !!ischeck(_king + move[i]);
		else check++;
	map[_king] = data; check += !!ischeck(_king);
	if (check == 9) for (int i = 0; i < count; i++) {
		pf[map[pos[i]]%10-1](); ischeck(pos[i]);
		for (int j = 11; j < 89; j++) if (100 <= map[j] % 1000)
			{free(pos); free(sub);return true;}
	}
	free(pos); free(sub);
	return false;
}
static void Print(void) {
	short baseX = 0, baseY = 0;
	char str[12] = { 0 };
	bool color = true;
	G.SetCursorPosition(baseX, baseY);
	for (int i = -1; i < 8; i++) {
		if (i != -1) G.SetColor(L_WHITE, L_GREEN);
		fseek(graf, ((i != y) ? SPACE : SIDE), SEEK_SET);
		for (int j = 0; j < CHARSIZE; j++) {
			G.SetCursorPosition(baseX, baseY + j + CHARSIZE * (i+1));
			fgets(str, (2 * CHARSIZE + 2), graf);
			printf("%*s", 2 * CHARSIZE, str);
		}
	}
	for (int i = 0; i < 8; i++) {
		baseX += 2 * CHARSIZE; fseek(graf, ((i != x) ? SPACE : TOP), SEEK_SET);
		for (int j = 0; j < CHARSIZE; j++) {
			G.SetCursorPosition(baseX, baseY + j);
			fgets(str, (2 * CHARSIZE + 2), graf); printf("%*s", 2 * CHARSIZE, str);
		}
	}
	baseX = 2 * CHARSIZE; baseY = CHARSIZE;
	for (int i = 11; i < 90; i++) {
		if ((i % 10) == 0) continue;
		color = !color;
		if ((i % 10) == 9) {
			baseY += (baseX = 2 * CHARSIZE, baseX / 2);
			continue;
		}
		fseek(graf, (map[i] % 1000<500 ? SPACE + (map[i] % 50)*NEXT : SPACE), SEEK_SET);
		G.SetColor((map[i] % 100 < 50 ? L_WHITE : L_BLACK), ((100 <= map[i] % 1000 && map[i] % 1000 < 500) ? (color ? H_RED : H_PURPLE) : (color ? L_RED : L_PURPLE)));
		for (int j = 0; j < CHARSIZE; j++) {
			G.SetCursorPosition(baseX, baseY + j);
			fgets(str, (2 * CHARSIZE + 2), graf);
			printf("%*s", 2 * CHARSIZE, str);
		}
		baseX += 2 * CHARSIZE;
	}
	G.SetCursorPosition(0, 9 * CHARSIZE);
	G.SetColor();printf("\r手番:%s", (turn ? "黒" : "白"));
};
static void lvup_pawn(Color color) {
	short baseX, baseY;
	char str[102] = { 0 };
	int celect = 0;
	while (1) {
		baseX = 2 * CHARSIZE + (8 * CHARSIZE) - (7 * CHARSIZE + 1), baseY = 3 * CHARSIZE;
		G.SetColor(L_YELLOW, H_CYAN);
		fseek(graf, SELECT, SEEK_SET);
		for (int j = 0; j < 2 * CHARSIZE; j++) {
			G.SetCursorPosition(baseX, baseY + j);
			fgets(str, 7 * 2 * CHARSIZE + 4, graf);
			printf("%*s", 7 * 2 * CHARSIZE +2, str);
		}
		baseX = 3 * 2 * CHARSIZE, baseY += 2 * CHARSIZE;
		for (int i = 0; i < 4; i++) {
			fseek(graf, SPACE + (i + 2)*NEXT, SEEK_SET);
			G.SetColor(color, (i == celect ? H_CYAN : L_CYAN));
			for (int j = 0; j < CHARSIZE; j++) {
				G.SetCursorPosition(baseX, baseY + j);
				fgets(str, (2 * CHARSIZE+2), graf);
				printf("%*s", 2 * CHARSIZE, str);
			}
			baseX += 2 * CHARSIZE;
		}
		while (1) {
			if (GetAsyncKeyState(VK_RIGHT)) celect = (celect + 1) % 4;
			else if (GetAsyncKeyState(VK_LEFT)) celect = (celect + 3) % 4;
			else if (GetAsyncKeyState(VK_RETURN)) {
				map[put] -= 4 - celect;
				return;
			}
			else continue;
			break;
		}
		while (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState(VK_LEFT));
	}
	Print();
}
static void motion(int val) {
	int baseX = 2 * (put % 10) * CHARSIZE, baseY = (int)(put / 10) * CHARSIZE; map[put] += 500;
	char str[12] = { 0 }, ch[4] = { 0 };
	bool turn = true;
	if (val < 0) val *= -1, turn = !turn;
	if (val % 9 == 0) { // 左下
		for (int i = 0; i < (val / 9) * CHARSIZE; i++) {
			Print();
			fseek(graf, SPACE + (map[put] % 50)*NEXT, SEEK_SET);
			for (int j = 0; j < CHARSIZE; j++) {
				G.SetCursorPosition(baseX - (2 * (turn ? i : -1 * i)), baseY + j + (turn ? i : -1 * i));
				fgets(str, (2 * CHARSIZE + 2), graf);
				for (int l = 0; l < (2 * CHARSIZE); l += 2) {
					G.SetColor((map[put] % 100 < 50 ? L_WHITE : L_BLACK), ((baseX - (2 * (turn ? i : -1 * i)) + l) / (2 * CHARSIZE) + (baseY + j + (turn ? i : -1 * i)) / CHARSIZE) % 2 ? L_RED : L_PURPLE);
					ch[0] = str[l], ch[1] = str[l + 1]; printf("%2s", ch);
				}
			}
			Sleep(time);
		}
	}
	else if (val % 10 == 0) {//下
		for (int i = 0; i < (int)(val / 10) * CHARSIZE; i++) {
			Print();
			fseek(graf, SPACE + (map[put] % 50)*NEXT, SEEK_SET);
			for (int j = 0; j < CHARSIZE; j++) {
				G.SetCursorPosition(baseX, baseY + j + (turn ? i : -1 * i));
				fgets(str, (2 * CHARSIZE + 2), graf);
				for (int l = 0; l < (2 * CHARSIZE); l += 2) {
					G.SetColor((map[put] % 100 < 50 ? L_WHITE : L_BLACK), ((baseX / (2 * CHARSIZE) + (baseY + j + (turn ? i : -1 * i)) / CHARSIZE) % 2 ? L_RED : L_PURPLE));
					ch[0] = str[l], ch[1] = str[l + 1]; printf("%2s", ch);
				}
			}
			Sleep(time);
		}
	}
	else if (val % 11 == 0) {//右下
		for (int i = 0; i < (val / 9) * CHARSIZE; i++) {
			Print();
			fseek(graf, SPACE + (map[put] % 50)*NEXT, SEEK_SET);
			for (int j = 0; j < CHARSIZE; j++) {
				G.SetCursorPosition(baseX + (2 * (turn ? i : -1 * i)), baseY + j + (turn ? i : -1 * i));
				fgets(str, (2 * CHARSIZE+2), graf);
				for (int l = 0; l < (2 * CHARSIZE); l += 2) {
					G.SetColor((map[put] % 100 < 50 ? L_WHITE : L_BLACK), ((baseX + (2 * (turn ? i : -1 * i)) + l) / (2 * CHARSIZE) + (baseY + j + (turn ? i : -1 * i)) / CHARSIZE) % 2 ? L_RED : L_PURPLE);
					ch[0] = str[l], ch[1] = str[l + 1]; printf("%2s", ch);
				}
			}
			Sleep(time);
		}
	}
	else if (val == 8) {//斜め左下
		for (int i = 0; i < 2 * CHARSIZE; i++) {
			Print();
			fseek(graf, SPACE + (map[put] % 50)*NEXT, SEEK_SET);
			for (int j = 0; j < CHARSIZE; j++) {
				G.SetCursorPosition(baseX - (2 * (turn ? i : -1 * i)), baseY + j + (turn ? i : -1 * i) / 2);
				fgets(str, (2 * CHARSIZE+2), graf);
				for (int l = 0; l < (2 * CHARSIZE); l += 2) {
					G.SetColor((map[put] % 100 < 50 ? L_WHITE : L_BLACK), ((baseX - (2 * (turn ? i : -1 * i)) + l) / (2 * CHARSIZE) + (baseY + j + (turn ? i : -1 * i) / 2) / CHARSIZE) % 2 ? L_RED : L_PURPLE);
					ch[0] = str[l], ch[1] = str[l + 1]; printf("%2s", ch);
				}
			}
			Sleep(time);
		}
	}
	else if (val == 12) {//斜め右下
		for (int i = 0; i < 2 * CHARSIZE; i++) {
			Print();
			fseek(graf, SPACE + (map[put] % 50)*NEXT, SEEK_SET);
			for (int j = 0; j < CHARSIZE; j++) {
				G.SetCursorPosition(baseX + (2 * (turn ? i : -1 * i)), baseY + j + (turn ? i : -1 * i) / 2);
				fgets(str, (2 * CHARSIZE+2), graf);
				for (int l = 0; l < (2 * CHARSIZE); l += 2) {
					G.SetColor((map[put] % 100 < 50 ? L_WHITE : L_BLACK), ((baseX + (2 * (turn ? i : -1 * i)) + l) / (2 * CHARSIZE) + (baseY + j + (turn ? i : -1 * i) / 2) / CHARSIZE) % 2 ? L_RED : L_PURPLE);
					ch[0] = str[l], ch[1] = str[l + 1]; printf("%2s", ch);
				}
			}
			Sleep(time);
		}
	}
	else if (val == 19) {//斜め下左
		for (int i = 0; i < 2 * CHARSIZE; i += 2) {
			Print();
			fseek(graf, SPACE + (map[put] % 50)*NEXT, SEEK_SET);
			for (int j = 0; j < CHARSIZE; j++) {
				G.SetCursorPosition(baseX - ((turn ? i : -1 * i)), baseY + j + (turn ? i : -1 * i));
				fgets(str, (2 * CHARSIZE+2), graf);
				for (int l = 0; l < (2 * CHARSIZE); l += 2) {
					G.SetColor((map[put] % 100 < 50 ? L_WHITE : L_BLACK), ((baseX - ((turn ? i : -1 * i)) + l) / (2 * CHARSIZE) + (baseY + j + (turn ? i : -1 * i)) / CHARSIZE) % 2 ? L_RED : L_PURPLE);
					ch[0] = str[l], ch[1] = str[l + 1]; printf("%2s", ch);
				}
			}
			Sleep(time);
		}
	}
	else if (val == 21) {//斜め下右
		for (int i = 0; i < 2 * CHARSIZE; i += 2) {
			Print();
			fseek(graf, SPACE + (map[put] % 50)*NEXT, SEEK_SET);
			for (int j = 0; j < CHARSIZE; j++) {
				G.SetCursorPosition(baseX + ((turn ? i : -1 * i)), baseY + j + (turn ? i : -1 * i));
				fgets(str, (2 * CHARSIZE+2), graf);
				for (int l = 0; l < (2 * CHARSIZE); l += 2) {
					G.SetColor((map[put] % 100 < 50 ? L_WHITE : L_BLACK), ((baseX + ((turn ? i : -1 * i)) + l) / (2 * CHARSIZE) + (baseY + j + (turn ? i : -1 * i)) / CHARSIZE) % 2 ? L_RED : L_PURPLE);
					ch[0] = str[l], ch[1] = str[l + 1]; printf("%2s", ch);
				}
			}
			Sleep(time);
		}
	}
	else {//みぎ
		for (int i = 0; i < val * 2 * CHARSIZE; i += 2) {
			Print();
			fseek(graf, SPACE + (map[put] % 50)*NEXT, SEEK_SET);
			for (int j = 0; j < CHARSIZE; j++) {
				G.SetCursorPosition(baseX + (turn ? i : -1 * i), baseY + j);
				fgets(str, (2 * CHARSIZE+2), graf);
				for (int l = 0; l < (2 * CHARSIZE); l += 2) {
					G.SetColor((map[put] % 100 < 50 ? L_WHITE : L_BLACK), ((baseX + (turn ? i : -1 * i) + l) / (2 * CHARSIZE) + (baseY + j) / CHARSIZE) % 2 ? L_RED : L_PURPLE);
					ch[0] = str[l], ch[1] = str[l + 1]; printf("%2s", ch);
				}
			}
			Sleep(time);
		}
	}
	for (int i = 0; i < 100; i++) map[i] %= 500;
}
int state() {
	if (ischeckmate()) return 1;
	if (isstatemate()) return 2;
	return 0;
}
void set() {
	fopen_s(&graf, FILENAME, "r");
	for (int i = 28; 10 < i; i--) {
		if (20 < i) map[i] = 6;
		else if (i == 11 || i == 18) map[i] = 5;
		else if (i == 12 || i == 17) map[i] = 4;
		else if (i == 13 || i == 16) map[i] = 3;
		else if (i == 14) map[i] = 2;
		else if (i == 15) map[i] = 1;
	}
	for (int i = 71; i< 89; i++) {
		if (i < 79) map[i] = 56;
		else if (i % 70 == 11 || i % 70 == 18) map[i] = 55;
		else if (i % 70 == 12 || i % 70 == 17) map[i] = 54;
		else if (i % 70 == 13 || i % 70 == 16) map[i] = 53;
		else if (i % 70 == 14) map[i] = 52;
		else if (i % 70 == 15) map[i] = 51;
	}
	stat[0].canLcastling = stat[1].canLcastling = stat[0].canRcastling = stat[1].canRcastling = false;
	stat[0].doKing = stat[1].doKing = stat[0].dopawn = stat[1].dopawn = stat[0].dorook = stat[1].dorook = true;
	stat[0].ischeck = stat[1].ischeck = stat[0].isstale = stat[1].isstale = stat[0].nocheck = stat[1].nocheck = false;
	Print();
}
