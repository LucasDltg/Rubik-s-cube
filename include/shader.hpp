#ifndef SHADER_HPP
#define SHADER_HPP
#include "glad.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

class Shader
{
    public:
        unsigned int ID;

    Shader(const GLchar* vertexShaderPath, const GLchar* fragmentShaderPath);

    void use();
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
};




#endif