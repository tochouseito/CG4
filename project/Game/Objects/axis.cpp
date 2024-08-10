#include "axis.h"
#include"assert.h"
#include"imgui.h"
/// <summary>
/// 初期化
/// </summary>
void axis::Initialize(Model* model, std::string textureHandle, ViewProjection* viewProjection) {
	/*Modelがなかったら止める*/
	assert(model);
	model_ = model;
	/*テクスチャ取得*/
	textureHandle_ = textureHandle;
	/*ViewProjectionのポインタを格納*/
	viewProjection_ = viewProjection;
	/*ワールドトランスフォームの初期化*/
	worldTransform_.Initialize();
	worldTransform_.rootMatrix_ = model_->GetModelData()->rootNode.localMatrix;
}

/// <summary>
/// 更新
/// </summary>
void axis::Update() {
#ifdef _DEBUG
	// ImGuiフレーム開始
	ImGui::Begin("3Dobjects");
	if (ImGui::CollapsingHeader("axis")) {
		ImGui::DragFloat3("translate##axis", &worldTransform_.translation_.x, 0.01f);
		ImGui::DragFloat3("Rotate##axis", &worldTransform_.rotation_.x, 0.01f);
		ImGui::DragFloat3("Scale##axis", &worldTransform_.scale_.x, 0.01f);
		if (ImGui::Button("Lighting##axis")) {
			if (model_->GetMaterial()->GetMaterialData()->enableLighting) {
				model_->GetMaterial()->GetMaterialData()->enableLighting = false;
			} else
			{
				model_->GetMaterial()->GetMaterialData()->enableLighting = true;
			}
		}
		// コンボボックスを表示する
		if (ImGui::BeginCombo("BlendMode##axis", model_->GetMaterial()->blendMode[current_blend].c_str())) {
			// アイテムリストをループ
			for (uint32_t n = 0; n < model_->GetMaterial()->blendMode.size(); n++) {
				bool is_selected = (current_blend == n);
				if (ImGui::Selectable(model_->GetMaterial()->blendMode[n].c_str(), is_selected)) {
					current_blend = n; // アイテムが選択された場合、インデックスを更新
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus(); // フォーカスを設定
				}
			}
			ImGui::EndCombo();
		}
		model_->SetBlendMode(current_blend);
	}
	ImGui::End();
#endif
	/*行列を定数バッファに更新、転送*/
	worldTransform_.UpdataMatrix();
}

/// <summary>
/// 描画
/// </summary>
void axis::Draw() {
	/*3Dモデルを描画*/
	model_->Draw(worldTransform_, *viewProjection_, textureHandle_);
}