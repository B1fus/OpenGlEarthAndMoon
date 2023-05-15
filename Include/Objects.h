#pragma once
#define M_PI 3.14159

#include "../Libs/glew-2.1.0/include/GL/glew.h"
#include <string>
#include <vector>
#include "../Libs/glm-0.9.9.8/glm/glm.hpp"
#include "../Libs/glm-0.9.9.8/glm/gtc/matrix_transform.hpp"
#include "../Libs/glm-0.9.9.8/glm/gtc/type_ptr.hpp"
#include "../Libs/glm-0.9.9.8/glm/gtc/quaternion.hpp"
#include "../Libs/glm-0.9.9.8/glm/gtx/quaternion.hpp"
#include "../Libs/glm-0.9.9.8/glm/common.hpp"

class Object {
protected:
	unsigned int VAO, EBO;
	unsigned int VBOVert, VBOTex, VBONorm;
	std::vector<float> vertices, texCoords, normals;
    std::vector<int> indices;
	GLuint texture;
    glm::vec3 pos, rot, scale; //position, rotation, scale
	glm::mat4 transMat{0.1f};

	GLint shaderProgram;
public:
	Object();
	virtual void draw() = 0;
	virtual void setTexture(std::string texturePath, int width, int height);
};

class Sphere : public Object{
private:
    float radius;
    int rings, sectors;

	void reBuildSphere();
	void updateVAO(GLint program);
	void updateTransMat();

public:
	Sphere(float radius, unsigned int rings, unsigned int sectors);
	void draw();
	void init(GLint program);
	void setPosition(glm::vec3 pos);
	void setRotation(glm::vec3 rot);
	void setScale(glm::vec3 scale);
	void setRadius(float radius);
};

class Circle : public Object{
private:
    float radius;
    int sectors;

	void reBuildCircle();
	void updateVAO(GLint program);
	void updateTransMat();

public:
	Circle(float radius, unsigned int sectors);
	void draw();
	void init(GLint program);
	void setPosition(glm::vec3 pos);
	void setRotation(glm::vec3 rot);
	void setScale(glm::vec3 scale);
	void setRadius(float radius);
};