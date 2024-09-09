#pragma once
#include <array>
#include<d3d12.h>
#include<DirectXTex.h>
#include<d3dx12.h>
#include <string>
#include <unordered_map>
#include <wrl.h>
#include<vector>
#include <cstdint>
#include<unordered_map>

class DirectXCommon;
class SrvManager;

/// <summary>
/// テクスチャマネージャ
/// </summary>
class TextureManager {
public:
	// デスクリプターの数
	static const size_t kNumDescriptors = 1024;

	/// <summary>
	/// テクスチャ
	/// </summary>
	struct Texture {
		DirectX::TexMetadata metadata;
		uint32_t srvIndex;
		// テクスチャリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;
		// シェーダリソースビューのハンドル(CPU)
		CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
		// シェーダリソースビューのハンドル(CPU)
		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	}; 
	

	Microsoft::WRL::ComPtr < ID3D12Resource> GetTextureResource(uint32_t handleNum)const { return textureResource_[handleNum]; }
	D3D12_SHADER_RESOURCE_VIEW_DESC GetSrvDesc()const { return srvDesc_; }

	/// <summary>
	/// 読み込み
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	/// <returns>テクスチャハンドル</returns>
	//static uint32_t Load(const std::string& filePath);
	static const std::string Load(const std::string& filePath);

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static TextureManager* GetInstance();

	/// <summary>
	/// システム初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	void Initialize(ID3D12Device* device, DirectXCommon* dxCommon,SrvManager* srvManager);

	

public:
	//DirectX::ScratchImage LordTexture(const std::string& filePath);
	
	Microsoft::WRL::ComPtr < ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);

	void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);
	Microsoft::WRL::ComPtr < ID3D12Resource> UploadTextureDataEx(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device,
		ID3D12GraphicsCommandList* commandList);

	// GPUハンドルの取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(const std::string& filePath);
	// メタデータの取得
	const DirectX::TexMetadata& GetMetaData(const std::string& filePath);
	// SRVインデックスの取得
	uint32_t GetSrvIndex(const std::string& filePath);

private:
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

	// DirectX基盤インスタンス（借りてくる）
	DirectXCommon* dxCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;
	// デバイス
	ID3D12Device* device_;
	// デスクリプタサイズ
	UINT sDescriptorHandleIncrementSize_ = 0u;
	// ディレクトリパス
	//static std::string& directoryPath_;
	// デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
	// SRV用ヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;
	uint32_t handle_ = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_ ;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_ ;
	Microsoft::WRL::ComPtr < ID3D12Resource> textureResource_[2];
	Microsoft::WRL::ComPtr < ID3D12Resource> intermediateResource_[2];
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc_{};

	// テクスチャコンテナ
	//Texture textures_[kNumDescriptors];
	std::unordered_map<std::string, Texture>textureDatas;
	/// <summary>
	/// 読み込み
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	//uint32_t LoadInternal(const std::string& fileName);
	const std::string LoadInternal(const std::string& fileName);
	
};

