#pragma once
#include"Camera.h"
#include "Global.h"

class Model
{
protected:
	Material mObjMatrial;
	ID3D11Buffer* mInstanceBuffer;
	std::vector<XMFLOAT4X4> mInstanceWorld;

	int mIndexCount;
	int mVertexOffset;
	int mIndexOffset;

#if DEBUG | _DEBUG
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;
#endif
	ID3D11ShaderResourceView* mTexSRV;
public:
	Model();
	virtual ~Model();
	virtual void Draw(const Camera & cam, ID3D11Buffer* vb, ID3D11Buffer* ib) = 0;

	//getter
	const int& GetIdxCnt() { return mIndexCount; };
	const int& GetVtxOffset() { return mVertexOffset; };
	const int& GetIdxOffset() { return mIndexOffset; };
	ID3D11ShaderResourceView* GetTexSRV() { return mTexSRV; };
	//setter
	void SetIdxCnt(const int& i) { mIndexCount = i; };
	void SetVtxOffset(const int& i) { mVertexOffset = i; };
	void SetIdxOffset(const int& i) { mIndexOffset = i; };
	void SetTexture(ID3D11ShaderResourceView* i) 
	{
		mTexSRV = i;
	};

	void BuildInstanceBuffer();
	void AddInstance(const XMFLOAT4X4& otherWorld);


};

