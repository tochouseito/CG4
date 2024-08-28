#pragma once
#include"Vector3.h"
#include"Color.h"
#include"Matrix4x4.h"
#include<d3d12.h>
#include<wrl.h>
#include<numbers>
// 定数バッファ用データ構造体
struct ConstBufferDataViewProjection {
	Matrix4x4 View;
	Matrix4x4 Projection;
	Vector3 cameraPosition;
};

struct CBProjection {
	Matrix4x4 ProjectionInverse;
};
class ViewProjection
{
public:
	ViewProjection() = default;
	~ViewProjection() = default;
	// X,Y,Z軸回りのローカル回転角
	Vector3 rotation_ = { 0.0f, 0.0f, 0.0f };
	// ローカル座標
	Vector3 translation_ = { 0.0f, 0.0f, -50.0f };
	
	struct wvpData {
		Matrix4x4 WVP_;
		Matrix4x4 World_;
	};
	
	Matrix4x4 matWorld;
	Matrix4x4 cameraMatrix_;

	uint32_t kNumInstance_ = 0;// インスタンス数
	const uint32_t kNumMaxInstance_ = 10;// インスタンス数
	Matrix4x4 worldViewProjectionMatrix_ = { 0 };
	Matrix4x4 viewMatrix_ = { 0 };
	Matrix4x4 projectionMatrix_ = { 0 };
	// 垂直方向視野角
	float fovAngleY = 45.0f * std::numbers::pi_v<float> / 180.0f;
	// ビューポートのアスペクト比
	float aspectRatio = (float)16 / 9;
	// 深度限界（手前側）
	float nearZ = 0.1f;
	// 深度限界（奥側）
	float farZ = 1000.0f;
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// マッピングする
	/// </summary>
	void Map();
	/// <summary>
	/// 行列を転送する
	/// </summary>
	void TransferMatrix();

	void UpdateMatrix();


	ID3D12Resource* GetWvpResource() { return wvpResource_.Get(); }
	ID3D12Resource* GetProjectionInverseResource() { return projectionInverseResource_.Get(); }

	uint32_t GetNumInstance() { return kNumInstance_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU() { return instancingSrvHandleGPU_; }
	void CreateConstBufferResource(uint32_t Instance = 1);
	void CBProjectionResource(uint32_t Instance = 1);
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> projectionInverseResource_;
	ConstBufferDataViewProjection* wvpData_ = nullptr;
	CBProjection* projectionInverse_ = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;
};

