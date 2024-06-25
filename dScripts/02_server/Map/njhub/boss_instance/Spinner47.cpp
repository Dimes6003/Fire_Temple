// Low blade spinner
#include "Spinner47.h"
#include "GameMessages.h"
#include "MovingPlatformComponent.h"
#include "DestroyableComponent.h"
#include "ProximityMonitorComponent.h"
#include "GameMessages.h"
#include "MissionComponent.h"
#include "EntityInfo.h"
#include "RenderComponent.h"
#include "eStateChangeType.h"
#include "SkillComponent.h"

void Spinner47::OnStartup(Entity* self) {
	

	self->AddTimer("MoveBack", 11.8f);


	self->SetNetworkVar(u"bIsInUse", false);
	self->SetVar(u"bActive", true);

	self->SetProximityRadius(3.5, "spin_distance");


}


void Spinner47::OnSkillEventFired(Entity* self, Entity* caster, const std::string& message) {
	if (message != "NinjagoSpinEvent" || self->GetNetworkVar<bool>(u"bIsInUse")) {
		return;
	} else {

		auto* proximityMonitorComponent = self->GetComponent<ProximityMonitorComponent>();

		if (proximityMonitorComponent == nullptr || !proximityMonitorComponent->IsInProximity("spin_distance", caster->GetObjectID())) {
			return;
		}
		self->SetNetworkVar(u"bIsInUse", true);		

//		Play cinematic
		auto cine = u"DartSpinners_DartSwitcher";
		GameMessages::SendPlayCinematic(caster->GetObjectID(), cine, caster->GetSystemAddress());		

			
//		End



		TriggerDrill(self);
	}
}

void Spinner47::TriggerDrill(Entity* self) {
	
//	Move spinner	
	GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 0, 1);

//	Play anims	
	RenderComponent::PlayAnimation(self, u"up");
	self->AddTimer("IdleUp", 1.0f);
	
//	Check if timed spinner
	auto ResetTime = self->GetVar<int32_t>(u"reset_time");
	
	if (ResetTime >= 1) {	
		self->AddTimer("Return", ResetTime + 2.5);	
//		2.5 = rough estimate of movetime for the timed platforms using this script			
	}
	
//	Ascend sfx
	GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{5c30c263-00ae-42a2-80a3-2ae33c8f13fe}");	
	self->AddTimer("AscentGUID", 0.1f);	
	
}

void Spinner47::OnTimerDone(Entity* self, std::string timerName) {

	if (timerName == "MoveBack") {	
		GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 1, 0, 0, eMovementPlatformState::Moving);
	}	
	
	if (timerName == "Return") {
		GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 1, 0, 0, eMovementPlatformState::Moving);		
		RenderComponent::PlayAnimation(self, u"down");
		self->AddTimer("Idle", 1.0f);

		self->SetNetworkVar(u"bIsInUse", false);
		self->SetVar(u"bActive", true);

		
//		Descend sfx
		GameMessages::SendStopNDAudioEmitter(self, self->GetSystemAddress(), "{dcd06295-949b-4179-8b99-129116def406}");	
		GameMessages::SendStopNDAudioEmitter(self, self->GetSystemAddress(), "{3062c5b2-b35a-4935-863f-a8c170aa1444}");	
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{40e86d71-084c-4149-884e-ab9b45b694dc}");	
		self->AddTimer("DescentGUID", 0.1f);		
//		End		
		return;	
	}
	if (timerName == "IdleUp") {	
		RenderComponent::PlayAnimation(self, u"idle-up");
	}
	if (timerName == "Idle") {	
		RenderComponent::PlayAnimation(self, u"idle");
	}
	
//Handle spinner sound orders
	if (timerName == "AscentGUID") {
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{7f770ade-b84c-46ad-b3ae-bdbace5985d4}");	
		self->AddTimer("BladeGUID", 1.4f);		
	}
	if (timerName == "BladeGUID") {
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{dcd06295-949b-4179-8b99-129116def406}");
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{3062c5b2-b35a-4935-863f-a8c170aa1444}");			

	}
	if (timerName == "DescentGUID") {
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{97b60c03-51f2-45b6-80cc-ccbbef0d94cf}");	
	}

}


