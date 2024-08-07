#pragma once
#include"WorldTransform.h"
#include"ViewProjection.h"
#include"Model.h"
class suzanne
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, std::string textureHandle, ViewProjection* viewProjection);
	/// <summary>
	/// 更新
	/// </summary>
	void Updata();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	WorldTransform worldTransform_;
	ViewProjection* viewProjection_ = nullptr;
	Model* model_ = nullptr;
	std::string textureHandle_;
};

