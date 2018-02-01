#ifndef SHADER_H
#define SHADER_H
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#ifndef GLEW_STATIC
#define GLEW_STATIC
#include <GLEW/glew.h>
#endif
using namespace std;
class Shader {
public:
    GLuint programID;//ID
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
    {
        // 1. 从文件路径中获取顶点/片段着色器
        string vertexCode, fragmentCode;
        ifstream vShaderFile, fShaderFile;
        vShaderFile.exceptions(ifstream::badbit);
        fShaderFile.exceptions(ifstream::badbit);
        try {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (ifstream::failure e)
        {
            cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
        }
        const GLchar* vShaderCode = vertexCode.c_str();
        const GLchar* fShaderCode = fragmentCode.c_str();
        // 2. 编译着色器
        GLuint vertex, fragment;
        GLint success;
        GLchar infoLog[512];

        // 顶点着色器
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        // 打印编译错误（如果有的话）
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        };
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        // 着色器程序
        this->programID = glCreateProgram();
        glAttachShader(this->programID, vertex);
        glAttachShader(this->programID, fragment);
        glLinkProgram(this->programID);
        // 打印连接错误（如果有的话）
        glGetProgramiv(this->programID, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(this->programID, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        // 删除着色器，它们已经链接到我们的程序中了，已经不再需要了
        glDeleteShader(vertex);
        glDeleteShader(fragment);

    }
    void Use() {
        glUseProgram(this->programID);
    }

    void SetFloat(const GLchar *name, GLfloat value, GLboolean useShader = false)
    {
        if (useShader)
            this->Use();
        glUniform1f(glGetUniformLocation(this->programID, name), value);
    }
    void SetInteger(const GLchar *name, GLint value, GLboolean useShader = false)
    {
        if (useShader)
            this->Use();
        glUniform1i(glGetUniformLocation(this->programID, name), value);
    }
    void SetVector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader = false)
    {
        if (useShader)
            this->Use();
        glUniform2f(glGetUniformLocation(this->programID, name), x, y);
    }
    void SetVector2f(const GLchar *name, const glm::vec2 &value, GLboolean useShader = false)
    {
        if (useShader)
            this->Use();
        glUniform2f(glGetUniformLocation(this->programID, name), value.x, value.y);
    }
    void SetVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = false)
    {
        if (useShader)
            this->Use();
        glUniform3f(glGetUniformLocation(this->programID, name), x, y, z);
    }
    void SetVector3f(const GLchar *name, const glm::vec3 &value, GLboolean useShader = false)
    {
        if (useShader)
            this->Use();
        glUniform3f(glGetUniformLocation(this->programID, name), value.x, value.y, value.z);
    }
    void SetVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader = false)
    {
        if (useShader)
            this->Use();
        glUniform4f(glGetUniformLocation(this->programID, name), x, y, z, w);
    }
    void SetVector4f(const GLchar *name, const glm::vec4 &value, GLboolean useShader = false)
    {
        if (useShader)
            this->Use();
        glUniform4f(glGetUniformLocation(this->programID, name), value.x, value.y, value.z, value.w);
    }
    void SetMatrix4(const GLchar *name, const glm::mat4 &matrix, GLboolean useShader = false)
    {
        if (useShader)
            this->Use();
        glUniformMatrix4fv(glGetUniformLocation(this->programID, name), 1, GL_FALSE, glm::value_ptr(matrix));
    }


};
#endif // !SHADER_H
