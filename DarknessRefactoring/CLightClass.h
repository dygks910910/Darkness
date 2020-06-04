#pragma once
#include <d3dx10math.h>
class CLightClass
{
public:
	CLightClass();
	CLightClass(const CLightClass&);
	~CLightClass();

	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);

	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR3 GetDirection();

private:
	D3DXVECTOR4 m_diffuseColor;
	D3DXVECTOR3 m_direction;
};

