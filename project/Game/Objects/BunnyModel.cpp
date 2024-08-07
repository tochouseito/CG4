#include "BunnyModel.h"
#include<assert.h>
#include"imgui.h"
void BunnyModel::Initialize(Model* model, std::string textureHandle, ViewProjection* viewProjection)
{
	/*Modelがなかったら止める*/
	assert(model);
	model_ = model;
	/*テクスチャ取得*/
	textureHandle_ = textureHandle;
	/*ViewProjectionのポインタを格納*/
	viewProjection_ = viewProjection;
	/*ワールドトランスフォームの初期化*/
	worldTransform_.Initialize();
	worldTransform_.translation_.x += 10.0f;
}

void BunnyModel::Updata()
{
#ifdef _DEBUG
	// ImGuiフレーム開始
	ImGui::Begin("3Dobjects");
	if (ImGui::CollapsingHeader("Bunny")) {
		ImGui::DragFloat3("translate##Bunny", &worldTransform_.translation_.x, 0.01f);
		ImGui::DragFloat3("Rotate##Bunny", &worldTransform_.rotation_.x, 0.01f);
		ImGui::DragFloat3("Scale##Bunny", &worldTransform_.scale_.x, 0.01f);
	}
	ImGui::End();
#endif
	/*行列を定数バッファに更新、転送*/
	worldTransform_.UpdataMatrix();
}

void BunnyModel::Draw()
{
	/*3Dモデルを描画*/
	model_->Draw(worldTransform_, *viewProjection_, textureHandle_);
}
