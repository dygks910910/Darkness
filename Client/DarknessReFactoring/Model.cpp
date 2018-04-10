#include "Model.h"



Model::Model()
{
	mInstanceBuffer = nullptr;
#if DEBUG | _DEBUG
	mBoxVB = nullptr;
	mBoxIB = nullptr;
#endif
	
}


Model::~Model()
{
	ReleaseCOM(mInstanceBuffer);
#if DEBUG | _DEBUG
	ReleaseCOM(mBoxVB);
	ReleaseCOM(mBoxIB);
#endif
}

void Model::BuildInstanceBuffer()
{
	D3D11_BUFFER_DESC basicvbd;
	basicvbd.Usage = D3D11_USAGE_IMMUTABLE;
	basicvbd.ByteWidth = sizeof(XMFLOAT4X4) * mInstanceWorld.size();
	basicvbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	basicvbd.CPUAccessFlags = 0;
	basicvbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA basicvinitData;
	basicvinitData.pSysMem = &mInstanceWorld[0];
	HR(md3dDevice->CreateBuffer(&basicvbd, &basicvinitData, &mInstanceBuffer));
}

void Model::AddInstance(const XMFLOAT4X4 & otherWorld)
{
	mInstanceWorld.push_back(otherWorld);
	ReleaseCOM(mInstanceBuffer);
	BuildInstanceBuffer();
}
