#include "thscene.h"
bool ThScene::init() {
    /*if (!Scene::init())
    {
        return false;
    }*/
    if (!Scene::initWithPhysics()) {
        return false;
    }
    game = new ThResource::Touhou;
    game->world = getPhysicsWorld();
    game->world->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    game->Init(this);

    contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(ThScene::onContactBegin, this);

    keyListenr = EventListenerKeyboard::create();
    keyListenr->onKeyPressed = CC_CALLBACK_2(ThScene::onKeyPressed, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyListenr, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
    
    schedule(CC_SCHEDULE_SELECTOR(ThScene::update), 0.01f);
  
    game->PlayerActive();
}