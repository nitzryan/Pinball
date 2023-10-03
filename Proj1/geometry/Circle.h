#ifndef CIRCLE
#define CIRCLE

#include "Pos2F.h"

class Circle {
    public:
        Circle(float r, const Pos2F& p) : pos(p) {
            radius = r;
        }

        float GetCircumference() const {
            return (2 * 3.141592653589) * radius;
        }

        float GetRadius() const {return radius;}
        Pos2F GetCenter() const {return pos;}
        void SetPos(const Pos2F& p) { pos.x = p.x; pos.y = p.y; }
    private:
        float radius;
        Pos2F pos;  
};

#endif