#include "SceneFactory.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName)
{
	/*次のシーン生成*/
	BaseScene* newScene = nullptr;
	if (sceneName == "TITLE") {
		newScene = new TitleScene();
	} else if (sceneName == "GAMEPLAY") {
		newScene = new GameScene();
	}
	return newScene;
}
