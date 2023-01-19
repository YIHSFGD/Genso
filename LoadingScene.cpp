#include"LoadingScene.h"


bool LodingScene::init() {
    if (!Scene::init())
    {
        return false;
    }
    loadingBar = ui::LoadingBar::create();

}