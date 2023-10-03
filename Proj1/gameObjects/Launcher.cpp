#include "Launcher.h"

const float TRAVEL_DOWN_SPEED = -3.f;

Launcher::Launcher(std::vector<Pos2F> points, std::vector<Circle> circles, const Material& mat, float travel, float mv) :
	PolygonObj(points, circles, mat)
{
	startHeight = points.front().y;
	currentHeight = 0;
	maxTravel = travel;
	maxVel = mv;
	currentVel = 0;
}

bool Launcher::CollidesWithLauncher(const Circle& c, const Pos2F& oldCenter, float objElast, float objFric, Vec2F& ballVel, Pos2F& newCircleCenter) const
{
	auto result = PolygonObj::CollidesWith(c, oldCenter, objElast, objFric, ballVel, newCircleCenter);
	if (!result)
		return false;

	if (currentVel > 0 && ballVel.y < currentVel) {
		ballVel.y = currentVel;
	}

	return true;
}

void Launcher::Update(bool pressed, float dt)
{
	if (pressed) {
		currentVel = TRAVEL_DOWN_SPEED;
	}
	else {
		currentVel = maxVel * (currentHeight / -maxTravel);
	}

	currentHeight += currentVel * dt;

	if (currentHeight <= -maxTravel) {
		currentHeight = -maxTravel;
		currentVel = 0;
	}
	else if (currentHeight >= 0) {
		currentHeight = 0;
		currentVel = 0;
	}
	
	edges[0].SetP1Y(startHeight + currentHeight);
	edges[0].SetP2Y(startHeight + currentHeight);
	edges[1].SetP1Y(startHeight + currentHeight);
	edges[1].SetP2Y(startHeight + currentHeight - 0.25);
	edges[2].SetP1Y(startHeight + currentHeight - 0.25);
	edges[2].SetP2Y(startHeight + currentHeight - 0.25);
	edges[3].SetP1Y(startHeight + currentHeight - 0.25);
	edges[3].SetP2Y(startHeight + currentHeight);
	center.y = startHeight + currentHeight - 0.125;
	
	boundingBox = PolygonObj::GenerateBoundingBox({ edges[0].GetP1(), edges[1].GetP1(), edges[2].GetP1(), edges[3].GetP1() }, {});
}
