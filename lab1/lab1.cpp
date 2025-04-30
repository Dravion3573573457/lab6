#define GLEW_DLL  
#define GLFW_DLL  

#include "GL/glew.h"  
#include "GLFW/glfw3.h"  
#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "ShaderLoader.h"
#include <iostream>  
#include "Mesh.h"
#include "Model.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

void settingMat4(int ID, const std::string& name, const glm::mat4& mat) {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}



GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;
bool firstmouse = true;
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 1024;
float lastX = SCR_WIDTH / 2;
float lastY = SCR_HEIGHT / 2;

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstmouse) {
        lastX = xpos;
        lastY = ypos;
        firstmouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.03f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
};

int main() {
    //Инициализация GLFW
    if (!glfwInit()) {
        std::cerr << "Ошибка не запустить GLFW" << std::endl;
        return 1;
    }

    //Параметры окна
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mainwindow", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;

    //Инициализация GLEW  
    GLenum ret = glewInit();
    if (GLEW_OK != ret) {
        std::cerr << "Ошибка не запустить GLEW" << glewGetErrorString(ret) << std::endl;
        return 1;
    }

    //Создание массива вершин (VAO), буфер вершин (VBO) и буфер индексов (EBO)
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Загрузка шейдеров из доп файлов
    GLuint shader_program = ShaderLoader::LoadShader("vertex_shader.glsl", "fragment_shader.glsl");


    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 4.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    Model ourModel("models/lab3BOB.obj");

    //Основной цикл
    while (!glfwWindowShouldClose(window)) {

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f,
            100.0f);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, mouse_callback);

        glClearColor(0.2, 0.3, 0.4, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        const float cameraSpeed = 0.002f;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

        glm::mat4 view = glm::lookAt(
            cameraPos,
            cameraPos + cameraFront,
            cameraUp);

        //Активация шейдерной программы, чтобы opengl знал какие шейдеры использовать
        glUseProgram(shader_program);
        settingMat4(shader_program, "projection", projection);
        settingMat4(shader_program, "view", view);

        glm::vec3 lightPos = cameraPos + glm::vec3(0.0f, 1.0f, 0.5f);
        glEnable(GL_DEPTH_TEST);

        glUniform3f(glGetUniformLocation(shader_program, "Light_1.ambient"), 0.3f, 0.3f, 0.3f);
        glUniform3f(glGetUniformLocation(shader_program, "Light_1.diffuse"), 0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(shader_program, "Light_1.specular"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shader_program, "Light_1.position"), cameraPos.x, cameraPos.y + 1.0f, cameraPos.z + 1.0f);

        glUniform3f(glGetUniformLocation(shader_program, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);

        glUniform3f(glGetUniformLocation(shader_program, "material.ambient"), 0.24725f, 0.1995f, 0.0745f);
        glUniform3f(glGetUniformLocation(shader_program, "material.diffuse"), 0.75164f, 0.60648f, 0.22648f);
        glUniform3f(glGetUniformLocation(shader_program, "material.specular"), 0.628281f, 0.555802f, 0.366065f);
        glUniform1f(glGetUniformLocation(shader_program, "material.shininess"), 51.2f);

        glm::mat4 model = glm::mat4(1.0f);
        settingMat4(shader_program, "model", model);
        ourModel.Draw(shader_program);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}