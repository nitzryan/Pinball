#pragma once

#include "ColorRGBA.h"

class Material
{
public:
	Material(const ColorRGBA& color, float elasticity, float frict);

	ColorRGBA color;
	float elasticity;
	float fricCoeff;

	static Material SILVER() { return Material(ColorRGBA::SILVER(), 1, 0.05); }
	static Material GLASS_RED() { return Material(ColorRGBA(255,0,0,0.5), 1, 0.05); }
	static Material STICKY() { return Material(ColorRGBA::GREEN(), 0.3, 0.3); }
	static Material BLACK() { return Material(ColorRGBA(0, 0, 0, 1), 0, 0); }
};

