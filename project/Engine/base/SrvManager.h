#pragma once
#include <cstdint>
#include<d3d12.h>
#include<wrl.h>
#include"fstream"
#include"sstream"
class DirectXCommon;
class SrvManager
{
public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static SrvManager* GetInstance();
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon"></param>
	void Initialize(DirectXCommon* dxCommon);

	uint32_t Allocate();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

	// SRV作成
	void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);
	// SRV生成(StructuredBuffer用)
	void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structuredByteStride);
	
	void PreDraw();

	void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIIndex);

	bool TextureMaxCheck();

	/*ディスクリプタヒープを取得する*/
	ID3D12DescriptorHeap* GetDescriptorHeap() { return descriptorHeap.Get(); }

private:
	SrvManager() = default;
	~SrvManager() = default;
	SrvManager(const SrvManager&) = delete;
	const SrvManager& operator=(const SrvManager&) = delete;

	DirectXCommon* dxCommon_ = nullptr;

	// 最大SRV数(最大テクスチャ数、StructuredBuffer数)
	static const uint32_t kMaxSRVCount;
	// SRV用のデスクリプタサイズ
	uint32_t descriptorSize;
	// SRV用のデスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	// 次に使用するSRVインデックス
	uint32_t useIndex = 1;

	
};

