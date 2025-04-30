#ifndef SHADER_LOADER_H  
#define SHADER_LOADER_H  

#include <GL/glew.h>  
#include <string>  
#include <fstream>  
#include <sstream>  
#include <iostream>  

class ShaderLoader {
public:
    //Функция для загрузки шейдеров  
    static GLuint LoadShader(const std::string& vertexPath, const std::string& fragmentPath) {
        //Читаем файлы шейдеров  
        std::string vertexCode = ReadFile(vertexPath);
        std::string fragmentCode = ReadFile(fragmentPath);
        //Создание объектов шейдеров
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        //Загрузка исходного кода шейдеров
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        //Компиляция шейдеров
        glShaderSource(vertexShader, 1, &vShaderCode, NULL);
        glCompileShader(vertexShader);
        CheckCompileErrors(vertexShader, "VERTEX");

        glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
        glCompileShader(fragmentShader);
        CheckCompileErrors(fragmentShader, "FRAGMENT");

        //Создание шейдерной программы  
        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        CheckProgramErrors(shaderProgram);

        //Удаление шейдеров, они больше не нужны после связки
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return shaderProgram;
    }

    //Функция для установки uniform переменной float  
    static void SetUniform1f(GLuint program, const std::string& name, float value) {
        glUniform1f(glGetUniformLocation(program, name.c_str()), value);
    }

private:
    //Функция для чтения файла  
    static std::string ReadFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Нельзя открыть файл. Ошибка " << filePath << std::endl;
            return "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    //Проверка ошибок компиляции шейдеров  
    static void CheckCompileErrors(GLuint shader, const std::string& type) {
        GLint success;
        GLchar infoLog[1024];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "Ошибка компиляции шейдеров.\n" << infoLog << "\n" << std::endl;
        }
    }

    //Проверка ошибок связки программы
    static void CheckProgramErrors(GLuint program) {
        GLint success;
        GLchar infoLog[1024];
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(program, 1024, NULL, infoLog);
            std::cerr << "Ошибка связки программы.\n" << infoLog << "\n" << std::endl;
        }
    }
};

#endif