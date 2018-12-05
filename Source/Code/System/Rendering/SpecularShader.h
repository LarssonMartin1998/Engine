#ifndef _SPECULARSHADER_H_
#define _SPECULARSHADER_H_

#include <d3d11.h>
#include <DirectXMath.h>

class SpecularShader
{
public:

	SpecularShader();
	SpecularShader(const SpecularShader&);
	~SpecularShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, DirectX::XMFLOAT3&, DirectX::XMMATRIX&, DirectX::XMMATRIX&, DirectX::XMMATRIX&, ID3D11ShaderResourceView*, DirectX::XMFLOAT3&, DirectX::XMFLOAT4&, DirectX::XMFLOAT4&, float, DirectX::XMFLOAT4&);

private:

	struct CameraBufferType
	{
		DirectX::XMFLOAT3 cameraPosition;
		float pad01;
	};

	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	struct LightBufferType
	{
		DirectX::XMFLOAT4 ambientColor;
		DirectX::XMFLOAT4 diffuseColor;
		DirectX::XMFLOAT3 lightDirection;
		float specularPower;
		DirectX::XMFLOAT4 specularColor;
	};

	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void CatchShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetUpVertexShaderParameters(ID3D11DeviceContext*, DirectX::XMFLOAT3&, DirectX::XMMATRIX&, DirectX::XMMATRIX&, DirectX::XMMATRIX&);
	bool SetUpPixelShaderParameters(ID3D11DeviceContext*, ID3D11ShaderResourceView*, DirectX::XMFLOAT3&, DirectX::XMFLOAT4&, DirectX::XMFLOAT4&, float, DirectX::XMFLOAT4&);
	void RenderShader(ID3D11DeviceContext*, int);

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11SamplerState* samplerState;

};

#endif