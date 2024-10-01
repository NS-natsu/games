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
	RECT lprc;
	GetClientRect(hConsole, &lprc);
	nScreenWidth = lprc.right - lprc.left;
	nScreenHeight = lprc.bottom - lprc.top;
}
int Graphs::ScreenWidth(void)
	{return nScreenWidth;}
int Graphs::ScreenHeight(void)
	{return nScreenHeight;}
bool Graphs::updateScreenBuff(void) {
	if(isVaryBuff() || screen==nullptr){
		biDIB.bmiHeader.biWidth = nScreenWidth;
		biDIB.bmiHeader.biHeight = nScreenHeight;
		if(screen!=nullptr) HeapFree(GetProcessHeap(), 0, screen);
		screen = (LPDWORD)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nScreenWidth * nScreenHeight * sizeof(DWORD));
		return true;
	}
	return false;
}
void Graphs::draw(void) {
	StretchDIBits(GetDC(hConsole), 0, nScreenHeight, nScreenWidth, -nScreenHeight,
		0, 0, biDIB.bmiHeader.biWidth, biDIB.bmiHeader.biHeight,
		screen, &biDIB, DIB_RGB_COLORS, SRCCOPY);
}
void Graphs::setPixel(int x, int y, DWORD color) {
	screen[y * nScreenWidth + x] = color;
}
void Graphs::fillRect(int offX, int offY, int w, int h, DWORD color) {
	if(offX < 0){
		w += offX;
		offX = 0;
	}
	if(nScreenWidth <= offX + w)
		w = nScreenWidth - offX - 1;

	if(offY < 0){
		h += offY;
		offY = 0;
	}
	if(nScreenHeight <= offY + h)
		h = nScreenHeight - offX - 1;

	if(w < 0 || h < 0) return;

	int off = offY * nScreenWidth + offX;
	for(int i = 0; i < h; i++)
		for(int j = 0; j < w; j++)
			screen[off + i * nScreenWidth + j] = color;
}
bool Graphs::isVaryBuff(void){
	GetScreenSize();
	return (nScreenWidth != biDIB.bmiHeader.biWidth)
		|| (nScreenHeight != biDIB.bmiHeader.biHeight);
}
Graphs::Graphs() {
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

	updateScreenBuff();
}
Graphs::~Graphs() {
	HeapFree(GetProcessHeap(), 0, screen);
}
#endif
#endif