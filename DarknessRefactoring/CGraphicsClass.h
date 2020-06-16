#pragma once

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class CD3dClass;
class CCameraClass;
class CModelClass;
class CLightShaderClass;
class CLightClass;
class CTextureShaderClass;
class BitmapClass;
class TextClass;
class ModelListClass;
class CFrustum;
class CEffect;

class CGraphicsClass
{
public:
	CGraphicsClass();
	CGraphicsClass(const CGraphicsClass&);
	~CGraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(int mouseX, int mouseY, int cpu, int fps, float frameTime, float rotateY);
	bool Render();
private:
	bool DebugRender();
	bool RenderTest();
private:
	CD3dClass* m_pD3d							 = nullptr;
	CCameraClass* m_Camera				= nullptr;
	CCameraClass* m_CameraTop			= nullptr;

	CModelClass* m_Model						 = nullptr;
	CLightShaderClass* m_LightShader = nullptr;
	CLightClass* m_Light						= nullptr;

	CTextureShaderClass* m_textureShader		 = nullptr;
	BitmapClass* m_Bitmap								 = nullptr;
	TextClass* m_Text											= nullptr;
	ModelListClass* m_ModelList = nullptr;
	CFrustum* m_Frustum = nullptr;
	static float m_Rotation;

};

