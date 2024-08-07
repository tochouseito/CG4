#pragma once
#include"WorldTransform.h"
#include"ViewProjection.h"
#include"Mesh.h"
#include"Material.h"
#include<string>
class Primitive
{
public:
	Primitive();
	~Primitive();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(std::string textureHandle, ViewProjection* viewProjection);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
private:

	WorldTransform worldTransform_;
	ViewProjection* viewProjection_;
	Mesh* mesh_ = nullptr;
	Material* material_ = nullptr;

	/*テクスチャハンドル*/
	std::string textureHandle_;

	/*ブレンドモード*/
	uint32_t current_blend = 0;

};

