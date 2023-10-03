#include "Teleport.h"

Teleport::Teleport(const Pos2F& center, float radius, std::string newScene) :
	CircleObj(center, radius, Material::BLACK()),
	scene(newScene)
{
}
