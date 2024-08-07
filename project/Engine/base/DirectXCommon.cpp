#include "DirectXCommon.h"
/*こいつらここにあったらあかん*/
#include"SrvManager.h"
#include"GraphicsPipelineState.h"
#include"TextureManager.h"
#include"imgui.h"

#include<cassert>
#include <wrl.h>

#include"Mymath.h"
/// <summary>
/// シングルトンインスタンスの取得
/// </summary>
D3DResourceLeakChecker* D3DResourceLeakChecker::GetInstance() {
	static D3DResourceLeakChecker instance;
	return &instance;
}
/// <summary>
/// シングルトンインスタンスの取得
/// </summary>
DirectXCommon* DirectXCommon::GetInstance() {
	static DirectXCommon instance;
	return &instance;
}
/// <summary>
/// 初期化
/// </summary>
void DirectXCommon::Initialize(WinApp* win, int32_t backBufferWidth,
	int32_t backBufferHeight,bool enableDebugLayer) {
	// FPS固定初期化
	InitializeFixFPS();
	winApp_ = win;
	backBufferHeight_ = backBufferHeight;
	backBufferWidth_ = backBufferWidth;
	/// デバイスの初期化
	InitializeDXGIDevice(enableDebugLayer);

	// コマンドの生成
	CreateCommand();

	// 深度リソースの作成
	CreateDepthStencil();

	// これは消す
	CreateMtlResource();
	CreateHSVResource();
}
/// <summary>
/// デバイスの初期化
/// </summary>
void DirectXCommon::InitializeDXGIDevice([[maybe_unused]]bool enableDebugLayer) {
#ifdef _DEBUG
	Microsoft::WRL::ComPtr< ID3D12Debug1> debugController;
	//ID3D12Debug1* debugController;
	if (enableDebugLayer) {
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {

			// デバッグレイヤーを有効化する
			debugController->EnableDebugLayer();

			// さらにGPU側でもチェックを行うようにする
			debugController->SetEnableGPUBasedValidation(TRUE);

		}
	}

#endif
	// DXGIファクトリーの生成
	//IDXGIFactory7* dxgiFactory = nullptr;
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(hr));
	Microsoft::WRL::ComPtr < IDXGIAdapter4> useAdapter = nullptr;
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; ++i) {
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;

		}
		useAdapter = nullptr;

	}
	assert(useAdapter != nullptr);
	//ID3D12Device* device = nullptr;
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0 };
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		if (SUCCEEDED(hr)) {
			Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	assert(device_ != nullptr);

	Log("Complete create D3D12Device!!!\n");

#ifdef _DEBUG
	Microsoft::WRL::ComPtr< ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {

		// ヤバいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);

		// エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);

		// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		// 抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {

			// Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			// https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};

		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;

		// 指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);

		// 解放
		//infoQueue->Release();
	}

#endif // DEBUG
}
/// <summary>
/// コマンドの初期化
/// </summary>
void DirectXCommon::CreateCommand() {
	HRESULT hr;
	// 初期値0でFenceを作る
	fence_ = nullptr;
	hr = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));

	// FenceのSignalを持つためのイベントを作成する
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);

	

	// コマンドキューを生成する
	//ID3D12CommandQueue* commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(hr));

	// コマンドアロケータを生成する
	//ID3D12CommandAllocator* commandAllocator = nullptr;
	hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	// コマンドアロケータの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	// コマンドリストを生成する
	//ID3D12GraphicsCommandList* commandList = nullptr;
	hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr,
		IID_PPV_ARGS(&commandList_));
	// コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	// スワップチェーンを生成する
	//IDXGISwapChain4* swapChain = nullptr;
	//DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc_.Width = backBufferWidth_;// 画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc_.Height = backBufferHeight_;// 画面の高さ。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;// 色の形式
	swapChainDesc_.SampleDesc.Count = 1;// マルチサンプルしない
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;// 描画のターゲットとして利用する
	swapChainDesc_.BufferCount = 2;// ダブルバッファ
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;// モニタにうつしたら、中身を破棄
	// コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), winApp_->GetHwnd(), &swapChainDesc_,
		nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr));

	// ディスクリプタヒープの生成
	
	// RTV用のヒープでディスクリプタの数は2。RTVはShader内で触るものではないので、ShaderVisibleはfalse
	rtvDescriptorHeap_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 3, false);

	// SRV用のヒープでディスクリプタの数は128。SRVはShader内で触るものではないので、ShaderVisibleはtrue
	//srvDescriptorHeap_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	// DescriptorSizeを取得している
	//descriptorSizeSRV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	descriptorSizeRTV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// SwapChainからResourceを引っ張ってくる
	hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResources_[0]));
	// うまく取得できなければ起動できない
	assert(SUCCEEDED(hr));
	hr = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResources_[1]));
	// うまく取得できなければ起動できない
	assert(SUCCEEDED(hr));

	

	// RTVの設定
	//D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;// 出力結果をSRGBに変換して書き込む
	rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;// 2dテクスチャとして書き込む
	// ディスクリプタの先頭取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	// RTVを２つ作るのでディスクリプタを２つ用意
	// まず１つ目を作る。１つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある
	rtvHandles_[0] = rtvStartHandle;
	device_->CreateRenderTargetView(swapChainResources_[0].Get(), &rtvDesc_, rtvHandles_[0]);
	// ２つ目のディスクリプタハンドルを得る（自力で）
	rtvHandles_[1].ptr = rtvHandles_[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// ２つ目を作る
	device_->CreateRenderTargetView(swapChainResources_[1].Get(), &rtvDesc_, rtvHandles_[1]);

	/*RTVの作成*/
	//const Color kRenderTargetClearValue{ 1.0f,0.0f,0.0f,1.0f };
	renderTextureResource_ = CreateRenderTextureResource(device_, backBufferWidth_, backBufferHeight_,
		rtvDesc_.Format, kRenderTargetClearValue);
	// 3つ目のディスクリプタハンドルを得る（自力で）
	rtvHandles_[2].ptr = rtvHandles_[1].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	device_->CreateRenderTargetView(renderTextureResource_.Get(), &rtvDesc_, rtvHandles_[2]);

	
}
//D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
//	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
//	handleCPU.ptr += (descriptorSize * index);
//	return handleCPU;
//}
//D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
//	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
//	handleGPU.ptr += (descriptorSize * index);
//	return handleGPU;
//}

//void DirectXCommon::CreateHandles() {
//	// SRVを作成するDescriptorHeapの場所を決める
//	textureSrvHandleCPU_[0] = srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
//	textureSrvHandleGPU_[0] = srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart();
//	// 先頭はImGuiが使っているのでその次を使う
//	textureSrvHandleCPU_[0].ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
//	textureSrvHandleGPU_[0].ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
//	// SRVの作成
//	device_->CreateShaderResourceView(textureResource_[0].Get(), &srvDesc_, textureSrvHandleCPU_[0]);
//	// SRVを作成するDescriptorHeapの場所を決める
//	textureSrvHandleCPU_[1] = GetCPUDescriptorHandle(srvDescriptorHeap_.Get(), descriptorSizeSRV_, 2);
//	textureSrvHandleGPU_[1] = GetGPUDescriptorHandle(srvDescriptorHeap_.Get(), descriptorSizeSRV_, 2);
//	// SRVの作成
//	device_->CreateShaderResourceView(textureResource_[1].Get(), &srvDesc_, textureSrvHandleCPU_[1]);
//}
/// <summary>
/// 描画前処理
/// </summary>
void DirectXCommon::PreDraw() {
	
	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	/*レンダーターゲットをコピーソースに*/
	BarrierTransition(renderTextureResource_.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	/*デプスリソース遷移*/
	BarrierTransition(depthStencilResource_.Get(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// TransitionBarrierの設定
	BarrierTransition(swapChainResources_[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	
	// 描画先のRTVとDSVを設定する
	//D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, nullptr);

	// 指定した色で画面全体をクリアする
	float clearColor[] = { 0.0f,0.0f,0.0f,0.0f };
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearColor, 0, nullptr);

	// 指定した深度で画面全体をクリアする
	//commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// ビューポートの設定
	D3D12_VIEWPORT viewport =
		D3D12_VIEWPORT(0.0f, 0.0f, float(backBufferWidth_), float(backBufferHeight_),0.0f,1.0f);
	commandList_->RSSetViewports(1, &viewport);// Viewportを設定

	// シザリング矩形の設定
	D3D12_RECT rect = D3D12_RECT(0, 0, backBufferWidth_, backBufferHeight_);
	commandList_->RSSetScissorRects(1, &rect);// Scirssorを設定

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	materialData_->time += 1.0f / 60.0f;
	
	//commandList_->SetGraphicsRootSignature(GraphicsPipelineState::GetInstance()->GetRootSignatureRandom());
	//commandList_->SetPipelineState(GraphicsPipelineState::GetInstance()->GetPipelineStateRandom(0));
	//commandList_->SetGraphicsRootSignature(GraphicsPipelineState::GetInstance()->GetRootSignatureCopy());
	//commandList_->SetPipelineState(GraphicsPipelineState::GetInstance()->GetPipelineStateCopy(0));
	commandList_->SetGraphicsRootSignature(GraphicsPipelineState::GetInstance()->GetRootSignatureHSV());
	commandList_->SetPipelineState(GraphicsPipelineState::GetInstance()->GetPipelineStateHSV(0));
	//commandList_->SetGraphicsRootSignature(GraphicsPipelineState::GetInstance()->GetRootSignatureDepthFilter());
	//commandList_->SetPipelineState(GraphicsPipelineState::GetInstance()->GetPipelineStateDepthFilter(0));
	//commandList_->SetGraphicsRootSignature(GraphicsPipelineState::GetInstance()->GetRootSignatureDissolve());
	//commandList_->SetPipelineState(GraphicsPipelineState::GetInstance()->GetPipelineStateDissolve(0));
	commandList_->SetGraphicsRootDescriptorTable(0, instancingSrvHandleGPU_);
	commandList_->SetGraphicsRootConstantBufferView(1, HSVResource_->GetGPUVirtualAddress());
	//commandList_->SetGraphicsRootConstantBufferView(1, materialResource_->GetGPUVirtualAddress());
	//commandList_->SetGraphicsRootDescriptorTable(1, TextureManager::GetInstance()->GetTextureHandle(textureHandle_));
	//commandList_->SetGraphicsRootDescriptorTable(2, depthSrvHandleGPU_);
	//commandList_->SetGraphicsRootConstantBufferView(1, viewProjection_->GetProjectionInverseResource()->GetGPUVirtualAddress());

	commandList_->DrawInstanced(3, 1, 0, 0);
}

/// <summary>
/// 描画後処理
/// </summary>
void DirectXCommon::PostDraw() {

	HRESULT hr;
	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	/*元に戻す*/
	BarrierTransition(renderTextureResource_.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

	BarrierTransition(depthStencilResource_.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);

	// TransitionBarrierの設定
	BarrierTransition(swapChainResources_[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
	hr = commandList_->Close();
	assert(SUCCEEDED(hr));

	// GPUにコマンドリストの実行を行わFせる
	Microsoft::WRL::ComPtr < ID3D12CommandList> commandLists[] = { commandList_};
	commandQueue_->ExecuteCommandLists(1, commandLists->GetAddressOf());

	// GPUとOSに画面の交換を行うように通知する
	swapChain_->Present(1, 0);

	// Fenceの値を更新
	fenceValue_++;

	// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceValue_);

	// Fenceの値が指定したSignal値にたどり着いているか確認する
	// GetCompletedValueの初期値はFence作成時に渡した初期値
	if (fence_->GetCompletedValue() < fenceValue_) {

		// 指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);

		// イベント待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}
	// FPS固定更新
	UpdateFixFPS();
	// 次のフレーム用のコマンドリストを準備
	hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));
}
void DirectXCommon::PreRTVDraw()
{
	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	// TransitionBarrierの設定
	/*BarrierTransition(renderTextureResource_.Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);*/

	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	commandList_->OMSetRenderTargets(1, &rtvHandles_[2], false, &dsvHandle);

	// 指定した色で画面全体をクリアする
	//float clearColor[] = { 0.0f,0.0f,0.0f,0.0f };
	float clearColor[] = {
		kRenderTargetClearValue.r,
		kRenderTargetClearValue.g,
		kRenderTargetClearValue.b,
		kRenderTargetClearValue.a,
	};
	commandList_->ClearRenderTargetView(rtvHandles_[2], clearColor, 0, nullptr);

	// 指定した深度で画面全体をクリアする
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// ビューポートの設定
	D3D12_VIEWPORT viewport =
		D3D12_VIEWPORT(0.0f, 0.0f, float(backBufferWidth_), float(backBufferHeight_), 0.0f, 1.0f);
	commandList_->RSSetViewports(1, &viewport);// Viewportを設定

	// シザリング矩形の設定
	D3D12_RECT rect = D3D12_RECT(0, 0, backBufferWidth_, backBufferHeight_);
	commandList_->RSSetScissorRects(1, &rect);// Scirssorを設定

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
}
void DirectXCommon::PostRTVDraw()
{
	HRESULT hr;
	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	// TransitionBarrierの設定
	BarrierTransition(renderTextureResource_.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
	hr = commandList_->Close();
	assert(SUCCEEDED(hr));

	// GPUにコマンドリストの実行を行わFせる
	Microsoft::WRL::ComPtr < ID3D12CommandList> commandLists[] = { commandList_ };
	commandQueue_->ExecuteCommandLists(1, commandLists->GetAddressOf());

	// GPUとOSに画面の交換を行うように通知する
	swapChain_->Present(1, 0);

	// Fenceの値を更新
	fenceValue_++;

	// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceValue_);

	// Fenceの値が指定したSignal値にたどり着いているか確認する
	// GetCompletedValueの初期値はFence作成時に渡した初期値
	if (fence_->GetCompletedValue() < fenceValue_) {

		// 指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);

		// イベント待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}
	// FPS固定更新
	UpdateFixFPS();
	// 次のフレーム用のコマンドリストを準備
	hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));
}
Microsoft::WRL::ComPtr < ID3D12Resource> DirectXCommon::CreateBufferResource(ID3D12Device* device, const size_t& sizeInBytes) {
	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;// UploadHeapを使う
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes;// リソースのサイズ。今回はVector4を３頂点分
	// バッファの場合はこれらは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	// バッファの場合ははこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));
	return vertexResource;
}
void DirectXCommon::FenceClose() {
	CloseHandle(fenceEvent_);
}

/// <summary>
/// ディスクリプタヒープの生成
/// </summary>
/// <param name="device"></param>
/// <param name="heapType"></param>
/// <param name="numDescriptors"></param>
/// <param name="shaderVisible"></param>
/// <returns></returns>
Microsoft::WRL::ComPtr < ID3D12DescriptorHeap> DirectXCommon::CreateDescriptorHeap(
	ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
	// ディスクリプタヒープの生成
	Microsoft::WRL::ComPtr< ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;// レンダーターゲットビュー用
	descriptorHeapDesc.NumDescriptors = numDescriptors;// ダブルバッファ用に２つ。多くても別に構わない
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));

	// ディスクリプタヒープが作れなかったので起動できない
	assert(SUCCEEDED(hr));

	return descriptorHeap;
}
int32_t DirectXCommon::GetBackBufferWidth() const { return backBufferWidth_; }

int32_t DirectXCommon::GetBackBufferHeight() const { return backBufferHeight_; }



Microsoft::WRL::ComPtr < ID3D12Resource	> DirectXCommon::CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height)
{
	// 生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;// Textureの幅
	resourceDesc.Height = height;// Textureの高さ
	resourceDesc.MipLevels = 1;//mipmapの数
	resourceDesc.DepthOrArraySize = 1;// 奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;// サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;// 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;// DepthStencilとして使う通知

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;// VRAM上に作る

	// 深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;// 1.0f（最大値）でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// フォーマット。Resourceと合わせる

	// Resourceの生成
	Microsoft::WRL::ComPtr < ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,// Heapの設定
		D3D12_HEAP_FLAG_NONE,// Heapの特殊な設定。特になし
		&resourceDesc,// Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,// 深度値を書き込む状態にしておく
		&depthClearValue,// Clear最適値
		IID_PPV_ARGS(&resource));// 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}
/// <summary>
/// 深度リソースの作成
/// </summary>
void DirectXCommon::CreateDepthStencil() {
	// DSV用のヒープでディスクリプタの数は1。DSVはShader内で触るものではないので、ShaderVisibleはfalse
	dsvDescriptorHeap_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	descriptorSizeDSV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	depthStencilResource_ = CreateDepthStencilTextureResource(device_.Get(), WinApp::kClientWidth, WinApp::kClientHeight);

	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// Format。基本的にはResourceと合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;// 2dTexture
	// DSVHeapの先頭にDSVを作る
	device_->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());
}
D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}
D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}
// FPS固定初期化
void DirectXCommon::InitializeFixFPS() {
	// 現在時間を記録
	reference_ = std::chrono::steady_clock::now();
}
// FPS固定更新
void DirectXCommon::UpdateFixFPS() {
	// 1/60秒ぴったりの時間
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
	// 1/60秒よりわずかに短い時間
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

	// 現在時間を取得する
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	// 前回記録からの経過時間
	std::chrono::microseconds elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);
	// 1/60秒（よりわずかに短い時間）立っていない場合
	if (elapsed < kMinCheckTime) {
		// 1/60秒経過するまで微小なスリープを繰り返す
		while (std::chrono::steady_clock::now() - reference_ < kMinTime)
		{
			// 1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}

	}
	// 現在の時間を記録する
	reference_ = std::chrono::steady_clock::now();
}

Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateRenderTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t width, uint32_t height, DXGI_FORMAT format, const Color& clearColor)
{
	// 生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;// Textureの幅
	resourceDesc.Height = height;// Textureの高さ
	resourceDesc.MipLevels = 1;//mipmapの数
	resourceDesc.DepthOrArraySize = 1;// 奥行き or 配列Textureの配列数
	resourceDesc.Format = format;// DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;// サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;// 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;// RenderTargetとして利用可能にする

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;// VRAM上に作る

	// 深度値のクリア設定
	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = format;
	clearValue.Color[0] = clearColor.r;
	clearValue.Color[1] = clearColor.g;
	clearValue.Color[2] = clearColor.b;
	clearValue.Color[3] = clearColor.a;

	// Resourceの生成
	Microsoft::WRL::ComPtr < ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,//これから描画することを前提としたTextureなのでRenderTargetとして使うとこから始める
		&clearValue,// Clear最適値。ClearRenderTargetをこの色でClearするようにする。最適化されているので高速である
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(hr));
	return resource;
}

void DirectXCommon::CreateRenderTexture()
{
	/*SRVの作成*/
	srvIndex_ = SrvManager::GetInstance()->Allocate();
	instancingSrvHandleCPU_ = SrvManager::GetInstance()->GetCPUDescriptorHandle(srvIndex_);
	instancingSrvHandleGPU_ = SrvManager::GetInstance()->GetGPUDescriptorHandle(srvIndex_);
	SrvManager::GetInstance()->CreateSRVforTexture2D(srvIndex_, renderTextureResource_.Get(), DXGI_FORMAT_R8G8B8A8_UNORM, 1);
	/*グラフィックパイプライン生成*/
	GraphicsPipelineState::GetInstance()->CreateGraphicsPipelineCopy(device_.Get());
}

//Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateDepthTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t width, uint32_t height, DXGI_FORMAT format)
//{
//	// 1.metadataを基にResourceの設定
//	D3D12_RESOURCE_DESC resourceDesc{};
//	resourceDesc.Width = width;// Textureの幅
//	resourceDesc.Height = height;// Textureの高さ
//	resourceDesc.MipLevels = 1;// mipmapの数
//	resourceDesc.DepthOrArraySize = 1;// 奥行き or 配列Textureの配列数
//	resourceDesc.Format = format;// TextureのFormat
//	resourceDesc.SampleDesc.Count = 1;// サンプリングカウント。1固定。
//	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;// 2次元
//	resourceDesc.state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
//	// 利用するHeapの設定
//	D3D12_HEAP_PROPERTIES heapProperties{};
//	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;// VRAM上に作る
//	// 3.Resourceを生成する
//	Microsoft::WRL::ComPtr < ID3D12Resource> resource = nullptr;
//	HRESULT hr = device->CreateCommittedResource(
//		&heapProperties,// Heapの設定
//		D3D12_HEAP_FLAG_NONE,// Heapの特殊な設定。特になし
//		&resourceDesc,// Resourceの設定
//		//D3D12_RESOURCE_STATE_GENERIC_READ,// 初回のResourceState。Textureは基本読むだけ
//		D3D12_RESOURCE_STATE_COPY_DEST,// 初回のResourceState。Textureは基本読むだけ
//		nullptr,// Clear最適値。使わないのでnullptr
//		IID_PPV_ARGS(&resource));// 作成するResourceポインタのポインタ
//	assert(SUCCEEDED(hr));
//	return resource;
//}

void DirectXCommon::CreateDepthTexture()
{
	/*SRVの作成*/
	depthSrvIndex_ = SrvManager::GetInstance()->Allocate();
	depthSrvHandleCPU_ = SrvManager::GetInstance()->GetCPUDescriptorHandle(depthSrvIndex_);
	depthSrvHandleGPU_ = SrvManager::GetInstance()->GetGPUDescriptorHandle(depthSrvIndex_);
	SrvManager::GetInstance()->CreateSRVforTexture2D(depthSrvIndex_, depthStencilResource_.Get(), DXGI_FORMAT_R24_UNORM_X8_TYPELESS, 1);
	/*グラフィックパイプライン生成*/
	GraphicsPipelineState::GetInstance()->CreateGraphicsPipelineDepthFilter(device_.Get());
	GraphicsPipelineState::GetInstance()->CreateGraphicsPipelineDissolve(device_.Get());
	GraphicsPipelineState::GetInstance()->CreateGraphicsPipelineRandom(device_.Get());
	GraphicsPipelineState::GetInstance()->CreateGraphicsPipelineHSV(device_.Get());
}

void DirectXCommon::BarrierTransition( ID3D12Resource* pResource,
	D3D12_RESOURCE_STATES Before, D3D12_RESOURCE_STATES After)
{
	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};

	// 今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;

	// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

	// バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = pResource;

	// 遷移前（現在）のResourceState
	barrier.Transition.StateBefore = Before;

	// 遷移後のResourceState
	barrier.Transition.StateAfter = After;

	// TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier);
}

void DirectXCommon::CreateMtlResource()
{
	// マテリアル用のリソースを作る。今回はcolor1tu分のサイズを用意する
	materialResource_ = CreateBufferResource(device_.Get(), sizeof(MtlData));
	// マテリアルデータを書き込む
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->time = 0.0f;
}

void DirectXCommon::CreateHSVResource()
{
	// マテリアル用のリソースを作る。今回はcolor1tu分のサイズを用意する
	HSVResource_ = CreateBufferResource(device_.Get(), sizeof(HSVData));
	// マテリアルデータを書き込む
	// 書き込むためのアドレスを取得
	HSVResource_->Map(0, nullptr, reinterpret_cast<void**>(&HSVData_));
	HSVData_->hue = 0.0f;
	HSVData_->saturation = 0.0f;
	HSVData_->value = 0.0f;

}
