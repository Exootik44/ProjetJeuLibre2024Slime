#include "pch.h"

#include "WindowDX.h"
#include "InputManager.h"

WindowDX::WindowDX(HINSTANCE hInstance) : m_appInstance(hInstance)
{
    // Only one WindowDX can be constructed
    assert(mp_Application == nullptr);
    mp_Application = this;
}

WindowDX* WindowDX::mp_Application = nullptr;

WindowDX* WindowDX::GetApp()
{
    return mp_Application;
}

LRESULT CALLBACK WindowDX::MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Verifie si l'application existe avant d'appeler MsgProc
    if (WindowDX::GetApp() == nullptr)
    {
        return DefWindowProc(hwnd, msg, wParam, lParam); // Si l'application n'existe pas, appelle la procedure par defaut
    }

    // Appeler la methode MsgProc sur l'instance active de WindowDX
    return WindowDX::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
}

bool WindowDX::InitWindow()
{
	// Initialise la classe window
    WNDCLASS windowClass = {};                          // Definition de la structure WNDCLASS
    windowClass.style = CS_HREDRAW | CS_VREDRAW;        // Redessine la fenetre si elle est redimensionnee
    windowClass.lpfnWndProc = MainWndProc;              // Fonction de callback pour gerer les messages
    windowClass.cbClsExtra = 0;                         // Pas d'espace supplementaire alloue pour cette classe
    windowClass.cbWndExtra = 0;                         // Pas d'espace supplementaire alloue pour chaque fenetre
    windowClass.hInstance = m_appInstance;              // Instance de l'application
    windowClass.hIcon = LoadIcon(0, IDI_APPLICATION);   // Utilisation de l'icone par defaut
    windowClass.hCursor = LoadCursor(0, IDC_ARROW);     // Curseur par defaut
    windowClass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH); // Pas de fond specifique
    windowClass.lpszMenuName = 0;                       // Pas de menu associe
    windowClass.lpszClassName = L"DX12WindowClass";     // Nom de la classe de fenetre

    if (!RegisterClass(&windowClass)) // Enregistre la classe de fenetre avec RegisterClass
    {
        MessageBox(0, L"Erreur d'initialisation de la classe window.", 0, 0);
        return false;
    }

    // Compute window rectangle dimensions based on requested client area dimensions
    RECT R = { 0, 0, m_clientWidth, m_clientHeight };
    AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
    int width = R.right - R.left;
    int height = R.bottom - R.top;

    // Creation de la fenetre
    m_mainWindow = CreateWindow(L"DX12WindowClass", m_windowTitle.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, m_appInstance, 0);
    
    if (!m_mainWindow)
    {
        MessageBox(0, L"CreateWindow Failed.", 0, 0);
        return false;
    }

    ShowWindow(m_mainWindow, SW_SHOW);
    UpdateWindow(m_mainWindow);

	return true;
}

bool WindowDX::Initialize()
{
    if (!InitWindow())
        return false;

    if (!InitDirect3D())
        return false;

    OnResize();
    //MessageBox(0, L"INIT REUSSI !", L"Init", MB_OK);
    return true;
}

int WindowDX::Run()
{
    MSG msg = { 0 };

    //SetDeltaTime(m_deltaTime); // AJOUTER SYSTEME DE TIMER
    while (msg.message != WM_QUIT)
    {
        // If there are Window messages then process them.
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        // Otherwise, do animation/game stuff.

        Update();
        /*DWORD t = timeGetTime();*/

        Draw();
        //DWORD dt = timeGetTime() - t;
        //wchar_t title[256];
        //swprintf_s(title, 256, L"lag meters: %d", (int)dt);
        //SetWindowText(GetActiveWindow(), title);
    }

    return (int)msg.wParam;
}

//void WindowDX::Update()
//{
//}
//
//void WindowDX::Draw()
//{
//}

LRESULT WindowDX::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_SIZE:
        // Gerer le redimensionnement de la fenetre
        break;
    case WM_KEYDOWN:
        // Gerer la pression de touche
        break; 
    case WM_MOUSEWHEEL:
            InputManager::ProcessMouseWheel(wParam);
            break;
    case WM_DESTROY:
        if (m_d3DDevice) 
        {
            // Libere les ressources Direct3D, les objets COM
            m_d3DDevice->Release();
        }
        PostQuitMessage(0); // Indique que l'application doit se terminer
        break;
    case WM_CLOSE:
        if (m_d3DDevice)
        {
            // Libere les ressources Direct3D, les objets COM
            m_d3DDevice->Release();
        }
        DestroyWindow(hwnd); // Ferme la fenetre
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam); // Traite les autres messages par defaut
    }

    return 0;
}

bool WindowDX::InitDirect3D()
{
    // Debug Log DirectX & Enable run-time memory check for debug build
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
        debugController->EnableDebugLayer();
    }
#endif

    // 1 Creation du factory DXGI
    ComPtr<IDXGIFactory4> dxgiFactory;
    if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory))))
    {
        MessageBox(0, L"Erreur lors de la creation du DXGIFactory.", 0, 0);
        return false;
    }

    // 2 Creation du device DirectX12
    if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0,IID_PPV_ARGS(&m_d3DDevice))))
    {
        MessageBox(0, L"Erreur lors de la creation du D3D12 Device.", 0, 0);
        return false;
    }

    // 3 Creation de la Command Queue
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    if (FAILED(m_d3DDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue))))
    {
        MessageBox(0, L"Erreur lors de la creation de la Command Queue.", 0, 0);
        return false;
    }

    if (m_clientWidth <= 0 || m_clientHeight <= 0)
    {
        MessageBox(0, L"Les dimensions de la fenetre sont invalides.", 0, 0);
        return false;
    }

    // 4 Creation du Swap Chain
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = m_clientWidth;
    swapChainDesc.Height = m_clientHeight;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = SwapChainBufferCount;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    ComPtr<IDXGISwapChain1> swapChain;

    HRESULT commandResult = dxgiFactory->CreateSwapChainForHwnd(m_commandQueue.Get(), m_mainWindow, &swapChainDesc, nullptr, nullptr, &swapChain);
    if (FAILED(commandResult))
    {
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, L"CreateSwapChainForHwnd failed with HRESULT 0x%08X.", commandResult);
        MessageBox(0, errorMsg, L"Swap Chain Error", MB_OK);
        return false;
    }
    swapChain.As(&m_swapChain); // convertion du swapchain1 au 3

    // 5 Creation du Descriptor RTVHeap pour les Render Target Views (RTV) & DSV
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    if (FAILED(m_d3DDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap))))
    {
        MessageBox(0, L"Erreur lors de la creation du RTV Heap.", 0, 0);
        return false;
    }

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;
    if (FAILED(m_d3DDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_dsvHeap.GetAddressOf()))))
    {
        MessageBox(0, L"Erreur lors de la creation du DSV Heap.", 0, 0);
        return false;
    }

    // 6 Creation des Render Target Views pour chaque buffer du swap chain
    m_rtvDescriptorSize = m_d3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < SwapChainBufferCount; i++)
    {
        if (FAILED(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]))))
        {
            MessageBox(0, L"Erreur lors de la recuperation du buffer du Swap Chain.", 0, 0);
            return false;
        }
        m_d3DDevice->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, m_rtvDescriptorSize);
    }

    // 7 Creation du CommandAllocator
    commandResult = m_d3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));
    if (FAILED(commandResult))
    {
        MessageBox(0, L"Erreur lors de la creation du Command Allocator.", 0, 0);
        return false;
    }

    // 8 Creation de la Command List
    commandResult = m_d3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList));
    if (FAILED(commandResult))
    {
        MessageBox(0, L"Erreur lors de la creation de la Command List.", 0, 0);
        return false;
    }
    m_commandList->Close(); // commandlist "en attente"

    // 9 Creation de fence
    commandResult = m_d3DDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
    if (FAILED(commandResult))
    {
        MessageBox(0, L"Erreur lors de la creation du Fence.", 0, 0);
        return false;
    }
    m_fenceValue = 1;

    return true;
}
void WindowDX::OnResize()
{
    assert(m_d3DDevice);
    assert(m_swapChain);
    assert(m_commandAllocator);

    // Flush before changing any resources.
    FlushCommandQueue();

    HRESULT hr = m_commandList->Reset(m_commandAllocator.Get(), nullptr);
    if (FAILED(hr))
    {
        MessageBox(0, L"Erreur CommandListResetAllocator.", 0, 0);
        return;
    }
    //ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));

    // Release the previous resources we will be recreating.
    for (int i = 0; i < SwapChainBufferCount; ++i)
        m_renderTargets[i].Reset();
    m_depthStencilBuffer.Reset();

    // Resize the swap chain.
    hr = m_swapChain->ResizeBuffers(SwapChainBufferCount,m_clientWidth, m_clientHeight,mBackBufferFormat,DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
    if (FAILED(hr))
    {
        MessageBox(0, L"Erreur resize buffer.", 0, 0);
        return;
    }

    m_currBackBuffer = 0;

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < SwapChainBufferCount; i++)
    {
        hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
        if (FAILED(hr))
        {
            MessageBox(0, L"Erreur get buffer.", 0, 0);
            return;
        }
        m_d3DDevice->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHeapHandle);
        rtvHeapHandle.Offset(1, m_rtvDescriptorSize);
    }

    // Create the depth/stencil buffer and view.
    D3D12_RESOURCE_DESC depthStencilDesc;
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = m_clientWidth;
    depthStencilDesc.Height = m_clientHeight;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;

    // Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
    // the depth buffer.  Therefore, because we need to create two views to the same resource:
    //   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
    //   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
    // we need to create the depth buffer resource with a typeless format.  
    depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

    depthStencilDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
    depthStencilDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optClear;
    optClear.Format = m_depthStencilFormat;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
    hr = m_d3DDevice->CreateCommittedResource(&heapProps,D3D12_HEAP_FLAG_NONE,&depthStencilDesc,D3D12_RESOURCE_STATE_COMMON,&optClear,IID_PPV_ARGS(m_depthStencilBuffer.GetAddressOf()));
    if (FAILED(hr))
    {
        MessageBox(0, L"Erreur CreatingCommittedResource.", 0, 0);
        return;
    }

    // Create descriptor to mip level 0 of entire resource using the format of the resource.
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = m_depthStencilFormat;
    dsvDesc.Texture2D.MipSlice = 0;
    m_d3DDevice->CreateDepthStencilView(m_depthStencilBuffer.Get(), &dsvDesc, DepthStencilView());

    // Transition the resource from its initial state to be used as a depth buffer.
    CD3DX12_RESOURCE_BARRIER barrierBuffer = CD3DX12_RESOURCE_BARRIER::Transition(m_depthStencilBuffer.Get(),D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    m_commandList->ResourceBarrier(1, &barrierBuffer);

    // Execute the resize commands.
    m_commandList->Close();
    ID3D12CommandList* cmdsLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // Wait until resize is complete.
    FlushCommandQueue();

    // Update the viewport transform to cover the client area.
    m_screenViewport.TopLeftX = 0;
    m_screenViewport.TopLeftY = 0;
    m_screenViewport.Width = static_cast<float>(m_clientWidth);
    m_screenViewport.Height = static_cast<float>(m_clientHeight);
    m_screenViewport.MinDepth = 0.0f;
    m_screenViewport.MaxDepth = 1.0f;

    m_scissorRect = { 0, 0, m_clientWidth, m_clientHeight };
}

void WindowDX::FlushCommandQueue()
{
    // Advance the fence value to mark commands up to this fence point.
    m_fenceValue++;

    // Add an instruction to the command queue to set a new fence point.  Because we 
    // are on the GPU timeline, the new fence point won't be set until the GPU finishes
    // processing all the commands prior to this Signal().
    HRESULT hr = m_commandQueue->Signal(m_fence.Get(), m_fenceValue);
    if (FAILED(hr))
    {
        MessageBox(0, L"Erreur lors du Signal du Fence.", 0, 0);
        return;
    }

    // Wait until the GPU has completed commands up to this fence point.
    if (m_fence->GetCompletedValue() < m_fenceValue)
    {
        // HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
        HANDLE eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);

        hr = m_fence->SetEventOnCompletion(m_fenceValue, eventHandle);
        if (FAILED(hr))
        {
            MessageBox(0, L"Erreur lors du SetEventOnCompletion du Fence.", 0, 0);
            return;
        }
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}

ID3D12Resource* WindowDX::CurrentBackBuffer()const
{
    return m_renderTargets[m_currBackBuffer].Get();
}
D3D12_CPU_DESCRIPTOR_HANDLE WindowDX::CurrentBackBufferView()const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),m_currBackBuffer,m_rtvDescriptorSize);
}
D3D12_CPU_DESCRIPTOR_HANDLE WindowDX::DepthStencilView()const
{
    return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
}