#include "Model.h"

#include <fstream>
#include <d3d11.h>

#include "fbxsdk.h"
#include "Texture.h"
#include "Application.h"
#include "Fbx/FbxHelper.h"


Model::Model()
: vertexBuffer(nullptr)
, indexBuffer(nullptr)
, modelData(nullptr)
, texture (nullptr)
{
	
}

Model::Model(const Model& model)
{

}

Model::~Model()
{

}

bool Model::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* modelFilename, char* textureFilename)
{
	bool result;

	//result = LoadModel(modelFilename);
	//if (!result)
	//{
	//	return false;
	//}

	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	result = LoadTexture(device, deviceContext, textureFilename);
	if (!result)
	{
		return false;
	}

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


bool Model::InitializeBuffers(ID3D11Device* device)
{
	HRESULT result;
	VertexShaderType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;

	// Create the vertex array.
	vertices = new VertexShaderType[vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[indexCount];
	if (!indices)
	{
		return false;
	}

	// Load the vertex array and index array with data.
	for (int vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
	{
		VertexShaderType& vertex = vertices[vertexIndex];
		ModelData& md = modelData[vertexIndex];

		vertex.position = DirectX::XMFLOAT3(md.x, md.y, md.z);
		vertex.texture = DirectX::XMFLOAT2(md.u, md.v);
		vertex.normal = DirectX::XMFLOAT3(md.normalX, md.normalY, md.normalZ);

		indices[vertexIndex] = vertexIndex;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexShaderType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}


void Model::ReleaseBuffers()
{
	// Release the index buffer.
	if (indexBuffer)
	{
		indexBuffer->Release();
		indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = 0;
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
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

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


bool Model::LoadFbx(char* filename)
{
	bool result;
	FbxImporter* importer;
	FbxScene* fbxScene;
	FbxNode* fbxRootNode;
	int polygonCount;
	int vertexCounter;

	FbxManager* fbxManager = Application::GetInstance()->GetFbxHelper()->GetFbxManager();

	fbxScene = FbxScene::Create(fbxManager, "");

	result = importer->Import(fbxScene);
	if (!result)
	{
		return false;
	}

	importer->Destroy();

	fbxRootNode = fbxScene->GetRootNode();

	if (fbxRootNode)
	{
		for (int i = 0; i < fbxRootNode->GetChildCount(); i++)
		{
			FbxNode* fbxChildNode = fbxRootNode->GetChild(i);
			FbxNodeAttribute::EType attributeType;
			FbxMesh* mesh;
			FbxVector4* vertices;

			if (fbxChildNode->GetNodeAttribute() == NULL)
			{
				continue;
			}
				

			attributeType = fbxChildNode->GetNodeAttribute()->GetAttributeType();

			if (attributeType != FbxNodeAttribute::eMesh)
			{
				continue;
			}

			mesh = (FbxMesh*)fbxChildNode->GetNodeAttribute();

			vertices = mesh->GetControlPoints();

			polygonCount = mesh->GetPolygonCount();
			vertexCount = polygonCount * 3;
			indexCount = vertexCount;
			modelData = new ModelData[vertexCount];

			vertexCounter = 0;
			for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
			{
				int numVertices = mesh->GetPolygonSize(polygonIndex);
				assert(numVertices == 3);

				for (int vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
				{
					int controlPointIndex = mesh->GetPolygonVertex(polygonIndex, vertexIndex);

					modelData[vertexCounter].x = (float)vertices[controlPointIndex].mData[0];
					modelData[vertexCounter].y = (float)vertices[controlPointIndex].mData[0];
					modelData[vertexCounter].z = (float)vertices[controlPointIndex].mData[0];

					vertexCounter++;
				}
			}

		}
	}

	return true;
}

//bool Model::LoadModel(char* filename)
//{
//	std::ifstream fin;
//	char input;
//
//	// Open the model file.
//	fin.open(filename);
//
//	// If it could not open the file then exit.
//	if (fin.fail())
//	{
//		return false;
//	}
//
//	// Read up to the value of vertex count.
//	fin.get(input);
//	while (input != ':')
//	{
//		fin.get(input);
//	}
//
//	// Read in the vertex count.
//	fin >> vertexCount;
//
//	// Set the number of indices to be the same as the vertex count.
//	indexCount = vertexCount;
//
//	// Create the model using the vertex count that was read in.
//	modelData = new ModelData[vertexCount];
//	if (!modelData)
//	{
//		return false;
//	}
//
//	// Read up to the beginning of the data.
//	fin.get(input);
//	while (input != ':')
//	{
//		fin.get(input);
//	}
//	fin.get(input);
//	fin.get(input);
//
//	// Read in the vertex data.
//	for (int vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
//	{
//		ModelData& md = modelData[vertexIndex];
//
//		fin >> md.x >> md.y >> md.z;
//		fin >> md.u >> md.v;
//		fin >> md.normalX >> md.normalY >> md.normalZ;
//	}
//
//	// Close the model file.
//	fin.close();
//
//	return true;
//}

ID3D11ShaderResourceView* Model::GetTexture()
{
	return texture->GetTexture();
}

void Model::ReleaseModel()
{
	if (modelData)
	{
		delete[] modelData;
		modelData = nullptr;
	}
}