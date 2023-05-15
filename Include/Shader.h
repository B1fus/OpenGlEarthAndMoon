#pragma once


#include "../Libs/glew-2.1.0/include/GL/glew.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader
{
private:
    GLuint m_programID;


    static const GLint INFO_LOG_SIZE = 512;


    static std::string readFromFile(const char* path);

public:

    Shader(const char* vertexPath, const char* fragmentPath);
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    void use() const;

    GLuint getProgramID() const;

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, GLint value) const;
    void setFloat(const std::string& name, GLfloat value) const;
};