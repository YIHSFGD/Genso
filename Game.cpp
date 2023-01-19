#include"Game.h"

namespace Resource {
    std::mt19937 mt = std::mt19937((unsigned int)time(NULL));
    Player* player;
    Units* units;
    int dSpriteCount = 0;
    ActionDone* actiondone = new ActionDone;
    Bullet* amiBullet1;
    Enemy* yousei;
    IUnit* AmiBarrierUnit;
    int shotSpeed1 = 5;
    Vec2 mousePos;

    //AMI//
    Bullet* fairyBullet;
    AmiUI* aui;
    Animation* playerDefaltAnime;
    Animation* playerAttackAnime;
    Animation** fairyAnimation;
    Animation* AmiBarrier;
}

using namespace Resource;
using namespace AmiResource;

void Resource::Init(Node* node, Units* units_, Player* pl, AmiUI* _aui) {
    player = pl;
    units = units_;
    aui = _aui;
    player->active = false;
}

void Resource::LoadDefaltSound() {
    SimpleAudioEngine::getInstance()->preloadBackgroundMusic("Sound/title.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("cancle00.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("extend.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("select00.mp3");
}

void Resource::LoadAmiSound() {
    SimpleAudioEngine::getInstance()->preloadBackgroundMusic("ami/aishitagensokyo.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("amishot.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("ami/ok00.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("ami/tan00.mp3");
}

void Resource::LoadGalagaSound(Node* node) {

}

void Resource::LoadAllAmiResource(Node* node) {
    LoadYousei(node);
    LoadBarrier(node);
    amiBullet1 = Bullet::create(GetSprite("ami0.png", 192, 192, 0, 0), node);
}

void Resource::LoadYousei(Node* node) {
    yousei = Enemy::create(GetSprite("fairy.png", 960 / 5, 768 / 4, 0, 0), node);
    yousei->spell = Spell<IUnit*>::create(SpellFairy);
    yousei->sprite->setScale(0.25);
    fairyAnimation = new Animation * [3];
    fairyAnimation[0] = GetAnimation("fairy.png", 960 / 5, 768 / 4, 0, 0, 4);
    fairyAnimation[1] = GetAnimation("fairy.png", 960 / 5, 768 / 4, 0, 768/4, 4);
    fairyAnimation[2] = GetAnimation("fairy.png", 960 / 5, 768 / 4, 0, 768 / 4*2, 5, 0.2f, 8);
    yousei->animation = fairyAnimation[0];
    for (int i = 0; i < 3; i++) {
        fairyAnimation[i]->retain();
    }
    fairyBullet = Bullet::create(GetSprite("fairy0.png", 120, 120, 0, 0), node);
}

void Resource::LoadBarrier(Node* node)
{
    AmiBarrier = GetAnimation("ami_barrier.png", 960 / 5, 768 / 4, 0, 0, 5, 0.2f, 20);
    AmiBarrierUnit = IUnit::create(GetSprite("ami_barrier.png", 960/5, 768/4, 0, 0), node);
    AmiBarrierUnit->AddAnime(AmiBarrier);
    AmiBarrierUnit->AddType(EFFECT);
    AmiBarrierUnit->sprite->setOpacity(255 / 2);
    AmiBarrierUnit->active = 0;
}

void Resource::Release() {
    delete amiBullet1;
    //
}

Player* Resource::GetPlayer()
{
    return player;
}

void Resource::UpdateMousePos(Vec2 pos)
{
    mousePos = pos;
}

UINT Resource::DeleteOutOfWindow(IUnit* unit, int* tick)
{
    if (unit->sprite->getPosition().y < 0) {
        return DELETE;
    }
    return 1;
}

UINT Resource::SpellDeleteActionEnd(IUnit* unit, int* tick)
{
    if (unit->sprite->getActionManager()->getNumberOfRunningActionsInTarget(unit->sprite) == 0) {
        return 0;
    }
    return 1;
}

void Resource::DeleteAllAction(IUnit* unit)
{
    unit->sprite->getActionManager()->removeAllActionsFromTarget(unit->sprite);
}

void Resource::ShotToPoint(Bullet* unit) {
    unit->AddAction(MoveBy::create(0, unit->to*unit->speed));
}

UINT Resource::BSpellToPoint(Bullet* unit, int* tick) {
    if (unit->target) {
        unit->to = (unit->target->sprite->getPosition() - unit->sprite->getPosition()).getNormalized() * shotSpeed1;
        if (unit->targetSetOnce) {
            unit->target = NULL;
        }
    }
    else if (unit->targetTo) {
        unit->to = (*unit->targetTo - unit->sprite->getPosition()).getNormalized();
        delete unit->targetTo;
        unit->targetTo = NULL;
    }
    else if ((unit->targetBy.x != 0)||(unit->targetBy.y != 0)) {
        unit->to = (unit->targetBy ).getNormalized();
    }
    ShotToPoint(unit);
    return 1;
}

//AmiResource
UINT AmiResource::HpCheck(IUnit* unit) {
    if (unit->hp <= 0) {
        return DELETE;
    }
}

UINT AmiResource::DeleteAnimationEndFSpell(IUnit* unit, int* tick)
{
    if (unit->sprite->getActionManager()->getNumberOfRunningActionsInTarget(unit->sprite) == 0) {
        unit->type = DELETE;
    }
    return 0;
}

UINT AmiResource::SpellDoriDori(IUnit* unit, int tick) {
    if (!(tick % 200)) {
        if (unit->sprite->isFlippedX()) {
            unit->sprite->setFlippedX(0);
        }
        else {
            unit->sprite->setFlippedX(1);
        }
    }
    return 1;
}

UINT AmiResource::SpellFairy(IUnit* unit, int* tick) {
    SpellDoriDori(unit, *tick);
    DefaltAnimationSpell(unit, *tick);
    Rect see = Rect(0, 0, 700, unit->sprite->getBoundingBox().size.height);
    if (!unit->sprite->isFlippedX()) {
        see.size.width = -see.size.width;
    }
    see.origin = Vec2(unit->sprite->getPosition().x, unit->sprite->getPosition().y + see.size.height/2);
    if (player->sprite->getBoundingBox().intersectsRect(see)) {
        unit->AddFinalSpell(FinalSpell<IUnit*>::create(FairyAttackMotion, unit));
        unit->AddFinalSpell(FinalSpell<IUnit*>::create(FairyShot, unit));
    }
    return 1;
}

UINT AmiResource::YouseiAttack() {
    Vec2 target = player->sprite->getPosition();
    return 1;
}

void AmiResource::ActivePlayer(Vec2 pos, Animation* playerDefalt, Animation* playerAttack, Player* player)
{
    playerDefaltAnime = playerDefalt;
    playerDefaltAnime->retain();
    playerAttackAnime = playerAttack;
    playerAttack->retain();
   // player->sprite->setPosition(pos);
    player->sprite->setOpacity(0);
    player->sprite->setVisible(1);
    //player->spell = PlayerSpell;
    player->AddAction(FadeIn::create(1));
    player->active = true;
}

UINT AmiResource::PlayerAttackMotion(IUnit* unit, int* tick)
{
    player->sprite->getActionManager()->removeAllActionsFromTarget(player->sprite);
    player->AddAction(Animate::create(playerAttackAnime));
    SimpleAudioEngine::getInstance()->playEffect("ami/amishot.mp3");
    return 1;
}

UINT AmiResource::FairyAttackMotion(IUnit* unit, int* tick) {
    unit->sprite->getActionManager()->removeAllActionsFromTarget(unit->sprite);//
    return 1;
}

UINT AmiResource::PlayerShot(IUnit* unit, int* tick) {
    AmiShot1(mousePos, unit);
    return 1;
}

UINT AmiResource::FairyShot(IUnit* unit, int* tick) {
    if (unit->sprite->isFlippedX()) {
        ShotSimple(fairyBullet, Vec2(unit->sprite->getPosition().x+100, unit->sprite->getPosition().y), unit, BSpellToPoint);
    }
    else {
        ShotSimple(fairyBullet, Vec2(unit->sprite->getPosition().x - 100, unit->sprite->getPosition().y), unit, BSpellToPoint);
    }
    SimpleAudioEngine::getInstance()->playEffect("tan00.mp3");
    return 1;
}

UINT AmiResource::Defence(IUnit* unit, int* tick)
{
    if (*tick == 0) {
        unit->sprite->setOpacity(255/2);
        OnBarrir(unit, AmiBarrierUnit, 3);
    }
    if (*tick < 300) {
        return 0;
    }
    unit->sprite->setOpacity(255);
    return 1;
}

void AmiResource::PlayerAttackToMousePos()
{
    if (mousePos.x > player->sprite->getPosition().x) {
        player->sprite->setFlippedX(1);
    }
    else {
        player->sprite->setFlippedX(0);
    }
    player->AddFinalSpell(FinalSpell<IUnit*>::create(PlayerAttackMotion, player));
    player->AddFinalSpell(FinalSpell<IUnit*>::create(PlayerShot, player));
}

void AmiResource::YouseiAttackToPlayer(Enemy* unit) {
    unit->AddFinalSpell(FinalSpell<IUnit*>::create(FairyShot, unit));
}

void AmiResource::PlayerDefence() {
    player->AddFinalSpell(FinalSpell<IUnit*>::create(Defence, player));
}

void AmiResource::SpawnEnemy(Vec2 pos) {
    IUnit* copyed = yousei->Copy();
    copyed->sprite->setPosition(pos);
    copyed->sprite->setVisible(1);
    units->AddUnit(copyed);
}

void AmiResource::ShotSimple(Bullet* bullet, IUnit* target, IUnit* from, UINT(*BSpell)(Bullet* unit, int* tick)) {
    Bullet* copyed = bullet->Copy();
    copyed->target = target;
    copyed->spell = Spell<Bullet*>::create(BSpell);
    copyed->SetFrom(from);
    copyed->sprite->setVisible(1);
    units->AddUnit(copyed);
    SimpleAudioEngine::getInstance()->playEffect("Sound/Shoot.mp3");
}

void AmiResource::ShotSimple(Bullet* bullet, Vec2 target, IUnit* from, UINT(*BSpell)(Bullet* unit, int* tick)) {
    Bullet* copyed = bullet->Copy();
    copyed->targetTo = new Vec2(target);
    copyed->spell = Spell<Bullet*>::create(BSpell);
    copyed->SetFrom(from);
    copyed->sprite->setVisible(1);
    units->AddUnit(copyed);
    SimpleAudioEngine::getInstance()->playEffect("Sound/Shoot.mp3");
}


void AmiResource::AmiShot1(IUnit* target, IUnit* from)
{
    Bullet* copyed = amiBullet1->Copy();
    copyed->target = target;
    copyed->spell = Spell<Bullet*>::create(BSpellToPoint);
    copyed->SetFrom(from);
    copyed->sprite->setVisible(1);
    units->AddUnit(copyed);
    SimpleAudioEngine::getInstance()->playEffect("Sound/Shoot.mp3");
}

void AmiResource::AmiShot1(Vec2 target, IUnit* from) {
    Bullet* copyed = amiBullet1->Copy();
    copyed->targetTo = new Vec2(target);
    copyed->spell = Spell<Bullet*>::create(BSpellToPoint);
    copyed->SetFrom(from);
    copyed->sprite->setVisible(1);
    units->AddUnit(copyed);
    SimpleAudioEngine::getInstance()->playEffect("Sound/Shoot.mp3");
}

void AmiResource::OnBarrir(IUnit* unit, IUnit* barrier, float time) {
    if (time != 0) {
        barrier->animation->setDelayPerUnit(time / barrier->animation->getFrames().size());
    }
    IUnit* copyed = AmiBarrierUnit->Copy();
   
    DeleteAllAction(copyed);
    copyed->active = 1;
    copyed->sprite->setVisible(1);
    copyed->ChangeParent(unit->sprite);
    copyed->sprite->setPosition(unit->sprite->getContentSize()/2);
    unit->AddFinalSpell(FinalSpell<IUnit*>::create(DefaltAnimationFSpell, copyed));
    unit->AddFinalSpell(FinalSpell<IUnit*>::create(DeleteAnimationEndFSpell, copyed));
}

void AmiResource::MovePlayer(Vec2 rot) {
    player->sprite->setPosition((0, player->sprite->getPosition() + rot * aui->playerSpeed));
}

// SPELL //
UINT AmiResource::PlayerSpell(IUnit* unit, int* tick) {
    UINT uir = 1;
    uir = HpCheck(unit);
    if (uir == DELETE) {
        return uir;
    }

    if (player->sprite->getActionManager()->getNumberOfRunningActionsInTarget(unit->sprite) == 0) {
        player->AddAction(Animate::create(playerDefaltAnime));
    }
    return uir;
}

UINT AmiResource::DanmakuDefaltSpell(IUnit* unit, int tick) {
    Vec2 pos = unit->sprite->getPosition();
    if ((pos.y < 0) || (pos.y > WINDOWHEIGHT)) {
        return DELETE;
    }
    return 1;
}

UINT Resource::DefaltAnimationSpell(IUnit* unit, int tick)
{
    if (unit->animation) {
        if (unit->sprite->getActionManager()->getNumberOfRunningActionsInTarget(unit->sprite) == 0) {
            unit->AddAction(Animate::create(unit->animation));
        }
    }
    return 1;
}

UINT AmiResource::DefaltAnimationFSpell(IUnit* unit, int* tick)
{
    DefaltAnimationSpell(unit, *tick);
    return 1;
}