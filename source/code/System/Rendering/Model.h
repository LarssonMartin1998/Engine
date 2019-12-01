
#ifndef _MODEL_H_
#define _MODEL_H_

#define OUT

#include <directxmath.h>
#include <vector>
#include "Assimp/Importer.hpp"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
class Texture;
struct aiNode;
struct aiScene;
struct aiMesh;

class Model
{
public:

	Model();
	Model(const Model&);
	~Model();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*, char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount() { return Indices.size(); }
	int GetVertexCount() { return Vertices.size(); }

	ID3D11ShaderResourceView* GetTexture();

private:

	struct VertexShaderType
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 TexCoord;
		DirectX::XMFLOAT3 Normal;
	};

	bool InitializeBuffers(ID3D11Device* Device, std::vector<aiMesh*>& Meshes);
	void ReleaseBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadModel(Assimp::Importer* Importer, char* Filename, OUT std::vector<aiMesh*>& Meshes);
	void ReleaseModel();

	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

	void ProcessNode(aiNode* Node, const aiScene* Scene, OUT std::vector<aiMesh*>& Meshes);

	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	std::vector<VertexShaderType> Vertices;
	std::vector<unsigned> Indices;

	Texture* texture;

};

#endif