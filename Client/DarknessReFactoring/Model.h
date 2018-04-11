#pragma once
#include"Camera.h"
#include "Global.h"
#include "struct.h"

//Ŭ������ ���������� �ذ�.����� ����,�ҽ����Ͽ��� include
class ModelMgr;

class Model
{
protected:
	ID3D11Buffer* mInstanceBuffer;
	ModelInfo m_Info;

#if DEBUG | _DEBUG
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;
#endif
	ID3D11ShaderResourceView* mTexSRV;
public:
	Model();
	virtual ~Model();
	virtual void Draw(const Camera & cam, const ModelMgr* mgr) = 0;
	//getter
	ID3D11ShaderResourceView* GetTexSRV() { return mTexSRV; };
	const ModelInfo& GetInfo() { return m_Info; }
	//setter
	void SetTexture(ID3D11ShaderResourceView* i) {mTexSRV = i;};
	void SetInfo(const ModelInfo& info) { m_Info = info; }
	void BuildInstanceBuffer();
	void AddInstance(const XMFLOAT4X4& otherWorld);


};

