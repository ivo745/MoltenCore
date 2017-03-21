/* ScriptData
SDAuthor: frostI
SDName: world_event
SD%Complete: 100
SDComment:
SDCategory: Custom
EndScriptData */

#include "ScriptPCH.h"

#define MIN_INTERVAL 1800000 // 30 minutes
#define MAX_INTERVAL 7200000 // 2 hours
#define DESPAWN_TIMER 60000 // 1 minutes
#define NPC_WRATHION 190050 // Task Master
#define SPELL_TELEPORT 33056 // Visual

class event_world : public WorldScript
{
public:
	uint32 Event_Timer;
	event_world() : WorldScript("event_world") { }

	void OnStartup()
	{
		Event_Timer = urand(MIN_INTERVAL, MAX_INTERVAL);
	}

	void OnUpdate(uint32 diff)
	{
		if (Event_Timer <= diff)
		{
			SessionMap const& sessions = sWorld->GetAllSessions();
			if (sessions.empty())
				return;

			for (SessionMap::const_iterator itr = sessions.find(urand(0, sessions.size() - 1)); itr != sessions.end(); ++itr)
			{
				if (!itr->second || !itr->second->GetPlayer() || !itr->second->GetPlayer()->IsInWorld())
				{
					Event_Timer = 60000;
					return;
				}

				Player* player = itr->second->GetPlayer();
				if (player->isDead() || player->isAFK() || player->GetMap()->IsBattlegroundOrArena() || player->GetMap()->IsDungeon() || player->GetMap()->IsRaidOrHeroicDungeon()
					|| player->inRandomLfgDungeon() || player->IsGameMaster())
				{
					Event_Timer = 60000;
					return;
				}

				if (Creature* wrathion = player->SummonCreature(NPC_WRATHION, player->GetPositionX(), player->GetPositionY() + 5.0f, player->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, DESPAWN_TIMER))
				{
					if (player->IsFlying() || player->IsInFlight() || player->IsInWater())
					{
						wrathion->SetCanFly(true);
						wrathion->SetSpeed(MOVE_FLIGHT, player->GetSpeed(MOVE_FLIGHT), true);
						wrathion->Mount(28060, 224, 33030);
					}
					else
						wrathion->SetCanFly(false);

					wrathion->CastSpell(wrathion, SPELL_TELEPORT, true);
					wrathion->GetMotionMaster()->MoveFollow(player, 0.0f, 0.0f);
					std::ostringstream ss;
					ss << "Greetings " << player->GetName() << ", could you assist me for a moment please?";
					wrathion->Say(ss.str().c_str(), LANG_UNIVERSAL);
					Event_Timer = urand(MIN_INTERVAL, MAX_INTERVAL);
				}
				break;
			}
		}
		else Event_Timer -= diff;
	}
};

#define GOSSIP_ITEM_1 "I would like to assist you."
#define GOSSIP_ITEM_2 "No, please leave me alone."

#define mapId 1
#define x -4325.570313f
#define y 3287.873779f
#define z 18.397894f
#define o 3.114720f

class npc_wrathion : public CreatureScript
{
public:
	npc_wrathion() : CreatureScript("npc_wrathion") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_TALK, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1, "Are you sure you want to assist me?", 0, 0);
		player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_TALK, GOSSIP_ITEM_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2, "Are you sure you want me to leave?", 0, 0);
		player->SEND_GOSSIP_MENU(60000, creature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();

		if (player->IsInCombat())
		{
			player->CLOSE_GOSSIP_MENU();
			creature->Whisper("You are required to leave combat!", LANG_UNIVERSAL, player);
			return false;
		}

		if (action == GOSSIP_ACTION_INFO_DEF + 1)
		{
			if (player->IsInFlight())
			{
				player->GetMotionMaster()->MovementExpired();
				player->CleanupAfterTaxiFlight();
			}
			creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
			creature->AI()->DoCast(player, SPELL_TELEPORT);
			creature->AI()->DoCast(creature, SPELL_TELEPORT);
			player->SaveRecallPosition();
			player->TeleportTo(mapId, x, y, z, o);
			creature->DespawnOrUnsummon(1);
		}

		if (action == GOSSIP_ACTION_INFO_DEF + 2)
		{
			creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
			creature->Say("I probably had too high expectations...", LANG_UNIVERSAL);
			creature->GetMotionMaster()->MoveIdle();
			creature->DespawnOrUnsummon(5000);
		}
		return true;
	}

	struct npc_wrathionAI : public ScriptedAI
	{
		npc_wrathionAI(Creature* creature) : ScriptedAI(creature) { }

		uint32 HurryUp_Timer;
		uint32 GoAway_Timer;

		void Reset()
		{
			HurryUp_Timer = 30000;
			GoAway_Timer = 55000;
		}

		void UpdateAI(uint32 diff)
		{
			if (HurryUp_Timer <= diff)
			{
				me->Say("I don't have much time, please choose swiftly.", LANG_UNIVERSAL);
				HurryUp_Timer = 1000000;
			}
			else HurryUp_Timer -= diff;
			if (GoAway_Timer <= diff)
			{
				me->Say("Excuse me, I have to leave now...", LANG_UNIVERSAL);
				GoAway_Timer = 1000000;
			}
			else GoAway_Timer -= diff;
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_wrathionAI(creature);
	}
};

void AddSC_event_world()
{
	new event_world;
	new npc_wrathion;
}