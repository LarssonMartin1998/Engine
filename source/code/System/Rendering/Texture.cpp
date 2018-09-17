#include "Texture.h"

#include <d3d11.h>
#include <stdio.h>

Texture::Texture()
: targaData (nullptr)
, texture (nullptr)
, textureView (nullptr)
{

}

Texture::Texture(const Texture& texture)
{

}

Texture::~Texture()
{

}

bool Texture::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;
	HRESULT hResult;
	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	unsigned rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	result = LoadTarga(filename, height, width);
	if (!result)
	{
		return false;
	}

	// Set up the description of the texture.
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// Create the empty texture.
	hResult = device->CreateTexture2D(&textureDesc, NULL, &texture);
	if (FAILED(hResult))
	{
		return false;
	}

	// Set the row pitch of the targa image data.
	rowPitch = (width * 4) * sizeof(unsigned char);

	// Copy the targa image data into the texture.
	deviceContext->UpdateSubresource(texture, 0, NULL, targaData, rowPitch, 0);

	// Set up the shade resource view description.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// Create the shade resource view for the texture.
	hResult = device->CreateShaderResourceView(texture, &srvDesc, &textureView);
	if (FAILED(hResult))
	{
		return false;
	}

	// Generate mipmaps for this texture.
	deviceContext->GenerateMips(textureView);

	// Release the targa image data now that the imag edata has been loaded into the texture.
	delete[] targaData;
	targaData = nullptr;

	return true;
}

void Texture::Shutdown()
{
	if (targaData)
	{
		delete[] targaData;
		targaData = nullptr;
	}

	if (texture)
	{
		texture->Release();
		texture = nullptr;
	}

	if (textureView)
	{
		textureView->Release();
		textureView = nullptr;
	}
}

// This function will reject 24-bit targa, only supports 32-bit targas
bool Texture::LoadTarga(char* filename, int& width, int& height)
{
	int error;
	int bpp;
	int imageSize;
	FILE* filePtr;
	unsigned count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;

	// Open the targa file for reading in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = (unsigned)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Get the important information from the header.
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	// Make sure that it's 32-bit and not 24-bit.
	if (bpp != 32)
	{
		return false;
	}

	// Calculate the size of the 32-bit image data.
	imageSize = width * height * 4;

	// Allocate memory for the targa image data.
	targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		return false;
	}

	// Read in the targa image data.
	count = (unsigned)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Allocate memory for the targa destination data.
	targaData = new unsigned char[imageSize];
	if (!targaData)
	{
		return false;
	}

	// Initialize the index into the targa destination data array.
	int index = 0;

	// Initialize the index into the targa image data.
	int k = (width * height * 4) - (width * 4);

	// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down.
	for (int j = 0; j < height; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			targaData[index + 0] = targaImage[k + 2]; // Red.
			targaData[index + 1] = targaImage[k + 1]; // Green.
			targaData[index + 2] = targaImage[k + 0]; // Blue.
			targaData[index + 3] = targaImage[k + 3]; // Alpha.

			// Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= width * 8;
	}

	// Release the targa image data now that it was copied into the destination array.
	delete[] targaImage;
	targaImage = nullptr;

	return true;
}