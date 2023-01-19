#include "Game.h"
#define DanmakuFollowTarget 0x00000010
using namespace backend;

namespace ThResource {
	//hide
	class UserInterface {
	public:
		Rect hpBar;

		Label* point;
		Label* bossName;
		Vec2 PlayOrigin;
		Size PlaySize;
		Node* playZone;
		DrawNode* drawNode;
		Node* drawZone;
		Sprite* backGround;
		UINT nowPoint;
		Vec2 enemyLinePos;
		int enemyLineWidth;
		Sprite* startShow;
		bool stageStarted = false;
		ProgramState* TitleProgramState;
		Material* met;

		void Init(Node* node) {
			Vec2 visibleSize = Director::getInstance()->getVisibleSize();
			playZone = Node::create();
			//playZone->setAnchorPoint(Vec2(0, 0));
			playZone->setPosition(Vec2(
				visibleSize.y / 20,
				visibleSize.y / 20)
			);
			playZone->setContentSize(Size(
				visibleSize.x / 2 - visibleSize.y / 20,
				visibleSize.y - visibleSize.y / 10
			));
			drawZone = Node::create();
			drawZone->setPosition(Vec2(0, 0));
			drawZone->setContentSize(Size(visibleSize.x / 2, visibleSize.y));
			backGround = Sprite::create("th/gameWindow.png");
			backGround->setPosition(Director::getInstance()->getVisibleSize() / 2);
			startShow = Sprite::create("th/start.png");
			startShow->setPosition(playZone->getContentSize() / 2);
			int width = playZone->getContentSize().width;
			enemyLinePos = Vec2(width / 8, playZone->getContentSize().height);
			enemyLineWidth = width - width / 8 * 2;
			if (backGround) {
				node->addChild(backGround, 1);
			}
			playZone->addChild(startShow, INT_MAX);
			node->addChild(playZone);
			node->addChild(drawZone);
			startShow->setOpacity(0);
			met = Material::createWithProperties(Properties::createNonRefCounted("effect.meterial#sample"));
			met->retain();
			//TitleProgramState
		}
		void StageStart() {
			if (!stageStarted) {
				startShow->runAction(Sequence::create(
					//MoveBy::create(0, Vec2(-startShow->getPosition().x, 0)),
					FadeOut::create(0),
					//MoveBy::create(2, Vec2(startShow->getPosition().x, 0)),
					FadeIn::create(2),
					DelayTime::create(2),
					//MoveBy::create(2, Vec2(startShow->getPosition().x, 0)),
					FadeOut::create(2),
					//MoveBy::create(0, Vec2(-startShow->getPosition().x, 0)),
					nullptr
				));
				stageStarted = 1;
			}
		}
	};

	class Boss : public IUnit {
	public:
		int shield;
		static Boss* create(Sprite* sp, Node* node, Animation* defaltAnimation = NULL) {
			Boss* enemy = new Boss;
			enemy->init(sp, node, defaltAnimation);
			return enemy;
		}
		virtual void init(Sprite* sp, Node* node, Animation* defaltAnimation = NULL) {
			AddType(ENEMY);
			animation = defaltAnimation;
			IUnit::init(sp, node);
		}
	};

	class BoostBullet : Bullet {
		int maxSpeed = 0;
		int speedPlus = 0;
	};

	class DefaltBackGround {
	public:
		Sprite* tile;
		Sprite*** map;
		Vec2 s = Vec2(0, 0);
		Vec2 index;
		void Init(Sprite* mapTile, Node* node);
		void DownFall(Vec2 speed) {
			for (int i = 0; i < index.x; i++) {
				for (int y = 0; y < index.y; y++) {
					map[i][y]->runAction(MoveBy::create(0, speed));
				}
			}
			s += speed;
			Vec2 size = map[0][0]->getContentSize();
			if ((abs(s.x) > size.x) ||
				(abs(s.y) > size.y)) {
				Vec2 move = Vec2((int)s.x / (int)size.x * (int)size.x, (int)s.y / (int)size.y * (int)size.y);
				s = Vec2((int)s.x % (int)size.x, (int)s.y % (int)size.y);
				for (int i = 0; i < index.x; i++) {
					for (int y = 0; y < index.y; y++) {
						map[i][y]->runAction(MoveBy::create(0, -move));
					}
				}
			}
		}
	};

	class BackGroundReimuLine {
		Texture2D* line;
		int lineLimit;
		Vec2* startPos;
		Vec2* endPos;
		
		void Init(int lineMax, Texture2D* line_, Vec2* startPos = NULL, Vec2* endPos = NULL);
		void Show();
	};

	class Touhou {
	public:
		Node* scene;
		UserInterface* UInterface;
		PhysicsWorld* world;
		std::mt19937 mt;
		Sprite* testBoss;
		int playerSpeed = 5;
		int playerLowSpeed = 2;
		bool isLow = false;
		Animation* ufoDeath;
		Animation* defaultDeathAnimation;
		Bullet* playerBullet1;
		Bullet* playerBullet2;
		Bullet* enemyKong;
		Bullet* enemySal;
		Sprite* playerGun;
		Sprite* playerSlowMagicCircle;
		Player* player;
		Units* units;
		Boss* pachuri;
		Enemy* ufo;
		Enemy* bYousei;
		Enemy* rYousei;
		Enemy* oYousei;
		Enemy* oBook;
		DrawNode* debug = NULL;
		DefaltBackGround* defaultBackground = NULL;
		Stage stageBegin;
		Stage stage1;
		Stage stage2;
		//ValueMap firePaticle = FileUtils::getInstance()->getValueMapFromFile("th/fire.plist");
		void Init(Node* node);
		void PlayerActive();
		void PlayerShot();
		void AddTestUfo();
		UINT PlayerMove(Vec2 pos);
		static UINT DeleteOutOfGame(IUnit* unit, int* tick);
		static UINT SpellPlayer(IUnit* unit, int* tick);
		static UINT SpellUfo(IUnit* unit, int* tick);
		static UINT SpellYouseiDP(IUnit* unit, int* tick);
		static UINT SpellFireRain(IUnit* unit, int* tick);
		static UINT SpellChangeColor(IUnit* unit, int* tick);
		static UINT SpellSpectrum(IUnit* unit, int* tick);
		static UINT SpellEffect(IUnit* unit, int* tick);
		static UINT BSpellDefalt(Bullet* unit, int* tick);
		static UINT BSpellWaitFollowTarget(Bullet* unit, int* tick);
		static UINT BSpellFollowTarget(Bullet* unit, int* tick);
		static UINT BSpellFireRainBullet(Bullet* unit, int* tick);
		static UINT FSpellYouseiDash(IUnit* unit, int* tick);
		static UINT FSpellDeleteSelf(IUnit* unit, int* tick);
		static UINT FSpellWaitActionDone(IUnit* unit, int* tick);

		static UINT CollidedTargetPlayer(IUnit* with);
		static UINT CollidedEnemy(IUnit* with);
		static UINT	DeathRattleChangeFollowTargetDanmaku(IUnit* unit, Units* unist);
		static UINT DeathRattleCreateNewDanmaku(IUnit* unit, Units* units);
		static UINT DeathRattleHpDown(IUnit* unit, Units* units);
		static UINT DeathRattleUfo(IUnit* unit, Units* units);
		static UINT DeathRattleDefault(IUnit* unit, Units* units);
		static int StageAgeru(int* tick, Units* units);
		static int Stage1(int* tick, Units* units);
		static int Stage2(int* tick, Units* units);
		static int StagePachuriPre1(int* tick, Units* units);

		void update() {
			units->Update(); 
			bool stageEnd = false;
			stageEnd = stageBegin.Update();
			if (stageEnd) {
				//stageEnd = stage1.Update();
				stageEnd = stage2.Update();
			}
			if (stageEnd) {
				//stageEnd = stage2.Update();
				stageEnd = stage1.Update();
			}
			if (isLow) {
				playerSlowMagicCircle->setVisible(1);
			}
			else {
				playerSlowMagicCircle->setVisible(0);
			}
		}//tkdurak sp����
	};
	

}