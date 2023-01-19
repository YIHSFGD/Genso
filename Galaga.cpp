#include"Galaga.h"

namespace GalagaResource {
    Galaga* galagaGame;
    
}

void GalagaResource::Ready(Galaga* game) {
    galagaGame = game;
}

using namespace Resource;
using namespace GalagaResource;

void Galaga::Init(Node* node)
{
    dui = GalagaUI::create(Sprite::create("push.png"));
    units = new Units;
    units->Init(5, 5);
    dui->AutoPosInit();
    dui->Active(node);
    dui->ViewPlayerPush();
    starField->Init(20, 5);
    LoadAllGalagaResource(node);
}

void Galaga::LoadAllGalagaEnemy(Node* node) {
    bee = IUnit::create(GetSprite("bug.png", 32, 32, 0, 32 * 3), node);
    bee->AddType(ENEMY);
    bee->Collided = CollidedEnemy;
    bee->DeathRattle = EnemyDeathRattle;
    galaga = IUnit::create(GetSprite("bug.png", 32, 32, 0, 32 * 2), node);
    galaga->AddType(ENEMY);
    galaga->Collided = CollidedEnemy;
    galaga->DeathRattle = EnemyDeathRattle;
    boss1 = IUnit::create(GetSprite("bug.png", 32, 32, 0, 0), node);
    boss1->AddType(ENEMY);
    boss1->Collided = CollidedEnemy;
    boss1->DeathRattle = BossDeathRattle;
    boss2 = IUnit::create(GetSprite("bug.png", 32, 32, 0, 32 * 1), node);
    boss2->AddType(ENEMY);
    boss2->Collided = CollidedEnemy;
    boss2->DeathRattle = BossDeathRattle2;
    plDanmaku = Bullet::create(GetSprite("danmaku.png", 32, 32, 0, 0), node);
    plDanmaku->AddType(PLDANMAKU);
    plDanmaku->speed = 5;
    plDanmaku->DeathRattle = DanmakuDeathRattle;
    danmaku = Bullet::create(GetSprite("danmaku.png", 32, 32, 32, 0), node);
    danmaku->speed = 5;
    enemyDeathAni = GetAnimation("death.png", 32, 32, 0, 0, 4);
    enemyDeathAni->retain();
    danmakuDeathAni = GetAnimation("ami0.png", 32, 32, 32, 0, 5);
    danmakuDeathAni->retain();
}

void Galaga::LoadAllGalagaResource(Node* node) {
    LoadAllGalagaEnemy(node);
    LoadPlayer(node);
    star = Sprite::create("star.png");
    node->addChild(star);
    starField->Init(20, 5);
    field->Init();/*
    stageBee->Init(Galaga::TestStageBee, field, units);
    stageGalaga->Init(Galaga::TestStageGalaga, field, units);
    fieldMove->Init(Galaga::MoveStage, field, units);
    StageDefalt->Init(Galaga::DefaltStage, field, units);*/
    SimpleAudioEngine::getInstance()->stopBackgroundMusic(1);
    SimpleAudioEngine::getInstance()->playBackgroundMusic("Sound/Theme.mp3", 1);
}


void GalagaResource::Galaga::LoadPlayer(Node* node)
{
    playerDeathAni = GetAnimation("playerdeath.png", 32, 32, 0, 0, 5);
    playerDeathAni->retain();
    player = Player::create(GetSprite("pl.png", 32, 32, 0, 0), node);
    player->Collided = CollidedPlayer;
    player->DeathRattle = PlayerDeathRattle;
    player->sprite->setPosition(WINDOWWIDTH / 2, WINDOWHEIGHT / 10);
    player->active = 0;
    units->AddUnit(player);
}

UINT Galaga::PlayerEffectSpell(IUnit* unit, int* tick) {
    if (unit->sprite->getActionManager()->getNumberOfRunningActionsInTarget(unit->sprite) == 0) {
        unit->AddType(PLAYER);
        //playerSpell use here
    }
    return 0;

}
UINT Galaga::AttackToPlayerSpell(IUnit* unit, int* tick) {
    if (!(*tick % 100)) {
        Bullet* copyed = galagaGame->danmaku->Copy();
        copyed->targetTo = new Vec2(galagaGame->player->sprite->getPosition());
        copyed->SetFrom(unit);
        copyed->sprite->setVisible(1);
        copyed->spell = Spell<Bullet*>::create(BSpellToPoint);

        galagaGame->units->AddUnit(copyed);
    }
    if (unit->sprite->getPosition().y < galagaGame->player->sprite->getPosition().y) {
        unit->spell = Spell<IUnit*>::create(EnemyDefaltSpell);
    }
    return 1;
}

void Galaga::PlayerAttack() {
    Bullet* copyed = plDanmaku->Copy();
    copyed->targetTo = new Vec2(player->sprite->getPosition().x, player->sprite->getPosition().y + 1000);
    copyed->spell = Spell<Bullet*>::create(BSpellToPoint);
    copyed->SetFrom(player);
    copyed->sprite->setVisible(1);

    units->AddUnit(copyed);
}

UINT Galaga::EnemyDefaltSpell(IUnit* unit, int* tick)
{
    if (!DeleteOutOfWindow(unit, tick)) {
        return 0;
    }
    return 1;
}

UINT Galaga::CollidedEnemy(IUnit* with)
{
    if (with->type == PLDANMAKU) {
        with->type = DELETE;
        return DELETE;
    }
}

UINT Galaga::CollidedPlayer(IUnit* with)
{
    if ((with->type == ENEMY) || (with->type == DANMAKU)) {
        with->type = DELETE;
        return DELETE;
    }
    return 1;
}

void Galaga::AddStar()
{
    std::uniform_int_distribution<int> range(0, WINDOWWIDTH);
    std::uniform_real_distribution<double> scale(1.0, 4.0);
    std::uniform_int_distribution<int> rgb(50, 200);
    starField->AddStar(star, Vec2(range(mt), WINDOWHEIGHT), scale(mt), Color3B(rgb(mt), rgb(mt), rgb(mt)));
}

void Galaga::SetPlayerMove(float f) {
    int ff = f * f / 2;
    if (ff > 10) {
        if (f > 0) {
            f = 10;
        }
        else {
            f = -10;
        }
    }
    if (player->activeMove) {
        player->AddAction(MoveBy::create(0, Vec2(f, 0)));
    }
}


Action* Galaga::GetStartUpAction(Vec2 space, int left, bool down, StartPos pos, int level, IUnit* unit) {
    Sequence* sequence;
    Vector<FiniteTimeAction*> action;
    Vec2 circleCenter = Vec2(WINDOWWIDTH / 4, WINDOWHEIGHT / 2);
    Vec2 circleEnd = Vec2(WINDOWWIDTH / 2, circleCenter.y);
    Vec2 ss;
    Vec2 start;
    Vec2 center;
    int loopR = WINDOWWIDTH / 4;
    loopR -= level * 20;
    switch (pos)
    {
    case StartPos::top:
        ss = Vec2(200 + level * 20, WINDOWHEIGHT);
        break;
    case StartPos::middle:
        ss = Vec2(0, WINDOWHEIGHT / 3 * 2 + (level * 20));
        break;
    case StartPos::bottom:
        ss = Vec2(0, WINDOWHEIGHT / 4 + (level * 20));
        break;
    default:
        break;
    }

    if (left) {
        start = circleCenter;
    }
    else {
        start = Vec2(WINDOWWIDTH - circleCenter.x, circleCenter.y);
        loopR = -loopR;
        ss.x = WINDOWWIDTH - ss.x;
    }
    center = start;
    if (down) {
        start.x -= loopR;
    }
    else {
        start.x += loopR;
    }
    float sr;
    sr = CC_DEGREES_TO_RADIANS(90); //skynet
    action.pushBack(RotateTo::create(0, RadToDeg(GetRadFromVec2(ss - unit->sprite->getPosition()))));
    action.pushBack(MoveTo::create(0, ss));
    action.pushBack(RotateTo::create(0, RadToDeg(GetRadFromVec2(start - ss))));
    action.pushBack(MoveTo::create(0.4, start));
    //action.pushBack(MoveCircleBy::create(1.3, center, sr, CC_DEGREES_TO_RADIANS(180), left));
    action.pushBack(MoveCircleBy::create(0.4, sr, CC_DEGREES_TO_RADIANS(90), loopR, left));
    action.pushBack(RotateTo::create(0, RadToDeg(GetRadFromVec2(space - start))));
    action.pushBack(MoveTo::create(0.4, space));
    action.pushBack(RotateTo::create(0, Vec3(0, 1, 0)));
    sequence = Sequence::create(action);
    return sequence;
}

Action* Galaga::GetDownFallAction(Vec2 target, Vec2 selfPos, IUnit* unit) {
    Sequence* sequence;
    Vector<FiniteTimeAction*> vec;
    Vec2 length = target - selfPos;
    Vec2 line1;
    Vec2 line2;

    line1 = Vec2(length.x, length.y / 4);
    line2 = Vec2(0, length.y / 4 * 3);
    vec.pushBack(RotateTo::create(0, RadToDeg(GetRadFromVec2(line1))));
    vec.pushBack(MoveBy::create(1, line1));
    vec.pushBack(ChangeSpell<IUnit*>::create(Spell<IUnit*>::create(AttackToPlayerSpell), unit));
    vec.pushBack(RotateTo::create(0, RadToDeg(GetRadFromVec2(line2))));
    vec.pushBack(MoveBy::create(2, line2));
    vec.pushBack(ChangeSpell<IUnit*>::create(Spell<IUnit*>::create(EnemyDefaltSpell), unit));
    vec.pushBack(MoveCircleBy::create(2, CC_DEGREES_TO_RADIANS(90), CC_DEGREES_TO_RADIANS(-360), 100, 0));
    vec.pushBack(MoveBy::create(1, Vec2(0, -300)));
    sequence = Sequence::create(vec);
    return sequence;
}
//
//UINT Galaga::EnemyFlyFSpell(IUnit* unit, int* tick)
//{
//    Vec2 pos = Vec2(0, 0);
//    if (field->GetEmptySpace(1, &pos)) {
//        unit->AddAction(GetStartUpAction(pos, 1, 1, StartPos::top, 1, unit));
//    }
//    return 1;
//}

UINT Galaga::DanmakuDeathRattle(IUnit* unit, Units* units) {
    if (unit->id != EFFECT) {
        unit->AddType(EFFECT);
        unit->spell = Spell<IUnit*>::create(EffectDefaltSpell);
        unit->Collided = NULL;
        unit->AddAction(Sequence::create(Animate::create(galagaGame->danmakuDeathAni), DelayTime::create(1), SetUnvisible::create(0), nullptr));
        return 1;
    }
    return DELETE;
}

unsigned int Galaga::EffectDefaltSpell(IUnit* unit, int* tick)
{
    if (!unit->sprite->isVisible()) {
        return DELETE;
    }
    return 1;
}

UINT Galaga::EnemyDeathRattleNoEffect(IUnit* unit, int tick) {
    return DELETE;
}

UINT Galaga::EnemyDeathRattle(IUnit* unit, Units* units) {
    if (unit->id != EFFECT) {
        unit->AddType(EFFECT);
        unit->spell = Spell<IUnit*>::create(EffectDefaltSpell);
        unit->Collided = NULL;
        DeleteAllAction(unit);
        unit->AddAction(Sequence::create(Animate::create(galagaGame->enemyDeathAni), SetUnvisible::create(0), nullptr));
        unit->AddAction(Sequence::create(Animate::create(galagaGame->enemyDeathAni), SetUnvisible::create(0), nullptr));
        galagaGame->dui->UpdatePoint(150, unit->sprite->getPosition());
        SimpleAudioEngine::getInstance()->playEffect("Sound/Kill.mp3");
        return 1;
    }
    return DELETE;
}

UINT Galaga::BossDeathRattle(IUnit* unit, Units* units) {
    IUnit* copyed = units->AddCopyUnit(galagaGame->boss2);
    copyed->active = 1;
    copyed->sprite->setPosition(unit->sprite->getPosition());
    return DELETE;
}

UINT Galaga::BossDeathRattle2(IUnit* unit, Units* units) {
    if (unit->id != EFFECT) {
        unit->AddType(EFFECT);
        unit->spell = Spell<IUnit*>::create(EffectDefaltSpell);
        unit->Collided = NULL;
        DeleteAllAction(unit);
        unit->AddAction(Sequence::create(Animate::create(galagaGame->enemyDeathAni), SetUnvisible::create(0), nullptr));
        galagaGame->dui->UpdatePoint(400, unit->sprite->getPosition());
        SimpleAudioEngine::getInstance()->playEffect("Sound/Kill.mp3");
        return 1;
    }
    return DELETE;
}

Animate* Galaga::GetPlayerDeathEffect(IUnit* unit, Units* units) {
    if (unit->id != EFFECT) {
        unit->Collided = NULL;
        DeleteAllAction(unit);
        Animate* ani = Animate::create(galagaGame->playerDeathAni);
        SimpleAudioEngine::getInstance()->playEffect("Sound/Fly.mp3");
        return ani;
    }
    return 0;
}

UINT Galaga::PlayerDeathMoveFSpell(IUnit* unit, int* tick)
{
    if (unit->sprite->getActionManager()->getNumberOfRunningActionsInTarget(unit->sprite) == 0) {
        galagaGame->player->activeMove = 1;
        return 1;
    }
    return 0;
}

UINT Galaga::PlayerDeathRattle(IUnit* unit, Units* units) {

    if (galagaGame->dui->hp - 1 != -1)
    {
        galagaGame->dui->hp--;
        galagaGame->dui->UpdateLife();
        Animate* pd = GetPlayerDeathEffect(unit, units);
        Sequence* PlayerSeqA = Sequence::create(
            pd,
            MoveTo::create(0, Vec2(WINDOWWIDTH / 2, 0)),
            MoveBy::create(3.0f, Vec2(0, 200)),
            DelayTime::create(3.5f),
            ChangeCollided<IUnit*>::create(CollidedPlayer, galagaGame->player),
            nullptr
        );
        units->units[0]->AddAction(PlayerSeqA);
        galagaGame->player->activeMove = 0;
        // Ready Action
        //collided action�̵� �֤� �ä���
        galagaGame->player->AddFinalSpell(FinalSpell<IUnit*>::create(PlayerDeathMoveFSpell, galagaGame->player));
    }
    else
    {
        galagaGame->dui->gameOverLabel->setVisible(1);
        Blink* blink = Blink::create(3.5f, 6);
        blink->reverse();
        SetUnvisible* unvisible = SetUnvisible::create(0);
        Sequence* gameover = Sequence::create(blink, unvisible, nullptr);
        galagaGame->dui->gameOverLabel->runAction(gameover);
        if (unit->id != EFFECT) {
            galagaGame->player->AddType(EFFECT);
            galagaGame->player->activeMove = 0;
            galagaGame->player->spell = Spell<IUnit*>::create(EffectDefaltSpell);
            unit->Collided = NULL;
            DeleteAllAction(unit);
            Animate* ani = Animate::create(galagaGame->playerDeathAni);
            galagaGame->player->AddAction(ani);
            SimpleAudioEngine::getInstance()->playEffect("Sound/Fly.mp3");
            return 1;
        }
        return DELETE;
    }

    return 1;
}

int Galaga::TestStageBee(int* tick, Units* stage, Field* field)
{
    if (*tick > 20 * 9 + 10) {
        return 0;
    }
    if (!(*tick % 20)) {
        IUnit* unit;
        unit = stage->AddCopyUnit(galagaGame->bee);
        Vec2 pos = Vec2(0, 0);
        if (field->GetEmptySpace(1, &pos)) {
            unit->AddAction(GetStartUpAction(pos, 1, 1, StartPos::top, 1, unit));
            unit->active = 1;
        }
    }
    if (!((*tick - 10) % 20)) {
        IUnit* unit;
        unit = stage->AddCopyUnit(galagaGame->bee);
        Vec2 pos = Vec2(0, 0);
        if (field->GetEmptySpace(0, &pos)) {
            unit->AddAction(GetStartUpAction(pos, 1, 1, StartPos::top, 1, unit));
            unit->active = 1;
        }
    }
    *tick += 1;
    return 1;
}

int Galaga::TestStageGalaga(int* tick, Units* stage, Field* field)
{
    if (*tick > 20 * 9 + 10 + 20 * 8 + 80) {
        return 0;
    }
    if (*tick > 20 * 9 + 10) {
        if (!(*tick % 20)) {
            Vec2 pos = Vec2(0, 0);
            if (field->GetEmptySpace(3, &pos)) {
                IUnit* unit;
                unit = stage->AddCopyUnit(galagaGame->galaga);
                unit->AddAction(GetStartUpAction(pos, 0, 1, StartPos::top, 1, unit));
                unit->active = 1;
            }
        }
    }
    if (!(*tick % 20)) {
        Vec2 pos = Vec2(0, 0);
        if (field->GetEmptySpace(2, &pos)) {
            IUnit* unit;
            unit = stage->AddCopyUnit(galagaGame->galaga);
            unit->AddAction(GetStartUpAction(pos, 0, 1, StartPos::top, 1, unit));
            unit->active = 1;
        }

    }
    if (!((*tick - 10) % 20)) {
        Vec2 pos = Vec2(0, 0);
        if (field->GetEmptySpace(4, &pos)) {
            IUnit* unit;
            unit = stage->AddCopyUnit(galagaGame->boss1);
            unit->AddAction(GetStartUpAction(pos, 0, 1, StartPos::top, 1, unit));
            unit->active = 1;
        }
    }

    *tick += 1;
    return 1;
}

int Galaga::MoveStage(int* tick, Units* stage, Field* field)
{
    if (*tick < 150 * 2) {
        if (!(*tick % 200)) {
            stage->AddActionByType(ENEMY, MoveBy::create(0, Vec2(-30, 0)));
        }
    }
    else if (*tick < 150 * 4) {
        if (!(*tick % 150)) {
            stage->AddActionByType(ENEMY, MoveBy::create(0, Vec2(30, 0)));
        }
    }
    else if (*tick < 150 * 6) {
        if (!(*tick % 150)) {
            stage->AddActionByType(ENEMY, MoveBy::create(0, Vec2(-30, 0)));
        }
    }
    else if (*tick < 150 * 8) {
        if (!(*tick % 150)) {
            stage->AddActionByType(ENEMY, RunAway::create(0, field->center, 20, 0, 0));
        }
    }
    else if (*tick < 150 * 10) {
        if (!(*tick % 150)) {
            stage->AddActionByType(ENEMY, RunAway::create(0, field->center, 20, 0, 1));
        }
    }
    else if (*tick < 150 * 14) {
        return 0;
    }
    *tick += 1;
    return 1;
}

int Galaga::DefaltStage(int* tick, Units* stage, Field* field) {
    if (!(*tick % 300)) {
        IUnit* unit = stage->GetRandomUnitByType(ENEMY);
        unit->AddAction(GetDownFallAction(
            galagaGame->player->sprite->getPosition(),
            unit->sprite->getPosition(),
            unit
        ));
    }
    if (galagaGame->player->activeMove != 0) {
        *tick += 1;
    }
    return 1;
}

void Galaga::Start() {
    player->active = 1;
    player->sprite->setVisible(1);
    stageIsStarted = 1;
}

void Galaga::UpdateGalaga()
{
    units->Update();
    units->CollitionSearch();
    starField->Update();
    if (starCoolTime.Update()) {
        AddStar();
    }
    if (!stageIsStarted) {
        return;
    }
    int stageEnd = 0;
    stageEnd = stageBee->Update();
    if (!stageEnd) {
        stageEnd = stageGalaga->Update();
    }
    if (!stageEnd) {
        stageEnd = fieldMove->Update();
    }
    if (!stageEnd) {
        StageDefalt->Update();
    }/*
    if (starCoolTIme->Update()) {
        std::uniform_int_distribution<int> range(0, WINDOWWIDTH);
        std::uniform_real_distribution<double> scale(1.0, 4.0);
        std::uniform_int_distribution<int> rgb(50, 200);
        starField->AddStar(star, Vec2(range(mt), WINDOWHEIGHT), scale(mt), Color3B(rgb(mt), rgb(mt), rgb(mt)));
        starField->Update();
    }*/
}