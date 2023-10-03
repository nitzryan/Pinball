#ifndef AXISBOX2F
#define AXISBOX2F

#include "Pos2F.h"
#include "Line2F.h"
#include "Vec2F.h"

class AxisBox2F {
    public:
        AxisBox2F(const Pos2F& pur, const Pos2F& pll) :
            ur(pur), ll(pll) {

            }

        AxisBox2F(float left, float right, float top, float bot):
            ur(right,top), ll(left,bot) {

            }

        Pos2F GetUR() const {return ur;}
        Pos2F GetLL() const {return ll;}
        float GetLeft() const {return ll.x;}
        float GetRight() const {return ur.x;}
        float GetTop() const {return ur.y;}
        float GetBot() const {return ll.y;}
        void SetRight(float r) { ur.x = r; }
        void SetLeft(float l) { ll.x = l; }
        void SetTop(float t) { ur.y = t; }
        void SetBot(float b) { ll.y = b; }
    private:
        Pos2F ur, ll;
        
};

#endif