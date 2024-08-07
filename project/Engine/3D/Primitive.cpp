#include "Primitive.h"

/*base*/
#include"DirectXCommon.h"
#include"GraphicsPipelineState.h"
#include"TextureManager.h"

#ifdef _DEBUG
#include"imgui.h"
#endif // _DEBUG

Primitive::Primitive() {
	mesh_ = new Mesh();
	material_ = new Material();
}
Primitive::~Primitive() {
	delete mesh_;
	delete material_;
}

/// <summary>
/// 初期化
/// </summary>
void Primitive::Initialize(std::string textureHandle, ViewProjection* viewProjection) {
	viewProjection_ = viewProjection;
	textureHandle_ = textureHandle;

	worldTransform_.Initialize();
	GraphicsPipelineState::GetInstance()->CreateGraphicsPipelineGSO(DirectXCommon::GetInstance()->GetDevice());
	mesh_->CreateGSVertexResource(1);
	material_->CreateMaterialResource();
}

/// <summary>
/// 更新
/// </summary>
void Primitive::Update() {

#ifdef _DEBUG
	ImGui::Begin("GSO");

	ImGui::DragFloat3("translate", &worldTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("Rotate", &worldTransform_.rotation_.x, 0.01f);
	ImGui::DragFloat3("Scale", &worldTransform_.scale_.x, 0.01f);

	ImGui::End();
#endif // _DEBUG


	worldTransform_.UpdataMatrix();
}

/// <summary>
/// 描画
/// </summary>
void Primitive::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Roosignatureを設定。PSOに設定しているけど別途設定が必要
	commandList->SetGraphicsRootSignature(GraphicsPipelineState::GetRootSignature());
	commandList->SetPipelineState(GraphicsPipelineState::GetPipelineStateGSO(current_blend));// PSOを設定
	commandList->IASetVertexBuffers(0, 1, mesh_->GetVertexBufferView());// VBVを設定
	// マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, material_->GetMaterialResource()->GetGPUVirtualAddress());
	// wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(4, viewProjection_->GetWvpResource()->GetGPUVirtualAddress());
	// wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(textureHandle_));
	commandList->SetGraphicsRootDescriptorTable(1, worldTransform_.GetSrvHandleGPU());
	// 描画！(DrawCall/ドローコール)。３頂点で1つのインスタンス。インスタンスについては今後
	commandList->DrawInstanced(static_cast<UINT>(mesh_->GetVertices()), 1, 0, 0);
}