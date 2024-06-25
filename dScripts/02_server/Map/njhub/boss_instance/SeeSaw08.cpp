#include "SeeSaw08.h"
#include "EntityManager.h"
#include "GameMessages.h"
#include "MovingPlatformComponent.h"
#include "DestroyableComponent.h"
#include "ProximityMonitorComponent.h"
#include "MissionComponent.h"
#include "EntityInfo.h"
#include "RenderComponent.h"
#include "eStateChangeType.h"

void SeeSaw08::OnStartup(Entity* self) {
	self->AddTimer("MoveBack", 11.9f);
	self->SetProximityRadius(3.5, "spin_distance");
}

void SeeSaw08::OnSkillEventFired(Entity* self, Entity* caster, const std::string& message) {
	if (message != "NinjagoSpinEvent" || self->GetNetworkVar<bool>(u"bIsInUse")) {
		return;
	}

	auto* proximityMonitorComponent = self->GetComponent<ProximityMonitorComponent>();

	if (proximityMonitorComponent == nullptr || !proximityMonitorComponent->IsInProximity("spin_distance", caster->GetObjectID())) {
		return;
	}

	self->SetNetworkVar(u"bIsInUse", true);
	
//	Play cinematic
	const auto& cine = self->GetVar<std::u16string>(u"cinematic");
	if (cine.empty()) {
		return;
	}
	GameMessages::SendPlayCinematic(caster->GetObjectID(), cine, caster->GetSystemAddress());
//	End	
	

	TriggerDrill(self);
	

}

void SeeSaw08::TriggerDrill(Entity* self) {
//	Move spinner first
	GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 0, 1);	

	
//	Ascend sfx
	GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{5c30c263-00ae-42a2-80a3-2ae33c8f13fe}");	
	self->AddTimer("AscentGUID", 0.1f);
	
// 	End		
	
	RenderComponent::PlayAnimation(self, u"up");
	self->AddTimer("IdleAnim", 1.0f);
	self->AddTimer("RespawnLeg", 3);
}

void SeeSaw08::SpawnLegs(Entity* self, const std::string& loc) {
	auto pos = self->GetPosition();
	auto rot = self->GetRotation();
	pos.y += self->GetVarAs<float>(u"vert_offset");

	auto newRot = rot;
	auto offset = self->GetVarAs<float>(u"hort_offset");
	
	std::vector<LDFBaseData*> config = { new LDFData<std::string>(u"Leg", loc) };	

	EntityInfo info{};
	info.lot = 32101;
	info.spawnerID = self->GetObjectID();
	info.settings = config;
	info.rot = newRot;
	
// read before edit IMPORTANT!!	
// if spinner up to down: pos.y = Spinner starting pos (Waypoint 0) - 0.2
//if spinner down to up: pos.y = Spinner starting pos (Waypoint 0) - 0.1

	if (loc == "Right") {
		const auto dir = rot.GetForwardVector();
		pos.x += dir.x * offset;
		pos.z += dir.z * offset;
		info.pos = pos;
	} else if (loc == "Rear") {
		const auto dir = rot.GetRightVector();
		pos.x = -257.146;
		pos.y = 258.36;
		pos.z = 96.99;
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

void SeeSaw08::OnChildLoaded(Entity* self, Entity* child) {	
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


void SeeSaw08::NotifyDie(Entity* self, Entity* other, Entity* killer) {
	auto players = self->GetVar<std::vector<LWOOBJID>>(u"Players");

	const auto& iter = std::find(players.begin(), players.end(), killer->GetObjectID());

	if (iter == players.end()) {
		players.push_back(killer->GetObjectID());
	}

	self->SetVar(u"Players", players);

	OnChildRemoved(self, other);
	RenderComponent::PlayAnimation(self, u"down");	
	self->AddTimer("DownAnim", 0.1f);
}

void SeeSaw08::OnChildRemoved(Entity* self, Entity* child) {
	RenderComponent::PlayAnimation(self, u"down");	
	self->AddTimer("DownAnim", 0.1f);	
	auto legTable = self->GetVar<std::vector<LWOOBJID>>(u"legTable");

	const auto& iter = std::find(legTable.begin(), legTable.end(), child->GetObjectID());

	if (iter != legTable.end()) {
		legTable.erase(iter);
	}

	self->SetVar(u"legTable", legTable);

	if (legTable.empty()) {
		//Move to waypoint 
		GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 1, 0, 0, eMovementPlatformState::Moving);

//		Descend sfx
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{40e86d71-084c-4149-884e-ab9b45b694dc}");	
		self->AddTimer("DescentGUID", 0.1f);
		
		//End
		
		RenderComponent::PlayAnimation(self, u"down");	

		self->SetNetworkVar(u"bIsInUse", false);
		self->SetVar(u"bActive", true);
	}

	auto deadLegs = self->GetVar<std::vector<std::string>>(u"DeadLegs");

	const auto& leg = child->GetVar<std::string>(u"Leg");

	const auto& legIter = std::find(deadLegs.begin(), deadLegs.end(), leg);

	if (legIter == deadLegs.end()) {
		deadLegs.push_back(leg);
	}

	self->SetVar(u"DeadLegs", deadLegs);
}

void SeeSaw08::OnWaypointReached(Entity* self, uint32_t waypointIndex) {
	if (waypointIndex == 0) {
		self->SetNetworkVar(u"bIsInUse", false);
		self->SetVar(u"bActive", true);
	}
	
	if (waypointIndex == 1) {		
		self->SetNetworkVar(u"bIsInUse", true);
		self->SetVar(u"bActive", false);	
		RenderComponent::PlayAnimation(self, u"idle-up");
	}	
}

void SeeSaw08::OnTimerDone(Entity* self, std::string timerName) {

	const auto& data = GeneralUtils::SplitString(timerName, '_');

	if (data.empty()) {
		return;
	}
	
	if (timerName == "RespawnLeg") {
		SpawnLegs(self, "Rear");
	}
	
	if (timerName == "IdleAnim") {	
		RenderComponent::PlayAnimation(self, u"idle-up");
	}
	if (timerName == "DownAnim") {	
		RenderComponent::PlayAnimation(self, u"down");
	}	

	if (timerName == "MoveBack") {	
		GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 1, 0, 0, eMovementPlatformState::Moving);
		self->SetNetworkVar(u"bIsInUse", false);
		self->SetVar(u"bActive", true);	
		self->AddTimer("DownAnim", 0.1f);		

//		Descend sfx
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{40e86d71-084c-4149-884e-ab9b45b694dc}");	
		self->AddTimer("DescentGUID", 0.1f);		
	}		
	
	
//Handle spinner sound orders
	if (timerName == "AscentGUID") {
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{7f770ade-b84c-46ad-b3ae-bdbace5985d4}");	
	}
	if (timerName == "DescentGUID") {
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{97b60c03-51f2-45b6-80cc-ccbbef0d94cf}");	
	}
}
