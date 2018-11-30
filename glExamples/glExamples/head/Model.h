#pragma once
#include <stdio.h>
#include "Tool.h"
#include "Mesh.h"

class Model {
public:
	Model(char* path)
	{
		loadModel(path);
	}
	Model() {}
	static GLuint loadTexture(char const *path);
	static GLuint loadskybox(std::vector<std::string> paths);
	void Draw(Shader shader);
private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;
	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};