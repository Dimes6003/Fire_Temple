#include "FtMovingDoorWaves.h"
#include "ProximityMonitorComponent.h"
#include "EntityManager.h"
#include "DestroyableComponent.h"
#include "MovingPlatformComponent.h"
#include "EntityInfo.h"
#include "GameMessages.h"
#include "RenderComponent.h"
#include "eStateChangeType.h"

void FtMovingDoorWaves::OnStartup(Entity* self) {
	self->SetNetworkVar(u"bIsInUse", true);
	self->SetVar(u"bActive", false);


	self->AddTimer("CheckActive", 13);
}

void FtMovingDoorWaves::SpawnLegs(Entity* self, const std::string& loc) {
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


	if (loc == "Rear") {
		const auto dir = rot.GetRightVector();
		pos.x = -1228.942505;
		pos.y = 358.59791;
		pos.z = -12.508;
		info.pos = pos;
	}

	info.rot = NiQuaternion::LookAt(info.pos, self->GetPosition());

	auto* entity = Game::entityManager->CreateEntity(info);

	Game::entityManager->ConstructEntity(entity);

	OnChildLoaded(self, entity);
}

void FtMovingDoorWaves::OnChildLoaded(Entity* self, Entity* child) {
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

void FtMovingDoorWaves::NotifyDie(Entity* self, Entity* other, Entity* killer) {
	auto players = self->GetVar<std::vector<LWOOBJID>>(u"Players");

	const auto& iter = std::find(players.begin(), players.end(), killer->GetObjectID());

	if (iter == players.end()) {
		players.push_back(killer->GetObjectID());
	}

	self->SetVar(u"Players", players);

	OnChildRemoved(self, other);
}

void FtMovingDoorWaves::OnChildRemoved(Entity* self, Entity* child) {
	auto legTable = self->GetVar<std::vector<LWOOBJID>>(u"legTable");

	const auto& iter = std::find(legTable.begin(), legTable.end(), child->GetObjectID());

	if (iter != legTable.end()) {
		legTable.erase(iter);
	}

	self->SetVar(u"legTable", legTable);

	if (legTable.empty()) {
		self->AddTimer("MoveDoor", 1);			
		
	}

	auto deadLegs = self->GetVar<std::vector<std::string>>(u"DeadLegs");

	const auto& leg = child->GetVar<std::string>(u"Leg");

	const auto& legIter = std::find(deadLegs.begin(), deadLegs.end(), leg);

	if (legIter == deadLegs.end()) {
		deadLegs.push_back(leg);
	}

	self->SetVar(u"DeadLegs", deadLegs);


}

void FtMovingDoorWaves::OnTimerDone(Entity* self, std::string timerName) {
	
	if (timerName == "CheckActive") {
		if (self->GetVar<bool>(u"bActive")) {
			
//			Spawn leg & communicate ready to spinner			
			SpawnLegs(self, "Rear");
			
			auto GoldSpinner = Game::entityManager->GetEntitiesInGroup("WavesElevatorSpinner");	
			for (auto* spinner : GoldSpinner) {		
				spinner->SetNetworkVar(u"bIsInUse", false);
				spinner->SetVar(u"bActive", true);
			}		
		} else {
//			 interval < 5 unecessary			
			self->AddTimer("CheckActive", 4);			
		}	
	} else if (timerName == "MoveDoor") {
		GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 1, 0, 0, eMovementPlatformState::Moving);		
	}	
	
}
