#include "ViewProjection.h"
#include"DirectXCommon.h"
#include"Mymath.h"
/// <summary>
/// 初期化
/// </summary>
void ViewProjection::Initialize() {
	CreateConstBufferResource();
	CBProjectionResource();
}
void ViewProjection::CreateConstBufferResource(uint32_t Instance) {
	// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	wvpResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(ConstBufferDataViewProjection) * Instance);
	// 書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	// 単位行列を書き込んでいく
	wvpData_->View = MakeIdentity4x4();
	wvpData_->Projection = MakeIdentity4x4();
	wvpData_->cameraPosition = Vector3(0.0f, 0.0f, 0.0f);
}
void ViewProjection::CBProjectionResource(uint32_t Instance)
{
	// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	projectionInverseResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(CBProjection) * Instance);
	// 書き込むためのアドレスを取得
	projectionInverseResource_->Map(0, nullptr, reinterpret_cast<void**>(&projectionInverse_));
	// 単位行列を書き込んでいく
	projectionInverse_->ProjectionInverse = MakeIdentity4x4();
}
/// <summary>
/// マッピングする
/// </summary>
void ViewProjection::Map() {
	
}
/// <summary>
/// 行列を転送する
/// </summary>
void ViewProjection::TransferMatrix() {
	wvpData_->View = viewMatrix_;
	wvpData_->Projection = projectionMatrix_;
	wvpData_->cameraPosition = translation_;
	projectionInverse_->ProjectionInverse = Inverse(projectionMatrix_);
}
void ViewProjection::UpdateMatrix() {

	Matrix4x4 scaleMatrix = MakeScaleMatrix(Vector3(1.0f, 1.0f, 1.0f));
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotation_.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotation_.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotation_.z);
	Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translation_);
	Matrix4x4 cameraMatrix = Multiply(scaleMatrix, Multiply(rotateXYZMatrix, translateMatrix));
	cameraMatrix_ = cameraMatrix;
	viewMatrix_ = Inverse(cameraMatrix);
	/*projectionMatrix_ = MakePerspectiveFovMatrix(0.45f,
		float(DirectXCommon::GetInstance()->GetBackBufferWidth()) / float(DirectXCommon::GetInstance()->GetBackBufferHeight()),
		0.1f, 100.0f);*/
	projectionMatrix_ = MakePerspectiveFovMatrix(fovAngleY, aspectRatio, nearZ, farZ);

	TransferMatrix();
}

