#pragma once
#include "BasicModel.h"

class ModelMgr;
class NormalModel :
	public BasicModel
{
	ID3D11ShaderResourceView* mTexNormalSRV;
public:
	NormalModel();
	virtual ~NormalModel();
	virtual void Draw(const Camera & cam, const ModelMgr* mgr) override;

	void SetTextNormalSRV(ID3D11ShaderResourceView* srv) { mTexNormalSRV = srv; };
};

