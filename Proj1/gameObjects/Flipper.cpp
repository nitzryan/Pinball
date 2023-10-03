#include "Flipper.h"
#include <cmath>
#include "../rendering/VBO_Writer.h"
#include "../geometry/CollisonDetection2D.h"

const float W_MAX = 10.0f;
const float W_ACCEL = 30.0f;
const float THETA_MAX = 3.141592 / 6;
const float THETA_MIN = -3.141592 / 6;

Flipper::Flipper(std::vector<Pos2F> points, std::vector<Circle> circles, const Material& mat, const Pos2F& rotPoint, bool rotateCW) :
	PolygonObj(points, circles, mat),
	rotPos(rotPoint),
	rotateCW(rotateCW),
	theta(THETA_MIN),
	w(0)
{

}

void Flipper::Update(bool pressed, float dt)
{
	if (pressed) {
		theta += W_MAX * dt;
		w = W_MAX;
	}
	else {
		theta -= W_MAX * dt;
		w = -W_MAX;
	}
	
	if (theta <= THETA_MIN) {
		theta = THETA_MIN;
		w = 0;
	}

	if (theta >= THETA_MAX) {
		theta = THETA_MAX;
		w = 0;
	}
		

	/*theta = 0;*/
}

bool Flipper::CollidesWithFlipper(const Circle& c, const Pos2F& oldCenter, float objElast, float objFric, Vec2F& ballVel, Pos2F& newCircleCenter) const
{
	// Create new bounding box
	std::vector<Pos2F> points;
	std::vector<Circle> circs;
	points.reserve(edges.size());
	circs.reserve(circles.size());
	for (auto& i : edges) {
		points.push_back(Transform(i.GetP1()));
	}
	for (auto& i : circles) {
		circs.emplace_back(i.GetRadius(), Transform(i.GetCenter()));
	}
	AxisBox2F currentBox = PolygonObj::GenerateBoundingBox(points, circs);

	if (!CollisionBetween(c, currentBox))
		return false;

	Pos2F firstCollisionPos = Pos2F(0, 0);
	Pos2F firstCenterPos = Pos2F(0, 0);
	Vec2F firstCollisionAxis = Vec2F(0, 0);
	float oldCenterDistance = 10000;

	Pos2F tPos = Pos2F(0, 0);
	Pos2F circColPos = Pos2F(0, 0);
	Vec2F colAxis = Vec2F(0, 0);
	for (auto& i : edges) {
		Line2F line = Line2F(Transform(i.GetP1()), Transform(i.GetP2()));
		if (CollisionBetweenPos(line, c, tPos, circColPos, colAxis)) {
			Vec2F distVec = Pos2F::SubtractBFromA(oldCenter, circColPos);
			float mag = distVec.GetMagnitude();
			if (mag < oldCenterDistance) {
				firstCollisionPos = tPos;
				firstCenterPos = circColPos;
				firstCollisionAxis = colAxis;
				oldCenterDistance = mag;
			}
		}
	}

	for (auto& i : circles) {
		Pos2F dummy = Pos2F(0, 0);
		Circle circ = Circle(i.GetRadius(), Transform(i.GetCenter()));
		if (CollisionBetweenPos(circ, c, tPos, dummy, circColPos, 1)) {
			Vec2F distVec = Pos2F::SubtractBFromA(oldCenter, circColPos);
			float mag = distVec.GetMagnitude();
			if (mag < oldCenterDistance) {
				firstCollisionPos = tPos;
				firstCenterPos = circColPos;
				firstCollisionAxis = Pos2F::SubtractBFromA(firstCollisionPos, circ.GetCenter()).GetNormalized();
				oldCenterDistance = mag;
			}
		}
	}

	if (oldCenterDistance < 10000) {
		// Calculate new Velocity
		Vec2F velProjAxis = Vec2F::Proj(ballVel, firstCollisionAxis);
		velProjAxis.Mul(-1 - objElast * material.elasticity);
		ballVel.Add(velProjAxis);
		newCircleCenter = firstCenterPos;

		// Add velocity due to flipper movement
		Vec2F rotToCol = Pos2F::SubtractBFromA(firstCollisionPos, rotPos);
		Vec2F flipperAxis = Pos2F::SubtractBFromA(rotPos, circles[0].GetCenter());
		flipperAxis.Normalize();
		float dist = Vec2F::Proj(rotToCol, flipperAxis).GetMagnitude();
		velProjAxis.Normalize();
		Vec2F v = Vec2F::Mul(velProjAxis, dist);
		v.Mul(w);

		ballVel.Add(v);

		return true;
	}

	return false;
}

void Flipper::RenderFlipper(float* vertices) const
{
	// Render Edges
	for (auto& i : edges) {
		auto p1 = Transform(i.GetP1());
		auto p2 = Transform(i.GetP2());
		VBOWriter::DrawTriangle(&vertices, Transform(center), p1, p2, material.color);
	}
	// Render Circle
	for (auto& c : circles) {
		auto p = Transform(c.GetCenter());
		VBOWriter::DrawCircle(&vertices, p, c.GetRadius(), 30, material.color);
	}
}

Pos2F Flipper::Transform(const Pos2F& p) const
{
	// Translate so that rotation point is origin
	Vec2F transToRot = Vec2F(-rotPos.x, -rotPos.y);

	// Rotate
	Vec2F pTox = Vec2F(0, 0);
	Vec2F pToy = Vec2F(0, 0);
	float cosTheta = cosf(theta);
	float sinTheta = sinf(theta);
	if (rotateCW) {
		pTox = Vec2F(cosTheta, sinTheta);
		pToy = Vec2F(-sinTheta, cosTheta);
	}
	else {
		pTox = Vec2F(cosTheta, -sinTheta);
		pToy = Vec2F(sinTheta, cosTheta);
	}

	// Translate back
	Vec2F translateBack = Vec2F(rotPos.x, rotPos.y);

	// Apply Transforms
	Pos2F pos = p;
	pos.Add(transToRot);
	float x = pos.x * pTox.x + pos.y * pTox.y;
	float y = pos.x * pToy.x + pos.y * pToy.y;
	pos.x = x;
	pos.y = y;
	pos.Add(translateBack);
	return pos;
}
