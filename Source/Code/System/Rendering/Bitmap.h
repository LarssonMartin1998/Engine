#ifndef _BITMAP_H_
#define _BITMAP_H_

#include <directxmath.h>
#include <Esent.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
class Texture;

class Bitmap
{
public:

	Bitmap();
	Bitmap(const Bitmap&);
	~Bitmap();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, int, int, char*, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int);

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
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
	void RenderBuffers(ID3D11DeviceContext*);

	void ReleaseTexture();
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int vertexCount;
	int indexCount;

	Texture* texture;

	int screenWidth;
	int screenHeight;

	int bitmapWidth;
	int bitmapHeight;

	int previousPosX;
	int previousPosY;
};

#endif