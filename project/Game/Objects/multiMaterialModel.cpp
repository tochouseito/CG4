#include "multiMaterialModel.h"
#include<assert.h>
#include"imgui.h"
void multiMaterialModel::Initialize(Model* model, std::string textureHandle, ViewProjection* viewProjection)
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
	worldTransform_.translation_.x -= 10.0f;
}

void multiMaterialModel::Updata()
{
#ifdef _DEBUG
	// ImGuiフレーム開始
	ImGui::Begin("3Dobjects");
	if (ImGui::CollapsingHeader("multiMaterial")) {
		ImGui::DragFloat3("translate##multiMaterial", &worldTransform_.translation_.x, 0.01f);
		ImGui::DragFloat3("Rotate##multiMaterial", &worldTransform_.rotation_.x, 0.01f);
		ImGui::DragFloat3("Scale##multiMaterial", &worldTransform_.scale_.x, 0.01f);
	}
	ImGui::End();
#endif
	/*行列を定数バッファに更新、転送*/
	worldTransform_.UpdataMatrix();
}

void multiMaterialModel::Draw()
{
	/*3Dモデルを描画*/
	model_->Draw(worldTransform_, *viewProjection_, textureHandle_);
}
