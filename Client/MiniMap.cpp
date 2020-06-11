#include "MiniMap.h"
#include<iostream>


CMiniMap::CMiniMap()
{
}

CMiniMap::CMiniMap(const CMiniMap & other)
{
}


CMiniMap::~CMiniMap()
{

}

bool CMiniMap::Initialize(ID3D11Device * device, int screenWidth, int screenHeight,
	XMMATRIX viewMatrix, float terrainWidth, float terrainHeight)
{
	bool result;
	//m_worldMtx = XMMatrixTranslation(0, 1, 9.5f);
	m_worldMtx = XMMatrixIdentity();
	// Set the size of the mini-map.
	

	m_mapSizeX = 200;
	m_mapSizeY = 200;
	// Initialize the location of the mini-map on the screen.
	m_mapLocationX = screenWidth- m_mapSizeX;
	m_mapLocationY = screenHeight - m_mapSizeY;

	

	// Store the base view matrix.
	m_viewMatrix = viewMatrix;

	// Store the terrain size.
	m_terrainWidth = terrainWidth;
	m_terrainHeight = terrainHeight;
	


	// Create the mini-map bitmap object.
	m_MiniMapBitmap = new CBitMap;
	if (!m_MiniMapBitmap)
	{
		return false;
	}

	// Initialize the mini-map bitmap object.
	result = m_MiniMapBitmap->Initialize(device, screenWidth, screenHeight,
 L"UITextures/minimapImage.PNG", 200,200);
	if (!result)
	{
		MessageBox(0, L"Could not initialize the mini-map object.", L"Error", MB_OK);
		return false;
	}


	// Create the point bitmap object.
	m_Point = new CBitMap;
	if (!m_Point)
	{
		return false;
	}
	// Initialize the point bitmap object.
	result = m_Point->Initialize(device, screenWidth, screenHeight, L"Textures/point01.dds", 7, 7);
	if (!result)
	{
		MessageBox(0, L"Could not initialize the point object.", L"Error", MB_OK);
		return false;
	}
	return true;
}

void CMiniMap::Shutdown()
{
	// Release the point bitmap object.
	if (m_Point)
	{
		m_Point->Shutdown();
		delete m_Point;
		m_Point = 0;
	}


	// Release the mini-map bitmap object.
	if (m_MiniMapBitmap)
	{
		m_MiniMapBitmap->Shutdown();
		delete m_MiniMapBitmap;
		m_MiniMapBitmap = 0;
	}

	return;
}

bool CMiniMap::Render(ID3D11DeviceContext * deviceContext, const Camera & camera, const int& scaleX, const int& scaleY)
{
	bool result;
	// center Sky about eye in world space
	XMFLOAT3 eyePos = camera.GetPosition();

	XMMATRIX WVP = XMMatrixMultiply(m_worldMtx, m_worldMtx* m_viewMatrix *camera.othMtx());
	Effects::TextureFX->SetWorldViewProj(WVP);
	Effects::TextureFX->SetDiffuseMap(m_MiniMapBitmap->GetTexture());

	D3DX11_TECHNIQUE_DESC techDesc;
	Effects::TextureFX->Tech->GetDesc(&techDesc);

	result = m_Point->Render(deviceContext, m_pointLocationX, m_pointLocationY, false);
	if (!result)
	{
		return false;
	}
	// Put the mini-map bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
	result = m_MiniMapBitmap->Render(deviceContext, m_mapLocationX, m_mapLocationY, false,scaleX, scaleY);
	if (!result)
	{
		return false;
	}
	// Put the point bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
	

	return true;
}

void CMiniMap::PositionUpdate(float positionX, float positionZ)
{
	float percentX, percentY;



	// Calculate the position of the camera on the minimap in terms of percentage.
	percentX =  positionX / m_terrainWidth;
	percentY = (positionZ / m_terrainHeight);

	// Determine the pixel location of the point on the mini-map.
	m_pointLocationX = m_mapSizeX/2+ m_mapLocationX + (int)-(percentX * m_mapSizeX) ;
	m_pointLocationY = m_mapSizeY/2 +m_mapLocationY + (int)(percentY * m_mapSizeY);

	// Subtract one from the location to center the point on the mini-map according to the 3x3 point pixel image size.
	m_pointLocationX = m_pointLocationX - 1;
	m_pointLocationY = m_pointLocationY - 1;

	return;
}
