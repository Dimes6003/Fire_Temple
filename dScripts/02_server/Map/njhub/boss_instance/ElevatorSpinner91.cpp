// Gold spinner for waves

#include "ElevatorSpinner91.h"
#include "GameMessages.h"
#include "MovingPlatformComponent.h"
#include "DestroyableComponent.h"
#include "ProximityMonitorComponent.h"
#include "MissionComponent.h"
#include "EntityInfo.h"
#include "RenderComponent.h"
#include "eStateChangeType.h"
#include "SoundTriggerComponent.h"

Entity* ElevatorSpinner91::caster1 = nullptr;

void ElevatorSpinner91::OnStartup(Entity* self) {

	self->SetNetworkVar(u"bIsInUse", true);
	self->SetVar(u"bActive", false);
	
	self->SetProximityRadius(3.5, "spin_distance");

}

void ElevatorSpinner91::OnSkillEventFired(Entity* self, Entity* caster, const std::string& message) {
	if (message != "NinjagoSpinEvent" || self->GetNetworkVar<bool>(u"bIsInUse")) {
		return;
	} else {		
		auto* proximityMonitorComponent = self->GetComponent<ProximityMonitorComponent>();

		if (proximityMonitorComponent == nullptr || !proximityMonitorComponent->IsInProximity("spin_distance", caster->GetObjectID())) {
			return;
		}
		self->SetNetworkVar(u"bIsInUse", true);		

		GameMessages::SendPlayCinematic(caster->GetObjectID(), u"Waves_SpinnerCam", caster->GetSystemAddress());		

		TriggerDrill(self);
	}
}

void ElevatorSpinner91::TriggerDrill(Entity* self) {
	
//	Move spinner	
	GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 0, 1, 1, eMovementPlatformState::Moving);

//	Play anim	
	RenderComponent::PlayAnimation(self, u"up");
	
//	Ascend sfx
	GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{5c30c263-00ae-42a2-80a3-2ae33c8f13fe}");	
	self->AddTimer("AscentGUID", 0.1f);
	
}


void ElevatorSpinner91::OnTimerDone(Entity* self, std::string timerName) {
	if (timerName == "AscentGUID") {
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{7f770ade-b84c-46ad-b3ae-bdbace5985d4}");	
	}		
}




