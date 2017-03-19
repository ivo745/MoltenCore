/* ScriptData
SDName: instance_molten_core.cpp
SDAuthor: frostI
SD%Complete: 100
SDComment:
SDCategory: Molten Core
EndScriptData */

/* Molten Core encounters:
1 - Haunted Eye of Maximus
2 - Maximus
3 - Jandice Barov
4 - Alexstrasza Event
5 - Corrupted Ragnaros
*/

#include "ScriptPCH.h"
#include "molten_core.h"

DoorData const doorData[] =
{
	{ GO_DOOR_MAXIMUS,         DATA_MAXIMUS,           DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
	{ GO_DOOR_JANDICE_BAROV_1, DATA_JANDICE_BAROV,     DOOR_TYPE_ROOM,    BOUNDARY_NONE },
	{ GO_DOOR_JANDICE_BAROV_2, DATA_JANDICE_BAROV,     DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
	{ GO_DOOR_ALEXSTRASZA,     DATA_ALEXSTRASZA_EVENT, DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
};

Position const BrannSpawnPos1 = { 746.603577f, -502.244812f, -214.687668f };
Position const BrannSpawnPos2 = { 720.771912f, -942.298889f, -189.168793f };
Position const ThrallSpawnPos1 = { 653.632690f, -1095.244751f, -193.092087f };
Position const CorruptedRagnarosSpawnPos = { 838.047180f, -830.688232f, -230.207306f };
Position const AlexstraszaSpawnPos2 = { 851.196350f, -788.307495f, -227.007187f };

class instance_molten_core : public InstanceMapScript
{
public:
	instance_molten_core() : InstanceMapScript(MCScriptName, 409) { }

	struct instance_molten_core_InstanceMapScript : public InstanceScript
	{
		instance_molten_core_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
		{
			SetHeaders(DataHeader);
			SetBossNumber(EncounterCount);
			LoadDoorData(doorData);
		}

		void OnCreatureCreate(Creature* creature)
		{
			switch (creature->GetEntry())
			{
			case NPC_HAUNTED_EYE_OF_MAXIMUS:
				HauntedEyeOfMaximusGUID = creature->GetGUID();
				break;
			case NPC_MAXIMUS:
				MaximusGUID = creature->GetGUID();
				break;
			case NPC_CRIMSON_FLAME:
				CrimsonFlameGUID = creature->GetGUID();
				break;
			case NPC_JANDICE_BAROV:
				JandiceBarovGUID = creature->GetGUID();
				break;
			case NPC_ALEXSTRASZA:
				AlexstraszaGUID = creature->GetGUID();
				break;
			case NPC_RED_DRAGON_COMBAT_TRIGGER:
				AlexstraszaTriggerGUID = creature->GetGUID();
				break;
			case NPC_ALEXSTRASZA_CONTROLLER:
				AlexstraszaControllerGUID = creature->GetGUID();
				break;
			case NPC_THRALL_2:
				Thrall2GUID = creature->GetGUID();
				break;
			case NPC_CORRUPTED_RAGNAROS:
				CorruptedRagnarosGUID = creature->GetGUID();
				break;
			case NPC_RAGNAROS_CONTROLLER:
				RagnarosControllerGUID = creature->GetGUID();
				break;
			default:
				break;
			}
		}

		void OnGameObjectCreate(GameObject* go)
		{
			switch (go->GetEntry())
			{
			case GO_DOOR_JANDICE_BAROV_1:
				DoorJandiceGUID = go->GetGUID();
				AddDoor(go, true);
				break;
			case GO_DOOR_HAUNTED_EYE_OF_MAXIMUS:
				doorHauntedEyeGUID = go->GetGUID();
				AddDoor(go, true);
				break;
			case GO_DOOR_MAXIMUS:
				DoorMaximusGUID = go->GetGUID();
				AddDoor(go, true);
				break;
			case GO_DOOR_JANDICE_BAROV_2:
				DoorJandice2GUID = go->GetGUID();
				AddDoor(go, true);
				break;
			case GO_DOOR_ALEXSTRASZA:
				DoorAlexstraszaGUID = go->GetGUID();
				AddDoor(go, true);
				break;
			case GO_DOOR_INSTANCE:
				DoorInstanceGUID = go->GetGUID();
				AddDoor(go, true);
				break;
			default:
				break;
			}
		}

		void OnGameObjectRemove(GameObject* go) override
		{
			switch (go->GetEntry())
			{
			case GO_DOOR_JANDICE_BAROV_1:
			case GO_DOOR_HAUNTED_EYE_OF_MAXIMUS:
			case GO_DOOR_MAXIMUS:
			case GO_DOOR_JANDICE_BAROV_2:
			case GO_DOOR_ALEXSTRASZA:
			case GO_DOOR_INSTANCE:
				AddDoor(go, false);
				break;
			default:
				break;
			}
		}

		ObjectGuid GetGuidData(uint32 id) const override
		{
			switch (id)
			{
			case DATA_HAUNTED_EYE_OF_MAXIMUS:
				return HauntedEyeOfMaximusGUID;
			case DATA_MAXIMUS:
				return MaximusGUID;
			case DATA_CRIMSON_FLAME:
				return CrimsonFlameGUID;
			case DATA_JANDICE_BAROV:
				return JandiceBarovGUID;
			case DATA_ALEXSTRASZA_EVENT:
				return AlexstraszaGUID;
			case DATA_ALEXSTRASZA_TRIGGER:
				return AlexstraszaTriggerGUID;
			case DATA_ALEXSTRASZA_CONTROLLER:
				return AlexstraszaControllerGUID;
			case DATA_THRALL_2:
				return Thrall2GUID;
			case DATA_CORRUPTED_RAGNAROS:
				return CorruptedRagnarosGUID;
			case DATA_RAGNAROS_CONTROLLER:
				return RagnarosControllerGUID;
			default:
				break;
			}

			return ObjectGuid::Empty;
		}

		bool SetBossState(uint32 type, EncounterState state)
		{
			if (!InstanceScript::SetBossState(type, state))
				return false;

			switch (type)
			{
			case DATA_HAUNTED_EYE_OF_MAXIMUS:
				if (state == DONE)
					instance->SummonCreature(NPC_BRANN_3, BrannSpawnPos1);
				break;
			case DATA_MAXIMUS:
				if (state == DONE)
				{
					if (GameObject* go = instance->GetGameObject(DoorMaximusGUID))
						go->SetGoState(GO_STATE_READY);
					instance->SummonCreature(NPC_BRANN_4, BrannSpawnPos2);
				}
				break;
			case DATA_JANDICE_BAROV:
				if (state == DONE)
				{
					if (GameObject* go = instance->GetGameObject(DoorJandiceGUID))
						go->SetGoState(GO_STATE_READY);
					instance->SummonCreature(NPC_THRALL_1, ThrallSpawnPos1);
				}
				break;
			case DATA_ALEXSTRASZA_EVENT:
				if (state == NOT_STARTED)
				{
					instance->SummonCreature(NPC_RED_DRAGON_COMBAT_TRIGGER, AlexstraszaSpawnPos2);
					instance->SummonCreature(NPC_ALEXSTRASZA_CONTROLLER, AlexstraszaSpawnPos2);
				}
				if (state == DONE)
			    instance->SummonCreature(NPC_ALEXSTRASZA_2, AlexstraszaSpawnPos2);
				break;
			default:
				break;
			}

			return true;
		}

	protected:
		ObjectGuid HauntedEyeOfMaximusGUID;
		ObjectGuid MaximusGUID;
		ObjectGuid CrimsonFlameGUID;
		ObjectGuid JandiceBarovGUID;
		ObjectGuid AlexstraszaGUID;
		ObjectGuid AlexstraszaTriggerGUID;
		ObjectGuid AlexstraszaControllerGUID;
		ObjectGuid Thrall2GUID;
		ObjectGuid CorruptedRagnarosGUID;
		ObjectGuid RagnarosControllerGUID;

		ObjectGuid DoorJandiceGUID;
		ObjectGuid doorHauntedEyeGUID;
		ObjectGuid DoorMaximusGUID;
		ObjectGuid DoorJandice2GUID;
		ObjectGuid DoorAlexstraszaGUID;
		ObjectGuid DoorInstanceGUID;
	};

	InstanceScript* GetInstanceScript(InstanceMap* map) const
	{
		return new instance_molten_core_InstanceMapScript(map);
	}
};

void AddSC_instance_molten_core()
{
	new instance_molten_core();
}