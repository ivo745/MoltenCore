/* ScriptData
SDCreator: frostI
SDName: boss_jandice_barov_mc
SD%Complete: 60
SDComment:
SDCategory: Molten Core
EndScriptData */

#include "ScriptPCH.h"
#include "molten_core.h"

enum Spells
{
	//Jandice
	SPELL_WONDEROUS_RAPIDITY_START = 62348,
	SPELL_WONDEROUS_RAPIDITY = 65112,
	SPELL_GRAVITY_FLUX = 73539,
	SPELL_BERSERK = 26662,
	//Illusions
	SPELL_SUMMON_VISUAL_1 = 52582,
	SPELL_SUMMON_VISUAL_2 = 62348,
	SPELL_WHIRLWIND_DAMAGE = 43116,
	SPELL_SPIRIT_BURST = 73806
};

enum Events
{
	//Jandice
	EVENT_WONDEROUS_RAPIDITY = 1,
	EVENT_GRAVITY_FLUX,
	EVENT_ENRAGE,
	//Illusions
	EVENT_WHIRLWIND
};

enum Phases
{
	PHASE_NORMAL = 1,
	PHASE_ILLUSION
};

enum Actions
{
	ACTION_ILLUSION = 1,
	ACTION_TRUE_JANDICE
};

enum Sounds
{
	SOUND_ONAGGRO = 8799,
	SOUND_ONSLAY = 8800,
	SOUND_ONDEATH = 12399,
	SOUND_ONILLUSIONS = 8795
};

struct Location
{
	float x, y, z;
};

static Location IllusionSpawnPos[] =
{
	{ 614.320129f, -1196.578369f, -195.74209f },
	{ 601.549133f, -1192.229248f, -195.820587f },
	{ 622.807800f, -1186.656372f, -195.792816f },
	{ 588.411255f, -1187.817505f, -195.791550f },
	{ 611.683533f, -1183.255615f, -195.895416f },
	{ 597.562683f, -1177.918091f, -195.950333f },
	{ 624.901428f, -1175.483154f, -194.975449f },
	{ 585.342712f, -1174.706055f, -195.153870f },
	{ 610.294312f, -1171.010742f, -195.44565f },
	{ 597.648315f, -1166.847778f, -196.644485f },
	{ 586.601868f, -1163.511841f, -195.558884f },
	{ 588.014954f, -1152.931152f, -197.779037f },
	{ 598.814392f, -1155.954956f, -198.222992f },
	{ 610.608398f, -1158.911743f, -198.010208f },
	{ 624.818909f, -1162.291504f, -194.922241f },
	{ 591.199158f, -1142.802490f, -199.377136f },
	{ 603.989197f, -1145.555908f, -200.052338f },
	{ 616.313843f, -1148.209106f, -199.519012f },
	{ 628.105957f, -1150.553101f, -197.164337f }
};

#define YELL_ON_ENTER_COMBAT "Your old lives. Your mortal desires mean nothing. You are accolites of the master now and you will serve the cause without question. The greatest glory is to die in the master's service."
#define YELL_ON_KILLED_UNIT "You have failed!"
#define YELL_ON_DEATH "Gaaaaaaaahhhaaahhhh!..."
#define YELL_ON_ILLUSION "You cannot hide from me!"

#define EMOTE_ILLUSION "Jandice dissipates into thin air, creating multiple illusions of herself!"
#define EMOTE_FAIL "Jandice enrages!"

class JandiceDespawner : public BasicEvent
{
public:
	explicit JandiceDespawner(Creature* creature) : creature(creature) { }

	bool Execute(uint64 /*currTime*/, uint32 /*diff*/)
	{
		Trinity::CreatureWorker<JandiceDespawner> worker(creature, *this);
		creature->VisitNearbyGridObject(1000.0f, worker);
		return true;
	}

	void operator()(Creature* creature) const
	{
		switch (creature->GetEntry())
		{
		case NPC_JANDICE_BAROV:
			if (InstanceScript* instance = creature->GetInstanceScript())
				instance->SetBossState(DATA_JANDICE_BAROV, FAIL);
			break;
		case NPC_SHADOW_TRAP:
		case NPC_TRUE_JANDICE_ILLUSION:
		case NPC_FALSE_JANDICE_ILLUSION:
		case NPC_ILLUSION_EFFECT_TRIGGER:
			creature->DespawnOrUnsummon();
			return;
			creature->Respawn(true);
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

		creature->DespawnOrUnsummon(1);
		creature->SetCorpseDelay(corpseDelay);
		creature->SetRespawnDelay(respawnDelay);
	}

private:
	Creature* creature;
};

class boss_jandice_barov_mc : public CreatureScript
{
public:
	boss_jandice_barov_mc() : CreatureScript("boss_jandice_barov_mc") { }

	struct boss_jandice_barov_mcAI : public BossAI
	{
		boss_jandice_barov_mcAI(Creature* c) : BossAI(c, DATA_JANDICE_BAROV) { }

		void Reset()
		{
			events.Reset();
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			WonderousRapidityCount = 0;
			illusionCount = 0;
			me->SetVisible(true);
		}

		void EnterCombat(Unit* /*who*/)
		{
			instance->SetBossState(DATA_JANDICE_BAROV, IN_PROGRESS);
			me->Yell(YELL_ON_ENTER_COMBAT, LANG_UNIVERSAL);
			DoPlaySoundToSet(me, SOUND_ONAGGRO);
			events.ScheduleEvent(EVENT_WONDEROUS_RAPIDITY, urand(12500, 17500), 0, PHASE_NORMAL);
			events.ScheduleEvent(EVENT_GRAVITY_FLUX, urand(20000, 25000), 0, PHASE_NORMAL);
			events.ScheduleEvent(EVENT_ENRAGE, 480000);
		}

		void JustDied(Unit* /*killer*/)
		{
			instance->SetBossState(DATA_JANDICE_BAROV, DONE);
			me->Yell(YELL_ON_DEATH, LANG_UNIVERSAL);
			DoPlaySoundToSet(me, SOUND_ONDEATH);
		}

		void KilledUnit(Unit* /*victim*/)
		{
			me->Yell(YELL_ON_KILLED_UNIT, LANG_UNIVERSAL);
			DoPlaySoundToSet(me, SOUND_ONSLAY);
		}

		void EnterEvadeMode()
		{
			me->m_Events.AddEvent(new JandiceDespawner(me), me->m_Events.CalculateTime(1));
		}

		void DamageTaken(Unit* /*attacker*/, uint32& damage)
		{
			if (me->HealthBelowPctDamaged(75, damage) && illusionCount == 0)
				DoAction(ACTION_ILLUSION);
			else if (me->HealthBelowPctDamaged(50, damage) && illusionCount == 1)
				DoAction(ACTION_ILLUSION);
			else if (me->HealthBelowPctDamaged(25, damage) && illusionCount == 2)
				DoAction(ACTION_ILLUSION);
		}

		void DoAction(int32 action)
		{
			switch (action)
			{
			case ACTION_TRUE_JANDICE:
				events.SetPhase(PHASE_NORMAL);
				summons.DespawnAll();
				me->GetMotionMaster()->Clear();
				me->GetMotionMaster()->MoveChase(me->GetVictim());
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				me->SetVisible(true);
				events.ScheduleEvent(EVENT_WONDEROUS_RAPIDITY, urand(7500, 10000), 0, PHASE_NORMAL);
				events.ScheduleEvent(EVENT_GRAVITY_FLUX, urand(12500, 17500), 0, PHASE_NORMAL);
				break;
			case ACTION_ILLUSION:
				events.SetPhase(PHASE_ILLUSION);
				me->TextEmote(EMOTE_ILLUSION, 0, true);
				me->Yell(YELL_ON_ILLUSION, LANG_UNIVERSAL);
				DoPlaySoundToSet(me, SOUND_ONILLUSIONS);
				summons.DespawnAll();
				SummonIllusions();
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				me->RemoveAllAuras();
				me->SetVisible(false);
				WonderousRapidityCount = 0;
				++illusionCount;
				break;
			}
		}

		void SummonIllusions(uint32 maxIndex = 19)
		{
			for (uint8 i = 0; i < maxIndex; ++i)
			{
				me->SummonCreature(NPC_FALSE_JANDICE_ILLUSION, IllusionSpawnPos[i].x, IllusionSpawnPos[i].y, IllusionSpawnPos[i].z);
				me->SummonCreature(NPC_ILLUSION_EFFECT_TRIGGER, IllusionSpawnPos[i].x, IllusionSpawnPos[i].y, IllusionSpawnPos[i].z);
			}

			int RandIndex = rand() % maxIndex;
			me->SummonCreature(NPC_TRUE_JANDICE_ILLUSION, IllusionSpawnPos[RandIndex].x, IllusionSpawnPos[RandIndex].y, IllusionSpawnPos[RandIndex].z);
		}

		void UpdateAI(uint32 diff)
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (me->IsVisible())
				DoMeleeAttackIfReady();

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_WONDEROUS_RAPIDITY:
					if (WonderousRapidityCount == 0)
					{
						me->GetMotionMaster()->Clear(false);
						me->GetMotionMaster()->MoveIdle();
						DoCast(SPELL_WONDEROUS_RAPIDITY_START);
					}
					else if (WonderousRapidityCount <= 5)
					{
						DoCastVictim(SPELL_WONDEROUS_RAPIDITY);
						++WonderousRapidityCount;
						events.ScheduleEvent(EVENT_WONDEROUS_RAPIDITY, 1000, 0, PHASE_NORMAL);
					}
					else if (WonderousRapidityCount >= 5)
					{
						me->GetMotionMaster()->Clear();
						me->GetMotionMaster()->MoveChase(me->GetVictim());
						me->RemoveAura(SPELL_WONDEROUS_RAPIDITY_START);
						WonderousRapidityCount = 0;
						events.CancelEvent(EVENT_WONDEROUS_RAPIDITY);
					}
					break;
				case EVENT_GRAVITY_FLUX:
					if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
						DoCast(target, SPELL_GRAVITY_FLUX);
					events.ScheduleEvent(EVENT_GRAVITY_FLUX, urand(30000, 35000), 0, PHASE_NORMAL);
					break;
				case EVENT_ENRAGE:
					DoCast(SPELL_BERSERK);
					me->TextEmote(EMOTE_FAIL, 0, true);
					events.CancelEvent(EVENT_ENRAGE);
				}
			}
		}

	private:
		EventMap events;
		uint32 WonderousRapidityCount;
		uint32 illusionCount;
	};

	CreatureAI* GetAI(Creature* c) const
	{
		return GetMoltenCoreAI<boss_jandice_barov_mcAI>(c);
	}
};

struct IllusionAI : PassiveAI
{
	IllusionAI(Creature* c) : PassiveAI(c)
	{
		instance = me->GetInstanceScript();
		events.Reset();

		switch (me->GetEntry())
		{
		case NPC_TRUE_JANDICE_ILLUSION:
			DespawnCreatures(NPC_FALSE_JANDICE_ILLUSION);
			events.ScheduleEvent(EVENT_WHIRLWIND, 1750);
			break;
		case NPC_FALSE_JANDICE_ILLUSION:
			events.ScheduleEvent(EVENT_WHIRLWIND, 1750);
			break;
		case NPC_ILLUSION_EFFECT_TRIGGER:
			DoCast(SPELL_SUMMON_VISUAL_1);
			DoCast(SPELL_SUMMON_VISUAL_2);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
			break;
		}
	}

	void DamageTaken(Unit* /*attacker*/, uint32& damage)
	{
		if (me->HealthBelowPctDamaged(1, damage))
			if (damage > me->GetHealth())
			{
				DespawnCreatures(NPC_ILLUSION_EFFECT_TRIGGER);
				switch (me->GetEntry())
				{
				case NPC_FALSE_JANDICE_ILLUSION:
					damage = 0;
					DoCast(SPELL_SPIRIT_BURST);
					break;
				case NPC_TRUE_JANDICE_ILLUSION:
					if (Creature* jandice = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_JANDICE_BAROV)))
						jandice->AI()->DoAction(ACTION_TRUE_JANDICE);
					break;
				}
			}
	}

	bool IsImmunedToSpellEffect(SpellInfo const* spellInfo, uint32 index) const
	{
		switch (spellInfo->Effects[index].ApplyAuraName)
		{
		case SPELL_AURA_PERIODIC_DAMAGE:
			return true;
		default:
			break;
		}
		return IsImmunedToSpellEffect(spellInfo, index);
	}

	void DespawnCreatures(uint32 entry, float distance = 1.0f, bool discs = false)
	{
		std::list<Creature*> creatureList;
		GetCreatureListWithEntryInGrid(creatureList, me, entry, distance);

		if (creatureList.empty())
			return;

		for (std::list<Creature*>::iterator itr = creatureList.begin(); itr != creatureList.end(); ++itr)
			(*itr)->DespawnOrUnsummon();
	}

	void UpdateAI(uint32 diff)
	{
		events.Update(diff);

		while (uint32 eventId = events.ExecuteEvent())
		{
			switch (eventId)
			{
			case EVENT_WHIRLWIND:
				DoCast(SPELL_WHIRLWIND_DAMAGE);
				events.CancelEvent(EVENT_WHIRLWIND);
				break;
			}
		}
	}

private:
	InstanceScript* instance;
	EventMap events;
};

class npc_false_jandice_illusion : public CreatureScript {
public: npc_false_jandice_illusion() : CreatureScript("npc_false_jandice_illusion") { }
		struct npc_false_jandice_illusionAI : public IllusionAI { npc_false_jandice_illusionAI(Creature* c) : IllusionAI(c) { } };
		CreatureAI* GetAI(Creature* c) const { return GetMoltenCoreAI<npc_false_jandice_illusionAI>(c); }
};

class npc_true_jandice_illusion : public CreatureScript {
public: npc_true_jandice_illusion() : CreatureScript("npc_true_jandice_illusion") { }
		struct npc_true_jandice_illusionAI : public IllusionAI { npc_true_jandice_illusionAI(Creature* c) : IllusionAI(c) { } };
		CreatureAI* GetAI(Creature* c) const { return GetMoltenCoreAI<npc_true_jandice_illusionAI>(c); }
};

class npc_illusion_effect_trigger : public CreatureScript {
public: npc_illusion_effect_trigger() : CreatureScript("npc_illusion_effect_trigger") { }
		struct npc_illusion_effect_triggerAI : public IllusionAI { npc_illusion_effect_triggerAI(Creature* c) : IllusionAI(c) { } };
		CreatureAI* GetAI(Creature* c) const { return GetMoltenCoreAI<npc_illusion_effect_triggerAI>(c); }
};

void AddSC_boss_jandice_barov_mc()
{
	new boss_jandice_barov_mc();
	new npc_false_jandice_illusion();
	new npc_true_jandice_illusion();
	new npc_illusion_effect_trigger();
}