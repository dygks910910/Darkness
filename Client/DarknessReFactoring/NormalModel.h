#pragma once
#include "BasicModel.h"
class NormalModel :
	public BasicModel
{
	ID3D11ShaderResourceView* mTexNormalSRV;
public:
	NormalModel();
	virtual ~NormalModel();
	virtual void Draw(const Camera & cam, ID3D11Buffer* vb, ID3D11Buffer* ib) override;

	void SetTextNormalSRV(ID3D11ShaderResourceView* srv) { mTexNormalSRV = srv; };
};

