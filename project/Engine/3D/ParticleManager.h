#pragma once
#include"WorldTransform.h"
#include"mathShapes.h"
#include"ObjectColor.h"
#include"Material.h"
#include"Mesh.h"
#include"DirectionalLight.h"
#include"ViewProjection.h"
#include<unordered_map>
#include<random>
#include<list>
class ParticleManager
{
public:/*パーティクルの要素構造体*/
	struct Particle {
		WorldTransform worldTransform;
		Vector3 Velocity;
		ObjectColor objectColor;
		float lifeTime;
		float currentTime;
	};
	struct Emitter {
		WorldTransform worldTransform;
		uint32_t count;		 //!<発生数
		float frequency;	 //!<発生頻度
		float frequencyTime; //!<頻度用タイマー
	};
	struct AccelerationField {
		Vector3 acceleration;//!<加速度
		AABB area;			 //!<範囲
	};
	struct ParticleGroup {
		std::list<Particle> particles;
		Emitter emitter;
		AccelerationField accelerationField;
		WorldTransform worldTransform;
		ObjectColor objectColor;
		std::string textureHandle;
		uint32_t current_blend = 2;
		uint32_t numInstance = 0;
		bool useBillboard = false;
		bool useField = false;
		/*グラフィックパイプラインに合わせるため*/
		std::unique_ptr<Material> material;
		std::unique_ptr<Mesh> mesh;
		std::unique_ptr<DirectionalLight> directionalLight;
	};
	struct GPUParticle {
		Vector3 translation;
		Vector3 rotation;
		Vector3 scale;
		float lifeTime;
		Vector3 velocity;
		float currentTime;
		Color color;
	};
	struct EmitterSphere {
		Vector3 translate;// 位置
		float radius;// 射出半径
		uint32_t count;// 射出数
		float frequency;// 射出間隔
		float frequencyTime;// 射出間隔調整用時間
		uint32_t emit;// 射出許可
	};
	struct PerFrame
	{
		float time;
		float deltaTime;
	};
	struct GPUParticleGroup {
		std::list<GPUParticle> particles;
		//Emitter emitter;
		EmitterSphere* emitterSphere=nullptr;
		AccelerationField accelerationField;
		std::string textureHandle;
		std::unique_ptr<Mesh> mesh;
		Microsoft::WRL::ComPtr<ID3D12Resource> particleResource;
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE>particleSrvHandle;
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE>particleUavHandle;
		uint32_t srvIndex;
		uint32_t uavIndex;
		Microsoft::WRL::ComPtr<ID3D12Resource> emitResource;
		Microsoft::WRL::ComPtr<ID3D12Resource> frameResource;
		PerFrame* perFrame = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> counterResource;
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE>counterUavHandle;
		uint32_t counterUavIndex;
		Microsoft::WRL::ComPtr<ID3D12Resource> listResource;
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE>listUavHandle;
		uint32_t listUavIndex;
		Microsoft::WRL::ComPtr<ID3D12Resource> listIndexResource;
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE>listIndexUavHandle;
		uint32_t listIndexUavIndex;
	};
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ParticleManager();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~ParticleManager();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ViewProjection* viewProjection, std::string textureHandle);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void DrawGPU();

	void CreateGPUParticleResource();

	void CreateGPUEmitResource();

	void CreateGPUFrameResource();

	void CreateGPUCounterResource();

	void CreateGPUFreeListResource();

	/// <summary>
	/// パーティクル追加
	/// </summary>
	/// <param name="name"></param>
	void AddParticle(std::string name,std::string textureHandle);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="randomEngine"></param>
	/// <param name="translate"></param>
	/// <returns></returns>
	Particle MakeParticles(std::mt19937& randomEngine, const Vector3& translate);

	/// <summary>
	/// パーティクル追加
	/// </summary>
	/// <param name="name"></param>
	void AddGPUParticle(std::string name, std::string textureHandle);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="randomEngine"></param>
	/// <param name="translate"></param>
	/// <returns></returns>
	GPUParticle MakeGPUParticles(std::mt19937& randomEngine, const Vector3& translate);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="emitter"></param>
	/// <param name="randomEngine"></param>
	/// <returns></returns>
	std::list<Particle> Emit(const Emitter& emitter, std::mt19937& randomEngine);

private:
	/*ビュープロジェクション*/
	ViewProjection* viewProjection_;
	/*デルタタイム*/
	const float kDeltaTime = 1.0f / 60.0f;
	/*最大インスタンス数*/
	uint32_t kNumMaxInstance_ = 10;
	/*パーティクルコンテナ*/
	std::unordered_map<std::string, ParticleGroup>particleGroups;

	/*GPU用*/
	/*最大インスタンス数*/
	uint32_t kGPUMAX_ = 1024;
	/*パーティクルコンテナ*/
	std::unordered_map<std::string, GPUParticleGroup>gpuParticleGroups;
	GPUParticleGroup* gpuParticleGroup = nullptr;

	bool init = true;
};

