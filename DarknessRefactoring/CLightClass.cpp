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
	m_diffuseColor = XMFLOAT4(red, green, blue, alpha);
	return;
}


void CLightClass::SetDirection(float x, float y, float z)
{
	m_direction = XMFLOAT3(x, y, z);
	return;
}


XMFLOAT4 CLightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}


XMFLOAT3 CLightClass::GetDirection()
{
	return m_direction;
}