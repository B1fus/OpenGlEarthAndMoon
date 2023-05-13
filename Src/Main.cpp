#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
#include <functional>
#include <string>
#include <errno.h>
#include "../Include/Globals.h"
#include "../Include/Control.h"

GLfloat xRotated, yRotated, zRotated;
GLdouble radius = 1;

//const char* vertshader =
//"#version 450 core                                                  \n"
//"layout(location = 0) in vec3 vertexPosition_modelspace;            \n"
////"uniform mat4 modelmatrix;                                          \n"
//"void main(){                                                       \n"
////"  gl_Position = modelmatrix * vec4(vertexPosition_modelspace, 1.0);\n"
//"  gl_Position = vec4(vertexPosition_modelspace, 1.0);\n"
//"}";
////const char* fragshader =
//"#version 450 core   \n"
//"out vec3 color;     \n"
//"uniform vec3 ucolor = vec3(.5,.5,.5);\n"
//"void main() {       \n"
//"  color = ucolor;   \n"
//"}";

const char* vertshader =
"#version 330 core \n"
"layout(location = 0) in vec3 aPos; \n"
"layout(location = 1) in vec3 aColor; \n"
"layout(location = 2) in vec2 aTexCoord; \n"
"out vec3 ourColor; \n"
"out vec2 TexCoord; \n"
"void main(){\n"
"	gl_Position = vec4(aPos, 1.0); \n"
"	ourColor = aColor; \n"
"	TexCoord = aTexCoord; \n"
"}";

const char* fragshader =
"#version 330 core\n"
"out vec4 FragColor; \n"
"in vec3 ourColor; \n"
"in vec2 TexCoord; \n"
"uniform sampler2D ourTexture; \n"
"void main(){\n"
"	FragColor = texture(ourTexture, TexCoord); \n"
"}\n";


int main_window;
MouseState mouseState;


void display();
void reshape(int x, int y);
void idle(int);
GLuint loadTexture(const char* filename);
static GLuint LoadShader(const char* vertexshader, const char* fragmentshader);

void drawBox(GLfloat size, GLenum type)
{
	static GLfloat n[6][3] =
	{
	  {-1.0, 0.0, 0.0},
	  {0.0, 1.0, 0.0},
	  {1.0, 0.0, 0.0},
	  {0.0, -1.0, 0.0},
	  {0.0, 0.0, 1.0},
	  {0.0, 0.0, -1.0}
	};
	static GLint faces[6][4] =
	{
	  {0, 1, 2, 3},
	  {3, 2, 6, 7},
	  {7, 6, 5, 4},
	  {4, 5, 1, 0},
	  {5, 6, 2, 1},
	  {7, 4, 0, 3}
	};
	GLfloat v[8][3];
	GLint i;

	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = size / 2;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = size / 2;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = size / 2;

	for (i = 5; i >= 0; i--) {
		glBegin(type);
		glNormal3fv(&n[i][0]);
		glVertex3fv(&v[faces[i][0]][0]);
		glVertex3fv(&v[faces[i][1]][0]);
		glVertex3fv(&v[faces[i][2]][0]);
		glVertex3fv(&v[faces[i][3]][0]);
		glEnd();
	}
}



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

class CanvasWindow : public SubWindow {
public:
	CanvasWindow(int mainWindow, float widthCoef, float heightCoef, float xCoef, float yCoef)
		: SubWindow(mainWindow, widthCoef, heightCoef, xCoef, yCoef) {
	}

	CanvasWindow() {};

	void display() override{
		glutSetWindow(thisWindow);
		glMatrixMode(GL_MODELVIEW);
		// clear the drawing buffer.
		glClearColor(0.04, 0.04, 0.04, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		// clear the identity matrix.
		glLoadIdentity();
		// traslate the draw by z = -4.0
		// Note this when you decrease z like -8.0 the drawing will looks far , or smaller.
		glTranslatef(0.0, 0.0, -5.0);
		// Red color used to draw.
		//glColor4f(1, 1, 1, 0.5);
		// changing in transformation matrix.
		// rotation about X axis
		glRotatef(xRotated, 1.0, 0.0, 0.0);
		// rotation about Y axis
		glRotatef(yRotated, 0.0, 1.0, 0.0);
		// rotation about Z axis
		glRotatef(zRotated, 0.0, 0.0, 1.0);
		// scaling transfomation 
		glScalef(1.0, 1.0, 1.0);
		
		glEnable(GL_TEXTURE_2D);

		GLuint texture = loadTexture("Images/1.bmp");

		glBindTexture(GL_TEXTURE_2D, texture);

		//glutSolidTeapot(radius);
		drawBox(radius, GL_QUADS);



		//glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
		//glEnable(GL_TEXTURE_GEN_T);
		//glBindTexture(GL_TEXTURE_2D, texture);
		//glutSolidSphere(radius, 10, 10);
		//glDisable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
		//glDisable(GL_TEXTURE_GEN_T);

		// built-in (glut library) function , draw you a sphere.
		//glutWireSphere(radius, 20, 20);
		//glutWireSphere(radius / 2, 10, 10);
		// Flush buffers to screen
		xRotated += 1;
	}
	void reshape(int x, int y) override{
		if (y == 0 || x == 0) return;
		glutSetWindow(thisWindow);
		glutPositionWindow(x * xCoef, y * yCoef);

		x *= widthCoef;
		y *= heightCoef;

		glutReshapeWindow(x, y);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(90, (GLdouble)x / (GLdouble)y, 0.1, 100);
		glMatrixMode(GL_MODELVIEW);
		glViewport(0, 0, x, y);
	}
};



class MenuWindow : public SubWindow {
private:
	Controls controls; //all buttons
	TextBoxDouble textBox;

	int baseWidth, baseHeight;
	static void plusOne(GLdouble& a) { a += 1; }
	static void minusOne(GLdouble& a) { a -= 1; }
public:
	MenuWindow(int mainWindow, float widthCoef, float heightCoef, float xCoef, float yCoef)
		: SubWindow(mainWindow, widthCoef, heightCoef, xCoef, yCoef) {
		controls = Controls();
		ButtonControl* button;

		button = new ButtonControl(30, 30, 20, 20, "-");
		button->onClick(minusOne, &radius);
		controls.addControl(button);

		button = new ButtonControl(60, 30, 20, 20, "+");
		button->onClick(plusOne,&radius);
		controls.addControl(button);

		textBox = TextBoxDouble(90, 48, "qwerty %.2f", &radius);

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

		glTranslatef(0, baseHeight-glutGet(GLUT_WINDOW_HEIGHT), 0);

		controls.drawControls();
		textBox.drawText();


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
	canvas = CanvasWindow(main_window, 0.7, 1, 0, 0);
	menu = MenuWindow(main_window, 0.3, 1, 0.7, 0);

	MouseState::addControls(menu.getControls());

	glewInit();
	LoadShader(vertshader, fragshader); 


	xRotated = yRotated = zRotated = 30.0;
	xRotated = 43;
	yRotated = FPS;

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

GLuint loadTexture(const char* filename)
{
	GLuint texture;
	int width, height;
	unsigned char* data;

	FILE* file;
	auto a = fopen_s(&file, filename, "rb");
	//std::cout<<strerror(a);
	//fopen(filename, "rb");

	if (file == NULL) return 0;
	width = 312;
	height = 124;
	data = (unsigned char*)malloc(width * height * 3);
	//int size = fseek(file,);
	fread(data, width * height * 3, 1, file);
	fclose(file);

	for (int i = 0; i < width * height; ++i)
	{
		int index = i * 3;
		unsigned char B, R;
		B = data[index];
		R = data[index + 2];

		data[index] = R;
		data[index + 2] = B;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	free(data);

	return texture;
}

static GLuint LoadShader(const char* vertexshader, const char* fragmentshader) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	glShaderSource(VertexShaderID, 1, &vertexshader, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	//if (InfoLogLength > 0) {
	//	std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
	//	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	//	printf("%s\n", &VertexShaderErrorMessage[0]);
	//}

	// Compile Fragment Shader
	//printf("Compiling shader : %s\n", fragment_file_path);
	//char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	//glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glShaderSource(FragmentShaderID, 1, &fragmentshader, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	//if (InfoLogLength > 0) {
	//	std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
	//	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	//	printf("%s\n", &FragmentShaderErrorMessage[0]);
	//}

	// Link the program
	//printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	//if (InfoLogLength > 0) {
	//	std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
	//	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	//	printf("%s\n", &ProgramErrorMessage[0]);
	//}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}