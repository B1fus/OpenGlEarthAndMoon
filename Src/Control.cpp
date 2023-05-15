#include "../Include/Control.h"

int MouseState::x = 0;
int MouseState::y = 0;
bool MouseState::leftButton = 0;
bool MouseState::rightButton = 0;
bool MouseState::middleButton = 0;
std::list<Controls*> MouseState::allControls = {};

void Controls::addControl(Control* control)
{
	controls.push_back(control);
}

void Controls::updateControls()
{
	for (auto i : controls) {
		i->updateControl();
	}
}

void Controls::drawControls()
{
	for (auto i : controls) {
		i->drawControl();
	}
}

Control::Control(int x, int y, int width, int height):
	x(x),y(y),width(width),height(height) {}

ButtonControl::ButtonControl(int x, int y, int width, int height, std::string label):
	Control(x,y,width,height)
{
	//funcOnClick* = std::bind([](GLdouble* a) {*a += 1; },0);
	this->label = label;
	active = 0;
	prevActive = 0;
}

void ButtonControl::updateControl()
{
	if (MouseState::x < x || MouseState::x >= x + width || MouseState::y < y || MouseState::y >= y + height)
		return;
	prevActive = active;
	if (MouseState::leftButton) {
		active = 1;
	}
	else {
		active = 0;
	}
	if (active == 0 && prevActive == 1) {
		this->click();
	}
}

void ButtonControl::drawControl()
{
	glColor3f(0.4, 0.4, 0.6);
	glBegin(GL_QUADS);
	glVertex2d(x, y);
	glVertex2d(x+width, y);
	glVertex2d(x+width, y+height);
	glVertex2d(x, y+height);
	glEnd();

	glColor3f(1, 1, 1);
	int len = 0;
	for (auto i : label) {
		len += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, i);
	}
	glRasterPos2f(x+(width)/2. - len/2, y + (height) / 2. + glutBitmapHeight(GLUT_BITMAP_HELVETICA_18) / 4);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)label.c_str());
}

void ButtonControl::click() {
	func(*val);
}

//template<typename...Args, typename...Params>
//void ButtonControl::onClick(std::function<void(Args...)> fun, Params... params)
//{
//	if (active == 0 && prevActive == 1) {
//		fun(params...);
//	}
//}


void ButtonControl::onClick(void(*func)(double&), double* val)
{
	this->func = std::function<void(double&)>(func);
	this->val = val;
}

void MouseState::updateAllControls()
{
	MouseState s;
	for (auto i : allControls) {
		i->updateControls();
	}
}

void MouseState::addControls(Controls* controls)
{
	allControls.push_back(controls);
}

void MouseState::setMouseState(int button, int state, int x, int y)
{
	MouseState::x = x;
	MouseState::y = y;
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_UP) leftButton = 0;
		else leftButton = 1;
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_UP) rightButton = 0;
		else rightButton = 1;
		break;
	case GLUT_MIDDLE_BUTTON:
		if (state == GLUT_UP) rightButton = 0;
		else rightButton = 1;
		break;
	}
	MouseState::updateAllControls();
}


TextBox::TextBox(int x, int y, std::string label) :
	x(x), y(y), label(label) {}

void TextBox::drawText() {
	glColor3f(1, 1, 1);
	glRasterPos2f(x,y);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)label.c_str());
}

TextBoxDouble::TextBoxDouble(int x, int y, std::string label, double* var):TextBox(x,y,label) {
	this->var = var;
}

void TextBoxDouble::drawText() {
	glColor3f(1, 1, 1);
	glRasterPos2f(x,y);
	char* buffer = new char[label.size() + 10];
	sprintf_s(buffer, label.size() + 10, label.c_str(), *var);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)buffer);
	delete[] buffer;
}