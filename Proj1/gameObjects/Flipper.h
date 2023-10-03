#pragma once
#include "PolygonObj.h"
#include "../geometry/Pos2F.h"

class Flipper :
    public PolygonObj
{
public:
    Flipper(std::vector<Pos2F> points, std::vector<Circle> circles, const Material& mat, const Pos2F& rotPoint, bool rotateCW);
    void Update(bool pressed, float dt);
    bool CollidesWithFlipper(const Circle& c, const Pos2F& oldCenter, float objElast, float objFric, Vec2F& ballVel, Pos2F& newCircleCenter) const;
    void RenderFlipper(float* vertices) const;
private:
    float theta;
    float w;
    bool rotateCW;
    Pos2F rotPos;

    Pos2F Transform(const Pos2F& p) const;
};

