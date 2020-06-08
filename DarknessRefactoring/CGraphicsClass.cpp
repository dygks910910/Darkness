#include"stdafx.h"
#include "CGraphicsClass.h"
#include"CD3dClass.h"
#include"CCameraClass.h"
#include"CModelClass.h"
#include"CColorShaderClass.h"
#include"CTextureShaderClass.h"
#include"CLightShaderClass.h"
#include"CLightClass.h"
#include"BitmapClass.h"
#include"TextClass.h"
#include"ModelListClass.h"
#include"FrustumClass.h"
float CGraphicsClass::m_Rotation = 0;

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
	//수직동기화 사용.
	//result = m_pD3d->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	//수직동기화 사용안함
	result = m_pD3d->Initialize(screenWidth, screenHeight, !VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);



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
	m_CameraTop = new CCameraClass;
	if (!m_CameraTop)
	{
		return false;
	}
	m_CameraTop->SetPosition(0.0f, 6.0f, -10.0f);
	m_CameraTop->Render(XMFLOAT3(0,-0.5f,0.5f));
	// Create the model object.
	m_Model = new CModelClass;
	IF_NOTX_RTFALSE(m_Model);

	// Initialize the model object.
	result = m_Model->Initialize(m_pD3d->GetDevice(),"data/sphere.txt",L"data/seafloor.dds");
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

	XMMATRIX baseViewMatrix;
	//m_Camera->SetPosition(0, 0, -1.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	m_Text = new TextClass;
	IF_NOTX_RTFALSE(m_Text);
	if (!m_Text->Initialize(m_pD3d->GetDevice(),m_pD3d->GetDeviceContext(),
		hwnd,screenWidth,screenHeight, baseViewMatrix))
	{
		MessageBox(hwnd, L"Could not initialize the Text object.", L"Error", MB_OK);
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

	m_ModelList = new ModelListClass;
	IF_NOTX_RTFALSE(m_ModelList);
	if (!m_ModelList->Initialize(25))
	{
		MessageBox(hwnd, L"Could not initialize the ModelList object.", L"Error", MB_OK);
		return false;
	}

	m_Frustum = new FrustumClass;
	IF_NOTX_RTFALSE(m_Frustum);

	return true;
}

void CGraphicsClass::Shutdown()
{
	// Release the D3D object.
	SAFE_DELETE_SHUTDOWN(m_pD3d);
	SAFE_DELETE_SHUTDOWN(m_Model);
	SAFE_DELETE_SHUTDOWN(m_LightShader);
	SAFE_DELETE_SHUTDOWN(m_textureShader);
	SAFE_DELETE_SHUTDOWN(m_Text);
	SAFE_DELETE(m_Camera);
	SAFE_DELETE(m_Light);
	SAFE_DELETE(m_Bitmap);
	SAFE_DELETE_SHUTDOWN(m_Text);
	SAFE_DELETE(m_CameraTop);
	SAFE_DELETE_SHUTDOWN(m_ModelList);
	SAFE_DELETE(m_Frustum);

}

bool CGraphicsClass::Frame(int mouseX, int mouseY, int cpu, int fps, float frameTime, float rotateY)
{
	bool bResult = false;

	m_Rotation += (float)XM_PI * 0.001f * frameTime;
	if (m_Rotation > 360.0f)
		m_Rotation -= 360.0f;

	// Render the graphics scene.
	//bResult = Render();
	//IF_NOTX_RTFALSE(bResult);
	if (!m_Text->SetMousePosition(mouseX, mouseY, m_pD3d->GetDeviceContext()))
		return false;
	if (!m_Text->SetCpu(cpu, m_pD3d->GetDeviceContext()))
		return false;
	if (!m_Text->SetFps(fps, m_pD3d->GetDeviceContext()))
		return false;

	m_Camera->SetPosition(0, 0, -20.0f);
	m_Camera->SetRotation(0, rotateY, 0);
	m_CameraTop->SetPosition(0.0f, 6.0f, -20.0f);

	return true;
}

bool CGraphicsClass::Render()
{

	float positionX = 0;
	float positionY = 0;
	float positionZ = 0;
	float radius= 1.0f;
	XMFLOAT4 color;
	// Clear the buffers to begin the scene.
	m_pD3d->BeginScene(0, 0,0, 1.0f);
	bool result;

	// Generate the view matrix based on the camera's position.
	m_pD3d->SetFirstViewport();
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, othMatrix;
	
	m_Camera->GetViewMatrix(viewMatrix);
	m_pD3d->GetWorldMatrix(worldMatrix);
	m_pD3d->GetProjectionMatrix(projectionMatrix);
	m_pD3d->GetOrthoMatrix(othMatrix);

	m_pD3d->TurnZBufferOff();
	m_pD3d->TurnOnAlphaBlending();

	
	
	if (!m_Text->Render(m_pD3d->GetDeviceContext(), worldMatrix, othMatrix))
		return false;

	/*if (!m_Bitmap->Render(m_pD3d->GetDeviceContext(), 100,100))
		return false;
	if (!m_textureShader->Render(m_pD3d->GetDeviceContext(), m_Bitmap->GetIndexCount(),
		worldMatrix, viewMatrix, othMatrix, m_Bitmap->GetTexture()))
		return false;*/

	m_pD3d->TurnOffAlphaBlending();
	m_pD3d->TurnZBufferOn();

	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);
	int nModelCount = m_ModelList->GetModelCount();
	int renderCount = 0;

	for (int index = 0; index < nModelCount; ++index)
	{
		m_ModelList->GetData(index, positionX, positionY, positionZ, color);
		if (m_Frustum->CheckSphere(positionX, positionY, positionZ, radius))
		{
			worldMatrix = XMMatrixTranslation(positionX, positionY, positionZ);
			m_Model->Render(m_pD3d->GetDeviceContext());
			m_LightShader->Render(m_pD3d->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
				m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(),
				m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
			m_pD3d->GetWorldMatrix(worldMatrix);
			renderCount++;
		}
	}
	worldMatrix = XMMatrixRotationY(m_Rotation);
	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	
	// Render the model using the color shader.
	result = m_LightShader->Render(m_pD3d->GetDeviceContext(),
		m_Model->GetIndexCount(),
		worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(),
		m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(), m_Camera->GetPosition()
		, m_Light->GetSpecularColor(), m_Light->GetSpecularPower());

#ifdef _DEBUG
	DebugRender();
#endif

	IF_NOTX_RTFALSE(result);

	// Present the rendered scene to the screen.
	m_pD3d->EndScene();
	return true;
}

bool CGraphicsClass::DebugRender()
{
	float positionX = 0;
	float positionY = 0;
	float positionZ = 0;
	float radius = 1.0f;
	XMFLOAT4 color;

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, othMatrix;

	m_Camera->GetViewMatrix(viewMatrix);
	m_pD3d->GetWorldMatrix(worldMatrix);
	m_pD3d->GetProjectionMatrix(projectionMatrix);
	m_pD3d->GetOrthoMatrix(othMatrix);

	m_pD3d->SetSecondViewport();
	//worldMatrix = XMMatrixRotationY(m_Rotation);
	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_pD3d->GetDeviceContext());
	bool result = true;

	m_CameraTop->Render();
	// Render the model using the color shader.
	m_CameraTop->GetViewMatrix(viewMatrix);
	worldMatrix = XMMatrixRotationY(m_Rotation);
	result = m_LightShader->Render(m_pD3d->GetDeviceContext(),
		m_Model->GetIndexCount(),
		worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(),
		m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(), m_CameraTop->GetPosition()
		, m_Light->GetSpecularColor(), m_Light->GetSpecularPower());

	IF_NOTX_RTFALSE(result);
	int nModelCount = m_ModelList->GetModelCount();

	int renderCount = 0;
	m_Model->Render(m_pD3d->GetDeviceContext());
	for (int index = 0; index < nModelCount; ++index)
	{
		m_ModelList->GetData(index, positionX, positionY, positionZ, color);
		if (m_Frustum->CheckCube(positionX, positionY, positionZ, radius))
		{
			worldMatrix = XMMatrixTranslation(positionX, positionY, positionZ);
			m_Model->Render(m_pD3d->GetDeviceContext());
			result = m_LightShader->Render(m_pD3d->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
				m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(),
				m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
			m_pD3d->GetWorldMatrix(worldMatrix);
			IF_NOTX_RTFALSE(result);
			renderCount++;
		}
	}
	return true;
}

bool TextClass::SetMousePosition(int mouseX, int mouseY, ID3D11DeviceContext* deviceContext)
{
	// mouseX 정수를 문자열 형식으로 변환합니다.
	char tempString[16] = { 0, };
	_itoa_s(mouseX, tempString, 10);

	// mouseX 문자열을 설정합니다.
	char mouseString[16] = { 0, };
	strcpy_s(mouseString, "Mouse X: ");
	strcat_s(mouseString, tempString);

	// 문장 정점 버퍼를 새 문자열 정보로 업데이트합니다.
	if (!UpdateSentence(m_sentence1, mouseString, 20, 20, 1.0f, 1.0f, 1.0f, deviceContext))
	{
		return false;
	}

	// mouseY 정수를 문자열 형식으로 변환합니다.
	_itoa_s(mouseY, tempString, 10);

	// mouseY 문자열을 설정합니다.
	strcpy_s(mouseString, "Mouse Y: ");
	strcat_s(mouseString, tempString);

	// 문장 정점 버퍼를 새 문자열 정보로 업데이트합니다.
	if (!UpdateSentence(m_sentence2, mouseString, 20, 40, 1.0f, 1.0f, 1.0f, deviceContext))
	{
		return false;
	}

	return true;
}

bool TextClass::SetFps(int fps, ID3D11DeviceContext* deviceContext)
{
	// fps를 10,000 이하로 자릅니다.
	if (fps > 9999)
	{
		fps = 9999;
	}

	// fps 정수를 문자열 형식으로 변환합니다.
	char tempString[16] = { 0, };
	_itoa_s(fps, tempString, 10);

	// fps 문자열을 설정합니다.
	char fpsString[16] = { 0, };
	strcpy_s(fpsString, "Fps: ");
	strcat_s(fpsString, tempString);

	float red = 0;
	float green = 0;
	float blue = 0;

	// fps가 60 이상이면 fps 색상을 녹색으로 설정합니다.
	if (fps >= 60)
	{
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	// fps가 60보다 작은 경우 fps 색상을 노란색으로 설정합니다.
	if (fps < 60)
	{
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	// fps가 30 미만이면 fps 색상을 빨간색으로 설정합니다.
	if (fps < 30)
	{
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}

	// 문장 정점 버퍼를 새 문자열 정보로 업데이트합니다.
	return UpdateSentence(m_sentence3, fpsString, 20, 60, red, green, blue, deviceContext);
}

bool TextClass::SetCpu(int cpu, ID3D11DeviceContext* deviceContext)
{
	// cpu 정수를 문자열 형식으로 변환합니다.
	char tempString[16] = { 0, };
	_itoa_s(cpu, tempString, 10);

	// cpu 문자열을 설정합니다.
	char cpuString[16] = { 0, };
	strcpy_s(cpuString, "Cpu: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	// 문장 정점 버퍼를 새 문자열 정보로 업데이트합니다.
	return UpdateSentence(m_sentence4, cpuString, 20, 80, 0.0f, 1.0f, 0.0f, deviceContext);
}
