#include "stdafx.h"
#include "CD3dClass.h"
#include "CCameraClass.h"
#include "CModelClass.h"
#include "CTextureShaderClass.h"
#include "RenderTextureClass.h"
#include "ReflectionShaderClass.h"
#include "CGraphicsClass.h"


GraphicsClass::GraphicsClass()
{
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	// Direct3D ��ü ����
	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
	{
		return false;
	}

	// Direct3D ��ü �ʱ�ȭ
	if (!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// m_Camera ��ü ����
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// �� ��ü ����
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}

	// �� ��ü �ʱ�ȭ
	if (!m_Model->Initialize(m_Direct3D->GetDevice(), (WCHAR*)L"data/seafloor.dds", (char*)"data/cube.txt"))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// �ؽ�ó ���̴� ��ü�� �����Ѵ�.
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}

	// �ؽ�ó ���̴� ��ü�� �ʱ�ȭ�Ѵ�.
	if (!m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// ������ �ؽ�ó ��ü�� �����Ѵ�.
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture)
	{
		return false;
	}

	// ������ �ؽ�ó ��ü�� �ʱ�ȭ�Ѵ�.
	if (!m_RenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight))
	{
		return false;
	}

	// Create the floor model object.
	m_FloorModel = new ModelClass;
	if (!m_FloorModel)
	{
		return false;
	}

	// Initialize the floor model object.
	if (!m_FloorModel->Initialize(m_Direct3D->GetDevice(), (WCHAR*)L"data/blue01.dds", (char*)"data/floor.txt"))
	{
		MessageBox(hwnd, L"Could not initialize the floor model object.", L"Error", MB_OK);
		return false;
	}

	// Create the reflection shader object.
	m_ReflectionShader = new ReflectionShaderClass;
	if (!m_ReflectionShader)
	{
		return false;
	}

	// Initialize the reflection shader object.
	if (!m_ReflectionShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the reflection shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the reflection shader object.
	if (m_ReflectionShader)
	{
		m_ReflectionShader->Shutdown();
		delete m_ReflectionShader;
		m_ReflectionShader = 0;
	}

	// Release the floor model object.
	if (m_FloorModel)
	{
		m_FloorModel->Shutdown();
		delete m_FloorModel;
		m_FloorModel = 0;
	}

	// ������ �ؽ��� ��ü�� �������Ѵ�.
	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	// �ؽ�ó ���̴� ��ü�� �����Ѵ�.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// �� ��ü ��ȯ
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// m_Camera ��ü ��ȯ
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Direct3D ��ü ��ȯ
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
}


bool GraphicsClass::Frame()
{
	// ī�޶� ��ġ ����
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	return true;
}


bool GraphicsClass::Render()
{
	// ��ü ����� ���� �ؽ�ó�� �������մϴ�.
	if (!RenderToTexture())
	{
		return false;
	}



	// �� ������ ����� ���������� �������մϴ�.
	if (!RenderScene())
	{
		return false;
	}

	return true;
}

bool GraphicsClass::RenderToTexture()
{
	XMMATRIX worldMatrix, reflectionViewMatrix, projectionMatrix;

	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView());

	// Clear the render to texture.
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Use the camera to calculate the reflection matrix.
	m_Camera->RenderReflection(-1.5f);

	// Get the camera reflection view matrix instead of the normal view matrix.
	reflectionViewMatrix = m_Camera->GetReflectionViewMatrix();

	// Get the world and projection matrices.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Update the rotation variable each frame.
	static float rotation = 0.0f;
	rotation += (float)XM_PI * 0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}
	worldMatrix = XMMatrixRotationY(rotation);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// Render the model using the texture shader and the reflection view matrix.
	m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, reflectionViewMatrix,
		projectionMatrix, m_Model->GetTexture());

	// ������ ����� ������ �� ���۷� �ٽ� �����ϰ� �������� ���� �������� �� �̻� �ٽ� �������� �ʽ��ϴ�.
	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}


bool GraphicsClass::RenderScene()
{
	// ���� �׸��� ���� ���۸� ����ϴ�
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�
	m_Camera->Render();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����ɴϴ�
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// �� �������� rotation ������ ������Ʈ�մϴ�.
	static float rotation = 0.0f;
	rotation += (float)XM_PI * 0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// ȸ�� ������ ���� ����� ȸ���մϴ�.
	worldMatrix = XMMatrixRotationY(rotation);

	// �� ���ؽ��� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� ������ �մϴ�.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// Render the model with the texture shader.
	if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_Model->GetTexture()))
	{
		return false;
	}

	// Get the world matrix again and translate down for the floor model to render underneath the cube.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	worldMatrix = XMMatrixTranslation(0.0f, -1.5f, 0.0f);

	// Get the camera reflection view matrix.
	XMMATRIX reflectionMatrix = m_Camera->GetReflectionViewMatrix();

	// Put the floor model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_FloorModel->Render(m_Direct3D->GetDeviceContext());

	// Render the floor model using the reflection shader, reflection texture, and reflection view matrix.
	if (!m_ReflectionShader->Render(m_Direct3D->GetDeviceContext(), m_FloorModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_FloorModel->GetTexture(), m_RenderTexture->GetShaderResourceView(), reflectionMatrix))
	{
		return false;
	}

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}