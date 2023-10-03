#pragma once

#include "CircleObj.h"
#include "../geometry/Vec2F.h"
#include "../geometry/Pos2F.h"

class BallObj : public CircleObj {
public:
	BallObj(const Pos2F& startPos, float radius, const Material& mat, float mass);
	void Update(const Vec2F& accel, float dt);
	void Rewind(float dt);
	void UpdateFromCollision(const Vec2F& newVel, const Pos2F& newCenterPos);
	Vec2F GetVel() const { return vel; }
	float GetMass() const { return mass; }
	Pos2F GetOldPos() const { return oldPos; }
private:
	Vec2F vel;
	Vec2F oldVel;
	Pos2F oldPos;
	float mass;
};