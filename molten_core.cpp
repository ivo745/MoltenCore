/*
* Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ScriptData
SDName: molten_core.cpp
SD%Complete: 100
SDComment:
SDCategory: Molten Core
EndScriptData */

#include "ScriptPCH.h"
#include "molten_core.h"

enum Events
{
	//On enter instance
	EVENT_MOVE_1 = 1,
	EVENT_MOVE_2 = 2,
	EVENT_MOVE_3 = 3,
	EVENT_MOVE_4 = 4,
	EVENT_MOVE_5 = 5,
	EVENT_MOVE_6 = 6,
	EVENT_MOVE_7 = 7,
	EVENT_MOVE_8 = 8,
	EVENT_MOVE_9 = 9,
	EVENT_TALK_MOVE_9 = 10,
	EVENT_WP1_COMPLETE = 11,
	//After finished WP1
	EVENT_MOVE_10 = 12,
	EVENT_STEALTH = 13,
	//After finished Boss1
	EVENT_MOVE_12 = 14,
	EVENT_MOVE_13 = 15,
	EVENT_REACHED_MOVE_13 = 16,
	EVENT_DOOR_EXPLOSION_1 = 17,
	EVENT_MOVE_14 = 18,
	EVENT_MOVE_15 = 19,
	EVENT_MOVE_16 = 20,
	EVENT_WP3_COMPLETE = 21,
	//After finished Boss2
	EVENT_MOVE_17 = 22,
	EVENT_MOVE_18 = 23,
	EVENT_MOVE_19 = 24,
	EVENT_MOVE_20 = 25,
	EVENT_MOVE_21 = 26,
	EVENT_MOVE_22 = 27,
	EVENT_TALK_MOVE_21 = 28,
	EVENT_WP4_COMPLETE = 29,
	//After finished boss3
	EVENT_MOVE_23 = 30,
	EVENT_MOVE_24 = 31,
	EVENT_MOVE_25 = 32,
	EVENT_MOVE_26 = 33,
	EVENT_WP5_COMPLETE = 34,
	//After finished WP5
	EVENT_MOVE_27 = 35,
	EVENT_MOVE_28 = 36,
	EVENT_STORMHAMMER = 37,
	EVENT_EARTHS_WRATH = 38,
	//After finished boss4
};

enum Actions
{
	ACTION_START_RAGNAROS = 1,
	ACTION_START_RAGNAROS_ALT = 2,
};

//On enter instance
Position const MovePositionOne = { 1088.671753f, -509.222473f, -104.416206f };
Position const MovePositionTwo = { 1123.623413f, -535.504761f, -106.074875f };
Position const MovePositionThree = { 1149.386108f, -563.895325f, -112.450943f };
Position const MovePositionFour = { 1156.456543f, -597.897644f, -115.531792f };
Position const MovePositionFive = { 1194.266113f, -627.508423f, -124.660027f };
Position const MovePositionSix = { 1198.222168f, -667.723022f, -128.895782f };
Position const MovePositionSeven = { 1145.912842f, -715.578430f, -133.735992f };
Position const MovePositionEight = { 1143.672852f, -707.283020f, -133.881622f };
Position const MovePositionNine = { 1125.512695f, -714.131409f, -141.283015f };
//After finished WP1
Position const MovePositionTen = { 1070.013184f, -685.529785f, -161.657288f };
Position const MovePositionEleven = { 746.603577f, -502.244812f, -214.687668f };
//After finished boss1
Position const MovePositionTwelve = { 724.689575f, -562.533997f, -215.170349f };
Position const MovePositionThirteen = { 710.606750f, -598.061157f, -209.818436f };
Position const MovePositionFourteen = { 691.028259f, -655.008972f, -209.704636f };
Position const MovePositionFifteen = { 712.119263f, -704.30598f, -209.654999f };
Position const MovePositionSixteen = { 662.099670f, -768.615906f, -208.961502f };
//After finished boss2
Position const MovePositionSeventeen = { 744.343994f, -960.114990f, -183.261002f };
Position const MovePositionEighteen = { 747.827026f, -992.828979f, -177.899002f };
Position const MovePositionNineteen = { 738.315002f, -1021.530029f, -177.899994f };
Position const MovePositionTwenty = { 722.072632f, -1031.155396f, -177.710495f };
Position const MovePositionTwentyone = { 651.380981f, -1066.010010f, -194.227005f };
Position const MovePositionTwentytwo = { 625.955017f, -1127.599976f, -200.606995f };
//After finished boss3
Position const MovePositionTwentythree = { 693.305664f, -1079.497437f, -182.602295f };
Position const MovePositionTwentyfour = { 721.921387f, -1080.195679f, -176.678085f };
Position const MovePositionTwentyfive = { 758.179504f, -1086.479004f, -164.565872f };
Position const MovePositionTwentysix = { 796.677856f, -1108.830688f, -156.154938f };
//After finished WP5
Position const Thrall2SpawnPos = { 684.579224f, -1178.350708f, -124.118835f };
Position const MovePositionTwentyseven = { 686.290466f, -1192.759155f, -121.399796f };
Position const MovePositionTwentyeight = { 708.764648f, -1182.959229f, -119.480843f };
//After finished boss4
Position const PlayerToRagnarosPos = { 838.552917f, -759.159607f, -224.077759f };
Position const RagnarosSummonPos = { 838.510f, -829.840f, -232.000f, 2.00000f };

//On enter instance
#define GOSSIP_ITEM_1     "We need to compromise the threat in here, let's go!"
#define GOSSIP_ITEM_2     "Perhaps you have any useful buff?"
//After finished WP1
#define GOSSIP_ITEM_3     "We'll see you back there."
//After finished boss1
#define GOSSIP_ITEM_4     "We are ready to face the next challange."
//After finished boss2
#define GOSSIP_ITEM_5     "We should check that dark spot in the back of the cave."
//After finished boss3
#define GOSSIP_ITEM_6     "We are ready to assist you."
//After finished WP5
#define GOSSIP_ITEM_7     "We are with you."
//After finished event alexstrasza
#define GOSSIP_ITEM_8     "I am ready to teleport!"
//After teleported to ragnaros
#define GOSSIP_ITEM_9     "We are ready to face Ragnaros"

#define SOUND1_ON_WP1     14259
#define SOUND2_ON_WP1     14247
#define SOUND1_ON_WP2     14251
#define SOUND1_ON_WP3     14255
#define SOUND2_ON_WP3     14273

class npc_brann_1 : public CreatureScript
{
public:
	npc_brann_1() : CreatureScript("npc_brann_1"){ }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
		player->SEND_GOSSIP_MENU(60000, creature->GetGUID());

		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		if (action == GOSSIP_ACTION_INFO_DEF + 1)
		{
			player->CLOSE_GOSSIP_MENU();
			CAST_AI(npc_brann_1::npc_brann_1AI, creature->AI())->StartWP1();
		}
		if (action == GOSSIP_ACTION_INFO_DEF + 2)
		{
			player->CLOSE_GOSSIP_MENU();
			player->CastSpell(player, SPELL_SWIFT_WIND);
		}

		return true;
	}

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_brann_1AI(creature);
	}

	struct npc_brann_1AI : public ScriptedAI
	{
		npc_brann_1AI(Creature* creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		EventMap events;
		InstanceScript* instance;

		bool WP1Complete;

		void Reset()
		{
			events.Reset();
			WP1Complete = false;
		}

		void DespawnCreatures(uint32 entry, float distance, bool discs = false)
		{
			std::list<Creature*> m_pCreatures;
			GetCreatureListWithEntryInGrid(m_pCreatures, me, entry, distance);

			if (m_pCreatures.empty())
				return;

			for (std::list<Creature*>::iterator iter = m_pCreatures.begin(); iter != m_pCreatures.end(); ++iter)
				(*iter)->DespawnOrUnsummon();
		}

		void StartWP1()
		{
			me->RemoveAllAuras();
			me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

			events.ScheduleEvent(EVENT_MOVE_1, 2000);
			events.ScheduleEvent(EVENT_MOVE_2, 3000);
			events.ScheduleEvent(EVENT_MOVE_3, 7000);
			events.ScheduleEvent(EVENT_MOVE_4, 11000);
			events.ScheduleEvent(EVENT_MOVE_5, 16500);
			events.ScheduleEvent(EVENT_MOVE_6, 24000);
			events.ScheduleEvent(EVENT_MOVE_7, 27750);
			events.ScheduleEvent(EVENT_MOVE_8, 29250);
			events.ScheduleEvent(EVENT_MOVE_9, 34000);
			events.ScheduleEvent(EVENT_TALK_MOVE_9, 40000);
			events.ScheduleEvent(EVENT_WP1_COMPLETE, 50000);

			//me->MonsterYell("Time to get some answers. Let's get this shorter road. ", LANG_UNIVERSAL, me->GetGUID());
			DoPlaySoundToSet(me, SOUND1_ON_WP1);
		}

		void UpdateAI(uint32 diff)
		{
			if (!me->GetVictim())
			{
				events.Update(diff);

				if (WP1Complete)
				{
					DespawnCreatures(NPC_BRANN_1, 1.0f);
					me->SummonCreature(NPC_BRANN_2, MovePositionNine, TEMPSUMMON_TIMED_DESPAWN, 7 * DAY);
				}

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case EVENT_MOVE_1:
						me->GetMotionMaster()->MovePoint(0, MovePositionOne);
						break;
					case EVENT_MOVE_2:
						me->GetMotionMaster()->MovePoint(0, MovePositionTwo);
						break;
					case EVENT_MOVE_3:
						me->GetMotionMaster()->MovePoint(0, MovePositionThree);
						break;
					case EVENT_MOVE_4:
						me->GetMotionMaster()->MovePoint(0, MovePositionFour);
						break;
					case EVENT_MOVE_5:
						me->GetMotionMaster()->MovePoint(0, MovePositionFive);
						break;
					case EVENT_MOVE_6:
						me->GetMotionMaster()->MovePoint(0, MovePositionSix);
						break;
					case EVENT_MOVE_7:
						me->GetMotionMaster()->MovePoint(0, MovePositionSeven);
						break;
					case EVENT_MOVE_8:
						me->GetMotionMaster()->MovePoint(0, MovePositionEight);
						break;
					case EVENT_MOVE_9:
						me->GetMotionMaster()->MovePoint(0, MovePositionNine);
						break;
					case EVENT_TALK_MOVE_9:
						me->SetFacingTo(5);
						//me->MonsterSay("Take a moment to relish this with me. Soon all will be revealed... Okay then, let's do this!", LANG_UNIVERSAL, me->GetGUID());
						DoPlaySoundToSet(me, SOUND2_ON_WP1);
						break;
					case EVENT_WP1_COMPLETE:
						WP1Complete = true;
						break;
					}
				}
			}
		}
	};
};

class npc_brann_2 : public CreatureScript
{
public:
	npc_brann_2() : CreatureScript("npc_brann_2"){ }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
		player->SEND_GOSSIP_MENU(60000, creature->GetGUID());

		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		if (action == GOSSIP_ACTION_INFO_DEF + 1)
		{
			player->CLOSE_GOSSIP_MENU();
			CAST_AI(npc_brann_2::npc_brann_2AI, creature->AI())->StartWP2();
		}

		return true;
	}

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_brann_2AI(creature);
	}

	struct npc_brann_2AI : public ScriptedAI
	{
		npc_brann_2AI(Creature* creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		EventMap events;
		InstanceScript* instance;

		bool WP2Complete;

		void Reset()
		{
			events.Reset();
			me->SetFacingTo(5);
			WP2Complete = false;
		}

		void DespawnCreatures(uint32 entry, float distance, bool discs = false)
		{
			std::list<Creature*> m_pCreatures;
			GetCreatureListWithEntryInGrid(m_pCreatures, me, entry, distance);

			if (m_pCreatures.empty())
				return;

			for (std::list<Creature*>::iterator iter = m_pCreatures.begin(); iter != m_pCreatures.end(); ++iter)
				(*iter)->DespawnOrUnsummon();
		}

		void StartWP2()
		{
			me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
			me->HandleEmoteCommand(66);

			events.ScheduleEvent(EVENT_MOVE_9, 2000);
			events.ScheduleEvent(EVENT_STEALTH, 6000);
		}

		void UpdateAI(uint32 diff)
		{
			if (!me->GetVictim())
			{
				events.Update(diff);

				if (WP2Complete)
				{
					DespawnCreatures(NPC_BRANN_2, 1.0f);
				}

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case EVENT_MOVE_9:
						me->SetSpeed(MOVE_RUN, 2);
						me->GetMotionMaster()->MovePoint(0, MovePositionTen);
						break;
					case EVENT_STEALTH:
						DoCast(me, SPELL_VANISH);
						WP2Complete = true;
						break;
					}
				}
			}
		}
	};
};

class npc_brann_3 : public CreatureScript
{
public:
	npc_brann_3() : CreatureScript("npc_brann_3"){ }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
		player->SEND_GOSSIP_MENU(60000, creature->GetGUID());

		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		if (action == GOSSIP_ACTION_INFO_DEF + 1)
		{
			player->CLOSE_GOSSIP_MENU();
			CAST_AI(npc_brann_3::npc_brann_3AI, creature->AI())->StartWP3();
		}

		return true;
	}

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_brann_3AI(creature);
	}

	struct npc_brann_3AI : public ScriptedAI
	{
		npc_brann_3AI(Creature* creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		EventMap events;
		InstanceScript* instance;

		bool WP3Complete;

		void Reset()
		{
			events.Reset();
			me->SetFacingTo(5);
			//me->MonsterYell("That looked kinda creepy, glad you made it!", LANG_UNIVERSAL, me->GetGUID());
			WP3Complete = false;
		}

		void DespawnCreatures(uint32 entry, float distance, bool discs = false)
		{
			std::list<Creature*> m_pCreatures;
			GetCreatureListWithEntryInGrid(m_pCreatures, me, entry, distance);

			if (m_pCreatures.empty())
				return;

			for (std::list<Creature*>::iterator iter = m_pCreatures.begin(); iter != m_pCreatures.end(); ++iter)
				(*iter)->DespawnOrUnsummon();
		}

		void StartWP3()
		{
			me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
			events.ScheduleEvent(EVENT_MOVE_12, 2000);
			events.ScheduleEvent(EVENT_MOVE_13, 10000);
			events.ScheduleEvent(EVENT_REACHED_MOVE_13, 15000);
			events.ScheduleEvent(EVENT_DOOR_EXPLOSION_1, 16750);
			events.ScheduleEvent(EVENT_MOVE_14, 19000);
			events.ScheduleEvent(EVENT_MOVE_15, 26000);
			events.ScheduleEvent(EVENT_MOVE_16, 33000);
			events.ScheduleEvent(EVENT_WP3_COMPLETE, 42500);
		}

		void UpdateAI(uint32 diff)
		{
			if (!me->GetVictim())
			{
				events.Update(diff);

				if (WP3Complete)
				{
					DespawnCreatures(NPC_BRANN_3, 1.0f);
				}

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case EVENT_MOVE_12:
						me->GetMotionMaster()->MovePoint(0, MovePositionTwelve);
						break;
					case EVENT_MOVE_13:
						me->GetMotionMaster()->MovePoint(0, MovePositionThirteen);
						break;
					case EVENT_REACHED_MOVE_13:
						//me->MonsterSay("A couple more minutes and I'll... Ohh", LANG_UNIVERSAL, me->GetGUID());
						DoPlaySoundToSet(me, SOUND1_ON_WP2);
						break;
					case EVENT_DOOR_EXPLOSION_1:
						DoCast(me, SPELL_DOOR_EXPLOSION);
						DoCast(me, SPELL_DOOR_EXPLOSION_KNOCKBACK);
						if (GameObject * pDoor = me->FindNearestGameObject(GO_DOOR_HAUNTED_EYE_OF_MAXIMUS, 20.0f))
							pDoor->SetGoState(GO_STATE_ACTIVE);
						break;
					case EVENT_MOVE_14:
						//me->MonsterSay("Heh, No magic fingers failing one through. Now let's get down on.", LANG_UNIVERSAL, me->GetGUID());
						DoPlaySoundToSet(me, SOUND1_ON_WP3);
						me->GetMotionMaster()->MovePoint(0, MovePositionFourteen);
						break;
					case EVENT_MOVE_15:
						me->GetMotionMaster()->MovePoint(0, MovePositionFifteen);
						break;
					case EVENT_MOVE_16:
						//me->MonsterSay("Well now that's a lot to digest, I'm gonna need some time to take this in.", LANG_UNIVERSAL, me->GetGUID());
						DoPlaySoundToSet(me, SOUND2_ON_WP3);
						me->GetMotionMaster()->MovePoint(0, MovePositionSixteen);
						break;
					case EVENT_WP3_COMPLETE:
						DoCast(me, SPELL_VANISH);
						WP3Complete = true;
						break;
					}
				}
			}
		}
	};
};

class npc_brann_4 : public CreatureScript
{
public:
	npc_brann_4() : CreatureScript("npc_brann_4"){ }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
		player->SEND_GOSSIP_MENU(60000, creature->GetGUID());

		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		if (action == GOSSIP_ACTION_INFO_DEF + 1)
		{
			player->CLOSE_GOSSIP_MENU();
			CAST_AI(npc_brann_4::npc_brann_4AI, creature->AI())->StartWP4();
		}

		return true;
	}

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_brann_4AI(creature);
	}

	struct npc_brann_4AI : public ScriptedAI
	{
		npc_brann_4AI(Creature* creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		EventMap events;
		InstanceScript* instance;
		uint32 DelayEvents_Timer;

		bool WP4Complete;

		void Reset()
		{
			events.Reset();
			//me->MonsterYell("Over here travellers!", LANG_UNIVERSAL, me->GetGUID());
			DelayEvents_Timer = 1000;
			WP4Complete = false;
		}

		void DespawnCreatures(uint32 entry, float distance, bool discs = false)
		{
			std::list<Creature*> m_pCreatures;
			GetCreatureListWithEntryInGrid(m_pCreatures, me, entry, distance);

			if (m_pCreatures.empty())
				return;

			for (std::list<Creature*>::iterator iter = m_pCreatures.begin(); iter != m_pCreatures.end(); ++iter)
				(*iter)->DespawnOrUnsummon();
		}

		void StartWP4()
		{
			me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
			events.ScheduleEvent(EVENT_MOVE_17, 2000);
			events.ScheduleEvent(EVENT_MOVE_18, 5750);
			events.ScheduleEvent(EVENT_MOVE_19, 9500);
			events.ScheduleEvent(EVENT_MOVE_20, 13500);
			events.ScheduleEvent(EVENT_MOVE_21, 16000);
			events.ScheduleEvent(EVENT_MOVE_22, 26000);
			events.ScheduleEvent(EVENT_TALK_MOVE_21, 30000);
			events.ScheduleEvent(EVENT_WP4_COMPLETE, 40000);
		}

		void UpdateAI(uint32 diff)
		{
			if (!me->GetVictim())
			{
				events.Update(diff);

				if (WP4Complete)
				{
					DespawnCreatures(NPC_BRANN_4, 1.0f);
				}

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case EVENT_MOVE_17:
						//me->MonsterSay("That was unexpected...", LANG_UNIVERSAL, me->GetGUID());
						me->GetMotionMaster()->MovePoint(0, MovePositionSeventeen);
						break;
					case EVENT_MOVE_18:
						me->GetMotionMaster()->MovePoint(0, MovePositionEighteen);
						break;
					case EVENT_MOVE_19:
						me->GetMotionMaster()->MovePoint(0, MovePositionNineteen);
						break;
					case EVENT_MOVE_20:
						me->GetMotionMaster()->MovePoint(0, MovePositionTwenty);
						break;
					case EVENT_MOVE_21:
						me->GetMotionMaster()->MovePoint(0, MovePositionTwentyone);
						break;
					case EVENT_MOVE_22:
						me->GetMotionMaster()->MovePoint(0, MovePositionTwentytwo);
						break;
					case EVENT_TALK_MOVE_21:
						//me->MonsterSay("Oh, what's this. I didn't expect to find an illusionist down here. Things might get tricky.", LANG_UNIVERSAL, me->GetGUID());
						break;
					case EVENT_WP4_COMPLETE:
						//me->MonsterYell("We'll meet sometime soon.", LANG_UNIVERSAL, me->GetGUID());
						DoCast(me, SPELL_VANISH);
						WP4Complete = true;
						break;
					}
				}
			}

			if (me->GetVictim())
			{
				if (DelayEvents_Timer <= diff)
				{
					events.DelayEvents(1000);
					DelayEvents_Timer = 1000;
				}
				else DelayEvents_Timer -= diff;
			}

			DoMeleeAttackIfReady();
		}
	};
};

class npc_thrall_1 : public CreatureScript
{
public:
	npc_thrall_1() : CreatureScript("npc_thrall_1"){ }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
		player->SEND_GOSSIP_MENU(907, creature->GetGUID());

		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		if (action == GOSSIP_ACTION_INFO_DEF + 1)
		{
			player->CLOSE_GOSSIP_MENU();
			CAST_AI(npc_thrall_1::npc_thrall_1AI, creature->AI())->StartWP5();
		}

		return true;
	}

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_thrall_1AI(creature);
	}
	struct npc_thrall_1AI : public ScriptedAI
	{
		npc_thrall_1AI(Creature* creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		EventMap events;
		InstanceScript* instance;

		bool WP5Complete;

		void Reset()
		{
			events.Reset();
			//me->MonsterYell("This job requires true heroes, Come forth to me!", LANG_UNIVERSAL, me->GetGUID());
			WP5Complete = false;
		}

		void DespawnCreatures(uint32 entry, float distance, bool discs = false)
		{
			std::list<Creature*> m_pCreatures;
			GetCreatureListWithEntryInGrid(m_pCreatures, me, entry, distance);

			if (m_pCreatures.empty())
				return;

			for (std::list<Creature*>::iterator iter = m_pCreatures.begin(); iter != m_pCreatures.end(); ++iter)
				(*iter)->DespawnOrUnsummon();
		}

		void StartWP5()
		{
			me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
			events.ScheduleEvent(EVENT_MOVE_23, 2000);
			events.ScheduleEvent(EVENT_MOVE_24, 4000);
			events.ScheduleEvent(EVENT_MOVE_25, 6250);
			events.ScheduleEvent(EVENT_MOVE_26, 9000);
			events.ScheduleEvent(EVENT_WP5_COMPLETE, 12000);
		}

		void UpdateAI(uint32 diff)
		{
			if (!me->GetVictim())
			{
				events.Update(diff);

				if (WP5Complete)
				{
					DespawnCreatures(NPC_THRALL_1, 1.0f);
				}

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case EVENT_MOVE_23:
						me->SetSpeed(MOVE_RUN, 2.6f);
						//me->MonsterSay("Meet me up the hill.", LANG_UNIVERSAL, me->GetGUID());
						me->GetMotionMaster()->MovePoint(0, MovePositionTwentythree);
						break;
					case EVENT_MOVE_24:
						me->GetMotionMaster()->MovePoint(0, MovePositionTwentyfour);
						break;
					case EVENT_MOVE_25:
						me->GetMotionMaster()->MovePoint(0, MovePositionTwentyfive);
						break;
					case EVENT_MOVE_26:
						me->GetMotionMaster()->MovePoint(0, MovePositionTwentysix);
						break;
					case EVENT_WP5_COMPLETE:
						DoCast(me, SPELL_VANISH);
						me->SummonCreature(NPC_THRALL_2, Thrall2SpawnPos);
						WP5Complete = true;
						break;
					}
				}
			}
		}
	};
};

class npc_thrall_2 : public CreatureScript
{
public:
	npc_thrall_2() : CreatureScript("npc_thrall_2"){ }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
		player->SEND_GOSSIP_MENU(907, creature->GetGUID());

		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		if (action == GOSSIP_ACTION_INFO_DEF + 1)
		{
			player->CLOSE_GOSSIP_MENU();
			CAST_AI(npc_thrall_2::npc_thrall_2AI, creature->AI())->StartEventAlexstrasza();
		}

		return true;
	}

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_thrall_2AI(creature);
	}

	struct npc_thrall_2AI : public ScriptedAI
	{
		npc_thrall_2AI(Creature* creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		EventMap events;
		InstanceScript* instance;
		uint32 ChangeTargetTimer;
		uint32 SaySomethingTimer;
		uint32 SayHealMeTimer;
		bool EventStarted;

		void Reset()
		{
			events.Reset();
			me->ApplySpellImmune(0, IMMUNITY_ID, 36797, true);
			ChangeTargetTimer = 5000;
			SaySomethingTimer = 12500;
			SayHealMeTimer = 2500;
			EventStarted = false;
		}

		void DespawnCreatures(uint32 entry, float distance, bool discs = false)
		{
			std::list<Creature*> m_pCreatures;
			GetCreatureListWithEntryInGrid(m_pCreatures, me, entry, distance);

			if (m_pCreatures.empty())
				return;

			for (std::list<Creature*>::iterator iter = m_pCreatures.begin(); iter != m_pCreatures.end(); ++iter)
				(*iter)->DespawnOrUnsummon();
		}

		void StartEventAlexstrasza()
		{
			me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
			events.ScheduleEvent(EVENT_MOVE_27, 2000);
			events.ScheduleEvent(EVENT_MOVE_28, 3500);
			events.ScheduleEvent(EVENT_EARTHS_WRATH, 8500);
			events.ScheduleEvent(EVENT_STORMHAMMER, 20500);
		}

		void UpdateAI(uint32 diff)
		{
			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (EventStarted && !me->GetVictim())
			{
				me->GetMotionMaster()->MoveRandom(20.0f);
			}

			if (me->HealthBelowPct(20))
			{
				if (SayHealMeTimer <= diff)
				{
					switch (urand(0, 1))
					{
					    case 0:
					    {
						    //me->MonsterYell("Healers! Focus.", LANG_UNIVERSAL, me->GetGUID());
						    break;
					    }
					    case 1:
					    {
					    	//me->MonsterYell("Heal me, or die...", LANG_UNIVERSAL, me->GetGUID());
						    break;
					    }
					}
					SayHealMeTimer = 12500;
				}
				else SayHealMeTimer -= diff;
			}

			if (!me->GetVictim())
			{
				ChangeTargetTimer = 2000;

				if (ChangeTargetTimer <= diff)
				{
					if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
					{
						me->GetMotionMaster()->MoveChase(target);
						me->Attack(target, true);
					}
					ChangeTargetTimer = 2000;
				}
				else ChangeTargetTimer -= diff;

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case EVENT_MOVE_27:
						//me->MonsterYell("For the Horde!!!", LANG_UNIVERSAL, me->GetGUID());
						me->GetMotionMaster()->MovePoint(0, MovePositionTwentyseven);
						break;
					case EVENT_MOVE_28:
						me->GetMotionMaster()->MovePoint(0, MovePositionTwentyeight);
						EventStarted = true;
						break;
					}
				}
			}

			if (me->GetVictim())
			{
				if (me->isMoving())
				{
					events.ScheduleEvent(EVENT_EARTHS_WRATH, 5000);
					events.ScheduleEvent(EVENT_STORMHAMMER, 12500);
				}

				if (SaySomethingTimer <= diff)
				{
					switch (urand(0, 2))
					{
					    case 0:
						{
							//me->MonsterYell("Take this, you wothless druids!", LANG_UNIVERSAL, me->GetGUID());
							break;
						}
						case 1:
						{
							//me->MonsterYell("I was just getting started!", LANG_UNIVERSAL, me->GetGUID());
							break;
						}
						case 2:
						{
							//me->MonsterYell("haven't you got enough of this yet?!", LANG_UNIVERSAL, me->GetGUID());
							break;
						}
					}
					SaySomethingTimer = urand(10000, 30000);
				}
				else SaySomethingTimer -= diff;

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case EVENT_EARTHS_WRATH:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
						{
							DoCast(target, SPELL_EARTHS_WRATH);
						}
						events.ScheduleEvent(EVENT_EARTHS_WRATH, urand(5000, 7500));
						break;
					case EVENT_STORMHAMMER:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
						{
							DoCast(target, SPELL_STORMHAMMER);
						}
						events.ScheduleEvent(EVENT_STORMHAMMER, urand(17500, 20000));
						break;
					}
				}
			}

			DoMeleeAttackIfReady();
		}
	};
};

class npc_alexstrasza_1 : public CreatureScript
{
public:
	npc_alexstrasza_1() : CreatureScript("npc_alexstrasza_1"){ }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_8, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
		player->SEND_GOSSIP_MENU(907, creature->GetGUID());

		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		if (action == GOSSIP_ACTION_INFO_DEF + 1)
		{
			player->CLOSE_GOSSIP_MENU();
			player->TeleportTo(409, 838.552917f, -759.159607f, -224.077759f, player->GetOrientation());
		}

		return true;
	}

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_alexstrasza_1AI(creature);
	}

	struct npc_alexstrasza_1AI : public ScriptedAI
	{
		npc_alexstrasza_1AI(Creature* creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
	};
};

class npc_alexstrasza_2 : public CreatureScript
{
public:
	npc_alexstrasza_2() : CreatureScript("npc_alexstrasza_2"){ }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_9, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
		player->SEND_GOSSIP_MENU(907, creature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		if (action == GOSSIP_ACTION_INFO_DEF + 1)
		{
			creature->AI()->DoAction(ACTION_START_RAGNAROS);
		}
		return true;
	}

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_alexstrasza_2AI(creature);
	}

	struct npc_alexstrasza_2AI : public ScriptedAI
	{
		npc_alexstrasza_2AI(Creature* creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		EventMap events;
		InstanceScript* instance;
		uint32 ChangeTargetTimer;
		uint32 SaySomethingTimer;
		uint32 SayHealMeTimer;
		bool EventStarted;

		void Reset()
		{
			events.Reset();
			me->ApplySpellImmune(0, IMMUNITY_ID, 36797, true);
			ChangeTargetTimer = 5000;
			SaySomethingTimer = 12500;
			SayHealMeTimer = 2500;
			EventStarted = false;
		}

		void DoAction(int32 action)
		{
			if (action == ACTION_START_RAGNAROS)
			{
				me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
				instance->instance->SummonCreature(NPC_CORRUPTED_RAGNAROS, RagnarosSummonPos);
			}
			else if (action == ACTION_START_RAGNAROS_ALT)
			{
				me->setFaction(35);
				me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
			}
		}

		void UpdateAI(uint32 diff)
		{
			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (EventStarted && !me->GetVictim())
			{
				me->GetMotionMaster()->MoveRandom(20.0f);
			}

			if (me->HealthBelowPct(20))
			{
				if (SayHealMeTimer <= diff)
				{
					switch (urand(0, 1))
					{
					    case 0:
					    {
						    //me->MonsterYell("Healers! Focus.", LANG_UNIVERSAL, me->GetGUID());
						    break;
					    }
					    case 1:
					    {
						    //me->MonsterYell("Grant me some strength!", LANG_UNIVERSAL, me->GetGUID());
						    break;
					    }
					}
					SayHealMeTimer = 12500;
				}
				else SayHealMeTimer -= diff;
			}

			if (!me->GetVictim())
			{
				ChangeTargetTimer = 2000;

				if (ChangeTargetTimer <= diff)
				{
					if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
					{
						me->GetMotionMaster()->MoveChase(target);
						me->Attack(target, true);
					}
					ChangeTargetTimer = 2000;
				}
				else ChangeTargetTimer -= diff;

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case EVENT_MOVE_27:
						//me->MonsterYell("For the Horde!!!", LANG_UNIVERSAL, me->GetGUID());
						me->GetMotionMaster()->MovePoint(0, MovePositionTwentyseven);
						break;
					case EVENT_MOVE_28:
						//me->GetMotionMaster()->MovePoint(0, MovePositionTwentyeight);
						EventStarted = true;
						break;
					}
				}
			}

			if (me->GetVictim())
			{
				if (me->isMoving())
				{
					events.ScheduleEvent(EVENT_EARTHS_WRATH, 5000);
					events.ScheduleEvent(EVENT_STORMHAMMER, 12500);
				}

				if (SaySomethingTimer <= diff)
				{
					switch (urand(0, 2))
					{
					    case 0:
				    	{
						    //me->MonsterYell("Take this, you wothless druids!", LANG_UNIVERSAL, me->GetGUID());
					    	break;
				    	}
					    case 1:
				    	{
						    //me->MonsterYell("I was just getting started!", LANG_UNIVERSAL, me->GetGUID());
					    	break;
					    }
					    case 2:
					    {
						    //me->MonsterYell("haven't you got enough of this yet?!", LANG_UNIVERSAL, me->GetGUID());
						    break;
					    }
					}
					SaySomethingTimer = urand(10000, 30000);
				}
				else SaySomethingTimer -= diff;

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case EVENT_EARTHS_WRATH:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
						{
							DoCast(target, SPELL_EARTHS_WRATH);
						}
						events.ScheduleEvent(EVENT_EARTHS_WRATH, urand(5000, 7500));
						break;
					case EVENT_STORMHAMMER:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
						{
							DoCast(target, SPELL_STORMHAMMER);
						}
						events.ScheduleEvent(EVENT_STORMHAMMER, urand(17500, 20000));
						break;
					}
				}
			}

			DoMeleeAttackIfReady();
		}
	};
};

void AddSC_molten_core()
{
	new npc_brann_1();
	new npc_brann_2();
	new npc_brann_3();
	new npc_brann_4();
	new npc_thrall_1();
	new npc_thrall_2();
	new npc_alexstrasza_1();
	new npc_alexstrasza_2();
}