#include "MiniMap.h"



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
	m_worldMtx = XMMatrixTranslation(0, 1, 9.5f);
	// 	m_worldMtx = XMMatrixIdentity();
	
	// Set the size of the mini-map.
	m_mapSizeX = 200;
	m_mapSizeY = 200;

	// Initialize the location of the mini-map on the screen.
	m_mapLocationX = 1500;
	m_mapLocationY = 800;

	
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
 L"Textures/minimapImage.PNG", 200, 200);
	if (!result)
	{
		MessageBox(0, L"Could not initialize the mini-map object.", L"Error", MB_OK);
		return false;
	}

	// Create the border bitmap object.
	//m_Border = new CBitMap;
// 	if (!m_Border)
// 	{
// 		return false;
// 	}
// 
// 	// Initialize the border bitmap object.
// 	result = m_Border->Initialize(device, screenWidth, screenHeight, L"Textures/border01.dds", 104, 104);
// 	if (!result)
// 	{
// 		MessageBox(0, L"Could not initialize the border object.", L"Error", MB_OK);
// 		return false;
// 	}

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

	// Release the border bitmap object.
// 	if (m_Border)
// 	{
// 		m_Border->Shutdown();
// 		delete m_Border;
// 		m_Border = 0;
// 	}

	// Release the mini-map bitmap object.
	if (m_MiniMapBitmap)
	{
		m_MiniMapBitmap->Shutdown();
		delete m_MiniMapBitmap;
		m_MiniMapBitmap = 0;
	}

	return;
}

bool CMiniMap::Render(ID3D11DeviceContext * deviceContext, const Camera & camera)
{
	bool result;
	// center Sky about eye in world space
	XMFLOAT3 eyePos = camera.GetPosition();

	XMMATRIX WVP = XMMatrixMultiply(m_worldMtx, m_worldMtx*camera.Proj()*camera.othMtx());
	Effects::BasicFX->SetWorldViewProj(WVP);
// 	Effects::BasicFX->SetDiffuseMap(m_Border->GetTexture());
// 	// Put the border bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
// 	result = m_Border->Render(deviceContext, (m_mapLocationX - 2), (m_mapLocationY - 2));
// 	if (!result)
// 	{
// 		return false;
// 	}
	D3DX11_TECHNIQUE_DESC techDesc;
	Effects::BasicFX->Light0TexTech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3DX11EffectPass* pass = Effects::BasicFX->Light0TexTech->GetPassByIndex(p);
		pass->Apply(0, deviceContext);

		deviceContext->DrawIndexed(6, 0, 0);
	}
	// Render the border bitmap using the texture shader.
	//textureShader->Render(deviceContext, m_Border->GetIndexCount(), worldMatrix, m_viewMatrix, orthoMatrix, m_Border->GetTexture());
	Effects::BasicFX->SetDiffuseMap(m_MiniMapBitmap->GetTexture());

	// Put the mini-map bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
	result = m_MiniMapBitmap->Render(deviceContext, m_mapLocationX, m_mapLocationY);
	if (!result)
	{
		return false;
	}
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3DX11EffectPass* pass = Effects::BasicFX->Light0TexTech->GetPassByIndex(p);
		pass->Apply(0, deviceContext);

		deviceContext->DrawIndexed(6, 0, 0);
	}
	// Render the mini-map bitmap using the texture shader.
	Effects::BasicFX->SetDiffuseMap(m_Point->GetTexture());

	// Put the point bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
	result = m_Point->Render(deviceContext, m_pointLocationX, m_pointLocationY);
	if (!result)
	{
		return false;
	}
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3DX11EffectPass* pass = Effects::BasicFX->Light0TexTech->GetPassByIndex(p);
		pass->Apply(0, deviceContext);

		deviceContext->DrawIndexed(6, 0, 0);
	}

	return true;
}

void CMiniMap::PositionUpdate(float positionX, float positionZ)
{
	float percentX, percentY;


	// Ensure the point does not leave the minimap borders even if
	//the camera goes past the terrain borders.
// 	if (positionX < 0)
// 	{
// 		positionX = 0;
// 	}
// 
// 	if (positionZ < 0)
// 	{
// 		positionZ = 0;
// 	}
// 
// 	if (positionX > m_terrainWidth)
// 	{
// 		positionX = m_terrainWidth;
// 	}
// 
// 	if (positionZ > m_terrainHeight)
// 	{
// 		positionZ = m_terrainHeight;
// 	}
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
