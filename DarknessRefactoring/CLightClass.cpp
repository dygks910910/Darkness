#include "stdafx.h"
#include "CLightClass.h"


LightClass::LightClass()
{
}


LightClass::LightClass(const LightClass& other)
{
}


LightClass::~LightClass()
{
}


void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = XMFLOAT4(red, green, blue, alpha);
}

void LightClass::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT4(x, y, z, 1.0f);
}


XMFLOAT4 LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}


XMFLOAT4 LightClass::GetPosition()
{
	return m_position;
}