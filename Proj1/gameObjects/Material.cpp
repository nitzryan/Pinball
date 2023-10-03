#include "Material.h"

Material::Material(const ColorRGBA& color, float elasticity, float frict) :
	color(color), elasticity(elasticity), fricCoeff(frict)
{
}
