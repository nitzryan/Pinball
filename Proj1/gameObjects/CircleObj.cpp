#include "CircleObj.h"
#include "../glad/glad.h"
#include "../rendering/VBO_Writer.h"
#include "../geometry/CollisonDetection2D.h"

CircleObj::CircleObj(const Pos2F& pos, float radius, const Material& mat, int segments) :
	circle(radius, pos),
	numSegments(segments),
	mat(mat)
{
	needsToRerender = false;
	reactionTimer = 0;
}

void CircleObj::Render(float* vertices) const
{
	ColorRGBA color = mat.color;
	ColorRGBA effect = ColorRGBA(0.2 * reactionTimer, 0.2 * reactionTimer, 0.2 * reactionTimer, 1);
	color.Add(effect);
	VBOWriter::DrawCircle(&vertices, circle.GetCenter(), circle.GetRadius(), numSegments, color);
}

void CircleObj::ApplyBallHit()
{
	if (OnHit != nullptr) {
		OnHit();
		reactionTimer += 1.f;
	}
}

bool CircleObj::CollidesWith(const Circle& c, float objElast, float objFric, Vec2F& exitVel, Pos2F& newCircleCenter) const
{
	Pos2F colPoint = Pos2F(0, 0);
	Pos2F dummy = Pos2F(0, 0); 
	if (CollisionBetweenPos(circle, c, colPoint, dummy, newCircleCenter, 1)) {
		Vec2F firstCollisionAxis = Pos2F::SubtractBFromA(colPoint, circle.GetCenter()).GetNormalized();
		// Calculate new Velocity
		Vec2F velProjAxis = Vec2F::Proj(exitVel, firstCollisionAxis);
		velProjAxis.Mul(-1 - objElast * mat.elasticity);
		exitVel.Add(velProjAxis);

		return true;
	}

	return false;
}

void CircleObj::IncrementTimer(float dt)
{
	reactionTimer -= dt;
	if (reactionTimer < 0)
		reactionTimer = 0;
}
