#pragma once
#include <Windows.h>
#include<cstdint>
class WinApp
{
public:// 静的メンバ変数
	// クライアント領域サイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

public:// 静的メンバ関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static WinApp* GetInstance();

	// ウィンドウプロシージャ
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg,
		WPARAM wparam, LPARAM lparam);

public:// メンバ関数
	// ゲームウィンドウの作成
	void CreateGameWindow();

	/// <summary>
	/// メッセージ処理
	/// </summary>
	bool ProcessMessage();

	HWND GetHwnd() const { return hwnd_; }

	HINSTANCE GetHInstance() const { return wc_.hInstance; }

private:// メンバ関数
	WinApp() = default;
	~WinApp() = default;
	WinApp(const WinApp&) = delete;
	const WinApp& operator=(const WinApp&) = delete;

private:// メンバ変数
	HWND hwnd_ = nullptr;

	WNDCLASS wc_{}; // ウィンドウクラス
};

