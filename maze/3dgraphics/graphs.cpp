#ifndef _GRAPHS_CPP_
#define _GRAPHS_CPP_
#include <Windows.h>
class Graphs {
	private:
		HWND hConsole;
		LPDWORD screen;
		BITMAPINFO biDIB;

		int nScreenWidth;
		int nScreenHeight;
	protected:
		void GetScreenSize(void);
		bool isVaryBuff(void);
		void draw(void);
		void setPixel(int, int, DWORD);
		void fillRect(int, int, int, int, DWORD);
	public:
		int ScreenWidth(void);
		int ScreenHeight(void);
		bool updateScreenBuff(void);
		Graphs();
		~Graphs();
};
#if __INCLUDE_LEVEL__ == 0

void Graphs::GetScreenSize(void) {
	RECT rc;
	GetClientRect(hConsole, &rc);
	nScreenWidth = rc.right - rc.left;
	nScreenHeight = rc.bottom - rc.top;
}
int Graphs::ScreenWidth(void)
	{return biDIB.bmiHeader.biWidth;}
int Graphs::ScreenHeight(void)
	{return biDIB.bmiHeader.biHeight;}
bool Graphs::updateScreenBuff(void) {
	if(screen==nullptr){
		//biDIB.bmiHeader.biWidth = nScreenWidth;
		//biDIB.bmiHeader.biHeight = nScreenHeight;
		if(screen!=nullptr) HeapFree(GetProcessHeap(), 0, screen);
		screen = (LPDWORD)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, biDIB.bmiHeader.biWidth * biDIB.bmiHeader.biHeight * sizeof(DWORD));
		return screen != NULL;
	}
	return false;
}
void Graphs::draw(void) {
	HDC dc = GetDC(hConsole);
	GetScreenSize();
	int imgW = biDIB.bmiHeader.biWidth;
	int imgH = biDIB.bmiHeader.biHeight;
	int offX=0,offY=0;
	if(nScreenWidth < imgW * .5){
		offX = (imgW - nScreenWidth) / 2;
		imgW = nScreenWidth;
	}
	if(nScreenHeight < imgH * .5){
		offY = (imgH - nScreenHeight) / 2;
		imgH = nScreenHeight;
	}

	SetStretchBltMode(dc, HALFTONE);
	StretchDIBits(dc, 0, nScreenHeight, nScreenWidth, -nScreenHeight,
		offX, offY, imgW, imgH,
		screen, &biDIB, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC(hConsole, dc);
}
void Graphs::setPixel(const int x, const int y, const DWORD color) {
	screen[y * biDIB.bmiHeader.biWidth + x] = color;
}
void Graphs::fillRect(int offX, int offY, int w, int h, DWORD color) {
	if(offX < 0){
		w += offX;
		offX = 0;
	}
	if(biDIB.bmiHeader.biWidth <= offX + w)
		w = biDIB.bmiHeader.biWidth - offX - 1;

	if(offY < 0){
		h += offY;
		offY = 0;
	}
	if(biDIB.bmiHeader.biHeight <= offY + h)
		h = biDIB.bmiHeader.biHeight - offX - 1;

	if(w < 0 || h < 0) return;

	int off = offY * biDIB.bmiHeader.biWidth + offX;
	for(int i = 0; i < h; i++)
		for(int j = 0; j < w; j++)
			screen[off + i * biDIB.bmiHeader.biWidth + j] = color;
}
bool Graphs::isVaryBuff(void){
	return false;
	//get monitor size
	//get dpi
	const int& oldW = biDIB.bmiHeader.biWidth;
	const int& oldH = biDIB.bmiHeader.biHeight;
	int newDPI;
	int newW;
	int newH;
}

Graphs::Graphs() {
	hConsole = GetConsoleWindow();
	GetScreenSize();

	screen = nullptr;

	ZeroMemory(&biDIB, sizeof(biDIB));
	biDIB.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	biDIB.bmiHeader.biPlanes = 1;
	biDIB.bmiHeader.biBitCount = 32;
	biDIB.bmiHeader.biCompression = BI_RGB;

	RECT cl, wn;
	GetClientRect(hConsole, &cl);
	GetWindowRect(hConsole, &wn);
	biDIB.bmiHeader.biWidth = GetSystemMetrics(SM_CXFULLSCREEN) - (wn.right - wn.left) + (cl.right - cl.left);
	biDIB.bmiHeader.biHeight = GetSystemMetrics(SM_CYFULLSCREEN) -  (wn.bottom - wn.top) + (cl.bottom - cl.top);
	biDIB.bmiHeader.biWidth *= 0.7f;
	biDIB.bmiHeader.biHeight *= 0.7f;
	updateScreenBuff();
}
Graphs::~Graphs() {
	HeapFree(GetProcessHeap(), 0, screen);
}
#endif
#endif