#include "CLightClass.h"

CLightClass::CLightClass()
{
}


CLightClass::CLightClass(const CLightClass& other)
{
}


CLightClass::~CLightClass()
{
}


void CLightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}


void CLightClass::SetDirection(float x, float y, float z)
{
	m_direction = D3DXVECTOR3(x, y, z);
	return;
}


D3DXVECTOR4 CLightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}


D3DXVECTOR3 CLightClass::GetDirection()
{
	return m_direction;
}