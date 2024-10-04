#include <stdio.h>
#include <conio.h>
#include <omp.h>
#include <chrono>
#include "graphs.cpp"
#include "map.cpp"
#include "player.cpp"
#include "screen.cpp"
#include "interface.cpp"

void start(){
	Map map = Map(33, 33);
	Player p = Player(1.5f, 1.5f);
	Screen s = Screen();
	Interface i = Interface();

	int r;
	auto tp1 = std::chrono::system_clock::now();
	auto tp2 = std::chrono::system_clock::now();
	std::chrono::duration<float> elapsedTime;
	while (map.getPos(p) != map.Goal) {
		tp2 = std::chrono::system_clock::now();
		elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();
		if(1.f<fElapsedTime) fElapsedTime = 1.f;
		r = i.onUserUpdate(fElapsedTime, p, map, s);

		if(r==-1) break;
		else if(s.updateScreenBuff()|| r){
			s.update(map, p);
		}
	}
	while(kbhit()) _getch();
	while(!kbhit());
}

int main(){
	omp_set_nested(1);
	start();
	omp_set_nested(0);
	return 0;
}