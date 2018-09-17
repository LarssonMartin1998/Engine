#ifndef _MODEL_H_
#define _MODEL_H_

#include <directxmath.h>

class ID3D11Device;
class ID3D11DeviceContext;
class ID3D11Buffer;
class ID3D11ShaderResourceView;

class Texture;

class Model
{
public:

	Model();
	Model(const Model&);
	~Model();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount() { return indexCount; }
	int GetVertexCount() { return vertexCount; }

	ID3D11ShaderResourceView* GetTexture();


private:

	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
	};

	bool InitializeBuffers(ID3D11Device*);
	void ReleaseBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int vertexCount;
	int indexCount;

	Texture* texture;

};

#endif