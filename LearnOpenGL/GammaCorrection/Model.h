
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
unsigned int TextureFromFile(const char* path, const string& directory);
class Model {
public: 
	vector<Texture> textures_loaded;
	Model(const char* path) {
		loadModel(path);
	}
	void Draw(Shader& shader);
private:
	//model data 
	vector<Mesh> meshes;
	string directory;

	void loadModel(string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};