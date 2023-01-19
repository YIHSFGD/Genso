#include"Touhou.h"
class ThScene : public Scene {
public:
    ThResource::Touhou* game;
    EventListenerKeyboard* keyListenr;
    EventListenerPhysicsContact* contactListener;
    Cooltime shotCT = Cooltime(5);
    static cocos2d::Scene* createScene() {
        return ThScene::create();
    }
    virtual bool init();
    void menuCloseCallback(Ref* pSender)
    {
        Director::getInstance()->end();
    }
    virtual void update(float dt) override {
        KeyUpdate();
        game->update();
        game->defaultBackground->DownFall(Vec2(0, -2));
    }

    bool onContactBegin(PhysicsContact& pc) {
        UnitBody* us = dynamic_cast<UnitBody*>(pc.getShapeA()->getBody());
        if (!us) { return 0; }
        UnitBody* us2 = dynamic_cast<UnitBody*>(pc.getShapeB()->getBody());
        if (!us2) { return 0; }
        if ((us->unit->active == 0) || (us2->unit->active == 0)) {
            return 0;
        }
        if (us->unit->Collided) {
            if (!us->unit->Collided(us2->unit)) {
                us->unit->type = DELETE;
            }
        }
        if (us2->unit->Collided) {
            if (!us2->unit->Collided(us->unit)) {
                us2->unit->type = DELETE;
            }
        }

        return 0;
    }
    
    void KeyUpdate() {
       game->isLow = GetAsyncKeyState(VK_LSHIFT);
        if (GetAsyncKeyState(VK_UP)) {
            game->PlayerMove(Vec2(0, 1));
        }
        if (GetAsyncKeyState(VK_LEFT)) {
            game->PlayerMove(Vec2(-1, 0));
        }
        if (GetAsyncKeyState(VK_DOWN)) {
            game->PlayerMove(Vec2(0, -1));
        }
        if (GetAsyncKeyState(VK_RIGHT)) {
            game->PlayerMove(Vec2(1, 0));
        }
        if (GetAsyncKeyState(0x5A)) {
            if (shotCT.Update()) {
                game->PlayerShot();
            }
        }
        //for debug
        if (GetAsyncKeyState(0x57)) {
            game->defaultBackground->DownFall(Vec2(0, 5));
        }
        if (GetAsyncKeyState(0x53)) {
            game->defaultBackground->DownFall(Vec2(0, -5));
        } 
        if (GetAsyncKeyState(0x41)) {
            game->defaultBackground->DownFall(Vec2(-5, 0));
        }
        if (GetAsyncKeyState(0x44)) {
            game->defaultBackground->DownFall(Vec2(5, 0));
        }
    }

    void onKeyPressed(EventKeyboard::KeyCode keycode, Event* event) {
        switch (keycode)
        {/*
        case EventKeyboard::KeyCode::KEY_W:
        case EventKeyboard::KeyCode::KEY_A:
        case EventKeyboard::KeyCode::KEY_S:
        case EventKeyboard::KeyCode::KEY_D:
            nowMoveKey = (int)keycode | nowMoveKey;
            break;*/
        case EventKeyboard::KeyCode::KEY_T:
            game->AddTestUfo();
            break;
        }
    }
    CREATE_FUNC(ThScene);
};