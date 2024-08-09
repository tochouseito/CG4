#include "GameScene.h"
#include "TextureManager.h"
#include <DirectXTex.h>
#include <cassert>
#include<fstream>
#ifdef _DEBUG
#include"imgui.h"
#endif


#include"Mymath.h"

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete mainCamera_;
	delete sprite_;
	delete particles_;
	delete plane_;
	delete planeModel_;
	delete axis_;
	delete axisModel_;
	delete sphereModel_;
	delete sphere_;
	delete primitive_;

	delete serial;
	delete bunnyModel_ ;
	delete bunny_ ;
	delete multiMaterialModel_;
	delete multiMaterial_ ;
	delete multiMeshModel_ ;
	delete multiMesh_;
	delete suzanneModel_;
	delete suzanne_ ;
	delete UtahTeapotModel_ ;
	delete utahTeapot_ ;
	delete animatedCubeModel_;
	delete animatedCube_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	audio_ = Audio::GetInstance();

	SoundData1 = audio_->SoundLordWave("./Resources/mokugyo.wav");
	// ファイル名でテクスチャを読み込む
	textureHandle_[0] = TextureManager::Load("./Resources/uvChecker.png");
	textureHandle_[1] = TextureManager::Load("./Resources/circle.png");
	textureHandle_[2] = TextureManager::Load("./Resources/monsterBall.png");
	textureHandle_[3] = TextureManager::Load("./Resources/noise.png");
	textureHandle_[4] = TextureManager::Load("./Resources/checkerBoard.png");
	
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
	DirectXCommon::GetInstance()->SetViewProjection(&viewProjection_);
	DirectXCommon::GetInstance()->SetMaskTexture(textureHandle_[3]);
	/*3D平面モデル*/
	planeModel_ = Model::CreatePlane();
	plane_ = new PlaneModel();
	plane_->Initialize(planeModel_, textureHandle_[0], &viewProjection_);

	/*3D軸モデル*/
	axisModel_ = Model::LordModel("walk.gltf");
	axis_ = new axis();
	axis_->Initialize(axisModel_, textureHandle_[0], &viewProjection_);

	/*3D球モデル*/
	sphereModel_ = Model::CreateSphere(32);
	sphere_ = new SphereModel();
	sphere_->Initialize(sphereModel_, textureHandle_[2], &viewProjection_);

	/*パーティクル*/
	particles_ = new Particle();
	particles_->Initialize(&viewProjection_, textureHandle_[1]);
	
	// メインカメラの生成
	mainCamera_ = new MainCamera();
	mainCamera_->Initialize(Vector3(0.0f, 0.0f, 30.0f), &viewProjection_);
	// スプライトの生成
	sprite_ = new Sprite();
	sprite_->Initialize(Vector3(-170.0f, -100.0f, 0.0f), &viewProjection_,textureHandle_[0]);

	primitive_ = new Primitive();
	primitive_->Initialize(textureHandle_[0],&viewProjection_);

	serial = new SerialToArduino();
	
	if (!serial->InitializeSerialPort("COM3")) {  // L"\\\\.\\COM3" はワイド文字列リテラルです
		Vector3();
	}
	///*パーティクルマネージャの生成*/
	particleManager_ = std::make_unique<ParticleManager>();
	particleManager_->Initialize(&viewProjection_);
	particleManager_->AddParticle("circle", textureHandle_[1]);
	particleManager_->AddParticle("uvChecker", textureHandle_[0]);
	/*エミッターマネージャの生成*/
	/*emitterManager_ = std::make_unique<EmitterManager>();
	emitterManager_->Initialize(&viewProjection_);
	emitterManager_->AddEmitter("uvChecker");
	emitterManager_->AddEmitter("circle");
	emitterManager_->AddParticle("uvChecker", textureHandle_[0]);
	emitterManager_->AddParticle("circle", textureHandle_[1]);*/
	/*デバッグカメラの生成*/
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(Vector3(0.0f, 0.0f, -30.0f) ,&viewProjection_);

	/*bunnyModel_ = Model::LordModel("bunny.obj");
	bunny_ = new BunnyModel();
	bunny_->Initialize(bunnyModel_, textureHandle_[0], &viewProjection_);

	multiMaterialModel_ = Model::LordModel("multiMaterial.obj");
	multiMaterial_ = new multiMaterialModel();
	multiMaterial_->Initialize(multiMaterialModel_, textureHandle_[0], &viewProjection_);

	multiMeshModel_ = Model::LordModel("multiMesh.obj");
	multiMesh_ = new multiMeshModel();
	multiMesh_->Initialize(multiMeshModel_, textureHandle_[0], &viewProjection_);

	suzanneModel_ = Model::LordModel("suzanne.obj");
	suzanne_ = new suzanne();
	suzanne_->Initialize(suzanneModel_, textureHandle_[0], &viewProjection_);

	UtahTeapotModel_ = Model::LordModel("teapot.obj");
	utahTeapot_ = new UtahTeapotModel();
	utahTeapot_->Initialize(UtahTeapotModel_, textureHandle_[0], &viewProjection_);*/

	animatedCubeModel_ = Model::LordModel("AnimatedCube.gltf");
	animation_ = Model::LordAnimationFile("./Resources", "AnimatedCube.gltf");
	animatedCube_ = new AnimatedCube();
	animatedCube_->Initialize(animatedCubeModel_, textureHandle_[0], &viewProjection_, animation_);
}

void GameScene::Finalize()
{
	audio_->Finalize();
	audio_->SoundUnlord(&SoundData1);
}

void GameScene::Update() {
#ifdef _DEBUG
	ImGui::Begin("Scene");
	if (ImGui::Button("ChangeScene")) {
		/*シーン切り替え依頼*/
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}
	ImGui::End();
#endif // _DEBUG
#ifdef _DEBUG
	
	ImGui::Begin("Sound");
	/*音声再生*/
	if (ImGui::Button("Start")) {
		audio_->SoundPlayWave(audio_->GetXAudio2(), SoundData1);
	}
	ImGui::End();
#endif // _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_0)) {
		OutputDebugStringA("OK");
	}
#ifdef _DEBUG
	ImGui::Begin("serialValue");
	if (ImGui::Button("Get")) {
#endif // _DEBUG
	 std::string data = serial->ReadSerialPort();
		if (!data.empty()) {
			std::cout << "Received: " << data << std::endl;

			// データの解析
			size_t posHeartRate = data.find("Heart Rate: ");
			size_t posGSR = data.find("GSR: ");
			if (posHeartRate != std::string::npos && posGSR != std::string::npos) {
				heartRate = std::stoi(data.substr(posHeartRate + 12, posGSR - (posHeartRate + 12) - 2));
				gsrValue = std::stoi(data.substr(posGSR + 5));
			}
		}
#ifdef _DEBUG
	}
	ImGui::Text("gsrValue:%d", gsrValue);
	ImGui::Text("hrValue:%d", heartRate);
	ImGui::End();
#endif // _DEBUG
	plane_->Update();
	axis_->Update();
	sphere_->Update();
	//primitive_->Update();
	sprite_->Update();
	//particles_->Update();
	/*パーティクルマネージャの更新*/
	//particleManager_->Update();
	//emitterManager_->Update();
	/*bunny_->Updata();
	multiMaterial_->Updata();
	multiMesh_->Updata();
	suzanne_->Updata();
	utahTeapot_->Updata();*/
	animatedCube_->Updata();
	ImGui::Begin("HSV");
	ImGui::DragFloat("hue", &DirectXCommon::GetInstance()->GetHSVData()->hue, 0.01f);
	ImGui::DragFloat("saturation", &DirectXCommon::GetInstance()->GetHSVData()->saturation, 0.01f);
	ImGui::DragFloat("value", &DirectXCommon::GetInstance()->GetHSVData()->value, 0.01f);
	ImGui::End();
	if (Input::GetInstance()->TriggerKey(DIK_C)) {
		useDebugCamera_ = !useDebugCamera_;
	}
	if (useDebugCamera_) {
		debugCamera_->Update();
		viewProjection_.TransferMatrix();
	} else
	{
		// メインカメラの処理
		mainCamera_->Update();
		viewProjection_.UpdateMatrix();
	}
#ifdef _DEBUG
	ImGui::Begin("ViewProjection");
	ImGui::DragFloat3("translation", &viewProjection_.translation_.x, 0.01f);
	ImGui::DragFloat3("rotation", &viewProjection_.rotation_.x, 0.01f);
	ImGui::End();
#endif // _DEBUG
	
}

void GameScene::Draw() {
	//plane_->Draw();
	axis_->Draw();
	sphere_->Draw();
	//primitive_->Draw();
	//particles_->Draw();
	/*パーティクルマネージャの描画*/
	//particleManager_->Draw();
	//emitterManager_->Draw();
	//sprite_->Draw();
	//bunny_->Draw();
	//multiMaterial_->Draw();
	//multiMesh_->Draw();
	//suzanne_->Draw();
	//utahTeapot_->Draw();
	animatedCube_->Draw();
}
