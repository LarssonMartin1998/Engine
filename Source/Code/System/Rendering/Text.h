#ifndef _TEXT_H_
#define _TEXT_H_

#include <windows.h>
#include <directxmath.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
class Font;
class FontShader;

class Text
{
public:

	Text();
	Text(const Text&);
	~Text();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, DirectX::XMMATRIX&);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, DirectX::XMMATRIX&, DirectX::XMMATRIX&);

private:

	struct SentenceType
	{
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;
		int vertexCount;
		int indexCount;
		int maxLength;
		float red;
		float green;
		float blue;
	};

	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
	};

	bool InitializeSentence(SentenceType**, int, ID3D11Device*);
	bool UpdateSentence(SentenceType*, char*, int, int, float, float, float, ID3D11DeviceContext*);
	void ReleaseSentence(SentenceType**);
	bool RenderSentence(ID3D11DeviceContext*, SentenceType*, DirectX::XMMATRIX&, DirectX::XMMATRIX&);

	Font* font;
	FontShader* fontShader;
	
	int screenWidth;
	int screenHeight;

	DirectX::XMMATRIX baseViewMatrix;

	SentenceType* sentence1;
	SentenceType* sentence2;
};

#endif