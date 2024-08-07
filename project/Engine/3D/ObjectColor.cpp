#include "ObjectColor.h"
#include"DirectXCommon.h"
#include"SrvManager.h"
#include"Mymath.h"
/// <summary>
/// 初期化
/// </summary>
void ObjectColor::Initialize() {
	CreateColorResource(numInstance_);
	
	// SRV確保
	srvIndex = SrvManager::GetInstance()->Allocate();
	instancingSrvHandleCPU_ = SrvManager::GetInstance()->GetCPUDescriptorHandle(srvIndex);
	instancingSrvHandleGPU_ = SrvManager::GetInstance()->GetGPUDescriptorHandle(srvIndex);
	SrvManager::GetInstance()->CreateSRVforStructuredBuffer(srvIndex, colorResource_.Get(), kNumMaxInstance_, sizeof(ConstBufferDataColor));
}
void ObjectColor::CreateColorResource(uint32_t Instance) {
	kNumMaxInstance_ = Instance;
	// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	colorResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(ConstBufferDataColor) * Instance);
	// 書き込むためのアドレスを取得
	colorResource_->Map(0, nullptr, reinterpret_cast<void**>(&colorData_));
	// 単位行列を書き込んでいく
	for (uint32_t index = 0; index < Instance; ++index) {
		colorData_[index].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	}
}
/// <summary>
/// 行列を転送する
/// </summary>
void ObjectColor::TransferMatrix() {
	if (numInstance_ < kNumMaxInstance_) {
		colorData_[numInstance_].color = color_;
		colorData_[numInstance_].color.a = color_.a;
	}
}
