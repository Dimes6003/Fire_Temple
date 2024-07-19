//Used for LOT 32050 to not forget

#include "MillstoneC.h"
#include "ProximityMonitorComponent.h"
#include "EntityManager.h"
#include "DestroyableComponent.h"
#include "MovingPlatformComponent.h"
#include "EntityInfo.h"
#include "GameMessages.h"
#include "MissionComponent.h"
#include "RenderComponent.h"
#include "eStateChangeType.h"

void MillstoneC::OnStartup(Entity* self) {
	self->SetNetworkVar(u"bIsInUse", false);
	self->SetVar(u"bActive", true);

	z_CurrentWaypoint = 0;	
	z_NextWaypoint = 1;	
		
	self->AddTimer("InitialMove", 19.9f);	
	


	SpawnLegs(self, "C");
	
	
}

void MillstoneC::SpawnLegs(Entity* self, const std::string& loc) {
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


	if (loc == "B") {
		const auto dir = rot.GetRightVector();
		pos.x = -1135.88;
		pos.y = 382.14838;
		pos.z = -538.77002;
		info.pos = pos;
	} 
	if (loc == "C") {
		const auto dir = rot.GetRightVector();
		pos.x = -1048.38;
		pos.y = 381.99848;
		pos.z = -538.70001;
		info.pos = pos;
	} 

	info.rot = NiQuaternion::LookAt(info.pos, self->GetPosition());

	auto* entity = Game::entityManager->CreateEntity(info);

	Game::entityManager->ConstructEntity(entity);

	OnChildLoaded(self, entity);
}

void MillstoneC::OnChildLoaded(Entity* self, Entity* child) {
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

void MillstoneC::NotifyDie(Entity* self, Entity* other, Entity* killer) {
	auto players = self->GetVar<std::vector<LWOOBJID>>(u"Players");

	const auto& iter = std::find(players.begin(), players.end(), killer->GetObjectID());

	if (iter == players.end()) {
		players.push_back(killer->GetObjectID());
	}

	self->SetVar(u"Players", players);

	OnChildRemoved(self, other);
}

void MillstoneC::OnChildRemoved(Entity* self, Entity* child) {
	auto legTable = self->GetVar<std::vector<LWOOBJID>>(u"legTable");

	const auto& iter = std::find(legTable.begin(), legTable.end(), child->GetObjectID());

	if (iter != legTable.end()) {
		legTable.erase(iter);
	}

	self->SetVar(u"legTable", legTable);

	if (legTable.empty()) {
		self->AddTimer("SpawnLeg", 4.0f);	

		GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, z_CurrentWaypoint, z_NextWaypoint, 
		z_NextWaypoint);
		z_CurrentWaypoint = z_NextWaypoint;	
		if (z_NextWaypoint == 3) {	
			z_NextWaypoint = 0;		
		} else if (z_NextWaypoint != 3) {
			z_NextWaypoint++;	
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

void MillstoneC::OnTimerDone(Entity* self, std::string timerName) {
	if (timerName == "InitialMove") {	
		z_CurrentWaypoint = 0;	
		z_NextWaypoint = 1;	
		GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 1, 0, 0, eMovementPlatformState::Moving);
	}	
	if (timerName == "SpawnLeg") {
		SpawnLegs(self, "C");
	}	
}
