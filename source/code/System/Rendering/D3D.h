#ifndef _D3D_H_
#define _D3D_H_

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <directxmath.h>

#define _XM_NO_INTRINSICS_

__declspec(align(16)) class D3D
{
public:

	D3D();
	D3D(const D3D&);
	~D3D();

	// Overriding new and delete operator to guarantee 16-byte alignment.
	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}

	/////////////////////////////////////////////////////////////////////

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	/* 
	TODO:
	void BeginScene(Color); OVERLOAD FOR FUTURE COLOR CLASS
	
	*/
	void EndScene();

	ID3D11Device* GetDevice() { return device; }
	ID3D11DeviceContext* GetDeviceContext() { return deviceContext; }

	void GetProjectionMatri(DirectX::XMMATRIX& projectionMatrixRef) { projectionMatrixRef = projectionMatrix; }
	void GetWorldMatrix(DirectX::XMMATRIX& worldMatrixRef) { worldMatrixRef = worldMatrix; }
	void GetOrthographicMatrix(DirectX::XMMATRIX& orthographcMatrixRef) { orthographcMatrixRef = orthographicMatrix; }

	void TurnOnZbuffer();
	void TurnOffZbuffer();

	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();

	void GetVideoCardInfo(char*, int&);

private:

	bool vSyncEnabled;
	int videoCardMemory;
	char videoCardDescription[128];
	IDXGISwapChain* swapChain;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11RasterizerState* rasterState;
	DirectX::XMMATRIX projectionMatrix;
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX orthographicMatrix;

	ID3D11DepthStencilState* depthStencilStateDisabled;

	ID3D11BlendState* alphaEnabledBlendState;
	ID3D11BlendState* alphaDisabledBlendState;
};

#endif