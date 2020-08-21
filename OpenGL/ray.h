#include <vector>
#include <algorithm>
#include "obstacle.h"
#include <math.h>

using namespace std;

class HitInfo
{
public:
	GLfloat dist;
	Obstacle hit;
	HitInfo(GLfloat distP, Obstacle hitP)
	{
		dist = distP;
		hit = hitP;
	}
	HitInfo() = default;
};

class Ray
{
public:
	Coordinate point;
	GLfloat angle;
	Ray(Coordinate pointP, GLfloat angleP)
	{
		point = pointP;
		angle = angleP;
	}
	Ray() = default;

	HitInfo CastToObs(std::vector<Obstacle> obstacles)
	{
		std::vector<GLfloat> dists;
		Obstacle hitOb;
		GLfloat rayGrad = (GLfloat)tan(angle * (PI / 180));
		GLfloat rayYIntercept = point.y - rayGrad * point.x;
		std::vector<Obstacle>::iterator ob;
		vector<Coordinate> inters;
		for (ob = obstacles.begin(); ob != obstacles.end(); ++ob)
		{
			Obstacle so = Obstacle(Coordinate(ob->p1.x, ob->p1.y), Coordinate(ob->p2.x, ob->p2.y));
			if (so.p1.x == so.p2.x)
			{
				so.p2.x += .001f;
			}
			if (so.p1.y == so.p2.y)
			{
				so.p2.y += .001f;
			}
			GLfloat obGrad;
			if ((so.p2.y - so.p1.y) / (so.p2.x - so.p1.x) != NULL)
			{
				obGrad = (so.p2.y - so.p1.y) / (so.p2.x - so.p1.x);
			}
			else
			{
				obGrad = FLT_MAX;
			}
			GLfloat obYIntercept = so.GetC();
			if (obGrad != rayGrad)
			{
				Coordinate interPoint = IntersectionPoint(rayGrad, rayYIntercept, obGrad, obYIntercept);
				if ((int)angle< 90 && (int)angle > -90)
				{
					if (interPoint.x > point.x)
					{
						if (interPoint.x < max(so.p1.x, so.p2.x) && interPoint.x > min(so.p1.x, so.p2.x))
						{
							dists.push_back(Dist(point, interPoint));
							inters.push_back(interPoint);

						}
					}
				}
				else
				{
					if (interPoint.x < point.x)
					{
						if (interPoint.x < max(so.p1.x, so.p2.x) && interPoint.x > min(so.p1.x, so.p2.x))
						{
							dists.push_back(Dist(point, interPoint));
							inters.push_back(interPoint);
						}
					}
				}
			}
		}
		sort(inters.begin(), inters.end(),
			[&](const Coordinate& a, const Coordinate& b)
			{
				return (Dist(point, a) < Dist(point, b));
			}
		);
		sort(dists.begin(), dists.end());
		hitOb = Obstacle();
		bool found = false;
		if (dists.size() != 0)
		{
			for (int i = 0; i < obstacles.size(); i++)
			{
				if (inters[0].y / (obstacles[i].GetM() * inters[0].x + obstacles[i].GetC()) - 1 < 0.07 && !found && inters[0].x < max(obstacles[i].p1.x, obstacles[i].p2.x) && inters[0].x > min(obstacles[i].p1.x, obstacles[i].p2.x))
				{
					hitOb = obstacles[i];
					found = true;
				}
			}
			return HitInfo(dists[0], hitOb);
		}
		return HitInfo(FLT_MAX, Obstacle(Coordinate(FLT_MAX, FLT_MAX), Coordinate(FLT_MAX, FLT_MAX), Colour(1, 1, 1)));
	}
	bool SortByDist(Coordinate& A, Coordinate& B)
	{
		return (Dist(point, A) < Dist(point, B));
	}
	GLfloat Dist(Coordinate p1, Coordinate p2)
	{
		return (GLfloat)sqrt(pow(p2.y - p1.y, 2) + pow(p2.x - p1.x, 2));
	}
};

vector<Obstacle> BSP(vector<Obstacle> obIn, Coordinate relativePoint)
{
	vector<Obstacle> obOut = splitIntersectedObs(obIn);
	for (int i = 0; i < obIn.size(); i++)
	{
		if (obIn[i].p1.x == obIn[i].p2.x)
		{
			obIn[i].p2.x += 0.000001;
		}
	}

	bool swaps = true;
	while (swaps) // while a() swap(+s) is(are) still (being) made
	{
		swaps = false;
		for (int i = 0; i < obOut.size() - 1; i++)
		{
			if (relativePoint.y < obOut[i].GetM() * relativePoint.x + obOut[i].GetC()) // player is relatavely 'below' subject obstacle
			{
				if (obOut[i + 1].p1.y < obOut[i].GetM() * obOut[i + 1].p1.x + obOut[i].GetC()) // obstacle contained within the player & subject obstacle boundary (only need to check x as obs are split if intersecting)
				{
					// obstacle goes before subject
					Obstacle temp = obOut[i];
					obOut[i] = obOut[i + 1];
					obOut[i + 1] = temp;
					swaps = true;
				}
			}
			else if (relativePoint.y > obOut[i].GetM() * relativePoint.x + obOut[i].GetC()) // player is relatively 'above' subject obstacle
			{
				if (obOut[i + 1].p1.y > obOut[i].GetM() * obOut[i + 1].p1.x + obOut[i].GetC()) // obstacle contained within the player & subject obstacle boundary
				{
					// obstacle goes before subject
					Obstacle temp = obOut[i];
					obOut[i] = obOut[i + 1];
					obOut[i + 1] = temp;
					swaps = true;
				}
			}
			else // player is on obstacle line
			{

			}
		}
	}

	return obOut;
}

vector<Obstacle> BinarySpacePartioningOld(vector<Obstacle> obIn, Coordinate relativePoint, float angle, float fov) //not properly functional
{
	obIn = splitIntersectedObs(obIn);

	for (int i = 0; i < obIn.size(); i++)
	{
		if (obIn[i].p1.x == obIn[i].p2.x)
		{
			obIn[i].p2.x += 0.000001;
		}
	}

	for (int i = 0; i < obIn.size(); i++)
	{
		float b1m = tan((PI / 180) * (angle + fov / 2));
		float b2m = tan((PI / 180) * (angle - fov / 2));
		float b1c = relativePoint.y - b1m * relativePoint.x;
		float b2c = relativePoint.y - b2m * relativePoint.x;
		Coordinate b1inter = IntersectionPoint(b1m, b1c, obIn[i].GetM(), obIn[i].GetC());
		Coordinate b2inter = IntersectionPoint(b2m, b2c, obIn[i].GetM(), obIn[i].GetC());

		if (b1inter.x < max(obIn[i].p1.x, obIn[i].p2.x) && b1inter.x > min(obIn[i].p1.x, obIn[i].p2.x))
		{
			obIn.push_back(Obstacle(obIn[i].p1, b1inter));
			obIn[i] = Obstacle(obIn[i].p2, b1inter);
		}

		if (b2inter.x < max(obIn[i].p1.x, obIn[i].p2.x) && b2inter.x > min(obIn[i].p1.x, obIn[i].p2.x))
		{
			obIn.push_back(Obstacle(obIn[i].p1, b2inter));
			obIn[i] = Obstacle(obIn[i].p2, b2inter);
		}
	}

	bool swaps = true;
	while (swaps)
	{
		swaps = false;
		for (int i = 0; i < obIn.size() - 1; i++)
		{
			if (obIn[i].p1.x != obIn[i].p2.x)
			{
				if (relativePoint.y < obIn[i].GetM() * relativePoint.x + obIn[i].GetC())
				{
					if (obIn[i + 1].p1.y < obIn[i].GetM() * obIn[i].p1.x + obIn[i].GetC() && obIn[i + 1].p2.y < obIn[i].GetM() * obIn[i].p2.x + obIn[i].GetC())
					{
						if (obIn[i + 1].p1.y > obIn[i].GetM() * obIn[i].p1.x + relativePoint.y - obIn[i].GetM() * relativePoint.x)
						{
							if (obIn[i + 1].p2.y > obIn[i].GetM() * obIn[i].p2.x + relativePoint.y - obIn[i].GetM() * relativePoint.x)
							{
								swaps = true;
								Obstacle obTemp = obIn[i];
								obIn[i] = obIn[i + 1];
								obIn[i + 1] = obTemp;
							}
						}
					}
				}
				else
				{
					if (relativePoint.y > obIn[i].GetM() * relativePoint.x + obIn[i].GetC())
					{
						if (obIn[i + 1].p1.y > obIn[i].GetM() * obIn[i].p1.x + obIn[i].GetC() && obIn[i + 1].p2.y > obIn[i].GetM() * obIn[i].p2.x + obIn[i].GetC())
						{
							if (obIn[i + 1].p1.y > obIn[i].GetM() * obIn[i].p1.x + relativePoint.y - obIn[i].GetM() * relativePoint.x)
							{
								if (obIn[i + 1].p2.y > obIn[i].GetM() * obIn[i].p2.x + relativePoint.y - obIn[i].GetM() * relativePoint.x)
								{
									swaps = true;
									Obstacle obTemp = obIn[i];
									obIn[i] = obIn[i + 1];
									obIn[i + 1] = obTemp;
								}
							}
						}
					}
				}
			}
			else
			{
				if (relativePoint.x > obIn[i].p1.x)
				{
					if (obIn[i + 1].p1.x > obIn[i].p1.x && obIn[i + 1].p1.x < relativePoint.x)
					{
						swaps = true;
						Obstacle obTemp = obIn[i];
						obIn[i] = obIn[i + 1];
						obIn[i + 1] = obTemp;
					}
				}
				else if (obIn[i + 1].p1.x < obIn[i].p1.x && obIn[i + 1].p1.x > relativePoint.x)
				{
					swaps = true;
					Obstacle obTemp = obIn[i];
					obIn[i] = obIn[i + 1];
					obIn[i + 1] = obTemp;
				}
			}
		}
		return obIn;
	}
}