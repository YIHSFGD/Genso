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

#include "Scenes.h"

USING_NS_CC;

bool Ami::init() {
    if (!Scene::init())
    {
        return false;
    }
    mouseListenr = EventListenerMouse::create();
    mouseListenr->onMouseDown = CC_CALLBACK_1(Ami::onMouseClick, this);
    mouseListenr->onMouseMove = CC_CALLBACK_1(Ami::onMouseMove, this);
    keyListenr = EventListenerKeyboard::create();
    keyListenr->onKeyPressed = CC_CALLBACK_2(Ami::onKeyPressed, this);
    //keyListenr->onKeyReleased = CC_CALLBACK_2(Ami::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListenr, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyListenr, this);
    player = Player::create(GetSprite("ami.png", 640 / 5, 512 / 4, 0, 0), this);
    player->sprite->setPosition(500, 500);
    
    auInterface = AmiUI::create(Sprite::create("push.png"));
    units->Init(5, 5);
    Resource::Init(this, units, player, auInterface);
    units->AddUnit(player);
    auInterface->AutoPosInit();
    auInterface->Active(this);
    auInterface->ViewPlayerPush();
    Resource::LoadAllAmiResource(this);
    auInterface->playerSpeed = 5;
    player->spell = Spell<IUnit*>::create(AmiResource::PlayerSpell);
    //this->scheduleUpdate();
    schedule(CC_SCHEDULE_SELECTOR(Ami::update), 0.01f);
}


Scene* SelectScene::createScene()
{
    return SelectScene::create();
}

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

bool SelectScene::init()
{
    if ( !Scene::init() )
    {
        return false;
    }
    //dynamic_cast<GLViewImpl*>(cocos2d::Director::getInstance()->getOpenGLView())->setFullscreen();
    uInterface = new SelectUI;
    uInterface->AutoPosInit();
    Image* img = new Image;
    img->initWithImageFile("bar.png"); 
    Texture2D* tx = new Texture2D;
    tx->initWithImage(img);

    Vector<Label*>* vec = new Vector<Label*>;
    vec->pushBack(CreateLabel("GALAGA"));
    vec->pushBack(CreateLabel("Option"));
    vec->pushBack(CreateLabel("TH"));
    vec->pushBack(CreateLabel("shutdown"));
    mouseListenr = EventListenerMouse::create();
    mouseListenr->onMouseMove = CC_CALLBACK_1(SelectScene::onMouseMove, this);
    mouseListenr->onMouseDown = CC_CALLBACK_1(SelectScene::onMouseClick, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListenr, this);
    Resource::LoadDefaltSound();

    Vector<Texture2D*>* vecT = new Vector<Texture2D*>;
    Texture2D* tx1 = new Texture2D;
    img = new Image;
    img->initWithImageFile("wall1.png");
    tx1->initWithImage(img);
    vecT->pushBack(tx1);
    Texture2D* tx2 = new Texture2D;
    img = new Image;
    img->initWithImageFile("wall2.png");
    tx2->initWithImage(img);
    vecT->pushBack(tx2);
    Texture2D* tx3 = new Texture2D;
    img = new Image;
    img->initWithImageFile("wall3.png");
    tx3->initWithImage(img);
    vecT->pushBack(tx3);
    Texture2D* tx4 = new Texture2D;
    img = new Image;
    img->initWithImageFile("wall4.png");
    tx4->initWithImage(img);
    vecT->pushBack(tx4);
    Texture2D* tx5 = new Texture2D;
    img = new Image;
    img->initWithImageFile("wall5.png");
    tx5->initWithImage(img);
    vecT->pushBack(tx5);
    /*Texture2D** test = new Texture2D * [4];
    test[0] = new Texture2D;
    Image* img2 = new Image;
    img2->initWithImageFile("wall1.png");
    test[0]->initWithImage(img2);*/

    /*auto name = ShaderNode::shaderNodeWithVertex("", "shadertoy_Glow.fsh");
    name->setPosition(Vec2(600, 600));
    addChild(name, INT_MAX);
    name->setScale(3);*/

    uInterface->Init(vecT, Sprite::create("logoRS.png"), tx, vec, GetAnimation("bar_ani.png", 317, 33, 5, 0, 5, 0.2f), Sprite::create("note.png"), this);
    uInterface->Active(this);
    uInterface->FadeIn();
    SimpleAudioEngine::getInstance()->playBackgroundMusic("Sound/title.mp3", 1);

    this->scheduleUpdate();
    return true;
}

void SelectScene::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
//
//bool SelectScene::onTouchEnded(Touch* touch, Event* e) {
//    Vec2 touchPos = touch->getLocation();
//    return 0;
//}
//
//bool SelectScene::onTouchMoved(Touch* touch, Event* e)
//{
//    Vec2 touchPos = touch->getLocation();
//    this->convertToWorldSpace(touchPos);
//    su->SetMousePos(touchPos);
//    return false;
//}