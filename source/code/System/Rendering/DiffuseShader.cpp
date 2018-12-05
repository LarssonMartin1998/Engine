#include "DiffuseShader.h"

#include <d3dcompiler.h>
#include <fstream>
#include <string.h>

#include "Application.h"
#include "FileSystem/FileSystem.h"

DiffuseShader::DiffuseShader()
: vertexShader (nullptr)
, pixelShader (nullptr)
, inputLayout (nullptr)
, matrixBuffer (nullptr)
, lightBuffer (nullptr)
, samplerState (nullptr)
{

}

DiffuseShader::DiffuseShader(const DiffuseShader& /*diffuseShader*/)
{

}

DiffuseShader::~DiffuseShader()
{

}

bool DiffuseShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"diffuse_vertex.vs", L"diffuse_pixel.ps");
	if (!result)
	{
		return false;
	}

	return true;
}

void DiffuseShader::Shutdown()
{
	ShutdownShader();
}

bool DiffuseShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, DirectX::XMMATRIX& worldMatrix, DirectX::XMMATRIX& viewMatrix, DirectX::XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, DirectX::XMFLOAT3& lightDirection, DirectX::XMFLOAT4& diffuseColor, DirectX::XMFLOAT4& ambientColor)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetUpVertexShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	result = SetUpPixelShaderParameters(deviceContext, texture, lightDirection, diffuseColor, ambientColor);

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);
	
	return true;
}

bool DiffuseShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	FileSystem* fileSystem = Application::GetInstance()->GetFileSystem();

	// Initialize the pointres this function will use to nullptr.
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;
	
	// Compile the vertex shader code.
	WCHAR* vsFilepath = fileSystem->GetCorrectPathShader(vsFilename);
	HRESULT result = D3DCompileFromFile(vsFilepath, NULL, NULL, "DiffuseVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		CatchShaderErrorMessage(errorMessage, hwnd, vsFilename);

		return false;
	}

	// Okay to delete vsFilepath
	// TODO, THIS MUST BE DELETED
	//delete vsFilepath;

	// Compile the pixel shader code.
	WCHAR* psFilepath = fileSystem->GetCorrectPathShader(psFilename);
	result = D3DCompileFromFile(psFilepath, NULL, NULL, "DiffusePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		CatchShaderErrorMessage(errorMessage, hwnd, psFilename);

		return false;
	}

	// Okay to delete psFilepath
	// TODO, THIS MUST BE DELETED
	//delete psFilepath;

	// Create the vertex shader from our buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from our buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the vertex input layout decription
	// This setup needs to match the VertexType structure in the ModeClass and in the shader.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;
	
	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	unsigned numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create teh vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &inputLayout);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	// Create a texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, &samplerState);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the dynamic matrix constant buffer that this is the vertex shader.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the dynamic light constant buffer that this is the vertex shader.
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void DiffuseShader::ShutdownShader()
{
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = nullptr;
	}

	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = nullptr;
	}

	if (inputLayout)
	{
		inputLayout->Release();
		inputLayout = nullptr;
	}

	if (pixelShader)
	{
		pixelShader->Release();
		pixelShader = nullptr;
	}

	if (vertexShader)
	{
		vertexShader->Release();
		vertexShader = nullptr;
	}

	if (samplerState)
	{
		samplerState->Release();
		samplerState = nullptr;
	}
}

bool DiffuseShader::SetUpVertexShaderParameters(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX& worldMatrix, DirectX::XMMATRIX& viewMatrix, DirectX::XMMATRIX& projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* matrixData;
	unsigned bufferNumber;

	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	matrixData = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	matrixData->world = worldMatrix;
	matrixData->view = viewMatrix;
	matrixData->projection = projectionMatrix;

	deviceContext->Unmap(matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finally set the constant bufer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);

	return true;
}

bool DiffuseShader::SetUpPixelShaderParameters(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, DirectX::XMFLOAT3& lightDirection, DirectX::XMFLOAT4& diffuseColor, DirectX::XMFLOAT4& ambientColor)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	LightBufferType* lightData;
	unsigned bufferNumber;

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	lightData = (LightBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	lightData->lightDirection = lightDirection;
	lightData->diffuseColor = diffuseColor;
	lightData->ambientColor = ambientColor;
	lightData->pad01 = 0.0f;

	deviceContext->Unmap(lightBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finally set the constant bufer in the vertex shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &lightBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void DiffuseShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(inputLayout);

	// Set the vertex and pixel shaders that will be used to render.
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &samplerState);

	// Render
	deviceContext->DrawIndexed(indexCount, 0, 0);
}

void DiffuseShader::CatchShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* filename)
{
	// if the shader failed to compile it should have written something to the error message.
	if (errorMessage)
	{
		OutputShaderErrorMessage(errorMessage, hwnd, filename);
	}
	// If it was nothing in the error message then it simply could not find the shader file itself.
	else
	{
		MessageBox(hwnd, (LPCSTR)filename, "Missing Shader File", MB_OK);
	}
}

void DiffuseShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileError;
	size_t bufferSize;
	std::ofstream fout;

	// Get a pointer to the error message text buffer.
	compileError = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (unsigned errorIndex = 0; errorIndex < bufferSize; ++errorIndex)
	{
		fout << compileError[errorIndex];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = nullptr;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, "Error compiling shader. Check shader-error.txt for message", (LPCSTR)shaderFilename, MB_OK);
}