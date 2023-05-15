#include "../Include/Objects.h"

Object::Object() {
	pos = { 0.,0,0 };
	rot = { 0.,0,0 };
	scale = { 1.,1,1 };
	texture = 0;
	VAO = EBO = 0;
	VBONorm = VBOTex = VBOVert = 0;
}

void Object::setTexture(std::string filename, int width, int height)
{
	GLuint texture;
	unsigned char* data;

	FILE* file;
	auto a = fopen_s(&file, filename.c_str(), "rb");
	//std::cout<<strerror(a);
	//fopen(filename, "rb");

	if (file == NULL) return;
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

	this->texture = texture;
}

Sphere::Sphere(float radius, unsigned int rings, unsigned int sectors) :
	Object(), radius(radius), rings(rings), sectors(sectors)
{
	reBuildSphere();
}

void Sphere::reBuildSphere() {
	vertices.clear();
	texCoords.clear();
	normals.clear();
	indices.clear();

	vertices.resize(3 * 8);
	normals.resize(3 * 4);
	texCoords.resize(2 * 4);
	indices.resize(6);

	float const R = 1. / (float)(rings - 1);
	float const S = 1. / (float)(sectors - 1);
	int r, s;

	vertices.resize(rings * sectors * 3);
	normals.resize(rings * sectors * 3);
	texCoords.resize(rings * sectors * 2);
	std::vector<GLfloat>::iterator v = vertices.begin();
	std::vector<GLfloat>::iterator n = normals.begin();
	std::vector<GLfloat>::iterator t = texCoords.begin();
	for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
		float const y = sin(-M_PI / 2 + M_PI * r * R);
		float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
		float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

		*t++ = s * S;
		*t++ = r * R;

		*v++ = x * radius;
		*v++ = y * radius;
		*v++ = z * radius;

		*n++ = x;
		*n++ = y;
		*n++ = z;
	}

	indices.resize((rings - 1) * (sectors - 1) * 4);
	std::vector<int>::iterator i = indices.begin();
	for (r = 0; r < rings - 1; r++) for (s = 0; s < sectors - 1; s++) {
		*i++ = r * sectors + s;
		*i++ = r * sectors + (s + 1);
		*i++ = (r + 1) * sectors + (s + 1);
		*i++ = (r + 1) * sectors + s;
	}
}

void Sphere::updateVAO(GLint program) {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBOVert);
	glGenBuffers(1, &VBOTex);
	glGenBuffers(1, &VBONorm);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBOVert);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);

	// position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBONorm);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(normals[0]), &normals[0], GL_STATIC_DRAW);

	// normals attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBOTex);
	glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(texCoords[0]), &texCoords[0], GL_STATIC_DRAW);

	// texture coord attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);


	shaderProgram = program;
	glUniformMatrix4fv(glGetUniformLocation(program, "transform"), 1, GL_FALSE, glm::value_ptr(transMat));

}

void Sphere::draw() {
	glBindVertexArray(VAO);
	//setTexture("Images/Earth.bmp",600,600);
	glBindTexture(GL_TEXTURE_2D, texture);

	//glPointSize(4);
	//glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0);
	//glDrawElements(GL_QUADS, indices.size()-121, GL_UNSIGNED_INT, 0);
	glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, 0);

	//glMatrixMode(GL_MODELVIEW);
	//glPushMatrix();
	//glTranslatef(0,0,-5);

	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_NORMAL_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
	//glNormalPointer(GL_FLOAT, 0, &normals[0]);
	//glTexCoordPointer(2, GL_FLOAT, 0, &texCoords[0]);
	//glDrawElements(GL_QUADS, indices.size() / 4, GL_UNSIGNED_SHORT, &indices[0]);
	//glPopMatrix();
}

void Sphere::init(GLint program) {
	updateVAO(program);
}


void Sphere::updateTransMat() {
	transMat = glm::identity<glm::mat4>();
	transMat = glm::translate(transMat, pos);
	//transMat = transMat*glm::toMat4(glm::quat(glm::vec4( glm::radians(rot.z),glm::normalize(glm::vec3(1.0,1,1)) )));
	transMat = glm::rotate(transMat, glm::radians(rot.x), glm::vec3(1, 0, 0));
	transMat = glm::rotate(transMat, glm::radians(rot.y), glm::vec3(0, 1, 0));
	transMat = glm::rotate(transMat, glm::radians(rot.z), glm::vec3(0, 0, 1));
	transMat = glm::scale(transMat, scale);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(transMat));
}

void Sphere::setPosition(glm::vec3 pos) {
	this->pos = pos;
	updateTransMat();
}
void Sphere::setRotation(glm::vec3 rot) {
	this->rot = rot;
	updateTransMat();
}
void Sphere::setScale(glm::vec3 scale) {
	this->scale = scale;
	updateTransMat();
}
void Sphere::setRadius(float radius) {
	this->radius = radius;
	reBuildSphere();
	updateVAO(this->shaderProgram);
	updateTransMat();
}


//circle

Circle::Circle(float radius, unsigned int sectors) :
	Object(), radius(radius), sectors(sectors)
{
	reBuildCircle();
}

void Circle::reBuildCircle() {
	vertices.clear();
	indices.clear();

	float const S = 1. / (float)(sectors - 1);

	int r, s;

	vertices.resize(sectors * 3);
	std::vector<GLfloat>::iterator v = vertices.begin();
	for (s = 0; s < sectors; s++) {
		float const x = cos(M_PI * 2 * s * S);
		float const y = sin(M_PI * 2 * s * S);

		*v++ = x * radius;
		*v++ = y * radius;
		*v++ = 0; //z
	}

	indices.resize((sectors - 1) * 2);
	std::vector<int>::iterator i = indices.begin();
	for (s = 0; s < sectors - 1; s++) {
		*i++ = s;
		*i++ = (s + 1);
	}
}

void Circle::updateVAO(GLint program) {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBOVert);
	glGenBuffers(1, &VBOTex);
	glGenBuffers(1, &VBONorm);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBOVert);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);

	// position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	shaderProgram = program;
	glUniformMatrix4fv(glGetUniformLocation(program, "transform"), 1, GL_FALSE, glm::value_ptr(transMat));

}

void Circle::draw() {
	glBindVertexArray(VAO);

	glLineWidth(3);
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Circle::init(GLint program) {
	updateVAO(program);
}


void Circle::updateTransMat() {
	transMat = glm::identity<glm::mat4>();
	transMat = glm::translate(transMat, pos);
	//transMat = transMat*glm::toMat4(glm::quat(glm::vec4( glm::radians(rot.z),glm::normalize(glm::vec3(1.0,1,1)) )));
	transMat = glm::rotate(transMat, glm::radians(rot.x), glm::vec3(1, 0, 0));
	transMat = glm::rotate(transMat, glm::radians(rot.y), glm::vec3(0, 1, 0));
	transMat = glm::rotate(transMat, glm::radians(rot.z), glm::vec3(0, 0, 1));
	transMat = glm::scale(transMat, scale);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(transMat));
}

void Circle::setPosition(glm::vec3 pos) {
	this->pos = pos;
	updateTransMat();
}
void Circle::setRotation(glm::vec3 rot) {
	this->rot = rot;
	updateTransMat();
}
void Circle::setScale(glm::vec3 scale) {
	this->scale = scale;
	updateTransMat();
}
void Circle::setRadius(float radius) {
	this->radius = radius;
	reBuildCircle();
	updateVAO(this->shaderProgram);
	updateTransMat();
}