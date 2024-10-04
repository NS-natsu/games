#ifndef _SCREEN_CPP_
#define _SCREEN_CPP_
#include <Windows.h>

#include "graphs.cpp"
#include "map.cpp"
#include "player.cpp"

class Screen : public Graphs {
private:
	float fDepth;
	struct Sprite {
		LPDWORD grafics;
		int nGraphWidth;
		int nGraphHeight;
	} spriteWall;
	typedef union {
		DWORD color;
		struct {
			DWORD Blue : 8;
			DWORD Green : 8;
			DWORD Red : 8;
			DWORD free : 8;
		} RGB;
	} Color;
	inline unsigned long synth(Color,float);
	float isr(float);
	float* normalize(float[3]);
protected:
	void generateWallGraph();
	void view(Player&,Map&);
	void mapVisualize(Map&,Player&);
public:
	Screen();
	~Screen();
	void update(Map&, Player&);
	bool setDepth(float);
};

#if __INCLUDE_LEVEL__ == 0
#include <math.h>
#include <omp.h>
#define PI (3.14159f)

inline unsigned long Screen::synth(Color c, float ratio){
	c.RGB.Red *= ratio;
	c.RGB.Blue *= ratio;
	c.RGB.Green *= ratio;
	return c.color;
}

float Screen::isr(float x){
	long X = *(long*)&x;
	long Y = 0x5F3759DF - (X >> 1);
	float y = *(float*)&Y;
	x *= .5f;
	y = y * (1.5f - (x * y * y));
	//y = y * (1.5f - (x * y * y));
	return y;
}

float* Screen::normalize(float v[3]){
	float n = isr(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	v[0] *= n; v[1] *= n; v[2] *= n;
	return v;
}
void Screen::generateWallGraph() {
	int index = 0;
	const int& w = spriteWall.nGraphWidth;
	const int& h = spriteWall.nGraphHeight;
	DWORD color;
	int quart = (h + 4) >> 2;
	for (int y = 0; y < h; y++){
		for (int x = 0; x < w; x++) {
			color = 0x00c16037;

			if (!y || !(y-h+1)) color = 0x00cccccc;
			else if(abs( (((y+2)%quart)<<1) - (h>>2) ) >> 1 == h >> 3) color = 0x00cccccc;
			else if(1&((y+2)/quart)){
				if (!x || !(x-w+1)) color = 0x00cccccc;
				if (!abs( ((x<<1)-w+1) >> 1 )) color = 0x00cccccc;
			}else if(!abs( ((abs(((x<<1)-w+1)>>1)<<2)-w+1)>>2 )) color = 0x00cccccc;

			spriteWall.grafics[index++] = color;
		}
	}
}
void Screen::view(Player& p, Map& m) {
	const float fStepSize = .01f;
	
	const float frxs = p.getHAngle() - p.getFOV((float)ScreenWidth(), 1.f) * .5f;
	const float frys = p.getVAngle() + p.getFOV((float)ScreenHeight(), 2.f) * .5f;


	float frxd = p.getFOV((float)ScreenWidth(),1.f) / (float)ScreenWidth();
	float fryd = p.getFOV((float)ScreenHeight(),2.f) / (float)ScreenHeight();

	float v[3] = {cosf(p.getHAngle()), sinf(p.getHAngle()), sinf(p.getVAngle())};
	normalize(v);
	//v[0]*= 1.2f;v[1]*= 1.2f;v[2]*= 1.2f;

	#pragma omp parallel for
	for (int x = 0; x < ScreenWidth(); x++){
		const float fEyeX = cosf(frxs + x * frxd);
		const float fEyeY = sinf(frxs + x * frxd);
		const float fiex = (fEyeX!=.0f) ? (1.f/fEyeX) : .0f;
		const float fiey = (fEyeY!=.0f) ? (1.f/fEyeY) : .0f;
		for (int y = 0; y < ScreenHeight(); y++){
			const float fEyeZ = sinf(frys - y * fryd);
			const float fiez = (fEyeZ!=.0f) ? (1.f/fEyeZ) : .0f;


			float fTestX = p.getX() - v[0] + fEyeX / (fEyeX * v[0] + fEyeY * v[1] + fEyeZ * v[2]); //p.getX() > .0f
			float fTestY = p.getY() - v[1] + fEyeY / (fEyeX * v[0] + fEyeY * v[1] + fEyeZ * v[2]); //p.getY() > .0f
			float fTestZ = p.getZ() - v[2] + fEyeZ / (fEyeX * v[0] + fEyeY * v[1] + fEyeZ * v[2]); //p.getZ() > .8f

			int nTestX = floorf(fTestX);
			int nTestY = floorf(fTestY);
			
			char block = m.Air;
			if(.0f <= fTestZ && fTestZ <= 1.f
			&& m.isArea(nTestX, nTestY))
				block = m.getPos(nTestX, nTestY);

			//1: wall, 2: floor, 4: none
			char bHit = 0;
			if (1.f < fTestZ && .0f <= fEyeZ)
				bHit = 4;
			else if (fTestZ < .0f && fEyeZ <= .0f)
				bHit = 2;

			float d, min;
			float fDistanceToWall = .0f;
			while(!bHit&&fDistanceToWall<fDepth){
				min = fDepth - fDistanceToWall + fStepSize;
				if(fEyeX!=.0f){
					d = ( (fEyeX > .0f ? floorf(fTestX + 1.f) : ceilf(fTestX - 1.f) - fStepSize) - fTestX ) * fiex;
					if(d < min) min = d;
				}
				if(fEyeY!=.0f){
					d = ( (fEyeY > .0f ? floorf(fTestY + 1.f) : ceilf(fTestY - 1.f) - fStepSize) - fTestY ) * fiey;
					if(d < min) min = d;
				}
				if(fEyeZ!=.0f){
					d = ( (fEyeZ > .0f ? floorf(fTestZ + 1.f) : ceilf(fTestZ - 1.f) - fStepSize) - fTestZ ) * fiez;
					if(d < min) min = d;
				}

				fDistanceToWall += min;
				if (fDepth <= fDistanceToWall) {
					bHit = 4;
					break;
				}

				nTestX = floor(fTestX += fEyeX * min);
				nTestY = floor(fTestY += fEyeY * min);

				if(!m.isArea(nTestX, nTestY)){
					bHit = 4;
					break;
				}

				fTestZ += fEyeZ * min;
				int inHeight = ((1.f < fTestZ) - (fTestZ < .0f));
				if(inHeight){
					if(.0f <= float(inHeight) * ((.0f <= fEyeZ) - (fEyeZ <= .0f)))
						bHit = 3 + inHeight;
					block = m.Air;
					continue;
				}else if(block == m.getPos(nTestX, nTestY)) continue;
				
				block = m.getPos(nTestX, nTestY);

				if (block == m.Wall){
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

			if (bHit&4)
				setPixel(x, y, 0x00000000);
			else if (bHit&2){
				block = m.getPos(nTestX, nTestY);
				Color Shade = {0x00565656};
				if (block == m.Goal) Shade.color = 0x00ff0056;
				if (block == m.Start) Shade.color = 0x000056ff;
				setPixel(x, y, synth(Shade, 1 - (fDistanceToWall / fDepth)));
			}else if(bHit&1){
				Color Shade = {spriteWall.grafics[(int(fTestY * 128) & 127) * 128 + (int(fTestX * 128) & 127)]};
				setPixel(x, y, synth(Shade, 1 - (fDistanceToWall / fDepth)));
			}else setPixel(x, y, 0x00ffffff);
		}
	}
}
void Screen::mapVisualize(Map& map, Player& player){
	for (int nx = 0; nx < map.getWidth(); nx++)
		for (int ny = 0; ny < map.getHeight(); ny++)
				fillRect(5 * nx, 5 * ny, 5, 5, map.getPos(nx, ny) * 5000);

	fillRect(int(player.getX()) * 5, int(player.getY()) * 5, 5, 5,L'P');
	
	float fPlayerAngleX = cosf(player.getHAngle()) * 2.5f;
	float fPlayerAngleY = sinf(player.getHAngle()) * 2.5f;
	setPixel(int(player.getX()) * 5 + int(2.5f + fPlayerAngleX),
			 int(player.getY()) * 5 + int(2.5f + fPlayerAngleY), 0x00ff0000);
}

void Screen::update(Map& map, Player& player){
	view(player, map);
	mapVisualize(map, player);
	draw();
}
Screen::Screen() : fDepth(8.f){
	spriteWall.grafics = (LPDWORD)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 128 * 128 * sizeof(DWORD));
	spriteWall.nGraphWidth = spriteWall.nGraphHeight = 128;
	generateWallGraph();
}
Screen::~Screen() {
	HeapFree(GetProcessHeap(), 0, spriteWall.grafics);
}
bool Screen::setDepth(float d){
	d += fDepth;
	if(d < .0f || 16.f < d) d = int(d);
	bool ret = (fDepth != d);
	fDepth = d;
	return ret;
}
#endif
#endif