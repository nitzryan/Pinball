#include "Camera.h"

Camera::Camera(float zoom, float aspect, const Pos2F& topLeftBounds, const Pos2F& botRightBounds, const Pos2F& startPos) :
	pos(startPos), topLeft(topLeftBounds), botRight(botRightBounds),
	zoom(zoom), aspect(aspect)
{
	ClampPosition();
}

void Camera::SetPosition(const Pos2F& p)
{
	pos = p;
	ClampPosition();
}

void Camera::IncreaseZoom(float inc)
{
	zoom += inc;
	if (zoom < 0.01)
		zoom = 0.01;
	else if (zoom > 10)
		zoom = 10;
}

void Camera::Translate(const Vec2F& vel, float dt)
{
	auto displacement = Vec2F::Mul(vel, dt);
	pos.x += displacement.x;
	pos.y += displacement.y;
	
	ClampPosition();
}

void Camera::ClampPosition()
{
	if (pos.x < botRight.x)
		pos.x = botRight.x;
	else if (pos.x > topLeft.x)
		pos.x = topLeft.x;

	if (pos.y < botRight.y)
		pos.y = botRight.y;
	else if (pos.y > topLeft.y)
		pos.y = topLeft.y;
}
