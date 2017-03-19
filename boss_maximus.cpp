/* ScriptData
SDCreator: frostI
SDName: boss_maximus
SD%Complete: 100
SDComment:
SDCategory: Molten Core
EndScriptData */

#include "ScriptPCH.h"
#include "molten_core.h"

enum Spells
{
	// Maximus
	SPELL_AURA_OF_SHADOWS             = 41142,
	SPELL_SUNDER_ARMOR                = 58461,
	SPELL_INCREASE_STRENGTH           = 69167,
	SPELL_METEOR_FISTS                = 66725,
	SPELL_MARK_FOR_FLAME              = 6767,
	SPELL_FLAME_TRAVEL                = 67330,
	SPELL_FLAME_HIT_VISUAL_1          = 74648,
	SPELL_FLAME_HIT_VISUAL_2          = 41284,
	SPELL_DRAW_FLAMES_CHANNEL_VISUAL  = 36730,
	SPELL_FABULOUS_FLAME              = 72811,
	SPELL_CORRUPTED_BLOOD_VISUAL      = 60451,
	SPELL_CORRUPTED_BLOOD_GRAVITATION = 28337,
	SPELL_CORRUPTED_BLOOD             = 71152,
	SPELL_SHADOW_PRISON               = 45922,
	SPELL_ENRAGE_VISUAL               = 65129,
	SPELL_ENRAGE                      = 26662,
	SPELL_FINGER_OF_DEATH             = 31984,
	// Smoldering Tsunami
	SPELL_TSUNAMI_VISUAL              = 57494,
	SPELL_TSUNAMI_LEAP                = 60241,
	SPELL_TSUNAMI_PERIODIC_DMG        = 57491,
	// Crimson Flame
	SPELL_FLAME_VISUAL                = 64561,
	SPELL_FLAME_GROWTH                = 36300,
	// Pool of Light
	SPELL_POOL_VISUAL_1               = 72054,
	SPELL_POOL_VISUAL_2               = 57772,
	SPELL_POOL_HEAL                   = 64844,
	SPELL_POOL_MANA                   = 64904,
	// Maiev Shadowsong
	SPELL_MAIEV_PROTECTION            = 60082,
	SPELL_MAIEV_TELEPORT_VISUAL       = 33056,
	// Volcano Trigger
	SPELL_VOLCANO_ERUPTION            = 63666
};

enum Events
{
	// Maximus
	EVENT_AURA_OF_SHADOWS = 1,
	EVENT_SUNDER_ARMOR,
	EVENT_INCREASE_STRENGTH,
	EVENT_METEOR_FISTS,
	EVENT_SUMMON_TSUNAMI,
	EVENT_MARK_FOR_FLAME,
	EVENT_CALL_FLAME,
	EVENT_DRAW_FLAMES,
	EVENT_FABULOUS_FLAMES,
	EVENT_START_CORRUPTED_BLOOD,
	EVENT_CORRUPTED_BLOOD,
	EVENT_CORRUPTED_BLOOD_GRAVITATION,
	EVENT_ENRAGE,
	EVENT_FINGER_OF_DEATH,
	// Heating Tsunami
	EVENT_TSUNAMI_PERIODIC_DMG,
	// Crimson Flame
	EVENT_FLAME_GROWTH,
	// Defense Trap
	EVENT_POOL_HEAL,
	EVENT_POOL_MANA,
	// Maiev Shadowsong
	EVENT_MAIEV_SAY,
	EVENT_MAIEV_PROTECTION_EMOTE,
	EVENT_MAIEV_TELEPORT,
	EVENT_MAIEV_PROTECTION,
	// Volcano Trigger
	EVENT_VOLCANO_ERUPTION
};

enum Actions
{
	// Maximus
	ACTION_DRAW_FLAMES = 1,
	ACTION_PRE_START_FABULOUS_FLAMES,
	ACTION_START_FABULOUS_FLAMES,
	ACTION_END_FABULOUS_FLAMES,
	ACTION_CORRUPT_BLOOD,
	ACTION_MAIEV,
	ACTION_ENRAGE,
	// Crimson Flame
	ACTION_DRAW_FLAME,
};

enum Sounds
{
	SOUND_ONAGGRO   = 8595,
	SOUND_ONSLAY    = 8602,
	SOUND_ONDEATH   = 8594,
	SOUND_ONBERSERK = 16148,
	SOUND_ONMAIEV   = 11492
};

struct Location
{
	float x, y, z;
};

static Location VolcanoSpawnPos[] =
{
	{ 594.836792f, -767.555176f, -191.923935f },
	{ 683.485046f, -808.293518f, -195.010727f },
	{ 601.663025f, -762.732971f, -197.011002f }
};

static Location TsunamiSpawnPos[] =
{
	{ 604.456421f, -857.171387f, -208.113358f },
	{ 569.781189f, -830.546021f, -206.399002f },
	{ 580.313293f, -766.924988f, -208.083527f }
};

static Location TsunamiOne =   { 623.283325f, -764.657227f, -208.215363f };
static Location TsunamiTwo =   { 693.175049f, -778.324951f, -209.290573f };
static Location TsunamiThree = { 625.636353f, -862.696838f, -208.596710f };
static Location MaximusFFPos = { 615.326294f, -808.754517f, -207.138535f };
static Location MaximusCBPos = { 585.261230f, -834.041565f, -206.385147f };
static Location PoolSpawn =    { 583.766541f, -806.713562f, -204.889053f };
static Location MaievSpawn =   { 697.306824f, -725.409424f, -209.381683f };

#define EMOTE_TSUNAMI           "A Smoldering Tsunami rises from the lava!"
#define EMOTE_STRENGTH          "Maximus' strength is increasing!"
#define EMOTE_DRAW_FLAMES       "Maximus begins to draw the Crimson Flames!"
#define EMOTE_FABULOUS_FLAMES   "Maximus still has some charges left!"
#define EMOTE_CORRUPTED_BLOOD   "Maximus' corrupted blood is becoming unstable!"
#define EMOTE_CORRUPTED_BLOOD_2 "Maximus' corrupted blood is becoming more unstable!"
#define EMOTE_ENRAGE            "Maximus Enrages!"
#define EMOTE_MAIEV_PROTECTION  "Maiev conjures a Twilight Barrier. Quickly, move in!"
							    
#define YELL_ON_ENTER_COMBAT    "My powers are renewed!"
#define YELL_ON_DEATH           "I have.. failed..."
#define YELL_ON_KILLED_UNIT     "You are terminated."
#define YELL_ON_ENRAGE          "Huhuhuhahahahahahaha..."
#define YELL_MAIEV_APPEAR       "Ah... my long hunt is finally over. Today, Justice will be done!"

class MaximusDespawner : public BasicEvent
{
public:
	explicit MaximusDespawner(Creature* creature) : creature(creature) { }

	bool Execute(uint64 /*currTime*/, uint32 /*diff*/)
	{
		Trinity::CreatureWorker<MaximusDespawner> worker(creature, *this);
		creature->VisitNearbyGridObject(1000.0f, worker);
		return true;
	}

	void operator()(Creature* creature) const
	{
		switch (creature->GetEntry())
		{
		    case NPC_MAXIMUS:
		    	if (InstanceScript* instance = creature->GetInstanceScript())
		    	{
		    		instance->SetBossState(DATA_MAXIMUS, FAIL);
		    		instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_FOR_FLAME);
		    	}
		    	break;
		    case NPC_CRIMSON_FLAME:
		    case NPC_POOL_OF_LIGHT:
		    case NPC_SMOLDERING_TSUNAMI_MC:
		    case NPC_MAIEV_SHADOWSONG:
		    case NPC_VOLCANO_2:
		    	creature->DespawnOrUnsummon();
		    	break;
		    default:
		    	return;
		}

		uint32 corpseDelay = creature->GetCorpseDelay();
		uint32 respawnDelay = creature->GetRespawnDelay();
		creature->SetCorpseDelay(1);
		creature->SetRespawnDelay(30);

		if (CreatureData const* data = creature->GetCreatureData())
			creature->SetPosition(data->posX, data->posY, data->posZ, data->orientation);

		creature->DespawnOrUnsummon();
		creature->SetCorpseDelay(corpseDelay);
		creature->SetRespawnDelay(respawnDelay);
	}

private:
	Creature* creature;
};

class boss_maximus : public CreatureScript
{
public:
	boss_maximus() : CreatureScript("boss_maximus") { }

	struct boss_maximusAI : public BossAI
	{
		boss_maximusAI(Creature* c) : BossAI(c, DATA_MAXIMUS)
		{
			instance = me->GetInstanceScript();
		}

		void Reset() override
		{
			events.Reset();
			me->SetReactState(REACT_AGGRESSIVE);
			me->SetStandState(UNIT_STAND_STATE_STAND);
			SetCombatMovement(true);
			fabulousFlame1Count = 0;
			fabulousFlame2Count = 0;
			crimsonFlameCount = 0;
			isValidToSpawnFlame = false;
			drawFlames = false;
			drawedFlames = false;
			preStartFabulousFlames1 = false;
			preStartFabulousFlames2 = false;
			startedFabulousFlames1 = false;
			startedFabulousFlames2 = false;
			belowNintyPercent = false;
			belowFiftyPercent = false;
			belowTwentyfivePercent = false;
			belowTenPercent = false;
			startedCorruption = false;
			corrupted = false;
			maiev = false;
			enraged = false;
		}

		void EnterCombat(Unit* /*who*/) override
		{
			instance->SetBossState(DATA_MAXIMUS, IN_PROGRESS);
			DoPlaySoundToSet(me, SOUND_ONAGGRO);
			me->Yell(YELL_ON_ENTER_COMBAT, LANG_UNIVERSAL);
			events.ScheduleEvent(EVENT_AURA_OF_SHADOWS, 5000);
			events.ScheduleEvent(EVENT_SUNDER_ARMOR, 7500);
			events.ScheduleEvent(EVENT_MARK_FOR_FLAME, 10000);
			events.ScheduleEvent(EVENT_METEOR_FISTS, 20000);
			events.ScheduleEvent(EVENT_SUMMON_TSUNAMI, 35000);
			events.ScheduleEvent(EVENT_INCREASE_STRENGTH, 45000);
			events.ScheduleEvent(EVENT_ENRAGE, 480000);
			for (uint8 i = 0; i < 3; ++i)
				me->SummonCreature(NPC_VOLCANO_2, VolcanoSpawnPos[i].x, VolcanoSpawnPos[i].y, VolcanoSpawnPos[i].z);
		}

		void JustDied(Unit* /*killer*/) override
		{
			instance->SetBossState(DATA_MAXIMUS, DONE);
			DoPlaySoundToSet(me, SOUND_ONDEATH);
			me->Yell(YELL_ON_DEATH, LANG_UNIVERSAL);
			summons.DespawnEntry(NPC_VOLCANO_2);
		}

		void KilledUnit(Unit* /*victim*/) override
		{
			DoPlaySoundToSet(me, SOUND_ONSLAY);
			me->Yell(YELL_ON_KILLED_UNIT, LANG_UNIVERSAL);
		}

		void EnterEvadeMode() override
		{
			me->m_Events.AddEvent(new MaximusDespawner(me), me->m_Events.CalculateTime(1));
		}

		void JustSummoned(Creature* summoned) override
		{
			summons.Summon(summoned);
			switch (summoned->GetEntry())
			{
			case NPC_VOLCANO_2:
				summoned->AI()->DoZoneInCombat(0, 333.0f);
				break;
			case NPC_MAIEV_SHADOWSONG:
				summoned->AI()->AttackStart(me);
				break;
			}
		}

		void SpellHitTarget(Unit* target, const SpellInfo* spell) override
		{
			if (spell->GetMaxRange() == 45)
			{
				target->CastSpell(0, SPELL_FLAME_HIT_VISUAL_1);
				target->CastSpell(0, SPELL_FLAME_HIT_VISUAL_2);
				me->SummonCreature(NPC_CRIMSON_FLAME, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
				++crimsonFlameCount;
			}
		}

		void TargetSelector(uint32 selectorType, Unit* target, float minDist, float maxDist, uint32 spellId)
		{
			/* 1 = crimson flame, 2 = mark for flame */
			std::list<HostileReference*> tList = me->getThreatManager().getThreatList();
			std::vector<Unit*> targetList;
			for (std::list<HostileReference*>::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
			{
				target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid());
				if (!target)
					continue;
				if (target->GetTypeId() == TYPEID_PLAYER && target->IsAlive() && !target->IsWithinDist(me, minDist, false))
					targetList.push_back(target);
				if (selectorType == 1 && !me->IsWithinDist(target, maxDist, false))
					targetList.push_back(target);
				if (selectorType == 2 && target->HasAura(spellId))
					targetList.push_back(target);
				target = NULL;
			}

			if (!targetList.empty())
				target = *(targetList.begin() + rand() % targetList.size());
			else
				target = me->GetVictim();

			if (target)
				DoCast(target, spellId);
		}

		void SummonTsunami()
		{
			int randIndex = rand() % 3;
			if (Creature* tsunami = me->SummonCreature(NPC_SMOLDERING_TSUNAMI_MC, TsunamiSpawnPos[randIndex].x, TsunamiSpawnPos[randIndex].y, TsunamiSpawnPos[randIndex].z))
			{
				if (randIndex == 0)
					tsunami->GetMotionMaster()->MovePoint(0, TsunamiOne.x, TsunamiOne.y, TsunamiOne.z);
				else if (randIndex == 1)
				{
					if (!corrupted && !enraged && !maiev)
						tsunami->GetMotionMaster()->MovePoint(0, TsunamiTwo.x, TsunamiTwo.y, TsunamiTwo.z);
					else
						SummonTsunami();
				}
				else if (randIndex == 2)
					tsunami->GetMotionMaster()->MovePoint(0, TsunamiThree.x, TsunamiThree.y, TsunamiThree.z);
			}
		}

		void DrawFlames()
		{
			std::list<Creature*> creatureList;
			GetCreatureListWithEntryInGrid(creatureList, me, NPC_CRIMSON_FLAME, 1000.0f);

			if (creatureList.empty())
				return;

			for (std::list<Creature*>::const_iterator itr = creatureList.begin(); itr != creatureList.end(); ++itr)
			{
				(*itr)->GetMotionMaster()->Clear();
				(*itr)->GetMotionMaster()->MoveChase(me);
				(*itr)->AI()->DoAction(ACTION_DRAW_FLAME);
			}
		}

		void FlameSearch()
		{
			std::list<Creature*> FlameList;
			Trinity::AllCreaturesOfEntryInRange checker(me, NPC_CRIMSON_FLAME, 1000.0f);
			Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange> searcher(me, FlameList, checker);

			me->VisitNearbyObject(1000.0f, searcher);

			if (FlameList.empty())
				drawedFlames = true;
		}

		void FabulousFlames()
		{
			Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true, SPELL_MARK_FOR_FLAME);
			if (target)
			{
				DoCast(target, SPELL_FABULOUS_FLAME);
				if (fabulousFlame1Count != 1000)
					++fabulousFlame1Count;
				else if (fabulousFlame2Count != 1000)
					++fabulousFlame2Count;
				events.ScheduleEvent(EVENT_FABULOUS_FLAMES, 2000);
			}
			else
				DoAction(ACTION_END_FABULOUS_FLAMES);
		}

		void CorruptedBlood(float distance, uint32 castSpell)
		{
			std::list<Player*> playerList;
			Trinity::AnyPlayerInObjectRangeCheck checker(me, distance);
			Trinity::PlayerListSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(me, playerList, checker);

			me->VisitNearbyWorldObject(distance, searcher);

			for (std::list<Player*>::iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
			{
				if (castSpell == SPELL_CORRUPTED_BLOOD)
					(*itr)->CastSpell((*itr), castSpell);
				else
					me->CastSpell((*itr), castSpell);
			}
		}

		void DoAction(int32 action) override
		{
			switch (action)
			{
			case ACTION_PRE_START_FABULOUS_FLAMES:
				if (!startedFabulousFlames1 && !preStartFabulousFlames1)
				{
					preStartFabulousFlames1 = true;
					me->AttackStop();
					me->SetReactState(REACT_PASSIVE);
					me->RemoveAura(SPELL_INCREASE_STRENGTH);
					me->RemoveAura(SPELL_METEOR_FISTS);
					me->SummonCreature(NPC_POOL_OF_LIGHT, PoolSpawn.x, PoolSpawn.y, PoolSpawn.z);
					me->GetMotionMaster()->MovePoint(0, MaximusFFPos.x, MaximusFFPos.y, MaximusFFPos.z);
					events.CancelEvent(EVENT_CALL_FLAME);
					events.CancelEvent(EVENT_INCREASE_STRENGTH);
					events.CancelEvent(EVENT_AURA_OF_SHADOWS);
					events.CancelEvent(EVENT_SUNDER_ARMOR);
					events.CancelEvent(EVENT_METEOR_FISTS);
					events.CancelEvent(EVENT_MARK_FOR_FLAME);
				}
				else if (!startedFabulousFlames2 && !preStartFabulousFlames2)
				{
					preStartFabulousFlames2 = true;
					corrupted = false;
					startedCorruption = false;
					me->TextEmote(EMOTE_FABULOUS_FLAMES, 0, true);
					me->SetStandState(UNIT_STAND_STATE_STAND);
					me->RemoveAura(SPELL_CORRUPTED_BLOOD_VISUAL);
					summons.DespawnEntry(NPC_POOL_OF_LIGHT);
					me->SummonCreature(NPC_POOL_OF_LIGHT, PoolSpawn.x, PoolSpawn.y, PoolSpawn.z);
					me->GetMotionMaster()->MovePoint(0, MaximusFFPos.x, MaximusFFPos.y, MaximusFFPos.z);
					events.CancelEvent(EVENT_START_CORRUPTED_BLOOD);
					events.CancelEvent(EVENT_CORRUPTED_BLOOD_GRAVITATION);
					events.CancelEvent(EVENT_CORRUPTED_BLOOD);
					events.CancelEvent(EVENT_MARK_FOR_FLAME);
				}
				break;
			case ACTION_DRAW_FLAMES:
				drawFlames = true;
				me->TextEmote(EMOTE_DRAW_FLAMES, 0, true);
				me->AddAura(SPELL_DRAW_FLAMES_CHANNEL_VISUAL, me);
				me->SetFacingTo(0.743379f);
				DrawFlames();
				events.ScheduleEvent(EVENT_DRAW_FLAMES, 1);
				break;
			case ACTION_START_FABULOUS_FLAMES:
				if (!startedFabulousFlames1)
				{
					startedFabulousFlames1 = true;
					me->RemoveAura(SPELL_DRAW_FLAMES_CHANNEL_VISUAL);
					events.CancelEvent(EVENT_DRAW_FLAMES);
				}
				else if (!startedFabulousFlames2)
					startedFabulousFlames2 = true;
				me->SetFacingTo(3.078448f);
				events.ScheduleEvent(EVENT_FABULOUS_FLAMES, 1500);
				break;
			case ACTION_END_FABULOUS_FLAMES:
				events.CancelEvent(EVENT_FABULOUS_FLAMES);
				if (fabulousFlame1Count != 1000 && fabulousFlame1Count >= crimsonFlameCount)
				{
					fabulousFlame1Count = 1000;
					events.ScheduleEvent(EVENT_MARK_FOR_FLAME, 5000);
				}
				else if (fabulousFlame2Count != 1000 && fabulousFlame1Count >= crimsonFlameCount/2)
					fabulousFlame2Count = 1000;
				summons.DespawnEntry(NPC_POOL_OF_LIGHT);
				events.ScheduleEvent(EVENT_START_CORRUPTED_BLOOD, 1500);
				break;
			case ACTION_CORRUPT_BLOOD:
				startedCorruption = false;
				corrupted = true;
				me->SetFacingTo(3.833134f);
				CorruptedBlood(1000.0f, SPELL_CORRUPTED_BLOOD_GRAVITATION);
				events.ScheduleEvent(EVENT_CORRUPTED_BLOOD, 2500);
				events.ScheduleEvent(EVENT_CORRUPTED_BLOOD_GRAVITATION, 15000);
				break;
			case ACTION_MAIEV:
				maiev = true;
				corrupted = false;
				startedCorruption = false;
				me->RemoveAllAuras();
				me->SetStandState(UNIT_STAND_STATE_STAND);
				me->SummonCreature(NPC_MAIEV_SHADOWSONG, MaievSpawn.x, MaievSpawn.y, MaievSpawn.z);
				DoCast(SPELL_SHADOW_PRISON);
				summons.DespawnEntry(NPC_POOL_OF_LIGHT);
				summons.DespawnEntry(NPC_CRIMSON_FLAME);
				events.CancelEvent(EVENT_START_CORRUPTED_BLOOD);
				events.CancelEvent(EVENT_CORRUPTED_BLOOD);
				events.CancelEvent(EVENT_CORRUPTED_BLOOD_GRAVITATION);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_FOR_FLAME);
				break;
			case ACTION_ENRAGE:
				enraged = true;
				corrupted = false;
				startedCorruption = false;
				DoPlaySoundToSet(me, SOUND_ONBERSERK);
				me->TextEmote(EMOTE_ENRAGE, 0, true);
				me->Yell(YELL_ON_ENRAGE, LANG_UNIVERSAL);
				me->SetReactState(REACT_AGGRESSIVE);
				me->RemoveAllAuras();
				me->SetStandState(UNIT_STAND_STATE_STAND);
				summons.DespawnEntry(NPC_POOL_OF_LIGHT);
				DoCast(SPELL_ENRAGE_VISUAL);
				DoCast(SPELL_ENRAGE);
				SetCombatMovement(false);
				events.CancelEvent(EVENT_AURA_OF_SHADOWS);
				events.CancelEvent(EVENT_SUNDER_ARMOR);
				events.CancelEvent(EVENT_MARK_FOR_FLAME);
				events.CancelEvent(EVENT_METEOR_FISTS);
				events.CancelEvent(EVENT_INCREASE_STRENGTH);
				events.CancelEvent(EVENT_CALL_FLAME);
				events.CancelEvent(EVENT_DRAW_FLAMES);
				events.CancelEvent(EVENT_MARK_FOR_FLAME);
				events.CancelEvent(EVENT_FABULOUS_FLAMES);
				events.CancelEvent(EVENT_START_CORRUPTED_BLOOD);
				events.CancelEvent(EVENT_CORRUPTED_BLOOD_GRAVITATION);
				events.CancelEvent(EVENT_CORRUPTED_BLOOD);
				events.ScheduleEvent(EVENT_FINGER_OF_DEATH, 3000);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_FOR_FLAME);
				break;
			}
		}

		void DamageTaken(Unit* attacker, uint32& damage) override
		{
			if (me->HealthBelowPctDamaged(1, damage))
				if (damage > me->GetHealth())
					instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_FOR_FLAME);
			if (me->HealthBelowPctDamaged(90, damage) && !belowNintyPercent && !isValidToSpawnFlame)
				belowNintyPercent = true;
			if (me->HealthBelowPctDamaged(50, damage) && !belowFiftyPercent && !enraged)
				belowFiftyPercent = true;
			if (me->HealthBelowPctDamaged(25, damage) && !belowTwentyfivePercent && !enraged)
				belowTwentyfivePercent = true;
			if (me->HealthBelowPctDamaged(10, damage) && !belowTenPercent && !enraged)
				belowTenPercent = true;
		}

		void UpdateAI(uint32 diff) override
		{
			if (!UpdateVictim())
				return;

			if (belowNintyPercent && !isValidToSpawnFlame)
			{
				isValidToSpawnFlame = true;
				if (!enraged)
					events.ScheduleEvent(EVENT_CALL_FLAME, 1);
			}

			if (belowFiftyPercent && !belowTwentyfivePercent && !belowTenPercent && !enraged)
			{
				if (!preStartFabulousFlames1)
					DoAction(ACTION_PRE_START_FABULOUS_FLAMES);
				if (!drawFlames && !me->isMoving())
					DoAction(ACTION_DRAW_FLAMES);
				if (me->HasAura(SPELL_DRAW_FLAMES_CHANNEL_VISUAL))
					me->HandleEmoteCommand(EMOTE_STATE_SPELL_CHANNEL_OMNI);
				if (drawedFlames && !startedFabulousFlames1 && !me->isMoving())
					DoAction(ACTION_START_FABULOUS_FLAMES);
				if (fabulousFlame1Count != 1000 && fabulousFlame1Count >= crimsonFlameCount)
					instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_FOR_FLAME);
			}

			if (belowTwentyfivePercent && !belowTenPercent && !enraged)
			{
				if (!preStartFabulousFlames2)
					DoAction(ACTION_PRE_START_FABULOUS_FLAMES);
				if (!startedFabulousFlames2 && !me->isMoving())
					DoAction(ACTION_START_FABULOUS_FLAMES);
				if (fabulousFlame2Count != 1000 && fabulousFlame2Count >= crimsonFlameCount / 2)
					instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_FOR_FLAME);
			}

			if (startedCorruption && !me->isMoving() && !belowTenPercent && !enraged)
				DoAction(ACTION_CORRUPT_BLOOD);
			if (corrupted && !enraged)
				me->SetStandState(UNIT_STAND_STATE_KNEEL);

			if (belowTenPercent && !enraged)
			{
				if (!maiev)
					DoAction(ACTION_MAIEV);
				if (!me->HasAura(SPELL_SHADOW_PRISON))
					DoCast(SPELL_SHADOW_PRISON);
				if (me->GetVictim())
					me->m_Events.AddEvent(new MaximusDespawner(me), me->m_Events.CalculateTime(1));
			}

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_AURA_OF_SHADOWS:
					DoCastAOE(SPELL_AURA_OF_SHADOWS);
					events.ScheduleEvent(EVENT_AURA_OF_SHADOWS, 2000);
					break;
				case EVENT_SUNDER_ARMOR:
					DoCastVictim(SPELL_SUNDER_ARMOR);
					events.ScheduleEvent(EVENT_SUNDER_ARMOR, 7500);
					break;
				case EVENT_METEOR_FISTS:
					DoCast(SPELL_METEOR_FISTS);
					events.ScheduleEvent(EVENT_METEOR_FISTS, 30000);
					break;
				case EVENT_INCREASE_STRENGTH:
					DoCast(SPELL_INCREASE_STRENGTH);
					me->TextEmote(EMOTE_STRENGTH, 0, true);
					events.ScheduleEvent(EVENT_INCREASE_STRENGTH, 40000);
					break;
				case EVENT_MARK_FOR_FLAME:
					TargetSelector(2, NULL, 12.5f, 0.0f, SPELL_MARK_FOR_FLAME);
					if (!startedFabulousFlames1)
						events.ScheduleEvent(EVENT_MARK_FOR_FLAME, 38000);
					else if (!startedFabulousFlames2)
						events.ScheduleEvent(EVENT_MARK_FOR_FLAME, 14250);
					break;
				case EVENT_SUMMON_TSUNAMI:
					SummonTsunami();
					me->TextEmote(EMOTE_TSUNAMI, 0, true);
					events.ScheduleEvent(EVENT_SUMMON_TSUNAMI, 30000);
					break;
				case EVENT_CALL_FLAME:
					TargetSelector(1, NULL, 12.5f, 45.0f, SPELL_FLAME_TRAVEL);
					events.ScheduleEvent(EVENT_CALL_FLAME, 13715);
					break;
				case EVENT_DRAW_FLAMES:
					FlameSearch();
					events.ScheduleEvent(EVENT_DRAW_FLAMES, 250);
					break;
				case EVENT_FABULOUS_FLAMES:
					FabulousFlames();
					break;
				case EVENT_START_CORRUPTED_BLOOD:
					DoCast(SPELL_CORRUPTED_BLOOD_VISUAL);
					me->GetMotionMaster()->MovePoint(0, MaximusCBPos.x, MaximusCBPos.y, MaximusCBPos.z);
					startedCorruption = true;
					break;
				case EVENT_CORRUPTED_BLOOD:
					CorruptedBlood(7.5f, SPELL_CORRUPTED_BLOOD);
					events.ScheduleEvent(EVENT_CORRUPTED_BLOOD, 2000);
					break;
				case EVENT_CORRUPTED_BLOOD_GRAVITATION:
					CorruptedBlood(1000.0f, SPELL_CORRUPTED_BLOOD_GRAVITATION);
					events.ScheduleEvent(EVENT_CORRUPTED_BLOOD_GRAVITATION, 15000);
					break;
				case EVENT_ENRAGE:
					DoAction(ACTION_ENRAGE);
					break;
				case EVENT_FINGER_OF_DEATH:
					DoCastAOE(SPELL_FINGER_OF_DEATH);
					events.ScheduleEvent(EVENT_FINGER_OF_DEATH, 1750);
					break;
				}
			}

			if (!belowFiftyPercent)
				DoMeleeAttackIfReady();
		}

	private:
		InstanceScript* instance;
		EventMap events;
		uint32 fabulousFlame1Count;
		uint32 fabulousFlame2Count;
		uint32 crimsonFlameCount;
		bool haltMovment;
		bool isValidToSpawnFlame;
		bool drawFlames;
		bool drawedFlames;
		bool preStartFabulousFlames1;
		bool preStartFabulousFlames2;
		bool startedFabulousFlames1;
		bool startedFabulousFlames2;
		bool belowNintyPercent;
		bool belowFiftyPercent;
		bool belowTwentyfivePercent;
		bool belowTenPercent;
		bool startedCorruption;
		bool corrupted;
		bool maiev;
		bool enraged;
	};

	CreatureAI* GetAI(Creature* c) const override
	{
		return GetMoltenCoreAI<boss_maximusAI>(c);
	}
};

class npc_volcano_2 : public CreatureScript
{
public:
	npc_volcano_2() : CreatureScript("npc_volcano_2") { }

	struct npc_volcano_2AI : public ScriptedAI
	{
		npc_volcano_2AI(Creature* c) : ScriptedAI(c)
		{
			events.Reset();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
		}

		void EnterCombat(Unit* /*who*/) override
		{
			events.ScheduleEvent(EVENT_VOLCANO_ERUPTION, urand(12500, 37500));
		}

		void UpdateAI(uint32 diff) override
		{
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_VOLCANO_ERUPTION:
					if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 333.0f, true))
						if (me->IsWithinLOS(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ()))
							DoCast(target, SPELL_VOLCANO_ERUPTION);
					events.ScheduleEvent(EVENT_VOLCANO_ERUPTION, urand(10000, 30000));
					break;
				}
			}
		}

	private:
		EventMap events;
	};

	CreatureAI* GetAI(Creature* c) const override
	{
		return GetMoltenCoreAI<npc_volcano_2AI>(c);
	}
};

class npc_smoldering_tsunami_mc : public CreatureScript
{
public:
	npc_smoldering_tsunami_mc() : CreatureScript("npc_smoldering_tsunami_mc") { }

	struct npc_smoldering_tsunami_mcAI : public PassiveAI
	{
		npc_smoldering_tsunami_mcAI(Creature* c) : PassiveAI(c)
		{
			events.Reset();
			me->AddAura(SPELL_TSUNAMI_VISUAL, me);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
			events.ScheduleEvent(EVENT_TSUNAMI_PERIODIC_DMG, 1000);
		}

		void SpellHitTarget(Unit* target, const SpellInfo* spell) override
		{
			if (spell->Id == SPELL_TSUNAMI_PERIODIC_DMG)
				if (!target->IsTotem())
					if (float orientation = target->GetOrientation())
					{
						target->SetOrientation(me->GetOrientation());
						target->CastSpell(0, SPELL_TSUNAMI_LEAP);
						target->SetOrientation(orientation);
					}
		}

		void UpdateAI(uint32 diff) override
		{
			if (!me->isMoving())
			{
				me->RemoveAllAuras();
				me->DespawnOrUnsummon(1000);
			}

			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_TSUNAMI_PERIODIC_DMG:
					DoCast(SPELL_TSUNAMI_PERIODIC_DMG);
					events.ScheduleEvent(EVENT_TSUNAMI_PERIODIC_DMG, 375);
					break;
				}
			}
		}

	private:
		EventMap events;
	};

	CreatureAI* GetAI(Creature* c) const override
	{
		return GetMoltenCoreAI<npc_smoldering_tsunami_mcAI>(c);
	}
};

class npc_crimson_flame : public CreatureScript
{
public:
	npc_crimson_flame() : CreatureScript("npc_crimson_flame") { }

	struct npc_crimson_flameAI : public PassiveAI
	{
		npc_crimson_flameAI(Creature* c) : PassiveAI(c)
		{
			events.Reset();
			events.ScheduleEvent(EVENT_FLAME_GROWTH, 20000);
			me->AddAura(SPELL_FLAME_VISUAL, me);
			me->GetMotionMaster()->MoveRandom(10.0f);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
			validForMovementCheck = false;
		}

		void DoAction(int32 action) override
		{
			switch (action)
			{
			case ACTION_DRAW_FLAME:
				events.CancelEvent(EVENT_FLAME_GROWTH);
				validForMovementCheck = true;
				break;
			}
		}

		void UpdateAI(uint32 diff) override
		{
			if (validForMovementCheck && !me->isMoving())
				me->DespawnOrUnsummon(1000);

			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_FLAME_GROWTH:
					DoCast(SPELL_FLAME_GROWTH);
					events.ScheduleEvent(EVENT_FLAME_GROWTH, 20000);
					break;
				}
			}
		}

	private:
		EventMap events;
		bool validForMovementCheck;
	};

	CreatureAI* GetAI(Creature* c) const override
	{
		return GetMoltenCoreAI<npc_crimson_flameAI>(c);
	}
};

class npc_pool_of_light : public CreatureScript
{
public:
	npc_pool_of_light() : CreatureScript("npc_pool_of_light") { }

	struct npc_pool_of_lightAI : public ScriptedAI
	{
		npc_pool_of_lightAI(Creature* c) : ScriptedAI(c)
		{
			events.Reset();
			events.ScheduleEvent(EVENT_POOL_HEAL, 2000);
			events.ScheduleEvent(EVENT_POOL_MANA, 2000);
			me->AddAura(SPELL_POOL_VISUAL_1, me);
			me->AddAura(SPELL_POOL_VISUAL_2, me);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		}

		void HealSelector(float distance, uint32 requiredAura, uint32 castSpell)
		{
			std::list<Player*> playerList;
			Trinity::AnyPlayerInObjectRangeCheck checker(me, distance);
			Trinity::PlayerListSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(me, playerList, checker);

			me->VisitNearbyWorldObject(distance, searcher);

			for (std::list<Player*>::iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
				if ((*itr)->HasAura(requiredAura))
					(*itr)->CastSpell((*itr), castSpell);
		}

		void UpdateAI(uint32 diff) override
		{
			if (!me->HasAura(SPELL_POOL_VISUAL_2))
				me->AddAura(SPELL_POOL_VISUAL_2, me);

			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_POOL_HEAL:
					HealSelector(5.0f, SPELL_MARK_FOR_FLAME, SPELL_POOL_HEAL);
					events.ScheduleEvent(EVENT_POOL_HEAL, 750);
					break;
				case EVENT_POOL_MANA:
					HealSelector(5.0f, SPELL_MARK_FOR_FLAME, SPELL_POOL_MANA);
					events.ScheduleEvent(EVENT_POOL_MANA, 7000);
					break;
				}
			}
		}

	private:
		EventMap events;
	};

	CreatureAI* GetAI(Creature* c) const override
	{
		return GetMoltenCoreAI<npc_pool_of_lightAI>(c);
	}
};

class npc_maiev_shadowsong : public CreatureScript
{
public:
	npc_maiev_shadowsong() : CreatureScript("npc_maiev_shadowsong") { }

	struct npc_maiev_shadowsongAI : public ScriptedAI
	{
		npc_maiev_shadowsongAI(Creature* c) : ScriptedAI(c)
		{
			instance = me->GetInstanceScript();
			events.Reset();
			events.ScheduleEvent(EVENT_MAIEV_SAY, 1);
			events.ScheduleEvent(EVENT_MAIEV_TELEPORT, 6750);
			events.ScheduleEvent(EVENT_MAIEV_PROTECTION_EMOTE, 8000);
			events.ScheduleEvent(EVENT_MAIEV_PROTECTION, 8000);
		}

		void BlinkTo(float x, float y, float z)
		{
			me->GetMotionMaster()->Clear(false);
			DoTeleportTo(x, y, z);
			DoCast(SPELL_MAIEV_TELEPORT_VISUAL);
		}

		void UpdateAI(uint32 diff) override
		{
			events.Update(diff);

			if (instance->GetBossState(DATA_MAXIMUS) == DONE)
				events.CancelEvent(EVENT_MAIEV_PROTECTION);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_MAIEV_SAY:
					me->Yell(YELL_MAIEV_APPEAR, LANG_UNIVERSAL);
					DoPlaySoundToSet(me, SOUND_ONMAIEV);
					break;
				case EVENT_MAIEV_PROTECTION_EMOTE:
					me->TextEmote(EMOTE_MAIEV_PROTECTION, 0, true);
					break;
				case EVENT_MAIEV_TELEPORT:
					if (Unit* target = me->GetVictim())
						BlinkTo(target->GetPositionX(), target->GetPositionY()-7.5f, target->GetPositionZ());
					break;
				case EVENT_MAIEV_PROTECTION:
					DoCast(SPELL_MAIEV_PROTECTION);
					events.ScheduleEvent(EVENT_MAIEV_PROTECTION, 30000);
					break;
				}
			}

			DoMeleeAttackIfReady();
		}

	private:
		InstanceScript* instance;
		EventMap events;
	};

	CreatureAI* GetAI(Creature* c) const override
	{
		return GetMoltenCoreAI<npc_maiev_shadowsongAI>(c);
	}
};

void AddSC_boss_maximus()
{
	new boss_maximus();
	new npc_volcano_2();
	new npc_smoldering_tsunami_mc();
	new npc_crimson_flame();
	new npc_pool_of_light();
	new npc_maiev_shadowsong();
}