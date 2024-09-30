#include <iostream>
#include <string>
#include <algorithm>
#include <chrono>
#include <Windows.h>
#include <conio.h>
#include <math.h>
#include <omp.h>

#define MIN(x,y) x<y?x:y
#define MAX(x,y) x<y?y:x

#define Gravity (9.8f)
#define PI (3.14159f)

using namespace std;

class Graphs {
private:
	HWND hConsole;
	LPDWORD screen;
	BITMAPINFO biDIB;

	int nScreenWidth;
	int nScreenHeight;

protected:
	void GetScreenSize() {
		RECT lprc;
		GetClientRect(hConsole, &lprc);
		nScreenWidth = lprc.right - lprc.left;
		nScreenHeight = lprc.bottom - lprc.top;
	}
	int ScreenWidth()
		{return nScreenWidth;}
	int ScreenHeight()
		{return nScreenHeight;}
	bool UpdataScreenBuff() {
		if(screen==nullptr
		|| nScreenWidth !=biDIB.bmiHeader.biWidth
		|| nScreenHeight!=biDIB.bmiHeader.biHeight){
			biDIB.bmiHeader.biWidth = nScreenWidth;
			biDIB.bmiHeader.biHeight = nScreenHeight;
			if(screen!=nullptr) HeapFree(GetProcessHeap(), 0, screen);
			screen = (LPDWORD)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nScreenWidth * nScreenHeight * sizeof(DWORD));
			return true;
		}
		return false;
	}
	void Draw() {
		StretchDIBits(GetDC(hConsole), 0, nScreenHeight, nScreenWidth, -nScreenHeight,
			0, 0, biDIB.bmiHeader.biWidth, biDIB.bmiHeader.biHeight,
			screen, &biDIB, DIB_RGB_COLORS, SRCCOPY);
	}
	void draw(int x, int y, DWORD color) {
		screen[y * ScreenWidth() + x] = color;
	}
	void init(){
		hConsole = GetConsoleWindow();
		GetScreenSize();

		screen = nullptr;

		ZeroMemory(&biDIB, sizeof(biDIB));
		biDIB.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		biDIB.bmiHeader.biWidth = nScreenWidth;
		biDIB.bmiHeader.biHeight = nScreenHeight;
		biDIB.bmiHeader.biPlanes = 1;
		biDIB.bmiHeader.biBitCount = 32;
		biDIB.bmiHeader.biCompression = BI_RGB;

		UpdataScreenBuff();
	}
public:
	Graphs() {
		init();
	}
	~Graphs() {
		HeapFree(GetProcessHeap(), 0, screen);
	}
};

class G : public Graphs {
public:

private:
	int nMapWidth;
	int nMapHeight;

	float fPlayerX;
	float fPlayerY;
	float fPlayerZ;
	float fPlayerAx;
	float fPlayerAy;
	float fSpeed;

	float fFOV;
	float fFOVres;
	float fDepth;

	float *fDepthBuffer;

	float fJump;

	wstring map;

	float fElapsedTime;

	bool canJump;

	struct Sprite {
		LPDWORD grafics;
		int nGraphWidth;
		int nGraphHeight;
	};

	struct Sprite spriteWall;

	typedef union {
		DWORD color;
		struct {
			DWORD Blue : 8;
			DWORD Green : 8;
			DWORD Red : 8;
			DWORD free : 8;
		} RGB;
	} Color;

	inline unsigned long synth(Color c, float ratio){
		c.RGB.Red *= ratio;
		c.RGB.Blue *= ratio;
		c.RGB.Green *= ratio;
		return c.color;
	}

	float isr(float x){
		long X = *(long*)&x;
		long Y = 0x5F3759DF - (X >> 1);
		float y = *(float*)&Y;
		x *= .5f;
		y = y * (1.5f - (x * y * y));
		//y = y * (1.5f - (x * y * y));
		return y;
	}

	float* normalize(float v[3]){
		float n = isr(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
		v[0] *= n; v[1] *= n; v[2] *= n;
		return v;
	}

	inline float getFOVw(){
		float f = fFOV * (float)ScreenWidth() * fFOVres;
			return ( f < fFOV * 1.f ) ? f : fFOV * 1.f;
	}
	inline float getFOVh(){
		float f = fFOV * (float)ScreenHeight() * fFOVres;
			return ( f < fFOV * 2.f ) ? f : fFOV * 2.f;
	}
protected:
	int move(float fdx, float fdy){
		//player:cylinder(r=0.4m)
		const float d[2]={.2f,-.2f};

		float fpx = fPlayerX + fdx, fpy = fPlayerY + fdy;


		if(floor(fpx - d[0]) < 0) fdx = d[0] - fPlayerX;
		else if(nMapWidth <= int(fpx - d[1])) fdx = (float)nMapWidth + d[1] - fPlayerX;

		if(floor(fpy - d[0]) < 0) fdy = d[0] - fPlayerY;
		else if(nMapHeight <= int(fpy - d[1])) fdy = (float)nMapHeight + d[1] - fPlayerY;

		fpx = fPlayerX + fdx;
		fpy = fPlayerY + fdy;
		if(fPlayerZ < 1.8f){
			fpx = fPlayerX + d[fdx<0.f], fpy = fPlayerY + d[fdy<0.f];
			if(map.c_str()[int(fPlayerY)*nMapWidth+int(fpx)]=='#') fdx = 0;
			if(map.c_str()[int(fpy)*nMapWidth+int(fPlayerX)]=='#') fdy = 0;
			fpx = fPlayerX + fdx;
			fpy = fPlayerY + fdy;
			if(map.c_str()[int(fpy)*nMapWidth+int(fpx)]=='#'){
				fpx=fPlayerX; fpy = fPlayerY;
			}
		}

		int ret = (fPlayerX!=fpx)+(fPlayerY!=fpy);
		fPlayerX=fpx;fPlayerY=fpy;
		return ret;
	}
	void WallGraph() {
		int index = 0;
		DWORD color;
		for (int y = 0; y < 100; y++){
			for (int x = 0; x < 100; x++) {
				color = 0x00c16037;

				if (!y || !(y-99)) color = 0x00cccccc;
				else if(abs(int(float((y+2)%26)-12.5))==12) color = 0x00cccccc;
				else if(1&((y+2)/26)){
					if (!x || !(x-99)) color = 0x00cccccc;
					if (!abs(int((float)x-49.5))) color = 0x00cccccc;
				}else switch(abs(int((float)x-49.5))){
					case 24: color = 0x00cccccc; break;
					case 25: color = 0x00cccccc; break;
					default: break;
				}

				spriteWall.grafics[index++] = color;
			}
		}
	}
	void View() {
		const float fStepSize = .01f;
		
		const float frxs = fPlayerAx - getFOVw() / 2.f;
		const float frys = fPlayerAy + getFOVh() / 2.f;
		float frxd = getFOVw() / (float)ScreenWidth();
		float fryd = getFOVh() / (float)ScreenHeight();

		float v[3] = {cosf(fPlayerAx), sinf(fPlayerAx), sinf(fPlayerAy)};
		normalize(v);
		v[0]*= 1.2f;v[1]*= 1.2f;v[2]*= 1.2f;

		#pragma omp parallel for
		for (int x = 0; x < ScreenWidth(); x++){
			const float fEyeX = cosf(frxs + x * frxd);
			const float fEyeY = sinf(frxs + x * frxd);
			const float fiex = (fEyeX!=.0f) ? (1.f/fEyeX) : .0f;
			const float fiey = (fEyeY!=.0f) ? (1.f/fEyeY) : .0f;
			for (int y = 0; y < ScreenHeight(); y++){
				const float fEyeZ = sinf(frys - y * fryd);
				const float fiez = (fEyeZ!=.0f) ? (1.f/fEyeZ) : .0f;


				float fTestX = fPlayerX - v[0] + fEyeX / (fEyeX * v[0] + fEyeY * v[1] + fEyeZ * v[2]); //fPlayerX > .0f
				float fTestY = fPlayerY - v[1] + fEyeY / (fEyeX * v[0] + fEyeY * v[1] + fEyeZ * v[2]); //fPlayerY > .0f
				float fTestZ = fPlayerZ - v[2] + fEyeZ / (fEyeX * v[0] + fEyeY * v[1] + fEyeZ * v[2]); //fPlayerZ > .8f

				int nTestX = floor(fTestX);
				int nTestY = floor(fTestY);
				
				char block = ' ';
				if(.0f <= fTestZ && fTestZ <= 1.f
				&& 0 <= nTestX && nTestX < nMapWidth
				&& 0 <= nTestY && nTestY < nMapHeight)
					block = map.c_str()[nTestY * nMapWidth + nTestX];

				/*1: wall, 2: floor, 4: ceiling*/
				char bHit = 0;
				if (1.f < fTestZ && .0f <= fEyeZ)
					bHit = 4;
				else if (fTestZ < .0f && fEyeZ <= .0f)
					bHit = 2;

				float fDistanceToWall = .0f;
				float dX, dY, dZ, min;

				//Œø—¦‰»‚µ‚½‚¢
				while(!bHit&&fDistanceToWall<fDepth){
					min = .0f;
					if(fEyeX!=.0f){
						dX = (floorf(fTestX)+float(fEyeX>.0f)-fTestX) * fiex;
						if(!dX) dX = -fiex;
						if(fEyeX<.0f) dX -= fStepSize * fiex;
						min = dX;
					}
					if(fEyeY!=.0f){
						dY = (floorf(fTestY)+float(fEyeY>.0f)-fTestY) * fiey;
						if(!dY) dY = -fiey;
						if(fEyeY<.0f) dY -= fStepSize * fiey;
						if(dY < min || min == .0f) min = dY;
					}
					if(fEyeZ!=.0f){
						dZ = (floorf(fTestZ)+float(fEyeZ>.0f)-fTestZ) * fiez;
						if(!dZ) dZ = -fiez;
						if(fEyeZ<.0f) dZ -= fStepSize * fiez;
						if(dZ < min || min == .0f) min = dZ;
					}

					if (min==.0f){
						fDistanceToWall = fDepth;
						break;
					}

					nTestX = floor(fTestX += fEyeX * min);
					nTestY = floor(fTestY += fEyeY * min);
					fTestZ += fEyeZ * min;
					fDistanceToWall += min;

					if ((nTestX < 0 && fEyeX <= .0f)
					||	(nTestX >= nMapWidth && fEyeX >= .0f)
					||	(nTestY < 0 && fEyeY <= .0f)
					||	(nTestY >= nMapHeight && fEyeY >= .0f)){
						fDistanceToWall = fDepth;
						break;
					}

					if (1.f < fTestZ && .0f <= fEyeZ)
						bHit = 4;
					else if (fTestZ < .0f && fEyeZ <= .0f)
						bHit = 2;
					else if(nTestX < 0 || nMapWidth <= nTestX
						|| nTestY < 0  || nMapHeight <= nTestY) continue;
					else if(block == map.c_str()[nTestY * nMapWidth + nTestX]) continue;
					
					if(fTestZ <= 1.f && .0f <= fTestZ){
						block = map.c_str()[nTestY * nMapWidth + nTestX];
					}else block = ' ';

					if (block == '#' && fTestZ <= 1.f){
						bHit = 1;

						if (fEyeZ < .0f && .99f <= fTestZ){
							fTestX -= nTestX;
							fTestY -= nTestY;
							break;
						}
						
						float fTestAngle = fabs(atan2f((fTestY - (float)nTestY - .5f), (fTestX - (float)nTestX - .5f)));

						if(PI * .25f <= fTestAngle && fTestAngle <= PI * .75f)
							fTestX -= (float)nTestX;
						else 
							fTestX = fTestY - (float)nTestY;

						fTestY = 1 - fTestZ;
					}
				}

				if(fDepth <= fDistanceToWall)
					draw(x, y, 0x00000000);
				else if (bHit&4)
					draw(x, y, 0x00000000);
				else if (bHit&2){
					if(0 <= nTestX && nTestX < nMapWidth
					&& 0 <= nTestY && nTestY < nMapHeight)
						block = map.c_str()[nTestY * nMapWidth + nTestX];

					Color Shade = {0x00565656};
					if (block == 'G') Shade.color = 0x00ff0056;
					if (block == 'S') Shade.color = 0x000056ff;
					draw(x, y, synth(Shade, 1 - (fDistanceToWall / fDepth)));
				}else if(bHit&1){
					Color Shade = {spriteWall.grafics[((int)((fTestY) * 100) % 100) * 100 + ((int)((fTestX) * 100) % 100)]};
					draw(x, y, synth(Shade, 1 - (fDistanceToWall / fDepth)));
				}else draw(x, y, 0x00ffffff);
			}
		}
	}
	void mapVisualize(){
		for (int nx = 0; nx < nMapWidth; nx++)
			for (int ny = 0; ny < nMapHeight; ny++)
				for (int xSize = 0; xSize < 5; xSize++)
					for (int ySize = 0; ySize < 5; ySize++)
						draw((5 * nx) + xSize, (5 * ny) + ySize, map[ny * nMapWidth + nx] * 5000);

		for (int xSize = 0; xSize < 5; xSize++)
			for (int ySize = 0; ySize < 5; ySize++)
				draw(1 + (int)(fPlayerX) * 5 + xSize, 1 + (int)(fPlayerY) * 5 + ySize, L'P');
		
		float fPlayerAngleX = cosf(fPlayerAx) * 2.5f;
		float fPlayerAngleY = sinf(fPlayerAx) * 2.5f;

		draw(1 + (int)(fPlayerX) * 5 + (int)(2.5f + fPlayerAngleX), 1 + (int)(fPlayerY) * 5 + (int)(2.5f + fPlayerAngleY), 0x00ff0000);
	}
	int OnUserUpdate(float fElapsedTime){
		int ret = 0;
		float fStep = fSpeed * fElapsedTime;
		float fm[3] = {.0f, .0f, .0f};

		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			return -1;

		if (GetAsyncKeyState(VK_SPACE) & 0x8000 && canJump == true)
			fJump = Gravity * .55f;

		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
			fm[0] -= fStep * .5f;

		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
			fm[0] += fStep * .5f;

		if (GetAsyncKeyState(VK_UP) & 0x8000)
			fm[1] += fStep / PI;

		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
			fm[1] -= fStep / PI;

		if(fm[0]!=.0f){
			fPlayerAx += fm[0];
			ret |= 1;
		}

		if(fm[1]!=.0f){
			fm[1] += fPlayerAy;
			if (.5f < fabsf(fm[1]))
				fm[1]=int(fm[1]*2)*.5f;

			if(fPlayerAy!=fm[1]){
				fPlayerAy=fm[1];
				ret |= 1;
			}
		}

		if (GetAsyncKeyState((unsigned short)'E') & 0x8000)
			fm[2] += .25f;

		if (GetAsyncKeyState((unsigned short)'Q') & 0x8000)
			fm[2] -= .25f;

		if(fm[2]!=.0f){
			fm[2] += fDepth;
			if(fDepth<.0f||16.f<fDepth)
				fm[2]=int(fm[2]);
			if(fDepth!=fm[2]){
				fDepth=fm[2];
				ret |= 1;
			}
		}
		fm[0] = fm[1] = fm[2] = .0f;

		if (GetAsyncKeyState((unsigned short)'W') & 0x8000){
			fm[0]+=cosf(fPlayerAx);
			fm[1]+=sinf(fPlayerAx);
		}

		if (GetAsyncKeyState((unsigned short)'S') & 0x8000){
			fm[0]-=cosf(fPlayerAx);
			fm[1]-=sinf(fPlayerAx);
		}

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000){
			fm[0]-=sinf(fPlayerAx);
			fm[1]+=cosf(fPlayerAx);
		}

		if (GetAsyncKeyState((unsigned short)'A') & 0x8000){
			fm[0]+=sinf(fPlayerAx);
			fm[1]-=cosf(fPlayerAx);
		}

		fm[2]=fm[0]*fm[0]+fm[1]*fm[1];
		if(fm[2]!=.0f) {
			fm[2]=isr(fm[2]);
			ret |= move(fm[0]*fm[2]*fStep,fm[1]*fm[2]*fStep);
		}


		fm[0] = fPlayerZ + fJump * fElapsedTime - .5f * Gravity * fElapsedTime * fElapsedTime;

		if (fm[0] < .8f){
			fm[0] = .8f;
			fJump = .0f;
		}else if ((map.c_str()[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#' && fm[0] <= 1.8f)){
			fm[0] = 1.8f;
			fJump = .0f;
		}else fJump -= Gravity * fElapsedTime;

		if(fPlayerZ != fm[0]){
			fPlayerZ = fm[0];
			ret |= 1;
		}

		canJump = (fJump == .0f);

		return ret;
	}
	inline void ReplaceMap(int *_map, int width, int height, int oldVal, int newVal) {
		if (oldVal != newVal)
			for (int y = 0; y < height; y++)
				for (int x = 0; x < width; x++, _map++)
					if (*_map == oldVal) *_map = newVal;
	}
	void MakeMap() {
		//even->odd(+1)
		nMapWidth|=1;
		nMapHeight|=1;
		int *_map = new int[nMapHeight*nMapWidth];
		int width = nMapWidth;
		int height = nMapHeight;
		bool isMakeComp = false;
		for (int *fill = _map, count = width*height; count; count--) (*fill++) = -1;
		for (int y = 1, temp = 0; y < height; y += 2)
			for (int x = 1; x < width; x += 2) {
				*(_map + (y*width + x)) = temp;
				temp++;
			}
		for (int y = 1; !isMakeComp; )
			for (int x = 1; !isMakeComp; y = 2 * (rand() % ((height - 1) / 2)) + 1,
				x = 2 * (rand() % ((width - 1) / 2)) + 1) {
				isMakeComp = true;
				switch (rand() % 4 + 1) {
				case 1://Up
					if (2 < y && *(_map + (y*width + x)) != *(_map + ((y - 2)*width + x)) && (x != 1 || y != 3 || *(_map + (width + 2)) != 0)) {
						*(_map + ((y - 1)*width + x)) = 0;
						ReplaceMap(_map, width, height, MAX(*(_map + ((y - 2)*width + x)), *(_map + (y*width + x))),
							MIN(*(_map + ((y - 2)*width + x)), *(_map + (y*width + x))));
					}
					break;
				case 2://Down
					if (y < height - 3 && *(_map + (y*width + x)) != *(_map + ((y + 2)*width + x)) && (x != 1 || y != 1 || *(_map + (width + 2)) != 0)) {
						*(_map + ((y + 1)*width + x)) = 0;
						ReplaceMap(_map, width, height, MAX(*(_map + ((y + 2)*width + x)), *(_map + (y*width + x))),
							MIN(*(_map + ((y + 2)*width + x)), *(_map + (y*width + x))));
					}
					break;
				case 3://Left
					if (x < width - 3 && *(_map + (y*width + x)) != *(_map + (y*width + x + 2)) && (x != 1 || y != 1 || *(_map + (2 * width + 1)) != 0)) {
						*(_map + (y*width + x + 1)) = 0;
						ReplaceMap(_map, width, height, MAX(*(_map + (y*width + x + 2)), *(_map + (y*width + x))),
							MIN(*(_map + (y*width + x + 2)), *(_map + (y*width + x))));
					}
					break;
				case 4://Right
					if (1 < x && *(_map + (y*width + x)) != *(_map + (y*width + x - 2)) && (x != 3 || y != 1 || *(_map + (2 * width + 1)) != 0)) {
						*(_map + (y*width + x - 1)) = 0;
						ReplaceMap(_map, width, height, MAX(*(_map + (y*width + x - 2)), *(_map + (y*width + x))),
							MIN(*(_map + (y*width + x - 2)), *(_map + (y*width + x))));
					}
					break;
				}
				for (int _y = 0; _y < height; _y++)
					for (int _x = 0; _x < width; _x++)
						if (*(_map + (_y*width + _x)) != -1 && *(_map + (_y*width + _x)) != 0) isMakeComp = false;
						
			}
		ReplaceMap(_map, width, height, -1,1);
		
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++, _map++)
				if ((x != 1 || y != 1) && (x != width - 2 || y != height - 2)){
					map += !(*_map) ? L" " : L"#";
				}else if (x == 1 && y == 1){
					map += L"S";
				}else if (x == width - 2 && y == height - 2){
					map += L"G";
				}
		}
		return;
	}
public:
	G() {
		nMapWidth = nMapHeight = 32;

		fPlayerX = fPlayerY = 1.5f;
		fPlayerAx = PI * .5f;
		fPlayerZ = .8f;
		fPlayerAy = .0f;
		fSpeed = 1.5f;

		fFOV = PI * .25f;
		fFOVres = 1.f / 980.f;
		fDepth = 8.f;

		fDepthBuffer = nullptr;

		fJump = 0;

		canJump = true;

		spriteWall.grafics = (LPDWORD)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 100 * 100 * sizeof(DWORD));
		spriteWall.nGraphWidth = spriteWall.nGraphHeight = 100;

		MakeMap();
		WallGraph();
		init();
		View();
		mapVisualize();
		Draw();
	}
	~G() {
		HeapFree(GetProcessHeap(), 0, spriteWall.grafics);
	}
	void start() {
		auto tp1 = chrono::system_clock::now();
		auto tp2 = chrono::system_clock::now();
		int r;
		while (map.c_str()[(int)fPlayerY * nMapWidth + (int)fPlayerX] != 'G') {
			tp2 = chrono::system_clock::now();
			chrono::duration<float> elapsedTime = tp2 - tp1;
			tp1 = tp2;
			fElapsedTime = elapsedTime.count();
			if(1.f<fElapsedTime) fElapsedTime = 1.f;
			GetScreenSize();
			r=OnUserUpdate(fElapsedTime);
			if(r==-1) break;
			else if(UpdataScreenBuff() || r){
				View();
				mapVisualize();
				Draw();
			}
		}
		system("cls");
		printf("\n");
		/*while (_getch() != EOF);*/
	}
};

int main() {
	class G gr;
	omp_set_nested(1);
	gr.start();
	omp_set_nested(0);
	return 0;
}