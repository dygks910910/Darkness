#include "Scene.h"

CScene::~CScene()
{
	ReleaseCOM(mDepthDisableState);
	ReleaseCOM(mDepthStencilState);
}

void CScene::ZbufferOn()
{
	mDc->OMSetDepthStencilState(mDepthStencilState, 1);
}

void CScene::ZbufferOff()
{
	mDc->OMSetDepthStencilState(mDepthDisableState, 1);
}

float CScene::AspectRatio() const
{
	return static_cast<float>(mClientWidth) / mClientHeight;
}
