#include "../Include/Shader.h"

#include <stdexcept>

std::string Shader::readFromFile(const char* path)
{
    
    std::ifstream file(path);

    std::stringstream fileStringstream;
    fileStringstream << file.rdbuf();

    file.close();

    return fileStringstream.str();
}

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{

    std::string vertexCode = readFromFile(vertexPath);
    std::string fragmentCode = readFromFile(fragmentPath);

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();


    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);


    GLint success;
    GLchar infoLog[INFO_LOG_SIZE];

    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, INFO_LOG_SIZE, NULL, infoLog);
        throw std::runtime_error("Vertex shader compilation error: " + std::string(infoLog));
    }



    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, INFO_LOG_SIZE, NULL, infoLog);
        throw std::runtime_error("Fragment shader compilation error: " + std::string(infoLog));
    }

    

    m_programID = glCreateProgram();

    glAttachShader(m_programID, vertexShader);
    glAttachShader(m_programID, fragmentShader);

    glLinkProgram(m_programID);
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
    auto a = glGetError();
    if (!success) {
        glGetProgramInfoLog(m_programID, INFO_LOG_SIZE, NULL, infoLog);
        throw std::runtime_error("Shader program linking error: " + std::string(infoLog));
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
    : Shader(vertexPath.c_str(), fragmentPath.c_str())
{

}

void Shader::use() const
{
    glUseProgram(m_programID);
}

GLuint Shader::getProgramID() const
{
    return m_programID;
}

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_programID, name.c_str()), (GLint)value);
}

void Shader::setInt(const std::string& name, GLint value) const
{
    glUniform1i(glGetUniformLocation(m_programID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, GLfloat value) const
{
    glUniform1f(glGetUniformLocation(m_programID, name.c_str()), value);
}
