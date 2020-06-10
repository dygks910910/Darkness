#pragma once
#include<string>
#include"d3dx11effect.h"

#pragma region CEffect
class CEffect
{
public:
	CEffect(ID3D11Device* device, const std::wstring& filename);
	virtual ~CEffect();

private:
	CEffect(const CEffect& rhs);
	CEffect& operator=(const CEffect& rhs);

protected:
	ID3DX11Effect* mFX;
};
#pragma endregion

#pragma region LightEffect
class LightEffect : public CEffect
{
public:
	LightEffect(ID3D11Device* device, const std::wstring& filename);
	~LightEffect();

	ID3DX11EffectMatrixVariable* m_pWorld;
	ID3DX11EffectMatrixVariable* m_pView;
	ID3DX11EffectMatrixVariable* m_pProj;

	ID3DX11EffectVectorVariable* m_pCamPos;
	ID3DX11EffectVectorVariable* m_pAmbientColor;
	ID3DX11EffectVectorVariable* m_pDiffuseColor;
	ID3DX11EffectVectorVariable* m_pLightDir;
	ID3DX11EffectVectorVariable* m_pSpecularColor;
	ID3DX11EffectScalarVariable* m_pSpecularPower;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;

	void SetWorld(CXMMATRIX M) { m_pWorld->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetView(CXMMATRIX M) { m_pView->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetProj(CXMMATRIX M) { m_pProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	
	void SetCamPos(const XMFLOAT3& v) { m_pCamPos->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetLightDir(const XMFLOAT3& v) { m_pLightDir->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetAmbientColor(const FXMVECTOR v) { m_pAmbientColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	void SetSpecularColor(const FXMVECTOR v) { m_pSpecularColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	void SetDiffuseColor(const FXMVECTOR v) { m_pDiffuseColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	void SetSpecularPower(float f) { m_pSpecularPower->SetFloat(f); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex) { DiffuseMap->SetResource(tex); }

	ID3DX11EffectTechnique* LightTexTech;



};
#pragma endregion

#pragma region CEffects
class CEffects
{
public:
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static LightEffect* LightFX;
};
#pragma endregion

