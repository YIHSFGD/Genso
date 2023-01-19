#pragma once
#include "CustomAction.h"
#include "cocos/editor-support/cocostudio/SimpleAudioEngine.h"
#include "types.h"
#include "CCGL-win32.h"

#define DELETE 0
#define PLAYER 0x00000001
#define ENEMY 0x00000002
#define DANMAKU 0x00000003
#define PLDANMAKU 0x00000004
#define EFFECT 0x00000005
#define BOSS 0x00000006
#define WINDOWWIDTH 1920
#define WINDOWHEIGHT 1080
using namespace cocos2d;
using namespace CocosDenshion;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
typedef unsigned int UINT;
#define S_OK 0L
#define INFINITE 0xffffffff
#endif
class Units;
class IUnit;
class Field;
static Label* CreateLabel(std::string str, std::string font = "fonts/Font.ttf")
{
	std::string Filepath = font;
	TTFConfig tc;
	Label* newLabel = Label::createWithTTF(str, Filepath, 20);
	return newLabel;
}
static Rect GetHitboxRect(Sprite* sp, Node* convert = NULL, DrawNode* debug = NULL) {

	Rect r = sp->getBoundingBox();
	if (convert) {
		r.origin += convert->getPosition();
	}
	if (debug) {
		debug->drawRect(r.origin, r.origin + r.size, Color4F::RED);
	}
	return r;
}

class UnitBody : public PhysicsBody {
public:
	IUnit* unit;
	static UnitBody* createCircle(float radius, const PhysicsMaterial& material = PHYSICSBODY_MATERIAL_DEFAULT, const Vec2& offset = Vec2::ZERO)
	{
		UnitBody* body = new (std::nothrow) UnitBody();
		if (body && body->init())
		{
			body->addShape(PhysicsShapeCircle::create(radius, material, offset));
			body->autorelease();
			return body;
		}

		CC_SAFE_DELETE(body);
		return nullptr;
	}

	static UnitBody* createBox(const Size& size, const PhysicsMaterial& material = PHYSICSBODY_MATERIAL_DEFAULT, const Vec2& offset = Vec2::ZERO) {
		{
			UnitBody* body = new (std::nothrow) UnitBody();
			if (body && body->init())
			{
				body->addShape(PhysicsShapeBox::create(size, material, offset));
				body->autorelease();
				return body;
			}

			CC_SAFE_DELETE(body);
			return nullptr;
		}
	}

	static UnitBody* createEdgeBox(const Size& size, const PhysicsMaterial& material = PHYSICSBODY_MATERIAL_DEFAULT, float border = 1, const Vec2& offset = Vec2::ZERO)
	{
		UnitBody* body = new (std::nothrow) UnitBody();
		if (body && body->init())
		{
			body->addShape(PhysicsShapeEdgeBox::create(size, material, border, offset));
			body->setDynamic(false);
			body->autorelease();
			return body;
		}

		CC_SAFE_DELETE(body);

		return nullptr;
	}

	UnitBody* copy() {
		
	}
};

template<class T>
class Spell {
protected:
	UINT(*_spell)(T unit, int* tick);
public:
	int* tick;
	static Spell* create(UINT(*spell)(T unit, int* tick)) {
		Spell* ss = new Spell;
		ss->Init(spell);
		return ss;
	}

	void virtual Init(UINT(*spell)(T unit, int* tick)) {
		_spell = spell;
		tick = new int(0);
	}

	virtual UINT Update(T unit) {
		if (_spell != nullptr) {
			*tick += 1;
			return _spell(unit, tick);
		}
		return -1;
	}

	virtual Spell<T>* Clone() {
		return Spell::create(_spell);
	}
};

class Cooltime {
public:
	UINT32 max = 0;
	UINT32 now = 0;

	Cooltime(){ }

	Cooltime(UINT32 Max) {
		max = Max;
	}

	int Update(bool unActive = false) {
		if (now == 0) {
			if (unActive) {
				return 1;
			}
			now = max;
			return 1;
		}
		else {
			if (now > 0) {
				now--;
			}
		}
		return 0;
	}
};

template<class T>
class CoolTimeSpell : public Spell<T> {
private:
	Cooltime ct;
public:
	static CoolTimeSpell* create(UINT(*spell)(T unit, int* tick), int cooltime) {
		CoolTimeSpell* ss = new CoolTimeSpell;
		ss->Init(spell, cooltime);
		return ss;
	}

	virtual void Init(UINT(*spell)(T unit, int* tick), int cti) {
		_spell = spell;
		tick = new int(0);
		ct = Cooltime(cti);
	}

	virtual UINT Update(IUnit* unit) override {
		if (_spell != nullptr) {
			if (ct.Update()) {
				*tick += 1;
				return _spell(unit, tick);
			}
		}
		return -1;
	}

	virtual CoolTimeSpell<T>* Clone() {
		return CoolTimeSpell::create(_spell, ct.max);
	}
};

template<class T>
class FinalSpell {
private:
	UINT(*_spell)(T unit, int* tick);
public:
	int* tick;
	int isComplete;
	T _unit;

	static FinalSpell* create(UINT(*spell)(T unit, int* tick), T unit) {
		FinalSpell* fs = new FinalSpell;
		fs->Init(spell, unit);
		return fs;
	}

	void Init(UINT(*spell)(T unit, int* tick), T unit) {
		_spell = spell;
		tick = new int(0);
		_unit = unit;
		isComplete = 0;
	}

	int Update() {
		if (_spell != nullptr) {
			isComplete = _spell(_unit, tick);
		}
		return isComplete;
	}
};

class IUnit {
private:
	int fSpellIndex = 0;
public:
	IUnit() {}
	UINT type = DELETE;
	UINT id = 0;
	int hp = 1;
	Sprite* sprite = NULL;
	Size size;
	Animation* animation = NULL;
	Spell<IUnit*>* spell = NULL;
	bool active = 1;
	virtual UINT UpdateSpell() {
		UINT ur = 1;
		if (spell != nullptr) {
			ur = spell->Update(this);
			if (ur == DELETE) {
				type = ur;
			}
		}
		return ur;
	}
	virtual UINT Update();
	UINT(*Collided)(IUnit* with) = NULL;
	FinalSpell<IUnit*>** fSpell = NULL;
	static IUnit* create(Sprite* sp, Node* node) {
		IUnit* unit = new IUnit;
		unit->init(sp, node);
		return unit;
	}
	virtual void init(Sprite* sp, Node* node);
	void init(std::string fileName, Node* node);
	void init(Sprite* sp, Node* node, Animation* anime) {
		sp->runAction(RepeatForever::create(Animate::create(anime)));
	}
	void SetBody(UnitBody* body = NULL, bool dynamic = false, int bitmask = 1, bool TestBitmask = true) {		
		if (body == NULL) {
			body = UnitBody::createBox(sprite->getContentSize());
		}
		body->setDynamic(dynamic);
		sprite->addComponent(body);
		body->setCollisionBitmask(bitmask);
		body->setContactTestBitmask(TestBitmask);
		body->unit = this;
	}
	virtual UINT UpdateFSpell() {
		UINT ur = 0;
		if (fSpell) {
			if (fSpellIndex > 0) {
				ur = fSpell[0]->Update();
				if (ur) {
					for (int i = 0; i < fSpellIndex; i++) {
						fSpell[i] = fSpell[i+1];
						fSpellIndex--;
					}
				}
				return ur;
			}
			return -1;
		}
		return -2;
	}
	void AddFinalSpell(FinalSpell<IUnit*>* spell) {
		if (fSpellIndex == 5) {
			return;
		}
		if (spell == NULL) {
			return;
		}
		fSpell[fSpellIndex] = spell;
		fSpellIndex++;
	}
	void AddType(UINT Type) {
		id = Type;
		type = Type;
	}
	void AddAction(Action* a) {
		if (sprite == NULL) {
			return;
		}
		sprite->runAction(a);
	}
	void AddAnime(Animation* animate) {
		if (animation != nullptr) {
			return;
		}
		CCASSERT(animate, "animate cannnot be null");
		animation = animate;
		animation->retain();
	}
	void ChangeParent(Node* node) {
		if (sprite->getParent()) {
			sprite->getParent()->removeChild(sprite);
		}
		node->addChild(sprite);
	}
	virtual IUnit* Copy();
	UINT(*DeathRattle)(IUnit* unit, Units* units) = NULL;
	~IUnit()
	{
		if (spell) {
			delete spell;
		}
		/*if (fSpell) {
			delete[] fSpell;
		}*/
		if (sprite) {
			sprite->onExit();
			/*
			if (sprite->getPhysicsBody()) {
				sprite->removeAllComponents();
			}*/
			if (sprite->getParent()) {
				sprite->getParent()->removeChild(sprite);
			}
			else {
				sprite->release();
			}
		}/*
		if (animation) {
			animation->release(); // change animate
		}*/
	}
};

class Enemy : public IUnit {
public:
	//Spell<Enemy*>* spell;
	static Enemy* create(Sprite* sp, Node* node, Animation* defaltAnimation = NULL) {
		Enemy* enemy = new Enemy;
		enemy->init(sp, node, defaltAnimation);
		return enemy;
	}
	virtual void init(Sprite* sp, Node* node, Animation* defaltAnimation = NULL) {
		AddType(ENEMY);
		animation = defaltAnimation;
		IUnit::init(sp, node);
	}
	/*
	virtual UINT UpdateSpell() override {
		UINT ur = 1;
		if (spell != nullptr) {
			ur = spell->Update(this);
			if (ur == DELETE) {
				type = ur;
			}
		}
		return ur;
	}*/
};

class Player : public IUnit {
public:
	bool activeMove = 1;
	static Player* create(Sprite* sp, Node* node) {
		Player* player = new Player;
		player->init(sp, node);
		return player;
	}
	virtual void init(Sprite* sp, Node* node) {
		AddType(PLAYER);
		IUnit::init(sp, node);
	}
	Animate* defalt;
};

class Bullet : public IUnit {
private:
	IUnit* from;
public:
	int speed = 1;
	IUnit* target = NULL;
	bool targetSetOnce = true;
	Vec2* targetTo = NULL;
	Vec2 targetBy = Vec2(0, 0);
	Vec2 to = Vec2(0, 0);
	Spell<Bullet*>* spell = NULL;
	static Bullet* create(Sprite* sp, Node* node, IUnit* from = NULL) {
		Bullet* bullet = new Bullet;
		bullet->init(sp, node, from);
		return bullet;
	}
	virtual void init(Sprite* sp, Node* node, IUnit* from_) {
		from = from_;
		AddType(DANMAKU);
		IUnit::init(sp, node);
	}
	void InitSimple(int _speed, Vec2 target, IUnit* unit = NULL) {
		targetTo = new Vec2(target);
		speed = _speed;
		if (unit) {
			sprite->setPosition(unit->sprite->getPosition());
		}
	}
	virtual UINT UpdateSpell() override {
		UINT ur = 1;
		if (spell != nullptr) {
			ur = spell->Update(this);
			if (ur == DELETE) {
				type = ur;
			}
		}
		return ur;
	}
	virtual UINT Update() override {
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
	void SetFrom(IUnit* from_) {
		from = from_;
		sprite->setPosition(from->sprite->getPosition());
		if (from->id == PLAYER) {
			AddType(PLDANMAKU);
		}
	}
	IUnit* GetFrom() {
		return from;
	}
	Bullet* Copy() {
		Bullet* copyed = new Bullet;
		Sprite* sp = Sprite::createWithSpriteFrame(sprite->getSpriteFrame());
		copyed->init(sp, sprite->getParent(), from);
		copyed->AddType(id);
		copyed->type = type;
		copyed->Collided = Collided;
		copyed->DeathRattle = DeathRattle;
		if (spell) {
			copyed->spell = spell->Clone();
		}
		copyed->size = size;
		copyed->speed = speed;
		copyed->active = 1;/*
		copyed->fSpell = fSpell;
		copyed->fSpellIndex = fSpellIndex;*/
		copyed->sprite->setRotation(sprite->getRotation());
		copyed->targetBy = targetBy;
		if (target) {
			copyed->target = target;
		}
		if (targetTo) {
			copyed->targetTo = targetTo;
		}

		if (animation) {
			copyed->AddAnime(this->animation);
		}
		return copyed;
	}
};

static Rect GetHitboxRect(IUnit* unit, Node* convert = NULL, DrawNode* debug = NULL) {
	return GetHitboxRect(unit->sprite, convert, debug);
}

class Units {
public:
	DrawNode* debug = NULL;
	IUnit** units;
	IUnit** resources;
	int unitIndex = 0;
	int maxIndex = 0;
	int resourceMaxIndex = 0;
	int resourceIndex = 0;
	void Init(int unitMax, int resourceMax);
	void Update();
	UINT AddUnit(IUnit* unit);
	UINT AddResource(IUnit* resource);
	IUnit* AddCopyUnit(IUnit* unit) {
		IUnit* copyed = unit->Copy();
		AddUnit(copyed);
		copyed->sprite->setVisible(1);
		return copyed;
	}
	UINT CollitionSearch();
	void AddActionByType(UINT type, Action* action) {
		for (int i = 0; i < unitIndex; i++) {
			if (units[i]->type == type) {
				units[i]->AddAction(action->clone());
			}
		}
	}
	IUnit* GetRandomUnitByType(UINT type) {
		std::mt19937 mt;
		int i = 0;
		int index = 0;
		IUnit** selectedUnits = new IUnit * [unitIndex];
		for (i; i < unitIndex; i++) {
			if (units[i]->type == type) {
				selectedUnits[index] = units[i];
				index++;
			}
		}
		if (index == 0) {
			delete selectedUnits;
			return NULL;
		}
		std::uniform_int_distribution<int> ran(0, index);
		IUnit* result = selectedUnits[ran(mt)];
		delete selectedUnits;
		return result;
	}
	void DeleteUnit(int index);
};
//userinterface

class BackGround {
public:
	Vector<Texture2D*>* textures;
	Sprite* sp;
	Sprite* nowShow;
	int maxIndex;
	int now = 1;
	void Init(Vector<Texture2D*>* texture, Node* node) {
		textures = texture;
		sp = Sprite::createWithTexture(textures->getRandomObject());
		nowShow = Sprite::createWithTexture(textures->at(0));
		sp->setPosition(Director::getInstance()->getVisibleSize() / 2);
		nowShow->setPosition(Director::getInstance()->getVisibleSize() / 2);
		nowShow->setContentSize(Size(1920, 1080));
		node->addChild(sp);
		node->addChild(nowShow);
		sp->setTexture(textures->at(1));
		maxIndex = textures->size();
	}
	void ChangeTexture() {
		sp->setLocalZOrder(1);
		nowShow->setLocalZOrder(0);
		sp->runAction(Sequence::create(
			FadeOut::create(0.7),
			ChangeTexture::create(0, textures->at(now), sp),
			nullptr));
		nowShow->runAction(Sequence::create(
			FadeIn::create(0),
			nullptr));
		Sprite* a = nowShow;
		nowShow = sp;
		sp = a;
		now++;
		if (now == maxIndex) {
			now = 0;
		}
	}
};

class GalagaUI {
public:
	Sprite* push = NULL;
	int playerCreated = 0;
	DrawNode* debug;

	Label** pointLabel;
	Label* point;
	Label* pointText;
	Label* hPoint;
	Label* hPointText;
	Sprite** lifes;
	Label* startLabel;
	Label* gameOverLabel;
	int pLabelMax = 20;
	int pLabelIndex = 0;
	int p = 0;
	int hp = 2;
	int maxhp = 2;
	char* ps = new char[100];
	char* hps = new char[100];
	char* nps = new char[10];
	static GalagaUI* create(Sprite* push) {
		GalagaUI* dUI = new GalagaUI;
		dUI->Init(push);
		return dUI;
	}
	void AutoPosInit() {
		if (push) {
			push->setPosition(Vec2(500, 500));
		}
	}
	void Init(Sprite* push_) {
		push = push_;
		push->retain();
		debug = DrawNode::create();
		point = CreateLabel("0");
		pointText = CreateLabel("POINT");
		hPoint = CreateLabel("0");
		hPointText = CreateLabel("HIGH POINT");
		startLabel = CreateLabel("START");
		startLabel->retain();
		startLabel->setVisible(0);
		startLabel->setPosition(Vec2(WINDOWWIDTH / 2, WINDOWHEIGHT / 2));
		gameOverLabel = CreateLabel("GAME OVER");
		gameOverLabel->retain();
		gameOverLabel->setVisible(0);
		gameOverLabel->setPosition(Vec2(WINDOWWIDTH / 2, WINDOWHEIGHT / 2));
		lifes = new Sprite * [hp];
		for (int i = 0; i < hp; i++) {
			lifes[i] = Sprite::create("pl.png");
			lifes[i]->setScale(3);
			if (i == 0) {
				lifes[i]->setPosition(Vec2(WINDOWWIDTH / 10, WINDOWHEIGHT / 10));
			}
			else {
				lifes[i]->setPosition(Vec2(lifes[i - 1]->getBoundingBox().size.width / 2 + lifes[i - 1]->getPosition().x,
					lifes[i - 1]->getPosition().y));
				lifes[i]->retain();
				lifes[i]->setVisible(1);
			}
		}
		pointLabel = new Label * [pLabelMax];
		for (int i = 0; i < pLabelMax; i++) {
			pointLabel[i] = CreateLabel(ps);
			pointLabel[i]->setTextColor(Color4B::RED);
			pointLabel[i]->setVisible(0);
		}

		point->setPosition(Vec2(WINDOWWIDTH / 10, WINDOWHEIGHT - WINDOWHEIGHT / 8));
		pointText->setPosition(Vec2(WINDOWWIDTH / 10, WINDOWHEIGHT - WINDOWHEIGHT / 11));
		hPoint->setPosition(Vec2(WINDOWWIDTH - WINDOWWIDTH / 10, WINDOWHEIGHT - WINDOWHEIGHT / 8));
		hPointText->setPosition(Vec2(WINDOWWIDTH - WINDOWWIDTH / 10, WINDOWHEIGHT - WINDOWHEIGHT / 11));
		point->retain();
		pointText->retain();
		hPoint->retain();
		hPointText->retain();
	}
	void Active(Node* node) {
		node->addChild(push);
		push->setVisible(0);
		node->addChild(debug);
		node->addChild(hPointText);
		node->addChild(pointText);
		node->addChild(point);
		node->addChild(hPoint);
		node->addChild(startLabel);
		node->addChild(gameOverLabel);
		for (int i = 0; i < hp; i++) {
			node->addChild(lifes[i]);
		}
		for (int i = 0; i < pLabelMax; i++) {
			node->addChild(pointLabel[i]);
		}
	}
	void ViewPlayerPush() {
		push->setVisible(1);
	}

	void UpdateLife() {
		for (int i = 0; i < maxhp; i++) {
			if (i > hp) {
				lifes[i]->setVisible(0);
			}
			else {
				lifes[i]->setVisible(1);
			}
		}
	}

	void UpdatePoint(int pp, Vec2 pos) {
		p += pp;
		sprintf(ps, "%d", p);
		sprintf(nps, "%d", pp);
		point->setString(ps);
		hPoint->setString(hps);
		pointLabel[pLabelIndex]->setString(nps);
		pointLabel[pLabelIndex]->setVisible(1);
		pointLabel[pLabelIndex]->setPosition(pos);
		Sequence* sq;
		Vector<FiniteTimeAction*> vec;
		vec.pushBack(MoveBy::create(1, Vec2(0, 20)));
		vec.pushBack(SetUnvisible::create(0));
		vec.pushBack(RemoveSelf::create());
		sq = Sequence::create(vec);
		pointLabel[pLabelIndex]->runAction(sq);
		pLabelIndex++;
		if (pLabelIndex >= pLabelMax) {
			pLabelIndex = 0;
		}
	}

	UINT MouseClick(Vec2 pos) {
		if ((!playerCreated) && (push->isVisible() == 1)) {
			if (GetHitboxRect(push).containsPoint(pos)) {
				SimpleAudioEngine::getInstance()->playEffect("extend.mp3");
				push->runAction(Sequence::create(FadeOut::create(1), RemoveSelf::create(), nullptr));
				playerCreated = 1;
				return 1;
			}
		}
		return 0;
	}
};

class AmiUI {
public:
	Sprite* push = NULL;
	int playerCreated = 0;
	DrawNode* debug;
	int playerSpeed = 1;
	static AmiUI* create(Sprite* push) {
		AmiUI* aUI = new AmiUI;
		aUI->Init(push);
		return aUI;
	}
	void AutoPosInit() {
		if (push) {
			push->setPosition(Vec2(500, 500));
		}
	}
	void Init(Sprite* push_) {
		push = push_;
		push->retain();
		debug = DrawNode::create();
	}
	UINT MouseClick(Vec2 pos) {
		if ((!playerCreated) && (push->isVisible() == 1)) {
			if (GetHitboxRect(push).containsPoint(pos)) {
				SimpleAudioEngine::getInstance()->playEffect("extend.mp3");
				push->runAction(Sequence::create(FadeOut::create(1), RemoveSelf::create(), nullptr));
				playerCreated = 1;
				return 1;
			}
		}
		return 0;
	}
	void Active(Node* node) {
		node->addChild(push);
		push->setVisible(0);
		node->addChild(debug);
	}
	void ViewPlayerPush() {
		push->setVisible(1);
	}

};

class OsuBar {
	Label** optionLabel = NULL;
	Sprite** selectBoxs = NULL;
	int optionIndex = 0;
	Vec2 optionPos;
	Vec2 optionWeight;
	Vec2 levelWeight;
	Vec2* optionBoxPos = new Vec2[5];

	void AutoInitPos() {
		optionPos = Vec2(WINDOWWIDTH - WINDOWWIDTH / 8, WINDOWHEIGHT - WINDOWHEIGHT / 12);
		optionWeight = Vec2(100, WINDOWHEIGHT / 6);
		levelWeight = Vec2(250, 100);
		for (int i = 0; i < 5; i++) {
			if (i > 3) {
				optionBoxPos[i] = Vec2(optionPos -
					Vec2(optionWeight.x * (5 - i), optionWeight.y * i));
			}
			else {
				optionBoxPos[i] = Vec2(optionPos - optionWeight * i);
			}
		}
	}

	void Init(Texture2D* selectbox, Vector<Label*>* option) {
		optionIndex = option->size();
		selectBoxs = new Sprite * [optionIndex];
		optionLabel = new Label * [optionIndex];
		for (int i = 0; i < optionIndex; i++) {
			optionLabel[i] = CreateLabel(option->at(i)->getString());
			optionLabel[i]->setPosition(optionPos - optionWeight * i);
			optionLabel[i]->setTextColor(Color4B::BLACK);
			optionLabel[i]->setScale(0.9);
			selectBoxs[i] = Sprite::create();
			selectBoxs[i]->initWithTexture(selectbox);
			selectBoxs[i]->setPosition(optionLabel[i]->getPosition());
			selectBoxs[i]->setOpacity(255 / 2);
			selectBoxs[i]->addChild(optionLabel[i]);
			selectBoxs[i]->setPosition(optionBoxPos[i]);
		}
	}

};

class SelectUI {
public:
	BackGround* backSpace = NULL;
	Sprite* backGround = NULL;
	Sprite* title = NULL;
	Sprite** activeSelectBox = NULL;
	Label** optionLabel = NULL;
	Sprite* note = NULL;
	int optionIndex = 0;
	Animate* selectedAnime = NULL;
	DrawNode* debug = NULL;
	Vec2 optionPos;
	Vec2 optionWeight;
	Vec2 titleVec;
	Vec2 noteVec;
	int nowSelected = -1;

	static SelectUI* create( Vector<Texture2D*>* textures, Sprite* title, Texture2D* selectBox, Vector<Label*>* option, Animation* selected, Sprite* note, Node* node) {
		SelectUI* sUI = new SelectUI;
		sUI->Init(textures, title, selectBox, option, selected, note, node);
		return sUI;
	}
	void AutoPosInit() {
		optionPos = Vec2(200, 250);
		optionWeight = Vec2(0, 50);
		titleVec = Vec2(200, 350);
		noteVec = Vec2(WINDOWWIDTH/4, WINDOWHEIGHT / 2);
	}
	void Init(Vector<Texture2D*>* textures, Sprite* title_, Texture2D* selectbox, Vector<Label*>* option, Animation* selected, Sprite* Note, Node* node) {
		/*auto properties = Properties::createNonRefCounted("effect.material#sample");
		Material* mat1 = Material::createWithProperties(properties);*/

		backSpace = new BackGround;
		backSpace->Init(textures, node);
		/*backGround = textures;
		backGround->setOpacity(255 / 2);
		backGround->setPosition(Director::getInstance()->getVisibleSize() / 2);*/
		title = title_;
		optionIndex = option->size();
		activeSelectBox = new Sprite * [optionIndex];
		optionLabel = new Label * [optionIndex];
		note = Note;
		note->setScale(2);
		note->setPosition(noteVec);
		note->retain();
		for (int i = 0; i < optionIndex; i++) {
			optionLabel[i] = CreateLabel(option->at(i)->getString());
			optionLabel[i]->setPosition(optionPos - optionWeight * i);
			optionLabel[i]->setTextColor(Color4B::BLACK);
			optionLabel[i]->setScale(0.9);
			note->addChild(optionLabel[i]);
			activeSelectBox[i] = Sprite::create();
			activeSelectBox[i]->initWithTexture(selectbox);
			activeSelectBox[i]->setPosition(optionLabel[i]->getPosition());
			activeSelectBox[i]->setOpacity(255 / 2);
			//activeSelectBox[i]->setScale(3);

			note->addChild(activeSelectBox[i]);
		}
		delete option;
		selectedAnime = Animate::create(selected);
		if (title) {
			title->setPosition(titleVec);
			title->retain();
		}
		selectedAnime->retain();
	}
	void Active(Node* parent) {
		if (note) {
			parent->addChild(note, 8);
		}
		if (debug) {
			note->addChild(debug, 10);
		}
		note->addChild(title);
		//parent->addChild(backGround, 0);
	}
	void FadeIn() {
		Vector<FiniteTimeAction*> vec;
		vec.pushBack(MoveBy::create(0, Vec2(-1000, 0)));
		vec.pushBack(MoveBy::create(1, Vec2(1000, 0)));
		note->runAction(Sequence::create(vec));

		Vector<FiniteTimeAction*> vec1;
		BlendFunc blend;
		blend.src = backend::BlendFactor::ONE;
		blend.dst = backend::BlendFactor::ONE;
		vec1.pushBack(SetBlendFunc::create(0, blend, title));/*
		vec1.pushBack(MoveBy::create(0, Vec2(1000, 0)));
		vec1.pushBack(MoveBy::create(1, Vec2(-1000, 0)));*/
		blend.src = backend::BlendFactor::BLEND_CLOLOR;
		blend.dst = backend::BlendFactor::BLEND_CLOLOR;
		vec1.pushBack(SetBlendFunc::create(0, blend, title));
		vec1.pushBack(TintTo::create(0, Color3B::WHITE));
		vec1.pushBack(TintTo::create(3, 0, 0, 0));
		title->runAction(Sequence::create(vec1));

	}
	UINT SetMousePos(Vec2 pos) {
		if (note->getBoundingBox().containsPoint(pos)) {
			pos = note->convertToNodeSpace(pos);
			for (int i = 0; i < optionIndex; i++) {
				if (GetHitboxRect(activeSelectBox[i], NULL, debug).containsPoint(pos)) { //pos위치 정립
					if (selectedAnime) {
						if (i != nowSelected) {
							//activeSelectBox[i]->runAction(Repeat::create(selectedAnime, 1));
							if (nowSelected != -1) {
								activeSelectBox[nowSelected]->setColor(Color3B::WHITE);
							}
							activeSelectBox[i]->setColor(Color3B::ORANGE);
							SimpleAudioEngine::getInstance()->playEffect("select00.mp3");
							nowSelected = i;
						}
					}
				}
			}

		}
		return 0;
	}
	UINT SetMouseClick(Vec2 pos) {
		pos = note->convertToNodeSpace(pos);
		for (int i = 0; i < optionIndex; i++) {
			if (GetHitboxRect(activeSelectBox[i], NULL, debug).containsPoint(pos)) { //pos위치 정립
				if (selectedAnime) {
					SimpleAudioEngine::getInstance()->playEffect("cancel00.mp3");
					return i + 1;
				}
			}
		}
		/*if (GetHitboxRect(title).containsPoint(pos)) {
			Image* img = new Image;
			img->initWithImageFile("freeS.png");
			Texture2D* tx = new Texture2D;
			tx->initWithImage(img);
			title->setTexture(tx);
		}*/
		return 0;
	}
	void release() {
		if (title) {
			title->release();
		}
		if (selectedAnime) {
			selectedAnime->release();
		}
		if (note) {
			note->release();
		}
		if (debug) {
			debug->release();
		}
	}
};

class ActionDone {
public:
	bool _actionDone;
	bool _invincibility = true;
};

static Sprite* GetSprite(std::string filename, int width, int height, int x, int y) {
	Image* image = new Image;
	image->initWithImageFile(filename);
	Texture2D* texture = new Texture2D;
	texture->initWithImage(image);

	Rect rect;
	rect.origin = Vec2(x, y);
	rect.size = Size(width, height); // spriteFrame init
	Sprite* sp = new Sprite;
	sp->initWithSpriteFrame(SpriteFrame::createWithTexture(texture, rect));
	return sp;
}

static Animation* GetAnimation(std::string filename, int width, int height, int x, int y, int xCount, float delayTime = 0.2f, int count = -1, int start = 0) {
	Animation* animation;
	Vector<SpriteFrame*> vec;
	Image* img = new Image;
	img->initWithImageFile(filename);
	Texture2D* texture = new Texture2D;
	texture->initWithImage(img);
	if (count == -1) {
		count = xCount;
	}
	xCount -= start;
	Rect rect;
	int p = 0;
	for (int i = 0; i < count; i++) {
		rect.size.width = width;
		rect.size.height = height;
		rect.origin.x = x;
		rect.origin.y = y;
		if ((!(i % xCount)) && (i != 0)) {
			y += height;
			x = 0;
			if (start) {
				xCount += start;
				start = 0;
			}
		}
		x += width;
		vec.pushBack(SpriteFrame::createWithTexture(texture, rect));
	}
	animation = Animation::createWithSpriteFrames(vec);
	animation->setDelayPerUnit(delayTime);
	return animation;
}

class Stage {
private:
	int(*_stage)(int* tick, Units* units);
	Units* _units;
	unsigned int repeat = 0;
public:
	int* tick;
	int isComplete;

	void Init(int(*stage)(int* tick, Units* units), Units* units = NULL) {
		_stage = stage;
		if (units) {
			_units = units;
		}
		tick = new int(0);
		isComplete = 0;
	}

	int Update() {
		if (_stage != nullptr) {
			isComplete = _stage(tick, _units);
			*tick += 1;
			if ((isComplete)&&(repeat)) {
				*tick = 0;
				repeat--;
				return 0;
			}
		}
		return isComplete;
	}
};

//GALAGA//

struct Line {
public:
	Vec2 pos = Vec2(0, 0);
	float width = 0;
	int unit = 0;
	int maxUnit = -1;
	float weight;
	void Init(int max, Vec2 _pos, int _width) {
		maxUnit = max;
		pos = _pos;
		width = _width;
		weight = width / maxUnit;
	}
	int GetPos(Vec2* val) {
		if (maxUnit <= unit) {
			return 0;
		}
		float y = pos.y;

		int unitWeight = unit/2 + 1;
		if (unit % 2) {
			unitWeight = -unitWeight;
		}
		val->x = unitWeight * weight + (pos.x + width / 2);
		val->y = y;
		unit++;
		return 1;
	}

};

class LineInfo {
public:
	Line* line;
	int index;
	static LineInfo* create(Line* line, int index) {
		LineInfo* li = new LineInfo;
		li->index = index;
		li->line = line;
		return li;
	}
};

class Field {
public:
	int height;
	int width;
	Line* lines;
	Vec2 center;
	void Init();
	bool GetEmptySpace(int line, __out Vec2* val);

};

class StarField {
public:
	Sprite** stars = NULL;
	int maxIndex = 0;
	int index = 0;
	float time_;

	void Init(int max, int time) {
		stars = new Sprite * [max];
		maxIndex = max;
		time_ = time;
	}
	void AddStar(Sprite* star, Vec2 pos, float size, Color3B color) {
		if (stars == nullptr) {
			Init(5, 5);
		}
		stars[index] = Sprite::createWithSpriteFrame(star->getSpriteFrame());
		star->getParent()->addChild(stars[index]);
		stars[index]->setScale(size);
		stars[index]->setColor(color);
		stars[index]->setVisible(1);
		stars[index]->setPosition(pos);
		stars[index]->runAction(MoveBy::create(time_, Vec2(0, -WINDOWHEIGHT)));
		index++;
		if (maxIndex == index) {
			Sprite** newStar = new Sprite * [maxIndex + 10];
			for (int i = 0; i < index; i++) {
				newStar[i] = stars[i];
			}
			stars = newStar;
		}
	}
	void DeleteStar(int ii)
	{
		stars[ii]->getParent()->removeChild(stars[ii]);
		for (int i = ii + 1; i < index; i++) {
			stars[i - 1] = stars[i];
		}
		index--;
	}
	void Update() {
		for (int i = 0; i < index; i++) {
			if (stars[i]->getPosition().y <= 0) {
				DeleteStar(i);
			}
		}
	}
};

//Action
template <class T>
class ChangeSpell : public ActionInterval {
public:
	static ChangeSpell<T>* create(Spell<T>* spell, T unit) {
		ChangeSpell<T>* cs = new ChangeSpell<T>;
		cs->initWithDuration(spell, unit);
		return cs;
	}

	bool initWithDuration(Spell<T>* spell_, T unit_) {
		spell = spell_;
		unit = unit_;
		return ActionInterval::initWithDuration(0);
	}

	virtual void update(float time) override {
		if (_target) {
			unit->spell = spell;
		}
	}

private:
	Spell<T>* spell;
	T unit;
};

enum class StartPos {
	top,
	middle,
	bottom
};

template<class T>
class ChangeCollided : public ActionInterval {
public:
	static ChangeCollided<T>* create(UINT(*collided)(IUnit* with), T unit) {
		ChangeCollided<T>* cc = new ChangeCollided<T>;
		cc->initWithDuration(collided, unit);
		return cc;
	}

	bool initWithDuration(UINT(*_collided)(IUnit* with), T _unit) {
		collided = _collided;
		unit = _unit;
		return ActionInterval::initWithDuration(0);
	}

	virtual void update(float time) {
		if (_target) {
			unit->Collided = collided;
		}
	}
private:
	UINT(*collided)(IUnit* with);
	T unit;
};


template <class T>
class AddFSpell : public ActionInterval {
public:
	static AddFSpell<T>* create(FinalSpell<T>* spell, T unit) {
		AddFSpell<T>* cs = new AddFSpell<T>;
		cs->initWithDuration(spell, unit);
		return cs;
	}

	bool initWithDuration(FinalSpell<T>* spell_, T unit_) {
		spell = spell_;
		unit = unit_;
		return ActionInterval::initWithDuration(0);
	}

	virtual void update(float time) override {
		if (_target) {
			unit->AddFinalSpell(spell);
		}
	}

private:
	FinalSpell<T>* spell;
	T unit;
};
