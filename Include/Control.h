#pragma once
#include <list>
#include <string>
#include <iostream>
#include <functional>
#include <utility>
#include "../Libs/glew-2.1.0/include/GL/glew.h"
#include "../Libs/freeglut/include/GL/freeglut.h"
#include "../Libs/glfw-3.3.8.bin.WIN64/include/GLFW/glfw3.h"

class Controls;

static class MouseState {
private:
	static std::list<Controls*> allControls;
	static void updateAllControls();
public:
	static int x, y;

	static bool leftButton, rightButton, middleButton;
	MouseState() {};
	static void addControls(Controls* controls);
	static void setMouseState(int button, int state, int x, int y);
};

class Control {
protected:
	int width, height, x, y; //x,y are position of control
public:

	Control(int x, int y, int width, int height);
	virtual void updateControl()=0;
	virtual void drawControl()=0;
};

class Controls {
protected:
	std::list<Control*> controls;
public:
	Controls() {};
	void addControl(Control* control);
	void updateControls();
	void drawControls();
};

class ButtonControl : public Control {
private:
	bool active;
	bool prevActive; //previous frame active of button
	std::string label;
	double* val;
	//void(*func)(GLdouble&) = nullptr;
	std::function<void(double&)> func;
public:
	ButtonControl(int x, int y, int width, int height, std::string label);
	void updateControl();
	void drawControl();
	void click();

	void onClick(void(*func)(double&), double* val);
};




class TextBox {
protected:
	int x, y; //x,y are position of text
	std::string label;
public:
	TextBox(int x, int y, std::string label);
	TextBox() {};
	virtual void drawText();
};

class TextBoxDouble : public TextBox {
protected:
	double* var = nullptr;
public:
	TextBoxDouble(int x, int y, std::string label, double* var);
	TextBoxDouble() {};
	void drawText();
};