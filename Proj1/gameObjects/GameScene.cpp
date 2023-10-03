#include "GameScene.h"
#include <algorithm> // remove_if, erase
#include "../geometry/CollisonDetection2D.h"
#include <iostream>
#include <fstream>

int GameScene::score = 0;
int GameScene::numLives = 3;

GameScene::GameScene()
{
	DisplayScore();
}

void GameScene::LoadScene(const char* filename)
{
	sceneOnDeath = filename;
	bonusLives = false;
	staticCircles.clear();
	balls.clear();
	staticPolygons.clear();
	flippers.clear();
	launcher.clear();
	teleports.clear();

	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::cout << "File " << filename << " failed to open\n";
		exit(1);
	}

	// Read Materials
	std::vector<Material> materials;
	int numMaterials;
	std::string text;
	file >> text;
	file >> numMaterials;
	materials.reserve(numMaterials);
	for (int i = 0; i < numMaterials; i++) {
		float r, g, b, a, e, u;
		file >> r;
		file >> g;
		file >> b;
		file >> a;
		file >> e;
		file >> u;
		materials.emplace_back(ColorRGBA(r, g, b, a), e, u);
	}

	// Read Balls
	file >> text;
	int numBalls;
	file >> numBalls;
	balls.reserve(numBalls);
	for (int i = 0; i < numBalls; i++) {
		float x, y, r, m;
		int mat;
		file >> x;
		file >> y;
		file >> r;
		file >> mat;
		file >> m;
		balls.emplace_back(Pos2F(x, y), r, materials[mat], m);
	}

	// Read Circles
	file >> text;
	int numCircles;
	file >> numCircles;
	staticCircles.reserve(numCircles);
	for (int i = 0; i < numCircles; i++) {
		float x, y, r;
		int mat;
		file >> x;
		file >> y;
		file >> r;
		file >> mat;
		staticCircles.emplace_back(Pos2F(x, y), r, materials[mat]);
	}

	// Read points
	file >> text;
	int numPoints;
	file >> numPoints;
	std::vector<Pos2F> points;
	points.reserve(numPoints);
	for (int i = 0; i < numPoints; i++) {
		float x, y;
		file >> x;
		file >> y;
		points.emplace_back(x, y);
	}

	// Read polygons
	file >> text;
	int numPolygons;
	file >> numPolygons;
	staticPolygons.reserve(numPolygons);
	for (int i = 0; i < numPolygons; i++) {
		// Points
		file >> text;
		file >> numPoints;
		int id;
		std::vector<Pos2F> ps;
		for (int j = 0; j < numPoints; j++) {
			file >> id;
			ps.push_back(points[id]);
		}
		// Circles
		file >> text;
		file >> numCircles;
		std::vector<Circle> cs;
		for (int j = 0; j < numCircles; j++) {
			float x, y, r;
			file >> x;
			file >> y;
			file >> r;
			cs.emplace_back(r, Pos2F(x, y));
		}
		int mat;
		file >> mat;
		staticPolygons.emplace_back(ps, cs, materials[mat]);
	}

	for (int i = 0; i < 2; i++)
	{
		// Read flippers
	// Points
		file >> text;
		file >> text;
		file >> numPoints;
		int id;
		std::vector<Pos2F> ps;
		for (int j = 0; j < numPoints; j++) {
			file >> id;
			ps.push_back(points[id]);
		}
		// Circles
		file >> text;
		file >> numCircles;
		std::vector<Circle> cs;
		for (int j = 0; j < numCircles; j++) {
			float x, y, r;
			file >> x;
			file >> y;
			file >> r;
			cs.emplace_back(r, Pos2F(x, y));
		}
		float x, y;
		file >> x;
		file >> y;
		Pos2F rotPos(x, y);
		int mat;
		file >> mat;
		flippers.emplace_back(ps, cs, materials[mat], rotPos, i == 1);
	}

	// Launcher
	file >> text;
	int numLaunchers;
	file >> numLaunchers;
	for (int i = 0; i < numLaunchers; i++) {
		file >> text;
		file >> numPoints;
		int id;
		std::vector<Pos2F> ps;
		for (int j = 0; j < numPoints; j++) {
			file >> id;
			ps.push_back(points[id]);
		}
		file >> text;
		float maxTravel;
		file >> maxTravel;
		file >> text;
		float maxVel;
		file >> maxVel;
		int mat;
		file >> mat;
		launcher.emplace_back(ps, std::vector<Circle>{}, materials[mat], maxTravel, maxVel);
	}

	// Reactions
	file >> text;
	int numReactions;
	file >> numReactions;
	for (int i = 0; i < numReactions; i++) {
		file >> text;
		if (text == "Bumper") {
			file >> text;
			if (text == "Circle") {
				int idx;
				file >> idx;
				int score;
				file >> score;
				if (score == 100) {
					staticCircles[idx].SetOnHit(IncrementScore100);
				}
				else if (score == 500) {
					staticCircles[idx].SetOnHit(IncrementScore500);
				}
			}
		}
		else if (text == "Teleport") {
			file >> text;
			float x, y, r;
			file >> x;
			file >> y;
			file >> r;
			teleports.emplace_back(Pos2F(x, y), r, text);
		} 
		else if (text == "NoLivesLost") {
			bonusLives = true;
		}
		else if (text == "TeleportOnDeath") {
			file >> sceneOnDeath;
		}
	}

	file.close();
	
}

void GameScene::UpdateScene(float dt, bool leftFlipper, bool rightFlipper, bool launch)
{
	if (dt > 0.005) {
		UpdateScene(dt - 0.005, leftFlipper, rightFlipper, launch);
		dt = 0.005;
	}
	for (auto& i : staticCircles) {
		i.IncrementTimer(dt);
	}
	// Update Positions
	for (auto& i : balls) {
		i.Update(gravity, dt);
	}

	assert(flippers.size() == 2);
	flippers[0].Update(leftFlipper, dt);
	flippers[1].Update(rightFlipper, dt);

	if (!launcher.empty())
		launcher.front().Update(launch, dt);

	// Check for collisions
	for (auto& i : balls) {
		ProcessBallCollision(i);
	}

	auto shouldRemove = [](BallObj& b) { return b.GetCircle().GetCenter().y < -4; };
	balls.erase(std::remove_if(balls.begin(), balls.end(), shouldRemove), balls.end());

	if (balls.empty()) {
		if (!bonusLives) {
			numLives--;
			DisplayScore();
			bonusLives = true;
		}
		if (numLives >= 0) {
			LoadScene(sceneOnDeath.c_str());
		}
	}
}

void GameScene::DrawScene(Renderer* renderer) const
{
	if (renderer == nullptr)
		return;

	for (auto& i : staticCircles)
		renderer->DrawCircle(i);

	for (auto& i : staticPolygons)
		renderer->DrawPolygon(i);

	for (auto& i : launcher)
		renderer->DrawPolygon(i);

	for (auto& i : flippers)
		renderer->DrawFlipper(i);

	for (auto& i : teleports)
		renderer->DrawCircle(i);

	for (auto& i : balls)
		renderer->DrawCircle(i);

}

void GameScene::IncrementScore(int inc)
{
	score += inc;
	DisplayScore();
}

void GameScene::DisplayScore()
{
	std::cout << "\rScore: " << score << " Lives Left: ";
	for (int i = 0; i < numLives; i++) {
		std::cout << " * ";
	}
	std::cout << "         " <<  std::flush; // Spaces to ensure that lives get overwritten
	if (numLives < 0) {
		std::cout << "GAME OVER\n";
	}
}

void GameScene::ProcessBallCollision(BallObj& ball)
{
	for (auto& i : teleports) {
		if (CollisionBetween(ball.GetCircle(), i.GetCircle())) {
			LoadScene(i.GetTeleport().c_str());
			return;
		}
	}

	for (auto& i : staticCircles) {
		Vec2F vel = ball.GetVel();
		Pos2F newCircleCenter = Pos2F(0, 0);
		if (i.CollidesWith(ball.GetCircle(), ball.GetElasticity(), ball.GetFriction(), vel, newCircleCenter)) {
			i.ApplyBallHit();
			ball.UpdateFromCollision(vel, newCircleCenter);
			//ProcessBallCollision(ball);
			//return;
		}
	}

	for (auto& i : staticPolygons) {
		Vec2F vel = ball.GetVel();
		Pos2F newCircleCenter = Pos2F(0, 0);
		if (i.CollidesWith(ball.GetCircle(), ball.GetOldPos(), ball.GetElasticity(), ball.GetFriction(), vel, newCircleCenter)) {
			ball.UpdateFromCollision(vel, newCircleCenter);
			//ProcessBallCollision(ball);
			//return;
		}
	}

	for (auto& i : launcher) {
		Vec2F vel = ball.GetVel();
		Pos2F newCircleCenter = Pos2F(0, 0);
		if (i.CollidesWithLauncher(ball.GetCircle(), ball.GetOldPos(), ball.GetElasticity(), ball.GetFriction(), vel, newCircleCenter)) {
			ball.UpdateFromCollision(vel, newCircleCenter);
			//ProcessBallCollision(ball);
			//return;
		}
	}

	for (auto& i : flippers) {
		Vec2F vel = ball.GetVel();
		Pos2F newCircleCenter = Pos2F(0, 0);
		if (i.CollidesWithFlipper(ball.GetCircle(), ball.GetOldPos(), ball.GetElasticity(), ball.GetFriction(), vel, newCircleCenter)) {
			ball.UpdateFromCollision(vel, newCircleCenter);
			//ProcessBallCollision(ball);
			//return;
		}
	}

	for (auto& i : balls) {
		if (&i == &ball) // same ball
			break;

		Pos2F colPos = Pos2F(0, 0);
		Pos2F newBallCenter = Pos2F(0, 0);
		Pos2F newICenter = Pos2F(0, 0);
		float m1 = ball.GetMass();
		float m2 = i.GetMass();
		if (CollisionBetweenPos(ball.GetCircle(), i.GetCircle(), colPos, newBallCenter, newICenter, m1 / (m1 + m2))) {
			// Get vector representing axis of collision
			auto v12 = Pos2F::SubtractBFromA(newICenter, newBallCenter);
			v12.Normalize();
			// Get component of each velocity that is along the axis
			auto v1p = Vec2F::Dot(ball.GetVel(), v12);
			auto v2p = Vec2F::Dot(i.GetVel(), v12);
			// Get new velocity along axis of collision
			auto e = ball.GetElasticity() * i.GetElasticity();
			auto v1pn = (m1 * v1p + m2 * v2p - m2 * (v1p - v2p) * e) / (m1 + m2);
			auto v2pn = (m1 * v1p + m2 * v2p - m1 * (v2p - v1p) * e) / (m1 + m2);

			// Potentiall Apply Friction here
			auto newVelA = Vec2F::Mul(v12, v1pn);
			auto newVelB = Vec2F::Mul(v12, v2pn);

			ball.UpdateFromCollision(newVelA, newBallCenter);
			i.UpdateFromCollision(newVelB, newICenter);
			// Need to check for all objects since moved, stop checking here
			//ProcessBallCollision(ball);
			//ProcessBallCollision(i);
			return;
		}
	}
}

