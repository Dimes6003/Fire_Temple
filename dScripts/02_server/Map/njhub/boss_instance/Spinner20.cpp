//////////
//////////////////////
// General script for moving pillars & related
// This is not a spinner


#include "Spinner20.h"
#include "ProximityMonitorComponent.h"
#include "EntityManager.h"
#include "DestroyableComponent.h"
#include "MovingPlatformComponent.h"
#include "EntityInfo.h"
#include "GameMessages.h"
#include "MissionComponent.h"
#include "RenderComponent.h"
#include "eStateChangeType.h"

void Spinner20::OnStartup(Entity* self) {
	self->SetNetworkVar(u"bIsInUse", false);
	self->SetVar(u"bActive", true);

	self->AddTimer("SpawnLeg", 14.1f);


}

void Spinner20::SpawnLegs(Entity* self, const std::string& loc) {
	auto pos = self->GetPosition();
	auto rot = self->GetRotation();
	pos.y += self->GetVarAs<float>(u"vert_offset");

	auto newRot = rot;
	auto offset = self->GetVarAs<float>(u"hort_offset");

	auto legLocX = self->GetVar<float>(u"legLocX");
	auto legLocY = self->GetVar<float>(u"legLocY");
	auto legLocZ = self->GetVar<float>(u"legLocZ");
	auto legLOT = self->GetVar<LOT>(u"legLOT");


	std::vector<LDFBaseData*> config = { new LDFData<std::string>(u"Leg", loc) };

	EntityInfo info{};
	if (legLOT == 32100) {
		info.lot = 32100;
	} else {
		info.lot = 32000;	
	}	
	info.spawnerID = self->GetObjectID();
	info.settings = config;
	info.rot = newRot;

	

// if spinner up to down: LegLocY in hf = Spinner starting pos (aka Waypoint 1) - 0.2
//if spinner down to up: LegLocY in hf = Spinner starting pos (aka Waypoint 1) - 0.1

	if (loc == "Right") {
		const auto dir = rot.GetForwardVector();
		pos.x += dir.x * offset;
		pos.z += dir.z * offset;
		info.pos = pos;
	} else if (loc == "Rear") {
		const auto dir = rot.GetRightVector();
		pos.x = legLocX;
		pos.y = legLocY;
		pos.z = legLocZ;
		info.pos = pos;
	} else if (loc == "Left") {
		const auto dir = rot.GetForwardVector() * -1;
		pos.x += dir.x * offset;
		pos.z += dir.z * offset;
		info.pos = pos;
	}

	info.rot = NiQuaternion::LookAt(info.pos, self->GetPosition());

	auto* entity = Game::entityManager->CreateEntity(info);

	Game::entityManager->ConstructEntity(entity);

	OnChildLoaded(self, entity);
}

void Spinner20::OnChildLoaded(Entity* self, Entity* child) {
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

void Spinner20::NotifyDie(Entity* self, Entity* other, Entity* killer) {
	auto players = self->GetVar<std::vector<LWOOBJID>>(u"Players");

	const auto& iter = std::find(players.begin(), players.end(), killer->GetObjectID());

	if (iter == players.end()) {
		players.push_back(killer->GetObjectID());
	}

	self->SetVar(u"Players", players);

	OnChildRemoved(self, other);
}

void Spinner20::OnChildRemoved(Entity* self, Entity* child) {
	auto legTable = self->GetVar<std::vector<LWOOBJID>>(u"legTable");

	const auto& iter = std::find(legTable.begin(), legTable.end(), child->GetObjectID());

	if (iter != legTable.end()) {
		legTable.erase(iter);
	}

	self->SetVar(u"legTable", legTable);

	if (legTable.size() == 2) {
	} else if (legTable.size() == 1) {
	} else if (legTable.empty()) {
		GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 1, 0, 0, eMovementPlatformState::Moving);
		
	}

	auto deadLegs = self->GetVar<std::vector<std::string>>(u"DeadLegs");

	const auto& leg = child->GetVar<std::string>(u"Leg");

	const auto& legIter = std::find(deadLegs.begin(), deadLegs.end(), leg);

	if (legIter == deadLegs.end()) {
		deadLegs.push_back(leg);
	}

	self->SetVar(u"DeadLegs", deadLegs);
}

void Spinner20::OnTimerDone(Entity* self, std::string timerName) {
	if (timerName == "SpawnLeg") {
		SpawnLegs(self, "Rear");
	}	
	
}
