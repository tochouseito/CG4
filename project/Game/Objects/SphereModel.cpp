#include "SphereModel.h"
#include"assert.h"
#include"imgui.h"
/// <summary>
/// 初期化
/// </summary>
void SphereModel::Initialize(Model* model, std::string textureHandle, ViewProjection* viewProjection) {
	/*Modelがなかったら止める*/
	assert(model);
	model_ = model;
	/*テクスチャ取得*/
	textureHandle_ = textureHandle;
	/*ViewProjectionのポインタを格納*/
	viewProjection_ = viewProjection;
	/*ワールドトランスフォームの初期化*/
	worldTransform_.Initialize();
	model_->GetMaterial()->GetMaterialData()->enableLighting = true;
}

/// <summary>
/// 更新
/// </summary>
void SphereModel::Update() {
#ifdef _DEBUG
	// ImGuiフレーム開始
	ImGui::Begin("3Dobjects");
	if (ImGui::CollapsingHeader("Sphere")) {
		ImGui::DragFloat3("translate##Sphere", &worldTransform_.translation_.x, 0.01f);
		ImGui::DragFloat3("Rotate##Sphere", &worldTransform_.rotation_.x, 0.01f);
		ImGui::DragFloat3("Scale##Sphere", &worldTransform_.scale_.x, 0.01f);
		if (ImGui::Button("Lighting##Sphere")) {
			if (model_->GetMaterial()->GetMaterialData()->enableLighting) {
				model_->GetMaterial()->GetMaterialData()->enableLighting = false;
			} else
			{
				model_->GetMaterial()->GetMaterialData()->enableLighting = true;
			}
		}
		//// コンボボックスを表示する
		//if (ImGui::BeginCombo("BlendMode##Sphere", model_->GetMaterial()->blendMode[current_blend].c_str())) {
		//	// アイテムリストをループ
		//	for (uint32_t n = 0; n < model_->GetMaterial()->blendMode.size(); n++) {
		//		bool is_selected = (current_blend == n);
		//		if (ImGui::Selectable(model_->GetMaterial()->blendMode[n].c_str(), is_selected)) {
		//			current_blend = n; // アイテムが選択された場合、インデックスを更新
		//		}
		//		if (is_selected) {
		//			ImGui::SetItemDefaultFocus(); // フォーカスを設定
		//		}
		//	}
		//	ImGui::EndCombo();
		//}
		//model_->SetBlendMode(current_blend);
	}
	ImGui::End();
#endif
	/*行列を定数バッファに更新、転送*/
	worldTransform_.UpdataMatrix();
}

/// <summary>
/// 描画
/// </summary>
void SphereModel::Draw() {
	/*3Dモデルを描画*/
	model_->Draw(worldTransform_, *viewProjection_, textureHandle_);
}