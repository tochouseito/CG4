#include "Sprite.h"
#include"DirectXCommon.h"
#include"TextureManager.h"
#include"Mymath.h"
#include"GraphicsPipelineState.h"
#include"imgui.h"


Sprite::Sprite() {
	mesh_ = new Mesh();
	material_ = new Material();
}
Sprite::~Sprite() {
	delete material_;
	delete mesh_;
}
/// <summary>
/// 初期化
/// </summary>
void Sprite::Initialize(const Vector3& position, ViewProjection* viewProjection, std::string textureHandle) {
	GraphicsPipelineState::GetInstance()->CreateGraphicsPipelineSprite(DirectXCommon::GetInstance()->GetDevice());
	// Transformのリソースを作る
	TransformResource();
	// 頂点リソースを作る
	//CreateVertexResource();
	mesh_->CreateSpriteVertexResource(6);
	// マテリアルリソースを作る
	//CreateMaterial();
	material_->CreateMaterialResource();
	// インデックスリソースを作る
	//CreateIndexResource();

	//worldTransform_.Initialize();
	//uvTransformSprite_.Initialize();

	translation_ = position;
	textureHandle_ = textureHandle;
	viewProjection_ = viewProjection;
	
}

/// <summary>
/// 毎フレーム処理
/// </summary>
void Sprite::Update() {

#ifdef _DEBUG

	ImGui::Begin("Sprite");

	ImGui::DragFloat3("translate##Sprite", &translation_.x, 1.0f);
	ImGui::DragFloat3("Rotate##Sprite", &rotation_.x, 0.01f);
	ImGui::DragFloat3("Scale##Sprite", &scale_.x, 0.01f);
	ImGui::DragFloat3("UVTransform##Sprite", &UVtranslation_.x, 0.01f);
	ImGui::DragFloat3("UVrotation##Sprite", &UVrotation_.x, 0.01f);
	ImGui::DragFloat3("UVscale##Sprite", &UVscale_.x, 0.01f);
		
	ImGui::End(); 

#endif
	UpdataTransform();
}

/// <summary>
/// 描画
/// </summary>
void Sprite::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
	// Spriteの描画。変更が必要なものだけ変更する
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Roosignatureを設定。PSOに設定しているけど別途設定が必要
	commandList->SetGraphicsRootSignature(GraphicsPipelineState::GetRootSignatureSprite());
	commandList->SetPipelineState(GraphicsPipelineState::GetPipelineStateSprite(0));// PSOを設定
	commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(textureHandle_));
	commandList->IASetVertexBuffers(0, 1, mesh_->GetVertexBufferView());// VBVを設定
	//commandList->IASetIndexBuffer(&indexBufferViewSprite_);// IBVを設定
	// マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, material_->GetMaterialResource()->GetGPUVirtualAddress());
	// TransformationMatrixCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite_->GetGPUVirtualAddress());
	// wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(3, viewProjection_->GetWvpResource()->GetGPUVirtualAddress());

	// 描画！(DrawCall/ドローコール)。３頂点で1つのインスタンス。インスタンスについては今後
	commandList->DrawInstanced(6, 1, 0, 0);
	//commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

/// <summary>
/// Sprite用のTransformationMatrix用のリソースを作る。
/// </summary>
void Sprite::TransformResource() {
	// Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	transformationMatrixResourceSprite_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(ConstBufferDataSpriteWorldTransform));
	// データを書き込む
	
	// 書き込むためのアドレスを取得
	transformationMatrixResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite_));
	// 単位行列を書き込んでいく
	transformationMatrixDataSprite_->matWorld = MakeIdentity4x4();
}
void Sprite::UpdataTransform() {
	// Sprite用のWorldViewProjectionMatrixを作る
	Matrix4x4 worldMatrixSprite = MakeAffineMatrix(scale_, rotation_, translation_);
	Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f,
		static_cast<float>(DirectXCommon::GetInstance()->GetBackBufferWidth()), static_cast<float>(DirectXCommon::GetInstance()->GetBackBufferHeight())
		, 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));
	transformationMatrixDataSprite_->matWorld = worldViewProjectionMatrixSprite;
	material_->GetMaterialData()->uvTransform = MakeAffineMatrix(UVscale_, UVrotation_, UVtranslation_);
}
void Sprite::CreateMaterial() {
	// スプライト用のマテリアルリソースを作る。
	materialResourceSprite_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Material::MaterialData));
	// マテリアルデータを書き込む
	// 書き込むためのアドレスを取得
	materialResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite_));
	// 今回は白を書き込んでいる
	materialDataSprite_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	// SpriteではLightingしないのでfalseを設定する
	materialDataSprite_->enableLighting = false;
	materialDataSprite_->uvTransform = MakeIdentity4x4();
}
void Sprite::CreateIndexResource() {
	// インデックスリソースを作成する
	indexResourceSprite_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(uint32_t) * 6);
	indexBufferViewSprite_.BufferLocation = indexResourceSprite_->GetGPUVirtualAddress();
	indexBufferViewSprite_.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_tとする
	indexBufferViewSprite_.Format = DXGI_FORMAT_R32_UINT;
	indexResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite_));
	indexDataSprite_[0] = 0; indexDataSprite_[1] = 1; indexDataSprite_[2] = 2;
	indexDataSprite_[3] = 1; indexDataSprite_[4] = 3; indexDataSprite_[5] = 2;
}
