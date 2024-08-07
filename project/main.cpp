#pragma warning(push)
#pragma warning(disable:28251)
#include"MyGame.h"
#include"Framework.h"
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#pragma warning(pop)
	Framework* game = new MyGame();

	game->Operation();

	delete game;

	return 0;
}