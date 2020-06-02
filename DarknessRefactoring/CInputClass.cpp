#include "CInputClass.h"

CInputClass::CInputClass()
{
}

CInputClass::CInputClass(const CInputClass&)
{
}

CInputClass::~CInputClass()
{
}

void CInputClass::Initialize()
{
	for (int i = 0; i < 256; ++i)
	{
		m_bKeys[i] = false;
	}
}

void CInputClass::KeyDown(unsigned int input)
{
	m_bKeys[input] = true;
}

void CInputClass::KeyUp(unsigned int input)
{
	m_bKeys[input] = false;
}

bool CInputClass::IsKeyDown(unsigned int key)
{
	return m_bKeys[key];
}
