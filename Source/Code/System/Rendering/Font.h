#ifndef _FONT_H_
#define _FONT_H_

#include <directxmath.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;
class Texture;

class Font
{
public:

	Font();
	Font(const Font&);
	~Font();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*, char*);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int);

	void BuildVertexArray(void*, char*, float, float);

	ID3D11ShaderResourceView* GetTexture();

private:

	struct FontType
	{
		float left;
		float right;
		int size;
	};

	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
	};
	bool LoadFontData(char*);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

	FontType* font;
	Texture* texture;
};

#endif