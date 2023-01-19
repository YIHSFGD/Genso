#pragma once
#include"GameObject.h"

UINT IUnit::Update() {
    if (active) {
        UpdateFSpell();
        if (!UpdateSpell()) {
            type = DELETE;
        }
        if (type == DELETE) {
            return type;
        }
    }
    return 1;
}

void IUnit::init(Sprite* sp, Node* node) {
    sprite = sp;
    node->addChild(sprite);
    sprite->setVisible(false);
    fSpell = new FinalSpell<IUnit*>*[7];
    this->size = sprite->getContentSize();
}

void IUnit::init(std::string fileName, Node* node) {
    sprite = Sprite::create(fileName);
    init(sprite, node);
}

IUnit* IUnit::Copy() {
    IUnit* copyed = new IUnit;
    Sprite* sp = Sprite::createWithSpriteFrame(sprite->getSpriteFrame());
    copyed->init(sp, sprite->getParent());
    copyed->AddType(id);
    copyed->hp = hp;
    copyed->Collided = Collided;
    copyed->DeathRattle = DeathRattle;
    if (spell) {
        copyed->spell = spell->Clone();
    }
    copyed->size = size;
    copyed->active = 1;
    /*copyed->fSpell = fSpell;
    copyed->fSpellIndex = fSpellIndex;*/
    copyed->sprite->setRotation(sprite->getRotation());
    copyed->sprite->setVisible(1);
    if (animation) {
        copyed->animation =  this->animation;
    }
    return copyed;
}

void Units::Init(int unitMax, int resourceMax) {
    units = new IUnit * [unitMax];
    maxIndex = unitMax;
    resources = new IUnit * [resourceMax];
    resourceMaxIndex = resourceMax;
}

void Units::Update()
{
    UINT result;
    for (int i = 0; i < unitIndex; i++) {
        result = units[i]->Update();
        if (result == DELETE){
            if (units[i]->DeathRattle != nullptr) {
                
                if (!(units[i]->DeathRattle(units[i], this))) {
                    DeleteUnit(i);
                }
                else {
                    units[i]->type = units[i]->id;
                }
            }
            else {
                DeleteUnit(i);
            }
        }
    }
}

UINT Units::AddUnit(IUnit* unit) {
    UINT result = S_OK;
    units[unitIndex] = unit;
    unitIndex++;
    if (maxIndex < unitIndex) {
        maxIndex += 10;
        IUnit** newUnits = new IUnit * [maxIndex+10];
        for (int i = 0; i < unitIndex; i++) {
            newUnits[i] = units[i];
        }
        units = newUnits;
    }
    return result;
}
UINT Units::CollitionSearch() {
    UINT result = 1;
    UINT returnVal = 1;
    for (int i = 0; i < unitIndex; i++) {
        for (int j = i + 1; j < unitIndex; j++) {
            if (GetHitboxRect(units[i], NULL, debug).intersectsRect(GetHitboxRect(units[j], NULL, debug))) {
                if (units[i]->Collided != nullptr){
                    CCLOG("Collied Object1 : %i with %i", units[i]->type, units[j]->type);
                    CCLOG("Collied Object2 : %i with %i", units[i]->id, units[j]->id);
                    returnVal = units[i]->Collided(units[j]);
                    if (returnVal == DELETE) {
                        units[i]->type = DELETE;
                    }
                }
                else if (units[j]->Collided != nullptr) {
                    returnVal = units[j]->Collided(units[i]);
                    if (returnVal == DELETE) {
                        units[j]->type = DELETE;
                    }
                }
            }
        }
    }
    return result;
}


void Units::DeleteUnit(int index)
{
    if (index == 0) {
        return;
    }
    delete units[index];
    for (int i = index + 1; i < unitIndex; i++) {
        units[i]->id -= 1;
        units[i - 1] = units[i];
    }
    unitIndex--;
}

UINT Units::AddResource(IUnit* resource)
{
    UINT result = S_OK;
    resources[resourceIndex] = resource;
    resourceIndex++;
    if (resourceMaxIndex < resourceIndex) {
        resourceMaxIndex += 10;
        IUnit** newUnits = new IUnit * [resourceMaxIndex];
        for (int i = 0; i < resourceIndex; i++) {
            newUnits[i] = units[i];
        }
        resources = newUnits;
    }
    return result;
}


void Field::Init()
{
    float widthMinus = WINDOWWIDTH / 5;
    float lineY = WINDOWHEIGHT / 2;
    float weight = WINDOWHEIGHT / 20;
    lines = new Line[5];
    for (int i = 0; i < 5; i++) {
        lines[i].pos = Vec2(widthMinus,lineY + weight*i);
        lines[i].width = WINDOWWIDTH - widthMinus;
    }
    center = Vec2(WINDOWWIDTH/2, (lines[4].pos.y - lines[0].pos.y) / 2 + lines[0].pos.y);
    lines[4].Init(4, lines[4].pos, lines[4].width);
    lines[3].Init(8, lines[3].pos, lines[3].width);
    lines[2].Init(8, lines[2].pos, lines[2].width);
    lines[1].Init(10, lines[1].pos, lines[1].width);
    lines[0].Init(10, lines[0].pos, lines[0].width);
}

bool Field::GetEmptySpace(int line, __out Vec2* val)
{
    if (!(lines[line].GetPos(val))) { 
        return 0;
    }
    return 1;
}
