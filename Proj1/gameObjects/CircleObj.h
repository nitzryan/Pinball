#pragma once

#include "../geometry/Circle.h"
#include "../geometry/Vec2F.h"
#include "Material.h"

class CircleObj
{
public:
	CircleObj(const Pos2F& pos, float radius, const Material& material, int drawSegments = 30);

	void Render(float* vertices) const;
	void SetSegments(int s) { numSegments = s; }
	int NumTriangles() const { return numSegments; }
	Circle GetCircle() const { return circle; }
	void UpdateCircle(const Circle& c) { circle = c; needsToRerender = true; }
	void SetPos(const Pos2F& p) { circle.SetPos(p); needsToRerender = true; }
	void ApplyBallHit();
	void SetOnHit(void (*f)()) { OnHit = f; }
	float GetElasticity() const { return mat.elasticity; }
	float GetFriction() const { return mat.fricCoeff; }
	bool CollidesWith(const Circle& c, float objElast, float objFric, Vec2F& exitVel, Pos2F& newCircleCenter) const;
	void IncrementTimer(float dt);
protected:
	Circle circle;
	Material mat;
	float reactionTimer;
	bool needsToRerender;
	void (*OnHit)() = nullptr;
private:
	int numSegments;
};

