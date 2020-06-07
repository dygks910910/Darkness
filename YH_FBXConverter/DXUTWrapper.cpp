#include "DXUTWrapper.h"

CLightShaderClass DXUTWrapper::m_LightShader;
CLightClass DXUTWrapper::m_Light;
CModelClass DXUTWrapper::m_Model;
CModelViewerCamera DXUTWrapper::m_Camera;

COpenFBXWrraper g_FBXrapper;
ID3D11RasterizerState* DXUTWrapper::m_rasterState = nullptr;

DXUTWrapper::DXUTWrapper()
{
    
}

DXUTWrapper::~DXUTWrapper()
{
   
}

//--------------------------------------------------------------------------------------
// Called right before creating a D3D9 or D3D11 device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool DXUTWrapper::ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext)
{
    return true;
}

//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void DXUTWrapper::OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
    m_Camera.FrameMove(fElapsedTime);
}

//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT DXUTWrapper::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext)
{
    m_Camera.HandleMessages(hWnd, uMsg, wParam, lParam);

    return LRESULT();
}

//--------------------------------------------------------------------------------------
// Reject any D3D11 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool DXUTWrapper::IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo* AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo* DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext)
{
    return true;
}

//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT DXUTWrapper::OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
    m_LightShader.Initialize(pd3dDevice,0);
    m_Model.Initialize(pd3dDevice, L"data/seafloor.dds");
    m_Light.SetDiffuseColor(1.0f, 0.0f, 1.0f, 1.0f);
    m_Light.SetDirection(0.0f, 0.0f, 1.0f);

    D3DXVECTOR3 vecEye(0.0f, 0.0f, -100.0f);
    D3DXVECTOR3 vecAt(0.0f, 0.0f, -0.0f);
    m_Camera.SetViewParams(&vecEye, &vecAt);
    //m_Camera.SetViewParams()
    D3D11_RASTERIZER_DESC rasterDesc;
    // Setup the raster description which will determine how and what polygons will be drawn.
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;
    HRESULT result;
    // Create the rasterizer state from the description we just filled out.
    result = pd3dDevice->CreateRasterizerState(&rasterDesc, &m_rasterState);
    
    return S_OK;
}

//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT DXUTWrapper::OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
    float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
    m_Camera.SetProjParams(D3DX_PI / 4, fAspectRatio, 2.0f, 4000.0f);
    m_Camera.SetWindow(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);
    m_Camera.SetButtonMasks(MOUSE_MIDDLE_BUTTON, MOUSE_WHEEL, MOUSE_LEFT_BUTTON);
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11ResizedSwapChain 
//--------------------------------------------------------------------------------------
void DXUTWrapper::OnD3D11ReleasingSwapChain(void* pUserContext)
{

}

//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11CreateDevice 
//--------------------------------------------------------------------------------------
void DXUTWrapper::OnD3D11DestroyDevice(void* pUserContext)
{
    m_Model.Shutdown();
    m_LightShader.Shutdown();
}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void DXUTWrapper::OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext)
{
    if (g_FBXrapper.GetExporter()->GetisLoaded())
     {
        m_Model.Shutdown();
        std::vector<CModelClass::VertexType> vetices;
        std::vector<int> indices;
        g_FBXrapper.GetVertices(vetices);
        g_FBXrapper.GetIndices(indices);
        m_Model.Initialize(pd3dDevice, L"data/seafloor.dds", vetices, indices);
        g_FBXrapper.GetExporter()->Shutdown();
    }
    pd3dImmediateContext->RSSetState(m_rasterState);
    // Clear render target and the depth stencil 
    float ClearColor[4] = { 0.176f, 0.196f, 0.667f, 0.0f };

    ID3D11RenderTargetView* pRTV = DXUTGetD3D11RenderTargetView();
    ID3D11DepthStencilView* pDSV = DXUTGetD3D11DepthStencilView();
    pd3dImmediateContext->ClearRenderTargetView(pRTV, ClearColor);
    pd3dImmediateContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0, 0);

    D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
    viewMatrix = *m_Camera.GetViewMatrix();
    D3DXMATRIX mProj = *m_Camera.GetProjMatrix();
    D3DXMATRIX mView = *m_Camera.GetViewMatrix();
    
    D3DXMatrixIdentity(&worldMatrix);

    // Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
    m_Model.Render(pd3dImmediateContext);

    // Render the model using the color shader.
    m_LightShader.Render(pd3dImmediateContext,
        m_Model.GetIndexCount(),
        *m_Camera.GetWorldMatrix(), mView, mProj, m_Model.GetTexture(),
        m_Light.GetDirection(), m_Light.GetDiffuseColor());
}

void DXUTWrapper::OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext)
{
}

void DXUTWrapper::OnMouse(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos, void* pUserContext)
{
}

bool DXUTWrapper::OnDeviceRemoved(void* pUserContext)
{
    return true;
}
