#include "Scene.h"

CScene::~CScene()
{
// 	ReleaseCOM(mDepthDisableState);
// 	ReleaseCOM(mDepthStencilState);
}


float CScene::AspectRatio() const
{
	return static_cast<float>(mClientWidth) / mClientHeight;
}
