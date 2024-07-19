#include "Spinner22.h"
#include "Entity.h"
#include "GameMessages.h"
#include "MovingPlatformComponent.h"
#include "DestroyableComponent.h"
#include "ProximityMonitorComponent.h"
#include "MissionComponent.h"
#include "EntityInfo.h"
#include "RenderComponent.h"
#include "eStateChangeType.h"
#include "SkillComponent.h"

void Spinner22::OnStartup(Entity* self) {
	m_Counter = 0;	
	self->SetProximityRadius(5.9, "damage_distance");	
	self->AddTimer("MoveBack", 21.8f);	
	self->SetNetworkVar(u"bIsInUse", false);
	self->SetVar(u"bActive", true);

	self->SetProximityRadius(3.5, "spin_distance");	
}

void Spinner22::OnSkillEventFired(Entity* self, Entity* caster, const std::string& message) {
	if (message != "NinjagoSpinEvent" || self->GetNetworkVar<bool>(u"bIsInUse")) {
		return;
	}

	auto* proximityMonitorComponent = self->GetComponent<ProximityMonitorComponent>();

	if (proximityMonitorComponent == nullptr || !proximityMonitorComponent->IsInProximity("spin_distance", caster->GetObjectID())) {
		return;
	}

	const auto& cine = self->GetVar<std::u16string>(u"cinematic");
	GameMessages::SendPlayCinematic(caster->GetObjectID(), cine, caster->GetSystemAddress());
	
	self->SetNetworkVar(u"bIsInUse", true);
	TriggerDrill(self);
}

void Spinner22::TriggerDrill(Entity* self) {
	
	GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 0, 1);	
	self->AddTimer("Return", 11.0f);		
	self->AddTimer("IdleAnim", 1.0f);
	RenderComponent::PlayAnimation(self, u"up");
	
	self->AddTimer("SkillPulse", 3.6f);
	self->AddTimer("ProxRadius", 3.5f);
	
//	Fake skill pulse until m_Counter = 1 to avoid instant user damage
	self->AddTimer("SkillPulse", 1.6f);
	self->AddTimer("SkillPulse", 2.6f);
	
//	Ascend sfx
	GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{5c30c263-00ae-42a2-80a3-2ae33c8f13fe}");	
	self->AddTimer("AscentGUID", 0.1f);	
	
}

void Spinner22::OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) {
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
			

//				Optional Blade sfx for collision	
//				GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{b1bbe65e-330d-4ef6-a534-63e98dd199ec}");				
			}

		}
	}
	
// End
}

void Spinner22::OnTimerDone(Entity* self, std::string timerName) {
	if (timerName == "Return") {
		m_Counter = 0;
		GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 1, 0, 0, eMovementPlatformState::Moving);		
		RenderComponent::PlayAnimation(self, u"down");
		self->AddTimer("DownAnim", 0.1f);
		
//		Descend sfx
		GameMessages::SendStopNDAudioEmitter(self, self->GetSystemAddress(), "{a5d15e58-e797-4d96-9c49-75f7b52637c8}");		
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{40e86d71-084c-4149-884e-ab9b45b694dc}");	
		self->AddTimer("DescentGUID", 0.1f);		
//		End

		self->SetNetworkVar(u"bIsInUse", false);
		self->SetVar(u"bActive", true);
		return;
	}

	const auto& data = GeneralUtils::SplitString(timerName, '_');

	if (data.empty()) {
		return;
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
	
	if (timerName == "IdleAnim") {	
		RenderComponent::PlayAnimation(self, u"idle-up");
	}	
	
	if (timerName == "DownAnim") {	
		RenderComponent::PlayAnimation(self, u"down");
	}	
	
	if (timerName == "MoveBack") {	
		m_Counter = 0;
		GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 1, 0, 0, eMovementPlatformState::Moving);
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