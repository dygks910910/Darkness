#pragma once

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class D3DClass;
class CameraClass;
class ModelClass;
class CLightShaderClass;
class LightClass;
class CTextureShaderClass;
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
	D3DClass* m_Direct3D = nullptr;
	CameraClass* m_Camera = nullptr;
	ModelClass* m_Model = nullptr;
	SpecMapShaderClass* m_SpecMapShader = nullptr;
	LightClass* m_Light = nullptr;

};
