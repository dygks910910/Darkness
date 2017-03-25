#pragma once
#include "Vertex.h"
#include "LightHelper.h"
#include<string>
#include"Effects.h"
#include "Camera.h"

class CStaticBasicModel
{
protected:
	std::string mObjName;
	XMFLOAT4X4 mWorld;
	Material mObjMatrial;

	int mIndexCount;
	int mVertexOffset;
	int mIndexOffset;

	ID3D11ShaderResourceView* mTexSRV;

public:
	CStaticBasicModel(const XMFLOAT4X4& world,
		const Material& matrial,
		const int& indexCount,
		const int& vertexOffset,
		const int& indexOffset,
		ID3D11ShaderResourceView* texSRV,
		const char* name
	);
	~CStaticBasicModel();
	virtual void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMMATRIX& shadowTransform, const Camera& cam);
	void DrawToShadowMap(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMFLOAT4X4& lightView, const XMFLOAT4X4& lightProj);
public:
};

class CStaticNomalModel 
{
private:
	std::string mObjName;
	XMFLOAT4X4 mWorld;
	Material mObjMatrial;

	int mIndexCount;
	int mVertexOffset;
	int mIndexOffset;

	ID3D11ShaderResourceView* mTexSRV;
	ID3D11ShaderResourceView* mTexNormalSRV;

public:
	CStaticNomalModel(const XMFLOAT4X4& world,
		const Material& matrial,
		const int& indexCount,
		const int& vertexOffset,
		const int& indexOffset,
		ID3D11ShaderResourceView* texSRV,
		ID3D11ShaderResourceView* texNormalSRV,
		const char* name
		);
	~CStaticNomalModel();
	virtual void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMMATRIX& shadowTransform, const Camera& cam);
	void DrawToShadowMap(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMFLOAT4X4& lightView, const XMFLOAT4X4& lightProj);


public:
};

