#include "Spinner55.h"
#include "ProximityMonitorComponent.h"
#include "EntityManager.h"
#include "DestroyableComponent.h"
#include "MovingPlatformComponent.h"
#include "EntityInfo.h"
#include "GameMessages.h"
#include "MissionComponent.h"
#include "RenderComponent.h"
#include "eStateChangeType.h"

void Spinner55::OnStartup(Entity* self) {
	self->SetNetworkVar(u"bIsInUse", false);
	self->SetVar(u"bActive", true);

	self->AddTimer("MoveDown", 11.9f);	

	SpawnLegs(self, "Rear");
}

void Spinner55::SpawnLegs(Entity* self, const std::string& loc) {
	auto pos = self->GetPosition();
	auto rot = self->GetRotation();
	pos.y += self->GetVarAs<float>(u"vert_offset");

	auto newRot = rot;
	auto offset = self->GetVarAs<float>(u"hort_offset");




	std::vector<LDFBaseData*> config = { new LDFData<std::string>(u"Leg", loc) };

	EntityInfo info{};
	info.lot = 32000;
	info.spawnerID = self->GetObjectID();
	info.settings = config;
	info.rot = newRot;
	


//if spinner down to up: LegLocY in hf = Spinner starting pos (aka Waypoint 1) - 0.1


	if (loc == "Rear") {
		auto CheckGroupID = GeneralUtils::UTF16ToWTF8(self->GetVar<std::u16string>(u"groupID"));	
		auto isDecoy = self->GetVar<int32_t>(u"isDecoy");		
		if (CheckGroupID != "MillstoneMPSpinner;") {	
			if (isDecoy >= 1) {
				info.lot = 32000;	
			} else {
				info.lot = 32100;	
			}
			const auto dir = rot.GetRightVector();
			pos.x = -1749.1518554;
			pos.y = 347.6085;
			pos.z = -223.03902;
			info.pos = pos;
		} else {
			const auto dir = rot.GetRightVector();
			pos.x = -1155.154175;
			pos.y = 355.038;
			pos.z = -737.182434;
			info.pos = pos;	
		}	
	} 

	info.rot = NiQuaternion::LookAt(info.pos, self->GetPosition());

	auto* entity = Game::entityManager->CreateEntity(info);

	Game::entityManager->ConstructEntity(entity);

	OnChildLoaded(self, entity);
}

void Spinner55::OnChildLoaded(Entity* self, Entity* child) {
	auto legTable = self->GetVar<std::vector<LWOOBJID>>(u"legTable");

	legTable.push_back(child->GetObjectID());

	self->SetVar(u"legTable", legTable);

	const auto selfID = self->GetObjectID();

	child->AddDieCallback([this, selfID, child]() {
		auto* self = Game::entityManager->GetEntity(selfID);
		auto* destroyableComponent = child->GetComponent<DestroyableComponent>();

		if (destroyableComponent == nullptr || self == nullptr) {
			return;
		}

		NotifyDie(self, child, destroyableComponent->GetKiller());
		});
}

void Spinner55::NotifyDie(Entity* self, Entity* other, Entity* killer) {
	auto players = self->GetVar<std::vector<LWOOBJID>>(u"Players");

	const auto& iter = std::find(players.begin(), players.end(), killer->GetObjectID());

	if (iter == players.end()) {
		players.push_back(killer->GetObjectID());
	}

	self->SetVar(u"Players", players);

	OnChildRemoved(self, other);
}

void Spinner55::OnChildRemoved(Entity* self, Entity* child) {
	auto legTable = self->GetVar<std::vector<LWOOBJID>>(u"legTable");

	const auto& iter = std::find(legTable.begin(), legTable.end(), child->GetObjectID());

	if (iter != legTable.end()) {
		legTable.erase(iter);
	}

	self->SetVar(u"legTable", legTable);
	auto CheckGroupID = GeneralUtils::UTF16ToWTF8(self->GetVar<std::u16string>(u"groupID"));
	if (legTable.empty()) {

		RenderComponent::PlayAnimation(self, u"up");
		GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 0, 1);	
		
//		Ascend sfx
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{5c30c263-00ae-42a2-80a3-2ae33c8f13fe}");	
		self->AddTimer("AscentGUID", 0.1f);
//		End
		
		if (CheckGroupID != "MillstoneMPSpinner;") {
			self->AddTimer("MoveDown", 11.5f);	
			self->AddTimer("SpawnLeg", 12.5f);						
		} else {
			self->AddTimer("MoveDown", 7.5f);	
			self->AddTimer("SpawnLeg", 8.5f);		
		}
	}

	auto deadLegs = self->GetVar<std::vector<std::string>>(u"DeadLegs");

	const auto& leg = child->GetVar<std::string>(u"Leg");

	const auto& legIter = std::find(deadLegs.begin(), deadLegs.end(), leg);

	if (legIter == deadLegs.end()) {
		deadLegs.push_back(leg);
	}

	self->SetVar(u"DeadLegs", deadLegs);
}

void Spinner55::OnTimerDone(Entity* self, std::string timerName) {
	

	if (timerName == "MoveDown") {	
		RenderComponent::PlayAnimation(self, u"down");
		GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 1, 0, 0, eMovementPlatformState::Moving);

//		Descend sfx
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{40e86d71-084c-4149-884e-ab9b45b694dc}");	
		self->AddTimer("DescentGUID", 0.1f);		
	}	
	if (timerName == "SpawnLeg") {	
		SpawnLegs(self, "Rear");
	}	

//Handle spinner sound orders
	if (timerName == "AscentGUID") {
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{7f770ade-b84c-46ad-b3ae-bdbace5985d4}");		
	}
	if (timerName == "DescentGUID") {
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{97b60c03-51f2-45b6-80cc-ccbbef0d94cf}");	
	}	
	
}
