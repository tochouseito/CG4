#pragma once
#include"WorldTransform.h"
#include"ViewProjection.h"
#include<unordered_map>
class ParticleManager;
class EmitterManager
{
public:
	EmitterManager();
	~EmitterManager();
	struct Emitter {
		WorldTransform worldTransform;
		uint32_t count;		 //!<発生数
		float frequency;	 //!<発生頻度
		float frequencyTime; //!<頻度用タイマー
	};
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ViewProjection* viewProjection);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// エミッター追加
	/// </summary>
	/// <param name="name"></param>
	void AddEmitter(std::string name);

	/// <summary>
	/// パーティクル追加
	/// </summary>
	/// <param name="name"></param>
	void AddParticle(std::string name, std::string textureHandle);
	/// <summary>
	/// 発生
	/// </summary>
	void Emit();
private:
	std::unordered_map<std::string,Emitter> emitters;
	/*パーティクルマネージャ*/
	std::unique_ptr<ParticleManager> particleManager_;
	/*デルタタイム*/
	const float kDeltaTime = 1.0f / 60.0f;
};

