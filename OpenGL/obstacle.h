#include "coordinate.h"
#include "colour.h"
#include <vector>

using namespace std;

class Obstacle
{
public:
	Coordinate p1;
	Coordinate p2;
	Colour colour;
	std::string tag;

	Obstacle(Coordinate p1P, Coordinate p2P, Colour cP = Colour(1, 1, 1), std::string pTag = "")
	{
		p1 = p1P;
		p2 = p2P;
		colour = cP;
		tag = pTag;
	}
	Obstacle() = default;
	~Obstacle()
	{
		p1 = Coordinate(0, 0);
		p2 = Coordinate(0, 0);
		colour = Colour(0, 0, 0);
		tag = "";
	}

	GLfloat GetC()
	{
		return p1.y - ((p2.y - p1.y) / (p2.x - p1.x) * p1.x);
	}
	GLfloat GetM()
	{
		if (p1.x == p2.x)
		{
			p2.x += .001f;
		}
		if (p1.y == p2.y)
		{
			p2.y += .001f;
		}
		return (p2.y - p1.y) / (p2.x - p1.x);
	}
	bool isEqual(Obstacle obstacle) {
		bool ret = false;
		if (p1.x == obstacle.p1.x)
		{
			if (p1.y == obstacle.p1.y)
			{
				if (p2.x == obstacle.p2.x)
				{
					if (p2.y == obstacle.p2.y)
					{
						if (colour.r == obstacle.colour.r)
						{
							if (colour.g == obstacle.colour.g)
							{
								if (colour.b == obstacle.colour.b)
								{
									ret = true;
								}
							}
						}
					}
				}
			}
		}
		return ret;
	}
};

Coordinate IntersectionPoint(float m1, float c1, float m2, float c2)
{
	if (m1 == m2)
	{
		return Coordinate::Coordinate(FLT_MAX, FLT_MAX);
	}
	else
	{
		GLfloat x = (c2 - c1) / (m1 - m2); // big ree time jemma
		GLfloat y = m1 * x + c1;
		Coordinate interP = Coordinate::Coordinate(x, y);
		return interP;
	}
}

//vector<Obstacle> splitIntersectedObsOld(vector<Obstacle> obs)
//{
//	vector<Obstacle> splitObs;
//	for (int i = 0; i < obs.size(); i++)
//	{
//		if (i == obs.size() - 1)
//		{
//			splitObs.push_back(obs[i]);
//		}
//		else
//		{
//			GLfloat m1 = (obs[i].p2.y - obs[i].p1.y) / (obs[i].p2.x - obs[i].p1.x);
//			GLfloat m2 = (obs[i + 1].p2.y - obs[i + 1].p1.y) / (obs[i + 1].p2.x - obs[i + 1].p1.x);
//			GLfloat c1 = obs[i].GetC();
//			GLfloat c2 = obs[i + 1].GetC();
//			if (m1 != m2)
//			{
//				Coordinate interPoint = IntersectionPoint(m1, c1, m2, c2);
//				if (interPoint.x < max(obs[i].p1.x, obs[i].p2.x) && interPoint.x > min(obs[i].p1.x, obs[i].p2.x))
//				{
//					if (interPoint.x < max(obs[i + 1].p1.x, obs[i + 1].p2.x) && interPoint.x > min(obs[i + 1].p1.x, obs[i + 1].p2.x))
//					{
//						splitObs.push_back(Obstacle(obs[i].p1, interPoint));
//						splitObs.push_back(Obstacle(interPoint, obs[i].p2));
//						splitObs.push_back(Obstacle(obs[i + 1].p1, interPoint));
//						splitObs.push_back(Obstacle(interPoint, obs[i + 1].p2));
//					}
//					else
//					{
//						splitObs.push_back(obs[i]);
//					}
//				}
//				else
//				{
//					splitObs.push_back(obs[i]);
//				}
//			}
//			else
//			{
//				splitObs.push_back(obs[i]);
//			}
//		}
//	}
//	if (splitObs.size() == obs.size())
//	{
//		return splitObs;
//	}
//	return splitIntersectedObs(splitObs);
//}

vector<Obstacle> splitIntersectedObs(vector<Obstacle> obs)
{
	vector<Obstacle> splitObs = obs;
	for (int i = 0; i < obs.size(); i++)
	{
		for (int j = i; j < obs.size(); j++) //initialise j as i as that previous value of i for j has already been checked against (change from j = 0 to j = i for optimisation and stops addind more than necessary obs)
		{
			//if obs[i] and obs[j] intersect, splitem
			GLfloat m1 = obs[i].GetM();
			GLfloat m2 = obs[j].GetM();
			GLfloat c1 = obs[i].GetC();
			GLfloat c2 = obs[j].GetC();

			if (m1 != m2)
			{
				Coordinate interPoint = IntersectionPoint(m1, c1, m2, c2);
				if (interPoint.x < max(obs[i].p1.x, obs[i].p2.x) && interPoint.x > min(obs[i].p1.x, obs[i].p2.x))
				{
					if (interPoint.x < max(obs[i + 1].p1.x, obs[i + 1].p2.x) && interPoint.x > min(obs[i + 1].p1.x, obs[i + 1].p2.x))
					{
						splitObs.push_back(Obstacle(obs[i].p1, interPoint));
						splitObs.push_back(Obstacle(interPoint, obs[i].p2));
						splitObs.push_back(Obstacle(obs[j].p1, interPoint));
						splitObs.push_back(Obstacle(interPoint, obs[j].p2));
					}
					else
					{
						splitObs.push_back(obs[i]);
					}
				}
				else
				{
					splitObs.push_back(obs[i]);
				}
			}
			else
			{
				splitObs.push_back(obs[i]);
			}
		}
	}
	return splitObs;
}

bool operator==(Obstacle lhs, Obstacle rhs)
{
	return lhs.isEqual(rhs);
}
