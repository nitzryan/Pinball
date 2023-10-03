#pragma once

#include "Material.h"
#include "../geometry/Line2F.h"
#include "../geometry/Pos2F.h"
#include "../geometry/Circle.h"
#include "../geometry/AxisBox2F.h"
#include <vector>

class PolygonObj
{
public:
	PolygonObj(std::vector<Pos2F> points, std::vector<Circle> circles, const Material& mat);
	void Render(float* vertices) const;
	int NumTriangles() const;
	bool CollidesWith(const Circle& c, const Pos2F& oldCenter, float objElast, float objFric, Vec2F& entryExitVel, Pos2F& newCircleCenter) const;

protected:
	bool needsToRerender = true;
	std::vector<Line2F> edges;
	std::vector<Circle> circles;
	Pos2F center;
	AxisBox2F boundingBox;
	Material material;

	static AxisBox2F GenerateBoundingBox(const std::vector<Pos2F>& points, const std::vector<Circle>& circles);
	static Pos2F GenerateCenter(const std::vector<Pos2F>& points);
};

