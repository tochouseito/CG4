#pragma once
#include<dxgidebug.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cassert>
#include <wrl.h>
#include<dxcapi.h>
#include <cstdint>
#include<chrono>
#include<thread>

#include "WinApp.h"
#include"ConvertString.h"
#include"Color.h"
//ここにあるのはまずい
#include"ViewProjection.h"


#pragma comment(lib,"dxcompiler.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
class D3DResourceLeakChecker {
public:
	~D3DResourceLeakChecker()
	{
		// リソースリークチェック

		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);

		}
	}
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static D3DResourceLeakChecker* GetInstance();
};
class DirectXCommon
{
public:// メンバ関数
	struct MtlData {
		float time = 0.0f;
	};
	struct HSVData {
		float hue;
		float saturation;
		float value;
	};
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static DirectXCommon* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(
		WinApp* win, int32_t backBufferWidth = WinApp::kClientWidth,
		int32_t backBufferHeight = WinApp::kClientHeight,bool enableDbugLayer=true);

	/// <summary>
	/// デバイスの初期化
	/// </summary>
	void InitializeDXGIDevice(bool enableDebugLayer);

	/// <summary>
	/// コマンドの初期化
	/// </summary>
	void CreateCommand();

	/// <summary>
	/// 深度リソースの作成
	/// </summary>
	void CreateDepthStencil();

	//まずい
	void SetViewProjection(ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
	

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	void PreRTVDraw();
	void PostRTVDraw();

	void FenceClose();


	/// <summary>
	/// デバイスの取得
	/// </summary>
	/// <returns>デバイス</returns>
	 ID3D12Device* GetDevice() const { return device_.Get(); }

	 IDxcUtils* GetDxcUtils_()const { return dxcUtils_.Get(); }
	 IDxcCompiler3* GetDxcCompiler_()const { return dxcCompiler_.Get(); }
	 IDxcIncludeHandler* GetIncludeHandler_()const { return includeHandler_; }

	/// <summary>
	/// 描画コマンドリストの取得
	/// </summary>
	/// <returns>描画コマンドリスト</returns>
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }

	/// <summary>
	/// バックバッファの幅取得
	/// </summary>
	/// <returns>バックバッファの幅</returns>
	int32_t GetBackBufferWidth() const;

	/// <summary>
	/// バックバッファの高さ取得
	/// </summary>
	/// <returns>バックバッファの高さ</returns>
	int32_t GetBackBufferHeight() const;

	// バックバッファの数を取得
	size_t GetBackBufferCount() const { return 2; }


	//int32_t GetBackBufferWidth()const { return backBufferWidth_; }

	//int32_t GetBackBufferHeight()const { return backBufferHeight_; }


	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(uint32_t handleNum)const { return textureSrvHandleGPU_[handleNum]; }

	void SetTextureResource(ID3D12Resource* textureResource,uint32_t handleNum) { textureResource_[handleNum] = textureResource; }
	//void CreateHandles();

	/// <summary>
	/// 
	/// </summary>
	/// <param name="device"></param>
	/// <param name="sizeInBytes"></param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr < ID3D12Resource> CreateBufferResource(ID3D12Device* device, const size_t& sizeInBytes);

	/// <summary>
	/// ディスクリプタヒープの生成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="heapType"></param>
	/// <param name="numDescriptors"></param>
	/// <param name="shaderVisible"></param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr < ID3D12DescriptorHeap> CreateDescriptorHeap(
		ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	/// <summary>
	/// 深度リソースの作成
	/// </summary>
	Microsoft::WRL::ComPtr < ID3D12Resource	> CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);


	// FPS固定初期化
	void InitializeFixFPS();
	// FPS固定更新
	void UpdateFixFPS();

	/// <summary>
	///	RenderTextureの生成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="height"></param>
	/// <param name="format"></param>
	/// <param name="clearColor"></param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr < ID3D12Resource	> CreateRenderTextureResource
	(Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t width ,uint32_t height, DXGI_FORMAT format,
		const Color& clearColor);

	void CreateRenderTexture();

	//Microsoft::WRL::ComPtr < ID3D12Resource	> CreateDepthTextureResource
	//(Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t width, uint32_t height, DXGI_FORMAT format);

	void CreateDepthTexture();

	void BarrierTransition(ID3D12Resource* pResource,
		D3D12_RESOURCE_STATES Before, D3D12_RESOURCE_STATES After);

	void SetMaskTexture(std::string textureHandle) { textureHandle_ = textureHandle; }
	
	void CreateMtlResource();
	void CreateHSVResource();
	HSVData* GetHSVData() { return HSVData_; }
private:// メンバ関数
	DirectXCommon() = default;
	~DirectXCommon() = default;
	DirectXCommon(const DirectXCommon&) = delete;
	const DirectXCommon& operator=(const DirectXCommon&) = delete;

private:// メンバ変数
	// ウィンドウズアプリケーション管理
	WinApp* winApp_;

	// 記録時間FPS固定用
	std::chrono::steady_clock::time_point reference_;

	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	Microsoft::WRL::ComPtr < ID3D12CommandQueue>commandQueue_;
	Microsoft::WRL::ComPtr < ID3D12CommandAllocator>commandAllocator_;
	Microsoft::WRL::ComPtr < ID3D12GraphicsCommandList>commandList_;
	Microsoft::WRL::ComPtr < IDXGISwapChain4>swapChain_;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
	//IDXGISwapChain4*swapChain_;
	Microsoft::WRL::ComPtr < ID3D12DescriptorHeap>rtvDescriptorHeap_;
	Microsoft::WRL::ComPtr < ID3D12DescriptorHeap> dsvDescriptorHeap_;
	Microsoft::WRL::ComPtr < ID3D12Resource>swapChainResources_[2];
	Microsoft::WRL::ComPtr <ID3D12Fence>fence_;
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState_;
	Microsoft::WRL::ComPtr < IDxcUtils> dxcUtils_;
	Microsoft::WRL::ComPtr < IDxcCompiler3> dxcCompiler_;
	Microsoft::WRL::ComPtr < ID3D12Resource>depthStencilResource_;
	//std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers_;
	//Microsoft::WRL::ComPtr<ID3D12Resource > backBuffers_;
	IDxcIncludeHandler* includeHandler_;
	
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_[2];
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_[2];
	Microsoft::WRL::ComPtr < ID3D12Resource> textureResource_[2];
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc_{};
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[3];
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
	// DescriptorSizeを取得している
	//uint32_t descriptorSizeSRV_;
	uint32_t descriptorSizeRTV_;
	uint32_t descriptorSizeDSV_;
	
	HANDLE fenceEvent_;
	uint64_t fenceValue_ = 0;
	/*TransForm transform_;
	TransForm cameraTransform_;*/
		/*Microsoft::WRL::ComPtr <*/
	int32_t backBufferWidth_ = 0;
	int32_t backBufferHeight_ = 0;
	uint32_t srvIndex_ = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;
	Microsoft::WRL::ComPtr < ID3D12Resource>  renderTextureResource_;
	const Color kRenderTargetClearValue{ 0.1f,0.25f,0.5f,1.0f };
	ViewProjection* viewProjection_;
	uint32_t depthSrvIndex_ = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE depthSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE depthSrvHandleGPU_;
	/*マスク用テクスチャハンドル*/
	std::string textureHandle_;
	Microsoft::WRL::ComPtr < ID3D12Resource> materialResource_;
	MtlData* materialData_ = nullptr;
	Microsoft::WRL::ComPtr < ID3D12Resource> HSVResource_;
	HSVData* HSVData_ = nullptr;
};

