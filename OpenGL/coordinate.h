#include <GLFW/glfw3.h>
#define PI 3.14159265358979323846

class Coordinate
{
public:
	GLfloat x;
	GLfloat y;
	Coordinate(GLfloat xP, GLfloat yP)
	{
		x = xP;
		y = yP;
	}
	GLfloat Dist(Coordinate a, Coordinate b)
	{
		return sqrt(pow(a.y - b.y, 2) + pow(a.x - b.x, 2));
	}
	Coordinate() = default;

	float RelativeCoordToScreenCoord(Coordinate c, Coordinate p, float rotation, int ScreenWidth, float fov)
	{
		return ScreenWidth * (((rotation + (fov / 2)) - (180 / PI) * atan((c.y - p.y) / (c.x - p.x))) / (fov));
	}
};
bool operator!=(Coordinate lhs, Coordinate rhs)
{
	if (lhs.x == rhs.x && lhs.y == rhs.y)
	{
		return true;
	}
	return false;
}