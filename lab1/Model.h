#ifndef MODEL_H

#define MODEL_H


#include <GL\GL.h>
#include "GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "ShaderLoader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

class Model
{
public:

    vector<Mesh> meshes;
    string directory;

    Model(string const& path)
    {
        loadModel(path);
        directory = path.substr(0, path.find_last_of('/'));
    }

    void Draw(GLuint shaderProgram) {
        for (auto& mesh : meshes) {
            mesh.Draw(shaderProgram);
        }
    }

private:

    void loadModel(string const& path)
    {
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(path,
                aiProcess_CalcTangentSpace |
                aiProcess_Triangulate |
                aiProcess_JoinIdenticalVertices |
                aiProcess_SortByPType);

            if (nullptr == scene) {
                cout << "ERROR: IMPORT FAILED", importer.GetErrorString();
            }
            processNode(scene->mRootNode, scene);
                //DoTheSceneProcessing(scene);

        
    }

    void processNode(aiNode* node, const aiScene* scene)
    {

        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; meshes.push_back(processMesh(mesh, scene));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
        vector<Vertex> vertices;
        vector<unsigned int> indices;

        // Проверяем, есть ли вершины
        if (!mesh->mVertices) {
            cerr << "WARNING: Mesh has no vertices!" << endl;
            return Mesh(vertices, indices); // Возвращаем пустой меш
        }

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

            // Проверяем, есть ли нормали
            if (mesh->mNormals) {
                vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            }
            else {
                vertex.Normal = glm::vec3(0.0f); // Задаём нулевые нормали
            }

            vertices.push_back(vertex); // Не забываем добавлять вершину в вектор!
        }

        // Обрабатываем индексы
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        return Mesh(vertices, indices);
    }
};

#endif