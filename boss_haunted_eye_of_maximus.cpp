/* ScriptData
SDCreator: frostI
SDName: boss_haunted_eye_of_maximus
SD%Complete: 80
SDComment:
SDCategory: Molten Core
EndScriptData */

#include "ScriptPCH.h"
#include "molten_core.h"

enum Spells
{
	//Haunted eye of maximus
	SPELL_CORRUPTING_BLIGHT = 60588,
	SPELL_CURSE_OF_TWISTED_FLESH = 58845,
	SPELL_SEAR_BEAM = 59509,
	SPELL_FEL_RAGE_ONE = 40604,
	SPELL_FEL_RAGE_TWO = 46787,
	SPELL_TAUNT_GURTOGG = 40603,
	SPELL_BLOOD_MIRROR = 70445,
	SPELL_BERSERK = 68378,
	//Haunted pumpkin				 
	SPELL_PUMPKIN_BLAST = 50066,
	SPELL_PUMPKIN_HEAD = 44016,
	//Haunted ghost					 
	SPELL_SPIRIT_BLAST = 73806,
	//Nightmare of spiders			 
	SPELL_TANGLED_WEBS = 56632,
	//Nightmare of zombies			 
	SPELL_VAMPIRIC_CURSE = 70423,
	//Eruption trigger				 
	SPELL_NAPALM_SHELL = 65026
};

enum Events
{
	//Haunted eye of maximus
	EVENT_SEAR_BEAM = 1,
	EVENT_CURSE_OF_TWISTED_FLESH,
	EVENT_CORRUPTING_BLIGHT,
	EVENT_SUMMON_NIGHTMARES,
	EVENT_FELRAGE,
	EVENT_ENRAGE,
	//Nightmare of zombies
	EVENT_VAMPIRIC_CURSE,
	//Nightmare of spiders
	EVENT_TANGLED_WEBS,
	EVENT_START_TANGLED_WEBS,
	//Haunted ghost
	EVENT_SUMMON_ABANDONED_GHOSTS,
	EVENT_SPIRIT_BLAST,
	//Haunted pumpkin
	EVENT_SUMMON_HALLOWED_PUMPKINS,
	EVENT_PUMPKIN_BLAST,
	EVENT_PUMPKIN_HEAD,
	//Eruption trigger
	EVENT_NAPALM_SHELL
};

enum Actions
{
	ACTION_TANGLED_WEBS = 1,
	ACTION_TARGET_PLAYER,
	ACTION_EXPLODE
};

enum Sounds
{
	SOUND_ONAGGRO = 8582,
	SOUND_ONSLAY = 8586,
	SOUND_ONCOMBAT1 = 8585,
	SOUND_ONCOMBAT2 = 8587
};

struct Location
{
	float x, y, z;
};

static Location NightmareOfHallowsEndSpawn = { 717.372803f, -547.677551f, -216.536270f };
static Location NightmareOfSpidersSpawn = { 724.692688f, -540.078934f, -217.078934f };
static Location NightmareOfGhostsSpawn = { 732.698181f, -532.406067f, -216.718704f };
static Location NightmareOfZombiesSpawn = { 738.402344f, -524.168884f, -216.300476f };
static Location HallowedPumpkinSpawn = { 711.7230022f, -528.443970f, -217.046997f };
static Location AbandonedGhostSpawn = { 727.051025f, -507.550995f, -216.931000f };

#define EMOTE_NIGHTMARES    "Which nightmare do you prefer?"
#define EMOTE_SPIDERS       "A nightmare of spiders has come true!"  
#define EMOTE_ZOMBIES       "A nightmare of zombies has come true!"  
#define EMOTE_GHOSTS        "A nightmare of ghosts has come true!"  
#define EMOTE_HALLOWSEND    "A nightmare of Hallow's End has come true!"

class HauntedEyeDespawner : public BasicEvent
{
public:
	explicit HauntedEyeDespawner(Creature* creature) : creature(creature) { }

	bool Execute(uint64 /*currTime*/, uint32 /*diff*/)
	{
		Trinity::CreatureWorker<HauntedEyeDespawner> worker(creature, *this);
		creature->VisitNearbyGridObject(1000.0f, worker);
		return true;
	}

	void operator()(Creature* creature) const
	{
		switch (creature->GetEntry())
		{
		case NPC_HAUNTED_EYE_OF_MAXIMUS:
			if (InstanceScript* instance = creature->GetInstanceScript())
			{
				instance->SetBossState(DATA_HAUNTED_EYE_OF_MAXIMUS, FAIL);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTING_BLIGHT);
			}
			break;
		case NPC_NIGHTMARE_OF_GHOSTS:
		case NPC_NIGHTMARE_OF_SPIDERS:
		case NPC_NIGHTMARE_OF_HALLOWS_END:
		case NPC_NIGHTMARE_OF_ZOMBIES:
		case NPC_ABANDONED_GHOST:
		case NPC_HALLOWED_PUMPKIN:
			creature->DespawnOrUnsummon(1);
			return;
		case NPC_VOLCANO_1:
			if (!creature->GetDBTableGUIDLow())
			{
				creature->DespawnOrUnsummon(1);
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

class boss_haunted_eye_of_maximus : public CreatureScript
{
public:
	boss_haunted_eye_of_maximus() : CreatureScript("boss_haunted_eye_of_maximus"){ }

	struct boss_haunted_eye_of_maximusAI : public BossAI
	{
		boss_haunted_eye_of_maximusAI(Creature* creature) : BossAI(creature, DATA_HAUNTED_EYE_OF_MAXIMUS)
		{
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
		}

		void Reset()
		{
			events.Reset();
			tangledWebsCount = 0;
			explodeCount = 0;
		}

		void EnterCombat(Unit* /*who*/)
		{
			instance->SetBossState(DATA_HAUNTED_EYE_OF_MAXIMUS, IN_PROGRESS);
			me->Yell("Your currage will pail.", LANG_UNIVERSAL);
			DoPlaySoundToSet(me, SOUND_ONAGGRO);
			SelectCreatureForAction(NPC_VOLCANO_1, 333.0f, ACTION_TARGET_PLAYER);
			events.ScheduleEvent(EVENT_CORRUPTING_BLIGHT, urand(25000, 35000));
			events.ScheduleEvent(EVENT_SEAR_BEAM, urand(5000, 7500));
			events.ScheduleEvent(EVENT_CURSE_OF_TWISTED_FLESH, urand(7500, 12500));
			events.ScheduleEvent(EVENT_SUMMON_NIGHTMARES, 45000);
			events.ScheduleEvent(EVENT_ENRAGE, 480000);
		}

		void JustDied(Unit* /*killer*/)
		{
			instance->SetBossState(DATA_HAUNTED_EYE_OF_MAXIMUS, DONE);
			DespawnCreatures(NPC_VOLCANO_1, 333.0f);
		}

		void KilledUnit(Unit* /*victim*/)
		{
			me->Yell("You are weak.", LANG_UNIVERSAL);
			DoPlaySoundToSet(me, SOUND_ONSLAY);
		}

		void EnterEvadeMode()
		{
			me->m_Events.AddEvent(new HauntedEyeDespawner(me), me->m_Events.CalculateTime(1));
		}

		void DamageTaken(Unit* /*attacker*/, uint32& damage)
		{
			if (me->HealthBelowPctDamaged(1, damage))
				if (damage > me->GetHealth())
					instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTING_BLIGHT);

			if (me->HealthBelowPctDamaged(75, damage) && explodeCount == 0)
				DoAction(ACTION_EXPLODE);
			else if (me->HealthBelowPctDamaged(50, damage) && explodeCount == 1)
				DoAction(ACTION_EXPLODE);
			else if (me->HealthBelowPctDamaged(25, damage) && explodeCount == 2)
				DoAction(ACTION_EXPLODE);
		}

		void DoAction(int32 action)
		{
			switch (action)
			{
			case ACTION_TANGLED_WEBS:
				events.ScheduleEvent(EVENT_TANGLED_WEBS, 1000);
				break;
			case ACTION_EXPLODE:
				Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me));
				if (target && target->IsWithinMeleeRange(me, 5.0f))
				{
					me->Yell("Your heart will explode.", LANG_UNIVERSAL);
					DoPlaySoundToSet(me, SOUND_ONCOMBAT2);
					target->CastSpell(me, SPELL_TAUNT_GURTOGG);
					target->CastSpell(me, SPELL_BLOOD_MIRROR);
					DoCast(target, SPELL_FEL_RAGE_ONE);
					DoCast(target, SPELL_FEL_RAGE_TWO);
					++explodeCount;
				}
				break;
			}
		}

		void JustSummoned(Creature* summon)
		{
			summons.Summon(summon);
			switch (summon->GetEntry())
			{
			case NPC_NIGHTMARE_OF_SPIDERS:
				summon->GetMotionMaster()->MoveChase(me);
				break;
			case NPC_NIGHTMARE_OF_ZOMBIES:
				summon->GetMotionMaster()->MoveChase(me);
				break;
			case NPC_NIGHTMARE_OF_GHOSTS:
				summon->GetMotionMaster()->MoveChase(me);
				break;
			case NPC_NIGHTMARE_OF_HALLOWS_END:
				summon->GetMotionMaster()->MoveChase(me);
				break;
			}
		}

		void DistanceCheck(float distance, uint64 t_offset)
		{
			Map* map = me->GetMap();
			if (me->IsInCombat())
			{
				Map::PlayerList const &PlayerList = map->GetPlayers();
				for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
					if (!me->IsWithinDist(itr->GetSource(), distance))
						me->m_Events.AddEvent(new HauntedEyeDespawner(me), me->m_Events.CalculateTime(t_offset));
			}
		}

		void SelectCreatureForAction(uint32 entry, float distance, uint32 action)
		{
			std::list<Creature*> m_pCreatures;
			GetCreatureListWithEntryInGrid(m_pCreatures, me, entry, distance);

			if (m_pCreatures.empty())
				return;

			for (std::list<Creature*>::iterator iter = m_pCreatures.begin(); iter != m_pCreatures.end(); ++iter)
				(*iter)->AI()->DoAction(action);
		}

		void DespawnCreatures(uint32 entry, float distance = 1.0f, bool discs = false)
		{
			std::list<Creature*> creatureList;
			GetCreatureListWithEntryInGrid(creatureList, me, entry, distance);

			if (creatureList.empty())
				return;

			for (std::list<Creature*>::iterator itr = creatureList.begin(); itr != creatureList.end(); ++itr)
				(*itr)->DespawnOrUnsummon(1);
		}

		void UpdateAI(uint32 diff)
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);

			DistanceCheck(60.0f, 1);

			if (!me->GetVictim()->IsWithinMeleeRange(me))
				me->SelectNearestTargetInAttackDistance();

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_SUMMON_NIGHTMARES:
					me->TextEmote(EMOTE_NIGHTMARES, 0, true);
					me->SummonCreature(NPC_NIGHTMARE_OF_HALLOWS_END, NightmareOfHallowsEndSpawn.x, NightmareOfHallowsEndSpawn.y, NightmareOfHallowsEndSpawn.z, 0, TEMPSUMMON_TIMED_DESPAWN, 15000);
					me->SummonCreature(NPC_NIGHTMARE_OF_SPIDERS, NightmareOfSpidersSpawn.x, NightmareOfSpidersSpawn.y, NightmareOfSpidersSpawn.z, 0, TEMPSUMMON_TIMED_DESPAWN, 15000);
					me->SummonCreature(NPC_NIGHTMARE_OF_ZOMBIES, NightmareOfZombiesSpawn.x, NightmareOfZombiesSpawn.y, NightmareOfZombiesSpawn.z, 0, TEMPSUMMON_TIMED_DESPAWN, 15000);
					me->SummonCreature(NPC_NIGHTMARE_OF_GHOSTS, NightmareOfGhostsSpawn.x, NightmareOfGhostsSpawn.y, NightmareOfGhostsSpawn.z, 0, TEMPSUMMON_TIMED_DESPAWN, 15000);
					events.ScheduleEvent(EVENT_SUMMON_NIGHTMARES, 60000);
					break;
				case EVENT_CORRUPTING_BLIGHT:
					me->Yell("You will die.", LANG_UNIVERSAL);
					DoPlaySoundToSet(me, SOUND_ONCOMBAT1);
					DoCastVictim(SPELL_CORRUPTING_BLIGHT);
					events.ScheduleEvent(EVENT_CORRUPTING_BLIGHT, urand(25000, 35000));
					break;
				case EVENT_SEAR_BEAM:
					if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
						DoCast(target, SPELL_SEAR_BEAM);
					events.ScheduleEvent(EVENT_SEAR_BEAM, urand(2500, 7500));
					break;
				case EVENT_CURSE_OF_TWISTED_FLESH:
					if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
						DoCast(target, SPELL_CURSE_OF_TWISTED_FLESH);
					events.ScheduleEvent(EVENT_CURSE_OF_TWISTED_FLESH, urand(7500, 12500));
					break;
				case EVENT_TANGLED_WEBS:
					if (tangledWebsCount < 4)
					{
						DoCast(me, SPELL_TANGLED_WEBS);
						++tangledWebsCount;
						events.ScheduleEvent(EVENT_TANGLED_WEBS, 8000);
					}
					else
					{
						tangledWebsCount = 0;
						events.CancelEvent(EVENT_TANGLED_WEBS);
					}
					break;
				case EVENT_ENRAGE:
					DoCast(SPELL_BERSERK);
					events.CancelEvent(EVENT_ENRAGE);
					break;
				}
			}
		}

	private:
		EventMap events;
		uint32 tangledWebsCount;
		uint32 explodeCount;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return GetMoltenCoreAI<boss_haunted_eye_of_maximusAI>(creature);
	}
};

struct NightmareAI : PassiveAI
{
	NightmareAI(Creature* c) : PassiveAI(c)
	{
		instance = me->GetInstanceScript();
		events.Reset();

		switch (me->GetEntry())
		{
		case NPC_NIGHTMARE_OF_HALLOWS_END:
			events.ScheduleEvent(EVENT_SUMMON_HALLOWED_PUMPKINS, 15000);
			break;
		case NPC_NIGHTMARE_OF_SPIDERS:
			events.ScheduleEvent(EVENT_START_TANGLED_WEBS, 15000);
			break;
		case NPC_NIGHTMARE_OF_ZOMBIES:
			events.ScheduleEvent(EVENT_VAMPIRIC_CURSE, 15000);
			break;
		case NPC_NIGHTMARE_OF_GHOSTS:
			events.ScheduleEvent(EVENT_START_TANGLED_WEBS, 15000);
			break;
		}
	}

	InstanceScript* instance;
	EventMap events;

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

	void UpdateAI(uint32 diff)
	{
		events.Update(diff);

		while (uint32 eventId = events.ExecuteEvent())
		{
			switch (eventId)
			{
			case EVENT_SUMMON_HALLOWED_PUMPKINS:
				me->TextEmote(EMOTE_HALLOWSEND, 0, true);
				instance->DoCastSpellOnPlayers(SPELL_PUMPKIN_HEAD);
				for (uint8 i = 0; i < 5; ++i)
					me->SummonCreature(NPC_HALLOWED_PUMPKIN, HallowedPumpkinSpawn.x, HallowedPumpkinSpawn.y, HallowedPumpkinSpawn.z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
				events.CancelEvent(EVENT_SUMMON_HALLOWED_PUMPKINS);
				break;
			case EVENT_VAMPIRIC_CURSE:
				me->TextEmote(EMOTE_ZOMBIES, 0, true);
				instance->DoCastSpellOnPlayers(SPELL_VAMPIRIC_CURSE);
				events.CancelEvent(EVENT_VAMPIRIC_CURSE);
				break;
			case EVENT_START_TANGLED_WEBS:
				me->TextEmote(EMOTE_SPIDERS, 0, true);
				if (Creature* hauntedEyeOfMaximus = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_HAUNTED_EYE_OF_MAXIMUS)))
					hauntedEyeOfMaximus->AI()->DoAction(ACTION_TANGLED_WEBS);
				events.CancelEvent(EVENT_START_TANGLED_WEBS);
				break;
			case EVENT_SUMMON_ABANDONED_GHOSTS:
				me->TextEmote(EMOTE_GHOSTS, 0, true);
				for (uint8 i = 0; i < 5; ++i)
					me->SummonCreature(NPC_ABANDONED_GHOST, AbandonedGhostSpawn.x, AbandonedGhostSpawn.y, AbandonedGhostSpawn.z);
				events.CancelEvent(EVENT_SUMMON_ABANDONED_GHOSTS);
				break;
			}
		}
	}
};

class npc_nightmare_of_hallows_end : public CreatureScript {
public: npc_nightmare_of_hallows_end() : CreatureScript("npc_nightmare_of_hallows_end") { }
		struct npc_nightmare_of_hallows_endAI : public NightmareAI { npc_nightmare_of_hallows_endAI(Creature* c) : NightmareAI(c) { } };
		CreatureAI* GetAI(Creature* c) const { return GetMoltenCoreAI<npc_nightmare_of_hallows_endAI>(c); }
};

class npc_nightmare_of_spiders : public CreatureScript {
public: npc_nightmare_of_spiders() : CreatureScript("npc_nightmare_of_spiders") { }
		struct npc_nightmare_of_spidersAI : public NightmareAI { npc_nightmare_of_spidersAI(Creature* c) : NightmareAI(c) { } };
		CreatureAI* GetAI(Creature* c) const { return GetMoltenCoreAI<npc_nightmare_of_spidersAI>(c); }
};

class npc_nightmare_of_zombies : public CreatureScript {
public: npc_nightmare_of_zombies() : CreatureScript("npc_nightmare_of_zombies") { }
		struct npc_nightmare_of_zombiesAI : public NightmareAI { npc_nightmare_of_zombiesAI(Creature* c) : NightmareAI(c) { } };
		CreatureAI* GetAI(Creature* c) const { return GetMoltenCoreAI<npc_nightmare_of_zombiesAI>(c); }
};

class npc_nightmare_of_ghosts : public CreatureScript {
public: npc_nightmare_of_ghosts() : CreatureScript("npc_nightmare_of_ghosts") { }
		struct npc_nightmare_of_ghostsAI : public NightmareAI { npc_nightmare_of_ghostsAI(Creature* c) : NightmareAI(c) { } };
		CreatureAI* GetAI(Creature* c) const { return GetMoltenCoreAI<npc_nightmare_of_ghostsAI>(c); }
};

class npc_abandoned_ghost : public CreatureScript
{
public:
	npc_abandoned_ghost() : CreatureScript("npc_abandoned_ghost") { }

	struct npc_abandoned_ghostAI : public ScriptedAI
	{
		npc_abandoned_ghostAI(Creature* c) : ScriptedAI(c)
		{
			me->GetMotionMaster()->MoveRandom(25.0f);
		}

		void DamageTaken(Unit* /*attacker*/, uint32& damage)
		{
			if (me->HealthBelowPctDamaged(1, damage))
				if (damage > me->GetHealth())
				{
					damage = 0;
					DoCast(SPELL_SPIRIT_BLAST);
					me->DespawnOrUnsummon(5000);
				}
		}

		void UpdateAI(uint32 /*diff*/)
		{
			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* c) const
	{
		return GetMoltenCoreAI<npc_abandoned_ghostAI>(c);
	}
};

class npc_hallowed_pumpkin : public CreatureScript
{
public:
	npc_hallowed_pumpkin() : CreatureScript("npc_hallowed_pumpkin"){ }

	struct npc_hallowed_pumpkinAI : public ScriptedAI
	{
		npc_hallowed_pumpkinAI(Creature* c) : ScriptedAI(c)
		{
			events.Reset();
			me->GetMotionMaster()->MoveRandom(25.0f);
			events.ScheduleEvent(EVENT_PUMPKIN_BLAST, urand(1500, 3000));
		}

		void UpdateAI(uint32 diff)
		{
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_PUMPKIN_BLAST:
					if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
						DoCast(target, SPELL_PUMPKIN_BLAST);
					events.ScheduleEvent(EVENT_PUMPKIN_BLAST, urand(1500, 3000));
					break;
				}
			}

			DoMeleeAttackIfReady();
		}

	private:
		EventMap events;
	};

	CreatureAI* GetAI(Creature* c) const
	{
		return GetMoltenCoreAI<npc_hallowed_pumpkinAI>(c);
	}
};

class npc_volcano_1 : public CreatureScript
{
public:
	npc_volcano_1() : CreatureScript("npc_volcano_1") { }

	struct npc_volcano_1AI : public ScriptedAI
	{
		npc_volcano_1AI(Creature* c) : ScriptedAI(c)
		{
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
		}

		void Reset()
		{
			events.Reset();
		}

		void DoAction(int32 action)
		{
			switch (action)
			{
			case ACTION_TARGET_PLAYER:
				me->AI()->DoZoneInCombat(0, 250.0f);
				break;
			}
		}

		void EnterCombat(Unit* /*who*/)
		{
			events.ScheduleEvent(EVENT_NAPALM_SHELL, urand(10000, 30000));
		}

		void UpdateAI(uint32 diff)
		{
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_NAPALM_SHELL:
					if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 333.0f, true))
						if (me->IsWithinLOS(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ()))
							DoCast(target, SPELL_NAPALM_SHELL);
					events.ScheduleEvent(EVENT_NAPALM_SHELL, urand(10000, 30000));
					break;
				}
			}
		}

	private:
		EventMap events;
	};

	CreatureAI* GetAI(Creature* c) const
	{
		return GetMoltenCoreAI<npc_volcano_1AI>(c);
	}
};

void AddSC_boss_haunted_eye_of_maximus()
{
	new boss_haunted_eye_of_maximus();
	new npc_nightmare_of_hallows_end();
	new npc_nightmare_of_spiders();
	new npc_nightmare_of_zombies();
	new npc_nightmare_of_ghosts();
	new npc_abandoned_ghost();
	new npc_hallowed_pumpkin();
	new npc_volcano_1();
}