#include "stdafx.h"
#include "CEffect.h"
#include<fstream>
#include<vector>
CEffect::CEffect(ID3D11Device* device, const std::wstring& filename)
	: mFX(0)
{
	/*std::ifstream fin(filename, std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();*/

	
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	ID3DBlob* pErrorBlob = nullptr;
	HR(D3DX11CompileEffectFromFile(filename.c_str(), nullptr, 
		D3D_COMPILE_STANDARD_FILE_INCLUDE, dwShaderFlags, 0, device, &mFX, &pErrorBlob));

	//HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size,
	//	0, device, &mFX));
}

CEffect::~CEffect()
{
	SAFE_RELEASE_D3D(mFX);
}

LightEffect::LightEffect(ID3D11Device* device, const std::wstring& filename)
	:CEffect(device,filename)
{
	LightTexTech = mFX->GetTechniqueByName("Light1");

	m_pWorld = mFX->GetVariableByName("worldMatrix")->AsMatrix();
	m_pView = mFX->GetVariableByName("viewMatrix")->AsMatrix();
	m_pProj = mFX->GetVariableByName("projectionMatrix")->AsMatrix();

	m_pCamPos = mFX->GetVariableByName("cameraPosition")->AsVector();
	m_pAmbientColor = mFX->GetVariableByName("ambientColor")->AsVector();
	m_pDiffuseColor = mFX->GetVariableByName("diffuseColor")->AsVector();
	m_pLightDir = mFX->GetVariableByName("lightDirection")->AsVector();
	m_pSpecularColor = mFX->GetVariableByName("specularColor")->AsVector();
	m_pSpecularPower = mFX->GetVariableByName("specularPower")->AsScalar();

	DiffuseMap = mFX->GetVariableByName("shaderTexture")->AsShaderResource();








}

LightEffect::~LightEffect()
{
}

LightEffect* CEffects::LightFX = nullptr;
void CEffects::InitAll(ID3D11Device* device)
{
	LightFX = new LightEffect(device, L"Light.hlsl");

}

void CEffects::DestroyAll()
{
	SAFE_DELETE(LightFX);
}
