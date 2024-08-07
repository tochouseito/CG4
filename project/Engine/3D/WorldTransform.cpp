#include "WorldTransform.h"
#include"ViewProjection.h"
#include"DirectXCommon.h"
#include<cassert>
#include"Mymath.h"
#include"SrvManager.h"

/// <summary>
/// 初期化
/// </summary>
void WorldTransform::Initialize() {
	rootMatrix_ = MakeIdentity4x4();
	CreateConstBufferResource(numInstance_);
	Map();
	srvIndex = SrvManager::GetInstance()->Allocate();
	instancingSrvHandleCPU_ = SrvManager::GetInstance()->GetCPUDescriptorHandle(srvIndex);
	instancingSrvHandleGPU_ = SrvManager::GetInstance()->GetGPUDescriptorHandle(srvIndex);
	SrvManager::GetInstance()->CreateSRVforStructuredBuffer(srvIndex, worldResource_.Get(), kNumMaxInstance_, sizeof(ConstBufferDataWorldTransform));
}
void WorldTransform::CreateConstBufferResource(uint32_t Instance) {
	if (Instance <= 0) {
		Instance = 1;
	}
	kNumMaxInstance_ = Instance;
	// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	worldResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(ConstBufferDataWorldTransform)*Instance);
	// 書き込むためのアドレスを取得
	worldResource_->Map(0, nullptr, reinterpret_cast<void**>(&worldData_));
	// 単位行列を書き込んでいく
	for (uint32_t index = 0; index < Instance; ++index) {
		worldData_[index].matWorld = MakeIdentity4x4();
		worldData_[index].worldInverse = MakeIdentity4x4();
		worldData_[index].rootNode = MakeIdentity4x4();
	}
}
/// <summary>
/// マッピングする
/// </summary>
void WorldTransform::Map() {
	
}
/// <summary>
/// 行列を転送する
/// </summary>
void WorldTransform::TransferMatrix() {
	if (numInstance_ <= kNumMaxInstance_) {
		worldData_[numInstance_].matWorld = matWorld_;
		worldData_[numInstance_].worldInverse = Transpose(Inverse(matWorld_));
		worldData_[numInstance_].rootNode = rootMatrix_;
	}
}
/// <summary>
/// 行列の更新
/// </summary>
void WorldTransform::UpdataMatrix() {
	matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);

	TransferMatrix();
}

