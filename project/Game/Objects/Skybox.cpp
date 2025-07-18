#include "Skybox.h"
#include"assert.h"
#include"imgui.h"
void Skybox::Initialize(Model* model, std::string textureHandle, ViewProjection* viewProjection)
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
	worldTransform_.scale_ = { 10.0f,10.0f ,10.0f };
}

void Skybox::Update()
{
	/*行列を定数バッファに更新、転送*/
	worldTransform_.UpdataMatrix();
}

void Skybox::Draw()
{
	/*3Dモデルを描画*/
	model_->DrawSkybox(worldTransform_, *viewProjection_, textureHandle_);
}
