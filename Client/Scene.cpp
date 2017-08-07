#include "Scene.h"

CScene::~CScene()
{
}


float CScene::AspectRatio() const
{
	return static_cast<float>(mClientWidth) / mClientHeight;
}
