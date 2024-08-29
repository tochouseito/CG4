#include "DebugCamera.h"
#include"DirectXCommon.h"
#include"Input.h"
#include"Mymath.h"
#include"imgui.h"

void DebugCamera::Initialize(const Vector3& position, ViewProjection* viewProjection)
{
	viewProjection_ = viewProjection;
	//viewProjection_->translation_ = position;
	translation_ = position;
	rotation_.y = 0.0f;// 3.14f;
	matRot_ = MakeIdentity4x4();
}

void DebugCamera::Update()
{
	
	rotation_ = { 0.0f,0.0f,0.0f };
	Vector3 offset = translation_;// { 0.0f,0.0f,translation_.z };
	if (useMouse) {
		CameraMove(rotation_, translation_, mouse);
	}
	/*追加分の回転行列を生成*/
	Matrix4x4 matRotDelta = MakeIdentity4x4();
	matRotDelta *= MakeRotateXMatrix(rotation_.x);
	matRotDelta *= MakeRotateYMatrix(rotation_.y);
	//matRot_ = MakeRotateXYZMatrix(rotation_);
	matRot_ = Multiply(matRotDelta, matRot_);
	offset = TransformNormal(offset, matRot_);
	Matrix4x4 scaleMatrix = MakeScaleMatrix(Vector3(1.0f, 1.0f, 1.0f));
	Matrix4x4 rotateXYZMatrix = matRot_;
	Matrix4x4 translateMatrix = MakeTranslateMatrix(offset);
	Matrix4x4 cameraMatrix = Multiply(scaleMatrix, Multiply(rotateXYZMatrix, translateMatrix));
	//Matrix4x4 cameraMatrix = Multiply(translateMatrix, Multiply(rotateXYZMatrix, scaleMatrix));
	viewProjection_->cameraMatrix_ = cameraMatrix;
	viewProjection_->viewMatrix_ = Inverse(cameraMatrix);
	viewProjection_->projectionMatrix_ = MakePerspectiveFovMatrix(0.45f,
		float(DirectXCommon::GetInstance()->GetBackBufferWidth()) / float(DirectXCommon::GetInstance()->GetBackBufferHeight()),
		0.1f, 100.0f);
	//viewProjection_->translation_ =  offset;
	//viewProjection_->rotation_ = rotation_;
#ifdef _DEBUG
	ImGui::Begin("DebugCamera");
	ImGui::DragFloat3("translation", &translation_.x, 0.01f);
	ImGui::DragFloat3("rotation", &rotation_.x, 0.01f);
	ImGui::Checkbox("useMouse", &useMouse);
	ImGui::End();
#endif // _DEBUG
}

void DebugCamera::CameraMove(Vector3& cameraRotate, Vector3& cameraTranslate, Vector2& clickPosition)
{
	// カーソルを動かすときの感度
	const float mouseSensitivity = 0.003f;
	// カメラの移動速度
	const float moveSpeed = 0.005f;

	// 各フラグ
	static bool isLeftClicked = false;
	static bool isWheelClicked = false;
	//static bool isDebugCamera = false;

	// 回転を考慮する
	Matrix4x4 rotationMatrix = MakeRotateXYZMatrix(cameraRotate);
	Vector3 X = { 1.0f, 0.0f, 0.0f };
	Vector3 Y = { 0.0f, 1.0f, 0.0f };
	Vector3 Z = { 0.0f, 0.0f, -1.0f };

	Vector3 rotatedX = Transform(X, rotationMatrix);
	Vector3 rotatedY = Transform(Y, rotationMatrix);
	Vector3 rotatedZ = Transform(Z, rotationMatrix);



	//if (isDebugCamera) {

		/// ========カメラ操作========
		// カメラの回転を更新する
	if (Input::GetInstance()->IsPressMouse(0) == 1) {
		if (!isLeftClicked) {
			// マウスがクリックされたときに現在のマウス位置を保存する
			clickPosition = Input::GetInstance()->GetMousePosition();
			isLeftClicked = true;
		} else {
			// マウスがクリックされている間はカメラの回転を更新する
			Vector2 currentMousePos;
			currentMousePos = Input::GetInstance()->GetMousePosition();

			float deltaX = static_cast<float>(currentMousePos.x - clickPosition.x);
			float deltaY = static_cast<float>(currentMousePos.y - clickPosition.y);

			cameraRotate.x -= (deltaY * mouseSensitivity);// / 5.0f;
			cameraRotate.y += (deltaX * mouseSensitivity); /// 5.0f;


			// 現在のマウス位置を保存する
			clickPosition = currentMousePos;
		}
	} else {
		// マウスがクリックされていない場合はフラグをリセットする
		isLeftClicked = false;
	}

	// カメラの位置を更新する
	if (Input::GetInstance()->IsPressMouse(2) == 1) {
		if (!isWheelClicked) {
			// マウスがクリックされたときに現在のマウス位置を保存する
			clickPosition = Input::GetInstance()->GetMousePosition();
			isWheelClicked = true;
		} else {
			// マウスがクリックされている間はカメラの位置を更新する
			Vector2 currentMousePos;
			currentMousePos = Input::GetInstance()->GetMousePosition();

			float deltaX = static_cast<float>(currentMousePos.x - clickPosition.x);
			float deltaY = static_cast<float>(currentMousePos.y - clickPosition.y);

			cameraTranslate -= (rotatedX * deltaX * mouseSensitivity);
			cameraTranslate += (rotatedY * deltaY * mouseSensitivity);

			// 現在のマウス位置を保存する
			clickPosition = currentMousePos;
		}
	} else {
		// マウスがクリックされていない場合はフラグをリセットする
		isWheelClicked = false;
	}

	// マウスホイールの移動量を取得する
	int wheelDelta = -Input::GetInstance()->GetWheel();

	// マウスホイールの移動量に応じてカメラの移動を更新する
	cameraTranslate.z += rotatedZ.z * float(wheelDelta) * moveSpeed;
	/// =====================
//}
}
