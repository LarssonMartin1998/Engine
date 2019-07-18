#include "Text.h"

#include "Font.h"
#include "FontShader.h"

Text::Text()
: font (nullptr)
, fontShader (nullptr)
, sentence1 (nullptr)
, sentence2 (nullptr)
{

}

Text::Text(const Text& /*text*/)
{

}

Text::~Text()
{

}

bool Text::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int scrnWidth, int scrnHeight, DirectX::XMMATRIX& bseViewMatrix)
{
	screenWidth = scrnWidth;
	screenHeight = scrnHeight;

	baseViewMatrix = bseViewMatrix;

	font = new Font();
	if (!font)
	{
		return false;
	}


	// TODO
	bool result = font->Initialize(device, deviceContext, "Fonts/font_default_texture.tga", "Fonts/font_default_data.txt");
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the Font Object.", "Error", MB_OK);
		return false;
	}

	fontShader = new FontShader();
	if (!fontShader)
	{
		return false;
	}

	result = fontShader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the FontShader Object.", "Error", MB_OK);
		return false;
	}

	result = InitializeSentence(&sentence1, 16, device);
	if (!result)
	{
		return false;
	}

	result = UpdateSentence(sentence1, "Hello", 0, 0, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	result = InitializeSentence(&sentence2, 16, device);
	if (!result)
	{
		return false;
	}

	result = UpdateSentence(sentence2, "Goodbye", 0, 10, 1.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}

void Text::Shutdown()
{
	ReleaseSentence(&sentence1);

	ReleaseSentence(&sentence2);

	if (fontShader)
	{
		fontShader->Shutdown();
		delete fontShader;
		fontShader = nullptr;
	}

	if (font)
	{
		font->Shutdown();
		delete font;
		font = nullptr;
	}
}

bool Text::Render(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX& worldMatrix, DirectX::XMMATRIX& orthoMatrix)
{
	bool result = RenderSentence(deviceContext, sentence1, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	result = RenderSentence(deviceContext, sentence2, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	return true;
}

bool Text::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	*sentence = new SentenceType;
	if (!*sentence)
	{
		return false;
	}

	(*sentence)->vertexBuffer = nullptr;
	(*sentence)->indexBuffer = nullptr;
	(*sentence)->maxLength = maxLength;
	(*sentence)->vertexCount = 6 * maxLength;
	(*sentence)->indexCount = (*sentence)->vertexCount;

	VertexType* vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices)
	{
		return false;
	}

	unsigned long* indices = new unsigned long[(*sentence)->indexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// Initialize the index array.
	for (unsigned index = 0; index < (*sentence)->indexCount; index++)
	{
		indices[index] = index;
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	HRESULT result = device->CreateBuffer(&vertexBufferDesc, NULL, &(*sentence)->vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	result = device->CreateBuffer(&indexBufferDesc, NULL, &(*sentence)->indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

bool Text::UpdateSentence(SentenceType* sentence, char* text, int posX, int posY, float red, float green, float blue, ID3D11DeviceContext* deviceContext)
{
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	int numLetters = static_cast<int>(strlen(text));

	if (numLetters > sentence->maxLength)
	{
		return false;
	}

	VertexType* vertices = new VertexType[sentence->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Initialize the vertex array to zeroes at first.
	memset(vertices, 0, sizeof(VertexType) * sentence->vertexCount);

	// Calculate the X and Y pixel coordinates on the screen to start drawing to.
	float drawX = static_cast<float>(((screenWidth / 2) * -1) + posX);
	float drawY = static_cast<float>((screenHeight / 2) - posY);

	font->BuildVertexArray(static_cast<void*>(vertices), text, drawX, drawY);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	VertexType* verticesPtr = static_cast<VertexType*>(mappedResource.pData);
	memcpy(verticesPtr, static_cast<void*>(vertices), sizeof(VertexType) * sentence->vertexCount);

	deviceContext->Unmap(sentence->vertexBuffer, 0);

	delete[] vertices;
	vertices = nullptr;

	return true;
}

void Text::ReleaseSentence(SentenceType** sentence)
{
	if (*sentence)
	{
		if ((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = nullptr;
		}

		if ((*sentence)->indexBuffer)
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = nullptr;
		}

		delete *sentence;
		*sentence = nullptr;
	}
}

bool Text::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, DirectX::XMMATRIX& worldMatrix, DirectX::XMMATRIX& orthoMatrix)
{
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// Set the vertex buffer to active in the input assembler so thatit can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so that it can be rendered.
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, offset);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DirectX::XMFLOAT4 pixelColor(sentence->red, sentence->green, sentence->blue, 1.0f);

	bool result = fontShader->Render(deviceContext, sentence->indexCount, worldMatrix, baseViewMatrix, orthoMatrix, font->GetTexture(), pixelColor);
	if (!result)
	{
		return false;
	}

	return true;
}