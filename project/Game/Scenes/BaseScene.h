#pragma once
class SceneManager;
class BaseScene
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseScene() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 終了
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// シーンの切り替え依頼
	/// </summary>
	virtual void ChangeScene();

	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }
private:
	/*シーンマネージャを借りてくる*/
	SceneManager* sceneManager_ = nullptr;
};

