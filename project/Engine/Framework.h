#pragma once
#include "WinApp.h"
#include"DirectXCommon.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include"SrvManager.h"
#include"Input.h"
#include"Audio.h"
//#include"SceneManager.h"
/// <summary>
/// ゲーム全体
/// </summary>
class Framework
{
public:/*メンバ関数*/
	virtual ~Framework() = default;
	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 終了
	/// </summary>
	virtual void Finalize();

	/// <summary>
	///	ゲームプレイ
	/// </summary>
	void Operation();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void PreDraw();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void PostDraw();

	virtual bool IsEndRequest();
private:
	D3DResourceLeakChecker* leakCheck = nullptr;
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	// 汎用機能
	Input* input = nullptr;
	Audio* audio = nullptr;
	ImGuiManager* imguiManager = nullptr;
	SrvManager* srvManager = nullptr;
	//SceneManager* sceneManager = nullptr;
	/*ループ終了*/
	bool endRequest_ = false;
};

