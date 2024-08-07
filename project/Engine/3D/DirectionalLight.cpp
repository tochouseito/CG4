#include "DirectionalLight.h"
#include"DirectXCommon.h"
void DirectionalLight::CreateDirectionalLightResource() {
	// Light用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	directionalLightResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(DirectionalLightData));
	// データを書き込む
	
	// 書き込むためのアドレスを取得
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	// 単位行列を書き込んでいく
	// デフォルト値
	directionalLightData_->color = Color{ 1.0f,1.0f,1.0f,1.0f };
	directionalLightData_->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData_->intensity = 0.0f;
}