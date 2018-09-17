#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <directxmath.h>

class Light
{
public:

	Light();
	Light(const Light&);
	~Light();
	
	DirectX::XMFLOAT3 lightDirection;
	DirectX::XMFLOAT4 diffuseColor;
};

#endif