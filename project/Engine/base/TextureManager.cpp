#include "TextureManager.h"
#include"ConvertString.h"
#include <cassert>
#include <format>

#include "DirectXCommon.h"
#include"SrvManager.h"

const std::string TextureManager::Load(const std::string& fileName) {
	return TextureManager::GetInstance()->LoadInternal(fileName);
}
TextureManager* TextureManager::GetInstance() {
	static TextureManager instance;
	return &instance;
}

void TextureManager::Initialize(ID3D12Device* device, DirectXCommon* dxCommon,SrvManager* srvManager) {
	assert(device);
	srvManager_ = srvManager;
	device_ = device;
	//directoryPath_ = directoryPath;
	dxCommon_ = dxCommon;
}


//DirectX::ScratchImage TextureManager::LordTexture(const std::string& filePath)
//{
//	// テクスチャファイルを読んでプログラムで扱えるようにする	
//	DirectX::ScratchImage image{};
//	std::wstring filePathW = ConvertString(filePath);
//	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
//	assert(SUCCEEDED(hr));
//
//	// ミップマップの生成	
//	DirectX::ScratchImage mipImages{};
//	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
//	assert(SUCCEEDED(hr));
//
//	// ミップマップ付きのデータを返す
//	return mipImages;
//}
Microsoft::WRL::ComPtr < ID3D12Resource> TextureManager::CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
{
	// 1.metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);// Textureの幅
	resourceDesc.Height = UINT(metadata.height);// Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);// mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);// 奥行き or 配列Textureの配列数
	resourceDesc.Format = metadata.format;// TextureのFormat
	resourceDesc.SampleDesc.Count = 1;// サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);// Textureの次元数。普段使っているのは2次元
	// 2.利用するHeapの設定 非常に特殊な運用。02_04exで一般的なケース版がある
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;// 細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;// WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;// プロセッサの近くに配置
	//heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;// 細かい設定を行う
	//heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;// WriteBackポリシーでCPUアクセス可能
	//heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;// プロセッサの近くに配置
	// 3.Resourceを生成する
	Microsoft::WRL::ComPtr < ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,// Heapの設定
		D3D12_HEAP_FLAG_NONE,// Heapの特殊な設定。特になし
		&resourceDesc,// Resourceの設定
		//D3D12_RESOURCE_STATE_GENERIC_READ,// 初回のResourceState。Textureは基本読むだけ
		D3D12_RESOURCE_STATE_COPY_DEST,// 初回のResourceState。Textureは基本読むだけ
		nullptr,// Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource));// 作成するResourceポインタのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}
void TextureManager::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages)
{
	// Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	// 全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
		// MipMpLevelを指定して各Imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		// Textureに転送
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,// 全領域へコピー
			img->pixels,// 元データアドレス
			UINT(img->rowPitch),// 1ラインサイズ
			UINT(img->slicePitch)// 1枚サイズ
		);
		assert(SUCCEEDED(hr));
	}
}
[[nodiscard]]
Microsoft::WRL::ComPtr < ID3D12Resource> TextureManager::UploadTextureDataEx(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device,
	ID3D12GraphicsCommandList* commandList)
{
	std::vector<D3D12_SUBRESOURCE_DATA>subresources;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	Microsoft::WRL::ComPtr < ID3D12Resource> intermediateResource = dxCommon_->CreateBufferResource(device, intermediateSize).Get();
	UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
	// Textureへの転送後は利用できるよう,D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);
	return intermediateResource;
}
const std::string TextureManager::LoadInternal(const std::string& filePath) {
	// 読み込み済みテクスチャを検索
	if (textureDatas.contains(filePath)) {
		return filePath;
	}
	// テクスチャデータを追加して書き込む
	Texture& textureData = textureDatas[filePath];
	
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	//ID3D12Device* device = dxCommon_->GetDevice();
	// Textureを読んで転送する
	//DirectX::ScratchImage mipImages = LordTexture(filePath);
	// テクスチャファイルを読んでプログラムで扱えるようにする	
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミップマップの生成	
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//textures_[DiscNum].resource = CreateTextureResource(device_, metadata);
	textureData.resource = CreateTextureResource(device_, metadata);
	//UploadTextureData(textureResource_.Get(), mipImages);
	textureData.intermediateResource = UploadTextureDataEx(textureData.resource.Get(), mipImages, device_, commandList);
	
	// metadataを基にSRVの設定
	srvDesc_.Format = metadata.format;
	srvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;// 2Dのテクスチャ
	srvDesc_.Texture2D.MipLevels = UINT(metadata.mipLevels);

	// SRV確保
	textureData.srvIndex = srvManager_->Allocate();
	textureData.cpuDescHandleSRV = srvManager_->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.gpuDescHandleSRV = srvManager_->GetGPUDescriptorHandle(textureData.srvIndex);
	textureData.metadata = metadata;

	// テクスチャ枚数上限チェック
	assert(srvManager_->TextureMaxCheck());

	srvManager_->CreateSRVforTexture2D(textureData.srvIndex, textureData.resource.Get(), metadata.format, UINT(metadata.mipLevels));

	return filePath;
}


// GPUハンドルの取得
D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetTextureHandle(const std::string& filePath) {
	return textureDatas[filePath].gpuDescHandleSRV;
}
// メタデータの取得
const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath) {
	return textureDatas[filePath].metadata;
}
// SRVインデックスの取得
uint32_t TextureManager::GetSrvIndex(const std::string& filePath) {
	return textureDatas[filePath].srvIndex;
}