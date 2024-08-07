#include "PointLight.h"
#include"DirectXCommon.h"
void PointLight::CreatePointLightResource() {
	// Light用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	PointLightResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(PointLightData));
	// データを書き込む

	// 書き込むためのアドレスを取得
	PointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&PointLightData_));
	// 単位行列を書き込んでいく
	// デフォルト値
	PointLightData_->color = Color{ 1.0f,1.0f,1.0f,1.0f };
	PointLightData_->position = { 0.0f,2.0f,0.0f };
	PointLightData_->intensity = 0.0f;
	PointLightData_->decay = 1.0f;
	PointLightData_->radius = 10.0f;
}