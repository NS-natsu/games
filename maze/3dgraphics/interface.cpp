#ifndef _INTERFACE_CPP_
#define _INTERFACE_CPP_

#include <Windows.h>

#include "player.cpp"
#include "map.cpp"
#include "screen.cpp"

class Interface {
private:
	float fDepth;
	float fElapsedTime;
	float isr(float);
public:
	int onUserUpdate(float,Player&,Map&,Screen&);
	Interface();
	~Interface();
};

#if __INCLUDE_LEVEL__ == 0
#define PI (3.14159f)

#define GetKey(k) ((GetAsyncKeyState(k) >>15) & 1)

float Interface::isr(float x){
	if(x == .0f) return x;
	long X = *(long*)&x;
	long Y = 0x5F3759DF - (X >> 1);
	float y = *(float*)&Y;
	x *= .5f;
	y = y * (1.5f - (x * y * y));
	//y = y * (1.5f - (x * y * y));
	return y;
}
int Interface::onUserUpdate(float fElapsedTime, Player& p, Map& map, Screen& s){
	int ret = 0;
	float fStep = p.getSpeed() * fElapsedTime;

	if (GetKey(VK_ESCAPE))
		return -1;

	if (GetKey(VK_SPACE)) ret = p.jump();

	float fm[3] = {
		fStep * .5f * float(GetKey(VK_RIGHT) - GetKey(VK_LEFT)),
		fStep / PI * float(GetKey(VK_UP) - GetKey(VK_DOWN)),
		.25f * float(GetKey('E') - GetKey('Q'))
	};

	ret |= p.rotH(fm[0]) | p.rotV(fm[1]) | s.setDepth(fm[2]);
	float angle = p.getHAngle();

	fm[0]  = fm[1]  = float(GetKey('W') - GetKey('S'));
	fm[2]  = float(GetKey('D') - GetKey('A'));
	fm[0] *= cosf(angle);
	fm[1] *= sinf(angle);
	fm[0] -= sinf(angle) * fm[2];
	fm[1] += cosf(angle) * fm[2];
	fm[2]=isr(fm[0]*fm[0]+fm[1]*fm[1]);

	ret |= map.move(fm[0]*fm[2]*fStep,fm[1]*fm[2]*fStep,p);

	ret |= p.gravity(float(map.getPos(p)==map.Wall), fElapsedTime);

	return ret;
}
Interface::Interface() {}
Interface::~Interface() {}
#endif
#endif