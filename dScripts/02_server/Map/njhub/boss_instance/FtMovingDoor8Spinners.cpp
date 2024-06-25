#include "FtMovingDoor8Spinners.h"
#include "ProximityMonitorComponent.h"
#include "EntityManager.h"
#include "DestroyableComponent.h"
#include "MovingPlatformComponent.h"
#include "EntityInfo.h"
#include "GameMessages.h"
#include "MissionComponent.h"
#include "RenderComponent.h"
#include "eStateChangeType.h"

void FtMovingDoor8Spinners::OnStartup(Entity* self) {
	self->SetNetworkVar(u"bIsInUse", false);
	self->SetVar(u"bActive", true);



	SpawnLegs(self, "Spinner1");	
	SpawnLegs(self, "Spinner2");	
	SpawnLegs(self, "Spinner3");	
	SpawnLegs(self, "Spinner4");	
	SpawnLegs(self, "Spinner5");	
	SpawnLegs(self, "Spinner6");	
	SpawnLegs(self, "Spinner7");	
	SpawnLegs(self, "Spinner8");
}

void FtMovingDoor8Spinners::SpawnLegs(Entity* self, const std::string& loc) {
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
	


//if spinner down to up: pos.y = Spinner starting pos (aka Waypoint 1) - 0.1

	if (loc == "Spinner1") {
		const auto dir = rot.GetRightVector();		
		pos.x = -1620.75806;
		pos.y = 363.62388;
		pos.z = -58.51917;
		info.pos = pos;	
	} else if (loc == "Spinner2") {	
		const auto dir = rot.GetRightVector();		
		pos.x = -1627.15234;
		pos.y = 353.26551;
		pos.z = 10.39728;
		info.pos = pos;
	} else if (loc == "Spinner3") {
		const auto dir = rot.GetRightVector();		
		pos.x = -1648.29675;
		pos.y = 378.76255;
		pos.z = 3.63825;
		info.pos = pos;
	} else if (loc == "Spinner4") {
		const auto dir = rot.GetRightVector();		
		pos.x = -1681.39697;
		pos.y = 367.84492;
		pos.z = -230.77191;
		info.pos = pos;
	} else if (loc == "Spinner5") {
		const auto dir = rot.GetRightVector();		
		pos.x = -1782.59766;
		pos.y = 378.79169;
		pos.z = 4.36175;
		info.pos = pos;
	} else if (loc == "Spinner6") {
		const auto dir = rot.GetRightVector();		
		pos.x = -1846.72534;
		pos.y = 365.9621;
		pos.z = -64.57022;
		info.pos = pos;
	} else if (loc == "Spinner7") {
		const auto dir = rot.GetRightVector();		
		pos.x = -1847.27747;
		pos.y = 365.09278;
		pos.z = -191.28635;
		info.pos = pos;
	} else if (loc == "Spinner8") {
		const auto dir = rot.GetRightVector();		
		pos.x = -1849.40466;
		pos.y = 330.00284;
		pos.z = -230.1709;
		info.pos = pos;
	}

	info.rot = NiQuaternion::LookAt(info.pos, self->GetPosition());

	auto* entity = Game::entityManager->CreateEntity(info);

	Game::entityManager->ConstructEntity(entity);

	OnChildLoaded(self, entity);
}

void FtMovingDoor8Spinners::OnChildLoaded(Entity* self, Entity* child) {
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

void FtMovingDoor8Spinners::NotifyDie(Entity* self, Entity* other, Entity* killer) {
	auto players = self->GetVar<std::vector<LWOOBJID>>(u"Players");

	const auto& iter = std::find(players.begin(), players.end(), killer->GetObjectID());

	if (iter == players.end()) {
		players.push_back(killer->GetObjectID());
	}

	self->SetVar(u"Players", players);

	OnChildRemoved(self, other);
}

void FtMovingDoor8Spinners::OnChildRemoved(Entity* self, Entity* child) {
	auto legTable = self->GetVar<std::vector<LWOOBJID>>(u"legTable");

	const auto& iter = std::find(legTable.begin(), legTable.end(), child->GetObjectID());

	if (iter != legTable.end()) {
		legTable.erase(iter);
	}

	self->SetVar(u"legTable", legTable);

	if (legTable.empty()) {
		self->AddTimer("MoveUp", 5.4f);	
		
	}

	auto deadLegs = self->GetVar<std::vector<std::string>>(u"DeadLegs");

	const auto& leg = child->GetVar<std::string>(u"Leg");

	const auto& legIter = std::find(deadLegs.begin(), deadLegs.end(), leg);

	if (legIter == deadLegs.end()) {
		deadLegs.push_back(leg);
	}

	self->SetVar(u"DeadLegs", deadLegs);


}

void FtMovingDoor8Spinners::OnTimerDone(Entity* self, std::string timerName) {
	if (timerName == "MoveUp") {	
		GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 1, 0, 0, eMovementPlatformState::Moving);
	}
}
