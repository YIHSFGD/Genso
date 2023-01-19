#include "GalagaScene.h"


bool GalagaScene::init() {
    if (!Scene::init())
    {
        return false;
    }

    mouseListenr = EventListenerMouse::create();
    mouseListenr->onMouseDown = CC_CALLBACK_1(GalagaScene::onMouseClick, this);
    mouseListenr->onMouseMove = CC_CALLBACK_1(GalagaScene::onMouseMove, this);
    keyListenr = EventListenerKeyboard::create();
    keyListenr->onKeyPressed = CC_CALLBACK_2(GalagaScene::onKeyPressed, this);
    touchListenr->onTouchesBegan = CC_CALLBACK_2(GalagaScene::onTouchesBegan, this);
    touchListenr->onTouchesEnded = CC_CALLBACK_2(GalagaScene::onTouchesEnded, this);
    touchListenr->onTouchesMoved = CC_CALLBACK_2(GalagaScene::onTouchesMoved, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListenr, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyListenr, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListenr, this);
    
    game->Init(this);
    GalagaResource::Ready(game);
    //this->scheduleUpdate();
    schedule(CC_SCHEDULE_SELECTOR(GalagaScene::update), 0.01f);
}
