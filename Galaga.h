#include "Game.h"

namespace GalagaResource {
	class Galaga {
	public:
		void Init(Node* node);
		void LoadAllGalagaResource(Node* node);
		void LoadAllGalagaEnemy(Node* node);
		void LoadPlayer(Node* node);

		static UINT AttackToPlayerSpell(IUnit* unit, int* tick);
		void PlayerAttack();
		static UINT EnemyDefaltSpell(IUnit* unit, int* tick);
		void AddStar();
		void SetPlayerMove(float f);
		static Action* GetStartUpAction(Vec2 space, int left, bool down, StartPos pos, int level, IUnit* unit);
		static Action* GetDownFallAction(Vec2 target, Vec2 selfPos, IUnit* unit);
		//UINT EnemyFlyFSpell(IUnit* unit, int* tick);
		//INFO//
		static UINT CollidedEnemy(IUnit* with);
		static UINT CollidedPlayer(IUnit* with);
		static UINT DanmakuDeathRattle(IUnit* unit, Units* units);
		static unsigned int EffectDefaltSpell(IUnit* unit, int* tick);
		static UINT EnemyDeathRattleNoEffect(IUnit* unit, int tick);
		static UINT EnemyDeathRattle(IUnit* unit, Units* units);
		static UINT BossDeathRattle(IUnit* unit, Units* units);
		static UINT BossDeathRattle2(IUnit* unit, Units* unist);
		static Animate* GetPlayerDeathEffect(IUnit* unit, Units* unist);
		static UINT PlayerDeathRattle(IUnit* unit, Units* units);
		static UINT PlayerDeathMoveFSpell(IUnit* unit, int* tick);
		UINT PlayerEffectSpell(IUnit* unit, int* tick);
		//STAGE//
		static int TestStageBee(int* tick, Units* stage, Field* field);
		static int TestStageGalaga(int* tick, Units* stage, Field* field);
		static int MoveStage(int* tick, Units* stage, Field* field);
		static int DefaltStage(int* tick, Units* stage, Field* field);
		//START//
		void Start();
		//UPDATE//
		void UpdateGalaga();

		Player* player;
		GalagaUI* dui;
	private:
		std::mt19937 mt = std::mt19937((unsigned int)time(NULL));
		Units* units;
		Texture2D* tx;
		Animation* playerDeathAni;
		Field* field = new Field;
		StarField* starField = new StarField;
		Cooltime starCoolTime = Cooltime(10);
		Sprite* star;
		bool stageIsStarted = false;
		bool stageIsEnd = false;
		//ENEMY//
		IUnit* bee;
		IUnit* galaga;
		IUnit* boss1;
		IUnit* boss2;
		Bullet* plDanmaku;
		Bullet* danmaku;
		Animation* enemyDeathAni;
		Animation* danmakuDeathAni;
		//STAGE//
		Stage* stageBee = new Stage;
		Stage* stageGalaga = new Stage;
		Stage* fieldMove = new Stage;
		Stage* StageDefalt = new Stage;
	};
	void Ready(Galaga* game);
}