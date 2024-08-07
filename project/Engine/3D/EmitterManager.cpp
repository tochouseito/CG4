#include "EmitterManager.h"
#include"ParticleManager.h"
#include"imgui.h"
//std::random_device seedGenerator;
//std::mt19937 randomEngine(seedGenerator());
EmitterManager::EmitterManager()
{
}

EmitterManager::~EmitterManager()
{

}

void EmitterManager::Initialize(ViewProjection* viewProjection)
{
	/*パーティクルマネージャの生成*/
	particleManager_ = std::make_unique<ParticleManager>();
	particleManager_->Initialize(viewProjection);
}

void EmitterManager::Update()
{
#ifdef _DEBUG
	// ImGuiフレーム開始
	ImGui::Begin("Particles");
	for (auto& emitter : emitters) {
		if (ImGui::CollapsingHeader(emitter.first.c_str())) {
			if (ImGui::Button(("Generate" + emitter.first).c_str())) {
			//	emitter.second.splice(emitter.second.particles.end(), Emit(emitter.second.emitter, randomEngine));
			}
		}
	}
	ImGui::End();
#endif
	for (auto& emitter : emitters) {
		emitter.second.frequencyTime += kDeltaTime;// 時刻を進める
	}
}

void EmitterManager::Draw()
{
	particleManager_->Draw();
}

void EmitterManager::AddEmitter(std::string name)
{
	/*同じ名前のパーティクルがあれば追加しない*/
	if (emitters.contains(name)) {
		return;
	}
	/*パーティクルを追加してデータを書き込む*/
	Emitter& emitter = emitters[name];

	emitter.count = 3;
	emitter.frequency = 0.5f;
	emitter.frequencyTime = 0.0f;
	emitter.worldTransform.translation_ = { 0.0f,0.0f,0.0f };
	emitter.worldTransform.rotation_ = { 0.0f,0.0f,0.0f };
	emitter.worldTransform.scale_ = { 1.0f,1.0f,1.0f };
}

void EmitterManager::AddParticle(std::string name, std::string textureHandle)
{
	particleManager_->AddParticle(name, textureHandle);
}

void EmitterManager::Emit()
{
	//particleManager_->Emit
}
