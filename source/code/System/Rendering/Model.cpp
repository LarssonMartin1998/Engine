#include "Model.h"

#include <fstream>
#include <vector>
#include <d3d11.h>

#include "Texture.h"
#include "Application.h"
#include "FileSystem/FileSystem.h"
#include "Fbx/FbxHelper.h"
#include "fbxsdk/scene/fbxaxissystem.h"

Model::Model()
: vertexBuffer(nullptr)
, indexBuffer(nullptr)
, texture (nullptr)
{
	
}

Model::Model(const Model& /*model*/)
{

}

Model::~Model()
{

}

bool Model::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* modelFilename, char* textureFilename)
{
	bool result;

	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}
	/*result = LoadFbx(modelFilename);
	if (!result)
	{
		return false;
	}*/

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
	// Create the index array.
	VertexShaderType* vertices = new VertexShaderType[vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	unsigned long* indices = new unsigned long[indexCount];
	if (!indices)
	{
		return false;
	}

	// Load the vertex array and index array with data.
	for (int vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
	{
		VertexShaderType& v = vertices[vertexIndex];
		ModelData& md = modelData[vertexIndex];

		v.position = md.position;
		v.texture = md.uv;
		v.normal = md.normal;

		indices[vertexIndex] = vertexIndex;
	}

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexShaderType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA indexData;
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
	Application* app = Application::GetInstance();

	FbxHelper* fbxHelper = app->GetFbxHelper();
	FbxManager* fbxManager = fbxHelper->GetFbxManager();
	FbxIOSettings* IOSettings = fbxManager->GetIOSettings();
	FbxScene* fbxScene = FbxScene::Create(fbxManager, "");
	FbxImporter* importer = FbxImporter::Create(fbxManager, "");

	std::string filepath = app->GetFileSystem()->GetCorrectPath(filename);
	bool result = importer->Initialize(filepath.c_str(), -1, IOSettings);
	if (!result)
	{
		return false;
	}

	result = importer->Import(fbxScene);
	if (!result)
	{
		return false;
	}

	// Importer has imported the scene, it's safe to destroy.
	importer->Destroy();

	// Convert the scene to left handed suited for DirectX
	fbxHelper->ConvertSceneToLeftHandAndMeters(fbxScene);

	FbxNode* fbxRootNode = fbxScene->GetRootNode();

	vertexCount = 0;
	indexCount = 0;

	int upDir;
	fbxScene->GetGlobalSettings().GetAxisSystem().GetUpVector(upDir);
	int frontDir;
	fbxScene->GetGlobalSettings().GetAxisSystem().GetFrontVector(frontDir);

	FbxAxisSystem directX;
	directX.ConvertScene(fbxScene);

	if (fbxRootNode)
	{
		for (int i = 0; i < fbxRootNode->GetChildCount(); i++)
		{
			FbxNode* fbxChildNode = fbxRootNode->GetChild(i);
			FbxNodeAttribute* nodeAttribute = fbxChildNode->GetNodeAttribute();

			if(nodeAttribute == NULL)
			{
				continue;
			}

			if (nodeAttribute->GetAttributeType() != FbxNodeAttribute::eMesh)
			{
				continue;
			}

			FbxMesh* mesh = static_cast<FbxMesh*>(nodeAttribute);
			FbxVector4* vertexArray = mesh->GetControlPoints();

			for (int j = 0; j < mesh->GetPolygonCount(); j++)
			{
				int numVertices = mesh->GetPolygonSize(j);

				for (int k = 0; k < numVertices; k++)
				{
					int controlPointIndex = mesh->GetPolygonVertex(j, k);

					ModelData md;

					md.position.x = (float)vertexArray[controlPointIndex].mData[0];
					md.position.y = (float)vertexArray[controlPointIndex].mData[1];
					md.position.z = (float)vertexArray[controlPointIndex].mData[2];

					ReadNormal(mesh, controlPointIndex, vertexCount, md.normal);

					// Todo: Fix UV's
					// Temporary
					md.uv.x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					md.uv.y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					// Temporary

					modelData.push_back(md);
					vertexCount++;
				}
			}
		}

		indexCount = vertexCount;
	}

	return true;
}

bool Model::LoadModel(char* filename)
{
	std::ifstream stream = Application::GetInstance()->GetFileSystem()->LoadFileToStream(filename);
	char input;

	// Read up to the value of vertex count.
	stream.get(input);
	while (input != ':')
	{
		stream.get(input);
	}

	// Read in the vertex count.
	stream >> vertexCount;
	indexCount = vertexCount;

	modelData.clear();

	// Read up to the beginning of the data.
	stream.get(input);
	while (input != ':')
	{
		stream.get(input);
	}
	stream.get(input);
	stream.get(input);

	// Read in the vertex data.
	for (int vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
	{
		ModelData md;
		stream >> md.position.x >> md.position.y >> md.position.z;
		stream >> md.uv.x >> md.uv.y;
		stream >> md.normal.x >> md.normal.y >> md.normal.z;
		modelData.push_back(md);
	}

	return true;
}

ID3D11ShaderResourceView* Model::GetTexture()
{
	return texture->GetTexture();
}

void Model::ReleaseModel()
{

}

void Model::ReadNormal(FbxMesh* mesh, int ctrlPointIndex, int vertexCounter, DirectX::XMFLOAT3& outNormal)
{
	if (mesh->GetElementNormalCount() < 1)
	{
		throw std::exception("Invalid Normal Number");
	}

	FbxGeometryElementNormal* vertexNormal = mesh->GetElementNormal();
	if (!vertexNormal)
	{
		throw std::exception("vertexNormal is nullptr");
	}

	if (vertexNormal->GetMappingMode() == FbxGeometryElementNormal::eByControlPoint)
	{
		int normalIndex = 0;

		if (vertexNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			normalIndex = ctrlPointIndex;
		}

		if (vertexNormal->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
		{
			normalIndex = vertexNormal->GetIndexArray().GetAt(ctrlPointIndex);
		}

		FbxVector4 normal = vertexNormal->GetDirectArray().GetAt(normalIndex);
		outNormal.x = static_cast<float>(normal.mData[0]);
		outNormal.y = static_cast<float>(normal.mData[1]);
		outNormal.z = static_cast<float>(normal.mData[2]);
	}
	else if (vertexNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		int normalIndex = 0;

		if (vertexNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			normalIndex = vertexCounter;
		}

		if (vertexNormal->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
		{
			normalIndex = vertexNormal->GetIndexArray().GetAt(vertexCounter);
		}

		FbxVector4 normal = vertexNormal->GetDirectArray().GetAt(normalIndex);
		outNormal.x = static_cast<float>(normal.mData[0]);
		outNormal.y = static_cast<float>(normal.mData[1]);
		outNormal.z = static_cast<float>(normal.mData[2]);
	}
}