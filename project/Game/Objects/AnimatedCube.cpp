#include "AnimatedCube.h"
#include<assert.h>
#include"imgui.h"
#include"Mymath.h"

void AnimatedCube::Initialize(Model* model, std::string textureHandle, ViewProjection* viewProjection, Model::Animation* animation)
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
	model_->SetAnimation(animation);
}

void AnimatedCube::Updata()
{
	animationTime += 1.0f / 60.0f;// 時刻を進める。1/60で固定してあるが、計測した時間を使って可変フレーム対応するほうが望ましい
	animationTime = std::fmod(animationTime, model_->GetAnimation()->duration);
	Model::NodeAnimation& rootNodeAnimation = model_->GetAnimation()->nodeAnimations[model_->GetModelData()->rootNode.name];// rootNodeのAnimationを取得
	Vector3 translate = model_->CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);// 指定時刻の値を取得。
	Quaternion rotate = model_->CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime);
	Vector3 scale = model_->CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);
	Matrix4x4 localMatrix = MakeAffineMatrix(scale, rotate, translate);
	worldTransform_.localMatrix_ = localMatrix;
#ifdef _DEBUG
	// ImGuiフレーム開始
	ImGui::Begin("3Dobjects");
	if (ImGui::CollapsingHeader("UtahTeapot")) {
		ImGui::DragFloat3("translate##UtahTeapot", &worldTransform_.translation_.x, 0.01f);
		ImGui::DragFloat3("Rotate##UtahTeapot", &worldTransform_.rotation_.x, 0.01f);
		ImGui::DragFloat3("Scale##UtahTeapot", &worldTransform_.scale_.x, 0.01f);
	}
	ImGui::End();
#endif
	/*行列を定数バッファに更新、転送*/
	//worldTransform_.UpdataMatrix();
	worldTransform_.UpdataAnimationMat();
}

void AnimatedCube::Draw()
{
	/*3Dモデルを描画*/
	model_->Draw(worldTransform_, *viewProjection_, textureHandle_);
}
