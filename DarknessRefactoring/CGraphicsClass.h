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
class RefractionShaderClass;
class WaterShaderClass;
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();
	bool Render();

private:

private:
	D3DClass* m_Direct3D = nullptr;
	CameraClass* m_Camera = nullptr;
	ModelClass* m_Model = nullptr;
	LightShaderClass* m_LightShader = nullptr;
	LightClass* m_Light1 = nullptr;
	LightClass* m_Light2 = nullptr;
	LightClass* m_Light3 = nullptr;
	LightClass* m_Light4 = nullptr;
};
