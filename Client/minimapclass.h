////////////////////////////////////////////////////////////////////////////////
// Filename: minimapclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MINIMAPCLASS_H_
#define _MINIMAPCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "bitmapclass.h"
#include<xnamath.h>
#include "Effects.h"
////////////////////////////////////////////////////////////////////////////////
// Class name: MiniMapClass
////////////////////////////////////////////////////////////////////////////////
class MiniMapClass
{
public:
	MiniMapClass();
	MiniMapClass(const MiniMapClass&);
	~MiniMapClass();

	bool Initialize(ID3D11Device* device,
		int screenWidth, int screenHeight, XMMATRIX viewMatrix, float terrainWidth,
		float terrainHeight);
	void Shutdown();
	bool Render(ID3D11DeviceContext*deviceContext, const Camera& camera);
	
	void PositionUpdate(float, float);

private:
	int m_mapLocationX, m_mapLocationY, m_pointLocationX, m_pointLocationY;
	float m_mapSizeX, m_mapSizeY, m_terrainWidth, m_terrainHeight;
	XMMATRIX m_viewMatrix;
	BitmapClass *m_MiniMapBitmap, *m_Border, *m_Point;
	XMMATRIX m_worldMtx;
};

#endif