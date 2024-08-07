#include "TitleScene.h"
#include"SceneManager.h"
#include"imgui.h"
void TitleScene::Initialize()
{
}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
{
#ifdef _DEBUG
	ImGui::Begin("Scene");
	if (ImGui::Button("ChangeScene")) {
		/*シーン切り替え依頼*/
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}
	ImGui::End();
#endif // _DEBUG
}

void TitleScene::Draw()
{
}

void TitleScene::ChangeScene()
{
	
}
