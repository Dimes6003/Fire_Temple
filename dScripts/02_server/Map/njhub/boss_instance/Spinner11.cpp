#include "Spinner11.h"
#include "ProximityMonitorComponent.h"
#include "EntityManager.h"
#include "DestroyableComponent.h"
#include "MovingPlatformComponent.h"
#include "EntityInfo.h"
#include "GameMessages.h"
#include "MissionComponent.h"
#include "RenderComponent.h"
#include "eStateChangeType.h"

void Spinner11::OnStartup(Entity* self) {
	self->SetNetworkVar(u"bIsInUse", false);
	self->SetVar(u"bActive", true);

	
	SpawnLegs(self, "Rear");
}

void Spinner11::SpawnLegs(Entity* self, const std::string& loc) {
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
	
// read before edit
//if spinner down to up: pos.y = Spinner starting pos (aka Waypoint 1) - 0.1

	if (loc == "Right") {
		const auto dir = rot.GetForwardVector();
		pos.x += dir.x * offset;
		pos.z += dir.z * offset;
		info.pos = pos;
	} else if (loc == "Rear") {
		const auto dir = rot.GetRightVector();
		pos.x = -286.50912;
		pos.y = 264.99845;
		pos.z = 288.91077;
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

void Spinner11::OnChildLoaded(Entity* self, Entity* child) {
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

void Spinner11::NotifyDie(Entity* self, Entity* other, Entity* killer) {
	auto players = self->GetVar<std::vector<LWOOBJID>>(u"Players");

	const auto& iter = std::find(players.begin(), players.end(), killer->GetObjectID());

	if (iter == players.end()) {
		players.push_back(killer->GetObjectID());
	}

	self->SetVar(u"Players", players);

	OnChildRemoved(self, other);
}

void Spinner11::OnChildRemoved(Entity* self, Entity* child) {
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

//		Delay anim until spinner visible
		self->AddTimer("PlayAnim", 0.2f);
		
	}

	auto deadLegs = self->GetVar<std::vector<std::string>>(u"DeadLegs");

	const auto& leg = child->GetVar<std::string>(u"Leg");

	const auto& legIter = std::find(deadLegs.begin(), deadLegs.end(), leg);

	if (legIter == deadLegs.end()) {
		deadLegs.push_back(leg);
	}

	self->SetVar(u"DeadLegs", deadLegs);
}

void Spinner11::OnTimerDone(Entity* self, std::string timerName) {
	if (timerName == "PlayAnim") {	
		RenderComponent::PlayAnimation(self, u"up");
		
		//	Ascend sfx
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{5c30c263-00ae-42a2-80a3-2ae33c8f13fe}");	
		self->AddTimer("AscentGUID", 0.1f);
	}
	
	
//Handle spinner sound orders
	if (timerName == "AscentGUID") {
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{7f770ade-b84c-46ad-b3ae-bdbace5985d4}");	
	}
}


