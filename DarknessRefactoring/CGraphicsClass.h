#pragma once
#include"CD3dClass.h"
#include"CCameraClass.h"
#include"CModelClass.h"
//#include"CColorShaderClass.h"
//#include"CTextureShaderClass.h"
#include"CLightShaderClass.h"
#include"CLightClass.h"
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


class CGraphicsClass
{
public:
	CGraphicsClass();
	CGraphicsClass(const CGraphicsClass&);
	~CGraphicsClass();
	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();
private:
	bool Render(float rotation);
private:
	CD3dClass* m_pD3d;
	CCameraClass* m_Camera;
	CModelClass* m_Model;
	CLightShaderClass* m_LightShader;
	CLightClass* m_Light;

};

