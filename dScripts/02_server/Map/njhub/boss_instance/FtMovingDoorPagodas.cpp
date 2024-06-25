#include "FtMovingDoorPagodas.h"
#include "ProximityMonitorComponent.h"
#include "EntityManager.h"
#include "DestroyableComponent.h"
#include "MovingPlatformComponent.h"
#include "EntityInfo.h"
#include "GameMessages.h"
#include "MissionComponent.h"
#include "RenderComponent.h"
#include "eStateChangeType.h"

void FtMovingDoorPagodas::OnStartup(Entity* self) {
	self->SetNetworkVar(u"bIsInUse", false);
	self->SetVar(u"bActive", true);



	SpawnLegs(self, "Rear");
	SpawnLegs(self, "Left");
	SpawnLegs(self, "Right");
}

void FtMovingDoorPagodas::SpawnLegs(Entity* self, const std::string& loc) {
	auto pos = self->GetPosition();
	auto rot = self->GetRotation();
	pos.y += self->GetVarAs<float>(u"vert_offset");

	auto newRot = rot;
	auto offset = self->GetVarAs<float>(u"hort_offset");

	auto legLOT = self->GetVar<LOT>(u"legLOT");

	if (legLOT == 0) {
		return;
	}

	std::vector<LDFBaseData*> config = { new LDFData<std::string>(u"Leg", loc) };

	EntityInfo info{};
	info.lot = legLOT;
	info.spawnerID = self->GetObjectID();
	info.settings = config;
	info.rot = newRot;
	


//if spinner down to up: pos.y = Spinner starting pos (aka Waypoint 1) - 0.1

	if (loc == "Right") {
		const auto dir = rot.GetForwardVector();
		pos.x = -521.04999;
		pos.y = 263.55646;
		pos.z = 189.92;
		info.pos = pos;
	} else if (loc == "Rear") {
		const auto dir = rot.GetRightVector();
		pos.x = -409.74203;
		pos.y = 264.96693;
		pos.z = 288.67252;
		info.pos = pos;
	} else if (loc == "Left") {
		const auto dir = rot.GetForwardVector();
		pos.x = -412.05371;
		pos.y = 265.11509;
		pos.z = 91.46653;
		info.pos = pos;
	}

	info.rot = NiQuaternion::LookAt(info.pos, self->GetPosition());

	auto* entity = Game::entityManager->CreateEntity(info);

	Game::entityManager->ConstructEntity(entity);

	OnChildLoaded(self, entity);
}

void FtMovingDoorPagodas::OnChildLoaded(Entity* self, Entity* child) {
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

void FtMovingDoorPagodas::NotifyDie(Entity* self, Entity* other, Entity* killer) {
	auto players = self->GetVar<std::vector<LWOOBJID>>(u"Players");

	const auto& iter = std::find(players.begin(), players.end(), killer->GetObjectID());

	if (iter == players.end()) {
		players.push_back(killer->GetObjectID());
	}

	self->SetVar(u"Players", players);

	OnChildRemoved(self, other);
}

void FtMovingDoorPagodas::OnChildRemoved(Entity* self, Entity* child) {
	auto legTable = self->GetVar<std::vector<LWOOBJID>>(u"legTable");

	const auto& iter = std::find(legTable.begin(), legTable.end(), child->GetObjectID());

	if (iter != legTable.end()) {
		legTable.erase(iter);
	}

	self->SetVar(u"legTable", legTable);


	if (legTable.empty()) {
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
