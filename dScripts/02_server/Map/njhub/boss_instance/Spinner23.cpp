#include "Spinner23.h"
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
#include "Entity.h"

void Spinner23::OnStartup(Entity* self) {
	m_Counter = 1;		
	GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 0, 1, 1, eMovementPlatformState::Moving);	
	
	self->SetNetworkVar(u"bIsInUse", false);
	self->SetVar(u"bActive", true);

	SpawnLegs(self, "Rear");
	
	self->AddTimer("ReturnIdle", 9.5f);	

	auto* proximityMonitorComponent = self->GetComponent<ProximityMonitorComponent>();
	self->SetProximityRadius(5.9, "damage_distance");	

	self->AddTimer("SkillPulse", 3.6f);
	self->AddTimer("ProxRadius", 3.5f);
}

void Spinner23::SpawnLegs(Entity* self, const std::string& loc) {
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
	

// if spinner up to down: pos.y = Spinner starting pos (aka Waypoint 1) - 0.2
// if spinner down to up: pos.y = Spinner starting pos (aka Waypoint 1) - 0.1

	if (loc == "Right") {
		const auto dir = rot.GetForwardVector();
		pos.x += dir.x * offset;
		pos.z += dir.z * offset;
		info.pos = pos;
	} else if (loc == "Rear") {
		const auto dir = rot.GetRightVector();
		pos.x = -832.65002;
		pos.y = 242.12;
		pos.z = -47.0;
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

void Spinner23::OnChildLoaded(Entity* self, Entity* child) {
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
		
	self->AddTimer("IdleAnim", 0.5f);		
	RenderComponent::PlayAnimation(self, u"idle-up");	
}

void Spinner23::NotifyDie(Entity* self, Entity* other, Entity* killer) {
	auto players = self->GetVar<std::vector<LWOOBJID>>(u"Players");

	const auto& iter = std::find(players.begin(), players.end(), killer->GetObjectID());

	if (iter == players.end()) {
		players.push_back(killer->GetObjectID());
	}

	self->SetVar(u"Players", players);

	OnChildRemoved(self, other);
	RenderComponent::PlayAnimation(self, u"down");	
}

void Spinner23::OnChildRemoved(Entity* self, Entity* child) {
	RenderComponent::PlayAnimation(self, u"down");	
	self->AddTimer("DownAnim", 0.1f);	
	auto legTable = self->GetVar<std::vector<LWOOBJID>>(u"legTable");

	const auto& iter = std::find(legTable.begin(), legTable.end(), child->GetObjectID());

	if (iter != legTable.end()) {
		legTable.erase(iter);
	}

	self->SetVar(u"legTable", legTable);

	if (legTable.empty()) {		
		m_Counter = 0;	
		GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 1, 0, 0, eMovementPlatformState::Moving);		
		RenderComponent::PlayAnimation(self, u"down");
		self->CancelTimer("RespawnLeg");	
		self->AddTimer("IdleDown", 1.0f);		
		self->AddTimer("ReturnIdle", 11.0f);
		self->AddTimer("RespawnLeg", 13.5f);
		
//		Descend sfx
		GameMessages::SendStopNDAudioEmitter(self, self->GetSystemAddress(), "{a5d15e58-e797-4d96-9c49-75f7b52637c8}");		
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{40e86d71-084c-4149-884e-ab9b45b694dc}");	
		self->AddTimer("DescentGUID", 0.1f);		
//		End	
	}

	auto deadLegs = self->GetVar<std::vector<std::string>>(u"DeadLegs");

	const auto& leg = child->GetVar<std::string>(u"Leg");

	const auto& legIter = std::find(deadLegs.begin(), deadLegs.end(), leg);

	if (legIter == deadLegs.end()) {
		deadLegs.push_back(leg);
	}

	self->SetVar(u"DeadLegs", deadLegs);
}

void Spinner23::OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) {
// Damage players & enemies via proximity radius 	
	if (name == "damage_distance") {
		if (m_Counter == 1) {		
		
			if (entering->IsPlayer()) {
				auto* skillComponent = self->GetComponent<SkillComponent>();
				auto* skillComponentPlayer = entering->GetComponent<SkillComponent>();
				
				if (skillComponent == nullptr) {
					return;
				}

				skillComponentPlayer->CalculateBehavior(99994, 99994, entering->GetObjectID(), true); // player's skill	(got hit fx)	

				auto dir = entering->GetRotation().GetForwardVector();

				dir.y = 11;
				dir.x = -dir.x * 14;
				dir.z = -dir.z * 14;

				GameMessages::SendKnockback(entering->GetObjectID(), self->GetObjectID(), self->GetObjectID(), 1000, dir);
			}

		}
	}
	
// End
}

void Spinner23::OnTimerDone(Entity* self, std::string timerName) {
	if (timerName == "RespawnLeg") {	
		auto deadLegs = self->GetVar<std::vector<std::string>>(u"DeadLegs");

		if (deadLegs.empty()) {
			return;
		}

		SpawnLegs(self, deadLegs[0]);

		deadLegs.erase(deadLegs.begin());

		self->SetVar<std::vector<std::string>>(u"DeadLegs", deadLegs);
	}  
	
	if (timerName == "ReturnIdle") {	
//		m_Counter = 1;
		GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 0, 1, 1, eMovementPlatformState::Moving);
	
		self->AddTimer("IdleAnim", 1.0f);		
		RenderComponent::PlayAnimation(self, u"up");	
		
		self->AddTimer("SkillPulse", 3.6f);
		self->AddTimer("ProxRadius", 3.5f);
	
//		Fake skill pulse until m_Counter = 1 to avoid instant user damage
		self->AddTimer("SkillPulse", 1.6f);
		self->AddTimer("SkillPulse", 2.6f);
		
//		Ascend sfx
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{5c30c263-00ae-42a2-80a3-2ae33c8f13fe}");	
		self->AddTimer("AscentGUID", 0.1f);	
	}
	
	if (timerName == "IdleAnim") {	
		RenderComponent::PlayAnimation(self, u"idle-up");
	}		
	
	if (timerName == "IdleDown") {	
		RenderComponent::PlayAnimation(self, u"idle");
	}	
	if (timerName == "DownAnim") {	
		RenderComponent::PlayAnimation(self, u"down");
	}	
	
	if (timerName == "SkillPulse") {	
// Spinner damage skill exclusive for enemies	
		auto* skillComponent = self->GetComponent<SkillComponent>();

		if (skillComponent == nullptr) {
			return;
		}

		skillComponent->CalculateBehavior(971, 20371, self->GetObjectID(), true); // spinner's skill
//		effectID from behaviorID 20370 removed -> inaccurate offset & annoying
		if (m_Counter == 1) {
			self->AddTimer("SkillPulse", 1);	// <- set based on pulseTime, default = 1 
		}
// End		
	}

	if (timerName == "ProxRadius") {		
		m_Counter = 1;
//		^^ If ProxRadius activated, m_Counter should be 1 anyways		
		auto* proximityMonitorComponent = self->GetComponent<ProximityMonitorComponent>();
		self->SetProximityRadius(5.9, "damage_distance");			
	}	

//Handle spinner sound orders
	if (timerName == "AscentGUID") {
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{7f770ade-b84c-46ad-b3ae-bdbace5985d4}");	
		self->AddTimer("BladeGUID", 1.4f);		
	}
	if (timerName == "BladeGUID") {
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{a5d15e58-e797-4d96-9c49-75f7b52637c8}");	

	}
	if (timerName == "DescentGUID") {
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{97b60c03-51f2-45b6-80cc-ccbbef0d94cf}");	
	}	
}
