#include "MyGame.h"

void MyGame::Initialize()
{
	/*基底クラスの初期化*/
	Framework::Initialize();
	/*シーンマネージャの生成*/
	//sceneManager = new SceneManager();
	sceneManager = SceneManager::GetInstance();
	sceneManager->Initialize();
	/*タイトルシートの初期化*/
	//titleScene = new TitleScene();
	//titleScene->Initialize();
	// ゲームシーンの初期化
	//gameScene = new GameScene();
	//gameScene->Initialize();
	//baseScene = new GameScene();
	//baseScene->Initialize();
	/*最初のシーン生成*/
	sceneManager->ChangeScene("GAMEPLAY");
}



void MyGame::Finalize()
{
	// 各種解放
	//delete gameScene;
	//delete titleScene;
	//delete baseScene;
	//delete sceneManager;
	SceneManager::GetInstance()->Finalize();
	/*基底クラスの終了処理*/
	Framework::Finalize();
}

void MyGame::Update()
{
	/*基底クラスの更新処理*/
	Framework::Update();
	/*シーンマネージャの更新*/
	sceneManager->Update();
	//titleScene->Update();
	//gameScene->Update();
	//baseScene->Update();
}

void MyGame::PreDraw()
{
	/*基底クラスの描画前処理*/
	Framework::PreDraw();
	/*シーンマネージャの描画*/
	sceneManager->Draw();
	//titleScene->Draw();
	//gameScene->Draw();
	//baseScene->Draw();
}

void MyGame::PostDraw()
{
	/*基底クラスの描画後処理*/
	Framework::PostDraw();
}


