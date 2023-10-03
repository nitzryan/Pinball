#pragma once

#include "CircleObj.h"
#include "BallObj.h"
#include <vector>
#include "../rendering/Renderer.h"
#include "PolygonObj.h"
#include "Flipper.h"
#include "Launcher.h"
#include "Teleport.h"
#include <iostream>

class GameScene
{
public:
	GameScene();
	void LoadScene(const char* filename);
	void UpdateScene(float dt, bool leftFlipper, bool rightFlipper, bool launch);
	void DrawScene(Renderer* renderer) const;

	static void IncrementScore100() { IncrementScore(100); }
	static void IncrementScore500() { IncrementScore(500); }
	static void IncrementScore(int inc);
	static void DisplayScore();
private:
	std::vector<CircleObj> staticCircles;
	std::vector<BallObj> balls;
	std::vector<PolygonObj> staticPolygons;
	std::vector<Flipper> flippers;
	std::vector<Launcher> launcher;
	std::vector<Teleport> teleports;
	Vec2F gravity = Vec2F(0, -2.5);
	bool bonusLives;
	static int score;
	static int numLives;
	std::string sceneOnDeath;

	void ProcessBallCollision(BallObj& ball);
};

