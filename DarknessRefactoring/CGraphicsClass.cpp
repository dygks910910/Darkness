#include "CGraphicsClass.h"
#include"Define.h"
CGraphicsClass::CGraphicsClass():
	m_pD3d(nullptr)
	,m_Camera(nullptr)
	,m_Model(nullptr)
	,m_LightShader(nullptr)
	,m_Light(nullptr)
{
}
CGraphicsClass::CGraphicsClass(const CGraphicsClass&)
{
}

CGraphicsClass::~CGraphicsClass()
{
}

bool CGraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;


	// Create the Direct3D object.
	m_pD3d = new CD3dClass;
	IF_NOTX_RTFALSE(m_pD3d);
	
	// Initialize the Direct3D object.
	result = m_pD3d->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CCameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -6.0f);

	// Create the model object.
	m_Model = new CModelClass;
	IF_NOTX_RTFALSE(m_Model);

	// Initialize the model object.
	result = m_Model->Initialize(m_pD3d->GetDevice(),"data/cube.txt",L"data/seafloor.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
	// Create the color shader object.
	m_LightShader = new CLightShaderClass;
	IF_NOTX_RTFALSE(m_LightShader);

	// Initialize the color shader object.
	result = m_LightShader->Initialize(m_pD3d->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}


	m_textureShader = new CTextureShaderClass;
	IF_NOTX_RTFALSE(m_textureShader);

	// Initialize the color shader object.
	result = m_textureShader->Initialize(m_pD3d->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	m_Bitmap = new BitmapClass;
	IF_NOTX_RTFALSE(m_Bitmap);
	if (!m_Bitmap->Initialize(m_pD3d->GetDevice(), screenWidth, screenHeight, L"data/seafloor.dds", 256, 256))
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

	m_Light = new CLightClass;
	IF_NOTX_RTFALSE(m_Light);
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	//m_Light->SetAmbientColor(0,0,0, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);


	return true;
}

void CGraphicsClass::Shutdown()
{
	// Release the D3D object.
	SAFE_DELETE_SHUTDOWN(m_pD3d);
	SAFE_DELETE_SHUTDOWN(m_Model);
	SAFE_DELETE_SHUTDOWN(m_LightShader);
	SAFE_DELETE_SHUTDOWN(m_textureShader);
	SAFE_DELETE(m_Camera);
	SAFE_DELETE(m_Light);
	SAFE_DELETE(m_Bitmap);
}

bool CGraphicsClass::Frame()
{
	bool bResult;
	static float rotation = 0.0f;

	rotation += (float)D3DX_PI * 0.001f;
	if (rotation > 360.0f)
		rotation -= 360.0f;

	// Render the graphics scene.
	bResult = Render(rotation);
	IF_NOTX_RTFALSE(bResult);

	return true;
}

bool CGraphicsClass::Render(float rotation)
{
	// Clear the buffers to begin the scene.
	m_pD3d->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);
	bool result;

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, othMatrix;
	
	m_Camera->GetViewMatrix(viewMatrix);
	m_pD3d->GetWorldMatrix(worldMatrix);
	m_pD3d->GetProjectionMatrix(projectionMatrix);
	m_pD3d->GetOrthoMatrix(othMatrix);

	m_pD3d->TurnZBufferOff();
	

	if (!m_Bitmap->Render(m_pD3d->GetDeviceContext(), 100, 100))
		return false;
	if (!m_textureShader->Render(m_pD3d->GetDeviceContext(), m_Bitmap->GetIndexCount(),
		worldMatrix, viewMatrix, othMatrix, m_Bitmap->GetTexture()))
		return false;

	m_pD3d->TurnZBufferOn();

	worldMatrix = XMMatrixRotationY(rotation);
	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_pD3d->GetDeviceContext());

	// Render the model using the color shader.
	result = m_LightShader->Render(m_pD3d->GetDeviceContext(),
		m_Model->GetIndexCount(),
		worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(),
		m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(), m_Camera->GetPosition()
		, m_Light->GetSpecularColor(), m_Light->GetSpecularPower());

	IF_NOTX_RTFALSE(result);
	// Present the rendered scene to the screen.
	m_pD3d->EndScene();
	return true;
}
