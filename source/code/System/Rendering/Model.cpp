#include "Model.h"

#include <fstream>
#include <d3d11.h>

#include "Texture.h"

Model::Model()
: vertexBuffer (nullptr)
, indexBuffer (nullptr)
, modelData (nullptr)
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

	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}

	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// Load the texture for this model.
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

	ReleaseModel();

	ReleaseBuffers();
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

	//// Load the vertex array and index array with data.
	//for (int vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
	//{
	//	ModelData& mdl = modelData[vertexIndex];
	//	VertexShaderType& vertex = vertices[vertexIndex];

	//	vertex.position = DirectX::XMFLOAT3(mdl.x, mdl.y, mdl.z);
	//	vertex.normal = DirectX::XMFLOAT3(mdl.normalX, mdl.normalY, mdl.normalZ);
	//	vertex.texture = DirectX::XMFLOAT2(mdl.u, mdl.v);

	//	indices[vertexIndex] = vertexIndex;
	//}

	vertexCount = 3;
	indexCount = 3;

	vertices[0].position = DirectX::XMFLOAT3(-1.0f, 1.0f, 0.0f);
	vertices[0].normal = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertices[0].texture = DirectX::XMFLOAT2(0.0f, 1.0f);

	vertices[1].position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertices[1].normal = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertices[1].texture = DirectX::XMFLOAT2(0.5f, 0.0f);

	vertices[2].position = DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertices[2].normal = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertices[2].texture = DirectX::XMFLOAT2(1.0f, 1.0f);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexShaderType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource strucutre a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource strucutre a pointer to the vertex data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Now create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and idnex buffers have been created and loaded.
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

void Model::ReleaseBuffers()
{
	if (indexBuffer)
	{
		indexBuffer->Release();
		indexBuffer = nullptr;
	}

	if (vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = nullptr;
	}
}

void Model::ReleaseModel()
{
	if (modelData)
	{
		delete modelData;
		modelData = nullptr;
	}
}

void Model::ReleaseTexture()
{
	if (texture)
	{
		texture->Shutdown();
		delete texture;
		texture = nullptr;
	}
}

void Model::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned stride;
	unsigned offset;

	// Set the buffer stride and offset.
	stride = sizeof(VertexShaderType);
	offset = 0;

	// Set the vertex buffer to tactive in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool Model::LoadModel(char* filename)
{
	std::ifstream fileInput;
	char input;

	fileInput.open(filename);

	if (fileInput.fail())
	{
		return false;
	}

	// Read up the value of vertex count.
	fileInput.get(input);
	while (input != ':')
	{
		fileInput.get(input);
	}

	fileInput >> vertexCount;
	indexCount = vertexCount;

	modelData = new ModelData;
	if (!modelData)
	{
		return false;
	}

	// Read up to the beginning of the data.
	fileInput.get(input);
	while (input != ':')
	{
		fileInput.get(input);
	}
	fileInput.get(input);
	fileInput.get(input);

	// Read the vertex data.
	for (int vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
	{
		ModelData& md = modelData[vertexIndex];
		fileInput >> md.x >> md.y >> md.z;
		fileInput >> md.u >> md.v;
		fileInput >> md.normalX >> md.normalY >> md.normalZ;
	}

	fileInput.close();

	return true;
}

bool Model::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;

	// Create the texture object.
	texture = new Texture();
	if (!texture)
	{
		return false;
	}

	result = texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

ID3D11ShaderResourceView* Model::GetTexture()
{
	return texture->GetTexture();
}