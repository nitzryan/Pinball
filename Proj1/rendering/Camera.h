#pragma once
#include "../geometry/Pos2F.h"

class Camera
{
public:
	Camera(float zoom, float aspect, const Pos2F& topLeftBounds, const Pos2F& botRightBounds, const Pos2F& startPos = Pos2F(0,0));

	void SetPosition(const Pos2F& pos);
	Pos2F GetPos() const { return pos; }
	Pos2F GetViewportTrans() const { return Pos2F(zoom, zoom * aspect); }
	void IncreaseZoom(float inc);
	void SetAspect(float w, float h) { aspect = w / h; }
	void Translate(const Vec2F& vel, float dt);
private:
	Pos2F pos;
	float zoom;
	float aspect;
	Pos2F topLeft;
	Pos2F botRight;

	void ClampPosition();
};

