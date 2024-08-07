#include "Framework.h"

void Framework::Initialize()
{
	CoInitializeEx(0, COINIT_MULTITHREADED);
	leakCheck = D3DResourceLeakChecker::GetInstance();
	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow();
	// DirectXの初期化
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);
#pragma region 汎用機能初期化
	// Srvマネージャの初期化
	srvManager = SrvManager::GetInstance();
	srvManager->Initialize(dxCommon);
	/*ちょっとここに置かせていただきます*/
	dxCommon->CreateRenderTexture();
	dxCommon->CreateDepthTexture();
	// ImGuiの初期化
	imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon, srvManager);
	// 入力の初期化
	input = Input::GetInstance();
	input->Initialize();
	/*音声の初期化*/
	audio = Audio::GetInstance();
	audio->Initialize();
	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice(), dxCommon, srvManager);
	

#pragma endregion
}

void Framework::Finalize()
{
	dxCommon->FenceClose();
	// ImGui解放
	imguiManager->Finalize();
	CloseWindow(win->GetHwnd());
	// COMの終了処理
	CoUninitialize();
}

void Framework::Operation()
{
	Initialize();
	// メインループ
	while (true) {
		/*毎フレーム更新*/
		Update();
		if (IsEndRequest()) {
			break;
		}
		PreDraw();
		PostDraw();
	}
	/*ゲーム終了処理*/
	Finalize();
}

void Framework::Update()
{
	if (win->ProcessMessage()) {
		endRequest_ = true;
	}
	// ImGui受付開始
	imguiManager->Begin();
	// 入力関連の毎フレーム処理
	input->Update();
}

void Framework::PreDraw()
{
	// ImGui受付終了
	imguiManager->End();
	// 描画開始
	//dxCommon->PreDraw();

	srvManager->PreDraw();
	// 描画開始
	dxCommon->PreRTVDraw();
}

void Framework::PostDraw()
{
	// 描画開始
	dxCommon->PreDraw();
	// ImGui描画
	imguiManager->Draw();
	// 描画終了
	dxCommon->PostDraw();
}

bool Framework::IsEndRequest()
{
	if (endRequest_) {
		return true;
	}
	return false;
}
