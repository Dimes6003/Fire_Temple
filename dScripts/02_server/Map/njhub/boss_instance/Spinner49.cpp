#include "Spinner49.h"
#include "ProximityMonitorComponent.h"
#include "EntityManager.h"
#include "DestroyableComponent.h"
#include "MovingPlatformComponent.h"
#include "EntityInfo.h"
#include "GameMessages.h"
#include "MissionComponent.h"
#include "RenderComponent.h"
#include "eStateChangeType.h"
#include "SkillComponent.h"

void Spinner49::OnStartup(Entity* self) {
	self->SetNetworkVar(u"bIsInUse", false);
	self->SetVar(u"bActive", true);
	self->AddTimer("MoveUp", 22.9f);	
	SpawnLegs(self, "Inverse");	
}

void Spinner49::SpawnLegs(Entity* self, const std::string& loc) {
	auto pos = self->GetPosition();
	auto rot = self->GetRotation();
	pos.y += self->GetVarAs<float>(u"vert_offset");

	auto newRot = rot;
	auto offset = self->GetVarAs<float>(u"hort_offset");


	std::vector<LDFBaseData*> config = { new LDFData<std::string>(u"Leg", loc) };

	EntityInfo info{};
	info.lot = 32100;
	info.spawnerID = self->GetObjectID();
	info.settings = config;
	info.rot = newRot;
	
	if (loc == "Inverse") {
		info.lot = 32100;
	}
//if spinner down to up: pos.y = Spinner starting pos (aka Waypoint 1) - 0.1
	const auto dir = rot.GetForwardVector();
	pos.x = -684.112915;
	pos.y = 308.70615;
	pos.z = -1037.077515;
	info.pos = pos;


	info.rot = NiQuaternion::LookAt(info.pos, self->GetPosition());

	auto* entity = Game::entityManager->CreateEntity(info);

	Game::entityManager->ConstructEntity(entity);

	OnChildLoaded(self, entity);
}

void Spinner49::OnChildLoaded(Entity* self, Entity* child) {
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

void Spinner49::NotifyDie(Entity* self, Entity* other, Entity* killer) {
	auto players = self->GetVar<std::vector<LWOOBJID>>(u"Players");

	const auto& iter = std::find(players.begin(), players.end(), killer->GetObjectID());

	if (iter == players.end()) {
		players.push_back(killer->GetObjectID());
	}

	self->SetVar(u"Players", players);

	OnChildRemoved(self, other);
}

void Spinner49::OnChildRemoved(Entity* self, Entity* child) {
	auto legTable = self->GetVar<std::vector<LWOOBJID>>(u"legTable");

	const auto& iter = std::find(legTable.begin(), legTable.end(), child->GetObjectID());

	if (iter != legTable.end()) {
		legTable.erase(iter);
	}

	self->SetVar(u"legTable", legTable);

	if (legTable.size() == 2) {
	} else if (legTable.size() == 1) {
	} else if (legTable.empty()) {
		self->AddTimer("RespawnLeg", 8.0f);		
	
		if (IsUp == 1) {
			self->AddTimer("MoveDown", 0.1f);			
		}
		if (IsUp == 0) {
			self->AddTimer("MoveUp", 0.1f);			
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

void Spinner49::OnTimerDone(Entity* self, std::string timerName) {
	if (timerName == "RespawnLeg") {	
		auto deadLegs = self->GetVar<std::vector<std::string>>(u"DeadLegs");

		if (deadLegs.empty()) {
			return;
		}

		SpawnLegs(self, deadLegs[0]);

		deadLegs.erase(deadLegs.begin());

		self->SetVar<std::vector<std::string>>(u"DeadLegs", deadLegs);	
	}
	
	if (timerName == "MoveUp") {
		IsUp = 1;
		RenderComponent::PlayAnimation(self, u"up");
		GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 0, 1);
		self->AddTimer("IdleUp", 1.0f);	
		
//		Ascend sfx
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{5c30c263-00ae-42a2-80a3-2ae33c8f13fe}");	
		self->AddTimer("AscentGUID", 0.1f);
	}
	
	if (timerName == "MoveDown") {	
		IsUp = 0;
		RenderComponent::PlayAnimation(self, u"down");
		GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 1, 0, 0, eMovementPlatformState::Moving);
		self->CancelTimer("IdleUp");		
		self->CancelTimer("Shoot");	
		
//		Descend sfx
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{40e86d71-084c-4149-884e-ab9b45b694dc}");	
		self->AddTimer("DescentGUID", 0.1f);
	}
	
	if (timerName == "IdleUp") {	
		RenderComponent::PlayAnimation(self, u"idle-up");	
		if (IsUp = 1) {
			self->AddTimer("Shoot", 1.0f);		
		}	
	}
	if (timerName == "Shoot") {	
		RenderComponent::PlayAnimation(self, u"attack");	
		
		auto* skillComponent = self->GetComponent<SkillComponent>();
		if (skillComponent == nullptr) {
			return;
		}
		skillComponent->CalculateBehavior(972, 20376, self->GetObjectID(), true);			

//		BehaviorTemplate: 20375 | 8 | 4648
//		To add annoying cast effect back if you wish 	
		
		if (IsUp = 1) {
			self->AddTimer("Shoot", 1.0f);		
		}			
		
	}
	
//Handle spinner sound orders
	if (timerName == "AscentGUID") {
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{7f770ade-b84c-46ad-b3ae-bdbace5985d4}");		
	}
	if (timerName == "DescentGUID") {
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{97b60c03-51f2-45b6-80cc-ccbbef0d94cf}");	
	}
}

