#pragma once
class CInputLayouts
{
	CInputLayouts();
	~CInputLayouts();
public:
	static void InitInputLayouts(ID3D11Device* device);
	static void Destroy();

	static  ID3D11InputLayout* m_LightBasic;

};

class CInputLayoutDesc
{
public:
	static const D3D11_INPUT_ELEMENT_DESC LightBasic[3];
};
