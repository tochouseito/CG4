#pragma once
// base
#include "DirectXCommon.h"
#include"Input.h"
#include"Audio.h"

// 3D
#include"Model.h"
#include"MainCamera.h"
#include"ViewProjection.h"
#include"Particle.h"
#include"ParticleManager.h"
//#include"EmitterManager.h"
#include"DebugCamera.h"
#include"AnimatedCube.h"


// 2D
#include"Sprite.h"

/*3Dオブジェクト*/
#include"PlaneModel.h"
#include"axis.h"
#include"SphereModel.h"
#include"Primitive.h"
#include"BunnyModel.h"
#include"multiMaterialModel.h"
#include"multiMeshModel.h"
#include"suzanne.h"
#include"UtahTeapotModel.h"
#include"Skybox.h"

/*Scene*/
#include"BaseScene.h"
#include"SceneManager.h"

/*externals*/
#include"SerialToArduino.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene:public BaseScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize()override;

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override;

	/// <summary>
	/// シーンの切り替え依頼
	/// </summary>
	void ChangeScene()override {}

	void SetSceneManager(SceneManager* sceneManager) override { sceneManager_ = sceneManager; }

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Audio* audio_ = nullptr;
	Audio::SoundData SoundData1;
	SceneManager* sceneManager_ = nullptr;
	
	// テクスチャハンドル
	std::string textureHandle_[6]{};

	
	Particle* particles_ = nullptr;
	Sprite* sprite_ = nullptr;
	MainCamera* mainCamera_ = nullptr;
	ViewProjection  viewProjection_;

	/*3D平面モデル*/
	Model* planeModel_ = nullptr;
	PlaneModel* plane_ = nullptr;
	/*3D軸モデル*/
	Model* axisModel_ = nullptr;
	axis* axis_ = nullptr;
	/*3D球モデル*/
	Model* sphereModel_ = nullptr;
	SphereModel* sphere_ = nullptr;

	/*ジオメトリ*/
	Primitive* primitive_ = nullptr;

	/*シリアルライブラリ*/
	SerialToArduino* serial=nullptr;
	int heartRate = 0;
	int gsrValue = 0;
	/*パーティクルマネージャ*/
	std::unique_ptr<ParticleManager> particleManager_;
	/*エミッターマネージャ*/
	//std::unique_ptr<EmitterManager> emitterManager_;
	/*デバッグカメラ*/
	std::unique_ptr < DebugCamera> debugCamera_;
	bool useDebugCamera_ = false;

	Model* bunnyModel_ = nullptr;
	BunnyModel* bunny_ = nullptr;
	Model* multiMaterialModel_ = nullptr;
	multiMaterialModel* multiMaterial_ = nullptr;
	Model* multiMeshModel_ = nullptr;
	multiMeshModel* multiMesh_ = nullptr;
	Model* suzanneModel_ = nullptr;
	suzanne* suzanne_ = nullptr;
	Model* UtahTeapotModel_ = nullptr;
	UtahTeapotModel* utahTeapot_ = nullptr;
	Model* animatedCubeModel_ = nullptr;
	AnimatedCube* animatedCube_ = nullptr;
	Model::Animation* animation_ = nullptr;

	Model* skyboxModel_ = nullptr;
	Skybox* skybox_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};

