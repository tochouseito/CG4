#pragma once
#include"Vector3.h"
#include"Color.h"
#include"mathShapes.h"
#include"Matrix4x4.h"
#include<d3d12.h>
#include<wrl.h>

#include <type_traits>
#include<vector>

// 定数バッファ用データ構造体
struct ConstBufferDataWorldTransform {
	Matrix4x4 matWorld; // ローカル → ワールド変換行列
	Matrix4x4 worldInverse;
	Matrix4x4 rootNode;// モデルのRootMatrix
};
class WorldTransform
{
public:
	
private:// メンバ関数
	
public:
	
	// ローカルスケール
	Vector3 scale_ = { 1.0f, 1.0f, 1.0f };
	// X,Y,Z軸回りのローカル回転角
	Vector3 rotation_ = { 0.0f, 0.0f, 0.0f };
	// ローカル座標
	Vector3 translation_ = { 0.0f, 0.0f, 0.0f };
	// ローカル → ワールド変換行列
	Matrix4x4 matWorld_ = { 0.0f };

	Matrix4x4 localMatrix_ = { 0.0f };

	Quaternion quaternion_ = { 0.0f };

	Matrix4x4 skeletonSpaceMatrix;// skeletonSpaceでの変換行列

	struct QuaternionTransform {
		Vector3 scale;
		Quaternion rotate;
		Vector3 translate;
	};
	
	Matrix4x4 rootMatrix_;
	uint32_t numInstance_ = 0;
	WorldTransform() = default;
	~WorldTransform() = default;


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

	/// <summary>
	/// 行列の更新
	/// </summary>
	void UpdataMatrix();

	void UpdataAnimationMat();

	ConstBufferDataWorldTransform* GetCBVWorldData() { return worldData_; }

	void CreateConstBufferResource(uint32_t Instance = 1);
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU() { return instancingSrvHandleGPU_; }
private:
	
	// コピー禁止
	//WorldTransform(const WorldTransform&) = delete;
	//WorldTransform& operator=(const WorldTransform&) = delete;
	ConstBufferDataWorldTransform* worldData_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> worldResource_;
	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;
	uint32_t kNumMaxInstance_ = 0;
	uint32_t srvIndex = 0;
};

