#include "stdafx.h"
#include "CD3dClass.h"
#include "CCameraClass.h"
#include "CModelClass.h"
#include "CTextureShaderClass.h"
#include "RenderTextureClass.h"
#include "BitmapClass.h"
#include "FadeShaderClass.h"
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

	// ��Ʈ �� ��ü�� ����ϴ�.
	m_Bitmap = new BitmapClass;
	if (!m_Bitmap)
	{
		return false;
	}

	// ��Ʈ �� ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_Bitmap->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, screenWidth, screenHeight))
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

	// ���̵� �� Ÿ���� 3000 �и� �ʷ� �����մϴ�.
	m_fadeInTime = 3000.0f;

	// ���� �� �ð��� 0 �и� �ʷ� �ʱ�ȭ�մϴ�.
	m_accumulatedTime = 0;

	// ���̵� ������� ó���� 0���� �ʱ�ȭ�Ͽ� ����� �˰� ǥ�õ˴ϴ�.
	m_fadePercentage = 0;

	// ȿ���� ��������� �����մϴ�.
	m_fadeDone = false;

	// ���̵� ���̴� ��ü�� ����ϴ�.
	m_FadeShader = new FadeShaderClass;
	if (!m_FadeShader)
	{
		return false;
	}

	// ���̵� ���̴� ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_FadeShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the fade shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// ���̵� ���̴� ��ü�� �����Ͻʽÿ�.
	if (m_FadeShader)
	{
		m_FadeShader->Shutdown();
		delete m_FadeShader;
		m_FadeShader = 0;
	}

	// ��Ʈ �� ��ü�� �����մϴ�.
	if (m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
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


bool GraphicsClass::Frame(float frameTime)
{
	if (!m_fadeDone)
	{
		// ���� �� �ð��� ������ ������ �ð� �߰��� ������Ʈ�Ͻʽÿ�.
		m_accumulatedTime += frameTime;

		// �ð��� ������ �� �������� ����ϴ� �ð���ŭ ���̵� ���� �����մϴ�.
		if (m_accumulatedTime < m_fadeInTime)
		{
			// ���� �� �ð��� �������� ȭ���� ������� ������ ����մϴ�.
			m_fadePercentage = m_accumulatedTime / m_fadeInTime;
		}
		else
		{
			// ���̵� �� Ÿ���� �Ϸ�Ǹ� ���̵� ȿ���� ���� ����� ���������� �������մϴ�.
			m_fadeDone = true;

			// ������� 100 %�� �����մϴ�.
			m_fadePercentage = 1.0f;
		}
	}

	// ī�޶� ��ġ�� �����մϴ�.
	m_Camera->SetPosition(0.0f, 0.0f, -6.0f);

	return true;
}


bool GraphicsClass::Render()
{
	bool result;
	static float rotation = 0.0f;


	// �� �������� rotation ������ ������Ʈ�մϴ�.
	rotation += (float)XM_PI * 0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	if (m_fadeDone)
	{
		// ���̵� ���� �Ϸ�Ǹ� �� ���۸� ����Ͽ� ����� ���������� �������մϴ�.
		RenderNormalScene(rotation);
	}
	else
	{
		// ���̵� ���� �Ϸ���� ���� ��� ����� �ؽ�ó�� �������ϰ� �ؽ�ó�� ���̵� ���մϴ�.
		result = RenderToTexture(rotation);
		if (!result)
		{
			return false;
		}

		result = RenderFadingScene();
		if (!result)
		{
			return false;
		}
	}

	return true;
}

bool GraphicsClass::RenderToTexture(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// ������ ����� �������� �°� �����մϴ�.
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView());

	// �������� �ؽ�ó�� �� ��ϴ�.
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
	m_Camera->Render();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����ɴϴ�.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// ȸ���� ���� ���� ��Ŀ� ���մϴ�.
	worldMatrix = XMMatrixRotationY(rotation);

	// �� ���ؽ��� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� ������� �غ��մϴ�.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// �ؽ�ó ���̴��� ���� �������Ѵ�.
	m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_Model->GetTexture());

	// ������ ����� ������ �� ���۷� �ٽ� �����ϰ� �������� ���� �������� �� �̻� �ٽ� �������� �ʽ��ϴ�.
	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}


bool GraphicsClass::RenderFadingScene()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;


	// ����� ������ ���۸� �����.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
	m_Camera->Render();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� (ortho) ����� �����ɴϴ�.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// ��� 2D �������� �����Ϸ��� Z ���۸� ���ϴ�.
	m_Direct3D->TurnZBufferOff();

	// ��Ʈ �� ���ؽ��� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� �׸��⸦ �غ��մϴ�.
	result = m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 0, 0);
	if (!result)
	{
		return false;
	}

	// ���̵� ���̴��� ����Ͽ� ��Ʈ ���� �������մϴ�.
	result = m_FadeShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
		m_RenderTexture->GetShaderResourceView(), m_fadePercentage);
	if (!result)
	{
		return false;
	}

	// ��� 2D �������� �Ϸ�Ǿ����Ƿ� Z ���۸� �ٽ� �ѽʽÿ�.
	m_Direct3D->TurnZBufferOn();

	// ������ �� ����� ȭ�鿡 ǥ���մϴ�.
	m_Direct3D->EndScene();

	return true;
}


bool GraphicsClass::RenderNormalScene(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// ����� ������ ���۸� �����.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
	m_Camera->Render();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����ɴϴ�.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// ȸ���� ���� ���� ��Ŀ� ���մϴ�.
	worldMatrix = XMMatrixRotationY(rotation);

	// �� ���ؽ��� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� ������� �غ��մϴ�.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// �ؽ�ó ���̴��� ���� �������Ѵ�.
	if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_Model->GetTexture()))
	{
		return false;
	}

	// ������ �� ����� ȭ�鿡 ǥ���մϴ�.
	m_Direct3D->EndScene();

	return true;
}