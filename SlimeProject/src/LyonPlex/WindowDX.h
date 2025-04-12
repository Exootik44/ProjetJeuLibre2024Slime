#pragma once

// DirectX12


// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

using Microsoft::WRL::ComPtr;

struct FrameResource
{
	ComPtr<ID3D12CommandAllocator> m_cmdListAlloc;
	ComPtr<ID3D12Resource> m_passCB;  // Buffer constant par frame (optionnel)
	UINT64 m_fence = 0; // Fence de la frame pour la synchronisation
};

class WindowDX
{
public:
	WindowDX(HINSTANCE hInstance);
	static WindowDX* GetApp();
	
	static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam); // Fonction de callback pour gerer les messages de la fenetre
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	bool InitWindow(); // Initialise la fenetre

	bool Initialize();

	int Run();

	virtual void Update() = 0;
	virtual void Draw() = 0;

	HWND GetMainWindow() { return m_mainWindow; };

	void FlushCommandQueue();

	int GetScreenWidth() { return m_clientWidth; };
	int GetScreenHeight() { return m_clientHeight; };

private:
	static WindowDX* mp_Application; // ref instance de la classe

	HINSTANCE m_appInstance = nullptr; // application instance handle

	// Window parameters
	HWND m_mainWindow = nullptr;
	std::wstring m_windowTitle = L"WINDOW 3D";
	int m_clientWidth = 1000;
	int m_clientHeight = 800;

protected:
	// Nouvelle methode pour initialiser DirectX12
	bool InitDirect3D();
	void OnResize();
	
	ID3D12Resource* CurrentBackBuffer()const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

	// Membres DirectX12
	//UINT mFrameIndex = 0;
	D3D12_VIEWPORT m_screenViewport;
	D3D12_RECT m_scissorRect;

	UINT64 m_fenceValue = 0;
	ComPtr<ID3D12Fence> m_fence;
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12Device>           m_d3DDevice;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<ID3D12CommandQueue>     m_commandQueue;
	ComPtr<IDXGISwapChain3>        m_swapChain;
	ComPtr<ID3D12DescriptorHeap>   m_rtvHeap;
	ComPtr<ID3D12DescriptorHeap>   m_dsvHeap;
	static const int			   SwapChainBufferCount = 2; // Double buffering
	int m_currBackBuffer = 0;
	ComPtr<ID3D12Resource>         m_renderTargets[SwapChainBufferCount];
	UINT                           m_rtvDescriptorSize = 0;

	ComPtr<ID3D12Resource> m_depthStencilBuffer;
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// Set true to use 4X MSAA The default is false.
	bool      m_4xMsaaState = false;    // 4X MSAA enabled
	UINT      m_4xMsaaQuality = 0;      // quality level of 4X MSAA

	// Init3DApp
	// PSO Rendu 3D scene
	ComPtr<ID3D12PipelineState> m_pipelineState;
	ComPtr<ID3D12RootSignature> m_rootSignature;

	// PSO UI rendu 2D
	ComPtr<ID3D12PipelineState> m_pipelineStateUI;
	ComPtr<ID3D12RootSignature> m_rootSignatureUI;

	//remplace flush
	const int gNum_frameResources = 3; // Triple buffering
	std::vector<std::unique_ptr<FrameResource>> m_frameResources;
	FrameResource* m_currFrameResource = nullptr;
	int m_currFrameIndex = 0;

	UINT64 m_currentFence = 0;
};

