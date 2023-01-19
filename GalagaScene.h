#include "Galaga.h"

USING_NS_CC;
class GalagaScene : public cocos2d::Scene {
public:
    GalagaResource::Galaga* game = new GalagaResource::Galaga;

    Vec2 savedPoint = Vec2(0, 0);
    EventListenerMouse* mouseListenr;
    EventListenerKeyboard* keyListenr;
    EventListenerTouchAllAtOnce* touchListenr = EventListenerTouchAllAtOnce::create();
    Cooltime shotCT = Cooltime(30);

    static cocos2d::Scene* createScene() {
        return GalagaScene::create();
    }
    virtual bool init();
    void menuCloseCallback(Ref* pSender)
    {
        Director::getInstance()->end();
    }
    virtual void update(float dt) override {
        shotCT.Update(1);
        game->UpdateGalaga();
    }
    void onKeyPressed(EventKeyboard::KeyCode keycode, Event* event) {
        switch (keycode)
        {
        case EventKeyboard::KeyCode::KEY_W:
            game->player->AddAction(MoveBy::create(0, Vec2(0, 1)));
            break;
        case EventKeyboard::KeyCode::KEY_E:
            AmiResource::PlayerDefence();
            break;
        }
    }
    void onMouseMove(Event* event) {
        EventMouse* em = (EventMouse*)event;
        if (game->player->active) {
            Resource::UpdateMousePos(Vec2(em->getCursorX(), em->getCursorY()));
        }
    }
    void onMouseClick(Event* event) {
        EventMouse* em = (EventMouse*)event;
        if (!game->player->active) {
            if (game->dui->MouseClick(Vec2(em->getCursorX(), em->getCursorY()))) {
                Animation* test = GetAnimation("ami.png", 640 / 5, 512 / 4, 640 / 5 * 2, 512 / 4, 5, 0.2, 8, 2);
                AmiResource::ActivePlayer(Vec2(em->getCursorX(), em->getCursorY()),
                    GetAnimation("ami.png", 640 / 5, 512 / 4, 0, 0, 5),
                    test,
                    game->player
                );
                game->Start();
            }
        }
    }

    void onTouchesMoved(std::vector<Touch*> touch, Event* e)
    {
        // Moved
        auto touch1 = touch[0];
        Vec2 touchPoint2 = touch1->getLocation();
        if ((touchPoint2.y < WINDOWHEIGHT / 2)) {
            // touchPoint2 = this->convertToWorldSpace(touchPoint2);
            touchPoint2 = savedPoint - touchPoint2;
            savedPoint = touch1->getLocation();
            if (game->player->activeMove) {
                game->SetPlayerMove(-touchPoint2.x);
            }
            CCLOG("x = %f", -touchPoint2.x);
        }
    }

    void onTouchesBegan(std::vector<Touch*> touch, Event* e)
    {
        auto touch1 = touch[0];
        savedPoint = touch1->getLocation();

        Vec2 touchPoint = touch[0]->getLocation();
        if ((WINDOWHEIGHT / 2 < touchPoint.y))
        {
            if (shotCT.Update()) {
                game->PlayerAttack();
            }
        }
    }
    void onTouchesEnded(std::vector<Touch*> touch, Event* e) { }
    CREATE_FUNC(GalagaScene);
};
