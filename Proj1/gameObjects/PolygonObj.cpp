#include "PolygonObj.h"
#include "../rendering/VBO_Writer.h"
#include "../geometry/CollisonDetection2D.h"

PolygonObj::PolygonObj(std::vector<Pos2F> points, std::vector<Circle> circles, const Material& mat):
    boundingBox(PolygonObj::GenerateBoundingBox(points, circles)), 
    center(PolygonObj::GenerateCenter(points)),
    material(mat),
    circles(circles)
{
    edges.reserve(points.size());
    for (size_t i = 0; i < points.size() - 1; i++) {
        edges.emplace_back(points[i], points[i + 1]);
    }
    edges.emplace_back(points.back(), points.front());
}

void PolygonObj::Render(float* vertices) const
{
    // Render Edges
    for (auto& i : edges) {
        auto p1 = i.GetP1();
        auto p2 = i.GetP2();
        VBOWriter::DrawTriangle(&vertices, center, p1, p2, material.color);
    }
    // Render Circle
    for (auto& c : circles) {
        VBOWriter::DrawCircle(&vertices, c.GetCenter(), c.GetRadius(), 30, material.color);
    }
}

int PolygonObj::NumTriangles() const
{
    return edges.size() + circles.size() * 30;
}

bool PolygonObj::CollidesWith(const Circle& c, const Pos2F& oldCenter, float objElast, float objFric, Vec2F& entryExitVel, Pos2F& newCircleCenter) const
{
    if (!CollisionBetween(c, boundingBox))
        return false;
    
    Pos2F firstCollisionPos = Pos2F(0, 0);
    Pos2F firstCenterPos = Pos2F(0, 0);
    Vec2F firstCollisionAxis = Vec2F(0, 0);
    float oldCenterDistance = 10000;

    Pos2F tPos = Pos2F(0, 0);
    Pos2F circColPos = Pos2F(0, 0);
    Vec2F colAxis = Vec2F(0, 0);
    for (auto& i : edges) {
        if (CollisionBetweenPos(i, c, tPos, circColPos, colAxis)) {
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
        if (CollisionBetweenPos(i, c, tPos, dummy, circColPos, 1)) {
            Vec2F distVec = Pos2F::SubtractBFromA(oldCenter, circColPos);
            float mag = distVec.GetMagnitude();
            if (mag < oldCenterDistance) {
                firstCollisionPos = tPos;
                firstCenterPos = circColPos;
                firstCollisionAxis = Pos2F::SubtractBFromA(firstCollisionPos, i.GetCenter()).GetNormalized();
                oldCenterDistance = mag;
            }
        }
    }

    if (oldCenterDistance < 10000) {
        // Calculate new Velocity
        Vec2F velProjAxis = Vec2F::Proj(entryExitVel, firstCollisionAxis);
        velProjAxis.Mul(-1 - objElast * material.elasticity);
        entryExitVel.Add(velProjAxis);
        newCircleCenter = firstCenterPos;

        return true;
    }

    return false;
}

AxisBox2F PolygonObj::GenerateBoundingBox(const std::vector<Pos2F>& points, const std::vector<Circle>& circles)
{
    AxisBox2F box = AxisBox2F(10000, -10000, -10000, 10000);
    for (auto& i : points)
    {
        if (i.x > box.GetRight())
            box.SetRight(i.x);
        if (i.x < box.GetLeft())
            box.SetLeft(i.x);
        if (i.y > box.GetTop())
            box.SetTop(i.y);
        if (i.y < box.GetBot())
            box.SetBot(i.y);
    }

    for (auto& i : circles) {
        auto center = i.GetCenter();
        auto radius = i.GetRadius();
        if (center.x - radius < box.GetLeft())
            box.SetLeft(center.x - radius);
        if (center.x + radius > box.GetRight())
            box.SetRight(center.x + radius);
        if (center.y - radius < box.GetBot())
            box.SetBot(center.y - radius);
        if (center.y + radius > box.GetTop())
            box.SetTop(center.y + radius);
    }

    return box;
}

Pos2F PolygonObj::GenerateCenter(const std::vector<Pos2F>& points)
{
    float x = 0;
    float y = 0;
    for (auto& i : points)
    {
        x += i.x;
        y += i.y;
    }

    return Pos2F(x / points.size(), y / points.size());
}
