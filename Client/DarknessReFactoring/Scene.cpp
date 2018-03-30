#include "Scene.h"



Scene::Scene()
{
}


Scene::~Scene()
{
}

float Scene::AspectRatio()
{
	return static_cast<float>(mClientWidth) / mClientHeight;
}
