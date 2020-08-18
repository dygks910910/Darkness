#include "stdafx.h"
#include "CD3dClass.h"
#include "CCameraClass.h"
#include "CModelClass.h"
#include "CTextureShaderClass.h"
#include "TransparentShaderClass.h"
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

	// ù ��° �� ��ü�� ����ϴ�.
	m_Model1 = new ModelClass;
	if (!m_Model1)
	{
		return false;
	}

	// ù ��° �� ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_Model1->Initialize(m_Direct3D->GetDevice(), (WCHAR*)L"data/dirt01.dds", (char*)"data/square.txt"))
	{
		MessageBox(hwnd, L"Could not initialize the first model object.", L"Error", MB_OK);
		return false;
	}

	// �� ��° �� ��ü�� ����ϴ�.
	m_Model2 = new ModelClass;
	if (!m_Model2)
	{
		return false;
	}

	// �� ��° �� ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_Model2->Initialize(m_Direct3D->GetDevice(), (WCHAR*)L"data/stone01.dds", (char*)"data/square.txt"))
	{
		MessageBox(hwnd, L"Could not initialize the second model object.", L"Error", MB_OK);
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

	// ������ ���̴� ��ü�� ����ϴ�.
	m_TransparentShader = new TransparentShaderClass;
	if (!m_TransparentShader)
	{
		return false;
	}

	// ���� ���̴� ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_TransparentShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the transparent shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// ���� ���̴� ��ü�� �����մϴ�.
	if (m_TransparentShader)
	{
		m_TransparentShader->Shutdown();
		delete m_TransparentShader;
		m_TransparentShader = 0;
	}

	// �ؽ�ó ���̴� ��ü�� �����Ѵ�.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// �� ��° �� ��ü�� �����մϴ�.
	if (m_Model2)
	{
		m_Model2->Shutdown();
		delete m_Model2;
		m_Model2 = 0;
	}

	// ù ��° �� ��ü�� �����մϴ�.
	if (m_Model1)
	{
		m_Model1->Shutdown();
		delete m_Model1;
		m_Model1 = 0;
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
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	return true;
}


bool GraphicsClass::Render()
{
	// ���� ���� 50% �����մϴ�.
	float blendAmount = 0.5f;

	// ���� �׸��� ���� ���۸� ����ϴ�
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�
	m_Camera->Render();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����ɴϴ�
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// �� ���ؽ��� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� ������� �غ��մϴ�.
	m_Model1->Render(m_Direct3D->GetDeviceContext());

	// �ؽ�ó ���̴��� ���� �������Ѵ�.
	if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model1->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_Model1->GetTexture()))
	{
		return false;
	}

	// �ϳ��� ������ ���������� �����ϰ� �ϳ��� ������ ī�޶�� �����մϴ�.
	worldMatrix = XMMatrixTranslation(1.0f, 0.0f, -1.0f);

	// ������ �۵��ϵ��� ���� ���带 �մϴ�.
	m_Direct3D->TurnOnAlphaBlending();

	// �� ��° �簢�� ���� �׷��� ������ ���ο� ��ġ�մϴ�.
	m_Model2->Render(m_Direct3D->GetDeviceContext());

	// �� �ؽ�ó�� �� ��° �簢�� ���� �������ϰ� ������ ���� 50 %�� ȥ�շ��� ����մϴ�.
	if (!m_TransparentShader->Render(m_Direct3D->GetDeviceContext(), m_Model2->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_Model2->GetTexture(), blendAmount))
	{
		return false;
	}

	// ���� ������ ���ϴ�.
	m_Direct3D->TurnOffAlphaBlending();

	// ������ ������ ȭ�鿡 ����մϴ�
	m_Direct3D->EndScene();

	return true;
}