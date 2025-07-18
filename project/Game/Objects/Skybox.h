#pragma once
#include"Model.h"
#include"WorldTransform.h"
#include"ViewProjection.h"
class Skybox
{
public:
	Skybox() = default;
	~Skybox() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, std::string textureHandle, ViewProjection* viewProjection);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
private:
	ViewProjection* viewProjection_;
	WorldTransform worldTransform_;
	/*3Dモデル*/
	Model* model_ = nullptr;
	/*テクスチャハンドル*/
	std::string textureHandle_;
	// 現在選択されているアイテムのインデックス
	uint32_t current_blend = 0;
};

