#include "BallObj.h"
#include <assert.h>
#include <iostream>
#include "Material.h"

BallObj::BallObj(const Pos2F& startPos, float radius, const Material& mat, float mass) :
	CircleObj(startPos, radius, mat, 50),
	vel(0,0),
	oldVel(0,0),
	oldPos(startPos),
	mass(mass)
{

}

void BallObj::Update(const Vec2F& accel, float dt)
{
	// Store data at start of frame, to be used if ball collides with other object
	oldPos = circle.GetCenter();
	oldVel = vel;

	//std::cout << "Old Pos: " << oldPos.x << "," << oldPos.y << std::endl;
	//std::cout << "Old Vel: " << oldVel.x << "," << oldVel.y << std::endl;

	Vec2F offset = Vec2F::Mul(oldVel, dt);
	Pos2F pos = Pos2F::Add(oldPos, offset);
	circle.SetPos(pos);
	vel.Add(Vec2F::Mul(accel, dt));
	//std::cout << "New Pos: " << pos.x << "," << pos.y << std::endl;
	//std::cout << "New Vel: " << vel.x << "," << vel.y << std::endl;
}

void BallObj::Rewind(float dt)
{
	assert(dt <= 0);
	oldPos = circle.GetCenter();
	Pos2F pos = Pos2F::Add(oldPos, Vec2F::Mul(oldVel, dt));
	circle.SetPos(pos);
}

void BallObj::UpdateFromCollision(const Vec2F& newVel, const Pos2F& newCenterPos)
{
	vel = newVel;
	circle.SetPos(newCenterPos);
}
