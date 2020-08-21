#include "ray.h"

class Player
{
public:
	Coordinate position;
	GLfloat rotation;
	GLfloat fov;
	void RotateClockwise(GLfloat amount)
	{
		rotation += amount;
	}
	void RotateAntiClockwise(GLfloat amount)
	{
		rotation -= amount;
	}
	void MoveForwards(GLfloat amount)
	{
		position.x += (GLfloat)(amount * sin((90 - rotation) * (PI / 180)));
		position.y += (GLfloat)(amount * cos((90 - rotation) * (PI / 180)));
	}
	void MoveBackwards(GLfloat amount)
	{
		MoveForwards(-amount);
	}
	void MoveRight(GLfloat amount)
	{
		RotateClockwise(90);
		MoveForwards(amount);
		RotateAntiClockwise(90);
	}
	void MoveLeft(GLfloat amount)
	{
		MoveRight(-amount);
	}
	Player(Coordinate pos, GLfloat rot, GLfloat fovP)
	{
		position = pos;
		rotation = rot;
		fov = fovP;
	}
	void ClampRotation()
	{
		if (rotation > 180)
		{
			rotation = -180;
		}
		if (rotation < -180)
		{
			rotation = 180;
		}
	}
	void HandleInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, 87) == GLFW_PRESS)
		{
			MoveForwards((GLfloat)0.1);
		}
		if (glfwGetKey(window, 83) == GLFW_PRESS)
		{
			MoveForwards((GLfloat)-0.1);
		}
		if (glfwGetKey(window, 65) == GLFW_PRESS)
		{
			MoveRight((GLfloat)-0.1);
		}
		if (glfwGetKey(window, 68) == GLFW_PRESS)
		{
			MoveRight((GLfloat)0.1);
		}
		if (glfwGetKey(window, 262) == GLFW_PRESS)
		{
			RotateClockwise((GLfloat)2);
		}
		if (glfwGetKey(window, 263) == GLFW_PRESS)
		{
			RotateAntiClockwise((GLfloat)2);
		}
	}
	Player() = default;
};

class Enemy
{
public:
	Coordinate position;
	GLfloat rotation;
	GLfloat size;

	vector<Obstacle> body;

	Enemy(Coordinate positionP, GLfloat rotationP, GLfloat sizeP, Colour colour = Colour(1,1,1))
	{
		position = positionP;
		rotation = rotationP;
		size = sizeP;

		GenSides(colour);
	}

	Enemy() = default;

	void Move(Coordinate moveAmount)
	{
		position.x += moveAmount.x;
		position.y += moveAmount.y;

		GenSides();
	}

private:
	void GenSides(Colour colour = Colour(1,1,1))
	{
		Coordinate topLeftPoint = Coordinate(position.x - (size / sqrt(2)) * cos((PI / 180) * (45 - rotation)), position.y + (size / sqrt(2)) * sin((PI / 180) * (45 - rotation)));
		Coordinate topRightPoint = Coordinate(position.x + (size / sqrt(2)) * sin((PI / 180) * (45 - rotation)), position.y + (size / sqrt(2)) * cos((PI / 180) * (45 - rotation)));
		Coordinate bottomLeftPoint = Coordinate(position.x - (size / sqrt(2)) * sin((PI / 180) * (45 - rotation)), position.y - (size / sqrt(2)) * cos((PI / 180) * (45 - rotation)));
		Coordinate bottomRightPoint = Coordinate(position.x + (size / sqrt(2)) * cos((PI / 180) * (45 - rotation)), position.y - (size / sqrt(2)) * sin((PI / 180) * (45 - rotation)));

		body = {
			Obstacle(topLeftPoint, topRightPoint, colour, "enemy"),
			Obstacle(topRightPoint, bottomRightPoint, colour, "enemy"),
			Obstacle(bottomRightPoint, bottomLeftPoint, colour, "enemy"),
			Obstacle(bottomLeftPoint, topLeftPoint, colour, "enemy")
		};
	}
};

void UpdateObsVectorForEnemyBodies(vector<Obstacle>& obstacles, vector<Enemy> enemies)
{
	for (int i = 0; i < obstacles.size(); i++)
	{
		if (obstacles[i].tag == "enemy")
		{
			obstacles.erase(obstacles.begin() + i);
		}
	}
	for (int i = 0; i < enemies.size(); i++)
	{
		for (int j = 0; j < enemies[i].body.size(); j++)
		{
			obstacles.push_back(enemies[i].body[j]);
		}
	}
}