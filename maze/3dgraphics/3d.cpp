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
	void SetScreenSize() {
		ZeroMemory(&biDIB, sizeof(biDIB));
		biDIB.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		biDIB.bmiHeader.biWidth = ScreenWidth();
		biDIB.bmiHeader.biHeight = ScreenHeight();
		biDIB.bmiHeader.biPlanes = 1;
		biDIB.bmiHeader.biBitCount = 32;
		biDIB.bmiHeader.biCompression = BI_RGB;

		HeapFree(GetProcessHeap(), 0, screen);
		screen = (LPDWORD)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ScreenWidth() * ScreenHeight() * sizeof(DWORD));
	}
	void Draw() {
		StretchDIBits(GetDC(hConsole), 0, nScreenHeight, nScreenWidth, -nScreenHeight,
			0, 0, biDIB.bmiHeader.biWidth, biDIB.bmiHeader.biHeight,
			screen, &biDIB, DIB_RGB_COLORS, SRCCOPY);
	}
	void draw(int x, int y, DWORD color) {
		screen[y * ScreenWidth() + x] = color;
	}
public:
	Graphs() {
		hConsole = GetConsoleWindow();
		GetScreenSize();

		screen = (LPDWORD)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 1);
		SetScreenSize();
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
protected:
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
		
		const float frxs = fPlayerAx - fFOV / 2.f;
		const float frys = fPlayerAy + fFOV / 2.f;
		const float frxd = fFOV / (float)ScreenWidth();
		const float fryd = fFOV / (float)ScreenHeight();

		#pragma omp parallel for
		for (int x = 0; x < ScreenWidth(); x++){
			const float fEyeX = cosf(frxs + x * frxd);
			const float fEyeY = sinf(frxs + x * frxd);
			for (int y = 0; y < ScreenHeight(); y++){
				const float fEyeZ = sinf(frys - y * fryd);
				float v[4] = {cosf(fPlayerAx), sinf(fPlayerAx), sinf(fPlayerAy), 0};
				v[3] = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
				v[0] /= v[3]; v[1] /= v[3]; v[2] /= v[3];

				/*1: wall, 2: floor, 4: ceiling*/
				char bHit = 0;

				int nTestX = 0;
				int nTestY = 0;

				float fTestX = fPlayerX - v[0] + fEyeX / fabsf(fEyeX * v[0] + fEyeY * v[1] + fEyeZ * v[2]); //fPlayerX > .0f
				float fTestY = fPlayerY - v[1] + fEyeY / fabsf(fEyeX * v[0] + fEyeY * v[1] + fEyeZ * v[2]); //fPlayerY > .0f
				float fTestZ = fPlayerZ - v[2] + fEyeZ / fabsf(fEyeX * v[0] + fEyeY * v[1] + fEyeZ * v[2]); //fPlayerZ > .8f

				float fSampleX = 0;
				float fSampleY = 0;
				
				char block = ' ';

				float fDistanceToWall = .0f;
				float dX, dY, dZ, min;
				while(!bHit && fDistanceToWall < fDepth){
					min = .0f;
					if(fEyeX!=.0f){
						dX = (floorf(fTestX)+float(fEyeX>.0f)-fTestX)/fEyeX;
						if(!dX) dX = (floorf(fTestX)-1.f-fTestX)/fEyeX;
						if(fEyeX<.0f) dX -= fStepSize/fEyeX;
						min = dX;
					}
					if(fEyeY!=.0f){
						dY = (floorf(fTestY)+float(fEyeY>.0f)-fTestY)/fEyeY;
						if(!dY) dY = (floorf(fTestY)-1.f-fTestY)/fEyeY;
						if(fEyeY<.0f) dY -= fStepSize/fEyeY;
						if(dY < min || min == .0f) min = dY;
					}
					if(fEyeZ!=.0f){
						dZ = (floorf(fTestZ)+float(fEyeZ>.0f)-fTestZ)/fEyeZ;
						if(!dZ) dZ = (floorf(fTestZ)-1.f-fTestZ)/fEyeZ;
						if(fEyeZ<.0f) dZ -= fStepSize/fEyeZ;
						if(dZ < min || min == .0f) min = dZ;
					}

					if (min==.0f){
						fDistanceToWall = fDepth;
						continue;
					}

					fTestX += fEyeX * min;
					fTestY += fEyeY * min;
					fTestZ += fEyeZ * min;
					fDistanceToWall += min;
					
					if (	fTestX < .0f || fTestX >= (float)nMapWidth
					||		fTestY < .0f || nTestY >= (float)nMapHeight){
						fDistanceToWall = fDepth;
						continue;
					}

					nTestX = (int)fTestX;
					nTestY = (int)fTestY;

					block = map.c_str()[nTestY * nMapWidth + nTestX];

					if (1.f < fTestZ && .0f < fEyeZ)
						bHit |= 4;
					else if (fTestZ < 0.f)
						bHit |= 2;
					else if (block == '#' && fTestZ <= 1.f){
						bHit |= 1;

						if (fEyeZ < .0f && .99f <= fTestZ){
							fSampleX = fTestX - (float)nTestX;
							fSampleY = fTestY - (float)nTestY;
							continue;
						}
						
						float fTestAngle = atan2f((fTestY - (float)nTestY - .5f), (fTestX - (float)nTestX - .5f));

						if (fTestAngle >= -PI * .25f && fTestAngle < PI * .25f)
							fSampleX = fTestY - (float)nTestY;

						else if (fTestAngle >= PI * .25f && fTestAngle < PI * .75f)
							fSampleX = fTestX - (float)nTestX;

						else if (fTestAngle < -PI * .25f && fTestAngle >= -PI * .75f)
							fSampleX = fTestX - (float)nTestX;

						else if (fTestAngle >= PI * .75f || fTestAngle < -PI * .75f)
							fSampleX = fTestY - (float)nTestY;

						fSampleY = 1 - fTestZ;
					}
				}
				
				if(fDepth <= fDistanceToWall)
					draw(x, y, 0x00000000);
				else if (bHit&4)
					draw(x, y, 0x00000000);
				else if (bHit&2){
					DWORD Shade = 0x00565656;
					if (block == 'G') Shade = 0x00ff0056;
					if (block == 'S') Shade = 0x000056ff;
					union {
						unsigned long color;
						struct {
							unsigned long Blue : 8;
							unsigned long Green : 8;
							unsigned long Red : 8;
							unsigned long free : 8;
						} RGB;
					} Color;
					Color.color = Shade;
					Color.RGB.Red *= 1 - (fDistanceToWall / fDepth);
					Color.RGB.Blue *= 1 - (fDistanceToWall / fDepth);
					Color.RGB.Green *= 1 - (fDistanceToWall / fDepth);
					Shade = Color.color;
					draw(x, y, Shade);
				}else if(bHit&1){
					DWORD Shade = spriteWall.grafics[((int)((fSampleY) * 100) % 100) * 100 + ((int)((fSampleX) * 100) % 100)];
					union {
						unsigned long color;
						struct {
							unsigned long Blue : 8;
							unsigned long Green : 8;
							unsigned long Red : 8;
							unsigned long free : 8;
						} RGB;
					} Color;
					Color.color = Shade;
					Color.RGB.Red *= 1 - (fDistanceToWall / fDepth);
					Color.RGB.Blue *= 1 - (fDistanceToWall / fDepth);
					Color.RGB.Green *= 1 - (fDistanceToWall / fDepth);
					Shade = Color.color;
					draw(x, y, Shade);
				}else draw(x, y, 0x00ffffff);
			}
		}
		
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
		
		Draw();
	}
	void OnUserUpdate(float fElapsedTime){
		if (GetAsyncKeyState(VK_SPACE) & 0x8000 && canJump == true)
			fJump = Gravity / 2.f;

		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
			fPlayerAx -= fSpeed * fElapsedTime;

		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
			fPlayerAx += fSpeed * fElapsedTime;

		if (GetAsyncKeyState(VK_UP) & 0x8000 && fPlayerAy < 1.f)
			fPlayerAy += fSpeed * fElapsedTime;

		if (GetAsyncKeyState(VK_DOWN) & 0x8000 && -1.f < fPlayerAy)
			fPlayerAy -= fSpeed * fElapsedTime;


		if (GetAsyncKeyState((unsigned short)'E') & 0x8000 && fDepth < 16.f)
			fDepth += .25f;

		if (GetAsyncKeyState((unsigned short)'Q') & 0x8000 && .0f < fDepth)
			fDepth -= .25f;

		if (GetAsyncKeyState((unsigned short)'W') & 0x8000){
			fPlayerX += cosf(fPlayerAx) * fSpeed * fElapsedTime;
			fPlayerY += sinf(fPlayerAx) * fSpeed * fElapsedTime;
			if (fPlayerX < .0f || nMapWidth <= (int)fPlayerX || fPlayerY < .0f || nMapHeight <= (int)fPlayerY || 
				(map.c_str()[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#' && fPlayerZ < 1.8f)){
				fPlayerX -= cosf(fPlayerAx) * fSpeed * fElapsedTime;
				fPlayerY -= sinf(fPlayerAx) * fSpeed * fElapsedTime;
			}
		}

		if (GetAsyncKeyState((unsigned short)'S') & 0x8000){
			fPlayerX -= cosf(fPlayerAx) * fSpeed * fElapsedTime;
			fPlayerY -= sinf(fPlayerAx) * fSpeed * fElapsedTime;
			if (fPlayerX < .0f || nMapWidth <= (int)fPlayerX || fPlayerY < .0f || nMapHeight <= (int)fPlayerY || 
				(map.c_str()[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#' && fPlayerZ < 1.8f)){
				fPlayerX += cosf(fPlayerAx) * fSpeed * fElapsedTime;
				fPlayerY += sinf(fPlayerAx) * fSpeed * fElapsedTime;
			}
		}

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000){
			fPlayerX -= sinf(fPlayerAx) * fSpeed * fElapsedTime;
			fPlayerY += cosf(fPlayerAx) * fSpeed * fElapsedTime;
			if (fPlayerX < .0f || nMapWidth <= (int)fPlayerX || fPlayerY < .0f || nMapHeight <= (int)fPlayerY || (
				map.c_str()[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#' && fPlayerZ < 1.8f)){
				fPlayerX += sinf(fPlayerAx) * fSpeed * fElapsedTime;
				fPlayerY -= cosf(fPlayerAx) * fSpeed * fElapsedTime;
			}
		}

		if (GetAsyncKeyState((unsigned short)'A') & 0x8000){
			fPlayerX += sinf(fPlayerAx) * fSpeed * fElapsedTime;
			fPlayerY -= cosf(fPlayerAx) * fSpeed * fElapsedTime;
			if (fPlayerX < .0f || nMapWidth <= (int)fPlayerX || fPlayerY < .0f || nMapHeight <= (int)fPlayerY || 
				(map.c_str()[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#' && fPlayerZ < 1.8f)){
				fPlayerX -= sinf(fPlayerAx) * fSpeed * fElapsedTime;
				fPlayerY += cosf(fPlayerAx) * fSpeed * fElapsedTime;
			}
		}

		if (1.f < fPlayerAy)
			fPlayerAy = 1.f;
		else if (fPlayerAy < -1.f)
			fPlayerAy = -1.f;

		fPlayerZ += fJump * fElapsedTime - .5f * Gravity * fElapsedTime * fElapsedTime;

		if (fPlayerZ < .8f){
			fPlayerZ = .8f;
			fJump = .0f;
		}else if ((map.c_str()[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#' && fPlayerZ <= 1.8f)){
			fPlayerZ = 1.8f;
			fJump = .0f;
		}else fJump -= Gravity * fElapsedTime;

		canJump = (fJump == .0f);
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
				case 1://è„
					if (2 < y && *(_map + (y*width + x)) != *(_map + ((y - 2)*width + x)) && (x != 1 || y != 3 || *(_map + (width + 2)) != 0)) {
						*(_map + ((y - 1)*width + x)) = 0;
						ReplaceMap(_map, width, height, MAX(*(_map + ((y - 2)*width + x)), *(_map + (y*width + x))),
							MIN(*(_map + ((y - 2)*width + x)), *(_map + (y*width + x))));
					}
					break;
				case 2://â∫
					if (y < height - 3 && *(_map + (y*width + x)) != *(_map + ((y + 2)*width + x)) && (x != 1 || y != 1 || *(_map + (width + 2)) != 0)) {
						*(_map + ((y + 1)*width + x)) = 0;
						ReplaceMap(_map, width, height, MAX(*(_map + ((y + 2)*width + x)), *(_map + (y*width + x))),
							MIN(*(_map + ((y + 2)*width + x)), *(_map + (y*width + x))));
					}
					break;
				case 3://âE
					if (x < width - 3 && *(_map + (y*width + x)) != *(_map + (y*width + x + 2)) && (x != 1 || y != 1 || *(_map + (2 * width + 1)) != 0)) {
						*(_map + (y*width + x + 1)) = 0;
						ReplaceMap(_map, width, height, MAX(*(_map + (y*width + x + 2)), *(_map + (y*width + x))),
							MIN(*(_map + (y*width + x + 2)), *(_map + (y*width + x))));
					}
					break;
				case 4://ç∂
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
		fPlayerAx = PI / 2.f;
		fPlayerZ = .8f;
		fPlayerAy = .0f;
		fSpeed = 3.f;

		fFOV = PI / 4.f;
		fDepth = 8.f;

		fDepthBuffer = nullptr;

		fJump = 0;

		canJump = true;

		spriteWall.grafics = (LPDWORD)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 100 * 100 * sizeof(DWORD));
		spriteWall.nGraphWidth = spriteWall.nGraphHeight = 100;

		MakeMap();
		WallGraph();
		GetScreenSize();
		SetScreenSize();
	}
	~G() {
	}
	void start() {
		auto tp1 = chrono::system_clock::now();
		auto tp2 = chrono::system_clock::now();
		while (map.c_str()[(int)fPlayerY * nMapWidth + (int)fPlayerX] != 'G') {
			tp2 = chrono::system_clock::now();
			chrono::duration<float> elapsedTime = tp2 - tp1;
			tp1 = tp2;
			fElapsedTime = elapsedTime.count();
			GetScreenSize();
			SetScreenSize();
			OnUserUpdate(fElapsedTime / 1.6f);
			View();
		}
		system("cls");
		printf("ÉNÉäÉA\n");
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