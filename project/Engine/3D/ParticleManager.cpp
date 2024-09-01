#include "ParticleManager.h"
#include"GraphicsPipelineState.h"
#include"DirectXCommon.h"
#include"TextureManager.h"
#include"Mymath.h"
#include"imgui.h"
#include"SrvManager.h"

std::random_device seedGenerator;
std::mt19937 randomEngine(seedGenerator());
ParticleManager::ParticleManager()
{

}

ParticleManager::~ParticleManager()
{
	delete gpuParticleGroup;
}

void ParticleManager::Initialize(ViewProjection* viewProjection, std::string textureHandle)
{
	// PSOの生成
	GraphicsPipelineState::GetInstance()->CreateParticleGraphicsPipeline(DirectXCommon::GetInstance()->GetDevice());
	GraphicsPipelineState::GetInstance()->CreateComputePipelineParticle(DirectXCommon::GetInstance()->GetDevice());
	GraphicsPipelineState::GetInstance()->CreateGraphicsPipelineGPUParticle(DirectXCommon::GetInstance()->GetDevice());
	GraphicsPipelineState::GetInstance()->CreateComputePipelineEmit(DirectXCommon::GetInstance()->GetDevice());
	GraphicsPipelineState::GetInstance()->CreateComputePipelineUpdate(DirectXCommon::GetInstance()->GetDevice());
	viewProjection_ = viewProjection;
	gpuParticleGroup = new GPUParticleGroup();
	/*リソースの作成*/
	gpuParticleGroup->mesh = std::make_unique<Mesh>();
	gpuParticleGroup->mesh->CreateParticleVertexResource(size_t(6));
	CreateGPUParticleResource();
	CreateGPUEmitResource();
	CreateGPUFrameResource();
	CreateGPUCounterResource();
	CreateGPUFreeListResource();
	/*gpuParticleGroup->emitter.count = 3;
	gpuParticleGroup->emitter.frequency = 0.5f;
	gpuParticleGroup->emitter.frequencyTime = 0.0f;
	gpuParticleGroup->emitter.worldTransform.translation_ = { 0.0f,0.0f,0.0f };
	gpuParticleGroup->emitter.worldTransform.rotation_ = { 0.0f,0.0f,0.0f };
	gpuParticleGroup->emitter.worldTransform.scale_ = { 1.0f,1.0f,1.0f };*/
	gpuParticleGroup->emitterSphere->count = 10;
	gpuParticleGroup->emitterSphere->frequency = 0.5f;
	gpuParticleGroup->emitterSphere->frequencyTime = 0.0f;
	gpuParticleGroup->emitterSphere->translate = Vector3(0.0f, 0.0f, 0.0f);
	gpuParticleGroup->emitterSphere->radius = 1.0f;
	gpuParticleGroup->emitterSphere->emit=1;
	gpuParticleGroup->accelerationField.acceleration = { 15.0f,0.0f,0.0f };
	gpuParticleGroup->accelerationField.area.min = { -1.0f,-1.0f,-1.0f };
	gpuParticleGroup->accelerationField.area.max = { 1.0f,1.0f,1.0f };
	gpuParticleGroup->textureHandle = textureHandle;
	gpuParticleGroup->perFrame->deltaTime = 1.0f / 60.0f;
}

void ParticleManager::Update()
{
#ifdef _DEBUG
	// ImGuiフレーム開始
	ImGui::Begin("Particles");
	for (auto& particle : particleGroups) {
		if (ImGui::CollapsingHeader(particle.first.c_str())) {
			if (ImGui::Button(("Generate" + particle.first).c_str())) {
				particle.second.particles.splice(particle.second.particles.end(), Emit(particle.second.emitter, randomEngine));
			}
		}
	}
	ImGui::End();
#endif
	for (auto& particle : particleGroups) {
		particle.second.emitter.frequencyTime += kDeltaTime;// 時刻を進める
		uint32_t numInstance = 0;
		for (std::list<Particle>::iterator particleIterator = particle.second.particles.begin();
			particleIterator != particle.second.particles.end();) {
			if ((*particleIterator).lifeTime <= (*particleIterator).currentTime) {
				particleIterator = particle.second.particles.erase(particleIterator);
				continue;
			}
			if (particle.second.useField) {
				// Fieldの範囲内のParticleには加速度を適用する
				if (IsCollision((*particleIterator).worldTransform.translation_, particle.second.accelerationField.area)) {
					(*particleIterator).Velocity += particle.second.accelerationField.acceleration * kDeltaTime;
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

			if (particle.second.useBillboard) {
				Matrix4x4 scaleMatrix = MakeScaleMatrix((*particleIterator).worldTransform.scale_);
				Matrix4x4 translateMatrix = MakeTranslateMatrix((*particleIterator).worldTransform.translation_);
				(*particleIterator).worldTransform.matWorld_ = Multiply(billboardMatrix, Multiply(scaleMatrix, translateMatrix));
				// そのまま転送
				particle.second.worldTransform.matWorld_ = (*particleIterator).worldTransform.matWorld_;
				particle.second.worldTransform.numInstance_ = numInstance;
				particle.second.objectColor.numInstance_ = numInstance;
				particle.second.worldTransform.TransferMatrix();
			} else {
				particle.second.worldTransform.scale_ = (*particleIterator).worldTransform.scale_;
				particle.second.worldTransform.rotation_ = (*particleIterator).worldTransform.rotation_;
				particle.second.worldTransform.translation_ = (*particleIterator).worldTransform.translation_;
				particle.second.worldTransform.numInstance_ = numInstance;
				particle.second.objectColor.numInstance_ = numInstance;
				particle.second.worldTransform.UpdataMatrix();
			}
			particle.second.objectColor.color_ = (*particleIterator).objectColor.color_;
			particle.second.objectColor.color_.a = 1.0f - ((*particleIterator).currentTime / (*particleIterator).lifeTime);
			particle.second.objectColor.TransferMatrix();
			++numInstance;
			++particleIterator;
		}
		particle.second.numInstance = numInstance;
	}
	gpuParticleGroup->emitterSphere->frequencyTime += kDeltaTime;// タイム加算
	gpuParticleGroup->perFrame->time += kDeltaTime;
	// 射出間隔を上回ったら射出許可を出して時間を調整
	if (gpuParticleGroup->emitterSphere->frequency <= gpuParticleGroup->emitterSphere->frequencyTime) {
		gpuParticleGroup->emitterSphere->frequencyTime -= gpuParticleGroup->emitterSphere->frequency;
		gpuParticleGroup->emitterSphere->emit = 1;

		// 射出間隔を上回ってないので、許可は出せない
	} else {
		gpuParticleGroup->emitterSphere->emit = 0;
	}

}

void ParticleManager::Draw()
{
	for (auto& particle : particleGroups) {
		// コマンドリストの取得
		ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
		// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// Roosignatureを設定。PSOに設定しているけど別途設定が必要
		commandList->SetGraphicsRootSignature(GraphicsPipelineState::GetRootSignatureParticle());
		commandList->SetPipelineState(GraphicsPipelineState::GetParticlePipelineState(particle.second.current_blend));// PSOを設定
		commandList->IASetVertexBuffers(0, 1,particle.second.mesh->GetVertexBufferView());// VBVを設定
		// マテリアルCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(0, particle.second.material->GetMaterialResource()->GetGPUVirtualAddress());
		// wvp用のCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(4, viewProjection_->GetWvpResource()->GetGPUVirtualAddress());
		// ライト用のCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(3, particle.second.directionalLight->GetLightResource()->GetGPUVirtualAddress());
		commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(particle.second.textureHandle));
		// instancing用のDataを読むためにStructuredBufferのSRVを設定する
		commandList->SetGraphicsRootDescriptorTable(1, particle.second.worldTransform.GetSrvHandleGPU());
		commandList->SetGraphicsRootDescriptorTable(5, particle.second.objectColor.GetSrvHandleGPU());
		// 描画！(DrawCall/ドローコール)。３頂点で1つのインスタンス。インスタンスについては今後
		commandList->DrawInstanced(static_cast<UINT> (6), particle.second.numInstance, 0, 0);
	}
}

void ParticleManager::CreateGPUParticleResource()
{
	gpuParticleGroup->particleResource = DirectXCommon::GetInstance()->CreateUAVResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(GPUParticle) * kGPUMAX_);

	gpuParticleGroup->srvIndex = SrvManager::GetInstance()->Allocate();
	gpuParticleGroup->uavIndex= SrvManager::GetInstance()->Allocate();
	gpuParticleGroup->particleSrvHandle.first = SrvManager::GetInstance()->GetCPUDescriptorHandle(gpuParticleGroup->srvIndex);
	gpuParticleGroup->particleSrvHandle.second = SrvManager::GetInstance()->GetGPUDescriptorHandle(gpuParticleGroup->srvIndex);
	gpuParticleGroup->particleUavHandle.first = SrvManager::GetInstance()->GetCPUDescriptorHandle(gpuParticleGroup->uavIndex);
	gpuParticleGroup->particleUavHandle.second = SrvManager::GetInstance()->GetGPUDescriptorHandle(gpuParticleGroup->uavIndex);
	SrvManager::GetInstance()->CreateUAVforStructuredBuffer(gpuParticleGroup->uavIndex, gpuParticleGroup->particleResource.Get(), 1024, sizeof(GPUParticle));
	SrvManager::GetInstance()->CreateSRVforStructuredBuffer(gpuParticleGroup->srvIndex, gpuParticleGroup->particleResource.Get(), 1024, sizeof(GPUParticle));
}

void ParticleManager::CreateGPUEmitResource()
{
	// リソースを作る。Matrix4x4 1つ分のサイズを用意する
	gpuParticleGroup->emitResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(EmitterSphere));
	// 書き込むためのアドレスを取得
	gpuParticleGroup->emitResource->Map(0, nullptr, reinterpret_cast<void**>(&gpuParticleGroup->emitterSphere));
}

void ParticleManager::CreateGPUFrameResource()
{
	// リソースを作る。Matrix4x4 1つ分のサイズを用意する
	gpuParticleGroup->frameResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(PerFrame));
	// 書き込むためのアドレスを取得
	gpuParticleGroup->frameResource->Map(0, nullptr, reinterpret_cast<void**>(&gpuParticleGroup->perFrame));
}

void ParticleManager::CreateGPUCounterResource()
{
	gpuParticleGroup->counterResource = DirectXCommon::GetInstance()->CreateUAVResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(int32_t));

	gpuParticleGroup->counterUavIndex = SrvManager::GetInstance()->Allocate();
	gpuParticleGroup->counterUavHandle.first = SrvManager::GetInstance()->GetCPUDescriptorHandle(gpuParticleGroup->counterUavIndex);
	gpuParticleGroup->counterUavHandle.second = SrvManager::GetInstance()->GetGPUDescriptorHandle(gpuParticleGroup->counterUavIndex);
	SrvManager::GetInstance()->CreateUAVforStructuredBuffer(gpuParticleGroup->counterUavIndex, gpuParticleGroup->counterResource.Get(), 1, sizeof(int32_t));
}

void ParticleManager::CreateGPUFreeListResource()
{
	gpuParticleGroup->listIndexResource = DirectXCommon::GetInstance()->CreateUAVResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(int32_t));

	gpuParticleGroup->listIndexUavIndex = SrvManager::GetInstance()->Allocate();
	gpuParticleGroup->listIndexUavHandle.first = SrvManager::GetInstance()->GetCPUDescriptorHandle(gpuParticleGroup->listIndexUavIndex);
	gpuParticleGroup->listIndexUavHandle.second = SrvManager::GetInstance()->GetGPUDescriptorHandle(gpuParticleGroup->listIndexUavIndex);
	SrvManager::GetInstance()->CreateUAVforStructuredBuffer(gpuParticleGroup->listIndexUavIndex, gpuParticleGroup->listIndexResource.Get(), 1, sizeof(int32_t));

	gpuParticleGroup->listResource = DirectXCommon::GetInstance()->CreateUAVResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(uint32_t) * kGPUMAX_);

	gpuParticleGroup->listUavIndex = SrvManager::GetInstance()->Allocate();
	gpuParticleGroup->listUavHandle.first = SrvManager::GetInstance()->GetCPUDescriptorHandle(gpuParticleGroup->listUavIndex);
	gpuParticleGroup->listUavHandle.second = SrvManager::GetInstance()->GetGPUDescriptorHandle(gpuParticleGroup->listUavIndex);
	SrvManager::GetInstance()->CreateUAVforStructuredBuffer(gpuParticleGroup->listUavIndex, gpuParticleGroup->listResource.Get(), kGPUMAX_, sizeof(uint32_t));
}

void ParticleManager::DrawGPU()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

	if (init) {
		commandList->SetComputeRootSignature(GraphicsPipelineState::GetInstance()->GetRootSignatureCSParticle());
		commandList->SetPipelineState(GraphicsPipelineState::GetInstance()->GetPipelineStateCSParticle());
		commandList->SetComputeRootDescriptorTable(0, gpuParticleGroup->particleUavHandle.second);
		//commandList->SetComputeRootDescriptorTable(1, gpuParticleGroup->counterUavHandle.second);
		commandList->SetComputeRootDescriptorTable(1, gpuParticleGroup->listIndexUavHandle.second);
		commandList->SetComputeRootDescriptorTable(2, gpuParticleGroup->listUavHandle.second);
		commandList->Dispatch(1, 1, 1);
		init = false;
	}
	/*D3D12_RESOURCE_BARRIER barrier5{};
	barrier5.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier5.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier5.UAV.pResource = gpuParticleGroup->particleResource.Get();
	commandList->ResourceBarrier(1, &barrier5);

	D3D12_RESOURCE_BARRIER barrier6{};
	barrier6.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier6.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier6.UAV.pResource = gpuParticleGroup->counterResource.Get();
	commandList->ResourceBarrier(1, &barrier6);

	D3D12_RESOURCE_BARRIER barrier7{};
	barrier7.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier7.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier7.UAV.pResource = gpuParticleGroup->listIndexResource.Get();
	commandList->ResourceBarrier(1, &barrier7);

	D3D12_RESOURCE_BARRIER barrier8{};
	barrier8.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier8.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier8.UAV.pResource = gpuParticleGroup->listResource.Get();
	commandList->ResourceBarrier(1, &barrier8);*/
	/*Emit*/
	commandList->SetComputeRootSignature(GraphicsPipelineState::GetInstance()->GetRootSignatureCSEmit());
	commandList->SetPipelineState(GraphicsPipelineState::GetInstance()->GetPipelineStateCSEmit());
	commandList->SetComputeRootDescriptorTable(0, gpuParticleGroup->particleUavHandle.second);
	commandList->SetComputeRootConstantBufferView(1, gpuParticleGroup->emitResource.Get()->GetGPUVirtualAddress());
	commandList->SetComputeRootConstantBufferView(2, gpuParticleGroup->frameResource.Get()->GetGPUVirtualAddress());
	//commandList->SetComputeRootDescriptorTable(3, gpuParticleGroup->counterUavHandle.second);
	commandList->SetComputeRootDescriptorTable(3, gpuParticleGroup->listIndexUavHandle.second);
	commandList->SetComputeRootDescriptorTable(4, gpuParticleGroup->listUavHandle.second);
	commandList->Dispatch(1, 1, 1);

	/*EmitとUpdateの並列を阻止*/
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.UAV.pResource = gpuParticleGroup->particleResource.Get();
	commandList->ResourceBarrier(1, &barrier);

	D3D12_RESOURCE_BARRIER barrier2{};
	barrier2.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier2.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier2.UAV.pResource = gpuParticleGroup->counterResource.Get();
	commandList->ResourceBarrier(1, &barrier2);

	D3D12_RESOURCE_BARRIER barrier3{};
	barrier3.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier3.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier3.UAV.pResource = gpuParticleGroup->listIndexResource.Get();
	commandList->ResourceBarrier(1, &barrier3);

	D3D12_RESOURCE_BARRIER barrier4{};
	barrier4.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier4.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier4.UAV.pResource = gpuParticleGroup->listResource.Get();
	commandList->ResourceBarrier(1, &barrier4);

	/*Update*/
	commandList->SetComputeRootSignature(GraphicsPipelineState::GetInstance()->GetRootSignatureCSUpdate());
	commandList->SetPipelineState(GraphicsPipelineState::GetInstance()->GetPipelineStateCSUpdate());
	commandList->SetComputeRootDescriptorTable(0, gpuParticleGroup->particleUavHandle.second);
	commandList->SetComputeRootConstantBufferView(1, gpuParticleGroup->frameResource.Get()->GetGPUVirtualAddress());
	commandList->SetComputeRootDescriptorTable(2, gpuParticleGroup->listIndexUavHandle.second);
	commandList->SetComputeRootDescriptorTable(3, gpuParticleGroup->listUavHandle.second);
	commandList->Dispatch(1, 1, 1);

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Roosignatureを設定。PSOに設定しているけど別途設定が必要
	commandList->SetGraphicsRootSignature(GraphicsPipelineState::GetRootSignatureGPUParticle());
	commandList->SetPipelineState(GraphicsPipelineState::GetPipelineStateGPUParticle(2));// PSOを設定
	commandList->IASetVertexBuffers(0, 1, gpuParticleGroup->mesh->GetVertexBufferView());// VBVを設定
	DirectXCommon::GetInstance()->BarrierTransition(gpuParticleGroup->particleResource.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,   // 現在の状態
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE // 目標の状態
	);
	commandList->SetGraphicsRootDescriptorTable(0, gpuParticleGroup->particleSrvHandle.second);
	commandList->SetGraphicsRootConstantBufferView(1, viewProjection_->GetWvpResource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(gpuParticleGroup->textureHandle));

	commandList->DrawInstanced(static_cast<UINT> (6), 1024, 0, 0);
	DirectXCommon::GetInstance()->BarrierTransition(gpuParticleGroup->particleResource.Get(),
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,   // 現在の状態
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS // 目標の状態
	);
}

void ParticleManager::AddParticle(std::string name, std::string textureHandle)
{
	/*同じ名前のパーティクルがあれば追加しない*/
	if (particleGroups.contains(name)) {
		return;
	}
	/*パーティクルを追加してデータを書き込む*/
	ParticleGroup& particleGroup= particleGroups[name];
	/*リソースの作成*/
	particleGroup.material = std::make_unique<Material>();
	particleGroup.mesh = std::make_unique<Mesh>();
	particleGroup.directionalLight = std::make_unique<DirectionalLight>();
	particleGroup.mesh->CreateParticleVertexResource(size_t(6));
	particleGroup.material->CreateMaterialResource();
	particleGroup.directionalLight->CreateDirectionalLightResource();
	particleGroup.emitter.count = 3;
	particleGroup.emitter.frequency = 0.5f;
	particleGroup.emitter.frequencyTime = 0.0f;
	particleGroup.emitter.worldTransform.translation_ = { 0.0f,0.0f,0.0f };
	particleGroup.emitter.worldTransform.rotation_ = { 0.0f,0.0f,0.0f };
	particleGroup.emitter.worldTransform.scale_ = { 1.0f,1.0f,1.0f };
	particleGroup.accelerationField.acceleration = { 15.0f,0.0f,0.0f };
	particleGroup.accelerationField.area.min = { -1.0f,-1.0f,-1.0f };
	particleGroup.accelerationField.area.max = { 1.0f,1.0f,1.0f };
	particleGroup.worldTransform.numInstance_ = 100;
	particleGroup.worldTransform.Initialize();
	particleGroup.objectColor.numInstance_ = 100;
	particleGroup.objectColor.Initialize();
	particleGroup.textureHandle = textureHandle;
	particleGroup.worldTransform.rootMatrix_ = MakeIdentity4x4();
}

ParticleManager::Particle ParticleManager::MakeParticles(std::mt19937& randomEngine, const Vector3& translate)
{
	Particle particles;
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

void ParticleManager::AddGPUParticle(std::string name, std::string textureHandle)
{
	/*同じ名前のパーティクルがあれば追加しない*/
	if (gpuParticleGroups.contains(name)) {
		return;
	}
	/*パーティクルを追加してデータを書き込む*/
	GPUParticleGroup& gpuParticleGroup = gpuParticleGroups[name];
	/*リソースの作成*/
	gpuParticleGroup.mesh = std::make_unique<Mesh>();
	gpuParticleGroup.mesh->CreateParticleVertexResource(size_t(6));
	/*gpuParticleGroup.emitter.count = 3;
	gpuParticleGroup.emitter.frequency = 0.5f;
	gpuParticleGroup.emitter.frequencyTime = 0.0f;
	gpuParticleGroup.emitter.worldTransform.translation_ = { 0.0f,0.0f,0.0f };
	gpuParticleGroup.emitter.worldTransform.rotation_ = { 0.0f,0.0f,0.0f };
	gpuParticleGroup.emitter.worldTransform.scale_ = { 1.0f,1.0f,1.0f };*/
	gpuParticleGroup.accelerationField.acceleration = { 15.0f,0.0f,0.0f };
	gpuParticleGroup.accelerationField.area.min = { -1.0f,-1.0f,-1.0f };
	gpuParticleGroup.accelerationField.area.max = { 1.0f,1.0f,1.0f };
	/*gpuParticleGroup.worldTransform.numInstance_ = 100;
	gpuParticleGroup.worldTransform.Initialize();
	gpuParticleGroup.objectColor.numInstance_ = 100;
	gpuParticleGroup.objectColor.Initialize();*/
	gpuParticleGroup.textureHandle = textureHandle;
	//gpuParticleGroup.worldTransform.rootMatrix_ = MakeIdentity4x4();
}

ParticleManager::GPUParticle ParticleManager::MakeGPUParticles(std::mt19937& randomEngine, const Vector3& translate)
{
	GPUParticle particles;
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);
	std::uniform_real_distribution<float> distTime(1.0f, 3.0f);

	particles.scale = { 1.0f,1.0f,1.0f };
	particles.rotation = { 0.0f,0.0f,0.0f };
	Vector3 randomTranslate = { distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) };
	particles.translation = translate + randomTranslate;
	particles.velocity = { distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) };
	particles.color = { distColor(randomEngine),distColor(randomEngine),distColor(randomEngine) ,1.0f };
	particles.lifeTime = distTime(randomEngine);
	particles.currentTime = 0.0f;
	return particles;
}



std::list<ParticleManager::Particle> ParticleManager::Emit(const Emitter& emitter, std::mt19937& randomEngine)
{
	std::list<Particle> particles;
	for (uint32_t count = 0; count < emitter.count; ++count) {
		particles.push_back(MakeParticles(randomEngine, emitter.worldTransform.translation_));
	}
	return particles;
}
