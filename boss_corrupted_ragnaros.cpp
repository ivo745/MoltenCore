/* ScriptData
SDCreator: frostI
SDName: Boss_Ragnaros
SD%Complete: 0
SDComment:
SDCategory: Molten Core
EndScriptData */

#include "ScriptPCH.h"
#include "MapManager.h"
#include "Spell.h"
#include "molten_core.h"

enum Spells
{
	SPELL_EMERGE = 20568,
	SPELL_FLAME_DART = 45737,
	SPELL_METEOR_STRIKE = 74637,
	SPELL_METEOR_STRIKE_COUNTDOWN = 74641,
	SPELL_METEOR_STRIKE_AOE_DAMAGE = 74648,
	SPELL_METEOR_STRIKE_FIRE_AURA_1 = 74713,
	SPELL_METEOR_STRIKE_FIRE_AURA_2 = 74718,
	SPELL_BIRTH_NO_VISUAL = 40031,
	SPELL_TWILIGHT_AURA = 52693,
	SPELL_PORTAL_VISUAL = 51807,
	SPELL_TWILIGHT_PULSE = 46087,
	SPELL_LIVING_METEOR_VISUAL = 45909,
};

enum Events
{
	//Normal
	EVENT_INTRO_1 = 1,
	EVENT_INTRO_2 = 2,
	EVENT_FLAME_DART = 3,
	EVENT_SUBMERGE = 4,
	EVENT_ATTACK_START = 5,
	EVENT_METEOR_STRIKE = 6,
	EVENT_SPAWN_METEOR_FLAME = 7,
	//Twilight
	EVENT_SUMMON_TWILIGHT_PORTAL = 8,
	EVENT_TWILIGHT_PULSE = 9,
};

enum Actions
{
	ACTION_START_METEOR_STRIKE = 1,
	ACTION_METEOR_STRIKE_BURN = 2,
	ACTION_METEOR_STRIKE_AOE = 3,
	ACTION_STOP_METEOR_STRIKE = 4,
};

Position const TwilightPortalSpawnPos = { 821.423340f, -847.826965f, -229.203964f };

#define	SOUND_INTRO          8045    //Now for you insects, boldly sat the power of Ragnaros, now you shall see it first hand!
#define	SOUND_ONSLAY         8051    //Die... insect!
#define SOUND_COMBAT_2       8046    //By fire, be purged!
#define SOUND_FLAMES         8047    //Taste the flames of sulfuron!
#define SOUND_FLAME_HIT      8049    //Come forth my servants, defend your master!

class RagnarosDespawner : public BasicEvent
{
public:
	explicit RagnarosDespawner(Creature* creature) : creature(creature){ }

	bool Execute(uint64 /*currTime*/, uint32 /*diff*/)
	{
		Trinity::CreatureWorker<RagnarosDespawner> worker(creature, *this);
		creature->VisitNearbyGridObject(600.0f, worker);
		return true;
	}

	void operator()(Creature* creature) const
	{
		switch (creature->GetEntry())
		{
		case NPC_CORRUPTED_RAGNAROS:
			if (InstanceScript* instance = creature->GetInstanceScript())
				instance->SetBossState(DATA_CORRUPTED_RAGNAROS, FAIL);
			break;
		case NPC_RAGNAROS_METEOR_STRIKE_MARK:
		case NPC_RAGNAROS_METEOR_STRIKE_NORTH:
		case NPC_RAGNAROS_METEOR_STRIKE_EAST:
		case NPC_RAGNAROS_METEOR_STRIKE_WEST:
		case NPC_RAGNAROS_METEOR_STRIKE_SOUTH:
		case NPC_RAGNAROS_METEOR_STRIKE_FLAME:
			creature->DespawnOrUnsummon(1);
			return;
		case NPC_ALEXSTRASZA_2:
		case NPC_RAGNAROS_CONTROLLER:
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

class boss_corrupted_ragnaros : public CreatureScript
{
public:
	boss_corrupted_ragnaros() : CreatureScript("boss_corrupted_ragnaros") { }

	struct boss_corrupted_ragnarosAI : public BossAI
	{
		boss_corrupted_ragnarosAI(Creature* creature) : BossAI(creature, DATA_CORRUPTED_RAGNAROS)
		{
			IntroState = 0;
			me->SetReactState(REACT_PASSIVE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
		}

		void Reset()
		{
			events.Reset();
			Emerge_Timer = 90000;
			me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
			HasSubmergedOnce = false;
			IsBanished = false;
			Twilight = false;
		}

		void EnterCombat(Unit* /*who*/)
		{
			instance->SetBossState(DATA_CORRUPTED_RAGNAROS, IN_PROGRESS);
			events.ScheduleEvent(EVENT_SUBMERGE, 90000);
			events.ScheduleEvent(EVENT_FLAME_DART, urand(7500, 12500));
		}

		void DamageTaken(Unit* /*attacker*/, uint32& damage)
		{
			if (me->HealthBelowPct(50))
			{
				if (!Twilight)
				{
					instance->DoCastSpellOnPlayers(SPELL_TWILIGHT_AURA);
					events.ScheduleEvent(EVENT_SUMMON_TWILIGHT_PORTAL, 10000);
					events.CancelEvent(EVENT_ATTACK_START);
					events.CancelEvent(EVENT_FLAME_DART);
					events.CancelEvent(EVENT_SUBMERGE);
					Twilight = true;
				}
			}

			if (me->HealthBelowPct(1))
			{
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TWILIGHT_AURA);
				damage = 0;
				me->Kill(me);
			}
		}

		void JustSummoned(Creature* summoned)
		{
			switch (summoned->GetEntry())
			{
			case NPC_TWILIGHT_FIELD:
				summoned->AI()->DoZoneInCombat();
				break;
			}
		}

		void EnterEvadeMode()
		{
			me->m_Events.AddEvent(new RagnarosDespawner(me), me->m_Events.CalculateTime(0));
		}

		void KilledUnit(Unit* /*victim*/)
		{
			me->Yell("Die... insect!", LANG_UNIVERSAL);
			DoPlaySoundToSet(me, SOUND_ONSLAY);
		}

		void JustDied(Unit* /*killer*/)
		{
			instance->SetBossState(DATA_CORRUPTED_RAGNAROS, DONE);
		}

		void DistanceCheck(float distance, uint64 t_offset)
		{
			Map* map = me->GetMap();
			if (me->IsInCombat())
			{
				Map::PlayerList const &PlayerList = map->GetPlayers();
				for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
					if (!me->IsWithinDist(i->GetSource(), distance))
					{
						me->m_Events.AddEvent(new RagnarosDespawner(me), me->m_Events.CalculateTime(t_offset));
					}
			}
		}

		void UpdateAI(uint32 diff)
		{
			if (IntroState != 2)
			{
				if (!IntroState)
				{
					me->HandleEmoteCommand(EMOTE_ONESHOT_EMERGE);
					events.ScheduleEvent(EVENT_INTRO_1, 4000);
					events.ScheduleEvent(EVENT_INTRO_2, 15000);
					IntroState = 1;
				}

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case EVENT_INTRO_1:
						me->Yell("Now for you insects, boldly sat the power of Ragnaros, now you shall see it first hand!", LANG_UNIVERSAL);
						DoPlaySoundToSet(me, SOUND_INTRO);
						break;
					case EVENT_INTRO_2:
						me->SetReactState(REACT_AGGRESSIVE);
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
						IntroState = 2;
						break;
					default:
						break;
					}
				}
			}
			else
			{
				if (instance)
				{
					if (IsBanished && Emerge_Timer <= diff)
					{
						me->setFaction(14);
						events.ScheduleEvent(EVENT_ATTACK_START, 4000);
						me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
						DoCast(me, SPELL_EMERGE);
						IsBanished = false;
					}
					else if (IsBanished)
					{
						Emerge_Timer -= diff;
						return;
					}
				}

				if (!UpdateVictim())
					return;

				events.Update(diff);

				DistanceCheck(100.0f, 1);

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case EVENT_SUBMERGE:
						if (instance && !IsBanished)
						{
							me->AttackStop();
							DoResetThreat();
							me->SetReactState(REACT_PASSIVE);
							me->InterruptNonMeleeSpells(false);
							me->setFaction(35);
							me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
							me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_SUBMERGED);
							me->HandleEmoteCommand(EMOTE_ONESHOT_SUBMERGE);
							me->Yell("Taste the flames of Sulfuron!", LANG_UNIVERSAL);
							DoPlaySoundToSet(me, SOUND_FLAMES);
							if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_RAGNAROS_CONTROLLER)))
							{
								controller->AI()->DoAction(ACTION_START_METEOR_STRIKE);
							}
							if (!HasSubmergedOnce)
							{
								HasSubmergedOnce = true;
								IsBanished = true;
								Emerge_Timer = 34000;

							}
							else
							{
								IsBanished = true;
								Emerge_Timer = 34000;
							}
						}
						events.ScheduleEvent(EVENT_SUBMERGE, 90000);
						break;
					case EVENT_ATTACK_START:
						me->SetReactState(REACT_AGGRESSIVE);
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
						if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_RAGNAROS_CONTROLLER)))
						{
							controller->AI()->DoAction(ACTION_STOP_METEOR_STRIKE);
						}
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
							AttackStart(target);
						events.CancelEvent(EVENT_ATTACK_START);
						break;
					case EVENT_FLAME_DART:
						DoCastAOE(SPELL_FLAME_DART);
						events.ScheduleEvent(EVENT_FLAME_DART, urand(7500, 12500));
						break;
					case EVENT_SUMMON_TWILIGHT_PORTAL:
						if (Unit * target = SelectTarget(SELECT_TARGET_FARTHEST, 0, 0, true))
							me->SummonCreature(NPC_TWILIGHT_FIELD, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 30000);
						events.ScheduleEvent(EVENT_SUMMON_TWILIGHT_PORTAL, 10000);
						break;
					default:
						break;
					}
				}
			}

			DoMeleeAttackIfReady();
		}

	private:
		InstanceScript* instance;
		uint8 IntroState;
		uint32 Emerge_Timer;
		bool HasSubmergedOnce;
		bool IsBanished;
		bool Twilight;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new boss_corrupted_ragnarosAI(creature);
	}
};

class npc_ragnaros_controller : public CreatureScript
{
public:
	npc_ragnaros_controller() : CreatureScript("npc_ragnaros_controller") { }

	struct npc_ragnaros_controllerAI : public ScriptedAI
	{
		npc_ragnaros_controllerAI(Creature* creature) : ScriptedAI(creature), summons(me)
		{
			instance = me->GetInstanceScript();
		}

		Position const* GetMeteorStrikePosition() const { return &MeteorStrikePos; }

		void Reset()
		{
			summons.DespawnAll();
			events.Reset();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_DISABLE_MOVE);
		}

		void JustSummoned(Creature* who)
		{
			summons.Summon(who);
		}

		void DoAction(int32 action)
		{
			switch (action)
			{
			case ACTION_START_METEOR_STRIKE:
				events.ScheduleEvent(EVENT_METEOR_STRIKE, 5000);
				break;
			case ACTION_STOP_METEOR_STRIKE:
				events.CancelEvent(EVENT_METEOR_STRIKE);
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
				case EVENT_METEOR_STRIKE:
					if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
					{
						MeteorStrikePos = target->GetPosition();
						me->CastSpell(MeteorStrikePos.GetPositionX(), MeteorStrikePos.GetPositionY(), MeteorStrikePos.GetPositionZ(), SPELL_METEOR_STRIKE, true, NULL, NULL, me->GetGUID());
					}
					events.ScheduleEvent(EVENT_METEOR_STRIKE, 5000);
					break;
				}
			}
		}

	private:
		EventMap events;
		InstanceScript* instance;
		SummonList summons;
		Position MeteorStrikePos;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return GetMoltenCoreAI<npc_ragnaros_controllerAI>(creature);
	}
};

typedef npc_ragnaros_controller::npc_ragnaros_controllerAI ControllerAI;

class npc_ragnaros_meteor_strike_initial : public CreatureScript
{
public:
	npc_ragnaros_meteor_strike_initial() : CreatureScript("npc_ragnaros_meteor_strike_initial") { }

	struct npc_ragnaros_meteor_strike_initialAI : public ScriptedAI
	{
		npc_ragnaros_meteor_strike_initialAI(Creature* creature) : ScriptedAI(creature)
		{
			instance = me->GetInstanceScript();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
		}

		void DoAction(int32 action)
		{
			switch (action)
			{
			case ACTION_METEOR_STRIKE_AOE:
				DoCast(me, SPELL_METEOR_STRIKE_AOE_DAMAGE, true);
				DoCast(me, SPELL_METEOR_STRIKE_FIRE_AURA_1, true);
				for (std::list<Creature*>::iterator itr = MeteorList.begin(); itr != MeteorList.end(); ++itr)
					(*itr)->AI()->DoAction(ACTION_METEOR_STRIKE_BURN);
				break;
			}
		}

		void IsSummonedBy(Unit* summoner)
		{
			Creature* owner = summoner->ToCreature();
			if (!owner)
				return;

			if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_RAGNAROS_CONTROLLER)))
				controller->AI()->JustSummoned(me);

			DoCast(me, SPELL_METEOR_STRIKE_COUNTDOWN);
			DoCast(me, SPELL_BIRTH_NO_VISUAL);

			if (ControllerAI* controllerAI = CAST_AI(ControllerAI, owner->AI()))
			{
				Position const* ownerPos = controllerAI->GetMeteorStrikePosition();
				float angle[4];
				angle[0] = me->GetAngle(ownerPos);
				angle[1] = me->GetAngle(ownerPos) - static_cast<float>(M_PI / 2);
				angle[2] = me->GetAngle(ownerPos) - static_cast<float>(-M_PI / 2);
				angle[3] = me->GetAngle(ownerPos) - static_cast<float>(M_PI);

				MeteorList.clear();
				for (uint8 i = 0; i < 4; i++)
				{
					angle[i] = Position::NormalizeOrientation(angle[i]);
					me->SetOrientation(angle[i]);
					Position newPos = me->GetNearPosition(10.0f, 0.0f);
					if (Creature* meteor = me->SummonCreature(NPC_RAGNAROS_METEOR_STRIKE_NORTH + i, newPos, TEMPSUMMON_TIMED_DESPAWN, 30000))
						MeteorList.push_back(meteor);
				}
			}
		}

	private:
		InstanceScript* instance;
		std::list<Creature*> MeteorList;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return GetMoltenCoreAI<npc_ragnaros_meteor_strike_initialAI>(creature);
	}
};

class npc_ragnaros_meteor_strike : public CreatureScript
{
public:
	npc_ragnaros_meteor_strike() : CreatureScript("npc_ragnaros_meteor_strike") { }

	struct npc_ragnaros_meteor_strikeAI : public ScriptedAI
	{
		npc_ragnaros_meteor_strikeAI(Creature* creature) : ScriptedAI(creature)
		{
			instance = me->GetInstanceScript();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
			Range = 5.0f;
			SpawnCount = 0;
		}

		void DoAction(int32 action)
		{
			switch (action)
			{
			case ACTION_METEOR_STRIKE_BURN:
				DoCast(me, SPELL_METEOR_STRIKE_FIRE_AURA_2, true);
				me->setActive(true);
				events.ScheduleEvent(EVENT_SPAWN_METEOR_FLAME, 500);
				break;
			}
		}

		void IsSummonedBy(Unit* /*summoner*/)
		{
			if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_RAGNAROS_CONTROLLER)))
				controller->AI()->JustSummoned(me);
		}

		void UpdateAI(uint32 diff)
		{
			if (SpawnCount > 5)
				return;

			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_SPAWN_METEOR_FLAME:
					Position pos;
					me->GetNearPosition(Range, 0.0f);
					if (Creature* flame = me->SummonCreature(NPC_RAGNAROS_METEOR_STRIKE_FLAME, pos, TEMPSUMMON_TIMED_DESPAWN, 25000))
					{
						if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_RAGNAROS_CONTROLLER)))
							controller->AI()->JustSummoned(flame);

						flame->CastSpell(flame, SPELL_METEOR_STRIKE_FIRE_AURA_2, true);
						++SpawnCount;
					}
					Range += 5.0f;
					events.ScheduleEvent(EVENT_SPAWN_METEOR_FLAME, 800);
					break;
				}
			}
		}

	private:
		InstanceScript* instance;
		EventMap events;
		float Range;
		uint8 SpawnCount;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return GetMoltenCoreAI<npc_ragnaros_meteor_strikeAI>(creature);
	}
};

class spell_ragnaros_meteor_strike_marker : public SpellScriptLoader
{
public:
	spell_ragnaros_meteor_strike_marker() : SpellScriptLoader("spell_ragnaros_meteor_strike_marker") { }

	class spell_ragnaros_meteor_strike_marker_AuraScript : public AuraScript
	{
		PrepareAuraScript(spell_ragnaros_meteor_strike_marker_AuraScript);

		void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
		{
			if (!GetCaster())
				return;

			if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
				if (Creature* creCaster = GetCaster()->ToCreature())
					creCaster->AI()->DoAction(ACTION_METEOR_STRIKE_AOE);
		}

		void Register()
		{
			AfterEffectRemove += AuraEffectRemoveFn(spell_ragnaros_meteor_strike_marker_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
		}
	};

	AuraScript* GetAuraScript() const
	{
		return new spell_ragnaros_meteor_strike_marker_AuraScript();
	}
};

class npc_twilight_field : public CreatureScript
{
public:
	npc_twilight_field() : CreatureScript("npc_twilight_field") { }

	struct npc_twilight_fieldAI : public ScriptedAI
	{
		npc_twilight_fieldAI(Creature* creature) : ScriptedAI(creature), summons(me)
		{
			instance = me->GetInstanceScript();
		}

		void Reset()
		{
			summons.DespawnAll();
			events.Reset();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
		}

		void JustSummoned(Creature* who)
		{
			summons.Summon(who);
		}

		void EnterCombat(Unit* /*who*/)
		{
			events.ScheduleEvent(EVENT_TWILIGHT_PULSE, 0);
		}

		void UpdateAI(uint32 diff)
		{
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_TWILIGHT_PULSE:
					me->RemoveAura(SPELL_PORTAL_VISUAL);
					DoCast(me, SPELL_PORTAL_VISUAL);
					DoCast(me, SPELL_TWILIGHT_PULSE);
					events.ScheduleEvent(EVENT_TWILIGHT_PULSE, 2000);
					break;
				}
			}
		}

	private:
		EventMap events;
		InstanceScript* instance;
		SummonList summons;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return GetMoltenCoreAI<npc_twilight_fieldAI>(creature);
	}
};

class npc_living_meteor : public CreatureScript
{
public:
	npc_living_meteor() : CreatureScript("npc_living_meteor") { }

	struct npc_living_meteorAI : public ScriptedAI
	{
		npc_living_meteorAI(Creature* creature) : ScriptedAI(creature)
		{
			instance = me->GetInstanceScript();
		}

		void Reset()
		{
			events.Reset();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->AddAura(SPELL_LIVING_METEOR_VISUAL, me);
		}

		void EnterCombat(Unit* /*who*/)
		{
			events.ScheduleEvent(EVENT_TWILIGHT_PULSE, 0);
		}

		void UpdateAI(uint32 diff)
		{
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_TWILIGHT_PULSE:

					events.ScheduleEvent(EVENT_TWILIGHT_PULSE, 2000);
					break;
				}
			}
		}

	private:
		EventMap events;
		InstanceScript* instance;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return GetMoltenCoreAI<npc_living_meteorAI>(creature);
	}
};


void AddSC_boss_corrupted_ragnaros()
{
	new boss_corrupted_ragnaros();
	new npc_ragnaros_controller();
	new npc_ragnaros_meteor_strike_initial();
	new npc_ragnaros_meteor_strike();
	new spell_ragnaros_meteor_strike_marker();
	new npc_twilight_field();
	new npc_living_meteor();
}