#include "Model.h"

Model::Model()
: vertexBuffer (nullptr)
, indexBuffer (nullptr)
{

}

Model::Model(const Model& model)
{

}

Model::~Model()
{

}

bool Model::Initialize(ID3D11Device* device)
{
	bool result;

	// Initialize the vertex and index buffers
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void Model::Shutdown()
{
	ShutdownBuffers();
}

void Model::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);
}

bool Model::InitializeBuffers(ID3D11Device* device)
{
	HRESULT result;
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;

	// Fake creating a triangle.
	vertexCount = 3;
	indexCount = 3;

	// Create the vertex array.
	vertices = new VertexType[vertexCount];
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

	// Load the vertex array with data.
	vertices[0].position = DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[1].position = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);  // Top middle.
	vertices[1].color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[2].color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	// Load the index array with data.
	indices[0] = 0; // Bottom left.
	indices[1] = 1; // Top middle.
	indices[2] = 2; // Bottom right.

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
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

void Model::ShutdownBuffers()
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

void Model::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned stride;
	unsigned offset;

	// Set the buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to tactive in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

