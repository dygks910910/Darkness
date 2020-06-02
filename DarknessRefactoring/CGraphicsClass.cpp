#include "CGraphicsClass.h"
CGraphicsClass::CGraphicsClass():
	m_pD3d(nullptr)
	,m_Camera(nullptr)
	,m_Model(nullptr)
	,m_ColorShader(nullptr)
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
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	// Create the model object.
	m_Model = new CModelClass;
	IF_NOTX_RTFALSE(m_Model);

	// Initialize the model object.
	result = m_Model->Initialize(m_pD3d->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create the color shader object.
	m_ColorShader = new CColorShaderClass;
	IF_NOTX_RTFALSE(m_ColorShader);

	// Initialize the color shader object.
	result = m_ColorShader->Initialize(m_pD3d->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void CGraphicsClass::Shutdown()
{
	// Release the D3D object.
	SAFE_DELETE_SHUTDOWN(m_pD3d);
	SAFE_DELETE_SHUTDOWN(m_Model);
	SAFE_DELETE_SHUTDOWN(m_ColorShader);
	SAFE_DELETE(m_Camera);
}

bool CGraphicsClass::Frame()
{
	bool bResult;


	// Render the graphics scene.
	bResult = Render();
	IF_NOTX_RTFALSE(bResult);
	return true;
}

bool CGraphicsClass::Render()
{
	// Clear the buffers to begin the scene.
	m_pD3d->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);
	bool result;

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_pD3d->GetWorldMatrix(worldMatrix);
	m_pD3d->GetProjectionMatrix(projectionMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_pD3d->GetDeviceContext());

	// Render the model using the color shader.
	result = m_ColorShader->Render(m_pD3d->GetDeviceContext(), m_Model->GetIndexCount(),
		worldMatrix, viewMatrix, projectionMatrix);
	IF_NOTX_RTFALSE(result);
	// Present the rendered scene to the screen.
	m_pD3d->EndScene();
	return true;
}
