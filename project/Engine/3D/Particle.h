#pragma once
#include "Mesh.h"
#include"Material.h"
#include"DirectionalLight.h"
#include"WorldTransform.h"
#include"ViewProjection.h"
#include"ObjectColor.h"
#include <cstdint>
#include<random>
#include<list>
#include"mathShapes.h"
class Particle
{
public:
	Particle() = default;
	~Particle();
	struct ParticleMesh {
		WorldTransform worldTransform;
		Vector3 Velocity;
		ObjectColor objectColor;
		float lifeTime;
		float currentTime;
	};
	struct Emitter {
		WorldTransform worldTransform;
		uint32_t count;//!<発生数
		float frequency;//!<発生頻度
		float frequencyTime;//!<頻度用タイマー
	};
	struct AccelerationField {
		Vector3 acceleration;//!<加速度
		AABB area;//!<範囲
	};
	struct ParticleGroup {
		WorldTransform worldTransform;
		Vector3 velocity;
		ObjectColor color;
		float lifeTime;
		float currentTime;
		Emitter emitter;
		AccelerationField accelerationField;
	};
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ViewProjection* viewProjection, const std::string& textureHandle);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	ParticleMesh MakeParticles(std::mt19937& randomEngine, const Vector3& translate);

	std::list<ParticleMesh> Emit(const Emitter& emitter, std::mt19937& randomEngine);
private:
	Material* material_= nullptr;
	Mesh* mesh_= nullptr;
	DirectionalLight* directionalLight_ = nullptr;
	WorldTransform worldTransform_;
	ObjectColor objectColor_;
	ViewProjection* viewProjection_;
	std::string textureHandle_;
	uint32_t kNumMaxInstance_ = 10;
	//ParticleMesh particles_[10];
	std::list<ParticleMesh> particles_;
	const float kDeltaTime = 1.0f / 60.0f;
	uint32_t numInstance_ = 0;
	bool useBillboard = false;
	uint32_t current_blend = 2;
	Emitter emitter{};
	AccelerationField accelerationField;
	bool useField = false;
};

