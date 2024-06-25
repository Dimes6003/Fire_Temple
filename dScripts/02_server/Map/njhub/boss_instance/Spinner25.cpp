#include "Spinner25.h"
#include "Entity.h"
#include "EntityManager.h"
#include "GameMessages.h"
#include "MovingPlatformComponent.h"
#include "DestroyableComponent.h"
#include "ProximityMonitorComponent.h"
#include "MissionComponent.h"
#include "EntityInfo.h"
#include "RenderComponent.h"
#include "eStateChangeType.h"
#include "SkillComponent.h"

void Spinner25::OnStartup(Entity* self) {
	auto* proximityMonitorComponent = self->GetComponent<ProximityMonitorComponent>();	
	self->SetProximityRadius(5.9, "damage_distance");		
	m_Counter = 0;
	
	self->SetNetworkVar(u"bIsInUse", false);
	self->SetVar(u"bActive", true);

	GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 0, 1);

	self->SetProximityRadius(3.5, "spin_distance");
	self->AddTimer("IdleAnim", 12.0f);
	self->AddTimer("MoveBack", 11.9f);	
}

void Spinner25::OnSkillEventFired(Entity* self, Entity* caster, const std::string& message) {
	if (message != "NinjagoSpinEvent" || self->GetNetworkVar<bool>(u"bIsInUse")) {	
		return;
	}	
	self->SetNetworkVar(u"bIsInUse", true);
	auto* proximityMonitorComponent = self->GetComponent<ProximityMonitorComponent>();

	if (proximityMonitorComponent == nullptr || !proximityMonitorComponent->IsInProximity("spin_distance", caster->GetObjectID())) {
		return;
	}
	
	const auto& cine = self->GetVar<std::u16string>(u"cinematic");

	if (cine.empty()) {
		return;
	}

	GameMessages::SendPlayCinematic(caster->GetObjectID(), cine, caster->GetSystemAddress());

	TriggerDrill(self);
}

void Spinner25::TriggerDrill(Entity* self) {
	GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 0, 1, 1, eMovementPlatformState::Moving);	
	RenderComponent::PlayAnimation(self, u"up");
	self->AddTimer("IdleAnim", 1.0f);
	self->AddTimer("SpawnLeg", 3.0f);
	self->AddTimer("SkillPulse", 3.6f);
	self->AddTimer("ProxRadius", 3.5f);
	
//	Fake skill pulse until m_Counter = 1 to avoid instant user damage
	self->AddTimer("SkillPulse", 1.6f);
	self->AddTimer("SkillPulse", 2.6f);	
	
//	Ascend sfx
	GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{5c30c263-00ae-42a2-80a3-2ae33c8f13fe}");	
	self->AddTimer("AscentGUID", 0.1f);	
}

void Spinner25::SpawnLegs(Entity* self, const std::string& loc) {
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
	

//if spinner down to up: LegLocY = Spinner starting pos (aka Waypoint 1) - 0.1

	if (loc == "Rear") {
		const auto dir = rot.GetRightVector();
		pos.x = -710.82758;
		pos.y = 242.12;
		pos.z = 3.54093;
		info.pos = pos;
	}

	info.rot = NiQuaternion::LookAt(info.pos, self->GetPosition());

	auto* entity = Game::entityManager->CreateEntity(info);

	Game::entityManager->ConstructEntity(entity);

	OnChildLoaded(self, entity);
}

void Spinner25::OnChildLoaded(Entity* self, Entity* child) {
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

void Spinner25::NotifyDie(Entity* self, Entity* other, Entity* killer) {
	auto players = self->GetVar<std::vector<LWOOBJID>>(u"Players");

	const auto& iter = std::find(players.begin(), players.end(), killer->GetObjectID());

	if (iter == players.end()) {
		players.push_back(killer->GetObjectID());
	}

	self->SetVar(u"Players", players);

	OnChildRemoved(self, other);
	RenderComponent::PlayAnimation(self, u"down");	
}

void Spinner25::OnChildRemoved(Entity* self, Entity* child) {
	RenderComponent::PlayAnimation(self, u"down");	
	self->AddTimer("DownAnim", 0.1f);
	auto legTable = self->GetVar<std::vector<LWOOBJID>>(u"legTable");

	const auto& iter = std::find(legTable.begin(), legTable.end(), child->GetObjectID());

	if (iter != legTable.end()) {
		legTable.erase(iter);
	}

	self->SetVar(u"legTable", legTable);

	if (legTable.empty()) {		
		//move to waypoint 
		GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 1, 0, 0, eMovementPlatformState::Moving);		
		//end
		m_Counter = 0;		

		RenderComponent::PlayAnimation(self, u"down");
		
//		Descend sfx
		GameMessages::SendStopNDAudioEmitter(self, self->GetSystemAddress(), "{423cc010-6667-4913-a494-8d51d1f9b61f}");		
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
	
	self->SetNetworkVar(u"bIsInUse", false);
	self->SetVar(u"bActive", true);		
}

void Spinner25::OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) {
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

void Spinner25::OnTimerDone(Entity* self, std::string timerName) {

	const auto& data = GeneralUtils::SplitString(timerName, '_');

	if (data.empty()) {
		return;
	}
	
	if (timerName == "MoveBack") {	
		m_Counter = 0;			
		//move to waypoint 
		GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 1, 0, 0, eMovementPlatformState::Moving);		
		//end
	}			
	
	if (timerName == "RespawnLeg") {

		auto deadLegs = self->GetVar<std::vector<std::string>>(u"DeadLegs");

		if (deadLegs.empty()) {
			return;
		}

		SpawnLegs(self, deadLegs[0]);

		deadLegs.erase(deadLegs.begin());

		self->SetVar<std::vector<std::string>>(u"DeadLegs", deadLegs);
	}
	
	if (timerName == "IdleAnim") {	
		RenderComponent::PlayAnimation(self, u"idle-up");
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
	if (timerName == "SpawnLeg") {		
		SpawnLegs(self, "Rear");	
	}
	if (timerName == "ProxRadius") {
		auto* proximityMonitorComponent = self->GetComponent<ProximityMonitorComponent>();		
		self->SetProximityRadius(5.9, "damage_distance");			
		m_Counter = 1;
//		^^ If ProxRadius activated, m_Counter should be 1 anyways		
		
	}	
	
//Handle spinner sound orders
	if (timerName == "AscentGUID") {
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{7f770ade-b84c-46ad-b3ae-bdbace5985d4}");	
		self->AddTimer("BladeGUID", 1.4f);		
	}
	if (timerName == "BladeGUID") {
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{423cc010-6667-4913-a494-8d51d1f9b61f}");	

	}
	if (timerName == "DescentGUID") {
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{97b60c03-51f2-45b6-80cc-ccbbef0d94cf}");	
	}
}
