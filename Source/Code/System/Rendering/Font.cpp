#include "Font.h"

#include <fstream>
#include <d3d11.h>
#include <string.h>

#include "Texture.h"
#include "Application.h"
#include "FileSystem/FileSystem.h"

Font::Font()
: texture (nullptr)
, font (nullptr)
{
	
}

Font::Font(const Font& /*font*/)
{

}

Font::~Font()
{

}

bool Font::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* fontTextureFilename, char* fontDataFilename)
{
	bool result = LoadFontData(fontDataFilename);
	if (!result)
	{
		return false;
	}

	result = LoadTexture(device, deviceContext, fontTextureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}


void Font::Shutdown()
{
	ReleaseTexture();

	ReleaseFontData();
}

bool Font::LoadFontData(char* filename)
{
	const unsigned numCharsInFont = 95;
	font = new FontType[numCharsInFont];
	if (!font)
	{
		return false;
	}

	std::string filepath = Application::GetInstance()->GetFileSystem()->GetCorrectPath(filename);

	std::ifstream inStream;
	inStream.open(filepath.c_str());
	if (inStream.fail())
	{
		return false;
	}

	char temp;
	for (unsigned cIndex = 0; cIndex < numCharsInFont; ++cIndex)
	{
		inStream.get(temp);
		while (temp != ' ')
		{
			inStream.get(temp);
		}
		inStream.get(temp);
		while (temp != ' ')
		{
			inStream.get(temp);
		}

		inStream >> font[cIndex].left;
		inStream >> font[cIndex].right;
		inStream >> font[cIndex].size;
	}

	inStream.close();

	return true;
}

bool Font::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
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

void Font::ReleaseFontData()
{
	if (font)
	{
		delete[] font;
		font = nullptr;
	}
}

void Font::ReleaseTexture()
{
	// Release the texture object.
	if (texture)
	{
		texture->Shutdown();
		delete texture;
		texture = nullptr;
	}
}

ID3D11ShaderResourceView* Font::GetTexture()
{
	return texture->GetTexture();
}

void Font::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	VertexType* vertexPtr = static_cast<VertexType*>(vertices);

	unsigned numLetters = static_cast<int>(strlen(sentence));

	int index = 0;
	for (unsigned letterIndex = 0; letterIndex < numLetters; letterIndex++)
	{
		unsigned letter = static_cast<unsigned>(sentence[letterIndex] - 32);

		// 0 == ' ', if it's a space, move the drawX by three pixels.
		if (letter == 0)
		{
			drawX += 3.0f;
		}
		else
		{
			// First triangle in quad.
			vertexPtr[index].position = DirectX::XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = DirectX::XMFLOAT2(font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = DirectX::XMFLOAT3((drawX + font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = DirectX::XMFLOAT2(font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = DirectX::XMFLOAT3(drawX, (drawY - 16), 0.0f);  // Bottom left.
			vertexPtr[index].texture = DirectX::XMFLOAT2(font[letter].left, 1.0f);
			index++;

			// Second triangle in quad.
			vertexPtr[index].position = DirectX::XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = DirectX::XMFLOAT2(font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = DirectX::XMFLOAT3(drawX + font[letter].size, drawY, 0.0f);  // Top right.
			vertexPtr[index].texture = DirectX::XMFLOAT2(font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = DirectX::XMFLOAT3((drawX + font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = DirectX::XMFLOAT2(font[letter].right, 1.0f);
			index++;

			// Update the x location for drawing by the size of the letter and one pixel.
			drawX = drawX + font[letter].size + 1.0f;
		}
	}
}