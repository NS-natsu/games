#ifndef _PLAYER_CPP_
#define _PLAYER_CPP_ 

class Player{
private:
	float x;
	float y;
	float z;
	float vAngle;
	float hAngle;
	float fov;
	float fovRes;
	float speed;
	const float stature;
	float vAcce;
	bool ground;
	float v[3];
public:
	Player(float, float);
	bool rotH(float);
	bool rotV(float);
	bool onGround(float);
	bool gravity(float,float);
	bool isOver(float);
	void move(float,float);
	float getX(void);
	float getY(void);
	float getZ(void);
	float getHAngle(void);
	float getVAngle(void);
	bool jump(void);
	float getSpeed(void);
	float getFOV(float,float);
};

inline float Player::getX(void) {
	return x;
}

inline float Player::getY(void) {
	return y;
}

inline float Player::getZ(void) {
	return z;
}

inline float Player::getHAngle(void){
	return hAngle;
}

inline float Player::getVAngle(void){
	return vAngle;
}

#if __INCLUDE_LEVEL__ == 0

#define Gravity (9.8f)
#define PI (3.14159f)

Player::Player(float _x, float _y) : stature(.8f), fov(PI * .25f), fovRes(1.f / 980.f), speed(1.5f){
	x = _x;
	y = _y;
	z = stature;
	vAngle = .0f;
	hAngle = PI * .5f;
	vAcce = 0;
	ground = true;
}

bool Player::rotH(float d){
	hAngle += d;
	return (d != .0f);
}
bool Player::rotV(float d){
	d += vAngle;
	const float lim = .5f;
	if (d < -lim || lim < d)
		d = float((d > lim) - (d < -lim)) * lim;
	bool ret = (vAngle!=d);
	vAngle = d;
	return ret;
}

bool Player::onGround(float gndHeight){
	ground = (z - stature < gndHeight);
	return ground;
}

bool Player::gravity(float gndHeight, float t){
	//if(onGround(gndHeight)) return false;
	float _z = z + vAcce * t - .5f * Gravity * t * t;

	if (_z <= gndHeight + stature){
		_z = gndHeight + stature;
		ground = true;
		vAcce = .0f;
	}else vAcce -= Gravity * t;

	bool ret = (z != _z);
	z = _z;
	return ret;
}

bool Player::isOver(float h){
	return h <= (z - stature);
}

void Player::move(float dx, float dy){
	x += dx;
	y += dy;
}

bool Player::jump(void){
	if(!ground) return false;
	vAcce = Gravity * .5f;
	ground = false;
	return true;
}

float Player::getSpeed(void){
	return speed;
}

float Player::getFOV(float scale, float lim){
	float f = fov * scale * fovRes;
	return ( f < fov * lim ) ? f : fov * lim;
}
#endif
#endif