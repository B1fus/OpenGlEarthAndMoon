#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include "../Libs/glew-2.1.0/include/GL/glew.h"
#include "../Libs/freeglut/include/GL/freeglut.h"
#include "../Libs/glfw-3.3.8.bin.WIN64/include/GLFW/glfw3.h"
#include "../Libs/glm-0.9.9.8/glm/glm.hpp"
#include "../Libs/glm-0.9.9.8/glm/gtc/matrix_transform.hpp"
#include "../Libs/glm-0.9.9.8/glm/gtc/type_ptr.hpp"
#include "../Include/Globals.h"
#include "../Include/Control.h"
#include "../Include/Shader.h"
#include "../Include/Objects.h"

double distance = -3, radius = 2;
double angleRotMoonFi = 0, angleRotMoonTheta = 0, angleRotMoonAlpha = 10, speedRotMoon = 1;
double speedRotEarth = 1, angleRotEarth = 0;

Shader* shader;

Sphere earth(1, 30, 30);
Sphere moon(0.5, 20, 20);
Circle circle(1, 48);

glm::mat4 projectionMat;

int main_window;
MouseState mouseState;

void display();
void reshape(int x, int y);
void idle(int);


//wrapper for subwindows
class SubWindow {
protected:
	int mainWindow;
	int thisWindow;
	float widthCoef;
	float heightCoef;
	float xCoef;
	float yCoef;
public:
	//create window with dynamic size
	SubWindow(int mainWindow, float widthCoef, float heightCoef, float xCoef, float yCoef) :
		mainWindow(mainWindow), widthCoef(widthCoef), heightCoef(heightCoef), xCoef(xCoef), yCoef(yCoef) {
		glutSetWindow(mainWindow);
		thisWindow = glutCreateSubWindow(mainWindow, glutGet(GLUT_WINDOW_WIDTH) * xCoef, glutGet(GLUT_WINDOW_HEIGHT) * yCoef, glutGet(GLUT_WINDOW_WIDTH) * widthCoef, glutGet(GLUT_WINDOW_HEIGHT) * heightCoef);
		glutDisplayFunc(::display);
		glutReshapeFunc(::reshape);
	}

	SubWindow() {};

	int getThisWindow() const { return thisWindow; }

	virtual void display() {};
	virtual void reshape(int x, int y) {};
};

//class where will draw scene
class CanvasWindow : public SubWindow {
public:
	CanvasWindow(int mainWindow, float widthCoef, float heightCoef, float xCoef, float yCoef)
		: SubWindow(mainWindow, widthCoef, heightCoef, xCoef, yCoef) {
	}

	CanvasWindow() {};

	void display() override {
		glutSetWindow(thisWindow);
		//auto a = glGetError();

		// clear the drawing buffer.
		glClearColor(0.04, 0.04, 0.04, 1);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		earth.setRotation(glm::vec3{ 0,angleRotEarth,0 });
		earth.setPosition({ 0,0,distance });
		earth.draw();


		glm::vec4 coordMoon(0, 0, 0, 0);
		coordMoon.x = cos(glm::radians(angleRotMoonFi));
		coordMoon.z = sin(glm::radians(angleRotMoonFi));

		auto matMoon = glm::identity<glm::mat4>();
		matMoon = glm::rotate(matMoon, (float)glm::radians(angleRotMoonAlpha), glm::vec3(1, 0, 0));
		matMoon = glm::rotate(matMoon, (float)glm::radians(angleRotMoonTheta), glm::vec3(0, 0, 1));
		matMoon = glm::scale(matMoon, { radius,radius,radius });
		coordMoon = matMoon * coordMoon;

		moon.setPosition({ coordMoon.x, coordMoon.y, distance + coordMoon.z });
		moon.setRotation(glm::vec3{ 0,radius,0 });
		moon.draw();

		circle.setPosition({ 0,0,distance });
		circle.setRotation({ 90+ angleRotMoonAlpha,angleRotMoonTheta,0 });
		circle.setScale({ radius,radius,radius });
		circle.draw();

		angleRotMoonFi += speedRotMoon/radius;
		angleRotMoonFi = angleRotMoonFi > 360 ? 0 : angleRotMoonFi;
		angleRotEarth += speedRotEarth;
		angleRotEarth = speedRotEarth > 360 ? 0 : angleRotEarth;
	}
	void reshape(int x, int y) override {
		if (y == 0 || x == 0) return;
		glutSetWindow(thisWindow);
		glutPositionWindow(x * xCoef, y * yCoef);

		x *= widthCoef;
		y *= heightCoef;

		glutReshapeWindow(x, y);

		projectionMat = glm::perspective<float>(glm::radians(100.), x / (float)y, 0.1, 100);
		glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(projectionMat));

		glViewport(0, 0, x, y);
	}
};



class MenuWindow : public SubWindow {
private:
	Controls controls; //all buttons
	std::list<TextBox*> textBoxes;

	int baseWidth, baseHeight;
	static void plusOne(GLdouble& a) { a += 1; }
	static void minusOne(GLdouble& a) { a -= 1; }
	void createButtonConnectedVar(int x, int y, std::string label, double* var) {
		ButtonControl* button;
		TextBox* textBox;

		button = new ButtonControl(x, y, 20, 20, "-");
		button->onClick(minusOne, var);
		controls.addControl(button);

		button = new ButtonControl(x + 30, y, 20, 20, "+");
		button->onClick(plusOne, var);
		controls.addControl(button);

		textBox = new TextBoxDouble(x + 60, y + 18, label, var);
		textBoxes.push_back(textBox);
	}
public:
	MenuWindow(int mainWindow, float widthCoef, float heightCoef, float xCoef, float yCoef)
		: SubWindow(mainWindow, widthCoef, heightCoef, xCoef, yCoef) {
		controls = Controls();

		int i = 1;
		createButtonConnectedVar(30, 30 * i++, "Radius %.2f", &radius);
		createButtonConnectedVar(30, 30 * i++, "Distance %.2f", &distance);
		createButtonConnectedVar(30, 30 * i++, "Angle 1 Moon %.2f", &angleRotMoonTheta);
		createButtonConnectedVar(30, 30 * i++, "Angle 2 Moon %.2f", &angleRotMoonAlpha);
		createButtonConnectedVar(30, 30 * i++, "Speed Earth %.2f", &speedRotEarth);
		createButtonConnectedVar(30, 30 * i++, "Speed Moon %.2f", &speedRotMoon);

		//MouseState::addControls(&this->controls);
		glutMouseFunc(mouseState.setMouseState);

		glutSetWindow(mainWindow);
		baseWidth = widthCoef * glutGet(GLUT_WINDOW_WIDTH);
		baseHeight = heightCoef * glutGet(GLUT_WINDOW_HEIGHT);
	}

	MenuWindow() {};

	MenuWindow(MenuWindow& menu) { this->controls = menu.controls, baseHeight = menu.baseHeight, baseWidth = menu.baseWidth; };

	Controls* getControls() { return &controls; }

	void display() override {
		glutSetWindow(thisWindow);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, baseWidth, baseHeight, 0);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		// clear the drawing buffer.
		glClearColor(0.2, 0.2, 0.3, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glTranslatef(0, baseHeight - glutGet(GLUT_WINDOW_HEIGHT), 0);

		controls.drawControls();

		for (auto i : textBoxes) {
			i->drawText();
		}


	}
	void reshape(int x, int y) override {
		if (y == 0 || x == 0) return;
		glutSetWindow(thisWindow);

		glutReshapeWindow(x * widthCoef, y * heightCoef);
		glutPositionWindow(x * xCoef, y * yCoef);

		//glViewport(0, 0, x, y);
	}
};

CanvasWindow canvas;
MenuWindow menu;

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	main_window = glutCreateWindow("Planet");

	glewInit();

	//glutSetOption(GLUT_RENDERING_CONTEXT, GLUT_CREATE_NEW_CONTEXT);
	canvas = CanvasWindow(main_window, 0.7, 1, 0, 0);
	shader = new Shader("../Shaders/shader.vert", "../Shaders/shader.frag");
	shader->use();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	earth.init(shader->getProgramID());
	moon.init(shader->getProgramID());
	circle.init(shader->getProgramID());
	circle.setPosition({ 0,0,-30 });

	earth.setTexture("../Images/Earth.bmp", 2048, 1024);
	moon.setTexture("../Images/Moon.bmp", 1024, 512);


	menu = MenuWindow(main_window, 0.3, 1, 0.7, 0);

	MouseState::addControls(menu.getControls());

	glutSetWindow(main_window);
	glutDisplayFunc(display);
	//glutIdleFunc(idle);
	glutTimerFunc(1000.0 / FPS, idle, 0);
	glutReshapeFunc(reshape);

	glutMainLoop();
	return 0;
}

void display()
{
	canvas.display();
	menu.display();

	glFlush();
}

void reshape(int x, int y)
{
	if (glutGetWindow() != main_window) return;
	canvas.reshape(x, y);
	menu.reshape(x, y);
}

void idle(int a) {
	glutPostRedisplay();
	glutTimerFunc(1000.0 / FPS, idle, 0);
}