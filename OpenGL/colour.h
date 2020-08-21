#include <math.h>
#define red Colour(1,0,0)
#define green Colour(0,1,0)
#define blue Colour(0,0,1)
class Colour
{
public:
	float r;
	float g;
	float b;
	Colour(float rP, float gP, float bP) {
		r = fmin(1, rP);
		g = fmin(1, gP);
		b = fmin(1, bP);
	}
	Colour() = default;
};