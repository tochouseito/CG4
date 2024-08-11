#pragma once
#include"WorldTransform.h"
#include"ViewProjection.h"
#include"Model.h"
class AnimatedCube
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, std::string textureHandle, ViewProjection* viewProjection,Model::Animation* animation);
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
	Model::Animation* animation_=nullptr;
	Model::Skeleton* skeleton_ = nullptr;
	ViewProjection* viewProjection_ = nullptr;
	Model* model_ = nullptr;
	std::string textureHandle_;
	float animationTime = 0.0f;
};

