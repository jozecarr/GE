#include <GLFW/glfw3.h>
#include <iostream>
#include <windows.h>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "player.h"

using namespace std;

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000

static void WindowSetup()
{
	glViewport((GLint)0, (GLint)0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

bool lineFixing = true;
GLfloat lineFixingTolerance = .1f;

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL", NULL, NULL);
	if (!window)
	{ 
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* General window and rendering setup */
	WindowSetup();

	ShowWindow(GetConsoleWindow(), SW_MINIMIZE);

	glEnable(GL_TEXTURE_2D);
	int img_width, img_height, img_channels;
	unsigned char* image = stbi_load(/*"C:/Users/jozef/OneDrive/Desktop/OpenGL/OpenGL/resources/file.jpg"*/"", &img_width, &img_height, &img_channels, STBI_rgb_alpha);
	GLuint texture;

	Player player = Player(Coordinate(-0.8, 4.4), -20, 100);
	vector<Enemy> enemies = { Enemy(Coordinate(10, 10), 20, 2, Colour(1,0.65,0)) };

	std::vector<Obstacle> obs;
	obs.push_back(Obstacle(Coordinate(3, 2), Coordinate(3, -3), Colour(0, 1, 0)));
	obs.push_back(Obstacle(Coordinate(3, 4), Coordinate(6, 2), Colour(1, 0, 0)));
	obs.push_back(Obstacle(Coordinate(6, 7), Coordinate(5, 5), Colour(0, 0, 1)));

	obs = BSP(obs, player.position);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		cout<<obs.size()<<endl;

		player.ClampRotation();
		player.HandleInput(window);

		//UpdateObsVectorForEnemyBodies(obs, enemies);

		GLfloat* columnBrightnesses = new GLfloat[SCREEN_WIDTH];
		GLfloat* columnHeights = new GLfloat[SCREEN_WIDTH];
		GLfloat* rs = new GLfloat[SCREEN_WIDTH];
		GLfloat* bs = new GLfloat[SCREEN_WIDTH];
		GLfloat* gs = new GLfloat[SCREEN_WIDTH];
		Colour* cs = new Colour[SCREEN_WIDTH];

		glClear(GL_COLOR_BUFFER_BIT);

		glBindTexture(GL_TEXTURE_2D, 0);

		glColor3f(0, 1, 0);

		//floor
		glBegin(GL_POLYGON);
			glColor3f(0, 1, 0);
			glVertex2f(0, 0);
			glVertex2f(SCREEN_WIDTH, 0);
			glColor3f(0, .4, 0);
			glVertex2f(SCREEN_WIDTH, SCREEN_HEIGHT / 2);
			glVertex2f(0, SCREEN_HEIGHT / 2);
		glEnd();

		//sky
		glBegin(GL_POLYGON);
			glColor3f(.6, .6, 1);
			glVertex2f(0, SCREEN_HEIGHT / 2);
			glVertex2f(SCREEN_WIDTH, SCREEN_HEIGHT / 2);
			glColor3f(.5, .5, 1);
			glVertex2f(SCREEN_WIDTH, SCREEN_HEIGHT);
			glVertex2f(0, SCREEN_HEIGHT);
		glEnd();

		/*glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);*/

		for (float i = player.rotation - (player.fov / 2), j = 0; i <= player.rotation + (player.fov / 2); i += player.fov / SCREEN_WIDTH, j++)
		{
			HitInfo hitInfo;
			Ray r = Ray(player.position, i);
			hitInfo = r.CastToObs(obs);
			columnBrightnesses[(int)j] = 50 / pow(hitInfo.dist, 2);
			columnHeights[(int)j] = 1250 / hitInfo.dist;
			rs[int(j)] = hitInfo.hit.colour.r;
			gs[int(j)] = hitInfo.hit.colour.g;
			bs[int(j)] = hitInfo.hit.colour.b;

			//cs[int(j)] = hitInfo.hit.colour;
		}

		if (lineFixing)
		{
			for (int i = 0; i < SCREEN_WIDTH; i++)
			{
				if (i < SCREEN_WIDTH - 1 && i>1)
				{
					if (columnHeights[i] < 1 && (columnHeights[i - 1] > 1 && columnHeights[i + 1] > 1))
					{
						columnHeights[i] = (columnHeights[i + 1] + columnHeights[i - 1]) / 2;
						columnBrightnesses[i] = (columnBrightnesses[i + 1] + columnBrightnesses[i - 1]) / 2;
					}
					if (columnHeights[i] / ((columnHeights[i - 1] + columnHeights[i + 1]) / 2) > (lineFixingTolerance + 1) && (columnHeights[i - 1] > 1 && columnHeights[i + 1] > 1))
					{
						columnHeights[i] = (columnHeights[i + 1] + columnHeights[i - 1]) / 2;
						columnBrightnesses[i] = (columnBrightnesses[i + 1] + columnBrightnesses[i - 1]) / 2;
					}
				}
			}
		}

		for (double i = 0; i < SCREEN_WIDTH; i++)
		{
			glColor3f(min(rs[(int)i], columnBrightnesses[(int)i] * rs[(int)i]), min(gs[(int)i], columnBrightnesses[(int)i] * gs[(int)i]), min(bs[(int)i], columnBrightnesses[(int)i] * bs[(int)i]));

			Coordinate bl = Coordinate(i / SCREEN_WIDTH, 0);
			Coordinate br = Coordinate((i + 1) / SCREEN_WIDTH, 0);
			Coordinate tr = Coordinate((i + 1) / SCREEN_WIDTH, 1);
			Coordinate tl = Coordinate(i / SCREEN_WIDTH, 1);

			glBegin(GL_POLYGON);
				glTexCoord2f(br.x, br.y);
				glVertex2f(i, (SCREEN_HEIGHT / 2) - (columnHeights[(int)i] / 2));
				glTexCoord2f(tr.x, tr.y);
				glVertex2f(i + 1, (SCREEN_HEIGHT / 2) - (columnHeights[(int)i] / 2));
				glTexCoord2f(tl.x, tl.y);
				glVertex2f(i + 1, (SCREEN_HEIGHT / 2) + (columnHeights[(int)i] / 2));
				glTexCoord2f(bl.x, bl.y);
				glVertex2f(i, (SCREEN_HEIGHT / 2) + (columnHeights[(int)i] / 2));
			glEnd();
		}

		glPopAttrib();

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}