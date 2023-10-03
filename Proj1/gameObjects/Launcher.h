#pragma once
#include "PolygonObj.h"


class Launcher :
    public PolygonObj
{
public:
    Launcher(std::vector<Pos2F> points, std::vector<Circle> circles, const Material& mat, float travel, float maxVel);
    bool CollidesWithLauncher(const Circle& c, const Pos2F& oldCenter, float objElast, float objFric, Vec2F& ballVel, Pos2F& newCircleCenter) const;
    void Update(bool pressed, float dt);
private:
    float startHeight;
    float currentHeight;
    float maxTravel;
    float maxVel;
    float currentVel;
};

