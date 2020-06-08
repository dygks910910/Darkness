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


class CGraphicsClass
{
public:
	CGraphicsClass();
	CGraphicsClass(const CGraphicsClass&);
	~CGraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(int mouseX, int mouseY, int cpu, int fps, float frameTime);
	bool Render();
private:
private:
	CD3dClass* m_pD3d = nullptr;
	CCameraClass* m_Camera = nullptr;
	CModelClass* m_Model = nullptr;
	CLightShaderClass* m_LightShader;
	CLightClass* m_Light;

	CTextureShaderClass* m_textureShader = nullptr;
	BitmapClass* m_Bitmap = nullptr;
	TextClass* m_Text = nullptr;

	static float m_Rotation;

};

