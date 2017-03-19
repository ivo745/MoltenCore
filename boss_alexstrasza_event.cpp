/* ScriptData
SDCreator: frostI
SDName: boss_alexstrasza_event
SD%Complete: 100
SDComment:
SDCategory: Molten Core
EndScriptData */

#include "ScriptPCH.h"
#include "molten_core.h"

enum Spells
{
	//Corrupted Druids
	SPELL_WELL_OF_CORRUPTION = 72362,
	SPELL_CORRUPT_SOUL = 68839,
	SPELL_SHADOW_CLEAVE = 70670,
	SPELL_CORRUPTING_DEVOTION_AURA = 38603,
	//Alexstrasza
	SPELL_CORRUPTION_CHANNEL = 76221,
	SPELL_CORRUPTION_ALEXSTRASZA = 70904,
	SPELL_COPY_DAMAGE = 71948,
	SPELL_DREAM_SLIP = 71196,
	SPELL_CLEAR_ALL = 71721,
	//Alexstrasza Controller
	SPELL_SPAWN_CHEST = 71207,
	//Malfurion Portal
	SPELL_MALFURION_PORTAL = 51807,
	SPELL_MALFURION_ROOT = 58540,
	//Malfurion
	SPELL_MIND_CONTROL = 36797,
	SPELL_TWILIGHT_CUTTER = 74768,
	SPELL_MALFURION_ESCAPE = 66296,
	//Arch Druid Lathorius
	SPELL_ARCANE_STORM = 61694,
	SPELL_LIGHT_OF_ELUNE = 6724,
	//Arch Druid Lilliandra
	SPELL_MOONFIRE = 67946,
	SPELL_REJUVENATION = 71142,
	//Arch Druid Staghelm
	SPELL_FLAME_WAVE = 75413,
	SPELL_REGROWTH = 67970,
	//Protector of the Keep
	SPELL_CURSE_OF_AGONY = 68138,
	//Guardian of the Secrets
	SPELL_ENRAGE = 61369,
	SPELL_EMPOWERED_SHADOW_LANCE = 71815,
	SPELL_SHADOW_MEND = 71107,
	//Fabulous Distractor
	SPELL_SHADOWSTEP = 72327,
	SPELL_GRAVITY_BOMB = 63024,
	SPELL_FEAR = 29321,

};

enum Events
{
	//Alexstrasza
	EVENT_ALEXSTRASZA_UNDER_80PERCENT_TALK = 1,
	EVENT_DREAM_SLIP = 2,
	EVENT_PORTAL_SUMMONS = 3,
	//Trigger
	EVENT_SUMMON_PROTECTOR_OF_THE_KEEP = 4,
	EVENT_SUMMON_GUARDIAN_OF_THE_SECRETS = 5,
	EVENT_SUMMON_FABULOUS_DISTRACTOR = 6,
	//Corrupted Druids
	EVENT_CORRUPTING_DEVOTION_AURA = 7,
	EVENT_SHADOW_CLEAVE = 8,
	EVENT_WELL_OF_CORRUPTION = 9,
	EVENT_CORRUPT_SOUL = 10,
	//Malfurion
	EVENT_MALFURION_ROOT = 11,
	EVENT_MALFURION_WALK = 12,
	EVENT_MALFURION_WALK_END = 13,
	EVENT_MALFURION_TALK_1 = 14,
	EVENT_MALFURION_TALK_2 = 15,
	EVENT_MALFURION_TALK_3 = 16,
	EVENT_MIND_CONTROL = 17,
	EVENT_TWILIGHT_CUTTER = 18,
	EVENT_MALFURION_ESCAPE = 19,
	//Arch Druid Lathorius
	EVENT_LATHORIUS_WALK_LEFT = 20,
	EVENT_LATHORIUS_WALK_RIGHT = 21,
	EVENT_ARCANE_STORM = 22,
	EVENT_LIGHT_OF_ELUNE = 23,
	//Arch Druid Lilliandra
	EVENT_LILLIANDRA_WALK_LEFT = 24,
	EVENT_LILLIANDRA_WALK_RIGHT = 25,
	EVENT_REJUVENATION = 26,
	EVENT_MOONFIRE = 27,
	//Arch Druid Staghelm
	EVENT_STAGHELM_WALK_LEFT = 28,
	EVENT_STAGHELM_WALK_RIGHT = 29,
	EVENT_REGROWTH = 30,
	EVENT_FLAME_WAVE = 31,
	//Protector of the Keep
	EVENT_CURSE_OF_AGONY = 32,
	//Guardian of the Secrets
	EVENT_EMPOWERED_SHADOW_LANCE = 33,
	EVENT_SHADOW_MEND = 34,
	EVENT_ENRAGE = 35,
	//Fabulous Distractor
	EVENT_SHADOWSTEP = 36,
	EVENT_FEAR = 37,
	EVENT_GRAVITY_BOMB = 38,
	//General events for adds
	EVENT_ENABLE_COMBAT = 39,
	EVENT_ADDS_MOVE_1 = 40,
	EVENT_ADDS_MOVE_2 = 41,
	EVENT_ADDS_MOVE_3 = 42,
	EVENT_ADDS_MOVE_4 = 43,
	EVENT_ADDS_MOVE_5 = 44,
};

enum Actions
{
	ACTION_ENTER_COMBAT = 1,
	ACTION_DEATH = 2,
	ACTION_DO_ADDS_WAYPOINT = 3,
	ACTION_CANCEL_ADDS = 4,
};

struct Location
{
	float x, y, z;
};

Position const AddsSpawnPos = { 777.598267f, -1132.265015f, -148.477966f };
Position const AddsMove1 = { 750.437988f, -1122.542725f, -144.177414f };
Position const AddsMove2 = { 711.523865f, -1128.354370f, -138.217300f };
Position const AddsMove3 = { 681.325562f, -1166.764648f, -126.760422f };
Position const AddsMove4 = { 680.855774f, -1187.722656f, -121.878876f };
Position const AddsMove5 = { 715.232422f, -1184.140381f, -118.967003f };

Position const MalfurionPortalSpawnPos = { 752.358093f, -1227.237305f, -119.492310f };
Position const MalfurionMovePos = { 748.394836f, -1217.826416f, -118.904053f };
Position const LathoriusMovePosLeft = { 746.457581f, -1221.109253f, -119.233856f };
Position const LilliandraMovePosLeft = { 744.582703f, -1223.067749f, -119.462875f };
Position const StaghelmMovePosLeft = { 743.545898f, -1225.765381f, -119.737206f };
Position const LathoriusMovePosRight = { 751.651917f, -1219.223877f, -119.214256f };
Position const LilliandraMovePosRight = { 754.707947f, -1218.894775f, -119.389717f };
Position const StaghelmMovePosRight = { 757.645081f, -1219.902344f, -119.649155f };
Position const AlexstraszaSpawnPos1 = { 736.859009f, -1176.910034f, -118.894997f };

#define SOUND_ALEXSTRASZA_UNDER_80PERCENT     17490
#define SOUND_ALEXSTRASZA_100PERCENT          17491
#define SOUND_MALFURION_INTRO_1               14033
#define SOUND_MALFURION_INTRO_2               14038

class CorruptedDruidCheck
{
public:
	bool operator()(Creature* creature)
	{
		return creature->IsAlive() && creature->GetEntry() == NPC_CORRUPTED_DRUID &&
			creature->GetDBTableGUIDLow() && !creature->IsInCombat();
	}
};

class AlexstraszaDespawner : public BasicEvent
{
public:
	explicit AlexstraszaDespawner(Creature* creature) : creature(creature){ }

	bool Execute(uint64 /*currTime*/, uint32 /*diff*/)
	{
		Trinity::CreatureWorker<AlexstraszaDespawner> worker(creature, *this);
		creature->VisitNearbyGridObject(600.0f, worker);
		return true;
	}

	void operator()(Creature* creature) const
	{
		switch (creature->GetEntry())
		{
		case NPC_ALEXSTRASZA:
			if (InstanceScript* instance = creature->GetInstanceScript())
				instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, creature);
			break;
		case NPC_MALFURION_PORTAL:
		case NPC_MALFURION:
		case NPC_LATHORIUS_RIGHT:
		case NPC_LILLIANDRA_RIGHT:
		case NPC_STAGHELM_RIGHT:
		case NPC_LATHORIUS_LEFT:
		case NPC_LILLIANDRA_LEFT:
		case NPC_STAGHELM_LEFT:
		case NPC_PROTECTOR_OF_THE_KEEP:
		case NPC_GUARDIAN_OF_THE_SECRETS:
		case NPC_FABULOUS_DISTRACTOR:
			creature->DespawnOrUnsummon(0);
			return;
		case NPC_CORRUPTED_DRUID:
		case NPC_ALEXSTRASZA_CONTROLLER:
		case NPC_RED_DRAGON_COMBAT_TRIGGER:
		case NPC_THRALL_2:
			if (!creature->GetDBTableGUIDLow())
			{
				creature->DespawnOrUnsummon(0);
				return;
			}
			creature->Respawn(true);
			break;
		default:
			return;
		}

		uint32 corpseDelay = creature->GetCorpseDelay();
		uint32 respawnDelay = creature->GetRespawnDelay();
		creature->SetCorpseDelay(1);
		creature->SetRespawnDelay(15);

		if (CreatureData const* data = creature->GetCreatureData())
			creature->SetPosition(data->posX, data->posY, data->posZ, data->orientation);

		creature->DespawnOrUnsummon();
		creature->SetCorpseDelay(corpseDelay);
		creature->SetRespawnDelay(respawnDelay);
	}

private:
	Creature* creature;
};

class boss_alexstrasza : public CreatureScript
{
public:
	boss_alexstrasza() : CreatureScript("boss_alexstrasza"){ }

	struct boss_alexstraszaAI : public ScriptedAI
	{
		boss_alexstraszaAI(Creature* creature) : ScriptedAI(creature), summons(me)
		{
			instance = creature->GetInstanceScript();
		}

		void InitializeAI()
		{
			if (CreatureData const* data = sObjectMgr->GetCreatureData(me->GetDBTableGUIDLow()))
				if (data->curhealth)
					spawnHealth = data->curhealth;

			if (!me->isDead())
				Reset();
		}

		void Reset()
		{
			events.Reset();
			me->SetHealth(spawnHealth);
			me->SetReactState(REACT_PASSIVE);
			me->LoadCreaturesAddon(true);
			me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_OBS_MOD_HEALTH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL_PCT, true);
			me->ApplySpellImmune(0, IMMUNITY_ID, 56131, true);
			me->ApplySpellImmune(0, IMMUNITY_ID, 63024, true);
			me->ApplySpellImmune(0, IMMUNITY_ID, 36797, true);
			me->ApplySpellImmune(0, IMMUNITY_ID, 75413, true);
			me->ApplySpellImmune(0, IMMUNITY_ID, 29321, true);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
			instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
			MalfurionEvent = false;
			JustDied = false;
			Done = false;
			DoCast(me, SPELL_CORRUPTION_ALEXSTRASZA);
		}

		void DoAction(int32 action)
		{
			switch (action)
			{
			case ACTION_ENTER_COMBAT:
				DoCast(me, SPELL_COPY_DAMAGE);
				instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
				events.ScheduleEvent(EVENT_ALEXSTRASZA_UNDER_80PERCENT_TALK, urand(30000, 45000));
				break;
			}
		}

		void HealReceived(Unit* healer, uint32& heal)
		{
			if (!me->hasLootRecipient())
				me->SetLootRecipient(healer);

			me->LowerPlayerDamageReq(heal);

			if (me->HealthAbovePctHealed(100, heal) && !Done)
			{
				if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_CONTROLLER)))
					controller->AI()->EnterEvadeMode();
				if (Creature* thrall = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_THRALL_2)))
					thrall->Yell("Well done heroes, this was merely a test, your true challange lies deeper within this cave.", LANG_UNIVERSAL);
				instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
				me->RemoveAurasDueToSpell(SPELL_CORRUPTION_ALEXSTRASZA);
				events.ScheduleEvent(EVENT_DREAM_SLIP, 5000);
				DoPlaySoundToSet(me, SOUND_ALEXSTRASZA_100PERCENT);
				me->Say("Thank you, I could not have held out much longer. A terrible thing has happened here", LANG_UNIVERSAL);
				Done = true;
			}

			if (me->HealthAbovePctHealed(80, heal) && !Done && !MalfurionEvent)
			{
				if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_CONTROLLER)))
					controller->AI()->DoAction(ACTION_CANCEL_ADDS);
				me->SummonCreature(NPC_MALFURION_PORTAL, MalfurionPortalSpawnPos);
				events.ScheduleEvent(EVENT_PORTAL_SUMMONS, 1000);
				MalfurionEvent = true;
			}

			if (instance->GetBossState(DATA_ALEXSTRASZA_EVENT) == NOT_STARTED)
				if (Creature* CorruptedDruid = me->FindNearestCreature(NPC_CORRUPTED_DRUID, 30.0f))
					CorruptedDruid->AI()->DoZoneInCombat();
		}

		void DamageTaken(Unit* /*attacker*/, uint32& damage)
		{
			if (me->HealthBelowPctDamaged(25, damage))
			{
				if (damage > me->GetHealth())
					damage = 0;
				if (!JustDied)
				{
					if (Creature* trigger = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_TRIGGER)))
						trigger->AI()->DoAction(ACTION_DEATH);
					instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
					JustDied = true;
				}
			}
		}

		void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_DREAM_SLIP)
			{
				if (Creature* trigger = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_TRIGGER)))
					me->Kill(trigger);
				if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_CONTROLLER)))
				{
					controller->CastSpell(controller, SPELL_SPAWN_CHEST, false);
					me->Kill(controller);
				}
				DoCast(me, SPELL_CLEAR_ALL);
				me->SetDisplayId(11686);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				me->DespawnOrUnsummon(5000);
			}
		}

		void JustSummoned(Creature* summoned)
		{
			summons.Summon(summoned);
			switch (summoned->GetEntry())
			{
			case NPC_MALFURION_PORTAL:
				summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				summoned->SetReactState(REACT_PASSIVE);
				break;
			case NPC_MALFURION:
				summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				summoned->AI()->DoZoneInCombat();
				summoned->AI()->AttackStart(me);
				summoned->SetReactState(REACT_PASSIVE);
				break;
			case NPC_LATHORIUS_LEFT:
				summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				summoned->AI()->DoZoneInCombat();
				summoned->AI()->AttackStart(me);
				summoned->SetReactState(REACT_PASSIVE);
				break;
			case NPC_LILLIANDRA_LEFT:
				summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				summoned->AI()->DoZoneInCombat();
				summoned->AI()->AttackStart(me);
				summoned->SetReactState(REACT_PASSIVE);
				break;
			case NPC_STAGHELM_LEFT:
				summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				summoned->AI()->DoZoneInCombat();
				summoned->AI()->AttackStart(me);
				summoned->SetReactState(REACT_PASSIVE);
				break;
			case NPC_LATHORIUS_RIGHT:
				summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				summoned->AI()->DoZoneInCombat();
				summoned->AI()->AttackStart(me);
				summoned->SetReactState(REACT_PASSIVE);
				break;
			case NPC_LILLIANDRA_RIGHT:
				summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				summoned->AI()->DoZoneInCombat();
				summoned->AI()->AttackStart(me);
				summoned->SetReactState(REACT_PASSIVE);
				break;
			case NPC_STAGHELM_RIGHT:
				summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				summoned->AI()->DoZoneInCombat();
				summoned->AI()->AttackStart(me);
				summoned->SetReactState(REACT_PASSIVE);
				break;
			}
		}

		void UpdateAI(uint32 diff)
		{
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_ALEXSTRASZA_UNDER_80PERCENT_TALK:
					me->Yell("Help! I am traped within this shield, I require aid!", LANG_UNIVERSAL);
					DoPlaySoundToSet(me, SOUND_ALEXSTRASZA_UNDER_80PERCENT);
					events.ScheduleEvent(EVENT_ALEXSTRASZA_UNDER_80PERCENT_TALK, urand(60000, 120000));
					break;
				case EVENT_PORTAL_SUMMONS:
					me->SummonCreature(NPC_MALFURION, MalfurionPortalSpawnPos);
					me->SummonCreature(NPC_LATHORIUS_LEFT, MalfurionPortalSpawnPos);
					me->SummonCreature(NPC_LILLIANDRA_LEFT, MalfurionPortalSpawnPos);
					me->SummonCreature(NPC_STAGHELM_LEFT, MalfurionPortalSpawnPos);
					me->SummonCreature(NPC_LATHORIUS_RIGHT, MalfurionPortalSpawnPos);
					me->SummonCreature(NPC_LILLIANDRA_RIGHT, MalfurionPortalSpawnPos);
					me->SummonCreature(NPC_STAGHELM_RIGHT, MalfurionPortalSpawnPos);
					events.CancelEvent(EVENT_PORTAL_SUMMONS);
					break;
				case EVENT_DREAM_SLIP:
					me->SummonCreature(NPC_ALEXSTRASZA_1, AlexstraszaSpawnPos1);
					DoCast(me, SPELL_DREAM_SLIP);
					break;
				}
			}
		}

	private:
		EventMap events;
		InstanceScript* instance;
		uint32 spawnHealth;
		SummonList summons;
		bool MalfurionEvent;
		bool JustDied;
		bool Done;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return GetMoltenCoreAI<boss_alexstraszaAI>(creature);
	}
};

class npc_red_dragon_combat_trigger : public CreatureScript
{
public:
	npc_red_dragon_combat_trigger() : CreatureScript("npc_red_dragon_combat_trigger") { }

	struct npc_red_dragon_combat_triggerAI : public BossAI
	{
		npc_red_dragon_combat_triggerAI(Creature* creature) : BossAI(creature, DATA_ALEXSTRASZA_EVENT){ }

		void Reset()
		{
			me->SetReactState(REACT_AGGRESSIVE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			IsInCombat = false;
			Despawned = false;
		}

		void EnterCombat(Unit* target)
		{
			me->setActive(true);
			DoZoneInCombat();
			instance->SetBossState(DATA_ALEXSTRASZA_EVENT, IN_PROGRESS);
			if (Creature* alexstrasza = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_EVENT)))
				alexstrasza->AI()->DoAction(ACTION_ENTER_COMBAT);
			me->SetReactState(REACT_PASSIVE);
			IsInCombat = true;
		}

		void AttackStart(Unit* target)
		{
			if (target->GetTypeId() == TYPEID_PLAYER)
				BossAI::AttackStart(target);
		}

		bool CanAIAttack(Unit const* target) const
		{
			return target->GetTypeId() == TYPEID_PLAYER;
		}

		void JustReachedHome()
		{
			DoAction(ACTION_DEATH);
		}

		void DoAction(int32 action)
		{
			switch (action)
			{
			case ACTION_DEATH:
				instance->SetBossState(DATA_ALEXSTRASZA_EVENT, NOT_STARTED);
				me->m_Events.AddEvent(new AlexstraszaDespawner(me), me->m_Events.CalculateTime(5000));
				break;
			}
		}

		void UpdateAI(uint32 /*diff*/)
		{
			Map* map = me->GetMap();
			if (map->IsRaid() && !Despawned && IsInCombat)
			{
				Map::PlayerList const &PlayerList = map->GetPlayers();
				for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
				{
					if (!me->IsWithinDist(i->GetSource(), 60.0f))
					{
						me->m_Events.AddEvent(new AlexstraszaDespawner(me), me->m_Events.CalculateTime(5000));
						Despawned = true;
						IsInCombat = false;
					}
				}
			}

			if (!me->IsInCombat())
				return;

			std::list<HostileReference*> const& threatList = me->getThreatManager().getThreatList();
			if (threatList.empty())
			{
				EnterEvadeMode();
				return;
			}

			EvadeCheck ^= true;
			if (!EvadeCheck)
				return;

			for (std::list<HostileReference*>::const_iterator itr = threatList.begin(); itr != threatList.end(); ++itr)
				if (Unit* target = (*itr)->getTarget())
					if (target->GetTypeId() == TYPEID_PLAYER)
						return;

			EnterEvadeMode();
		}

	private:
		bool EvadeCheck;
		bool IsInCombat;
		bool Despawned;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return GetMoltenCoreAI<npc_red_dragon_combat_triggerAI>(creature);
	}
};

class npc_alexstrasza_controller : public CreatureScript
{
public:
	npc_alexstrasza_controller() : CreatureScript("npc_alexstrasza_controller") { }

	struct npc_alexstrasza_controllerAI : public ScriptedAI
	{
		npc_alexstrasza_controllerAI(Creature* creature) : ScriptedAI(creature), summons(me)
		{
			instance = creature->GetInstanceScript();
		}

		void Reset()
		{
			events.Reset();
			me->SetReactState(REACT_AGGRESSIVE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		}

		void JustReachedHome()
		{
			me->setActive(false);
		}

		void DoAction(int32 action)
		{
			switch (action)
			{
			case ACTION_CANCEL_ADDS:
				events.CancelEvent(EVENT_ALEXSTRASZA_UNDER_80PERCENT_TALK);
				events.CancelEvent(EVENT_SUMMON_PROTECTOR_OF_THE_KEEP);
				events.CancelEvent(EVENT_SUMMON_GUARDIAN_OF_THE_SECRETS);
				events.CancelEvent(EVENT_SUMMON_FABULOUS_DISTRACTOR);
				summons.DespawnEntry(NPC_PROTECTOR_OF_THE_KEEP);
				summons.DespawnEntry(NPC_GUARDIAN_OF_THE_SECRETS);
				summons.DespawnEntry(NPC_FABULOUS_DISTRACTOR);
				break;
			}
		}

		void EnterCombat(Unit* /*target*/)
		{
			events.ScheduleEvent(EVENT_SUMMON_PROTECTOR_OF_THE_KEEP, urand(37500, 42500));
			events.ScheduleEvent(EVENT_SUMMON_GUARDIAN_OF_THE_SECRETS, urand(45000, 52500));
			events.ScheduleEvent(EVENT_SUMMON_FABULOUS_DISTRACTOR, urand(55000, 62500));
			me->SetReactState(REACT_PASSIVE);
			me->setActive(true);
		}

		void JustSummoned(Creature* summoned)
		{
			switch (summoned->GetEntry())
			{
			case NPC_PROTECTOR_OF_THE_KEEP:
				summoned->AI()->DoAction(ACTION_DO_ADDS_WAYPOINT);
				break;
			case NPC_GUARDIAN_OF_THE_SECRETS:
				summoned->AI()->DoAction(ACTION_DO_ADDS_WAYPOINT);
				break;
			case NPC_FABULOUS_DISTRACTOR:
				summoned->AI()->DoAction(ACTION_DO_ADDS_WAYPOINT);
				break;
			}
		}

		void UpdateAI(uint32 diff)
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_SUMMON_PROTECTOR_OF_THE_KEEP:
					me->SummonCreature(NPC_PROTECTOR_OF_THE_KEEP, AddsSpawnPos);
					me->SummonCreature(NPC_PROTECTOR_OF_THE_KEEP, AddsSpawnPos);
					me->SummonCreature(NPC_PROTECTOR_OF_THE_KEEP, AddsSpawnPos);
					events.ScheduleEvent(EVENT_SUMMON_PROTECTOR_OF_THE_KEEP, 10000);
					break;
				case EVENT_SUMMON_GUARDIAN_OF_THE_SECRETS:
					me->SummonCreature(NPC_GUARDIAN_OF_THE_SECRETS, AddsSpawnPos);
					me->SummonCreature(NPC_GUARDIAN_OF_THE_SECRETS, AddsSpawnPos);
					events.ScheduleEvent(EVENT_SUMMON_GUARDIAN_OF_THE_SECRETS, 20000);
					break;
				case EVENT_SUMMON_FABULOUS_DISTRACTOR:
					me->SummonCreature(NPC_FABULOUS_DISTRACTOR, AddsSpawnPos);
					events.ScheduleEvent(EVENT_SUMMON_FABULOUS_DISTRACTOR, 30000);
					break;
				default:
					break;
				}
			}
		}

	private:
		EventMap events;
		SummonList summons;
		InstanceScript* instance;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_alexstrasza_controllerAI(creature);
	}
};

class npc_corrupted_druid : public CreatureScript
{
public:
	npc_corrupted_druid() : CreatureScript("npc_corrupted_druid") { }

	struct npc_corrupted_druidAI : public ScriptedAI
	{
		npc_corrupted_druidAI(Creature* creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		bool CanAIAttack(Unit const* target) const
		{
			return target->GetEntry() != NPC_ALEXSTRASZA;
		}

		void Reset()
		{
			events.Reset();
			events.ScheduleEvent(EVENT_CORRUPTING_DEVOTION_AURA, 1000);
			events.ScheduleEvent(EVENT_SHADOW_CLEAVE, urand(7500, 15000));
			events.ScheduleEvent(EVENT_CORRUPT_SOUL, urand(10000, 17500));
			events.ScheduleEvent(EVENT_WELL_OF_CORRUPTION, urand(12500, 20000));
			SaySomethingTimer = urand(10000, 60000);
			CanCallEnterCombat = true;
		}

		void EnterCombat(Unit* /*target*/)
		{
			me->FinishSpell(CURRENT_CHANNELED_SPELL, false);
			if (me->GetDBTableGUIDLow() && CanCallEnterCombat)
			{
				if (Creature* trigger = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_TRIGGER)))
					trigger->AI()->DoZoneInCombat();
				if (Creature* Controller = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_CONTROLLER)))
					Controller->AI()->DoZoneInCombat();
				std::list<Creature*> druids;
				CorruptedDruidCheck check;
				Trinity::CreatureListSearcher<CorruptedDruidCheck> searcher(me, druids, check);
				me->VisitNearbyGridObject(100.0f, searcher);
				for (std::list<Creature*>::iterator itr = druids.begin(); itr != druids.end(); ++itr)
					(*itr)->AI()->DoAction(ACTION_ENTER_COMBAT);
			}
		}

		void DoAction(int32 action)
		{
			if (action != ACTION_ENTER_COMBAT)
				return;

			CanCallEnterCombat = false;
			DoZoneInCombat();
			CanCallEnterCombat = true;
		}

		void UpdateAI(uint32 diff)
		{
			if (!me->IsInCombat())
				if (me->GetDBTableGUIDLow())
					if (!me->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
						DoCast(me, SPELL_CORRUPTION_CHANNEL);

			if (!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SaySomethingTimer <= diff)
			{
				switch (urand(0, 2))
				{
				case 0: { me->Say("Return while you can.", LANG_UNIVERSAL); break; }
				case 1: { me->Say("You do not belong here!", LANG_UNIVERSAL); break; }
				case 2: { me->Say("This will be your end!", LANG_UNIVERSAL); break; }
				}
				SaySomethingTimer = urand(10000, 90000);
			}
			else SaySomethingTimer -= diff;

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_CORRUPTING_DEVOTION_AURA:
					DoCast(me, SPELL_CORRUPTING_DEVOTION_AURA);
					events.CancelEvent(EVENT_CORRUPTING_DEVOTION_AURA);
					break;
				case EVENT_SHADOW_CLEAVE:
					if (me->IsWithinMeleeRange(me->GetVictim()))
						DoCastVictim(SPELL_SHADOW_CLEAVE);
					events.ScheduleEvent(EVENT_SHADOW_CLEAVE, urand(7500, 15000));
					break;
				case EVENT_CORRUPT_SOUL:
					if (Unit * target = SelectTarget(SELECT_TARGET_RANDOM))
						if (!me->IsWithinMeleeRange(me->GetVictim()))
							DoCast(target, SPELL_CORRUPT_SOUL);
					events.ScheduleEvent(EVENT_CORRUPT_SOUL, urand(12500, 17500));
					break;
				case EVENT_WELL_OF_CORRUPTION:
					if (me->IsWithinMeleeRange(me->GetVictim()))
						DoCastVictim(SPELL_WELL_OF_CORRUPTION);
					events.ScheduleEvent(EVENT_WELL_OF_CORRUPTION, urand(15000, 25000));
					break;
				}
			}

			DoMeleeAttackIfReady();
		}

	private:
		EventMap events;
		InstanceScript* instance;
		uint32 SaySomethingTimer;
		bool CanCallEnterCombat;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return GetMoltenCoreAI<npc_corrupted_druidAI>(creature);
	}
};

class npc_malfurion_portal : public CreatureScript
{
public:
	npc_malfurion_portal() : CreatureScript("npc_malfurion_portal") { }

	struct npc_malfurion_portalAI : public ScriptedAI
	{
		npc_malfurion_portalAI(Creature* creature) : ScriptedAI(creature)
		{
			me->AddAura(SPELL_MALFURION_PORTAL, me);
		}

		void Reset()
		{
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		}

		void EnterCombat(Unit* /*who*/)
		{
			me->SetFacingTo(5);
		}

		void UpdateAI(uint32 diff)
		{
			if (!UpdateVictim())
				return;
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return GetMoltenCoreAI<npc_malfurion_portalAI>(creature);
	}
};

class npc_malfurion : public CreatureScript
{
public:
	npc_malfurion() : CreatureScript("npc_malfurion") { }

	struct npc_malfurionAI : public ScriptedAI
	{
		npc_malfurionAI(Creature* creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		bool CanAIAttack(Unit const* target) const
		{
			return target->GetEntry() != NPC_ALEXSTRASZA;
		}

		void Reset()
		{
			events.Reset();
			malfurionEscaped = false;
		}

		void EnterCombat(Unit* /*who*/)
		{
			me->SetFacingTo(90);
			events.ScheduleEvent(EVENT_MALFURION_ROOT, 500);
			events.ScheduleEvent(EVENT_MALFURION_WALK, 2000);
			events.ScheduleEvent(EVENT_MALFURION_TALK_1, 3000);
			events.ScheduleEvent(EVENT_MALFURION_TALK_2, 7250);
			events.ScheduleEvent(EVENT_ENABLE_COMBAT, 12500);
			events.ScheduleEvent(EVENT_MIND_CONTROL, urand(20000, 27500));
			events.ScheduleEvent(EVENT_TWILIGHT_CUTTER, 40000);
		}

		void DamageTaken(Unit* /*attacker*/, uint32& damage)
		{
			if (me->HealthBelowPct(10))
			{
				damage = 0;
				if (!malfurionEscaped)
				{
					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					me->Yell("We will meet again soon...", LANG_UNIVERSAL);
					events.ScheduleEvent(EVENT_MALFURION_ESCAPE, 2500);
				}
			}
		}

		void UpdateAI(uint32 diff)
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_MALFURION_ROOT:
					instance->DoCastSpellOnPlayers(SPELL_MALFURION_ROOT);
					events.CancelEvent(EVENT_MALFURION_ROOT);
					break;
				case EVENT_MALFURION_WALK:
					me->SetSpeed(MOVE_RUN, 0.5);
					me->GetMotionMaster()->MovePoint(0, MalfurionMovePos);
					events.CancelEvent(EVENT_MALFURION_WALK);
					break;
				case EVENT_MALFURION_TALK_1:
					DoPlaySoundToSet(me, SOUND_MALFURION_INTRO_1);
					me->Yell("The secrets in this keep shall remain hidden.", LANG_UNIVERSAL);
					events.CancelEvent(EVENT_MALFURION_TALK_1);
					break;
				case EVENT_MALFURION_TALK_2:
					DoPlaySoundToSet(me, SOUND_MALFURION_INTRO_2);
					me->Yell("Perhaps we will be allies soon.", LANG_UNIVERSAL);
					events.CancelEvent(EVENT_MALFURION_TALK_2);
					break;
				case EVENT_ENABLE_COMBAT:
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MALFURION_ROOT);
					me->SetReactState(REACT_AGGRESSIVE);
					me->SetSpeed(MOVE_RUN, 1.14286f);
					me->GetMotionMaster()->MoveChase(me->GetVictim());
					events.CancelEvent(EVENT_ENABLE_COMBAT);
					break;
				case EVENT_MIND_CONTROL:
					if (Unit * target = SelectTarget(SELECT_TARGET_RANDOM))
						DoCast(target, SPELL_MIND_CONTROL);
					events.ScheduleEvent(EVENT_MIND_CONTROL, 15000);
					break;
				case EVENT_TWILIGHT_CUTTER:
					if (Unit * target = SelectTarget(SELECT_TARGET_RANDOM))
						if (!me->IsWithinMeleeRange(me->GetVictim()))
							DoCast(target, SPELL_TWILIGHT_CUTTER);
					events.ScheduleEvent(EVENT_TWILIGHT_CUTTER, 25000);
					break;
				case EVENT_MALFURION_ESCAPE:
					DoCast(me, SPELL_MALFURION_ESCAPE);
					me->DespawnOrUnsummon();
					events.CancelEvent(EVENT_MALFURION_ESCAPE);
					break;
				}
			}

			DoMeleeAttackIfReady();
		}

	private:
		EventMap events;
		InstanceScript* instance;
		bool malfurionEscaped;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return GetMoltenCoreAI<npc_malfurionAI>(creature);
	}
};

class npc_lathorius_left : public CreatureScript
{
public:
	npc_lathorius_left() : CreatureScript("npc_lathorius_left") { }

	struct npc_lathorius_leftAI : public ScriptedAI
	{
		npc_lathorius_leftAI(Creature* creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		bool CanAIAttack(Unit const* target) const
		{
			return target->GetEntry() != NPC_ALEXSTRASZA;
		}

		void Reset()
		{
			events.Reset();
		}

		void EnterCombat(Unit* /*who*/)
		{
			me->SetFacingTo(90);
			events.ScheduleEvent(EVENT_LATHORIUS_WALK_LEFT, 2000);
			events.ScheduleEvent(EVENT_ENABLE_COMBAT, 10000);
			events.ScheduleEvent(EVENT_ARCANE_STORM, urand(20000, 30000));
			events.ScheduleEvent(EVENT_REJUVENATION, urand(17500, 27500));
		}

		void UpdateAI(uint32 diff)
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_LATHORIUS_WALK_LEFT:
					me->SetSpeed(MOVE_RUN, 0.5);
					me->GetMotionMaster()->MovePoint(0, LathoriusMovePosLeft);
					events.CancelEvent(EVENT_LATHORIUS_WALK_LEFT);
					break;
				case EVENT_ENABLE_COMBAT:
					me->SetReactState(REACT_AGGRESSIVE);
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					me->SetSpeed(MOVE_RUN, 1.14286f);
					me->GetMotionMaster()->MoveChase(me->GetVictim());
					events.CancelEvent(EVENT_ENABLE_COMBAT);
					break;
				case EVENT_ARCANE_STORM:
					DoCast(me, SPELL_ARCANE_STORM);
					events.ScheduleEvent(EVENT_ARCANE_STORM, urand(10500, 17500));
					break;
				case EVENT_REJUVENATION:
					if (Unit* target = DoSelectLowestHpFriendly(40, RAID_MODE(75000, 400000)))
					{
						DoCast(target, SPELL_REJUVENATION);
						events.ScheduleEvent(EVENT_REJUVENATION, urand(8500, 17500));
					}
					else
						events.ScheduleEvent(EVENT_REJUVENATION, 2500);
					return;
					break;
				}
			}

			DoMeleeAttackIfReady();
		}

	private:
		EventMap events;
		InstanceScript* instance;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return GetMoltenCoreAI<npc_lathorius_leftAI>(creature);
	}
};

class npc_lilliandra_left : public CreatureScript
{
public:
	npc_lilliandra_left() : CreatureScript("npc_lilliandra_left") { }

	struct npc_lilliandra_leftAI : public ScriptedAI
	{
		npc_lilliandra_leftAI(Creature* creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		bool CanAIAttack(Unit const* target) const
		{
			return target->GetEntry() != NPC_ALEXSTRASZA;
		}

		void Reset()
		{
			events.Reset();
		}

		void EnterCombat(Unit* /*who*/)
		{
			me->SetFacingTo(90);
			events.ScheduleEvent(EVENT_LILLIANDRA_WALK_LEFT, 2000);
			events.ScheduleEvent(EVENT_ENABLE_COMBAT, 10000);
			events.ScheduleEvent(EVENT_MOONFIRE, urand(17500, 22500));
			events.ScheduleEvent(EVENT_LIGHT_OF_ELUNE, urand(37500, 42500));
		}

		void UpdateAI(uint32 diff)
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_LILLIANDRA_WALK_LEFT:
					me->SetSpeed(MOVE_RUN, 0.5);
					me->GetMotionMaster()->MovePoint(0, LilliandraMovePosLeft);
					events.CancelEvent(EVENT_LATHORIUS_WALK_LEFT);
					break;
				case EVENT_ENABLE_COMBAT:
					me->SetReactState(REACT_AGGRESSIVE);
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					me->SetSpeed(MOVE_RUN, 1.14286f);
					me->GetMotionMaster()->MoveChase(me->GetVictim());
					events.CancelEvent(EVENT_ENABLE_COMBAT);
					break;
				case EVENT_MOONFIRE:
					DoCastVictim(SPELL_MOONFIRE);
					events.ScheduleEvent(EVENT_MOONFIRE, urand(7500, 12500));
					break;
				case EVENT_LIGHT_OF_ELUNE:
					DoCast(me, SPELL_LIGHT_OF_ELUNE);
					events.ScheduleEvent(EVENT_LIGHT_OF_ELUNE, urand(27500, 32500));
					break;
				}
			}

			DoMeleeAttackIfReady();
		}

	private:
		EventMap events;
		InstanceScript* instance;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return GetMoltenCoreAI<npc_lilliandra_leftAI>(creature);
	}
};

class npc_staghelm_left : public CreatureScript
{
public:
	npc_staghelm_left() : CreatureScript("npc_staghelm_left") { }

	struct npc_staghelm_leftAI : public ScriptedAI
	{
		npc_staghelm_leftAI(Creature* creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		bool CanAIAttack(Unit const* target) const
		{
			return target->GetEntry() != NPC_ALEXSTRASZA;
		}

		void reset()
		{
			events.Reset();
		}

		void EnterCombat(Unit* /*who*/)
		{
			me->SetFacingTo(90);
			events.ScheduleEvent(EVENT_STAGHELM_WALK_LEFT, 2000);
			events.ScheduleEvent(EVENT_ENABLE_COMBAT, 10000);
			events.ScheduleEvent(EVENT_REGROWTH, urand(17500, 27500));
			events.ScheduleEvent(EVENT_FLAME_WAVE, urand(20000, 37500));
		}

		void UpdateAI(uint32 diff)
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_STAGHELM_WALK_LEFT:
					me->SetSpeed(MOVE_RUN, 0.5);
					me->GetMotionMaster()->MovePoint(0, StaghelmMovePosLeft);
					events.CancelEvent(EVENT_STAGHELM_WALK_LEFT);
					break;
				case EVENT_ENABLE_COMBAT:
					me->SetReactState(REACT_AGGRESSIVE);
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					me->SetSpeed(MOVE_RUN, 1.14286f);
					me->GetMotionMaster()->MoveChase(me->GetVictim());
					events.CancelEvent(EVENT_ENABLE_COMBAT);
					break;
				case EVENT_REGROWTH:
					if (Unit* target = DoSelectLowestHpFriendly(40, RAID_MODE(75000, 250000)))
					{
						DoCast(target, SPELL_REGROWTH);
						events.ScheduleEvent(EVENT_REGROWTH, urand(10000, 17500));
					}
					else
						events.ScheduleEvent(EVENT_REGROWTH, 2500);
					return;
					break;
				case EVENT_FLAME_WAVE:
					DoCast(me, SPELL_FLAME_WAVE);
					events.ScheduleEvent(EVENT_FLAME_WAVE, urand(17500, 32500));
					break;
				}
			}

			DoMeleeAttackIfReady();
		}

	private:
		EventMap events;
		InstanceScript* instance;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return GetMoltenCoreAI<npc_staghelm_leftAI>(creature);
	}
};

class npc_lathorius_right : public CreatureScript
{
public:
	npc_lathorius_right() : CreatureScript("npc_lathorius_right") { }

	struct npc_lathorius_rightAI : public ScriptedAI
	{
		npc_lathorius_rightAI(Creature* creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		bool CanAIAttack(Unit const* target) const
		{
			return target->GetEntry() != NPC_ALEXSTRASZA;
		}

		void Reset()
		{
			events.Reset();
		}

		void EnterCombat(Unit* /*who*/)
		{
			me->SetFacingTo(90);
			events.ScheduleEvent(EVENT_LATHORIUS_WALK_RIGHT, 2000);
			events.ScheduleEvent(EVENT_ENABLE_COMBAT, 10000);
			events.ScheduleEvent(EVENT_ARCANE_STORM, urand(20000, 30000));
			events.ScheduleEvent(EVENT_REJUVENATION, urand(17500, 27500));
		}

		void UpdateAI(uint32 diff)
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_LATHORIUS_WALK_RIGHT:
					me->SetSpeed(MOVE_RUN, 0.5);
					me->GetMotionMaster()->MovePoint(0, LathoriusMovePosRight);
					events.CancelEvent(EVENT_LATHORIUS_WALK_RIGHT);
					break;
				case EVENT_ENABLE_COMBAT:
					me->SetReactState(REACT_AGGRESSIVE);
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					me->SetSpeed(MOVE_RUN, 1.14286f);
					me->GetMotionMaster()->MoveChase(me->GetVictim());
					events.CancelEvent(EVENT_ENABLE_COMBAT);
					break;
				case EVENT_ARCANE_STORM:
					DoCast(me, SPELL_ARCANE_STORM);
					events.ScheduleEvent(EVENT_ARCANE_STORM, urand(10500, 17500));
					break;
				case EVENT_REJUVENATION:
					if (Unit* target = DoSelectLowestHpFriendly(40, RAID_MODE(75000, 400000)))
					{
						DoCast(target, SPELL_REJUVENATION);
						events.ScheduleEvent(EVENT_REJUVENATION, urand(8500, 17500));
					}
					else
						events.ScheduleEvent(EVENT_REJUVENATION, 2500);
					return;
					break;
				}
			}

			DoMeleeAttackIfReady();
		}

	private:
		EventMap events;
		InstanceScript* instance;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return GetMoltenCoreAI<npc_lathorius_rightAI>(creature);
	}
};

class npc_lilliandra_right : public CreatureScript
{
public:
	npc_lilliandra_right() : CreatureScript("npc_lilliandra_right") { }

	struct npc_lilliandra_rightAI : public ScriptedAI
	{
		npc_lilliandra_rightAI(Creature* creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		bool CanAIAttack(Unit const* target) const
		{
			return target->GetEntry() != NPC_ALEXSTRASZA;
		}

		void Reset()
		{
			events.Reset();
		}

		void EnterCombat(Unit* /*who*/)
		{
			me->SetFacingTo(90);
			events.ScheduleEvent(EVENT_LILLIANDRA_WALK_RIGHT, 2000);
			events.ScheduleEvent(EVENT_ENABLE_COMBAT, 10000);
			events.ScheduleEvent(EVENT_MOONFIRE, urand(17500, 22500));
			events.ScheduleEvent(EVENT_LIGHT_OF_ELUNE, urand(37500, 42500));
		}

		void UpdateAI(uint32 diff)
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_LILLIANDRA_WALK_RIGHT:
					me->SetSpeed(MOVE_RUN, 0.5);
					me->GetMotionMaster()->MovePoint(0, LilliandraMovePosRight);
					events.CancelEvent(EVENT_LILLIANDRA_WALK_RIGHT);
					break;
				case EVENT_ENABLE_COMBAT:
					me->SetReactState(REACT_AGGRESSIVE);
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					me->SetSpeed(MOVE_RUN, 1.14286f);
					me->GetMotionMaster()->MoveChase(me->GetVictim());
					events.CancelEvent(EVENT_ENABLE_COMBAT);
					break;
				case EVENT_MOONFIRE:
					DoCastVictim(SPELL_MOONFIRE);
					events.ScheduleEvent(EVENT_MOONFIRE, urand(7500, 12500));
					break;
				case EVENT_LIGHT_OF_ELUNE:
					DoCast(me, SPELL_LIGHT_OF_ELUNE);
					events.ScheduleEvent(EVENT_LIGHT_OF_ELUNE, urand(27500, 32500));
					break;
				}
			}

			DoMeleeAttackIfReady();
		}

	private:
		EventMap events;
		InstanceScript* instance;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return GetMoltenCoreAI<npc_lilliandra_rightAI>(creature);
	}
};

class npc_staghelm_right : public CreatureScript
{
public:
	npc_staghelm_right() : CreatureScript("npc_staghelm_right") { }

	struct npc_staghelm_rightAI : public ScriptedAI
	{
		npc_staghelm_rightAI(Creature* creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		bool CanAIAttack(Unit const* target) const
		{
			return target->GetEntry() != NPC_ALEXSTRASZA;
		}

		void Reset()
		{
			events.Reset();
		}

		void EnterCombat(Unit* /*who*/)
		{
			me->SetFacingTo(90);
			events.ScheduleEvent(EVENT_STAGHELM_WALK_RIGHT, 2000);
			events.ScheduleEvent(EVENT_ENABLE_COMBAT, 10000);
			events.ScheduleEvent(EVENT_REGROWTH, urand(17500, 27500));
			events.ScheduleEvent(EVENT_FLAME_WAVE, urand(20000, 37500));
		}

		void UpdateAI(uint32 diff)
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_STAGHELM_WALK_RIGHT:
					me->SetSpeed(MOVE_RUN, 0.5);
					me->GetMotionMaster()->MovePoint(0, StaghelmMovePosRight);
					events.CancelEvent(EVENT_STAGHELM_WALK_RIGHT);
					break;
				case EVENT_ENABLE_COMBAT:
					me->SetReactState(REACT_AGGRESSIVE);
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					me->SetSpeed(MOVE_RUN, 1.14286f);
					me->GetMotionMaster()->MoveChase(me->GetVictim());
					events.CancelEvent(EVENT_ENABLE_COMBAT);
					break;
				case EVENT_REGROWTH:
					if (Unit* target = DoSelectLowestHpFriendly(40, RAID_MODE(75000, 250000)))
					{
						DoCast(target, SPELL_REGROWTH);
						events.ScheduleEvent(EVENT_REGROWTH, urand(10000, 17500));
					}
					else
						events.ScheduleEvent(EVENT_REGROWTH, 2500);
					return;
					break;
				case EVENT_FLAME_WAVE:
					DoCast(me, SPELL_FLAME_WAVE);
					events.ScheduleEvent(EVENT_FLAME_WAVE, urand(17500, 32500));
					break;
				}
			}

			DoMeleeAttackIfReady();
		}

	private:
		EventMap events;
		InstanceScript* instance;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return GetMoltenCoreAI<npc_staghelm_rightAI>(creature);
	}
};

class npc_protector_of_the_keep : public CreatureScript
{
public:
	npc_protector_of_the_keep() : CreatureScript("npc_protector_of_the_keep") { }

	struct npc_protector_of_the_keepAI : public ScriptedAI
	{
		npc_protector_of_the_keepAI(Creature* creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		bool CanAIAttack(Unit const* target) const
		{
			return target->GetEntry() != NPC_ALEXSTRASZA;
		}

		void Reset()
		{
			events.Reset();
		}

		void DoAction(int32 action)
		{
			switch (action)
			{
			case ACTION_DO_ADDS_WAYPOINT:
				events.ScheduleEvent(EVENT_ADDS_MOVE_1, 1000);
				events.ScheduleEvent(EVENT_ADDS_MOVE_2, 4500);
				events.ScheduleEvent(EVENT_ADDS_MOVE_3, 8500);
				events.ScheduleEvent(EVENT_ADDS_MOVE_4, 14000);
				events.ScheduleEvent(EVENT_ADDS_MOVE_5, 16750);
				break;
			}
		}

		void EnterCombat(Unit* /*who*/)
		{
			events.ScheduleEvent(EVENT_CURSE_OF_AGONY, urand(2500, 7500));
		}

		void UpdateAI(uint32 diff)
		{
			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			events.Update(diff);

			if (!UpdateVictim())
			{
				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case EVENT_ADDS_MOVE_1:
						me->GetMotionMaster()->MovePoint(0, AddsMove1);
						break;
					case EVENT_ADDS_MOVE_2:
						me->GetMotionMaster()->MovePoint(0, AddsMove2);
						break;
					case EVENT_ADDS_MOVE_3:
						me->GetMotionMaster()->MovePoint(0, AddsMove3);
						break;
					case EVENT_ADDS_MOVE_4:
						me->GetMotionMaster()->MovePoint(0, AddsMove4);
						break;
					case EVENT_ADDS_MOVE_5:
						me->GetMotionMaster()->MovePoint(0, AddsMove5);
						break;
					}
				}
			}

			if (UpdateVictim())
			{
				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case EVENT_CURSE_OF_AGONY:
						if (Unit * target = SelectTarget(SELECT_TARGET_RANDOM))
							DoCast(target, SPELL_CURSE_OF_AGONY);
						events.ScheduleEvent(EVENT_CURSE_OF_AGONY, urand(2500, 7500));
						break;
					}
				}
			}

			DoMeleeAttackIfReady();
		}

	private:
		EventMap events;
		InstanceScript* instance;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return GetMoltenCoreAI<npc_protector_of_the_keepAI>(creature);
	}
};

class npc_guardian_of_the_secrets : public CreatureScript
{
public:
	npc_guardian_of_the_secrets() : CreatureScript("npc_guardian_of_the_secrets") { }

	struct npc_guardian_of_the_secretsAI : public ScriptedAI
	{
		npc_guardian_of_the_secretsAI(Creature* creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		bool CanAIAttack(Unit const* target) const
		{
			return target->GetEntry() != NPC_ALEXSTRASZA;
		}

		void Reset()
		{
			events.Reset();
		}

		void DoAction(int32 action)
		{
			switch (action)
			{
			case ACTION_DO_ADDS_WAYPOINT:
				events.ScheduleEvent(EVENT_ADDS_MOVE_1, 1000);
				events.ScheduleEvent(EVENT_ADDS_MOVE_2, 4500);
				events.ScheduleEvent(EVENT_ADDS_MOVE_3, 8500);
				events.ScheduleEvent(EVENT_ADDS_MOVE_4, 14000);
				events.ScheduleEvent(EVENT_ADDS_MOVE_5, 16750);
				break;
			}
		}

		void EnterCombat(Unit* /*who*/)
		{
			events.ScheduleEvent(EVENT_EMPOWERED_SHADOW_LANCE, urand(5000, 12500));
			events.ScheduleEvent(EVENT_SHADOW_MEND, urand(10000, 17500));
			events.ScheduleEvent(EVENT_ENRAGE, 30000);
		}

		void UpdateAI(uint32 diff)
		{
			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			events.Update(diff);

			if (!UpdateVictim())
			{
				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case EVENT_ADDS_MOVE_1:
						me->GetMotionMaster()->MovePoint(0, AddsMove1);
						break;
					case EVENT_ADDS_MOVE_2:
						me->GetMotionMaster()->MovePoint(0, AddsMove2);
						break;
					case EVENT_ADDS_MOVE_3:
						me->GetMotionMaster()->MovePoint(0, AddsMove3);
						break;
					case EVENT_ADDS_MOVE_4:
						me->GetMotionMaster()->MovePoint(0, AddsMove4);
						break;
					case EVENT_ADDS_MOVE_5:
						me->GetMotionMaster()->MovePoint(0, AddsMove5);
						break;
					}
				}
			}

			if (UpdateVictim())
			{
				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case EVENT_EMPOWERED_SHADOW_LANCE:
						if (Unit * target = SelectTarget(SELECT_TARGET_RANDOM))
							DoCast(target, SPELL_EMPOWERED_SHADOW_LANCE);
						events.ScheduleEvent(EVENT_EMPOWERED_SHADOW_LANCE, urand(5000, 12500));
						break;
					case EVENT_SHADOW_MEND:
						if (Unit* target = DoSelectLowestHpFriendly(40, RAID_MODE(75000, 400000)))
						{
							DoCast(target, SPELL_SHADOW_MEND);
							events.ScheduleEvent(EVENT_SHADOW_MEND, urand(12500, 17500));
						}
						else
							events.ScheduleEvent(EVENT_SHADOW_MEND, urand(2500, 5000));
						return;
						break;
					case EVENT_ENRAGE:
						DoCast(me, SPELL_ENRAGE);
						events.CancelEvent(EVENT_ENRAGE);
						break;
					}
				}
			}

			DoMeleeAttackIfReady();
		}

	private:
		EventMap events;
		InstanceScript* instance;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return GetMoltenCoreAI<npc_guardian_of_the_secretsAI>(creature);
	}
};

class npc_fabulous_distractor : public CreatureScript
{
public:
	npc_fabulous_distractor() : CreatureScript("npc_fabulous_distractor") { }

	struct npc_fabulous_distractorAI : public ScriptedAI
	{
		npc_fabulous_distractorAI(Creature* creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		bool CanAIAttack(Unit const* target) const
		{
			return target->GetEntry() != NPC_ALEXSTRASZA;
		}

		void Reset()
		{
			events.Reset();
		}

		void DoAction(int32 action)
		{
			switch (action)
			{
			case ACTION_DO_ADDS_WAYPOINT:
				events.ScheduleEvent(EVENT_ADDS_MOVE_1, 1000);
				events.ScheduleEvent(EVENT_ADDS_MOVE_2, 4500);
				events.ScheduleEvent(EVENT_ADDS_MOVE_3, 8500);
				events.ScheduleEvent(EVENT_ADDS_MOVE_4, 14000);
				events.ScheduleEvent(EVENT_ADDS_MOVE_5, 16750);
				break;
			}
		}

		void EnterCombat(Unit* /*who*/)
		{
			events.ScheduleEvent(EVENT_SHADOWSTEP, urand(5000, 7500));
			events.ScheduleEvent(EVENT_FEAR, urand(12500, 17500));
			events.ScheduleEvent(EVENT_GRAVITY_BOMB, urand(20000, 27500));
		}

		void UpdateAI(uint32 diff)
		{
			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			events.Update(diff);

			if (!UpdateVictim())
			{
				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case EVENT_ADDS_MOVE_1:
						me->GetMotionMaster()->MovePoint(0, AddsMove1);
						break;
					case EVENT_ADDS_MOVE_2:
						me->GetMotionMaster()->MovePoint(0, AddsMove2);
						break;
					case EVENT_ADDS_MOVE_3:
						me->GetMotionMaster()->MovePoint(0, AddsMove3);
						break;
					case EVENT_ADDS_MOVE_4:
						me->GetMotionMaster()->MovePoint(0, AddsMove4);
						break;
					case EVENT_ADDS_MOVE_5:
						me->GetMotionMaster()->MovePoint(0, AddsMove5);
						break;
					}
				}
			}

			if (UpdateVictim())
			{
				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case EVENT_SHADOWSTEP:
						if (Unit * target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
							DoCast(target, SPELL_SHADOWSTEP);
						events.ScheduleEvent(EVENT_SHADOWSTEP, urand(5000, 7500));
						break;
					case EVENT_FEAR:
						DoCast(me, SPELL_FEAR);
						events.ScheduleEvent(EVENT_FEAR, urand(12500, 17500));
						break;
					case EVENT_GRAVITY_BOMB:
						if (Unit * target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
							DoCast(target, SPELL_GRAVITY_BOMB);
						events.ScheduleEvent(EVENT_GRAVITY_BOMB, urand(15000, 22500));
						break;
					}
				}
			}

			DoMeleeAttackIfReady();
		}

	private:
		EventMap events;
		InstanceScript* instance;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return GetMoltenCoreAI<npc_fabulous_distractorAI>(creature);
	}
};

void AddSC_boss_alexstrasza_event()
{
	new boss_alexstrasza();
	new npc_red_dragon_combat_trigger();
	new npc_alexstrasza_controller();
	new npc_corrupted_druid();
	new npc_malfurion_portal();
	new npc_malfurion();
	new npc_lathorius_left();
	new npc_lilliandra_left();
	new npc_staghelm_left();
	new npc_lathorius_right();
	new npc_lilliandra_right();
	new npc_staghelm_right();
	new npc_protector_of_the_keep();
	new npc_guardian_of_the_secrets();
	new npc_fabulous_distractor();
}