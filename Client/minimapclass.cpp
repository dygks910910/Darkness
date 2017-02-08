////////////////////////////////////////////////////////////////////////////////
// Filename: minimapclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "minimapclass.h"


MiniMapClass::MiniMapClass()
{
	m_MiniMapBitmap = 0;
	m_Border = 0;
	m_Point = 0;
}


MiniMapClass::MiniMapClass(const MiniMapClass& other)
{
}


MiniMapClass::~MiniMapClass()
{
}


bool MiniMapClass::Initialize(ID3D11Device* device,
	int screenWidth, int screenHeight, XMMATRIX viewMatrix, float terrainWidth,
	float terrainHeight)
{
	bool result;
	m_worldMtx = XMMatrixTranslation(0, 8, 120);
	// Initialize the location of the mini-map on the screen.
	m_mapLocationX = 150;
	m_mapLocationY = 75;

	// Set the size of the mini-map.
	m_mapSizeX = 150.0f;
	m_mapSizeY = 150.0f;

	// Store the base view matrix.
	m_viewMatrix = viewMatrix;

	// Store the terrain size.
	m_terrainWidth = terrainWidth;
	m_terrainHeight = terrainHeight;

	// Create the mini-map bitmap object.
	m_MiniMapBitmap = new BitmapClass;
	if(!m_MiniMapBitmap)
	{
		return false;
	}

	// Initialize the mini-map bitmap object.
	result = m_MiniMapBitmap->Initialize(device, screenWidth, screenHeight, L"colorm01.dds", 150, 150);
	if(!result)
	{
		MessageBox(0, L"Could not initialize the mini-map object.", L"Error", MB_OK);
		return false;
	}

	// Create the border bitmap object.
	m_Border = new BitmapClass;
	if(!m_Border)
	{
		return false;
	}

	// Initialize the border bitmap object.
	result = m_Border->Initialize(device, screenWidth, screenHeight, L"border01.dds", 154, 154);
	if(!result)
	{
		MessageBox(0, L"Could not initialize the border object.", L"Error", MB_OK);
		return false;
	}

	// Create the point bitmap object.
	m_Point = new BitmapClass;
	if(!m_Point)
	{
		return false;
	}
	// Initialize the point bitmap object.
	result = m_Point->Initialize(device, screenWidth, screenHeight, L"point01.dds", 3, 3);
	if(!result)
	{
		MessageBox(0, L"Could not initialize the point object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void MiniMapClass::Shutdown()
{
	// Release the point bitmap object.
	if(m_Point)
	{
		m_Point->Shutdown();
		delete m_Point;
		m_Point = 0;
	}

	// Release the border bitmap object.
	if(m_Border)
	{
		m_Border->Shutdown();
		delete m_Border;
		m_Border = 0;
	}

	// Release the mini-map bitmap object.
	if(m_MiniMapBitmap)
	{
		m_MiniMapBitmap->Shutdown();
		delete m_MiniMapBitmap;
		m_MiniMapBitmap = 0;
	}

	return;
}


bool MiniMapClass::Render(ID3D11DeviceContext* deviceContext,const Camera& camera)
{
	bool result;
	// center Sky about eye in world space
	XMFLOAT3 eyePos = camera.GetPosition();
	
	

	XMMATRIX WVP = XMMatrixMultiply(m_worldMtx, m_worldMtx*camera.ViewProj());
	Effects::BasicFX->SetWorldViewProj(WVP);
	Effects::BasicFX->SetDiffuseMap(m_Border->GetTexture());
	// Put the border bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
	result = m_Border->Render(deviceContext, (m_mapLocationX - 2), (m_mapLocationY - 2),camera);
	if(!result)
	{
		return false;
	}

	// Render the border bitmap using the texture shader.
	//textureShader->Render(deviceContext, m_Border->GetIndexCount(), worldMatrix, m_viewMatrix, orthoMatrix, m_Border->GetTexture());
	Effects::BasicFX->SetDiffuseMap(m_MiniMapBitmap->GetTexture());

	// Put the mini-map bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
	result = m_MiniMapBitmap->Render(deviceContext, m_mapLocationX, m_mapLocationY, camera);
	if(!result)
	{
		return false;
	}

	// Render the mini-map bitmap using the texture shader.
	//textureShader->Render(deviceContext, m_MiniMapBitmap->GetIndexCount(), worldMatrix, m_viewMatrix, orthoMatrix, m_MiniMapBitmap->GetTexture());
	Effects::BasicFX->SetDiffuseMap(m_Point->GetTexture());

	// Put the point bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
	result = m_Point->Render(deviceContext, m_pointLocationX, m_pointLocationY, camera);
	if(!result)
	{
		return false;
	}

	// Render the point bitmap using the texture shader.
	//textureShader->Render(deviceContext, m_Point->GetIndexCount(), worldMatrix, m_viewMatrix, orthoMatrix, m_Point->GetTexture());

	return true;
}


void MiniMapClass::PositionUpdate(float positionX, float positionZ)
{
	float percentX, percentY;


	// Ensure the point does not leave the minimap borders even if the camera goes past the terrain borders.
	if(positionX < 0)
	{
		positionX = 0;
	}

	if(positionZ < 0)
	{
		positionZ = 0;
	}

	if(positionX > m_terrainWidth)
	{
		positionX = m_terrainWidth;
	}

	if(positionZ > m_terrainHeight)
	{
		positionZ = m_terrainHeight;
	}

	// Calculate the position of the camera on the minimap in terms of percentage.
	percentX = positionX / m_terrainWidth;
	percentY = 1.0f - (positionZ / m_terrainHeight);

	// Determine the pixel location of the point on the mini-map.
	m_pointLocationX = m_mapLocationX + (int)(percentX * m_mapSizeX);
	m_pointLocationY = m_mapLocationY + (int)(percentY * m_mapSizeY);

	// Subtract one from the location to center the point on the mini-map according to the 3x3 point pixel image size.
	m_pointLocationX = m_pointLocationX - 1;
	m_pointLocationY = m_pointLocationY - 1;

	return;
}