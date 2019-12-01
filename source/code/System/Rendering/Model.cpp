#include "Model.h"

#include <iostream>
#include <fstream>
#include <d3d11.h>

#include "Texture.h"
#include "Application.h"
#include "FileSystem/FileSystem.h"
#include "Assimp/scene.h"
#include "Assimp/postprocess.h"

Model::Model()
	: VertexBuffer(nullptr)
	, IndexBuffer(nullptr)
	, texture(nullptr)
{

}

Model::Model(const Model& /*model*/)
{

}

Model::~Model()
{

}
bool Model::Initialize(ID3D11Device* Device, ID3D11DeviceContext* DeviceContext, char* ModelFilename, char* TextureFilename)
{
	bool result;
	Assimp::Importer* Importer = new Assimp::Importer();

	std::vector<aiMesh*> Meshes;
	result = LoadModel(Importer, ModelFilename, OUT Meshes);
	if (!result)
	{
		return false;
	}

	result = InitializeBuffers(Device, Meshes);
	if (!result)
	{
		return false;
	}

	result = LoadTexture(Device, DeviceContext, TextureFilename);
	if (!result)
	{
		return false;
	}


	Importer->FreeScene();
	delete Importer;
	Importer = nullptr;

	return true;
}


void Model::Shutdown()
{
	ReleaseTexture();

	ReleaseBuffers();

	ReleaseModel();
}


void Model::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);
}


bool Model::InitializeBuffers(ID3D11Device* Device, std::vector<aiMesh*>& Meshes)
{
	// 1. Clear buffers.
	Vertices.clear();
	Indices.clear();

	// 2. Initialize vertex buffer
	for (unsigned I = 0; I < Meshes.size(); ++I)
	{
		aiMesh* Mesh = Meshes[I];

		for (unsigned J = 0; J < Mesh->mNumVertices; ++J)
		{
			VertexShaderType Vertex;

			Vertex.Position.x = Mesh->mVertices[J].x;
			Vertex.Position.y = Mesh->mVertices[J].y;
			Vertex.Position.z = Mesh->mVertices[J].z;

			Vertex.Normal.x = Mesh->mNormals[J].x;
			Vertex.Normal.y = Mesh->mNormals[J].y;
			Vertex.Normal.z = Mesh->mNormals[J].z;

			// Only supports a single text-coord atm.
			Vertex.TexCoord.x = Mesh->mTextureCoords[0] ? Mesh->mTextureCoords[0][J].x : 0.0f;
			Vertex.TexCoord.y = Mesh->mTextureCoords[0] ? Mesh->mTextureCoords[0][J].y : 0.0f;

			Vertices.push_back(Vertex);
		}

		for (unsigned J = 0; J < Mesh->mNumFaces; ++J)
		{
			aiFace face = Mesh->mFaces[J];

			for (unsigned K = 0; K < face.mNumIndices; K++)
			{
				Indices.push_back(face.mIndices[K]);
			}
		}
	}

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexShaderType) * Vertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = &Vertices[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	HRESULT result = Device->CreateBuffer(&vertexBufferDesc, &vertexData, &VertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * Indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = &Indices[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = Device->CreateBuffer(&indexBufferDesc, &indexData, &IndexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


void Model::ReleaseBuffers()
{
	// Release the index buffer.
	if (IndexBuffer)
	{
		IndexBuffer->Release();
		IndexBuffer = 0;
	}

	// Release the vertex buffer.
	if (VertexBuffer)
	{
		VertexBuffer->Release();
		VertexBuffer = 0;
	}
}


void Model::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned stride;
	unsigned offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexShaderType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


bool Model::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;

	// Create the texture object.
	texture = new Texture;
	if (!texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}


void Model::ReleaseTexture()
{
	// Release the texture object.
	if (texture)
	{
		texture->Shutdown();
		delete texture;
		texture = nullptr;
	}
}


void Model::ProcessNode(aiNode* Node, const aiScene* Scene, OUT std::vector<aiMesh*>& Meshes)
{
	for (unsigned I = 0; I < Node->mNumMeshes; ++I)
	{
		aiMesh* Mesh = Scene->mMeshes[Node->mMeshes[I]];
		Meshes.push_back(Mesh);
	}

	for (unsigned I = 0; I < Node->mNumChildren; ++I)
	{
		ProcessNode(Node->mChildren[I], Scene, OUT Meshes);
	}
}

bool Model::LoadModel(Assimp::Importer* Importer, char* Filename, OUT std::vector<aiMesh*>& Meshes)
{
	std::string Path = Application::GetInstance()->GetFileSystem()->GetCorrectPath(Filename);
	const aiScene* Scene = Importer->ReadFile(Path, aiProcess_Triangulate);

	if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << Importer->GetErrorString() << std::endl;
		return false;
	}

	std::string Directory = Path.substr(0, Path.find_last_of('/'));
	ProcessNode(Scene->mRootNode, Scene, OUT Meshes);

	return true;
}

ID3D11ShaderResourceView* Model::GetTexture()
{
	return texture->GetTexture();
}

void Model::ReleaseModel()
{

}