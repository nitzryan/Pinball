#pragma once
#include "CircleObj.h"
#include <string>

class Teleport :
    public CircleObj
{
public:
    Teleport(const Pos2F& center, float radius, std::string newScene);
    std::string GetTeleport() const { return scene; }
private:
    std::string scene;
};

