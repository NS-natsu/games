#ifndef _MAP_CPP_
#define _MAP_CPP_ 
#include <math.h>
#include <string.h>
#include <iostream>

#include "player.cpp"

class Map {
private:
	int nMapWidth;
	int nMapHeight;

	std::wstring map;
	inline void ReplaceMap(int*,int,int,int,int);
protected:
	void generate(void);
public:
	const char Air;
	const char Wall;
	const char Goal;
	const char Start;
	Map();
	Map(int,int);
	Map(char,char,char,char);
	bool isArea(int,int);
	char getPos(int,int);
	char getPos(Player&);
	bool move(float,float,Player&);
	int getWidth(void);
	int getHeight(void);
	void output(void);
};


#if __INCLUDE_LEVEL__ == 0
#define MIN(x,y) (x<y?x:y)
#define MAX(x,y) (x<y?y:x)

inline void Map::ReplaceMap(int *m, int w, int h, int t, int v) {
	if (t != v)
		for (int y = 0; y < h; y++)
			for (int x = 0; x < w; x++, m++)
				if (*m == t) *m = v;
}

void Map::generate(void) {
	bool isMakeComp = false;
	int *_map = new int[nMapHeight * nMapWidth];
	memset(_map, -1, sizeof(int) * nMapHeight * nMapWidth);

	for (int y = 1, temp = 0; y < nMapHeight; y += 2)
		for (int x = 1; x < nMapWidth; x += 2)
			*(_map + y * nMapWidth + x) = temp++;

	//(x y) (1 1) -> (1 3)
	_map[2 * nMapWidth + 1] = 0;
	_map[3 * nMapWidth + 1] = _map[nMapWidth + 1];
	//(x y) (nMapWidth - 1 nMapHeight - 1) -> (nMapWidth - 1 nMapHeight - 1)
	_map[(nMapHeight - 1) * nMapWidth - 3] = 0;
	_map[(nMapHeight - 1) * nMapWidth - 2] = _map[(nMapHeight - 1) * nMapWidth - 4];

	int *itr;
	while(!isMakeComp){
		int x = 2 * (rand() % ((nMapWidth - 1) / 2)) + 1,
			y = 2 * (rand() % ((nMapHeight - 1) / 2)) + 1;

		itr = _map + y * nMapWidth + x;

		switch (rand() & 3) {
		case 0://Up
			if (2 < y && *itr != *(itr - 2 * nMapWidth) && (x != nMapWidth - 2 || y != nMapHeight - 2)) {
				*(itr - nMapWidth) = 0;
				ReplaceMap(_map, nMapWidth, nMapHeight,
					MAX(*(itr - 2 * nMapWidth), *itr), MIN(*(itr - 2 * nMapWidth), *itr));
				isMakeComp = true;
			}
			break;
		case 1://Down
			if (y < nMapHeight - 3 && *itr != *(itr + 2 * nMapWidth) && (x != nMapWidth - 2 || y != nMapHeight - 4)) {
				*(itr + nMapWidth) = 0;
				ReplaceMap(_map, nMapWidth, nMapHeight,
					MAX(*(itr + 2 * nMapWidth), *itr), MIN(*(itr + 2 * nMapWidth), *itr));
				isMakeComp = true;
			}
			break;
		case 2://Right
			if (2 < x && *itr != *(itr - 2) && (x != 3 || y != 1)) {
				*(itr - 1) = 0;
				ReplaceMap(_map, nMapWidth, nMapHeight,
					MAX(*(itr - 2), *itr), MIN(*(itr - 2), *itr));
				isMakeComp = true;
			}
			break;
		default://Left
			if (x < nMapWidth - 3 && *itr != *(itr + 2) && (x != 1 || y != 1)) {
				*(itr + 1) = 0;
				ReplaceMap(_map, nMapWidth, nMapHeight,
					MAX(*(itr + 2), *itr), MIN(*(itr + 2), *itr));
				isMakeComp = true;
			}
			break;
		}
		if(isMakeComp)
			for (y = 0; y < nMapHeight; y++)
				for (x = 0; x < nMapWidth; x++)
					if (0 < *(_map + (y*nMapWidth + x))) {
						isMakeComp = false;
						y = nMapHeight;
						break;
					}
	}
	ReplaceMap(_map, nMapWidth, nMapHeight, -1, 1);

	itr = _map;
	for (int y = 0; y < nMapHeight; y++) {
		for (int x = 0; x < nMapWidth; x++, itr++)
			if (x == 1 && y == 1){
				map += L"S";
			}else if (x == nMapWidth - 2 && y == nMapHeight - 2){
				map += L"G";
			} else map += !(*itr) ? L" " : L"#";
	}
	delete _map;
	return;
}

Map::Map() : Air(' '), Wall('#'), Start('S'), Goal('G'){
	nMapWidth = nMapHeight = 33;
	generate();
}
Map::Map(int w, int h) : Air(' '), Wall('#'), Start('S'), Goal('G'){
	//even->odd(+1)
	nMapWidth = w | 1;
	nMapHeight = h | 1;
	if(nMapWidth < 5) nMapWidth = 33;
	if(nMapHeight < 5) nMapHeight = 33;
	generate();
}

Map::Map(char a, char w, char s, char g) : Air(a), Wall(w), Start(s), Goal(g){
	nMapWidth = nMapHeight = 33;
	generate();
}

bool Map::isArea(int x, int y)
	{ return (0 <= x) & (x < nMapWidth) & (0 <= y) & (y < nMapHeight); }

char Map::getPos(int x, int y){
	if(!isArea(x, y)) return 0;
	return map.c_str()[y * nMapWidth + x];
}
char Map::getPos(Player& p){
	return getPos((int)(p.getX()), (int)(p.getY()));
}
bool Map::move(float dx, float dy, Player& p){
	//player:cylinder(r=0.4m)
	const float d[2]={.2f,-.2f};
	float x = p.getX(), y = p.getY();

	float fpx = x + dx, fpy = y + dy;

	if(floor(fpx - d[0]) < 0) dx = d[0] - x;
	else if(nMapWidth <= int(fpx - d[1])) dx = (float)nMapWidth + d[1] - x;

	if(floor(fpy - d[0]) < 0) dy = d[0] - y;
	else if(nMapHeight <= int(fpy - d[1])) dy = (float)nMapHeight + d[1] - y;

	if(!p.isOver(0.99f)){
		if(map.c_str()[int(y)*nMapWidth+int(x + d[dx<0.f])]==Wall) dx = 0;
		if(map.c_str()[int(y + d[dy<0.f])*nMapWidth+int(x)]==Wall) dy = 0;
		if(map.c_str()[int(y + dy)*nMapWidth+int(x + dx)]==Wall){
			dx = dy = 0;
		}
	}

	int ret = !dx | !dy;
	p.move(dx, dy);
	return ret;
}

int Map::getWidth(void) {return nMapWidth;}
int Map::getHeight(void) {return nMapHeight;}

void Map::output(void){
	for(int i = 0; i < nMapHeight; i++)
		std::wcout << map.substr(nMapWidth * i, nMapWidth) << L"\n";
}
#endif
#endif