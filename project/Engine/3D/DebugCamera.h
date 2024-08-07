#pragma once
#include"ViewProjection.h"
#include"mathShapes.h"
#include"Vector3.h"
#include"Vector2.h"
#include"Matrix4x4.h"
class DebugCamera
{
public:
	// X,Y,Z軸回りのローカル回転角
	Vector3 rotation_ = { 0.0f, 0.0f, 0.0f };
	// ローカル座標
	Vector3 translation_ = { 0.0f, 0.0f, -50.0f };
	Matrix4x4 matRot_;
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const Vector3& position, ViewProjection* viewProjection);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	void CameraMove(Vector3& cameraRotate, Vector3& cameraTranslate, Vector2& clickPosition);
private:
	ViewProjection* viewProjection_;
	Vector2 mouse;
	bool useMouse = false;
};

