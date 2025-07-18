#include "SpotLight.h"
#include"DirectXCommon.h"
#include"Mymath.h"
#include<numbers>
void SpotLight::CreateSpotLightResource() {
	// Light用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	SpotLightResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(SpotLightData));
	// データを書き込む

	// 書き込むためのアドレスを取得
	SpotLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&SpotLightData_));
	// 単位行列を書き込んでいく
	// デフォルト値
	SpotLightData_->color = Color{ 1.0f,1.0f,1.0f,1.0f };
	SpotLightData_->position = { 2.0f,1.25f,0.0f };
	SpotLightData_->intensity = 4.0f;
	SpotLightData_->direction = Normalize(Vector3{ -1.0f,-1.0f,0.0f });
	SpotLightData_->distance = 7.0f;
	SpotLightData_->decay = 2.0f;
	SpotLightData_->cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
	SpotLightData_->cosFalloffStart = 1.0f;
}