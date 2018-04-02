#pragma once
#include "Vertex.h"
#include "LightHelper.h"
#include<string>
#include"Effects.h"
#include "Camera.h"
#include "xnacollision.h"
#include "GeometryGenerator.h"
#include "RenderStates.h"


class CStaticBasicModel
{
protected:
	XMFLOAT4X4 mWorld;
	Material mObjMatrial;

	int mIndexCount;
	int mVertexOffset;
	int mIndexOffset;
	XNA::OrientedBox mBox;
	
#if DEBUG | _DEBUG
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;
#endif
	ID3D11ShaderResourceView* mTexSRV;
public:
#if DEBUG|_DEBUG
	void DrawBox(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const Camera& cam);
#endif // DEBUG|_DEBUG

public:
	CStaticBasicModel(const XMFLOAT4X4& world,
		const Material& matrial,
		const int& indexCount,
		const int& vertexOffset,
		const int& indexOffset,
		ID3D11ShaderResourceView* texSRV,
		const char* name
	);
	//orientedBox�߰�����.
	CStaticBasicModel(const XMFLOAT4X4& world,
		const Material& matrial,
		const int& indexCount,
		const int& vertexOffset,
		const int& indexOffset,
		ID3D11ShaderResourceView* texSRV,
		const char* name,
		const XNA::OrientedBox& box,
		ID3D11Device* device
	);
	~CStaticBasicModel();
	virtual void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMFLOAT4X4& shadowTransform, const Camera& cam);

	virtual void DrawToShadowMap(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMFLOAT4X4& lightView, const XMFLOAT4X4& lightProj);
	void DrawSsao(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMMATRIX& shadowTransform, const Camera& cam);



	ID3D11Buffer* GetBoxVB() { return mBoxVB; }
	ID3D11Buffer* GetBoxIB() { return mBoxIB; }

public:
	std::string mObjName;

};

class CStaticNomalModel : public CStaticBasicModel
{
private:
	
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
	virtual void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMFLOAT4X4& shadowTransform, const Camera& cam);
	virtual void DrawToShadowMap(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMFLOAT4X4& lightView, const XMFLOAT4X4& lightProj);

	void DrawSsao(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMMATRIX& shadowTransform, const Camera& cam);

public:
};

class CInstanceBasicModel
{
	protected:
	std::string mName;
	ID3D11Buffer* mInstanceBuffer;
	int mIndexCount;
	int mVertexOffset;
	int mIndexOffset;
	Material mMatrial;
	ID3D11ShaderResourceView* mTexSRV;
	std::vector<XMFLOAT4X4> mInstanceWorld;
public:
	CInstanceBasicModel(const Material& matrial,
		const int& indexCount,
		const int& vertexOffset,
		const int& indexOffset,
		ID3D11ShaderResourceView* texSRV,
		const char* name,
		ID3D11Device* device);
	CInstanceBasicModel();
	~CInstanceBasicModel();
	void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMFLOAT4X4& shadowTransform, const Camera& cam);
	void DrawToShadowMap(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMFLOAT4X4& lightView, const XMFLOAT4X4& lightProj);
	
	//�ν��Ͻ� ��������� �߰�.
	void AddInstanceWorld(const XMFLOAT4X4& mtx) { mInstanceWorld.push_back(mtx); };
	//���۸� ����.
	void BuildInstanceBuffer(ID3D11Device* device);
	//indexCount,vertexOffset,indexOffset �� ����.
	void SetDrawInfomation(const int& indexCount, const int& vertexOffset, const int& indexOffset);
	void SetSRV(ID3D11ShaderResourceView* srv) { mTexSRV = srv; }
	void SetMatrial(const Material& matrial) { mMatrial = matrial; }

	ID3D11Buffer* GetInstanceBuffer() { return mInstanceBuffer; }
	void Destroy();
};
