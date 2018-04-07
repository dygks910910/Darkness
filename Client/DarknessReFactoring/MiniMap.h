#pragma once

#include "BitMap.h"
#include<xnamath.h>
#include "Effects.h"
#include "Global.h"
class CMiniMap
{
public:
	CMiniMap();
	CMiniMap(const CMiniMap& other);
	~CMiniMap();

	bool Initialize(XMMATRIX viewMatrix, float terrainWidth,
		float terrainHeight);
	void Shutdown();
	bool Render(const Camera& camera);

	void PositionUpdate(float positionX, float positionZ);
private:
	int m_mapLocationX, m_mapLocationY, m_pointLocationX, m_pointLocationY;
	float m_mapSizeX, m_mapSizeY, m_terrainWidth, m_terrainHeight;
	XMMATRIX m_viewMatrix;
	CBitMap *m_MiniMapBitmap,/* *m_Border, */*m_Point;
	XMMATRIX m_worldMtx;
	const int& MINIMAP_WIDTH = 200;
	const int& MINIMAP_HEIGHT = 250;

};

