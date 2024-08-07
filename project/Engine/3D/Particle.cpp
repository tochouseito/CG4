#include "Particle.h"

/*base*/
#include"GraphicsPipelineState.h"
#include"DirectXCommon.h"
#include"TextureManager.h"

#include"imgui.h"
#include"Mymath.h"
#include<numbers>

Particle::~Particle() {
	delete mesh_;
	delete material_;
	delete directionalLight_;
}

/// <summary>
/// 初期化
/// </summary>
void Particle::Initialize(ViewProjection* viewProjection, const std::string& textureHandle) {
	// 各インスタンスの生成
	mesh_ = new Mesh();
	material_ = new Material();
	directionalLight_ = new DirectionalLight();
	// PSOの生成
	GraphicsPipelineState::GetInstance()->CreateParticleGraphicsPipeline(DirectXCommon::GetInstance()->GetDevice());

	mesh_->CreateParticleVertexResource(size_t(6));
	material_->CreateMaterialResource();
	directionalLight_->CreateDirectionalLightResource();
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());
	emitter.count = 3;
	emitter.frequency = 0.5f;
	emitter.frequencyTime = 0.0f;
	emitter.worldTransform.translation_ = { 0.0f,0.0f,0.0f };
	emitter.worldTransform.rotation_ = { 0.0f,0.0f,0.0f };
	emitter.worldTransform.scale_ = { 1.0f,1.0f,1.0f };
	accelerationField.acceleration = { 15.0f,0.0f,0.0f };
	accelerationField.area.min = { -1.0f,-1.0f,-1.0f };
	accelerationField.area.max = { 1.0f,1.0f,1.0f };
	worldTransform_.numInstance_ = 100;
	worldTransform_.Initialize();
	objectColor_.numInstance_ = 100;
	objectColor_.Initialize();
	textureHandle_ = textureHandle;
	viewProjection_ = viewProjection;
	worldTransform_.rootMatrix_ = MakeIdentity4x4();
}

/// <summary>
/// 毎フレーム処理
/// </summary>
void Particle::Update() {
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());
#ifdef _DEBUG
	// ImGuiフレーム開始
	ImGui::Begin("3Dobjects");
	if (ImGui::CollapsingHeader("Particles")) {
		if (ImGui::Button("Add Particle")) {
			particles_.splice(particles_.end(), Emit(emitter, randomEngine));
		}
		ImGui::DragFloat3("EmitterTranslate", &emitter.worldTransform.translation_.x, 0.01f, -100.0f, 100.0f);
		ImGui::Checkbox("usebillboard", &useBillboard);
		ImGui::Checkbox("useField", &useField);
		// コンボボックスを表示する
		if (ImGui::BeginCombo("BlendMode", material_->blendMode[current_blend].c_str())) {
			// アイテムリストをループ
			for (uint32_t n = 0; n < material_->blendMode.size(); n++) {
				bool is_selected = (current_blend == n);
				if (ImGui::Selectable(material_->blendMode[n].c_str(), is_selected)) {
					current_blend = n; // アイテムが選択された場合、インデックスを更新
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus(); // フォーカスを設定
				}
			}
			ImGui::EndCombo();
		}
	}
	ImGui::End();
#endif

	emitter.frequencyTime += kDeltaTime;// 時刻を進める
	
		//if (emitter.frequency <= emitter.frequencyTime) {// 頻度より大きいなら発生
		//	if (particles_.size() < 100) {
		//		particles_.splice(particles_.end(), Emit(emitter, randomEngine));// 発生処理
		//	}
		//	emitter.frequencyTime -= emitter.frequency;// 余計に過ぎた時間も加味して頻度計算する
		//}
	
	
	
	uint32_t numInstance = 0;
	for (std::list<ParticleMesh>::iterator particleIterator = particles_.begin();
		particleIterator != particles_.end();) {
		if ((*particleIterator).lifeTime <= (*particleIterator).currentTime) {
			particleIterator = particles_.erase(particleIterator);
			continue;
		}
		if (useField) {
			// Fieldの範囲内のParticleには加速度を適用する
			if (IsCollision((*particleIterator).worldTransform.translation_, accelerationField.area)) {
				(*particleIterator).Velocity += accelerationField.acceleration * kDeltaTime;
			}
		}
		Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
		Matrix4x4 billboardMatrix = Multiply(backToFrontMatrix, viewProjection_->cameraMatrix_);
		billboardMatrix.m[3][0] = 0.0f;
		billboardMatrix.m[3][1] = 0.0f;
		billboardMatrix.m[3][2] = 0.0f;

		std::random_device seedGenerator;
		std::mt19937 randomEngine(seedGenerator());
		(*particleIterator).worldTransform.translation_ += (*particleIterator).Velocity * kDeltaTime;
		(*particleIterator).currentTime += kDeltaTime;

		if (useBillboard) {
			Matrix4x4 scaleMatrix = MakeScaleMatrix((*particleIterator).worldTransform.scale_);
			Matrix4x4 translateMatrix = MakeTranslateMatrix((*particleIterator).worldTransform.translation_);
			(*particleIterator).worldTransform.matWorld_ = Multiply(billboardMatrix, Multiply(scaleMatrix, translateMatrix));
			// そのまま転送
			worldTransform_.matWorld_ = (*particleIterator).worldTransform.matWorld_;
			worldTransform_.numInstance_ = numInstance;
			objectColor_.numInstance_ = numInstance;
			worldTransform_.TransferMatrix();
		} else {
			worldTransform_.scale_ = (*particleIterator).worldTransform.scale_;
			worldTransform_.rotation_ = (*particleIterator).worldTransform.rotation_;
			worldTransform_.translation_ = (*particleIterator).worldTransform.translation_;
			worldTransform_.numInstance_ = numInstance;
			objectColor_.numInstance_ = numInstance;
			worldTransform_.UpdataMatrix();
		}
		objectColor_.color_ = (*particleIterator).objectColor.color_;
		objectColor_.color_.a = 1.0f - ((*particleIterator).currentTime / (*particleIterator).lifeTime);
		objectColor_.TransferMatrix();
		++numInstance;
		++particleIterator;
	}
	numInstance_ = numInstance;
}

/// <summary>
/// 描画
/// </summary>
void Particle::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Roosignatureを設定。PSOに設定しているけど別途設定が必要
	commandList->SetGraphicsRootSignature(GraphicsPipelineState::GetRootSignatureParticle());
	commandList->SetPipelineState(GraphicsPipelineState::GetParticlePipelineState(current_blend));// PSOを設定
	commandList->IASetVertexBuffers(0, 1, mesh_->GetVertexBufferView());// VBVを設定
	// マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, material_->GetMaterialResource()->GetGPUVirtualAddress());
	// wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(4, viewProjection_->GetWvpResource()->GetGPUVirtualAddress());
	// ライト用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(3, directionalLight_->GetLightResource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(textureHandle_));
	// instancing用のDataを読むためにStructuredBufferのSRVを設定する
	commandList->SetGraphicsRootDescriptorTable(1, worldTransform_.GetSrvHandleGPU());
	commandList->SetGraphicsRootDescriptorTable(5,objectColor_.GetSrvHandleGPU());
	// 描画！(DrawCall/ドローコール)。３頂点で1つのインスタンス。インスタンスについては今後
	commandList->DrawInstanced(static_cast<UINT> (6), numInstance_, 0, 0);
}
Particle::ParticleMesh Particle::MakeParticles(std::mt19937& randomEngine,const Vector3&translate) {
	ParticleMesh particles;
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);
	std::uniform_real_distribution<float> distTime(1.0f, 3.0f);
	
	particles.worldTransform.scale_ = { 1.0f,1.0f,1.0f };
	particles.worldTransform.rotation_ = { 0.0f,0.0f,0.0f };
	Vector3 randomTranslate = { distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) };
	particles.worldTransform.translation_ = translate + randomTranslate;
	particles.Velocity = { distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) };
	particles.objectColor.color_ = { distColor(randomEngine),distColor(randomEngine),distColor(randomEngine) ,1.0f };
	particles.lifeTime = distTime(randomEngine);
	particles.currentTime = 0.0f;
	return particles;
}
std::list<Particle::ParticleMesh> Particle::Emit(const Emitter& emitter, std::mt19937& randomEngine) {
	std::list<ParticleMesh> particles;
	for (uint32_t count = 0; count < emitter.count; ++count) {
		particles.push_back(MakeParticles(randomEngine,emitter.worldTransform.translation_));
	}
	return particles;
}