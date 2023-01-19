#include"Game.h"
#include"ui/CocosGUI.h"
#include<thread>
class LodingScene : public Scene {
public:
    ui::LoadingBar* loadingBar;
    static cocos2d::Scene* createScene() {
        return LodingScene::create();
    }
    virtual bool init();
    void menuCloseCallback(Ref* pSender)
    {
        Director::getInstance()->end();
    }

    CREATE_FUNC(LodingScene);
};