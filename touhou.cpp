#include "touhou.h"

namespace ThResource {
    Touhou* game;

    void BackGroundReimuLine::Init(int lineMax, Texture2D* line_, Vec2* startPos_, Vec2* endPos_) {
        if ((startPos_ == NULL) || (endPos_ == NULL)) {
            lineLimit = lineMax;
            startPos = new Vec2[lineLimit];
            endPos = new Vec2[lineLimit];
            int val;
            for (int i = 0; i < lineLimit; i++) {
                Vec2 gameSize = game->UInterface->playZone->getContentSize();
                std::uniform_int_distribution<int> weight(0, gameSize.x + gameSize.y);
                val = weight(game->mt);
                if (val > gameSize.x) {
                    startPos[i] = Vec2(0, val - gameSize.x);
                }
                startPos[i] = Vec2(val, 0);
                if (val > gameSize.x) {
                    endPos[i] = Vec2(gameSize.x, val - gameSize.x);
                }
                endPos[i] = Vec2(val, gameSize.y);
            }
        }
        else {
            startPos = startPos_;
            endPos = endPos_;
        }
        line = line_;
    }
    void BackGroundReimuLine::Show()
    {
        Sprite* sp = Sprite::create();
        sp->initWithTexture(line);
        Bullet* bullet = Bullet::create(sp, game->scene);
        Vec2 moveLine;
        Vec2 bulletPos;
        for (int i = 0; i < lineLimit; i++) {
            moveLine = endPos[i] - startPos[i];
            int bulletIndex = moveLine.getLength() / sp->getBoundingBox().size.height;
            moveLine = moveLine / bulletIndex;
            bulletPos = startPos[i] - moveLine;
            float rad = GetRadFromVec2(moveLine);
            for (int j = 0; j < bulletIndex; j++) {
                Bullet* copyed = bullet->Copy();
                copyed->sprite->setPosition(moveLine);
                copyed->AddAction(RotateTo::create(0, rad));
                copyed->AddAction(MoveBy::create(1.0f * ((float)j / bulletIndex), moveLine * j + moveLine));
                game->units->AddUnit(copyed);
            }
        }
    }
    void DefaltBackGround::Init(Sprite* mapTile, Node* node) {
        tile = mapTile;
        if (tile == NULL) {
            tile = Sprite::create("sky02.jpg");
        }
        Vec2 size = game->UInterface->playZone->getContentSize();
        Vec2 tileSize = tile->getContentSize();
        index = Vec2(size.x / tileSize.x + 3, size.y / tileSize.y + 4);
        Texture2D* tx = mapTile->getTexture();
        map = new Sprite * *[index.x];
        for (int i = 0; i < index.x; i++) {
            map[i] = new Sprite * [index.y];
            for (int y = 0; y < index.y; y++) {
                map[i][y] = Sprite::createWithTexture(tx);
                map[i][y]->setPosition(Vec2(tileSize.x * i - tileSize.x, tileSize.y * y - tileSize.y));
                node->addChild(map[i][y], INT_MIN);
            }
        }
    }
}

using namespace ThResource;

void ThResource::Touhou::Init(Node* node)
{
    scene = node;
    UInterface = new UserInterface;
    UInterface->Init(node);

    node = UInterface->playZone;
    units = new Units;
    units->Init(5, 5);
    player = Player::create(GetSprite("marisa.png", 32, 52, 0, 0), node);
    player->sprite->setLocalZOrder(20);
    playerBullet1 = Bullet::create(GetSprite("marisa.png", 32, 16, 0, 156), node, player);
    playerBullet1->spell = Spell<Bullet*>::create(BSpellDefalt);
    playerBullet1->targetBy = Vec2(0, 1);
    playerBullet1->speed = 10;
    playerBullet1->sprite->setRotation(-90);
    playerBullet1->AddType(PLDANMAKU);
    playerBullet1->active = 0;
    playerBullet1->SetBody(UnitBody::createBox(playerBullet1->sprite->getContentSize()));
    playerBullet2 = Bullet::create(GetSprite("marisa.png", 32, 16, 48, 156), node, player);
    playerBullet2->spell = Spell<Bullet*>::create(BSpellDefalt);
    playerBullet2->targetBy = Vec2(0, 1);
    playerBullet2->AddType(PLDANMAKU);
    playerBullet2->active = 0;
    playerSlowMagicCircle = GetSprite("th/stg7enm.png", 66, 66, 0, 96);
    player->sprite->addChild(playerSlowMagicCircle);
    playerSlowMagicCircle->setOpacity(0);
    playerSlowMagicCircle->setScale(0.8);
    playerSlowMagicCircle->setOpacity(255/2);
    playerSlowMagicCircle->setPosition(player->sprite->getContentSize() / 2);
    playerSlowMagicCircle->runAction(RepeatForever::create(RotateBy::create(3, 360)));

    enemyKong = Bullet::create(GetSprite("kShot.png", 21, 21, 0, 0), node);
    enemyKong->spell = Spell<Bullet*>::create(BSpellDefalt);
    enemySal = Bullet::create(GetSprite("kShot.png", 11, 20, 0, 61), node);
    enemySal->spell = Spell<Bullet*>::create(BSpellDefalt);
    bYousei = Enemy::create(GetSprite("th/stg7enm.png", 33, 33, 0, 0), node, GetAnimation("th/stg7enm.png", 33, 33, 0, 0, 8));
    rYousei = Enemy::create(GetSprite("th/stg7enm.png", 33, 33, 0, 33), node, GetAnimation("th/stg7enm.png", 33, 33, 0, 33, 8));
    oYousei = Enemy::create(GetSprite("th/stg7enm.png", 33, 33, 0, 66), node, GetAnimation("th/stg7enm.png", 33, 33, 0, 66, 8));
    bYousei->Collided = CollidedEnemy;
    bYousei->DeathRattle = DeathRattleHpDown;
    bYousei->hp = 10;
    rYousei->Collided = CollidedEnemy;
    rYousei->DeathRattle = DeathRattleHpDown;
    rYousei->hp = 10;
    oYousei->Collided = CollidedEnemy;
    oYousei->DeathRattle = DeathRattleHpDown;
    oYousei->hp = 10;

    playerGun = GetSprite("marisa.png", 16, 16, 32, 156);
    player->AddAnime(GetAnimation("marisa.png", 32, 52, 0, 0, 4));
    player->active = 0;
    player->sprite->setVisible(0);
    player->spell = CoolTimeSpell<IUnit*>::create(SpellPlayer, 60);
    player->SetBody(UnitBody::createCircle(2), false);
    player->sprite->setScale(2);

    units->AddUnit(player);
    ufo = Enemy::create(GetSprite("ufo.png", 960 / 5, 1344 / 7, 0, 0), node);
    ufo->AddAnime(GetAnimation("ufo.png", 960 / 5, 1344 / 7, 0, 0, 5, 0.2f, 15));
    ufo->hp = 10;
    ufo->spell = Spell<IUnit*>::create(SpellUfo);
    ufo->Collided = CollidedEnemy;
    ufo->DeathRattle = DeathRattleUfo;
    ufo->active = 0;
    ufoDeath = GetAnimation("ufo.png", 960 / 5, 1344 / 7, 0, 1344 / 7 * 6, 5);
    ufoDeath->retain();
    game = this;
    defaultBackground = new DefaltBackGround;
    defaultBackground->Init(Sprite::create("sky02.jpg"), node);

    oBook = Enemy::create(GetSprite("th/stg7enm.png", 45, 32, 190, 96), node);
    
    pachuri = Boss::create(GetSprite("th/stg7enm.png", 33, 56, 33, 200), node);
    pachuri->hp = 400;
    pachuri->Collided = CollidedEnemy;
    pachuri->active = 0;
    /*paticleFire = ParticleSystem::create("th/fire.plist");
    paticleFire->retain();*/

    //defaultDeathAnimation = Animation::createWithSpriteFrames(SpriteFrame::create("th/stg7enm.png", Rect(0, 96, 66, 66))));
    
    stage1.Init(Stage1, units);
    stage2.Init(Stage2, units);
    stageBegin.Init(StageAgeru, units);
}

UINT ThResource::Touhou::SpellYouseiDP(IUnit* unit, int* tick)
{
    if ((!(*tick % 100)) && (tick != 0)) {
        Bullet* bullet = game->enemyKong->Copy();
        bullet->SetBody(UnitBody::createCircle(10));
        bullet->InitSimple(5, Vec2(0, -1), unit);
        game->units->AddUnit(bullet);
        bullet = game->enemyKong->Copy();
        bullet->SetBody(UnitBody::createCircle(10));
        bullet->InitSimple(5, game->player->sprite->getPosition(), unit);
        game->units->AddUnit(bullet);
    }
    unit->sprite->setPosition(unit->sprite->getPosition() + Vec2(0, -1));
    return 1;
}

UINT ThResource::Touhou::SpellEffect(IUnit* unit, int* tick)
{
    if (!unit->sprite->isVisible()) {
        return DELETE;
    }
    return 1;
}

void ThResource::Touhou::PlayerShot()
{
    if (!player->active) {
        return;
    }
    Vec2 playerPos = player->sprite->getPosition();
    Vec2 playerSize = player->sprite->getContentSize();

    auto copyed = playerBullet1->Copy();
    copyed->sprite->setPosition(Vec2(playerPos.x - playerSize.x / 2, playerPos.y + playerSize.y));
    auto copyed2 = playerBullet1->Copy();
    copyed2->sprite->setPosition(Vec2(playerPos.x + playerSize.x / 2, playerPos.y + playerSize.y));
    Size size = Size(playerBullet1->sprite->getContentSize().height, playerBullet1->sprite->getContentSize().width);
    copyed->sprite->setVisible(1);
    copyed->SetBody(UnitBody::createBox(size));
    copyed2->sprite->setVisible(1);
    copyed2->SetBody(UnitBody::createBox(size));
    game->units->AddUnit(copyed);
    game->units->AddUnit(copyed2);
}

void ThResource::Touhou::PlayerActive()
{
    player->sprite->setPosition(game->UInterface->playZone->getContentSize().width/2, 100);
    player->active = 1;
    player->sprite->setVisible(1);
}

UINT ThResource::Touhou::SpellChangeColor(IUnit* unit, int* tick)
{
    //unit->sprite->setColor
    return 0;
}

UINT ThResource::Touhou::SpellSpectrum(IUnit* unit, int* tick)
{
    Sprite* sp = Sprite::create();
    sp->initWithSpriteFrame(unit->sprite->getSpriteFrame());

    unit->sprite->getParent()->addChild(sp);
    sp->setPosition(unit->sprite->getPosition());
    sp->setRotation(unit->sprite->getRotation());
    sp->runAction(Sequence::create(
        FadeOut::create(3),
        RemoveSelf::create(),
        nullptr
    ));
    //IUnit* effect = IUnit::create(sp, unit->sprite->getParent());
    //effect->AddType(EFFECT);
    //effect->AddAction(FadeOut::create(3));
    //effect->spell = Spell<IUnit*>::create(Resource::SpellDeleteActionEnd); //set special effect
    //game->units->AddUnit(effect);
    return 1;
}

UINT Touhou::PlayerMove(Vec2 normalPos) {
    if (!player->active) {
        return 0;
    }
    Vec2 move = normalPos * (isLow ?  game->playerLowSpeed : game->playerSpeed);
    game->player->sprite->setPosition(game->player->sprite->getPosition() + move);
    if (!DeleteOutOfGame(game->player, NULL)) {
        game->player->sprite->setPosition(game->player->sprite->getPosition() - move);
    }
    return 1;
}

void ThResource::Touhou::AddTestUfo()
{
    IUnit* enemy = ufo->Copy();
    enemy->sprite->setVisible(1);
    enemy->sprite->setPosition(Vec2(100, game->UInterface->playZone->getContentSize().height + 100));

    ccBezierConfig bc;
    bc.controlPoint_1 = Vec2(0, -200);
    bc.controlPoint_2 = Vec2(-200, 0);
    bc.endPosition = Vec2(400, -400);
    enemy->AddAction(BezierBy::create(3, bc));
    enemy->SetBody(UnitBody::createBox(playerBullet1->sprite->getContentSize()));
    units->AddUnit(enemy);
}

UINT ThResource::Touhou::SpellPlayer(IUnit* unit, int* tick)
{
    SpellFireRain(unit, tick);
    Resource::DefaltAnimationSpell(unit, *tick);
    return 1;
}

UINT ThResource::Touhou::SpellUfo(IUnit* unit, int* tick)
{
    Resource::DefaltAnimationSpell(unit, *tick);
    if (!(*tick % 100)) {
        if (!(*tick % 500)) {
            Vec2 weight = Vec2(-60, 0);
            for (int i = 0; i < 5; i++) {
                weight.x += 30;
                Bullet* copyed = game->enemyKong->Copy();
                copyed->sprite->setPosition(unit->sprite->getPosition());
                copyed->InitSimple(2, game->player->sprite->getPosition() + weight);
                copyed->SetBody(UnitBody::createCircle(10));
                game->units->AddUnit(copyed);
            }
            return 1;
        }
        Bullet* copyed = game->enemyKong->Copy();
        copyed->sprite->setPosition(unit->sprite->getPosition());
        copyed->InitSimple(5, game->player->sprite->getPosition());
        copyed->SetBody(UnitBody::createCircle(10));
        game->units->AddUnit(copyed);
        return 1;
    }
    return 1;
}

UINT ThResource::Touhou::BSpellDefalt(Bullet* unit, int* tick)
{
    if (!DeleteOutOfGame(unit, tick)) {
        return 0;
    }
    if (unit->target) {
        unit->to = (unit->target->sprite->getPosition() - unit->sprite->getPosition()).getNormalized();
        if (unit->targetSetOnce) {
            unit->target = NULL;
        }
    }
    else if (unit->targetTo) {
        unit->to = (*unit->targetTo - unit->sprite->getPosition()).getNormalized();
        unit->targetTo = NULL;
    }
    else if ((unit->targetBy.x != 0) || (unit->targetBy.y != 0)) {
        unit->to = (unit->targetBy).getNormalized();
        unit->targetBy = Vec2(0, 0);
    }

    unit->AddAction(MoveBy::create(0, unit->to * unit->speed));
    //BSpellShotPoint(unit, tick);
    return 1;
}


UINT ThResource::Touhou::BSpellWaitFollowTarget(Bullet* unit, int* tick)
{
    if (unit->type == DanmakuFollowTarget) {
        unit->spell = Spell<Bullet*>::create(BSpellFollowTarget);
    }
    return Resource::DeleteOutOfWindow(unit, tick);
}

UINT ThResource::Touhou::BSpellFollowTarget(Bullet* unit, int* tick) {
    return 0;
}

UINT ThResource::Touhou::BSpellFireRainBullet(Bullet* unit, int* tick)
{
    BSpellDefalt(unit, tick);
    return 1;
}

UINT ThResource::Touhou::FSpellYouseiDash(IUnit* unit, int* tick) {
    ////shot
    Vec2 speed = (game->player->sprite->getPosition() - unit->sprite->getPosition()).getNormalized() * 100;
    unit->sprite->getPhysicsBody()->setVelocity(
        speed
    );
    return 1;
}

UINT ThResource::Touhou::CollidedTargetPlayer(IUnit* with)
{
    return DELETE;
}

UINT ThResource::Touhou::DeathRattleChangeFollowTargetDanmaku(IUnit* unit, Units* unist)
{
    unit->type = DanmakuFollowTarget;
    
    return 1;
}

UINT ThResource::Touhou::DeathRattleCreateNewDanmaku(IUnit* unit, Units* units)
{
    Vec2 pos = unit->sprite->getPosition();
    for (int i = 0; i < 10; i++) {
        Bullet* copyed = game->enemyKong->Copy();
        copyed->spell = Spell<Bullet*>::create(Resource::BSpellToPoint);
        std::uniform_int_distribution<int> range(-30, 30);
        copyed->sprite->setPosition(Vec2(pos.x + range(game->mt), pos.y + range(game->mt)));
        copyed->SetBody(UnitBody::createCircle(10));
        copyed->targetBy = Vec2(game->player->sprite->getPosition()+ Vec2(range(game->mt), 0) - unit->sprite->getPosition());
        game->units->AddUnit(copyed);
    }

    return 0;
}

UINT ThResource::Touhou::CollidedEnemy(IUnit* with)
{
    if (with->type == PLDANMAKU) {
        with->type = DELETE;
        return DELETE;
    }
    return 0;
}

UINT ThResource::Touhou::DeathRattleHpDown(IUnit* unit, Units* units)
{
    if (unit->hp > 0) {
        unit->hp--;
        return 1;
    }
    return 0;
}

UINT ThResource::Touhou::DeathRattleUfo(IUnit* unit, Units* units)
{
    if (DeathRattleHpDown(unit, units)) {
        return 1;
    }
    if (unit->id != EFFECT) {
        DeathRattleCreateNewDanmaku(unit, units);
        unit->AddType(EFFECT);
        unit->spell = Spell<IUnit*>::create(SpellEffect);
        unit->Collided = NULL;
        Resource::DeleteAllAction(unit);
        unit->AddAction(Sequence::create(Animate::create(game->ufoDeath), SetUnvisible::create(0), nullptr));
        unit->AddAction(Sequence::create(Animate::create(game->ufoDeath), SetUnvisible::create(0), nullptr));
        //SimpleAudioEngine::getInstance()->playEffect("Sound/Kill.mp3");
        return 1;
    }
    return DELETE;
}

UINT ThResource::Touhou::DeathRattleDefault(IUnit* unit, Units* units)
{
    if (DeathRattleHpDown(unit, units)) {
        return 1;
    }
    if (unit->id != EFFECT) {
        unit->AddType(EFFECT);
        unit->spell = Spell<IUnit*>::create(SpellEffect);
        unit->Collided = NULL;
        Resource::DeleteAllAction(unit);
        //unit->AddAction(Sequence::create(Animate::create()))////////////
    }
    return 0;
}

UINT ThResource::Touhou::DeleteOutOfGame(IUnit* unit, int* tick)
{
    Vec2 unitPos = unit->sprite->getPosition();
    Vec2 unitMaxPos = unitPos + unit->sprite->getContentSize();
    Size playSize = game->UInterface->playZone->getContentSize();
    if ((unitPos.x < 0) ||
        (unitPos.y < 0) ||
        (unitPos.x > playSize.width) ||
        (unitPos.y > playSize.height))
    { return DELETE; }
    return 1;
}

int ThResource::Touhou::StageAgeru(int* unit, Units* units)
{
    game->UInterface->StageStart();
    if (game->UInterface->startShow->getActionManager()->getNumberOfRunningActionsInTarget(
        game->UInterface->startShow) == 0) {
        return 1;
    }
    return 0;
}

UINT ThResource::Touhou::FSpellDeleteSelf(IUnit* unit, int* tick)
{
    unit->type = DELETE;
    return 0;
}

UINT ThResource::Touhou::FSpellWaitActionDone(IUnit* unit, int* tick)
{
    if (unit->sprite->getActionManager()->getNumberOfRunningActionsInTarget(unit->sprite) == 0) {
        return 1;
    }
    return 0;
}

int ThResource::Touhou::Stage1(int* tick, Units* units) {
    if (*tick > 70 * 12) {
        return 1;
    }
    Vec2 pos = game->UInterface->enemyLinePos;
    int width = game->UInterface->enemyLineWidth;
    IUnit* copyed;
    if (*tick <= 350) {
        if (!(*tick % 70)) {
            copyed = game->rYousei->Copy();
            copyed->spell = Spell<IUnit*>::create(SpellYouseiDP);
            copyed->SetBody();
            copyed->sprite->setPosition(pos);
            game->units->AddUnit(copyed);
            copyed = game->rYousei->Copy();
            copyed->spell = Spell<IUnit*>::create(SpellYouseiDP);
            copyed->SetBody();
            copyed->sprite->setPosition(Vec2(pos.x + width, pos.y));
            game->units->AddUnit(copyed);
        }
    }
    if ((*tick >= 210) && (*tick <= 490)) {
        if (!(*tick % 70)) {
            copyed = game->rYousei->Copy();
            copyed->spell = Spell<IUnit*>::create(SpellYouseiDP);
            copyed->SetBody();
            copyed->sprite->setPosition(Vec2(pos.x + width/3, pos.y));
            game->units->AddUnit(copyed);
            copyed = game->rYousei->Copy();
            copyed->spell = Spell<IUnit*>::create(SpellYouseiDP);
            copyed->SetBody();
            copyed->sprite->setPosition(Vec2(pos.x + width - width/3, pos.y));
            game->units->AddUnit(copyed);
        }
    }
    if (*tick >= 70 * 6) {
        if (!(*tick % 70)) {
            copyed = game->rYousei->Copy();
            copyed->spell = Spell<IUnit*>::create(SpellYouseiDP);
            copyed->SetBody();
            copyed->sprite->setPosition(Vec2(pos.x + width/2, pos.y));
            game->units->AddUnit(copyed);
        }
    }
    return 0;
}

UINT ThResource::Touhou::SpellFireRain(IUnit* unit, int* tick)
{
    if ((*tick % 50)) {
        Bullet* bullet = game->enemySal->Copy();
        bullet->sprite->setPosition(game->player->sprite->getPosition());
        bullet->SetBody(UnitBody::createBox(Size(11, 20), PhysicsMaterial(1, 1, 1)), true);
        Size size = game->UInterface->playZone->getContentSize();
        std::uniform_int_distribution<int> width(0, size.width);
        bullet->sprite->getPhysicsBody()->setVelocity(Vec2(width(game->mt),size.height) - bullet->sprite->getPosition());
        
        ParticleSystemQuad* sun = ParticleSun::create();
        bullet->sprite->addChild(sun);
        game->units->AddUnit(bullet);
    }
    return 1;
}

int ThResource::Touhou::Stage2(int* tick, Units* units) {
    if (*tick > 70 * 4 * 5) {
        return 1;
    }
    IUnit* copyed;
    Vec2 pos = game->UInterface->drawZone->getContentSize();
    Vec2 maxPos = game->UInterface->enemyLinePos;
    int maxWidth = game->UInterface->enemyLineWidth;
    int y = pos.y / 4 * 3;
    if (*tick <= 70 * 4) {
        if (!(*tick % 70)) {
            int weight = *tick / 70 + 1;
            copyed = game->bYousei->Copy();
            copyed->SetBody();
            copyed->sprite->setPosition(Vec2(-100, y));
            copyed->AddAction(Sequence::create(
                MoveTo::create(3, Vec2(pos.x / 2 - maxWidth / 8 * weight, y)),
                DelayTime::create(3),
                AddFSpell<IUnit*>::create(FinalSpell<IUnit*>::create(FSpellYouseiDash, copyed), copyed),
                DelayTime::create(10),
                AddFSpell<IUnit*>::create(FinalSpell<IUnit*>::create(FSpellWaitActionDone, copyed), copyed),
                AddFSpell<IUnit*>::create(FinalSpell<IUnit*>::create(FSpellDeleteSelf, copyed), copyed),
                nullptr));
            game->units->AddUnit(copyed);
            copyed = game->rYousei->Copy();
            copyed->SetBody();
            copyed->sprite->setPosition(Vec2(pos.x + 100, y));
            copyed->AddAction(Sequence::create(
                MoveTo::create(3, Vec2(pos.x / 2 + maxWidth / 8 * weight, y)),
                DelayTime::create(3),
                AddFSpell<IUnit*>::create(FinalSpell<IUnit*>::create(FSpellYouseiDash, copyed), copyed),
                DelayTime::create(10),
                AddFSpell<IUnit*>::create(FinalSpell<IUnit*>::create(FSpellWaitActionDone, copyed), copyed),
                AddFSpell<IUnit*>::create(FinalSpell<IUnit*>::create(FSpellDeleteSelf, copyed), copyed),
                nullptr));
            game->units->AddUnit(copyed);
        }
    }
    return 0;
}

int ThResource::Touhou::StagePachuriPre1(int* tick, Units* units) {
    return 1;
}