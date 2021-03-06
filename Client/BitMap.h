#pragma once
#include <d3d11.h>
#include <d3dx10math.h>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include"Texture.h"
#include "Effects.h"
#include "Common/Camera.h"
#include "Vertex.h"
#include "Common/xnacollision.h"

class CBitMap
{
public:
	CBitMap();
	CBitMap(const CBitMap& other);
	~CBitMap();
	bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight,
		WCHAR* textureFilename, int bitmapWidth, int bitmapHeight);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext,

		int positionX, int positionY, bool isFullScreen,const float& scaleX = 1.0f, const float& scaleY = 1.0f);
	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
	XNA::AxisAlignedBox mMeshBox;

	void GetPos(int& x, int& y) { x = m_previousPosX; y = m_previousPosY; };
	void GetBmpWidth(int& x) { x = m_bitmapWidth*mScaleX; };
	void GetBmpHeight(int& x) { x = m_bitmapHeight * mScaleY; };
	float mScaleX = 0;
	float mScaleY = 0;

private:
private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY, bool isFullscreen);
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

	bool LoadTexture(ID3D11Device* device, WCHAR* filename);
	void ReleaseTexture();

	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	CTexture* m_Texture;
	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;
};