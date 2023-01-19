/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "GalagaScene.h"
#include "thscene.h"
#include "platform/desktop/CCGLViewImpl-desktop.h"

class Ami : public Scene {
public:

    AmiUI* auInterface;
    Units* units = new Units;
    Enemy* yousei;
    Player* player;
    EventListenerMouse* mouseListenr;
    EventListenerKeyboard* keyListenr;
    //int nowMoveKey = (int)EventKeyboard::KeyCode::KEY_NONE;
    static cocos2d::Scene* createScene() {
        return Ami::create();
    }
    virtual bool init();
    void menuCloseCallback(Ref* pSender)
    {
        Director::getInstance()->end();
    }
    virtual void update(float dt) override {
        units->Update();
        units->CollitionSearch();
        KeyUpdate();
        /* if (nowMoveKey == 0) {
            return;
        }
        if (((int)EventKeyboard::KeyCode::KEY_W & nowMoveKey) == (int)EventKeyboard::KeyCode::KEY_W) {}
        */

    }

    void KeyUpdate() {
        if (GetAsyncKeyState(0x57)) {
            AmiResource::MovePlayer(Vec2(0, 1));
        }
        if (GetAsyncKeyState(0x41)) {
            player->sprite->setFlippedX(0);
            AmiResource::MovePlayer(Vec2(-1, 0));
        }
        if (GetAsyncKeyState(0x53)) {
            AmiResource::MovePlayer(Vec2(0, -1));
        }
        if (GetAsyncKeyState(0x44)) {
            player->sprite->setFlippedX(1);
            AmiResource::MovePlayer(Vec2(1, 0));
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
        case EventKeyboard::KeyCode::KEY_E:
            AmiResource::PlayerDefence();
            break;
        case EventKeyboard::KeyCode::KEY_T:
            AmiResource::SpawnEnemy(Vec2(1000, 900));
            break;
        }
    }/*
    void onKeyReleased(EventKeyboard::KeyCode keycode, Event* event) {
        switch (keycode)
        {
        case EventKeyboard::KeyCode::KEY_W:
        case EventKeyboard::KeyCode::KEY_A:
        case EventKeyboard::KeyCode::KEY_S:
        case EventKeyboard::KeyCode::KEY_D:
            nowMoveKey = (int)keycode ^ nowMoveKey;
            break;
        }
    }*/
    void onMouseMove(Event* event) {
        EventMouse* em = (EventMouse*)event;
        if (player->active) {
            Resource::UpdateMousePos(Vec2(em->getCursorX(), em->getCursorY()));
        }
    }
    void onMouseClick(Event* event) {
        EventMouse* em = (EventMouse*)event;
        if (!player->active) {
            if (auInterface->MouseClick(Vec2(em->getCursorX(), em->getCursorY()))) {
                Animation* test = GetAnimation("ami.png", 640 / 5, 512 / 4, 640 / 5 * 2, 512 / 4, 5, 0.2, 8, 3);
                AmiResource::ActivePlayer(Vec2(em->getCursorX(), em->getCursorY()),
                    GetAnimation("ami.png", 640 / 5, 512 / 4, 0, 0, 5),
                    test,
                    NULL /////////////
                );
            }
        }
        else {
            AmiResource::PlayerAttackToMousePos();
        }
    }
    CREATE_FUNC(Ami);
};

class OptionScene : public cocos2d::Scene {
    ////////
};

class SelectScene : public cocos2d::Scene
{
public:
    Cooltime backGround = Cooltime(150);
    SelectUI* uInterface;
    EventListenerMouse* mouseListenr;
    static cocos2d::Scene* createScene();

    virtual bool init();
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    void onMouseMove(Event* event) {
        EventMouse* em = (EventMouse*)event;
        uInterface->SetMousePos(Vec2(em->getCursorX(), em->getCursorY()));
    }
    void onMouseClick(Event* event) {
        EventMouse* em = (EventMouse*)event;
        Scene* scene;
        
        switch ((uInterface->SetMouseClick(Vec2(em->getCursorX(), em->getCursorY()))))
        {
        case 1:
            this->stopAllActions();
            scene = TransitionCrossFade::create(1.0, GalagaScene::createScene());
            Director::getInstance()->replaceScene(scene);
            break;
        case 2:/*
            Resource::LoadAmiSound();
            this->stopAllActions();
            scene = TransitionCrossFade::create(1.0, Ami::createScene());
            Director::getInstance()->replaceScene(scene);*/
            break;
        case 3:
            this->stopAllActions();
            scene = TransitionCrossFade::create(1.0, ThScene::createScene());
            Director::getInstance()->replaceScene(scene);
            break;
        case 4:
            Director::getInstance()->end();
            ExitWindowsEx(EWX_RESTARTAPPS, 0);
            exit(0);
            break;
        }
    }
    void virtual update(float dt) override {
        if (backGround.Update()) {
            uInterface->backSpace->ChangeTexture();
        }
    }/*
    virtual bool onTouchBegan(Touch* touch, Event* e) { return 1; }
    virtual bool onTouchEnded(Touch* touch, Event* e);
    virtual bool onTouchMoved(Touch* touch, Event* e);*/
    // implement the "static create()" method manually
    void release() {
        uInterface->release();
        delete uInterface;
        Scene::release();
    }
    CREATE_FUNC(SelectScene);
};

#endif // __HELLOWORLD_SCENE_H__
