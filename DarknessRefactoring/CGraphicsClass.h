#pragma once

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class D3DClass;
class CameraClass;
class ModelClass;
class LightShaderClass;
class LightClass;
class TextureShaderClass;
class BitmapClass;
class TextClass;
class ModelListClass;
class FrustumClass;
class CEffect;
class MultiTextureShaderClass;
class LightMapShaderClass;
class AlphaMapShaderClass;
class BumpMapShaderClass;
class SpecMapShaderClass;
class RenderTextureClass;
class DebugWindowClass;
class FogShaderClass;
class ClipPlaneShaderClass;
class TranslateShaderClass;
class TransparentShaderClass;
class ReflectionShaderClass;
class FadeShaderClass;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(float);
	bool Render();

private:
	bool RenderToTexture(float);
	bool RenderFadingScene();
	bool RenderNormalScene(float);

private:
	D3DClass* m_Direct3D = nullptr;
	CameraClass* m_Camera = nullptr;
	ModelClass* m_Model = nullptr;
	TextureShaderClass* m_TextureShader = nullptr;
	RenderTextureClass* m_RenderTexture = nullptr;
	BitmapClass* m_Bitmap = nullptr;
	FadeShaderClass* m_FadeShader = nullptr;
	float m_fadeInTime = 0;
	float m_accumulatedTime = 0;
	float m_fadePercentage = 0;
	bool m_fadeDone = false;
};
