#include "Scene.h"

float CScene::AspectRatio() const
{
	return static_cast<float>(mClientWidth) / mClientHeight;
}
