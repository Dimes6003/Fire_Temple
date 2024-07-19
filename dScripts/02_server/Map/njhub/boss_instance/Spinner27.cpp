#include "Spinner27.h"
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

void Spinner27::OnStartup(Entity* self) {
	m_Counter = 0;	
	self->SetProximityRadius(5.9, "damage_distance");	
	self->AddTimer("MoveBack", 20.8f);	
	self->SetNetworkVar(u"bIsInUse", false);
	self->SetVar(u"bActive", true);

	self->SetProximityRadius(3.5, "spin_distance");	
}

void Spinner27::OnSkillEventFired(Entity* self, Entity* caster, const std::string& message) {
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

void Spinner27::TriggerDrill(Entity* self) {
	
	GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 0, 1);	
	self->AddTimer("Return", 15.0f);		
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

void Spinner27::OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) {
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

void Spinner27::OnTimerDone(Entity* self, std::string timerName) {
	if (timerName == "Return") {
		m_Counter = 0;
		GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 1, 0, 0, eMovementPlatformState::Moving);		
		RenderComponent::PlayAnimation(self, u"down");
		self->AddTimer("DownAnim", 0.1f);

		self->SetNetworkVar(u"bIsInUse", false);
		self->SetVar(u"bActive", true);
		
//		Descend sfx
		GameMessages::SendStopNDAudioEmitter(self, self->GetSystemAddress(), "{4e222a0c-89e8-4141-91b5-74629d30caf7}");		
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{40e86d71-084c-4149-884e-ab9b45b694dc}");	
		self->AddTimer("DescentGUID", 0.1f);		
//		End

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
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{4e222a0c-89e8-4141-91b5-74629d30caf7}");	

	}
	if (timerName == "DescentGUID") {
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{97b60c03-51f2-45b6-80cc-ccbbef0d94cf}");	
	}
}