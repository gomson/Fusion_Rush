#include "model.h"

Model::Model()
	: loadTangent(false), directory("")
{

}

void Model::DeleteModel()
{
	// Delete all loaded textures
	for (std::vector<ModelTexures>::iterator it = texturesLoaded.begin(); it != texturesLoaded.end(); ++it)
	{
		glDeleteTextures(GL_TEXTURE_2D, &it->id);
	}
}

void Model::Draw(Shader &shader)
{
	for (GLuint i = 0; i < this->meshes.size(); i++)
	{
		this->meshes[i].Draw(shader);
	}
}

void Model::LoadModel(string path, bool loadTangent)
{
	this->loadTangent = loadTangent;

	// Read file via ASSIMP
	Assimp::Importer importer;
	unsigned int flags;
	// By setting aiProcess_Triangulate we tell Assimp that if the model does not (entirely) consist of 
	// triangles it should transform all the model's primitive shapes to triangles. 
	// The aiPorcess_FlipUVs flips the texture coordinates on the y-axis where necessary during processing
	if (loadTangent)
		flags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace;
	else
		flags = aiProcess_Triangulate | aiProcess_FlipUVs;
	const aiScene* scene = importer.ReadFile(path, flags);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // Check for errors
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	else
	{
		std::cout << "MODEL LOADED: " << path << "\n" << std::endl;
	}

	this->directory = path.substr(0, path.find_last_of('/')); // Retrieve the directory path of the filepath

	this->processNode(scene->mRootNode, scene); // Process ASSIMP's root node recursively

												// Everything will be clearned by the import destructor.
												// Source: http://www.assimp.org/lib_html/usage.html
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// Process all the node's meshes (if any) Process each mesh located at the current node
	// Check each of the node's mesh indices
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		// The node object only contains indices to index the actual objects in the scene. 
		// The scene contains all the data, node is just to keep stuff organized (like relations between nodes). 
		// Retrieve the corresponding mesh by indexing the scene's mMeshes array
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		// Returned mesh is then passed to the processMesh function that returns a Mesh object that we can store in the meshes list/vector
		this->meshes.push_back(this->processMesh(mesh, scene));
	}

	// Then do the same for each of its children. After we've processed all of the meshes (if any) we then recursively process each of the children nodes
	// Once all the meshes have been processed we iterate through all of the node's children
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		// and call the same processNode function for each of the node's children. Once a node no longer has any children the function stops executing
		this->processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{	// Data to fill
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<ModelTexures> textures;

	//Process vertices. Walk through each of the mesh's vertices
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		Vector3 vector;
		// Positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// Normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;
		// Texture Coordinates
		// Does the mesh contain texture coordinates?
		if (mesh->mTextureCoords[0])
		{
			Vector2 vec;
			// Assimp allows a model to have up to 8 different texture coordinates per vertex which we're not going to use so we only care about the first set of texture coordinates
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = Vector2(0.0f, 0.0f);

		if (loadTangent)
		{
			// Tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangents = vector;
		}
		vertices.push_back(vertex);
	}

	// Process indices
	/*
	Assimp's interface defined each mesh having an array of faces where each face represents a single primitive,
	which in our case (due to the aiProcess_Triangulate option) are always triangles. A face contains the indices
	that define which vertices we need to draw in what order for each primitive so if we iterate over all the
	faces and store all the face's indices in the indices vector we're all set:
	*/
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// Retrieve all indices of the face and store them in the indices vector
		for (GLuint j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// Process material
	/*
	Just like with nodes, a mesh only contains an index to a material object and to retrieve
	the actual material of a mesh we need to index the scene's mMaterials array
	*/
	if (mesh->mMaterialIndex >= 0) //to check if the mesh actually contains a material or not
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex]; // Retrieve the aiMaterial object from the scene's mMaterials array
																		// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
																		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
																		// Same applies to other texture as the following list summarizes:
																		// Diffuse: texture_diffuseN
																		// Specular: texture_specularN
																		// Normal: texture_normalN

																		// 1. Diffuse maps
		vector<ModelTexures> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse"); // Retrieve the textures from the material
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. Specular maps
		vector<ModelTexures> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular"); // Retrieve the textures from the material
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// so we'll cheat a little by defining the reflection maps as ambient maps in the .obj file, which ASSIMP is able to load)
		if (loadTangent)
		{
			// 4. Normal map
			vector<ModelTexures> normalMaps = this->loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");//retrieve the textures from the material
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		}
	}

	// Return a mesh object created from the extracted mesh data
	return Mesh(vertices, indices, textures, this->loadTangent);
}

/*
Iterates over all the texture locations of the given texture type, retrieves the texture's
file location and then loads and generates the texture and stores the information in a Vertex struct
*/

vector<ModelTexures>Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
	std::vector<ModelTexures> textures;
	// Check the amount of textures stored in the material via its GetTextureCount function that expects one of the texture types we've given
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		// Retrieve each of the texture's file locations. Store the result in an aiString
		mat->GetTexture(type, i, &str);
		GLboolean skip = false;

		for (GLuint j = 0; j < texturesLoaded.size(); j++)
		{
			// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			if (texturesLoaded[j].path == str)
			{
				textures.push_back(texturesLoaded[j]);
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip) // If texture hasn't been loaded already, load it
		{
			ModelTexures texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);
			this->texturesLoaded.push_back(texture); // Add to loaded textures. Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

GLuint TextureFromFile(const char* path, std::string directory)
{
	// Generate texture ID and load texture data
	std::string filename = std::string(path);
	filename = directory + '/' + filename;
	Texture texture;
	GLuint textureID = texture.LoadTexture(filename.c_str());

	return textureID;
}